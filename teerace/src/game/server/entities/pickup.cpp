/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>
#include <generated/server_data.h>
#include <game/server/gamecontext.h>
#include <game/server/player.h>

#include "character.h"
#include "pickup.h"

CPickup::CPickup(CGameWorld *pGameWorld, int Type, vec2 Pos)
: CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP, Pos, PickupPhysSize)
{
	m_Type = Type;

	Reset();

	GameWorld()->InsertEntity(this);
}

void CPickup::Reset()
{
}

void CPickup::Tick()
{
	Move();
	// Check if a player intersected us
	CCharacter *apChrs[MAX_CLIENTS];
	int Num = GameServer()->m_World.FindEntities(m_Pos, 20.0f, (CEntity**)apChrs, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
	for(int j = 0; j < Num; j++)
	{
		CCharacter *pChr = apChrs[j];
		int ClientID = pChr->GetPlayer()->GetCID();

		if(!pChr->IsAlive() || m_SpawnTick[ClientID] != -1)
			continue;

		if(m_Layer == LAYER_SWITCH && !GameServer()->Collision()->m_pSwitchers[m_Number].m_Status[pChr->Team()]) continue;

		// player picked us up, is someone was hooking us, let them go
		bool Picked = false;
		switch (m_Type)
		{
			case PICKUP_HEALTH:
				if(pChr->Freeze()) GameServer()->CreateSound(m_Pos, SOUND_PICKUP_HEALTH, CmaskOne(ClientID));
				break;

			case PICKUP_ARMOR:
				if(!pChr->m_FreezeTime && pChr->GetActiveWeapon() >= WEAPON_SHOTGUN)
					pChr->SetActiveWeapon(WEAPON_HAMMER);
				for(int i = WEAPON_SHOTGUN; i < NUM_WEAPONS; i++)
				{
					if(pChr->GetWeaponGot(i))
					{
						if(!(pChr->m_FreezeTime && i == WEAPON_NINJA))
						{
							pChr->SetWeaponGot(i, false);
							pChr->SetWeaponAmmo(i, 0);
							GameServer()->CreateSound(m_Pos, SOUND_PICKUP_ARMOR, CmaskOne(ClientID));
						}
					}
				}
				pChr->SetNinjaActivationDir(vec2(0,0));
				pChr->SetNinjaActivationTick(-500);
				pChr->SetNinjaCurrentMoveTime(0);
				break;

			case PICKUP_GRENADE:
				if(!pChr->GetWeaponGot(WEAPON_GRENADE) || (pChr->GetWeaponAmmo(WEAPON_GRENADE) != -1 && !pChr->m_FreezeTime))
					if(pChr->GiveWeapon(WEAPON_GRENADE, -1))
					{
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_GRENADE, CmaskOne(ClientID));
						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCID(), WEAPON_GRENADE);
					}
				break;
			case PICKUP_SHOTGUN:
				if(!pChr->GetWeaponGot(WEAPON_SHOTGUN) || (pChr->GetWeaponAmmo(WEAPON_SHOTGUN) != -1 && !pChr->m_FreezeTime))
					if(pChr->GiveWeapon(WEAPON_SHOTGUN, -1))
					{
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN, CmaskOne(ClientID));
						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCID(), WEAPON_SHOTGUN);
					}
				break;
			case PICKUP_LASER:
				if(!pChr->GetWeaponGot(WEAPON_LASER) || (pChr->GetWeaponAmmo(WEAPON_LASER) != -1 && !pChr->m_FreezeTime))
					if(pChr->GiveWeapon(WEAPON_LASER, -1))
					{
						GameServer()->CreateSound(m_Pos, SOUND_PICKUP_SHOTGUN, CmaskOne(ClientID));
						if(pChr->GetPlayer())
							GameServer()->SendWeaponPickup(pChr->GetPlayer()->GetCID(), WEAPON_LASER);
					}
				break;

			case PICKUP_NINJA:
				{
					// activate ninja on target player
					pChr->GiveNinja();

					pChr->SetEmote(EMOTE_ANGRY, Server()->Tick() + 1200 * Server()->TickSpeed() / 1000);
					break;
				}

			default:
				break;
		};

		if(Picked)
		{
			char aBuf[256];
			str_format(aBuf, sizeof(aBuf), "pickup player='%d:%s' item=%d",
				pChr->GetPlayer()->GetCID(), Server()->ClientName(pChr->GetPlayer()->GetCID()), m_Type);
			GameServer()->Console()->Print(IConsole::OUTPUT_LEVEL_DEBUG, "game", aBuf);

			if(g_Config.m_SvPickupRespawn > -1)
				m_SpawnTick[ClientID] = Server()->Tick() + Server()->TickSpeed() * g_Config.m_SvPickupRespawn;
			else
				m_SpawnTick[ClientID] = 1;
		}
	}
}

void CPickup::TickPaused()
{
	for(int i = 0; i < MAX_CLIENTS; i++)
		if(m_SpawnTick[i] != -1)
			++m_SpawnTick[i];
}

void CPickup::Snap(int SnappingClient)
{
	int SpecID = SnappingClient == -1 ? -1 : GameServer()->m_apPlayers[SnappingClient]->GetSpectatorID();
	int OwnerID = SpecID == -1 ? SnappingClient : SpecID;
	if((OwnerID != -1 && m_SpawnTick[OwnerID] != -1) || NetworkClipped(SnappingClient))
		return;

	CCharacter *Char = GameServer()->GetPlayerChar(SnappingClient);

	if(SnappingClient > -1 && (GameServer()->m_apPlayers[SnappingClient]->GetTeam() == -1
				|| GameServer()->m_apPlayers[SnappingClient]->IsPaused())
			&& GameServer()->m_apPlayers[SnappingClient]->m_SpectatorID != SPEC_FREEVIEW)
		Char = GameServer()->GetPlayerChar(GameServer()->m_apPlayers[SnappingClient]->m_SpectatorID);

	int Tick = (Server()->Tick()%Server()->TickSpeed())%11;
	if (Char && Char->IsAlive() &&
			(m_Layer == LAYER_SWITCH &&
					!GameServer()->Collision()->m_pSwitchers[m_Number].m_Status[Char->Team()])
					&& (!Tick))
		return;

	CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, GetID(), sizeof(CNetObj_Pickup)));
	if(!pP)
		return;

	pP->m_X = (int)m_Pos.x;
	pP->m_Y = (int)m_Pos.y;
	pP->m_Type = m_Type;
}

void CPickup::Move()
{
	if (Server()->Tick()%int(Server()->TickSpeed() * 0.15f) == 0)
	{
		int Flags;
		int index = GameServer()->Collision()->IsMover(m_Pos.x,m_Pos.y, &Flags);
		if (index)
		{
			m_Core=GameServer()->Collision()->CpSpeed(index, Flags);
		}
		m_Pos += m_Core;
	}
}
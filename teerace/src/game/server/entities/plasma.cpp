/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#include <engine/server.h>
#include <engine/config.h>

#include <game/server/gamecontext.h>
#include <game/server/player.h>
#include "character.h"
#include "plasma.h"

const float ACCEL = 1.1f;

CPlasma::CPlasma(CGameWorld *pGameWorld, vec2 Pos, vec2 Dir, bool Freeze,
		bool Explosive, int ResponsibleTeam) :
		CEntity(pGameWorld, CGameWorld::ENTTYPE_LASER, Pos)
{
	m_Pos = Pos;
	m_Core = Dir;
	m_Freeze = Freeze;
	m_Explosive = Explosive;
	m_EvalTick = Server()->Tick();
	m_LifeTime = Server()->TickSpeed() * 1.5;
	m_ResponsibleTeam = ResponsibleTeam;
	GameWorld()->InsertEntity(this);
}

bool CPlasma::HitCharacter()
{
	vec2 To2;
	CCharacter *Hit = GameServer()->m_World.IntersectCharacter(m_Pos,
			m_Pos + m_Core, 0.0f, To2);

	//if(!g_Config.m_SvHit)
	//	return false;

	if (!Hit)
		return false;

	m_Freeze ? Hit->Freeze() : Hit->Unfreeze();
	if (m_Explosive)
		GameServer()->CreateExplosion(m_Pos, -1, WEAPON_GRENADE, true);
	GameServer()->m_World.DestroyEntity(this);
	return true;
}

void CPlasma::Move()
{
	m_Pos += m_Core;
	m_Core *= ACCEL;
}

void CPlasma::Reset()
{
	GameServer()->m_World.DestroyEntity(this);
}

void CPlasma::Tick()
{
	if (m_LifeTime == 0)
	{
		Reset();
		return;
	}
	m_LifeTime--;
	Move();
	HitCharacter();

	int Res = 0;
	Res = GameServer()->Collision()->IntersectNoLaser(m_Pos, m_Pos + m_Core, 0,
			0);
	if (Res)
	{
		if (m_Explosive)
			GameServer()->CreateExplosion(
					m_Pos,
					-1,
					WEAPON_GRENADE,
					true);
		Reset();
	}

}

void CPlasma::Snap(int SnappingClient)
{
	if (NetworkClipped(SnappingClient))
		return;
	CCharacter* SnapChar = GameServer()->GetPlayerChar(SnappingClient);
	CPlayer* SnapPlayer = SnappingClient > -1 ? GameServer()->m_apPlayers[SnappingClient] : 0;
	int Tick = (Server()->Tick() % Server()->TickSpeed()) % 11;

	if (SnapChar && SnapChar->IsAlive()
			&& (m_Layer == LAYER_SWITCH
					&& !GameServer()->Collision()->m_pSwitchers[m_Number].m_Status[SnapChar->Team()])
			&& (!Tick))
		return;

	if(SnapPlayer && (SnapPlayer->GetTeam() == TEAM_SPECTATORS || SnapPlayer->IsPaused()) && SnapPlayer->m_SpectatorID != -1
		&& GameServer()->GetPlayerChar(SnapPlayer->m_SpectatorID)
		&& !SnapPlayer->m_ShowOthers)
		return;

	if(SnapPlayer && (SnapPlayer->GetTeam() == TEAM_SPECTATORS || SnapPlayer->IsPaused()) && SnapPlayer->m_SpectatorID == -1
		&& SnapChar)
		return;

	CNetObj_Laser *pObj = static_cast<CNetObj_Laser *>(Server()->SnapNewItem(
			NETOBJTYPE_LASER, GetID(), sizeof(CNetObj_Laser)));

	if(!pObj)
		return;

	pObj->m_X = (int)m_Pos.x;
	pObj->m_Y = (int)m_Pos.y;
	pObj->m_FromX = (int)m_Pos.x;
	pObj->m_FromY = (int)m_Pos.y;
	pObj->m_StartTick = m_EvalTick;
}

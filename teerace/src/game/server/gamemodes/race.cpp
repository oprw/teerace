/* copyright (c) 2007 rajh, race mod stuff */
#include <engine/shared/config.h>
#include <game/server/entities/character.h>
#include <game/server/entities/pickup.h>
#include <game/server/player.h>
#include <game/server/gamecontext.h>
#include <game/server/score.h>
#include <game/race.h>
#include "race.h"
#include <game/server/mkjson.cpp>

CGameControllerRACE::CGameControllerRACE(class CGameContext *pGameServer) : IGameController(pGameServer)
{
	m_pGameType = "TeeRace";
	
	for(int i = 0; i < MAX_CLIENTS; i++)
		m_aRace[i].Reset();
	
	SetRunning();
}

CGameControllerRACE::~CGameControllerRACE()
{
}

void CGameControllerRACE::OnCharacterSpawn(CCharacter *pChr)
{
	IGameController::OnCharacterSpawn(pChr);

	ResetPickups(pChr->GetPlayer()->GetCID());
}

int CGameControllerRACE::OnCharacterDeath(class CCharacter *pVictim, class CPlayer *pKiller, int Weapon)
{
	int ClientID = pVictim->GetPlayer()->GetCID();
	StopRace(ClientID);
	m_aRace[ClientID].Reset();
	return 0;
}

void CGameControllerRACE::DoWincheck()
{
	/*if(m_GameOverTick == -1 && !m_Warmup)
	{
		if((g_Config.m_SvTimelimit > 0 && (Server()->Tick()-m_RoundStartTick) >= g_Config.m_SvTimelimit*Server()->TickSpeed()*60))
			EndRound();
	}*/
}

void CGameControllerRACE::Snap(int SnappingClient)
{
	const int i = SnappingClient;
	if(i >= 0 && i < MAX_CLIENTS)
	{
		int SpecID = (GameServer()->m_apPlayers[i] && GameServer()->m_apPlayers[i]->IsPaused()) ? GameServer()->m_apPlayers[i]->GetSpectatorID() : -1;
		if(SpecID != -1)
		{
			if(m_aRace[SpecID].m_RaceState == RACE_STARTED)
				this->DoSnap(i, m_aRace[SpecID].m_StartTick);
			else
				this->DoSnap(i, m_GameStartTick);
		}
		else if(m_aRace[i].m_RaceState == RACE_STARTED)
			this->DoSnap(i, m_aRace[i].m_StartTick);
		else
			this->DoSnap(i, m_GameStartTick);
        }
	else
                this->DoSnap(i, m_GameStartTick);
}

void CGameControllerRACE::SendTime(int ClientID, int To)
{
	CRaceData *p = &m_aRace[ClientID];
	bool Checkpoint = p->m_CpTick != -1 && p->m_CpTick > Server()->Tick();
	if(Checkpoint){
		char aDiff[64];
		IRace::FormatTimeDiff(aDiff, sizeof(aDiff), p->m_CpDiff, false);
		const char *pColor = (p->m_CpDiff <= 0) ? "^090" : "^900";
		char aBuf[128] = {0};
		str_format(aBuf, sizeof(aBuf), "%s%s", pColor, aDiff);
		GameServer()->SendBroadcast(aBuf, To);
	}
}

void CGameControllerRACE::Tick()
{
	IGameController::Tick();
	DoWincheck();
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(m_aRace[i].m_RaceState == RACE_STARTED && (Server()->Tick() - m_aRace[i].m_StartTick) % Server()->TickSpeed() == 0)
			SendTime(i, i);

		int SpecID = GameServer()->m_apPlayers[i] ? GameServer()->m_apPlayers[i]->GetSpectatorID() : -1;
		if(SpecID != -1 && g_Config.m_SvShowTimes && m_aRace[SpecID].m_RaceState == RACE_STARTED &&
			(Server()->Tick() - m_aRace[SpecID].m_StartTick) % Server()->TickSpeed() == 0)
			SendTime(SpecID, i);
	}
}

void CGameControllerRACE::OnCheckpoint(int ID, int z)
{
	CRaceData *p = &m_aRace[ID];
	const CPlayerData *pBest = GameServer()->Score()->PlayerData(ID);
	if(p->m_RaceState != RACE_STARTED)
		return;

	p->m_aCpCurrent[z] = GetTime(ID);

	if(pBest->m_Time && pBest->m_aCpTime[z] != 0)
	{
		p->m_CpDiff = p->m_aCpCurrent[z] - pBest->m_aCpTime[z];
		p->m_CpTick = Server()->Tick() + Server()->TickSpeed() * 2;
	}
}

void CGameControllerRACE::OnRaceStart(int ID)
{
	CRaceData *p = &m_aRace[ID];
	CCharacter *pChr = GameServer()->GetPlayerChar(ID);

	if(p->m_RaceState != RACE_NONE)
	{
		// reset pickups
		if(!pChr->GetWeaponGot(WEAPON_GRENADE))
			ResetPickups(ID);
	}
	
	p->m_RaceState = RACE_STARTED;
	p->m_StartTick = Server()->Tick();
	p->m_AddTime = 0.f;
}

void CGameControllerRACE::OnRaceEnd(int ID, int FinishTime)
{
	CRaceData *p = &m_aRace[ID];
	p->m_RaceState = RACE_FINISHED;

	CCharacter *pChr = GameServer()->GetPlayerChar(ID);
	

	if(!FinishTime || pChr->m_Super)
		return;

	

	// TODO:
	// move all this into the scoring classes so the selected
	// scoring backend can decide how to handle the situation

	int Improved = GameServer()->Score()->PlayerData(ID)->m_Time - FinishTime;

	// save the score
	GameServer()->Score()->OnPlayerFinish(ID, FinishTime, p->m_aCpCurrent);

	char aBuf[128];
	char aTime[64];
	IRace::FormatTimeLong(aTime, sizeof(aTime), FinishTime, true);
	str_format(aBuf, sizeof(aBuf), "%s finished in: %s", Server()->ClientName(ID), aTime);
	dumpjson("event", "finish", "player", json_plr(Server(), ID), "time", FinishTime);
	int To = g_Config.m_SvShowTimes ? -1 : ID;
	GameServer()->SendChat(-1, CHAT_ALL, To, aBuf);

	if(Improved > 0)
	{
		str_format(aBuf, sizeof(aBuf), "New record: %d.%03d second(s) better", Improved / 1000, Improved % 1000);
		GameServer()->SendChat(-1, CHAT_ALL, To, aBuf);
	}
}

bool CGameControllerRACE::IsStart(vec2 Pos, int Team) const
{
	return GameServer()->Collision()->CheckIndexEx(Pos, TILE_BEGIN);
}

bool CGameControllerRACE::IsEnd(vec2 Pos, int Team) const
{
	return GameServer()->Collision()->CheckIndexEx(Pos, TILE_END);
}

void CGameControllerRACE::OnPhysicsStep(int ID, vec2 Pos, float IntraTick)
{
	int Cp = GameServer()->Collision()->CheckCheckpoint(Pos);
	if(Cp != -1)
		OnCheckpoint(ID, Cp);

	float IntraTime = 1000.f / Server()->TickSpeed() * IntraTick;
	int Team = GameServer()->m_apPlayers[ID]->GetTeam();
	//if(CanStartRace(ID) && IsStart(Pos, Team))
	if(IsStart(Pos, Team))
	{
		OnRaceStart(ID);
		m_aRace[ID].m_AddTime -= IntraTime;
	}
	else if(CanEndRace(ID) && IsEnd(Pos, Team))
	{
		m_aRace[ID].m_AddTime += IntraTime;
		OnRaceEnd(ID, GetTimeExact(ID));
	}
}

bool CGameControllerRACE::CanStartRace(int ID) const
{
	CCharacter *pChr = GameServer()->GetPlayerChar(ID);
	bool AllowRestart = g_Config.m_SvAllowRestartOld && !pChr->GetWeaponGot(WEAPON_GRENADE) && !pChr->Armor();
	return (m_aRace[ID].m_RaceState == RACE_NONE || AllowRestart);
}

bool CGameControllerRACE::CanEndRace(int ID) const
{
	return m_aRace[ID].m_RaceState == RACE_STARTED;
}

void CGameControllerRACE::ResetPickups(int ClientID)
{
	CPickup *pPickup = static_cast<CPickup *>(GameServer()->m_World.FindFirst(CGameWorld::ENTTYPE_PICKUP));
	for(; pPickup; pPickup = (CPickup *)pPickup->TypeNext())
		pPickup->Respawn(ClientID);
}

int CGameControllerRACE::GetTime(int ID) const
{
	return (Server()->Tick() - m_aRace[ID].m_StartTick) * 1000 / Server()->TickSpeed();
}

int CGameControllerRACE::GetTimeExact(int ID) const
{
	return GetTime(ID) + round_to_int(m_aRace[ID].m_AddTime);
}

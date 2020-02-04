#include <base/math.h>
#include <base/color.h>

#include <engine/shared/config.h>
#include <engine/storage.h>
#include <game/race.h>
#include <game/version.h>

#include "gamecontext.h"
#include "player.h"

#include "entities/character.h"

#include "gamemodes/race.h"
#include "score.h"

void CGameContext::ConTeleport(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int CID1 = clamp(pResult->GetInteger(0), 0, (int)MAX_CLIENTS-1);
	int CID2 = clamp(pResult->GetInteger(1), 0, (int)MAX_CLIENTS-1);
	if(pSelf->m_apPlayers[CID1] && pSelf->m_apPlayers[CID2])
	{
		CCharacter* pChr = pSelf->GetPlayerChar(CID1);
		if(pChr)
		{
			pChr->SetPos(pSelf->m_apPlayers[CID2]->m_ViewPos);
			//pSelf->RaceController()->StopRace(CID1);
		}
		else
			pSelf->m_apPlayers[CID1]->m_ViewPos = pSelf->m_apPlayers[CID2]->m_ViewPos;
	}
}

void CGameContext::ConTeleportTo(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int CID = clamp(pResult->GetInteger(0), 0, (int)MAX_CLIENTS-1);
	if(pSelf->m_apPlayers[CID])
	{
		CCharacter* pChr = pSelf->GetPlayerChar(CID);
		vec2 TelePos = vec2(pResult->GetInteger(1), pResult->GetInteger(2));
		if(pChr)
		{
			pChr->SetPos(TelePos);
			pSelf->RaceController()->StopRace(CID);
		}
		else
			pSelf->m_apPlayers[CID]->m_ViewPos = TelePos;
	}
}

void CGameContext::ConGetPos(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int CID = clamp(pResult->GetInteger(0), 0, (int)MAX_CLIENTS-1);
	if(pSelf->m_apPlayers[CID])
	{
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "%s pos: %d @ %d", pSelf->Server()->ClientName(CID),
			(int)pSelf->m_apPlayers[CID]->m_ViewPos.x, (int)pSelf->m_apPlayers[CID]->m_ViewPos.y);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "race", aBuf);
	}
}

void CGameContext::ChatConInfo(IConsole::IResult *pResult, void *pUser)
{
	CGameContext *pSelf = (CGameContext *)pUser;

	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "TeeRace v%s", TEERACE_VERSION);
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", aBuf);
	str_format(aBuf, sizeof(aBuf), "Based on mkRace %s made by [SN] Snoop!", MKRACE_VERSION);
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", aBuf);
	str_format(aBuf, sizeof(aBuf), "Based on Simple DDRace %s by Dune", DDRACE_VERSION);
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", aBuf);
	str_format(aBuf, sizeof(aBuf), "Which is modified from Race %s (C)Rajh, Redix and Sushi", RACE_VERSION);
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", aBuf);
	//pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "https://github.com/SNSnoop/mkRace");
}

void CGameContext::ChatConTop5(IConsole::IResult *pResult, void *pUser)
{
	CGameContext *pSelf = (CGameContext *)pUser;

	if(!g_Config.m_SvShowTimes)
	{
		pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "Showing the Top5 is not allowed on this server.");
		return;
	}

	if(pResult->NumArguments() > 0)
		pSelf->Score()->ShowTop5(pSelf->m_ChatConsoleClientID, max(1, pResult->GetInteger(0)));
	else
		pSelf->Score()->ShowTop5(pSelf->m_ChatConsoleClientID);
}

void CGameContext::ChatConRank(IConsole::IResult *pResult, void *pUser)
{
	CGameContext *pSelf = (CGameContext *)pUser;

	if(g_Config.m_SvShowTimes && pResult->NumArguments() > 0)
	{
		char aStr[256];
		str_copy(aStr, pResult->GetString(0), sizeof(aStr));
		str_clean_whitespaces(aStr);
		pSelf->Score()->ShowRank(pSelf->m_ChatConsoleClientID, aStr);
	}
	else
		pSelf->Score()->ShowRank(pSelf->m_ChatConsoleClientID);
}

void CGameContext::ChatConShowOthers(IConsole::IResult *pResult, void *pUser)
{
	CGameContext *pSelf = (CGameContext *)pUser;

	if(!g_Config.m_SvShowOthers)
		pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "This command is not allowed on this server.");
	else
		pSelf->m_apPlayers[pSelf->m_ChatConsoleClientID]->ToggleShowOthers();
}

void CGameContext::ChatConHelp(IConsole::IResult *pResult, void *pUser)
{
	CGameContext *pSelf = (CGameContext *)pUser;

	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "---Command List---");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/res\" - Teleport yourself out of freeze");
	//pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/kill\" - Kill yourself");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/pause\" or \"/spec\"  - Toggles pause");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/dr\" - Rescue to location before disconnect");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/swap\" - Swap places with any player");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/rank\" \"/rank NAME\" - shows your/other map rank");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/top5\" - shows the top 5");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"/emote\" - sets your tee's eye emote");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"!stat\" \"!stat NAME\" - count of maps you/other finished");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"!top\" - top 5 players who finished maps");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"!time\" - show your current local time");
	pSelf->m_pChatConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "chat", "\"!online\" - shows the number of players on the server");
}

void CGameContext::InitChatConsole()
{
	if(m_pChatConsole)
		return;

	m_pChatConsole = CreateConsole(CFGFLAG_SERVERCHAT);
	m_ChatConsoleClientID = -1;

	m_pChatConsole->RegisterPrintCallback(IConsole::OUTPUT_LEVEL_STANDARD, SendChatResponse, this);

	m_pChatConsole->Register("info", "", CFGFLAG_SERVERCHAT, ChatConInfo, this, "");
	m_pChatConsole->Register("top5", "?i", CFGFLAG_SERVERCHAT, ChatConTop5, this, "");
	m_pChatConsole->Register("rank", "?r", CFGFLAG_SERVERCHAT, ChatConRank, this, "");
	// m_pChatConsole->Register("show_others", "", CFGFLAG_SERVERCHAT, ChatConShowOthers, this, "");
	m_pChatConsole->Register("help", "", CFGFLAG_SERVERCHAT, ChatConHelp, this, "");
	
	#define CHAT_COMMAND(name, params, flags, callback, userdata, help) m_pChatConsole->Register(name, params, flags, callback, userdata, help);
	#include "mkrace.h"
}

void CGameContext::SendChatResponse(const char *pLine, void *pUser, bool Highlighted)
{
	CGameContext *pSelf = (CGameContext *)pUser;
	if(pSelf->m_ChatConsoleClientID == -1)
		return;

	static volatile int ReentryGuard = 0;
	if(ReentryGuard)
		return;
	ReentryGuard++;

	while(*pLine && *pLine != ' ')
		pLine++;
	if(*pLine && *(pLine + 1))
		pSelf->SendChat(-1, CHAT_ALL, pSelf->m_ChatConsoleClientID, pLine + 1);

	ReentryGuard--;
}

void CGameContext::LoadMapSettings()
{
	/*if(m_Layers.SettingsLayer())
	{
		CMapItemLayerTilemap *pLayer = m_Layers.SettingsLayer();
		CTile *pTiles = static_cast<CTile *>(m_Layers.Map()->GetData(pLayer->m_Data));
		char *pCommand = new char[pLayer->m_Width+1];
		pCommand[pLayer->m_Width] = 0;

		for(int i = 0; i < pLayer->m_Height; i++)
		{
			for(int j = 0; j < pLayer->m_Width; j++)
				pCommand[j] = pTiles[i*pLayer->m_Width+j].m_Index;
			Console()->ExecuteLineFlag(pCommand, CFGFLAG_MAPSETTINGS);
		}

		delete[] pCommand;
		m_Layers.Map()->UnloadData(pLayer->m_Data);
	}*/
}

int64 CmaskRace(CGameContext *pGameServer, int Owner)
{
	int64 Mask = CmaskOne(Owner);
	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(pGameServer->m_apPlayers[i] && pGameServer->m_apPlayers[i]->ShowOthers())
			Mask = Mask | CmaskOne(i);
	}

	return Mask;
}

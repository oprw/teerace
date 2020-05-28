/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_PLAYER_H
#define GAME_SERVER_PLAYER_H

#include "alloc.h"
#include "gamecontext.h"


enum
{
	WEAPON_GAME = -3, // team switching etc
	WEAPON_SELF = -2, // console kill command
	WEAPON_WORLD = -1, // death tiles etc
};

// player object
class CPlayer
{
	MACRO_ALLOC_POOL_ID()

public:
	CPlayer(CGameContext *pGameServer, int ClientID, bool Dummy, bool AsSpec = false);
	~CPlayer();

	void Init(int CID);

	void TryRespawn();
	void Respawn();
	void SetTeam(int Team, bool DoChatMsg=true);
	int GetTeam() const { return m_Team; };
	int GetCID() const { return m_ClientID; };
	bool IsDummy() const { return m_Dummy; }

	void Tick();
	void PostTick();
	void Snap(int SnappingClient);

	void OnDirectInput(CNetObj_PlayerInput *NewInput);
	void OnPredictedInput(CNetObj_PlayerInput *NewInput);
	void OnDisconnect();

	void GetCustomTuning(CTuningParams* Tuning) const;
	bool RequiresCustomTuning() const;
	void KillCharacter(int Weapon = WEAPON_GAME);
	CCharacter *GetCharacter();
	const CCharacter *GetCharacter() const;

	//---------------------------------------------------------
	// this is used for snapping so we know how we can clip the view for the player
	vec2 m_ViewPos;

	// states if the client is chatting, accessing a menu etc.
	int m_PlayerFlags;

	// used for snapping to just update latency if the scoreboard is active
	int m_aActLatency[MAX_CLIENTS];

	// used for spectator mode
	int GetSpectatorID() const { return m_SpectatorID; }
	bool SetSpectatorID(int SpecMode, int SpectatorID);
	bool m_DeadSpecMode;
	bool DeadCanFollow(CPlayer *pPlayer) const;
	void UpdateDeadSpecMode();

	void ToggleShowOthers() { m_ShowOthers ^= 1; }
	bool ShowOthers() const;

	bool m_IsReadyToEnter;
	bool m_IsReadyToPlay;

	bool m_RespawnDisabled;

	//
	int m_Vote;
	int m_VotePos;
	//
	int m_LastVoteCall;
	int m_LastVoteTry;
	int m_LastChat;
	int m_LastSetTeam;
	int m_LastSetSpectatorMode;
	int m_LastChangeInfo;
	int m_LastEmote;
	int m_LastKill;
	int m_LastReadyChange;

	// TODO: clean this up
	struct
	{
		char m_aaSkinPartNames[NUM_SKINPARTS][24];
		int m_aUseCustomColors[NUM_SKINPARTS];
		int m_aSkinPartColors[NUM_SKINPARTS];
	} m_TeeInfos;

	int m_RespawnTick;
	int m_DieTick;
	int m_FreezeTick;
	int m_Score;
	int m_ScoreStartTick;
	int m_LastActionTick;
	int m_TeamChangeTick;

	int m_InactivityTickCounter;

	struct
	{
		int m_TargetX;
		int m_TargetY;
	} m_LatestActivity;

	// network latency calculations
	struct
	{
		int m_Accum;
		int m_AccumMin;
		int m_AccumMax;
		int m_Avg;
		int m_Min;
		int m_Max;
	} m_Latency;
	
	// mkRace
	enum
	{
		PAUSE_NONE=0,
		PAUSE_PAUSED,
		PAUSE_SPEC
	};
	void ProcessPause();
	int Pause(int State, bool Force);
	int IsPaused();
	int m_SpectatorID;
	int64 m_ForcePauseTime;
	int64 m_LastPause;
	int64 m_LastEyeEmote;
	bool m_EyeEmote;
	int m_DefEmote;
	int m_DefEmoteReset;
	void SpectatePlayerName(const char *pName);
	bool m_ShowOthers;
	bool m_ToDisconnect;
	CGameContext::CPlayerRescueState m_UndoState;
	int m_CanUndoKill;
	bool m_died;

private:
	CCharacter *m_pCharacter;
	CGameContext *m_pGameServer;

	CGameContext *GameServer() const { return m_pGameServer; }
	IServer *Server() const;

	//
	bool m_Spawning;
	int m_ClientID;
	int m_Team;
	bool m_Dummy;

	// used for spectator mode
	int m_SpecMode;
	class CFlag *m_pSpecFlag;
	bool m_ActiveSpecSwitch;

	// mkRace
	int m_Paused;
};

#endif

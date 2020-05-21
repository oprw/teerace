/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_ENTITIES_CHARACTER_H
#define GAME_SERVER_ENTITIES_CHARACTER_H

#include <generated/protocol.h>

#include <game/gamecore.h>
#include <game/server/entity.h>

//class CGameTeams;

class CCharacter : public CEntity
{
	MACRO_ALLOC_POOL_ID()

public:
	//character's size
	static const int ms_PhysSize = 28;

	CCharacter(CGameWorld *pWorld);

	virtual void Reset();
	virtual void Destroy();
	virtual void Tick();
	virtual void TickDefered();
	virtual void TickPaused();
	virtual void Snap(int SnappingClient);
	virtual void PostSnap();

	bool IsGrounded();

	void SetWeapon(int W);
	void HandleWeaponSwitch();
	void DoWeaponSwitch();

	void HandleWeapons();
	void HandleNinja();
	void HandleJetpack();

	void OnPredictedInput(CNetObj_PlayerInput *pNewInput);
	void OnDirectInput(CNetObj_PlayerInput *pNewInput);
	void ResetInput();
	void FireWeapon();

	void Die(int Killer, int Weapon);
	bool TakeDamage(vec2 Force, vec2 Source, int Dmg, int From, int Weapon);

	bool Spawn(class CPlayer *pPlayer, vec2 Pos);
	bool Remove();

	bool IncreaseHealth(int Amount);
	bool IncreaseArmor(int Amount);

	bool GiveWeapon(int Weapon, int Ammo);
	void GiveNinja();
	void FreezeIndicator(unsigned Amount);

	void SetEmote(int Emote, int Tick);

	bool Freeze(int Time);
	bool Freeze();
	bool Unfreeze();

	void Rescue();

	bool IsAlive() const { return m_Alive; }
	bool IsFrozen() const { return m_Frozen; }

	void GetCustomTuning(CTuningParams* Tuning) const;
	bool RequiresCustomTuning() const;

	class CPlayer *GetPlayer() { return m_pPlayer; }

	int Armor() const { return m_Armor; }

	// only for debugging... do not modify the position outside of gamecore!
	void SetPos(vec2 Pos) { m_Core.m_Pos = Pos; }

	static void OnPhysicsStep(vec2 Pos, float IntraTick, void *pUserData);
	
	// mkRace
	vec2 m_Intersection;
	bool m_EndlessHook;
	bool m_Jetpack;
	int m_TileIndex;
	int m_TileFlags;
	int m_TileFIndex;
	int m_TileFFlags;
	int m_TileSIndex;
	int m_TileSFlags;
	int m_TileIndexL;
	int m_TileFlagsL;
	int m_TileFIndexL;
	int m_TileFFlagsL;
	int m_TileSIndexL;
	int m_TileSFlagsL;
	int m_TileIndexR;
	int m_TileFlagsR;
	int m_TileFIndexR;
	int m_TileFFlagsR;
	int m_TileSIndexR;
	int m_TileSFlagsR;
	int m_TileIndexT;
	int m_TileFlagsT;
	int m_TileFIndexT;
	int m_TileFFlagsT;
	int m_TileSIndexT;
	int m_TileSFlagsT;
	int m_TileIndexB;
	int m_TileFlagsB;
	int m_TileFIndexB;
	int m_TileFFlagsB;
	int m_TileSIndexB;
	int m_TileSFlagsB;
	//CGameTeams* Teams();
	int m_PainSoundTimer;
	bool m_DeepFreeze;
	int m_SwapRequest;
	int m_LastMove;
	int m_FreezeTime;
	int m_FreezeTick;
	vec2 m_PrevPos;

	vec2 m_PrevSavePos;
	int m_RescueCount;
	bool m_SuperJump;
	bool m_Super;
	int m_TeleCheckpoint;
	bool m_LastRefillJumps;
	bool m_HasTeleGun;
	bool m_HasTeleGrenade;
	bool m_HasTeleLaser;
	vec2 m_TeleGunPos;
	bool m_TeleGunTeleport;
	bool m_IsBlueTeleGunTeleport;

	int Team();
	CCharacterCore* Core() { return &m_Core; };
	void Pause(bool Pause);
	bool IsPaused() const { return m_Paused; }
	int GetLastWeapon() { return m_LastWeapon; };
	void SetLastWeapon(int LastWeap) {m_LastWeapon = LastWeap; };
	int GetActiveWeapon() { return m_ActiveWeapon; };
	void SetActiveWeapon(int ActiveWeap) {m_ActiveWeapon = ActiveWeap; };
	bool GetWeaponGot(int Type) { return m_aWeapons[Type].m_Got; };
	void SetWeaponGot(int Type, bool Value) { m_aWeapons[Type].m_Got = Value; };
	int GetWeaponAmmo(int Type) { return m_aWeapons[Type].m_Ammo; };
	void SetWeaponAmmo(int Type, int Value) { m_aWeapons[Type].m_Ammo = Value; };
	void SetEmoteType(int EmoteType) { m_EmoteType = EmoteType; };
	void SetEmoteStop(int EmoteStop) { m_EmoteStop = EmoteStop; };
	void SetNinjaActivationDir(vec2 ActivationDir) { m_Ninja.m_ActivationDir = ActivationDir; };
	void SetNinjaActivationTick(int ActivationTick) { m_Ninja.m_ActivationTick = ActivationTick; };
	void SetNinjaCurrentMoveTime(int CurrentMoveTime) { m_Ninja.m_CurrentMoveTime = CurrentMoveTime; };

private:
	// player controlling this character
	class CPlayer *m_pPlayer;

	bool m_Alive;

	// ddrace stuff
	bool m_Frozen; // not sure if we should use this

	// weapon info
	CEntity *m_apHitObjects[10];
	int m_NumObjectsHit;

	struct WeaponStat
	{
		int m_AmmoRegenStart;
		int m_Ammo;
		bool m_Got;

	} m_aWeapons[NUM_WEAPONS], m_aSavedWeapons[NUM_WEAPONS];

	int m_ActiveWeapon;
	int m_LastWeapon;
	int m_QueuedWeapon;

	int m_ReloadTimer;
	int m_AttackTick;

	int m_EmoteType;
	int m_EmoteStop;

	// last tick that the player took any action ie some input
	int m_LastAction;
	int m_LastNoAmmoSound;

	// these are non-heldback inputs
	CNetObj_PlayerInput m_LatestPrevInput;
	CNetObj_PlayerInput m_LatestInput;

	// input
	CNetObj_PlayerInput m_Input;
	CNetObj_PlayerInput m_FreezedInput;
	int m_NumInputs;
	int m_Jumped;

	int m_Health;
	int m_Armor;

	int m_TriggeredEvents;

	bool m_SetSavePos;

	// ninja
	struct
	{
		vec2 m_ActivationDir;
		int m_ActivationTick;
		int m_IndicatorTick;
		int m_CurrentMoveTime;
		int m_OldVelAmount;
	} m_Ninja;

	// the player core for the physics
	CCharacterCore m_Core;

	// info for dead reckoning
	int m_ReckoningTick; // tick that we are performing dead reckoning From
	CCharacterCore m_SendCore; // core that we should send
	CCharacterCore m_ReckoningCore; // the dead reckoning core

	// mkRace
	bool m_Paused;
	void DDRaceTick();
	void DDRacePostCoreTick();
	void HandleBroadcast();
	void HandleTiles(int Index);
	void HandleSkippableTiles(int Index);
};

#endif

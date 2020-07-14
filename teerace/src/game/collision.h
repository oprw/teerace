/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_COLLISION_H
#define GAME_COLLISION_H

#include <engine/shared/protocol.h>
#include <base/vmath.h>

#include <vector>
#include <list>
#include <map>

enum
{
	PHYSICSFLAG_STOPPER=1,
	PHYSICSFLAG_TELEPORT=2,
	PHYSICSFLAG_SPEEDUP=4,

	PHYSICSFLAG_RACE_ALL=PHYSICSFLAG_STOPPER|PHYSICSFLAG_TELEPORT|PHYSICSFLAG_SPEEDUP,
};

typedef void (*FPhysicsStepCallback)(vec2 Pos, float IntraTick, void *pUserData);

struct CCollisionData
{
	FPhysicsStepCallback m_pfnPhysicsStepCallback;
	void *m_pPhysicsStepUserData;
	int m_PhysicsFlags;
	ivec2 m_LastSpeedupTilePos;
	bool m_Teleported;
};

class CCollision
{
	class CTile *m_pTiles;
	class CTeleTile *m_pTele;
	class CSpeedupTile *m_pSpeedup;
	class CTile *m_pFront;
	class CTuneTile *m_pTune;
	class CDoorTile *m_pDoor;
	class CSwitchTile *m_pSwitch;
	struct SSwitchers
	{
		bool m_Status[MAX_CLIENTS];
		bool m_Initial;
		int m_EndTick[MAX_CLIENTS];
		int m_Type[MAX_CLIENTS];
	};

	int m_Width;
	int m_Height;
	class CLayers *m_pLayers;

	void InitTeleporter();

	bool IsTile(int x, int y, int Flag=COLFLAG_SOLID) const;
		bool IsRaceTile(int TilePos, int Mask) const;

public:
	enum
	{
		COLFLAG_SOLID=1,
		COLFLAG_DEATH=2,
		COLFLAG_NOHOOK=4,
	};

	CCollision();
	void Init(class CLayers *pLayers);
	bool CheckPoint(float x, float y, int Flag=COLFLAG_SOLID) const { return IsTile(round_to_int(x), round_to_int(y), Flag); }
	bool CheckPoint(vec2 Pos, int Flag=COLFLAG_SOLID) const { return CheckPoint(Pos.x, Pos.y, Flag); }
	int GetCollisionAt(float x, float y) const { return GetTile(round_to_int(x), round_to_int(y)); }
	int GetWidth() const { return m_Width; };
	int GetHeight() const { return m_Height; };
	int IntersectLine(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int IntersectLineTeleHook(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, int *pTeleNr) const;
	int IntersectLineTeleWeapon(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision, int *pTeleNr) const;
	void MovePoint(vec2 *pInoutPos, vec2 *pInoutVel, float Elasticity, int *pBounces) const;
	void MoveBox(vec2 *pInoutPos, vec2 *pInoutVel, vec2 Size, float Elasticity, CCollisionData* pCollisionData = 0, bool *pDeath = 0) const;
	bool TestBox(vec2 Pos, vec2 Size, int Flag=COLFLAG_SOLID) const;
	// race
	vec2 GetPos(int TilePos) const;
	int GetTilePosLayer(const class CMapItemLayerTilemap *pLayer, vec2 Pos) const;

	bool CheckIndexEx(vec2 Pos, int Index) const;
	int CheckIndexExRange(vec2 Pos, int MinIndex, int MaxIndex) const;

	int CheckCheckpoint(vec2 Pos) const;

	class CTeleTile *TeleLayer() { return m_pTele; }
	class CSwitchTile *SwitchLayer() { return m_pSwitch; }
	int m_NumSwitchers;
	SSwitchers *m_pSwitchers;

	std::map<int, std::vector<vec2> > m_TeleOuts;
	std::map<int, std::vector<vec2> > m_TeleCheckOuts;

	void Dest();
	int IsSwitch(int Index) const;
	int GetSwitchNumber(int Index) const;
	int GetSwitchDelay(int Index) const;
	int GetTile(int x, int y) const;
	void SetDTile(float x, float y, bool State) const;
	void SetCollisionAt(float x, float y, int id);
	void SetDCollisionAt(float x, float y, int Type, int Flags, int Number) const;
	int GetDTileIndex(int Index) const;
	int GetDTileFlags(int Index) const;
	int GetDTileNumber(int Index) const;
	int Entity(int x, int y, int Layer) const;
	int GetIndex(int x, int y) const;
	int GetIndex(vec2 PrevPos, vec2 Pos) const;
	int GetFIndex(int x, int y) const;
	int IsWallJump(int Index) const;
	int IsNoLaser(int x, int y) const;
	int IsFNoLaser(int x, int y) const;
	int GetTileIndex(int Index) const;
	int GetFTileIndex(int Index) const;
	int GetTileFlags(int Index) const;
	int GetFTileFlags(int Index) const;
	int IntersectNoLaser(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int IntersectNoLaserNW(vec2 Pos0, vec2 Pos1, vec2 *pOutCollision, vec2 *pOutBeforeCollision) const;
	int GetPureMapIndex(float x, float y) const;
	int GetPureMapIndex(vec2 Pos) const { return GetPureMapIndex(Pos.x, Pos.y); }
	int IsTeleport(int Index) const;
	bool TileExists(int Index) const;
	bool TileExistsNext(int Index) const;
	vec2 CpSpeed(int index, int Flags = 0) const;
	int GetMapIndex(vec2 Pos) const;
	std::list<int> GetMapIndices(vec2 PrevPos, vec2 Pos, unsigned MaxIndices = 0) const;
	int IsEvilTeleport(int Index) const;
	int IsCheckTeleport(int Index) const;
	int IsCheckEvilTeleport(int Index) const;
	int IsTeleportHook(int Index) const;
	int IsMover(int x, int y, int *pFlags) const;
	bool IsThrough(int x, int y, int xoff, int yoff, vec2 pos0, vec2 pos1) const;
	bool IsHookBlocker(int x, int y, vec2 pos0, vec2 pos1) const;
	int GetFCollisionAt(float x, float y) const { return GetFTile(round_to_int(x), round_to_int(y)); }
	int GetFTile(int x, int y) const;
	int IsSpeedup(int Index) const;
	void GetSpeedup(int Index, vec2 *Dir, int *Force, int *MaxSpeed) const;

	int IsTCheckpoint(int Index) const;
	int IsTeleportWeapon(int Index) const;
};

void ThroughOffset(vec2 Pos0, vec2 Pos1, int *Ox, int *Oy);
#endif

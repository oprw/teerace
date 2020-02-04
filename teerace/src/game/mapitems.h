/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_MAPITEMS_H
#define GAME_MAPITEMS_H

// layer types
enum
{
	LAYERTYPE_INVALID=0,
	LAYERTYPE_GAME,
	LAYERTYPE_TILES,
	LAYERTYPE_QUADS,

	MAPITEMTYPE_VERSION=0,
	MAPITEMTYPE_INFO,
	MAPITEMTYPE_IMAGE,
	MAPITEMTYPE_ENVELOPE,
	MAPITEMTYPE_GROUP,
	MAPITEMTYPE_LAYER,
	MAPITEMTYPE_ENVPOINTS,


	CURVETYPE_STEP=0,
	CURVETYPE_LINEAR,
	CURVETYPE_SLOW,
	CURVETYPE_FAST,
	CURVETYPE_SMOOTH,
	CURVETYPE_BEZIER,
	NUM_CURVETYPES,

	// game layer tiles
	ENTITY_NULL=0,
	ENTITY_SPAWN=1,
	ENTITY_SPAWN_RED=2,
	ENTITY_SPAWN_BLUE=3,
	ENTITY_FLAGSTAND_RED=4,
	ENTITY_FLAGSTAND_BLUE=5,
	ENTITY_ARMOR_1=6,
	ENTITY_HEALTH_1=7,
	ENTITY_WEAPON_SHOTGUN=8,
	ENTITY_WEAPON_GRENADE=9,
	ENTITY_POWERUP_NINJA=10,
	ENTITY_WEAPON_LASER,
	ENTITY_WEAPON_RIFLE=11,
	ENTITY_LASER_FAST_CW=12,
	ENTITY_LASER_NORMAL_CW=13,
	ENTITY_LASER_SLOW_CW=14,
	ENTITY_LASER_STOP=15,
	ENTITY_LASER_SLOW_CCW=16,
	ENTITY_LASER_NORMAL_CCW=17,
	ENTITY_LASER_FAST_CCW=18,
	ENTITY_LASER_SHORT=19,
	ENTITY_LASER_MEDIUM=20,
	ENTITY_LASER_LONG=21,
	ENTITY_LASER_C_SLOW=22,
	ENTITY_LASER_C_NORMAL=23,
	ENTITY_LASER_C_FAST=24,
	ENTITY_LASER_O_SLOW=25,
	ENTITY_LASER_O_NORMAL=26,
	ENTITY_LASER_O_FAST=27,
	ENTITY_PLASMAE=29,
	ENTITY_PLASMAF=30,
	ENTITY_PLASMA=31,
	ENTITY_PLASMAU=32,
	ENTITY_CRAZY_SHOTGUN_EX=33,
	ENTITY_CRAZY_SHOTGUN=34,
	ENTITY_DRAGGER_WEAK=42,
	ENTITY_DRAGGER_NORMAL=43,
	ENTITY_DRAGGER_STRONG=44,
	ENTITY_DRAGGER_WEAK_NW=45,
	ENTITY_DRAGGER_NORMAL_NW=46,
	ENTITY_DRAGGER_STRONG_NW=47,
	ENTITY_DOOR=49,
	NUM_ENTITIES=50,

	TILE_AIR=0,
	TILE_SOLID=1,
	TILE_DEATH=2,
	TILE_NOHOOK=3,
	TILE_NOLASER=4,
	TILE_THROUGH_CUT=5,
	TILE_THROUGH=6,
	TILE_JUMP=7,
	TILE_FREEZE = 9,
	TILE_TELEINEVIL=10,
	TILE_UNFREEZE=11,
	TILE_DFREEZE=12,
	TILE_DUNFREEZE=13,
	TILE_TELEINWEAPON=14,
	TILE_TELEINHOOK=15,
	TILE_WALLJUMP = 16,
	TILE_EHOOK_START=17,
	TILE_EHOOK_END=18,
	TILE_HIT_START=19,
	TILE_HIT_END=20,
	TILE_SOLO_START=21,
	TILE_SOLO_END=22,
	TILE_SWITCHTIMEDOPEN = 22,
	TILE_SWITCHTIMEDCLOSE=23,
	TILE_SWITCHOPEN=24,
	//TILE_SWITCHCLOSE=25,
	TILE_SWITCHCLOSE,
	TILE_TELEIN_STOP=25,
	TILE_TELEIN=26,
	TILE_TELEOUT=27,
	TILE_BOOST=28,
	TILE_TELECHECK=29,
	TILE_TELECHECKOUT=30,
	TILE_TELECHECKIN=31,
	TILE_REFILL_JUMPS = 32,
	TILE_BEGIN=33,
	TILE_END=34,
	TILE_STOP = 60,
	TILE_STOPS=61,
	TILE_STOPA=62,
	TILE_TELECHECKINEVIL=63,
	TILE_CP=64,
	TILE_CP_F=65,
	TILE_THROUGH_ALL=66,
	TILE_THROUGH_DIR=67,
	TILE_TUNE1=68,
	TILE_OLDLASER = 71,
	TILE_NPC=72,
	TILE_EHOOK=73,
	TILE_NOHIT=74,
	TILE_NPH=75,
	TILE_UNLOCK_TEAM=76,
	TILE_PENALTY = 79,
	TILE_NPC_END = 88,
	TILE_SUPER_END=89,
	TILE_JETPACK_END=90,
	TILE_NPH_END=91,
	TILE_BONUS = 95,
	TILE_ALLOW_TELE_GUN = 98,
	TILE_ALLOW_BLUE_TELE_GUN = 99,
	TILE_NPC_START = 104,
	TILE_SUPER_START=105,
	TILE_JETPACK_START=106,
	TILE_NPH_START=107,
	TILE_TELE_LASER_ENABLE = 128,
	TILE_TELE_LASER_DISABLE = 129,
	TILE_ENTITIES_OFF_1 = 190,
	TILE_ENTITIES_OFF_2=191,

	LAYER_GAME=0,
	LAYER_FRONT,
	LAYER_TELE,
	LAYER_SPEEDUP,
	LAYER_SWITCH,
	LAYER_TUNE,
	NUM_LAYERS,

	TILEFLAG_VFLIP=1,
	TILEFLAG_HFLIP=2,
	TILEFLAG_OPAQUE=4,
	TILEFLAG_ROTATE=8,

	ROTATION_0 = 0,
	ROTATION_90 = TILEFLAG_ROTATE,
	ROTATION_180 = (TILEFLAG_VFLIP|TILEFLAG_HFLIP),
	ROTATION_270 = (TILEFLAG_VFLIP|TILEFLAG_HFLIP|TILEFLAG_ROTATE),

	LAYERFLAG_DETAIL=1,
	TILESLAYERFLAG_GAME=1,
	TILESLAYERFLAG_TELE=2,
	TILESLAYERFLAG_SPEEDUP=4,
	TILESLAYERFLAG_FRONT=8,
	TILESLAYERFLAG_SWITCH=16,
	TILESLAYERFLAG_TUNE=32,

	ENTITY_OFFSET=255-16*4,
};

struct CPoint
{
	int x, y; // 22.10 fixed point
};

struct CColor
{
	int r, g, b, a;
};

struct CQuad
{
	CPoint m_aPoints[5];
	CColor m_aColors[4];
	CPoint m_aTexcoords[4];

	int m_PosEnv;
	int m_PosEnvOffset;

	int m_ColorEnv;
	int m_ColorEnvOffset;
};

class CTile
{
public:
	unsigned char m_Index;
	unsigned char m_Flags;
	unsigned char m_Skip;
	unsigned char m_Reserved;
};

class CTeleTile
{
public:
	unsigned char m_Number;
	unsigned char m_Type;
};

class CSpeedupTile
{
public:
	unsigned char m_Force;
	unsigned char m_MaxSpeed;
	unsigned char m_Type;
	short m_Angle;
};

class CTuneTile
{
public:
	unsigned char m_Number;
	unsigned char m_Type;
};

class CSwitchTile
{
public:
	unsigned char m_Number;
	unsigned char m_Type;
	unsigned char m_Flags;
	unsigned char m_Delay;
};

class CDoorTile
{
public:
	unsigned char m_Index;
	unsigned char m_Flags;
	int m_Number;
};

struct CMapItemInfo
{
	enum { CURRENT_VERSION=1 };
	int m_Version;
	int m_Author;
	int m_MapVersion;
	int m_Credits;
	int m_License;
} ;

struct CMapItemImage_v1
{
	int m_Version;
	int m_Width;
	int m_Height;
	int m_External;
	int m_ImageName;
	int m_ImageData;
} ;

struct CMapItemImage : public CMapItemImage_v1
{
	enum { CURRENT_VERSION=2 };
	int m_Format;
};

struct CMapItemGroup_v1
{
	int m_Version;
	int m_OffsetX;
	int m_OffsetY;
	int m_ParallaxX;
	int m_ParallaxY;

	int m_StartLayer;
	int m_NumLayers;
} ;


struct CMapItemGroup : public CMapItemGroup_v1
{
	enum { CURRENT_VERSION=3 };

	int m_UseClipping;
	int m_ClipX;
	int m_ClipY;
	int m_ClipW;
	int m_ClipH;

	int m_aName[3];
} ;

struct CMapItemLayer
{
	int m_Version;
	int m_Type;
	int m_Flags;
} ;

struct CMapItemLayerTilemap
{
	enum { CURRENT_VERSION=4 };

	CMapItemLayer m_Layer;
	int m_Version;

	int m_Width;
	int m_Height;
	int m_Flags;

	CColor m_Color;
	int m_ColorEnv;
	int m_ColorEnvOffset;

	int m_Image;
	int m_Data;

	int m_aName[3];
	
	// mkRace
	int m_Tele;
	int m_Speedup;
	int m_Front;
	int m_Switch;
	int m_Tune;
} ;

struct CMapItemLayerQuads
{
	enum { CURRENT_VERSION=2 };

	CMapItemLayer m_Layer;
	int m_Version;

	int m_NumQuads;
	int m_Data;
	int m_Image;

	int m_aName[3];
} ;

struct CMapItemVersion
{
	enum { CURRENT_VERSION=1 };

	int m_Version;
} ;

struct CEnvPoint_v1
{
	int m_Time; // in ms
	int m_Curvetype;
	int m_aValues[4]; // 1-4 depending on envelope (22.10 fixed point)

	bool operator<(const CEnvPoint_v1 &Other) const { return m_Time < Other.m_Time; }
} ;

struct CEnvPoint : public CEnvPoint_v1
{
	// bezier curve only
	// dx in ms and dy as 22.10 fxp
	int m_aInTangentdx[4];
	int m_aInTangentdy[4];
	int m_aOutTangentdx[4];
	int m_aOutTangentdy[4];

	bool operator<(const CEnvPoint& other) const { return m_Time < other.m_Time; }
};

struct CMapItemEnvelope_v1
{
	int m_Version;
	int m_Channels;
	int m_StartPoint;
	int m_NumPoints;
	int m_aName[8];
} ;

struct CMapItemEnvelope_v2 : public CMapItemEnvelope_v1
{
	enum { CURRENT_VERSION=2 };
	int m_Synchronized;
};

struct CMapItemEnvelope : public CMapItemEnvelope_v2
{
	// bezier curve support
	enum { CURRENT_VERSION=3 };
};

#endif

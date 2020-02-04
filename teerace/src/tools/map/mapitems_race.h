#include <game/mapitems.h>

enum
{
//	TILESLAYERFLAG_TELE=2,
//	TILESLAYERFLAG_SPEEDUP=4,

	CFGFLAG_MAPSETTINGS=128
};
/*
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
	short m_Angle;
};
*/
struct CMapItemInfoRace : public CMapItemInfo
{
	int m_Settings;
};

struct CMapItemLayerTilemapRace : public CMapItemLayerTilemap
{
	int m_Tele;
	int m_Speedup;
};


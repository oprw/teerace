/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/color.h>
#include <base/math.h>

#include <engine/console.h>

#include "map.h"

CLayerGame::CLayerGame(int w, int h)
: CLayerTiles(w, h)
{
	str_copy(m_aName, "Game", sizeof(m_aName));
	m_Game = 1;
}

CLayerGame::~CLayerGame()
{
}

CLayerTiles::CLayerTiles(int w, int h)
{
	m_Type = LAYERTYPE_TILES;
	str_copy(m_aName, "Tiles", sizeof(m_aName));
	m_Width = w;
	m_Height = h;
	m_Image = -1;
	m_Game = 0;
	m_Color.r = 255;
	m_Color.g = 255;
	m_Color.b = 255;
	m_Color.a = 255;
	m_ColorEnv = -1;
	m_ColorEnvOffset = 0;

	m_pTiles = new CTile[m_Width*m_Height];
	mem_zero(m_pTiles, m_Width*m_Height*sizeof(CTile));

	m_pSaveTiles = 0;
	m_SaveTilesSize = 0;
}

CLayerTiles::~CLayerTiles()
{
	delete [] m_pTiles;
	m_pTiles = 0;
	delete [] m_pSaveTiles;
	m_pSaveTiles = 0;
	m_SaveTilesSize = 0;
}

void CLayerTiles::PrepareForSave()
{
	int Invisible = 0;
	int OneColor = 0;

	for(int y = 0; y < m_Height; y++)
		for(int x = 0; x < m_Width; x++)
		{
			m_pTiles[y*m_Width+x].m_Flags &= TILEFLAG_VFLIP|TILEFLAG_HFLIP|TILEFLAG_ROTATE;

			char FlagsEx = (m_Image != -1) ? m_pMap->m_lImages[m_Image]->m_aTileFlagsExtended[m_pTiles[y*m_Width+x].m_Index] : 0;
			if(FlagsEx&TILEFLAG_EX_INVISIBLE)
			{
				if(m_pTiles[y*m_Width+x].m_Index != 0)
					Invisible++;
				m_pTiles[y*m_Width+x].m_Index = 0;
			}
			if(FlagsEx&TILEFLAG_EX_ONE_COLOR)
			{
				if(m_pTiles[y*m_Width+x].m_Index != 0 && m_pTiles[y*m_Width+x].m_Flags != 0)
					OneColor++;
				m_pTiles[y*m_Width+x].m_Flags = 0;
			}

			if(m_pTiles[y*m_Width+x].m_Index == 0)
				m_pTiles[y*m_Width+x].m_Flags = 0;
		}

	if(Invisible > 0 || OneColor > 0)
		dbg_msg("mapoptimize", "invisible: %d | one color: %d", Invisible, OneColor);

	if(m_Image != -1 && m_Color.a == 255)
	{
		for(int y = 0; y < m_Height; y++)
			for(int x = 0; x < m_Width; x++)
				m_pTiles[y*m_Width+x].m_Flags |= m_pMap->m_lImages[m_Image]->m_aTileFlags[m_pTiles[y*m_Width+x].m_Index];
	}

	int NumSaveTiles = 0; // number of unique tiles that we have to save
	CTile Tile; // current tile to be duplicated
	Tile.m_Skip = MAX_SKIP; // tell the code that we can't skip the first tile

	int NumHitMaxSkip = -1;

	for(int i = 0; i < m_Width * m_Height; i++)
	{
		// we can only store MAX_SKIP empty tiles in one tile
		if(Tile.m_Skip == MAX_SKIP)
		{
			Tile = m_pTiles[i];
			Tile.m_Skip = 0;
			NumSaveTiles++;
			NumHitMaxSkip++;
		}
		// tile is different from last one? - can't skip it
		else if(m_pTiles[i].m_Index != Tile.m_Index || m_pTiles[i].m_Flags != Tile.m_Flags)
		{
			Tile = m_pTiles[i];
			Tile.m_Skip = 0;
			NumSaveTiles++;
		}
		// if the tile is the same as the previous one - no need to
		// save it separately
		else
			Tile.m_Skip++;
	}

	if(m_pSaveTiles)
		delete [] m_pSaveTiles;

	m_pSaveTiles = new CTile[NumSaveTiles];
	m_SaveTilesSize = sizeof(CTile) * NumSaveTiles;

	int NumWrittenSaveTiles = 0;
	Tile.m_Skip = MAX_SKIP;
	for(int i = 0; i < m_Width * m_Height + 1; i++)
	{
		// again, if an tile is the same as the previous one
		// and we have place to store it, skip it!
		// if we are at the end of the layer, write one more tile
		if(i != m_Width * m_Height && Tile.m_Skip != MAX_SKIP && m_pTiles[i].m_Index == Tile.m_Index && m_pTiles[i].m_Flags == Tile.m_Flags)
		{
			Tile.m_Skip++;
		}
		// tile is not skippable
		else
		{
			// if this is not the first tile, we have to save the previous
			// tile beforehand
			if(i != 0)
				m_pSaveTiles[NumWrittenSaveTiles++] = Tile;

			// if this isn't the last tile, store it so we can check how
			// many tiles to skip
			if(i != m_Width * m_Height)
			{
				Tile = m_pTiles[i];
				Tile.m_Skip = 0;
			}
		}
	}
}

void CLayerTiles::ExtractTiles(CTile *pSavedTiles)
{
	int i = 0;
	while(i < m_Width * m_Height)
	{
		for(unsigned Counter = 0; Counter <= pSavedTiles->m_Skip && i < m_Width * m_Height; Counter++)
		{
			m_pTiles[i] = *pSavedTiles;
			m_pTiles[i++].m_Skip = 0;
		}

		pSavedTiles++;
	}
}

void CLayerTiles::ModifyImageIndex(INDEX_MODIFY_FUNC Func)
{
	Func(&m_Image);
}

void CLayerTiles::ModifyEnvelopeIndex(INDEX_MODIFY_FUNC Func)
{
}

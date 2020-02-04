/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/color.h>
#include <base/math.h>

#include <engine/console.h>

#include "map.h"


CLayerQuads::CLayerQuads()
{
	m_Type = LAYERTYPE_QUADS;
	str_copy(m_aName, "Quads", sizeof(m_aName));
	m_Image = -1;
}

CLayerQuads::~CLayerQuads()
{
}

CQuad *CLayerQuads::NewQuad()
{
	//m_pEditor->m_Map.m_Modified = true;

	CQuad *q = &m_lQuads[m_lQuads.add(CQuad())];

	q->m_PosEnv = -1;
	q->m_ColorEnv = -1;
	q->m_PosEnvOffset = 0;
	q->m_ColorEnvOffset = 0;
	int x = 0, y = 0;
	q->m_aPoints[0].x = i2fx(x);
	q->m_aPoints[0].y = i2fx(y);
	q->m_aPoints[1].x = i2fx(x+64);
	q->m_aPoints[1].y = i2fx(y);
	q->m_aPoints[2].x = i2fx(x);
	q->m_aPoints[2].y = i2fx(y+64);
	q->m_aPoints[3].x = i2fx(x+64);
	q->m_aPoints[3].y = i2fx(y+64);

	q->m_aPoints[4].x = i2fx(x+32); // pivot
	q->m_aPoints[4].y = i2fx(y+32);

	q->m_aTexcoords[0].x = i2fx(0);
	q->m_aTexcoords[0].y = i2fx(0);

	q->m_aTexcoords[1].x = i2fx(1);
	q->m_aTexcoords[1].y = i2fx(0);

	q->m_aTexcoords[2].x = i2fx(0);
	q->m_aTexcoords[2].y = i2fx(1);

	q->m_aTexcoords[3].x = i2fx(1);
	q->m_aTexcoords[3].y = i2fx(1);

	q->m_aColors[0].r = 255; q->m_aColors[0].g = 255; q->m_aColors[0].b = 255; q->m_aColors[0].a = 255;
	q->m_aColors[1].r = 255; q->m_aColors[1].g = 255; q->m_aColors[1].b = 255; q->m_aColors[1].a = 255;
	q->m_aColors[2].r = 255; q->m_aColors[2].g = 255; q->m_aColors[2].b = 255; q->m_aColors[2].a = 255;
	q->m_aColors[3].r = 255; q->m_aColors[3].g = 255; q->m_aColors[3].b = 255; q->m_aColors[3].a = 255;

	return q;
}

void CLayerQuads::GetSize(float *w, float *h) const
{
	*w = 0; *h = 0;

	for(int i = 0; i < m_lQuads.size(); i++)
	{
		for(int p = 0; p < 5; p++)
		{
			*w = max(*w, fx2f(m_lQuads[i].m_aPoints[p].x));
			*h = max(*h, fx2f(m_lQuads[i].m_aPoints[p].y));
		}
	}
}

void CLayerQuads::ModifyImageIndex(INDEX_MODIFY_FUNC Func)
{
	Func(&m_Image);
}

void CLayerQuads::ModifyEnvelopeIndex(INDEX_MODIFY_FUNC Func)
{
	for(int i = 0; i < m_lQuads.size(); i++)
	{
		Func(&m_lQuads[i].m_PosEnv);
		Func(&m_lQuads[i].m_ColorEnv);
	}
}

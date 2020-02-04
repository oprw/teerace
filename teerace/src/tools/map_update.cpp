/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/system.h>
#include <engine/console.h>
#include <engine/storage.h>
#include <engine/shared/datafile.h>

#include "map/map.h"

static IStorage *s_pStorage = 0;


int main(int argc, const char **argv)
{
  dbg_logger_stdout();
  dbg_logger_debugger();

  s_pStorage = CreateStorage("Teeworlds", IStorage::STORAGETYPE_BASIC, argc, argv);
  if(!s_pStorage || argc != 3)
    return -1;

  CEditorMap Map;
  Map.m_pConsole = CreateConsole(0);
  auto m_pConsole = Map.m_pConsole;
  auto pStorage = s_pStorage;

  CDataFileReader DataFile;
  if(!DataFile.Open(s_pStorage, argv[1], IStorage::TYPE_ALL))
    return -1;

  CDataFileWriter df;
  if(!df.Open(s_pStorage, argv[2]))
  {
    char aBuf[256];
    str_format(aBuf, sizeof(aBuf), "failed to open file '%s'...", argv[2]);
    Map.m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "editor", aBuf);
    return 0;
  }
  int Index, ID = 0, Type = 0, Size;
  void *pPtr;

  // add all data
  for(Index = 0; Index < DataFile.NumData(); Index++)
  {
    pPtr = DataFile.GetData(Index);
    Size = DataFile.GetUncompressedDataSize(Index);
    df.AddData(Size, pPtr);
    char aBuf[256];
    str_format(aBuf, sizeof(aBuf), "data %d", Index);
    Map.m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "editor", aBuf);

  }

  // add all items
  for(Index = 0; Index < DataFile.NumItems(); Index++)
  {
    char aBuf[256];
    str_format(aBuf, sizeof(aBuf), "item %d", Index);
    Map.m_pConsole->Print(IConsole::OUTPUT_LEVEL_STANDARD, "editor", aBuf);

    pPtr = DataFile.GetItem(Index, &Type, &ID);
    Size = DataFile.GetItemSize(Index);

    int Start, Num;
    DataFile.GetType( MAPITEMTYPE_IMAGE, &Start, &Num);
    if(Index >= Start && Index < Start+Num)
    {
      CMapItemImage *pItem = (CMapItemImage *)pPtr;
      char *pName = (char *)DataFile.GetData(pItem->m_ImageName);
      // copy base info
      CEditorImage *pImg = new CEditorImage(m_pConsole);
      pImg->m_External = pItem->m_External;

      if(pItem->m_External || (pItem->m_Version > 1 && pItem->m_Format != CImageInfo::FORMAT_RGB && pItem->m_Format != CImageInfo::FORMAT_RGBA))
      {
        LoadExternalImage(m_pConsole, pStorage, pImg, pName);
        pItem->m_External = 0;
        int PixelSize = pImg->m_Format == CImageInfo::FORMAT_RGB ? 3 : 4;
        pItem->m_ImageData = df.AddData(pImg->m_Width*pImg->m_Height*PixelSize, pImg->m_pData);
      }

    }
    df.AddItem(Type, ID, Size, pPtr);
  }

  DataFile.Close();
  df.Finish();

  return 0;
}

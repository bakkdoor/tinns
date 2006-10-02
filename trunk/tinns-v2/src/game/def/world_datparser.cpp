/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community
	maintainer Akiko <akiko@gmx.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
	02110-1301, USA.
*/



/*
	world_datparser.h
	Class to parse .dat world files

	MODIFIED: 29 Sep 2006 Hammag
	REASON: - Creation
*/

#include "main.h"
#include "world_datparser.h"
#include "world_datstruct.h"

 
PWorldDatParser::PWorldDatParser()
{
  f = NULL;
}

PWorldDatParser::~PWorldDatParser()
{
  
}

bool PWorldDatParser::LoadDatFile(char* nFilename)
{
  PWorldFileHeader FileHeader;
  PSectionHeader SectionHeader;
  PSec2ElemHeader Sec2ElemHeader;
  
  u32 FileLen;
  u32 NextSectionOffset = 0;
  u32 NextElementOffset;
  
Console->Print("Reading file %s", nFilename);  
	f = Filesystem->Open("", nFilename);

	if (f)
	{
    FileLen = f->GetSize();

    // Section 1    
Console->Print("Reading file header (section 1) ... ");
    f->Read(&FileHeader, sizeof(PWorldFileHeader));
    if ((FileHeader.mHeaderSize == 0x00000008)
        && (FileHeader.mHeaderSig == 0x000fcfcf)
        && (FileHeader.mSection == 0x00000001))
    {
//Console->Print(GREEN, BLACK, "[OK]");
    }
    else
    {
      Console->Print(RED, BLACK, "Bad data [ERROR]");
      Filesystem->Close(f);
      return false;
    }
    NextSectionOffset += FileHeader.mHeaderSize + 4;
    
    // Other Sections
    //  Header
    while(! f->Eof())
    {
      f->Seek(NextSectionOffset); // Make sure we are at the computed offset
Console->Print("Reading next section header ... ");
      if ((u32)(f->Read(&SectionHeader, sizeof(PSectionHeader))) < sizeof(PSectionHeader))
      {
        Console->Print(RED, BLACK, "Unexpected end of file [ERROR]");
        Filesystem->Close(f);
        return false;
      }
      
      if ((SectionHeader.mHeaderSize == 0x0000000c) && (SectionHeader.mHeaderSig == 0x0000ffcf))
      {
//Console->LPrint(GREEN, BLACK, "[OK]");
      }
      else
      {
        Console->Print(RED, BLACK, "Bad data [ERROR]");
        Filesystem->Close(f);
        return false;
      }
      
      if (SectionHeader.mSection == 0)
      {
Console->Print("Ending section reached");
        break;
      }
    
      NextElementOffset = NextSectionOffset + SectionHeader.mHeaderSize + 4;
      NextSectionOffset = NextElementOffset + SectionHeader.mDataSize;
Console->Print("Processing section %d (size %d)", SectionHeader.mSection, SectionHeader.mDataSize);
      
      if (SectionHeader.mSection == 2)
      {
//int cnt=0;
Console->Print("Element Type 3 size: %d", sizeof(PSec2ElemType3));
        while (NextElementOffset < NextSectionOffset)
        {
          f->Seek(NextElementOffset); // Make sure we are at the computed offset
          
//Console->Print("Reading next element header ... ");
          if ((u32)(f->Read(&Sec2ElemHeader, sizeof(PSec2ElemHeader))) < sizeof(PSec2ElemHeader))
          {
            Console->Print(RED, BLACK, "Unexpected end of file [ERROR]");
            Filesystem->Close(f);
            return false;
          }
          if ((Sec2ElemHeader.mHeaderSize == 0x0000000c) && (Sec2ElemHeader.mHeaderSig == 0x0ffefef1))
          {
//Console->Print(GREEN, BLACK, "[OK]");
          }
          else
          {
            Console->Print(RED, BLACK, "Bad data [ERROR]");
            Filesystem->Close(f);
            return false;
          }  
          NextElementOffset += (Sec2ElemHeader.mHeaderSize + 4 + Sec2ElemHeader.mDataSize);
//Console->Print("Found element %d of type %d, size %d", ++cnt, Sec2ElemHeader.mElementType, Sec2ElemHeader.mDataSize);
          switch(Sec2ElemHeader.mElementType)
          {
            case 1000003:
            {
              ProcessSec2ElemType3(Sec2ElemHeader.mDataSize);
              break;
            }
          }
        }
      }
      else
      {
Console->Print("Section %d ignored", SectionHeader.mSection);
        continue;       
      }
      
    }
    
    Filesystem->Close(f);
  }
  else
    Console->Print("%s Can't read file %s", Console->ColorText(RED, BLACK, "[Error]"), nFilename);
	return true;
}

bool PWorldDatParser::ProcessSec2ElemType3(u32 nSize)
{
  PSec2ElemType3 Data;
  const PDefWorldModel* nWorldModel;
  
  if ((u32)(f->Read(&Data, sizeof(PSec2ElemType3))) < sizeof(PSec2ElemType3))
  {
    Console->Print(RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType3");
    return false;
  }
  
  if (Data.mWorldmodelID > 0)
  {
    nWorldModel = GameDefs->GetWorldModelDef(Data.mWorldmodelID);
    std::string nName;
    if(nWorldModel)
      nName = nWorldModel->GetName();
    else
      nName = "UNKNOWN";
    
    Console->Print("y:%f z:%f x:%f model %d %s (%d)", Data.mPosY , Data.mPosZ, Data.mPosX, Data.mModelID, nName.c_str(), Data.mWorldmodelID);
  }
  
  nSize=0; // just to avoid warning
  return true;
}

/*
struct PSec2ElemType3 //static object ?
{
  s16 mPosY; //= u16 PosY - 32000
  s16 mPosZ;
  s16 mPosX;
  s16 mRotY;
  s16 mRotZ;
  s16 mRotX;
  u32 mUnknown1; //00 00 80 3F ?
  u32 mUnknown2; //01 00 00 00 ?
  u16 mModelID; // points to models.ini
  u32 mUnknown3; //00 00 00 00 ?
  u32 mUnknown4; //00 00 00 00 ?
  u16 mUnknown4bis; //00 00 ?
  u16 mWorldmodelID; // points to worldmodel.def
  u16 mUnknown5; //12 00 ?
  u32 mObjectID;
  s16 mBoxLowerY; //Bounding box, for useflag "64 - selfconstructing colisionbox"
  s16 mBoxLowerZ; // s16 or u16 ?
  s16 mBoxLowerX;
  s16 mBoxUpperY;
  s16 mBoxUpperZ;
  s16 mBoxUpperX;
};
*/

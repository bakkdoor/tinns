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

#include "worlddatatemplate.h"
#include "furnituretemplate.h"

const u16 nonDiscardUseFlags = ufTouchable|ufUsable|ufChair|ufToolTarget ; // furniture always to kept even if function type = 0

PWorldDatParser::PWorldDatParser()
{
  f = NULL;
}

PWorldDatParser::~PWorldDatParser()
{
  
}

int PWorldDatParser::LoadDatFile(const std::string& nFilename, PWorldDataTemplate* nWorld, const bool nDiscardPassiveObjects, const bool nTestAccesOnly)
{
  PWorldFileHeader FileHeader;
  PSectionHeader SectionHeader;
  PSec2ElemHeader Sec2ElemHeader;
  
  u32 FileLen;
  u32 NextSectionOffset = 0;
  u32 NextElementOffset;
  bool ProcessOK;
  
  mWorld = nWorld;
  mDiscardPassiveObjects = nDiscardPassiveObjects;
  
  if (mNCDataPath == "")
  {
    mNCDataPath =Config->GetOption("nc_data_path");
    if (mNCDataPath[mNCDataPath.length()-1] != '/')
      mNCDataPath += '/';
  }
  
if (gDevDebug) Console->Print("Reading file %s", (mNCDataPath + nFilename).c_str());  
	f = Filesystem->Open("", (mNCDataPath + nFilename).c_str());

  if (nTestAccesOnly)
  {
    ProcessOK = (bool)f;
    Filesystem->Close(f);
    return (ProcessOK ? 0 : -1);
  }

	if (f)
	{
    FileLen = f->GetSize();

    // Section 1    
if (gDevDebug) Console->Print("Reading file header (section 1) ... ");
    f->Read(&FileHeader, sizeof(PWorldFileHeader));
    if ((FileHeader.mHeaderSize != 0x00000008)
        || (FileHeader.mHeaderSig != 0x000fcfcf)
        || (FileHeader.mSection != 0x00000001))
    {
      Filesystem->Close(f);
      return -2;
    }
    NextSectionOffset += FileHeader.mHeaderSize + 4;
    
    // Other Sections
    //  Header
    while(! f->Eof())
    {
      f->Seek(NextSectionOffset); // Make sure we are at the computed offset
if (gDevDebug) Console->Print("Reading next section header ... ");
      if ((u32)(f->Read(&SectionHeader, sizeof(PSectionHeader))) < sizeof(PSectionHeader))
      {
        Filesystem->Close(f);
        return -3;
      }
      
      if ((SectionHeader.mHeaderSize != 0x0000000c) || (SectionHeader.mHeaderSig != 0x0000ffcf))
      {
        Filesystem->Close(f);
        return -2;
      }
      
      if (SectionHeader.mSection == 0)
      {
if (gDevDebug) Console->Print("Ending section reached");
        break;
      }
    
      NextElementOffset = NextSectionOffset + SectionHeader.mHeaderSize + 4;
      NextSectionOffset = NextElementOffset + SectionHeader.mDataSize;
if (gDevDebug) Console->Print("Processing section %d (size %d)", SectionHeader.mSection, SectionHeader.mDataSize);
      
      if (SectionHeader.mSection == 2)
      {
int cnt=0;
if (gDevDebug) Console->Print("Element Type 3 size: %d", sizeof(PSec2ElemType3));
        while (NextElementOffset < NextSectionOffset)
        {
          f->Seek(NextElementOffset); // Make sure we are at the computed offset
          
//Console->Print("Reading next element header ... ");
          if ((u32)(f->Read(&Sec2ElemHeader, sizeof(PSec2ElemHeader))) < sizeof(PSec2ElemHeader))
          {
            Filesystem->Close(f);
            return -3;
          }
          if ((Sec2ElemHeader.mHeaderSize != 0x0000000c) || (Sec2ElemHeader.mHeaderSig != 0x0ffefef1))
          {
            Filesystem->Close(f);
            return -2;
          }  
          NextElementOffset += (Sec2ElemHeader.mHeaderSize + 4 + Sec2ElemHeader.mDataSize);
if (gDevDebug) Console->Print("Found element %d of type %d, size %d", ++cnt, Sec2ElemHeader.mElementType, Sec2ElemHeader.mDataSize);
          switch(Sec2ElemHeader.mElementType)
          {
            case 1000003:
            {
              ProcessOK = ProcessSec2ElemType3(Sec2ElemHeader.mDataSize);
              break;
            }
            default:
            {
              ProcessOK = true;
              break;
            }
          }
          
          if (!ProcessOK)
            return -4;
        }
      }
      else
      {
if (gDevDebug) Console->Print("Section %d ignored", SectionHeader.mSection);
        continue;       
      }
      
    }
    
    Filesystem->Close(f);
  }
  else
  { 
    return -1;
  }

	return 0;
}

bool PWorldDatParser::ProcessSec2ElemType3(u32 nSize)
{
  PSec2ElemType3 Data;
  const PDefWorldModel* nWorldModel;
  std::string nName;

  if (nSize != sizeof(PSec2ElemType3))
  {
    Console->Print(RED, BLACK, "[ERROR] Wrong size for Sec2ElemType3 (%d read vs %d needed", nSize, sizeof(PSec2ElemType3));
    return false;
  }   
  if ((u32)(f->Read(&Data, sizeof(PSec2ElemType3))) < sizeof(PSec2ElemType3))
  {
    Console->Print(RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType3");
    return false;
  }
  
  if (Data.mWorldmodelID)
  {
    nWorldModel = GameDefs->GetWorldModelDef(Data.mWorldmodelID);    
    if(nWorldModel)
      nName = nWorldModel->GetName();
    else
      nName = "UNKNOWN";
  }
  else
  {
    nName = "PASSIVE";
    nWorldModel = NULL;
  }

if (gDevDebug) {   
Console->Print("-------------------------------------------------------");
Console->Print("%s (%d) : ID %d", nName.c_str(), Data.mWorldmodelID, Data.mObjectID);
if (!nWorldModel) Console->Print("y:%f z:%f x:%f model %d", Data.mPosY , Data.mPosZ, Data.mPosX, Data.mModelID);
Console->Print("Scale:%f Uk2:0x%08x Uk3:0x%08x", Data.mScale, Data.mUnknown2, Data.mUnknown3);
Console->Print("Uk4:0x%08x Uk5:0x%04x", Data.mUnknown4, Data.mUnknown5);
//Console->Print("Ly:%f Lz:%f Lx:%f", Data.mBoxLowerY, Data.mBoxLowerZ, Data.mBoxLowerX);
//Console->Print("Uy:%f Uz:%f Ux:%f", Data.mBoxUpperY, Data.mBoxUpperZ, Data.mBoxUpperX);
}
  
  if ((!nWorldModel || (!nWorldModel->GetFunctionType() && !(nWorldModel->GetUseFlags() & nonDiscardUseFlags))) && mDiscardPassiveObjects)
  {
if (gDevDebug) Console->Print("Discarded");
    return true; 
  }
  
  PFurnitureItemTemplate* nItem = new PFurnitureItemTemplate;
  nItem->mObjectID = Data.mObjectID;

  // The commented out values are not loaded from dat file atm because they are not used yet.
//  nItem->mPosY = 32000 + Data.mPosY;
//  nItem->mPosZ = 32000 + Data.mPosZ;
//  nItem->mPosX = 32000 + Data.mPosX;
//  nItem->mRotY = Data.mRotY;
//  nItem->mRotZ = Data.mRotZ;
//  nItem->mRotX = Data.mRotX;
//  nItem->mScale = Data.mScale;
//  nItem->mUnknown2 = Data.mUnknown2;
  nItem->mModelID = Data.mModelID;
//  nItem->mUnknown3 = Data.mUnknown3;
//  nItem->mUnknown4 = Data.mUnknown4;
  nItem->mWorldmodelID = Data.mWorldmodelID;
//  nItem->mUnknown5 = Data.mUnknown5;

//  nItem->mBoxLowerY = Data.mBoxLowerY;
//  nItem->mBoxLowerZ = Data.mBoxLowerZ;
//  nItem->mBoxLowerX = Data.mBoxLowerX;
//  nItem->mBoxUpperY = Data.mBoxUpperY;
//  nItem->mBoxUpperZ = Data.mBoxUpperZ;
//  nItem->mBoxUpperX = Data.mBoxUpperX;
    
  if(nWorldModel)
  {
    nItem->mUseFlags = nWorldModel->GetUseFlags();
    nItem->mFunctionType = nWorldModel->GetFunctionType();
    nItem->mFunctionValue = nWorldModel->GetFunctionValue();
  }
  
  mWorld->AddFurnitureItem(nItem);
  
  return true;
}

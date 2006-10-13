/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  worlds.cpp - world class and world map class

	MODIFIED: 06 Oct 2006 Hammag
	REASON: - creation
	
*/


#include "main.h"

#include "filesystem.h"
#include "worlds.h"
#include "worlddatatemplate.h"
#include "gamedefs.h"
#include "def_worlds.h"

#include <set>

/**** PWorld ****/

PWorld::PWorld()
{
  
}

PWorld::~PWorld()
{

}

bool PWorld::Create(u32 nWorldID)
{
  nWorldID = nWorldID;
  return true; 
} 
 

/**** PWorlds ****/

PWorlds::PWorlds()
{
    mPreloadWorldsTemplates = false; // to be put as config option
    mPreloadStaticWorlds = true; // to be put as config option
}

PWorlds::~PWorlds()
{
  for (PWorldsMap::iterator i=mStaticWorldsMap.begin(); i!=mStaticWorldsMap.end(); i++)
    if (i->second) delete i->second;
  for (PWorldsMap::iterator i=mOnDemandWorldsMap.begin(); i!=mOnDemandWorldsMap.end(); i++)
    if (i->second) delete i->second;
  for (PWorldDataTemplatesMap::iterator i=mWorldDataTemplatesMap.begin(); i!=mWorldDataTemplatesMap.end(); i++)
    if (i->second) delete i->second;   
}

bool PWorlds::LeaseWorldDataTemplate(const std::string& nWorldName, const std::string& nFileName, const bool nPreloadPhase)
{
  PWorldDataTemplate* tWorldDataTemplate;
  
  PWorldDataTemplatesMap::iterator it = mWorldDataTemplatesMap.find(nWorldName);
	if(it == mWorldDataTemplatesMap.end()) // template unkown yet
	{
	  if (nPreloadPhase) // if in preload phase, we try to load it or make it known
	  {
	    tWorldDataTemplate = new PWorldDataTemplate;
	    if (tWorldDataTemplate->LoadDatFile(nFileName, !mPreloadWorldsTemplates))
	    {
        if (mPreloadWorldsTemplates)
        {
          mWorldDataTemplatesMap.insert(std::make_pair(nWorldName, tWorldDataTemplate));
          tWorldDataTemplate->IncreaseUseCount();
        }
        else
        {
          mWorldDataTemplatesMap.insert(std::make_pair(nWorldName, (PWorldDataTemplate*)NULL)); // NULL means file access OK but not preloaded yet
          delete tWorldDataTemplate;
        }	      
	      //return true;
	    }
	    else
	    {
	      delete tWorldDataTemplate;
	      return false;
	    }
	  }
	  else
	  {
	    return false;
	  }
	}
	else // template already known
	{
	  if (!it->second  &&  !nPreloadPhase) // template known but not already loaded and not in preload ?
	  {
	    tWorldDataTemplate = new PWorldDataTemplate;
	    if (tWorldDataTemplate->LoadDatFile(nFileName))
	    {
	      it->second = tWorldDataTemplate;
	    }
	    else
	    {
	      Console->Print("%s Invalid world data template file %s", Console->ColorText(RED, BLACK, "[ERROR]"), nFileName.c_str());
	      mWorldDataTemplatesMap.erase(it);
	      delete tWorldDataTemplate;
	      return false;
	    }
	  }

	  if (!nPreloadPhase)
	    it->second->IncreaseUseCount();
	 
	  //return true;
	}
  
  return true;
}

void PWorlds::ReleaseWorldDataTemplate(const std::string& nWorldName)
{  
  PWorldDataTemplatesMap::iterator it = mWorldDataTemplatesMap.find(nWorldName);
	if((it != mWorldDataTemplatesMap.end()) && it->second)
	{
	   it->second->DecreaseUseCount();
	}
	else
	  Console->Print("%s PWorlds::ReleaseWorldDataTemplate : try to relese not loaded template %s", Console->ColorText(RED, BLACK, "[Warning]"), nWorldName.c_str());
}

void PWorlds::UnloadWorldDataTemplate(const std::string& nWorldName)
{  
  PWorldDataTemplate* tWorldDataTemplate;
  
  PWorldDataTemplatesMap::iterator it = mWorldDataTemplatesMap.find(nWorldName);
	if((it != mWorldDataTemplatesMap.end()) && it->second)
	{
    tWorldDataTemplate = it->second;
    if (mPreloadWorldsTemplates ||(tWorldDataTemplate->GetUseCount() > 0))
	    Console->Print("%s PWorlds::UnloadWorldDataTemplate : attempt to unload template %s when use count not null ou preload set", Console->ColorText(RED, BLACK, "[Warning]"), nWorldName.c_str());
	  else
	  {
	    it->second = (PWorldDataTemplate*)NULL;
	    delete tWorldDataTemplate;
	  }
	}
	else
	  Console->Print("%s PWorlds::UnloadWorldDataTemplate : attempt to release not loaded template %s", Console->ColorText(RED, BLACK, "[Warning]"), nWorldName.c_str());
}

bool PWorlds::LoadWorlds() // once Load is done, only WorldDataTemplate registred in mWorldDataTemplatesMap
{                          //   will be considered as valid
  std::string tFileName;
  const PDefWorld* tDefWorld;
  bool tCheckOK;
  int ValidCount = 0, InvalidCount = 0, DblInvCount = 0;
  int DatTmplCount, BadDatTmplCount;
  std::set<std::string> InvalideFiles;
  
  // Appartment templates checking or preloading
  PDefAppartementMap::const_iterator itAptStart = GameDefs->GetAppartementDefsConstIteratorBegin();
	PDefAppartementMap::const_iterator itAptEnd = GameDefs->GetAppartementDefsConstIteratorEnd();
  for (PDefAppartementMap::const_iterator i=itAptStart; i!=itAptEnd; i++)
  {
    tCheckOK = false;
    tFileName = std::string("worlds/") + i->second->GetWorldName() + ".dat";
    if (!InvalideFiles.count(tFileName))
    {
      tCheckOK = LeaseWorldDataTemplate(i->second->GetWorldName(), tFileName, true);
      if (!tCheckOK)
      {
        InvalideFiles.insert(tFileName);
//if (gDevDebug) Console->Print(RED, BLACK, "Template file %s invalid", tFileName.c_str());
      }
    }
    if (!tCheckOK) // in case full path was given without omiting worlds/ or in another dir/archive ...
    {
      tFileName = i->second->GetWorldName() + ".dat";
      if(!InvalideFiles.count(tFileName))
      {
        tCheckOK = LeaseWorldDataTemplate(i->second->GetWorldName(), tFileName, true);
        if (!tCheckOK)
        {
          InvalideFiles.insert(tFileName);
          ++DblInvCount;
        }
      }
    }
    
    if (tCheckOK)
    {
      ++ValidCount;
//if (gDevDebug) Console->Print(GREEN, BLACK, "Template file %s for appartment %d (%s) loaded", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str());
    }
    else
    {
      ++InvalidCount;
//if (gDevDebug) Console->Print(RED, BLACK, "Template file %s for appartment %d (%s) not available or invalid", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str());
    }
  }

  DatTmplCount = mWorldDataTemplatesMap.size();
  BadDatTmplCount = InvalideFiles.size();
  Console->Print("%s %d valid appartement templates checked (%d dat files)", Console->ColorText(GREEN, BLACK, "[Success]"), ValidCount, DatTmplCount);
  if (InvalidCount)
    Console->Print("%s %d invalid appartement templates rejected (%d dat files)", Console->ColorText(YELLOW, BLACK, "[Notice]"), InvalidCount, BadDatTmplCount - DblInvCount);
  
  // Static worlds & static worlds templates checking or preloading
  ValidCount = InvalidCount = 0;
  const PDefWorldFile* tDefWorldFile;
  PDefWorldFileMap::const_iterator itFilStart = GameDefs->GetWorldFileDefsConstIteratorBegin();
	PDefWorldFileMap::const_iterator itFilEnd = GameDefs->GetWorldFileDefsConstIteratorEnd();
  for (PDefWorldFileMap::const_iterator i=itFilStart; i!=itFilEnd; i++)
  {
    tDefWorldFile = i->second;
    
    tDefWorld = GameDefs->GetWorldDef(tDefWorldFile->GetIndex());
    if (tDefWorld) // we only care for worlds that are present in worldinfo.def too
    {
      if(!(tDefWorld->GetDatFile().empty()))
        tFileName = tDefWorld->GetDatFile();
      else
        tFileName = i->second->GetBasicFileName() + ".dat";
    
      tCheckOK = false;
      if (!InvalideFiles.count(tFileName))
      {
        tCheckOK = LeaseWorldDataTemplate(i->second->GetName(), tFileName, true);
        if (!tCheckOK)
        {
          InvalideFiles.insert(tFileName);
//if (gDevDebug) Console->Print(RED, BLACK, "Template file %s invalid", tFileName.c_str());
        }
      }
            
      if (tCheckOK)
      {
        ++ValidCount;
        if (mPreloadStaticWorlds)
        {
          LeaseWorld(tDefWorldFile->GetIndex()); // This will make the world ready and kept in mem (use count always >0 )
        }
//if (gDevDebug) Console->Print(GREEN, BLACK, "Template file %s for world %d (%s) loaded", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str());
      }
      else
      {
        ++InvalidCount;
//if (gDevDebug) Console->Print(RED, BLACK, "Template file %s for world %d (%s) not available or invalid", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str());
      }
    }
  }

  // Manually add neofrag worlds ... oO ... and neofrag4.dat can't be found :-/
  for (int i = 1; i <= 16; i++)
	{
	  char worldName[19];
	  int MatchID = i;
	  if (MatchID > 8) // to care for Neofrag 1 & 2
	    MatchID -= 8;
	  if (MatchID > 6)
	    MatchID = 6; // holomatch 7 and 8 are same as 6
	  snprintf(worldName, 19, "holomatch/neofrag%d", MatchID);
	  
    tDefWorld = GameDefs->GetWorldDef(90000 + i);
    if (tDefWorld) // we only care for worlds that are present in worldinfo.def too
    {
      if(!(tDefWorld->GetDatFile().empty()))
        tFileName = tDefWorld->GetDatFile();
      else
      {
        tFileName = "worlds/";
        tFileName += worldName;
        tFileName += ".dat";
      }
    
      tCheckOK = false;
      if (!InvalideFiles.count(tFileName))
      {
        tCheckOK = LeaseWorldDataTemplate(std::string(worldName), tFileName, true);
        if (!tCheckOK)
        {
          InvalideFiles.insert(tFileName);
if (gDevDebug) Console->Print(RED, BLACK, "Template file %s invalid", tFileName.c_str());
        }
      }
            
      if (tCheckOK)
      {
        ++ValidCount;
        if (mPreloadStaticWorlds)
        {
          LeaseWorld(90000 + i); // This will make the world ready and kept in mem (use count always >0 )
        }
if (gDevDebug) Console->Print(GREEN, BLACK, "Template file %s for world %d (%s) loaded", tFileName.c_str(), 90000+i, worldName);
      }
      else
      {
        ++InvalidCount;
if (gDevDebug) Console->Print(RED, BLACK, "Template file %s for world %d (%s) not available or invalid", tFileName.c_str(), 90000+i, worldName);
      }
	  }
	}
    
  Console->Print("%s %d valid world templates checked (%d dat files)", Console->ColorText(GREEN, BLACK, "[Success]"), ValidCount, mWorldDataTemplatesMap.size() - DatTmplCount);
  if (InvalidCount)
    Console->Print("%s %d invalid world templates rejected (%d dat files)", Console->ColorText(YELLOW, BLACK, "[Notice]"), InvalidCount, InvalideFiles.size() - BadDatTmplCount - DblInvCount);
  
  return true;
}

bool PWorlds::LeaseWorld(u32 nWorldID)
{
  nWorldID = nWorldID; // temp
  return true; // temp
}

PWorld* PWorlds::GetWorld(u32 nWorldID)
{
  nWorldID = nWorldID; // temp
  return NULL;  // temp
}

void ReleaseWorld(u32 nWorldID)
{
  nWorldID = nWorldID; // temp
}

bool PWorlds::IsAppartment(u32 nWorldID)
{
  nWorldID = nWorldID; // temp
  return true;  // temp
} 

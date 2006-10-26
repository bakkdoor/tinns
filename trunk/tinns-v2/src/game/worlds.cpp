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
	mID = 0;
	mUseCount = 0;
	mWorldDataTemplate = NULL;
}

PWorld::~PWorld()
{

}

bool PWorld::Load(u32 nWorldID)
{
  std::string WorldTemplateName;
  std::string tFileName;
  bool tCheckOK;
    
  if (nWorldID > APT_BASE_WORLD_ID)
  {
    int AptTmplID = MySQL->GetAptType(nWorldID - APT_BASE_WORLD_ID);
    if (!AptTmplID)
      return false;
    const PDefAppartement* nAppDef = GameDefs->GetAppartementDef(AptTmplID);
    if (!nAppDef)
      return false;
    WorldTemplateName = nAppDef->GetWorldName();
    
    tFileName = std::string("worlds/") + WorldTemplateName + ".dat";
    tCheckOK = Worlds->LeaseWorldDataTemplate(WorldTemplateName, tFileName);
    if (!tCheckOK)
    {
      tFileName = WorldTemplateName + ".dat";
      tCheckOK = Worlds->LeaseWorldDataTemplate(WorldTemplateName, tFileName);
    }
    if (!tCheckOK)
      return false;
  }
  else
  {
    const PDefWorldFile* nWorldFileDef = GameDefs->GetWorldFileDef(nWorldID);
    if (!nWorldFileDef)
      return false;
    WorldTemplateName = nWorldFileDef->GetName();
    
    const PDefWorld* tWorldDef = GameDefs->GetWorldDef(nWorldID);
    if (tWorldDef) // should always be true here
    {
      if(!(tWorldDef->GetDatFile().empty()))
        tFileName = tWorldDef->GetDatFile();
      else
        tFileName = nWorldFileDef->GetBasicFileName() + ".dat";
    }
    else
      return false; // should'nt happen here
      
    if (!Worlds->LeaseWorldDataTemplate(WorldTemplateName, tFileName))
      return false;    
  }
  
  mWorldDataTemplate = Worlds->GetWorldDataTemplate(WorldTemplateName);
  if(!mWorldDataTemplate)
  {
    Console->Print("%s PWorld::Load : Unexpected world %d not found error", Console->ColorText(RED, BLACK, "[Warning]"), nWorldID);
    return false;
  }
  
  mID = nWorldID;
   // furniture & other world stuff loading here
Console->Print("%s Loaded world %d", Console->ColorText(GREEN, BLACK, "[Debug]"), nWorldID);
  return true; 
} 


/**** PWorlds ****/

PWorlds::PWorlds()
{
    mPreloadWorldsTemplates = false; // to be put as config option
    mPreloadStaticWorlds = false; // to be put as config option
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

PWorldDataTemplate* PWorlds::GetWorldDataTemplate(const std::string& nWorldName)
{
  PWorldDataTemplatesMap::const_iterator it = mWorldDataTemplatesMap.find(nWorldName);
  if(it != mWorldDataTemplatesMap.end())
    return it->second;
  else
    return NULL;
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
          LeaseWorld(tDefWorldFile->GetIndex(), true); // This will make the world ready and kept in mem (use count always >0 )
        }
        else
        {
          mStaticWorldsMap.insert(std::make_pair(tDefWorldFile->GetIndex(), (PWorld*)NULL));
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

  // Hardcoded neofrag worlds ... oO ... and neofrag4.dat can't be found :-/
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
//if (gDevDebug) Console->Print(RED, BLACK, "Template file %s invalid", tFileName.c_str());
        }
      }
            
      if (tCheckOK)
      {
        ++ValidCount;
        if (mPreloadStaticWorlds)
        {
          LeaseWorld(90000 + i, true); // This will make the world ready and kept in mem (use count always >0 )
        }
        else
        {
          mStaticWorldsMap.insert(std::make_pair(90000 + i, (PWorld*)NULL));
        }
//if (gDevDebug) Console->Print(GREEN, BLACK, "Template file %s for world %d (%s) loaded", tFileName.c_str(), 90000+i, worldName);
      }
      else
      {
        ++InvalidCount;
//if (gDevDebug) Console->Print(RED, BLACK, "Template file %s for world %d (%s) not available or invalid", tFileName.c_str(), 90000+i, worldName);
      }
	  }
	}
    
  Console->Print("%s %d valid world templates checked (%d dat files)", Console->ColorText(GREEN, BLACK, "[Success]"), ValidCount, mWorldDataTemplatesMap.size() - DatTmplCount);
  if (InvalidCount)
    Console->Print("%s %d invalid world templates rejected (%d dat files)", Console->ColorText(YELLOW, BLACK, "[Notice]"), InvalidCount, InvalideFiles.size() - BadDatTmplCount - DblInvCount);
  Console->Print("%s %d static worlds prepared", Console->ColorText(GREEN, BLACK, "[Success]"), mStaticWorldsMap.size());
  
  // release memory if World templates preload activated, this cache that won't be used anymore
  // if (mPreloadWorldsTemplates) Filesystem->ClearCache();
  
  return true;
}

bool PWorlds::IsValidWorld(u32 nWorldID)
{
  if (nWorldID > APT_BASE_WORLD_ID)
	{
    if (mOnDemandWorldsMap.count(nWorldID)) // Check if already loaded
      return true;
    else
      return true; //do a check using the PAppartements class object to get the world template
	}
	else
	{
if (gDevDebug) Console->Print("%s Checking validity for world %d : %s ", Console->ColorText(GREEN, BLACK, "[Debug]"), nWorldID, mStaticWorldsMap.count(nWorldID) ? "OK" : "BAD");
		return (mStaticWorldsMap.count(nWorldID));
	}
}

PWorld* PWorlds::LeaseWorld(u32 nWorldID, const bool nPreloadPhase)
{
  PWorldsMap::iterator it;
    
  if (nWorldID > APT_BASE_WORLD_ID)
	{
    it = mOnDemandWorldsMap.find(nWorldID); // Check if already loaded
    if((it != mOnDemandWorldsMap.end()) && it->second) // Dynamic world shall not have a NULL it->second 
    { // if loaded
      it->second->IncreaseUseCount();
      return it->second;
    }
    else // not loaded yet or invalid
    {
      PWorld* nWorld = new PWorld;
      if (! nWorld->Load(nWorldID)) // Error when loading (shouldn't happen)
      {
        delete nWorld;
        return NULL; 
      }
      else
      {
        mOnDemandWorldsMap.insert(std::make_pair(nWorldID, nWorld));
        nWorld->IncreaseUseCount();
        return nWorld;
      }
    }
	}
	else
	{
    it = mStaticWorldsMap.find(nWorldID); // Check if already loaded
    if((it == mStaticWorldsMap.end()) && nPreloadPhase)
    {
      mStaticWorldsMap.insert(std::make_pair(nWorldID, (PWorld*)NULL));
      it = mStaticWorldsMap.find(nWorldID);
    }
    if(it != mStaticWorldsMap.end())
    { // if valid
      if (! it->second) // if not loaded yet
      {
        it->second = new PWorld;
        if (! it->second->Load(nWorldID)) // Error when loading (shouldn't happen)
        {
          delete it->second;
          mStaticWorldsMap.erase(it); // remove from valid worlds map
          return NULL; 
        }
      }
      it->second->IncreaseUseCount();
if (gDevDebug) Console->Print("%s Leased world %d", Console->ColorText(GREEN, BLACK, "[Debug]"), nWorldID);
      return it->second;
    }
    else // invalid worldID
    {
      return NULL;
    }	  
	}
}

PWorld* PWorlds::GetWorld(u32 nWorldID)
{
  PWorldsMap* tMap;
  PWorldsMap::iterator it;
    
  tMap = ((nWorldID > APT_BASE_WORLD_ID) ? &mOnDemandWorldsMap : &mStaticWorldsMap);
	
  it = tMap->find(nWorldID);
  if((it != tMap->end()) && it->second)
  {
    return it->second;
  }
  else
  {
    Console->Print("%s PWorlds::GetWorld : Trying to get world %d without lease !", Console->ColorText(RED, BLACK, "[Warning]"), nWorldID);
    return NULL;
  }
}

void PWorlds::ReleaseWorld(u32 nWorldID) // no dynamic unload is performed atm
{
  PWorld* tWorld = GetWorld(nWorldID);
  if (tWorld)
  {
    if (tWorld->GetUseCount()) // this check is for dev time only
    {
      tWorld->DecreaseUseCount();
if (gDevDebug) Console->Print("%s Released world %d", Console->ColorText(GREEN, BLACK, "[Debug]"), nWorldID);
    }
    else
    {
      Console->Print("%s PWorlds::ReleaseWorld : Trying to release world %d with use count 0 !", Console->ColorText(RED, BLACK, "[Warning]"), nWorldID);
    }
  }
  else
  {
    Console->Print("%s PWorlds::ReleaseWorld : Generated the invalid get world %d", Console->ColorText(RED, BLACK, "[Warning]"), nWorldID);
  }
}

bool PWorlds::IsAppartment(u32 nWorldID)
{
  return ((nWorldID > APT_BASE_WORLD_ID) && IsValidWorld(nWorldID));
}

void PWorlds::Update()
{
}

void PWorlds::Shutdown()
{
}

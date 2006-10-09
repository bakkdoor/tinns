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
	  if (!it->second  &&  !nPreloadPhase) // is template not already loaded and we are not in preload ?
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

void ReleaseWorldDataTemplate(const std::string& nWorldName)
{
  string t = nWorldName; t = t; // tmp
}

bool PWorlds::LoadWorlds() // once Load is done, only WorldDataTemplate registred in mWorldDataTemplatesMap
{                          //   will be cobnsidered as valid
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
	PDefWorldFileMap::const_iterator itFilEnd = GameDefs->GetWorldFileConstIteratorEnd();
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

  Console->Print("%s %d valid world templates checked (%d dat files)", Console->ColorText(GREEN, BLACK, "[Success]"), ValidCount, mWorldDataTemplatesMap.size() - DatTmplCount);
  if (InvalidCount)
    Console->Print("%s %d invalid world templates rejected (%d dat files)", Console->ColorText(YELLOW, BLACK, "[Notice]"), InvalidCount, InvalideFiles.size() - BadDatTmplCount - DblInvCount);

  /**** neofrag zones must be manually added oO ****/ 
  
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

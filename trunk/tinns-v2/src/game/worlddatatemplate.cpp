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
  worlddatatemplate.cpp - world data template (from worlds .dat files) class

	MODIFIED: 04 Oct 2006 Hammag
	REASON: - creation
	
*/


#include "main.h"

#include "worlddatatemplate.h"
#include "world_datparser.h"
#include "furnituretemplate.h"
#include "doortemplate.h"

PWorldDataTemplate::PWorldDataTemplate()
{
  mUseCount = 0;
}

PWorldDataTemplate::~PWorldDataTemplate()
{
  DatFileDataCleanup();
}

void PWorldDataTemplate::DatFileDataCleanup()
{
  for (PFurnitureItemsMap::iterator i=mFurnitureItems.begin(); i!=mFurnitureItems.end(); i++)
    delete i->second;
  for (PDoorsMap::iterator i=mDoors.begin(); i!=mDoors.end(); i++)
    delete i->second;
}

bool PWorldDataTemplate::LoadDatFile(const std::string& nWorldName, const std::string& nFilename, const bool nTestAccesOnly)
{
  PWorldDatParser WDatLoader;
  int LoadResult;

  DatFileDataCleanup();
if (gDevDebug) Console->Print("Loading %s", nFilename.c_str());
  LoadResult = WDatLoader.LoadDatFile(nFilename, this, true, nTestAccesOnly) ; // We want to discard passive objects for now
  switch (LoadResult)
  {
    case 0:
    {
      mName = nWorldName;
      return true;
    }
      
    case -1:
    {
      if (!nTestAccesOnly)
        Console->Print("%s Can't read file %s", Console->ColorText(RED, BLACK, "[Error]"), nFilename.c_str());
      break;
    }
    
    case -2:
    {
      Console->Print("%s Bad data", Console->ColorText(RED, BLACK, "[Error]"));
      break;
    }
      
    case -3:
    {
      Console->Print("%s Unexpected end of file", Console->ColorText(RED, BLACK, "[Error]"));
      break;
    }
    
    default:
      Console->Print("%s Unknown error %d",Console->ColorText(RED, BLACK, "[Error]"), LoadResult);
  }
  return false;
}

u32 PWorldDataTemplate::AddFurnitureItem(PFurnitureItemTemplate* nItem)
{
  if (nItem)
  {
    if(mFurnitureItems.insert(std::make_pair(nItem->GetID(), nItem)).second)
    {
if (gDevDebug) Console->Print("Furniture item %d added to world template", nItem->GetID());
if (gDevDebug)
{
if (nItem->GetDefWorldModel()->GetID() == 24)
  Console->Print("Added Door access IF %d", nItem->GetID());
else if (nItem->GetDefWorldModel()->GetID() == 26)
  Console->Print("Added Access/Communicator IF %d", nItem->GetID());  
}  
      return nItem->GetID();
    }
    else
    {
      Console->Print(RED, BLACK, "Duplicate furniture item ID %d !!! Not added to world template", nItem->GetID());      
    }
  }
  return 0;
}

const PFurnitureItemTemplate* PWorldDataTemplate::GetFurnitureItem(u32 ItemID)
{
  PFurnitureItemsMap::const_iterator it = mFurnitureItems.find(ItemID);
	if(it == mFurnitureItems.end())
	  return NULL;
  else
    return it->second;
}

u32 PWorldDataTemplate::AddDoor(PDoorTemplate* nDoor)
{
  if (nDoor)
  {
    if(mDoors.insert(std::make_pair(nDoor->GetID(), nDoor)).second)
    {
if (gDevDebug) Console->Print("Door %d added to world template", nDoor->GetID());
      return nDoor->GetID();
    }
    else
    {
      Console->Print(RED, BLACK, "Duplicate Door ID %d !!! Not added to world template", nDoor->GetID());      
    }
  }
  return 0;
}

const PDoorTemplate* PWorldDataTemplate::GetDoor(u32 DoorID)
{
  PDoorsMap::const_iterator it = mDoors.find(DoorID);
	if(it == mDoors.end())
	  return NULL;
  else
    return it->second;
}

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
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "common/filesystem.h"
#include "include/def_worlds.h"
#include "include/gamedefs.h"
#include "include/furnituretemplate.h"
#include "include/worlddatatemplate.h"
#include "include/worlds.h"
#include "include/appartements.h"
#include "include/vehicle.h"

#include <set>


/**** PWorld ****/
u16 const PWorld::mZoneOutLimitOffset = 0x100;
u16 const PWorld::mBottomZoneOutLimit = 0x4800 - PWorld::mZoneOutLimitOffset;
u16 const PWorld::mBottomZoneInLimit = 0x4a00;
u16 const PWorld::mTopZoneOutLimit = 0xb200 + PWorld::mZoneOutLimitOffset;
u16 const PWorld::mTopZoneInLimit = 0xb000;

PWorld::PWorld()
{
  mID = 0;
  mUseCount = 0;
  mWorldDataTemplate = NULL;
}

PWorld::~PWorld()
{
}

bool PWorld::Load( u32 nWorldID )
{
  std::string WorldTemplateName;
  std::string tFileName;
  bool tCheckOK;

  if ( nWorldID > PWorlds::mAptBaseWorldId )
  {
    //int AptTmplID = Appartements->GetAptType(nWorldID - PWorlds::mAptBaseWorldId);
    int AptTmplID = Appartements->GetAptType( nWorldID );
    if ( !AptTmplID )
    {
      Console->Print( "%s PWorld::Load - invalid apt %d", Console->ColorText( RED, BLACK, "[WARNING]" ), nWorldID - PWorlds::mAptBaseWorldId );
      return false;
    }
    const PDefAppartement* nAppDef = GameDefs->Appartements()->GetDef( AptTmplID );
    if ( !nAppDef )
    {
      Console->Print( "%s PWorld::Load - invalid apt type %d", Console->ColorText( RED, BLACK, "[WARNING]" ), AptTmplID );
      return false;
    }
    WorldTemplateName = nAppDef->GetWorldName();

    tFileName = std::string( "worlds/" ) + WorldTemplateName + ".dat";
    tCheckOK = Worlds->LeaseWorldDataTemplate( WorldTemplateName, tFileName );
    if ( !tCheckOK )
    {
      tFileName = WorldTemplateName + ".dat";
      tCheckOK = Worlds->LeaseWorldDataTemplate( WorldTemplateName, tFileName );
    }
    if ( !tCheckOK )
    {
      Console->Print( "%s PWorld::Load - unable to lease apt world %s (%s)", Console->ColorText( RED, BLACK, "[WARNING]" ), WorldTemplateName.c_str(), tFileName.c_str() );
      return false;
    }
  }
  else
  {
    const PDefWorldFile* nWorldFileDef;

    if (( nWorldID > 90000 ) && ( nWorldID < 90017 ) ) // hardcoded holomatch hack
    {
      nWorldFileDef = NULL;
      char worldName[19];
      int MatchID = nWorldID - 90000;
      if ( MatchID > 8 ) // to care for Neofrag 1 & 2
        MatchID -= 8;
      if ( MatchID > 6 )
        MatchID = 6; // holomatch 7 and 8 are same as 6
      snprintf( worldName, 19, "holomatch/neofrag%d", MatchID );
      WorldTemplateName = worldName;
    }
    else
    {
      nWorldFileDef = GameDefs->WorldFiles()->GetDef( nWorldID );
      if ( !nWorldFileDef )
        return false;
      WorldTemplateName = nWorldFileDef->GetName();
    }

    const PDefWorld* tWorldDef = GameDefs->Worlds()->GetDef( nWorldID );
    if ( tWorldDef ) // should always be true here
    {
      if ( !( tWorldDef->GetDatFile().empty() ) )
        tFileName = tWorldDef->GetDatFile();
      else if ( nWorldFileDef )
        tFileName = nWorldFileDef->GetBasicFileName() + ".dat";
      else
      {
        tFileName = "worlds/";
        tFileName += WorldTemplateName;
        tFileName += ".dat";
      }
    }
    else
      return false; // should'nt happen here

    if ( !Worlds->LeaseWorldDataTemplate( WorldTemplateName, tFileName ) )
      return false;
  }

  mWorldDataTemplate = Worlds->GetWorldDataTemplate( tFileName );
  if ( !mWorldDataTemplate )
  {
    Console->Print( "%s PWorld::Load : Unexpected world %d not found error", Console->ColorText( RED, BLACK, "[WARNING]" ), nWorldID );
    return false;
  }

  mID = nWorldID;
  mSpawnedVehicles.SetLocation( nWorldID );
  // furniture & other world stuff loading here
  Console->Print( "%s Loaded world %d", Console->ColorText( GREEN, BLACK, "[DEBUG]" ), nWorldID );
  return true;
}

const PDefWorldModel* PWorld::GetFurnitureItemModel( u32 nItemID )
{
  if ( mWorldDataTemplate )
  {
    const PFurnitureItemTemplate* tFurniture = mWorldDataTemplate->GetFurnitureItem( nItemID );
    if ( tFurniture )
      return tFurniture->GetDefWorldModel();
  }
  return NULL;
}

bool PWorld::CharUseChair( int CharLocalID, u32 nItemID )
{
  PChairsInUseMap::iterator it = mChairsInUseMap.find( nItemID );
  if ( it == mChairsInUseMap.end() ) // chair is free
  {
    if ( gDevDebug )
      Console->Print( "%s Localchar %d now using free chair %d.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), CharLocalID, nItemID );
    mChairsInUseMap.insert( std::make_pair( nItemID, CharLocalID ) );
    return true;
  }
  else // chair is already in use
  {
    if ( gDevDebug )
    {
      if ( it->second == CharLocalID )
      {
        Console->Print( "%s Localchar %d already using chair %d.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), CharLocalID, nItemID );
      }
      else
      {
        Console->Print( "%s Localchar %d can't sit on chair %d used by localchar %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), CharLocalID, nItemID, it->second );
      }
    }
    return ( it->second == CharLocalID );
  }
}

void PWorld::CharLeaveChair( int CharLocalID, u32 nItemID )
{
  PChairsInUseMap::iterator it = mChairsInUseMap.find( nItemID );
  if (( it != mChairsInUseMap.end() ) && ( it->second == CharLocalID ) ) // chair is in use by this char
  {
    if ( gDevDebug )
      Console->Print( "%s Localchar %d leaving chair %d.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), CharLocalID, nItemID );
    mChairsInUseMap.erase( it );
  }
}

PClient* PWorld::GetClientByCharLocalId( u32 rawObjectId ) const
{
  // Temp implementation
  return ClientManager->GetClientByCharLocalId( rawObjectId, mID );
}

bool PWorld::CheckVhcNeedZoning( PVhcCoordinates const* nPos ) const
{
  if ( ( nPos->GetX() <= mBottomZoneOutLimit ) || ( nPos->GetX() >= mTopZoneOutLimit ) || ( nPos->GetY() <= mBottomZoneOutLimit )   || ( nPos->GetY() >= mTopZoneOutLimit ) )
    return true;
  else
    return false;
}

u32 PWorld::GetVhcZoningDestination( PSpawnedVehicle const* nVhc, PVhcCoordinates* nPos ) const
{
  u32 destWorldId = 0;
  s16 vChange = 0;
  s16 hChange = 0;
  PVhcCoordinates const vhcPos = nVhc->GetPosition();
  u16 posX = vhcPos.GetX();
  u16 posY = vhcPos.GetY();

  if ( posX <= mBottomZoneOutLimit )
  {
    vChange = 1;
  }
  else if ( posX >= mTopZoneOutLimit )
  {
    vChange = -1;
  }

  if ( posY <= mBottomZoneOutLimit )
  {
    hChange = -1;
  }
  else if ( posY >= mTopZoneOutLimit )
  {
    hChange = 1;
  }

  if ( vChange || hChange )
  {
    //Console->Print( YELLOW, BLACK, "[DEBUG] Limit Reached V:%d H:%d mapId:%d", vChange,hChange,mID );
    u8 currH = 0;
    u8 currV = 0;
    s16 newH, newV;

    if ( Worlds->GetWorldmapFromWorldId( mID, currH, currV ) )
    {
      //Console->Print( YELLOW, BLACK, "[DEBUG] current map pos V:%d H:%d", currV,currH );
      newH = currH + hChange;
      if ( newH < 0 )
        newH = 0;
      else if ( newH >= PWorlds::mOutdoorWorldmapHSize )
        newH = PWorlds::mOutdoorWorldmapHSize;

      newV = currV + vChange;
      if ( newV < 0 )
        newV = 0;
      else if ( newV >= PWorlds::mOutdoorWorldmapHSize )
        newV = PWorlds::mOutdoorWorldmapHSize;

      //Console->Print( YELLOW, BLACK, "[DEBUG] new map pos V:%d H:%d", newV,newH );
      if (( newH != currH ) || ( newV != currV ) )
      {
        destWorldId = Worlds->GetWorldIdFromWorldmap( newH, newV );

        if ( destWorldId && nPos )
        {
          if ( vChange == 1 )
            posX = mTopZoneInLimit;
          else if ( vChange == -1 )
            posX = mBottomZoneInLimit;

          if ( hChange == -1 )
            posY = mTopZoneInLimit;
          else if ( hChange == 1 )
            posY = mBottomZoneInLimit;

          nPos->SetPosition( posY, vhcPos.GetZ(), posX, vhcPos.GetUD(), vhcPos.GetLR(), vhcPos.GetRoll(), vhcPos.GetAct(), vhcPos.GetUnknown(), vhcPos.GetFF() );
        }
      }
    }
  }

  return destWorldId;
}

/**** PWorlds ****/
u32 const PWorlds::mNcSubwayWorldId = 1000;
u32 const PWorlds::mAptBaseWorldId = 100000;
u32 const PWorlds::mOutdoorBaseWorldId = 2001;
u32 const PWorlds::mOutdoorWorldIdVIncrement = 20;
u8 const PWorlds::mOutdoorWorldmapHSize = 16;
u8 const PWorlds::mOutdoorWorldmapVSize = 11;
u32 const PWorlds::mOutdoorMaxWorldId = PWorlds::mOutdoorBaseWorldId + PWorlds::mOutdoorWorldIdVIncrement * ( PWorlds::mOutdoorWorldmapVSize - 1 ) + PWorlds::mOutdoorWorldmapHSize - 1;

PWorlds::PWorlds()
{
  mPreloadWorldsTemplates = false; // to be put as config option
  mPreloadStaticWorlds = false; // to be put as config option
}

PWorlds::~PWorlds()
{
  for ( PWorldsMap::iterator i = mStaticWorldsMap.begin(); i != mStaticWorldsMap.end(); i++ )
    if ( i->second )
    {
      delete i->second;
    }
  for ( PWorldsMap::iterator i = mOnDemandWorldsMap.begin(); i != mOnDemandWorldsMap.end(); i++ )
    if ( i->second )
    {
      delete i->second;
    }
  for ( PWorldDataTemplatesMap::iterator i = mWorldDataTemplatesMap.begin(); i != mWorldDataTemplatesMap.end(); i++ )
    if ( i->second )
    {
      delete i->second;
    }
}

bool PWorlds::LeaseWorldDataTemplate( const std::string& nBspName, const std::string& nFileName, const bool nPreloadPhase )
{
  PWorldDataTemplate* tWorldDataTemplate;

  PWorldDataTemplatesMap::iterator it = mWorldDataTemplatesMap.find( nFileName );
  if ( it == mWorldDataTemplatesMap.end() ) // template unkown yet
  {
    if ( nPreloadPhase ) // if in preload phase, we try to load it or make it known
    {
      tWorldDataTemplate = new PWorldDataTemplate;
      if ( tWorldDataTemplate->LoadDatFile( nBspName, nFileName, !mPreloadWorldsTemplates ) )
      {
        if ( mPreloadWorldsTemplates )
        {
          mWorldDataTemplatesMap.insert( std::make_pair( nFileName, tWorldDataTemplate ) );
          tWorldDataTemplate->IncreaseUseCount();
        }
        else
        {
          mWorldDataTemplatesMap.insert( std::make_pair( nFileName, ( PWorldDataTemplate* )NULL ) ); // NULL means file access OK but not preloaded yet
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
    if ( !it->second  &&  !nPreloadPhase ) // template known but not already loaded and not in preload ?
    {
      tWorldDataTemplate = new PWorldDataTemplate;
      if ( tWorldDataTemplate->LoadDatFile( nBspName, nFileName ) )
      {
        it->second = tWorldDataTemplate;
      }
      else
      {
        Console->Print( "%s Invalid world data template file %s", Console->ColorText( RED, BLACK, "[ERROR]" ), nFileName.c_str() );
        mWorldDataTemplatesMap.erase( it );
        delete tWorldDataTemplate;
        return false;
      }
    }

    if ( !nPreloadPhase )
      it->second->IncreaseUseCount();

    //return true;
  }

  return true;
}

void PWorlds::ReleaseWorldDataTemplate( const std::string& nFileName )
{
  PWorldDataTemplatesMap::iterator it = mWorldDataTemplatesMap.find( nFileName );
  if (( it != mWorldDataTemplatesMap.end() ) && it->second )
  {
    it->second->DecreaseUseCount();
  }
  else
    Console->Print( "%s PWorlds::ReleaseWorldDataTemplate : try to relese not loaded template %s", Console->ColorText( RED, BLACK, "[Warning]" ), nFileName.c_str() );
}

void PWorlds::UnloadWorldDataTemplate( const std::string& nFileName )
{
  PWorldDataTemplate* tWorldDataTemplate;

  PWorldDataTemplatesMap::iterator it = mWorldDataTemplatesMap.find( nFileName );
  if (( it != mWorldDataTemplatesMap.end() ) && it->second )
  {
    tWorldDataTemplate = it->second;
    if ( mPreloadWorldsTemplates || ( tWorldDataTemplate->GetUseCount() > 0 ) )
      Console->Print( "%s PWorlds::UnloadWorldDataTemplate : attempt to unload template %s when use count not null ou preload set", Console->ColorText( RED, BLACK, "[Warning]" ), nFileName.c_str() );
    else
    {
      it->second = ( PWorldDataTemplate* )NULL;
      delete tWorldDataTemplate;
    }
  }
  else
    Console->Print( "%s PWorlds::UnloadWorldDataTemplate : attempt to release not loaded template %s", Console->ColorText( RED, BLACK, "[Warning]" ), nFileName.c_str() );
}

PWorldDataTemplate* PWorlds::GetWorldDataTemplate( const std::string& nFileName )
{
  PWorldDataTemplatesMap::const_iterator it = mWorldDataTemplatesMap.find( nFileName );
  if ( it != mWorldDataTemplatesMap.end() )
    return it->second;
  else
    return NULL;
}

bool PWorlds::LoadWorlds() // once Load is done, only WorldDataTemplate registred in mWorldDataTemplatesMap
{
  //   will be considered as valid
  std::string tFileName;
  std::string tBspName;
  const PDefWorld* tDefWorld;
  bool tCheckOK;
  int ValidCount = 0, InvalidCount = 0, DblInvCount = 0;
  int DatTmplCount, BadDatTmplCount;
  std::set<std::string>
  InvalideFiles;

  // Appartment templates checking or preloading
  std::map<int, PDefAppartement*>::const_iterator itAptStart = GameDefs->Appartements()->ConstIteratorBegin();
  std::map<int, PDefAppartement*>::const_iterator itAptEnd = GameDefs->Appartements()->ConstIteratorEnd();
  for ( std::map<int, PDefAppartement*>::const_iterator i = itAptStart; i != itAptEnd; i++ )
  {
    tCheckOK = false;
    tBspName = i->second->GetWorldName();
    tFileName = std::string( "worlds/" ) + tBspName + ".dat";
    if ( !InvalideFiles.count( tFileName ) )
    {
      tCheckOK = LeaseWorldDataTemplate( tBspName, tFileName, true );
      if ( !tCheckOK )
      {
        InvalideFiles.insert( tFileName );
        if ( gDevDebug )
          Console->Print( RED, BLACK, "Template file %s invalid", tFileName.c_str() );
      }
    }
    if ( !tCheckOK ) // in case full path was given without omiting worlds/ or in another dir/archive ...
    {
      tFileName = tBspName + ".dat";
      if ( !InvalideFiles.count( tFileName ) )
      {
        tCheckOK = LeaseWorldDataTemplate( tBspName, tFileName, true );
        if ( !tCheckOK )
        {
          InvalideFiles.insert( tFileName );
          ++DblInvCount;
        }
      }
    }

    if ( tCheckOK )
    {
      ++ValidCount;
      if ( gDevDebug )
        Console->Print( GREEN, BLACK, "Template file %s for appartment %d (%s) loaded", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str() );
    }
    else
    {
      ++InvalidCount;
      if ( gDevDebug )
        Console->Print( RED, BLACK, "Template file %s for appartment %d (%s) not available or invalid", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str() );
    }
  }

  DatTmplCount = mWorldDataTemplatesMap.size();
  BadDatTmplCount = InvalideFiles.size();
  Console->Print( "%s %d valid appartement templates checked (%d dat files)", Console->ColorText( GREEN, BLACK, "[Success]" ), ValidCount, DatTmplCount );
  if ( InvalidCount )
    Console->Print( "%s %d invalid appartement templates rejected (%d dat files)", Console->ColorText( YELLOW, BLACK, "[Notice]" ), InvalidCount, BadDatTmplCount - DblInvCount );

  // Static worlds & static worlds templates checking or preloading
  ValidCount = InvalidCount = 0;
  const PDefWorldFile* tDefWorldFile;
  std::map<int, PDefWorldFile*>::const_iterator itFilStart = GameDefs->WorldFiles()->ConstIteratorBegin();
  std::map<int, PDefWorldFile*>::const_iterator itFilEnd = GameDefs->WorldFiles()->ConstIteratorEnd();
  for ( std::map<int, PDefWorldFile*>::const_iterator i = itFilStart; i != itFilEnd; i++ )
  {
    tDefWorldFile = i->second;

    tDefWorld = GameDefs->Worlds()->GetDef( tDefWorldFile->GetIndex() );
    if ( tDefWorld ) // we only care for worlds that are present in worldinfo.def too
    {
      tBspName = tDefWorldFile->GetName();
      if ( !( tDefWorld->GetDatFile().empty() ) )
        tFileName = tDefWorld->GetDatFile();
      else
        tFileName = i->second->GetBasicFileName() + ".dat";

      tCheckOK = false;
      if ( !InvalideFiles.count( tFileName ) )
      {
        tCheckOK = LeaseWorldDataTemplate( tBspName, tFileName, true );
        if ( !tCheckOK )
        {
          InvalideFiles.insert( tFileName );
          if ( gDevDebug )
            Console->Print( RED, BLACK, "Template file %s invalid", tFileName.c_str() );
        }
      }

      if ( tCheckOK )
      {
        ++ValidCount;
        if ( mPreloadStaticWorlds )
        {
          LeaseWorld( tDefWorldFile->GetIndex(), true ); // This will make the world ready and kept in mem (use count always >0 )
        }
        else
        {
          mStaticWorldsMap.insert( std::make_pair( tDefWorldFile->GetIndex(), ( PWorld* )NULL ) );
        }
        if ( gDevDebug )
          Console->Print( GREEN, BLACK, "Template file %s for world %d (%s) loaded", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str() );
      }
      else
      {
        ++InvalidCount;
        if ( gDevDebug )
          Console->Print( RED, BLACK, "Template file %s for world %d (%s) not available or invalid", tFileName.c_str(), i->second->GetIndex(), i->second->GetName().c_str() );
      }
    }
  }

  // Hardcoded neofrag worlds ... oO ... and neofrag4.dat can't be found :-/
  for ( int i = 1; i <= 16; i++ )
  {
    char worldName[19];
    int MatchID = i;
    if ( MatchID > 8 ) // to care for Neofrag 1 & 2
      MatchID -= 8;
    if ( MatchID > 6 )
      MatchID = 6; // holomatch 7 and 8 are same as 6
    snprintf( worldName, 19, "holomatch/neofrag%d", MatchID );

    tDefWorld = GameDefs->Worlds()->GetDef( 90000 + i );
    if ( tDefWorld ) // we only care for worlds that are present in worldinfo.def too
    {
      if ( !( tDefWorld->GetDatFile().empty() ) )
        tFileName = tDefWorld->GetDatFile();
      else
      {
        tFileName = "worlds/";
        tFileName += worldName;
        tFileName += ".dat";
      }

      tCheckOK = false;
      if ( !InvalideFiles.count( tFileName ) )
      {
        tCheckOK = LeaseWorldDataTemplate( worldName, tFileName, true );
        if ( !tCheckOK )
        {
          InvalideFiles.insert( tFileName );
          if ( gDevDebug )
            Console->Print( RED, BLACK, "Template file %s invalid", tFileName.c_str() );
        }
      }

      if ( tCheckOK )
      {
        ++ValidCount;
        if ( mPreloadStaticWorlds )
        {
          LeaseWorld( 90000 + i, true ); // This will make the world ready and kept in mem (use count always >0 )
        }
        else
        {
          mStaticWorldsMap.insert( std::make_pair( 90000 + i, ( PWorld* )NULL ) );
        }
        if ( gDevDebug )
          Console->Print( GREEN, BLACK, "Template file %s for world %d (%s) loaded", tFileName.c_str(), 90000 + i, worldName );
      }
      else
      {
        ++InvalidCount;
        if ( gDevDebug )
          Console->Print( RED, BLACK, "Template file %s for world %d (%s) not available or invalid", tFileName.c_str(), 90000 + i, worldName );
      }
    }
  }

  Console->Print( "%s %d valid world templates checked (%d dat files)", Console->ColorText( GREEN, BLACK, "[Success]" ), ValidCount, mWorldDataTemplatesMap.size() - DatTmplCount );
  if ( InvalidCount )
    Console->Print( "%s %d invalid world templates rejected (%d dat files)", Console->ColorText( YELLOW, BLACK, "[Notice]" ), InvalidCount, InvalideFiles.size() - BadDatTmplCount - DblInvCount );
  Console->Print( "%s %d static worlds prepared", Console->ColorText( GREEN, BLACK, "[Success]" ), mStaticWorldsMap.size() );

  // release memory if World templates preload activated, this cache that won't be used anymore
  // if (mPreloadWorldsTemplates) Filesystem->ClearCache();

  return true;
}

bool PWorlds::IsValidWorld( u32 nWorldID ) const
{
  if ( nWorldID > PWorlds::mAptBaseWorldId )
  {
    if ( mOnDemandWorldsMap.count( nWorldID ) ) // Check if already loaded
      return true;
    else //should better do a check using a PAppartements class object to get the world template
    {
      //int AptTmplID = Appartements->GetAptType(nWorldID - PWorlds::mAptBaseWorldId);
      int AptTmplID = Appartements->GetAptType( nWorldID );
      if ( !AptTmplID )
        return false;

      const PDefAppartement* nAppDef = GameDefs->Appartements()->GetDef( AptTmplID );
      if ( !nAppDef )
        return false;

      std::string tFileName = "worlds/" + nAppDef->GetWorldName() + ".dat";
      PWorldDataTemplatesMap::const_iterator it = mWorldDataTemplatesMap.find( tFileName );
      return ( it != mWorldDataTemplatesMap.end() );
    }
  }
  else
  {
    if ( gDevDebug )
      Console->Print( "%s Checking validity for world %d : %s", Console->ColorText( GREEN, BLACK, "[Debug]" ), nWorldID, mStaticWorldsMap.count( nWorldID ) ? "OK" : "BAD" );
    return ( mStaticWorldsMap.count( nWorldID ) );
  }
}

PWorld* PWorlds::LeaseWorld( u32 nWorldID, const bool nPreloadPhase )
{
  PWorldsMap::iterator it;

  if ( nWorldID > PWorlds::mAptBaseWorldId )
  {
    it = mOnDemandWorldsMap.find( nWorldID ); // Check if already loaded
    if (( it != mOnDemandWorldsMap.end() ) && it->second ) // Dynamic world shall not have a NULL it->second
    {
      // if loaded
      it->second->IncreaseUseCount();
      if ( gDevDebug )
        Console->Print( "%s Leased world %d", Console->ColorText( GREEN, BLACK, "[Debug]" ), nWorldID );
      return it->second;
    }
    else // not loaded yet or invalid
    {
      PWorld* nWorld = new PWorld;
      if ( ! nWorld->Load( nWorldID ) ) // Error when loading (shouldn't happen)
      {
        delete nWorld;
        Console->Print( "% Could not load world %d - World now set as invalid", Console->ColorText( RED, BLACK, "[Warning]" ), nWorldID );
        return NULL;
      }
      else
      {
        mOnDemandWorldsMap.insert( std::make_pair( nWorldID, nWorld ) );
        nWorld->IncreaseUseCount();
        if ( gDevDebug )
          Console->Print( "%s Leased world %d", Console->ColorText( GREEN, BLACK, "[Debug]" ), nWorldID );
        return nWorld;
      }
    }
  }
  else
  {
    it = mStaticWorldsMap.find( nWorldID ); // Check if already loaded
    if (( it == mStaticWorldsMap.end() ) && nPreloadPhase )
    {
      mStaticWorldsMap.insert( std::make_pair( nWorldID, ( PWorld* )NULL ) );
      it = mStaticWorldsMap.find( nWorldID );
    }
    if ( it != mStaticWorldsMap.end() )
    {
      // if valid
      if ( ! it->second ) // if not loaded yet
      {
        it->second = new PWorld;
        if ( ! it->second->Load( nWorldID ) ) // Error when loading (shouldn't happen)
        {
          delete it->second;
          Console->Print( "%s Could not load world %d - World now set as invalid", Console->ColorText( RED, BLACK, "[Warning]" ), nWorldID );
          mStaticWorldsMap.erase( it ); // remove from valid worlds map
          return NULL;
        }
      }
      it->second->IncreaseUseCount();
      if ( gDevDebug )
        Console->Print( "%s Leased world %d", Console->ColorText( GREEN, BLACK, "[Debug]" ), nWorldID );
      return it->second;
    }
    else // invalid worldID
    {
      return NULL;
    }
  }
}

PWorld* PWorlds::GetWorld( u32 nWorldID )
{
  PWorldsMap* tMap;
  PWorldsMap::iterator it;

  tMap = (( nWorldID > PWorlds::mAptBaseWorldId ) ? &mOnDemandWorldsMap : &mStaticWorldsMap );

  it = tMap->find( nWorldID );
  if (( it != tMap->end() ) && it->second )
  {
    return it->second;
  }
  else
  {
    Console->Print( "%s PWorlds::GetWorld : Trying to get world %d without lease !", Console->ColorText( RED, BLACK, "[Warning]" ), nWorldID );
    return NULL;
  }
}

void PWorlds::ReleaseWorld( u32 nWorldID ) // no dynamic unload is performed atm + don't forget spawned vhc !
{
  PWorld* tWorld = GetWorld( nWorldID );
  if ( tWorld )
  {
    if ( tWorld->GetUseCount() ) // this check is for dev time only
    {
      tWorld->DecreaseUseCount();
      if ( gDevDebug )
        Console->Print( "%s Released world %d", Console->ColorText( GREEN, BLACK, "[Debug]" ), nWorldID );
    }
    else
    {
      Console->Print( "%s PWorlds::ReleaseWorld : Trying to release world %d with use count 0 !", Console->ColorText( RED, BLACK, "[Warning]" ), nWorldID );
    }
  }
  else
  {
    Console->Print( "%s PWorlds::ReleaseWorld : Generated the invalid get world %d", Console->ColorText( RED, BLACK, "[Warning]" ), nWorldID );
  }
}

bool PWorlds::IsAppartment( u32 nWorldID )
{
  return (( nWorldID > PWorlds::mAptBaseWorldId ) && IsValidWorld( nWorldID ) );
}

void PWorlds::Update()
{}

void PWorlds::Shutdown()
{}

u32 PWorlds::GetWorldIdFromWorldmap( u8 mapH, u8 mapV ) const
{
  u32 loc = 0;
  if (( mapH < mOutdoorWorldmapHSize ) && ( mapV < mOutdoorWorldmapVSize ) )
  {
    loc = mOutdoorBaseWorldId + mOutdoorWorldIdVIncrement * mapV + mapH;
    if ( ! IsValidWorld( loc ) )
      loc = 0;
  }
  return loc;
}

bool PWorlds::GetWorldmapFromWorldId( u32 nWorldId, u8& mapH, u8& mapV ) const
{
  if (( nWorldId >= mOutdoorBaseWorldId ) && ( nWorldId <= mOutdoorMaxWorldId ) )
  {
    mapV = ( nWorldId - mOutdoorBaseWorldId ) / mOutdoorWorldIdVIncrement;
    mapH = ( nWorldId - mOutdoorBaseWorldId ) % mOutdoorWorldIdVIncrement;
    return true;
  }
  return false;
}

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
  worlds.h - world class and world map class

 MODIFIED: 06 Oct 2006 Hammag
 REASON: - creation

*/


#ifndef WORLDS_H
#define WORLDS_H

#include "worlddatatemplate.h"
#include "vehicle.h"

typedef std::map<u32, int> PChairsInUseMap;

class PWorld
{
    friend class PWorlds;

  public:
    static u16 const mZoneOutLimitOffset;
    static u16 const mBottomZoneOutLimit;
    static u16 const mBottomZoneInLimit;
    static u16 const mTopZoneOutLimit;
    static u16 const mTopZoneInLimit;

  private:
    u32 mID;
    bool mIsAppartment;
    int mUseCount;
    PWorldDataTemplate* mWorldDataTemplate;
    PChairsInUseMap mChairsInUseMap;
    PSpawnedVehicles mSpawnedVehicles;

    inline void IncreaseUseCount() { ++mUseCount; }
    inline int DecreaseUseCount() { return ( mUseCount ? --mUseCount : 0 ); }
    inline int GetUseCount() { return mUseCount; }
    bool Load( u32 nWorldID );

  public:
    PWorld();
    ~PWorld();

    inline std::string GetName() { return ( mWorldDataTemplate ? mWorldDataTemplate->GetName() : EmptyString ); }
    inline std::string GetBspName() { return ( mWorldDataTemplate ? mWorldDataTemplate->GetBspName() : EmptyString ); }
    inline bool IsAppartment() { return mIsAppartment; }
    inline const PFurnitureItemTemplate* GetFurnitureItemTemplate( u32 nItemID ) { return ( mWorldDataTemplate ? mWorldDataTemplate->GetFurnitureItem( nItemID ) : NULL ) ; }
    const PDefWorldModel* GetFurnitureItemModel( u32 nItemID );
    inline const PDoorTemplate* GetDoor( u32 nDoorID ) { return ( mWorldDataTemplate ? mWorldDataTemplate->GetDoor( nDoorID ) : NULL ); }
    inline bool getPositionItemPosition( u8 PosID, f32* pX, f32* pY, f32* pZ ) { return ( mWorldDataTemplate ? mWorldDataTemplate->getPositionItemPosition( PosID, pX, pY, pZ ) : false ); }

    bool CharUseChair( int CharLocalID, u32 nItemID );
    void CharLeaveChair( int CharLocalID, u32 nItemID );

    PClient* GetClientByCharLocalId( u32 rawObjectId ) const; // returns Client if object is a PC char, and 0 if not.

    inline PSpawnedVehicles* GetSpawnedVehicules() { return &mSpawnedVehicles; }
    bool CheckVhcNeedZoning( PVhcCoordinates const* nPos ) const;
    u32 GetVhcZoningDestination( PSpawnedVehicle const* nVhc, PVhcCoordinates* nPos = 0 ) const;
};


typedef std::map<u32, PWorld*> PWorldsMap;
typedef std::map<std::string, PWorldDataTemplate*> PWorldDataTemplatesMap;

class PWorlds
{
    friend class PWorld;

  public:
    static u32 const mNcSubwayWorldId;
    static u32 const mAptBaseWorldId;
    static u32 const mOutdoorBaseWorldId;
    static u32 const mOutdoorWorldIdVIncrement;
    static u8 const mOutdoorWorldmapHSize;
    static u8 const mOutdoorWorldmapVSize;
    static u32 const mOutdoorMaxWorldId;

  private:
    bool mPreloadWorldsTemplates;
    bool mPreloadStaticWorlds;

    PWorldsMap mStaticWorldsMap; // lists all valid static worlds, with second=NULL if not loaded
    PWorldsMap mOnDemandWorldsMap; // mostly appartments. Could be used for instance dungeons too, but instance dungeons are crap :p
    PWorldDataTemplatesMap mWorldDataTemplatesMap;

    bool LeaseWorldDataTemplate( const std::string& nBspName, const std::string& nFileName, const bool nPreloadPhase = false );
    void ReleaseWorldDataTemplate( const std::string& nFileName );
    void UnloadWorldDataTemplate( const std::string& nFileName );
    PWorldDataTemplate* GetWorldDataTemplate( const std::string& nFileName );
    PWorld* LeaseWorld( u32 nWorldID, const bool nPreloadPhase );

  public:
    PWorlds();
    ~PWorlds();

    bool LoadWorlds();
    bool IsValidWorld( u32 nWorldID ) const;
    inline PWorld* LeaseWorld( u32 nWorldID ) { return LeaseWorld( nWorldID, false ); }
    PWorld* GetWorld( u32 nWorldID );
    void ReleaseWorld( u32 nWorldID );
    bool IsAppartment( u32 nWorldID );
    inline bool IsPotentialAppartement( u32 nWorldID ) { return ( nWorldID > PWorlds::mAptBaseWorldId ); }

    void Update();
    void Shutdown();

    u32 GetWorldIdFromWorldmap( u8 mapH, u8 mapV ) const;  // H & V pos are 0-based
    bool GetWorldmapFromWorldId( u32 nWorldId, u8& mapH, u8& mapV ) const;
};

#endif

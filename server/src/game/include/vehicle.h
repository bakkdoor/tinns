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
 vehicle.h - Vehicle handling

 Authors:
 - Namikon

        MODIFIED: 08 Jan 2006 Namikon
        REASON: - initial release by Namikon
*/

#ifndef VEHICLE_H
#define VEHICLE_H

class PDefVhc;

class PVhcCoordinates
{
    friend class PSpawnedVehicle;

  private:
    u16 mY;     // Y-Position in world
    u16 mZ;     // Z-Position in world
    u16 mX;     // X-Position in world
    u8 mUD;     // Up - Mid - Down (d6 - 80 - 2a)
    u16 mLR;     //
    u16 mRoll;
    u8 mAct;    // Last user action state
    // mAct: bit field
    //0x00 = not moving
    //0x01 = Left
    //0x02 = Right
    //0x04 = Forward
    //0x08 = Back
    //0x20 = Pushing down
    //0x40 = Pulling up

  public:
    inline PVhcCoordinates() { mX = mY = mZ = mAct = 0; mUD = 128; mLR = 34683; mRoll = 32403;}

    void SetPosition( u16 nY, u16 nZ, u16 nX, u8 nUD, u16 nLR, u16 nRoll, u8 nAct = 0 );
    void SetInterpolate( const PVhcCoordinates& Pos1, const PVhcCoordinates& Pos2, f32 nCoef );
    inline u16 GetX() const { return mX; }
    inline u16 GetY() const { return mY; }
    inline u16 GetZ() const { return mZ; }
    inline u8 GetUD() const { return mUD; }
    inline u16 GetLR() const { return mLR; }
    inline u16 GetRoll() const { return mRoll; }
    inline u8 GetAct() const { return mAct; }
};

class PVehicleInformation
{
    friend class PSpawnedVehicle;
    friend class PVehicles;

  private:
    u32 mVehicleId;
    u32 mOwnerCharId;
    u32 mHealth; // or f32 ???
    u8 mVehicleType;
    u8 mStatus; //vhcStatus 0:parking, 1:in_service, 2:destroyed

  public:
    inline PVehicleInformation( u32 nVehicleId = 0, u32 nOwnerCharId = 0, u32 nHealth = 0, u8 nVehicleType = 0, u8 nStatus = 0 )
    {
      mVehicleId = nVehicleId;
      mOwnerCharId = nOwnerCharId;
      mHealth = nHealth;
      mVehicleType = nVehicleType;
      mStatus = nStatus;
    }

    inline u32 GetVehicleId() const { return mVehicleId; }
    inline u32 GetOwnerCharId() const { return mOwnerCharId; }
    inline u32 GetHealth() const { return mHealth; } // or f32 ???
    inline u8 GetVehicleType() const { return mVehicleType; }
    inline u8 GetStatus() const { return mStatus; }
    bool SetStatus( const u8 nStatus );

    bool Load( u32 nVehicleId );
    bool Save();
};

class PSpawnedVehicle
{
  private:
    u32 mLocalId;
    PVehicleInformation mInfo;
    u32 mLocation;
    PVhcCoordinates mCoords;
    const PDefVhc* mVhcDef;

    u32 mSeatUserId[8];

  public:
    PSpawnedVehicle( const u32 nLocalId, const PVehicleInformation* const nVhcInfo, const u32 nLocation, const PVhcCoordinates* const nVhcPos );

    inline u32 GetVehicleId() const { return mInfo.mVehicleId; }
    inline u32 GetLocalId() const { return mLocalId; }
    inline const PVhcCoordinates& GetPosition() const { return mCoords; }
    inline const PVehicleInformation& GetInformation() const { return mInfo; }
    inline bool SetStatus( const u8 nStatus ) { return mInfo.SetStatus( nStatus ); }

    void SetLocation( const u32 nLocation );
    inline u32 GetLocation() const { return mLocation; }
    void SetPosition( const PVhcCoordinates* const nVhcPos );

    inline bool Save() { return mInfo.Save(); }

    int GetNumSeats() const;
    inline u32 GetSeatUser( const u8 nSeatId ) const { return ( (nSeatId < 8) ? mSeatUserId[nSeatId] : 0); } 
    bool SetSeatUser( const u8 nSeatId, const u32 nCharId );
    bool UnsetSeatUser( const u8 nSeatId, const u32 nCharId );
    bool IsCharInside(const u32 nCharId) const;
    u8 GetFreeSeats() const;
    //SetHealth(const u32 nHealth);
    //u32 DoDamage(const u32 nHealthDec);
    //u32 DoRepair(const u32 nHealthInc);
};

typedef std::map<u32, PSpawnedVehicle*> PSpawnedVhcMap;
typedef std::queue<PVehicleInformation*> PVhcInfoList;
class PVehicles
{
  private:
    PSpawnedVhcMap mSpawnedVehicles;

    bool RegisterSpawnedVehicle( PSpawnedVehicle*  nSpawnedVehicle );
    bool UnregisterSpawnedVehicle( const u32 nVehicleId );

  public:
    PVehicles();
    ~PVehicles();

    //u32 CreateVehicle(const u32 nOwnerChar, const u8 mVehicleType);
    //bool RegisterVehicleOwner(const u32 nVehiculeId, const u32 nOwnerChar);
    //bool DestroyVehicle(const u32 nVehiculeId);
    bool IsValidVehicle( const u32 nVehicleId, const bool nCheckOwner = false, const u32 nOwnerId = 0 ) const;
    inline bool IsSpawned( const u32 nVehicleId ) const { return ( mSpawnedVehicles.find( nVehicleId ) != mSpawnedVehicles.end() ); }
    PSpawnedVehicle* GetSpawnedVehicle( const u32 nVehicleId ) const;
    bool GetVehicleInfo( const u32 nVehicleId, PVehicleInformation* nInfo ) const;
    PSpawnedVehicle* SpawnVehicle( const u32 nVehicleId, const u32 nLocation, const PVhcCoordinates* const nVhcPos ); // Refuses for subway zone atm
    bool UnspawnVehicle( const u32 nVehicleId );

    PVhcInfoList* GetCharVehicles( const u32 nCharId, const u16 nMaxCount = 0, const u16 nStartIndex = 0 );

};

typedef std::vector<PSpawnedVehicle*> PSpawnedVhcVector;
typedef std::queue<PSpawnedVehicle*> PSpawnedVhcList;

class PSpawnedVehicles
{
    friend class PWorld;

  public:
    static const u32 mVhcBaseLocalId = 0x03ff;
    static const u16 mMaxLocalVhc = 127;

    inline static bool IsPotentialSpawnedVehicle( const u32 nLocalId )
    { return (( nLocalId <= mVhcBaseLocalId ) && (( mVhcBaseLocalId - nLocalId ) < mMaxLocalVhc ) ); }

  private:
    PSpawnedVhcVector mSpawnedVehicles;
    u16 mNextFreeHint;
    u32 mLocation;

    inline void SetLocation( const u32 nLocation ) { mLocation = nLocation; }

  public:
    PSpawnedVehicles();
    ~PSpawnedVehicles();

    inline bool IsSpawned( const u32 nLocalId ) const { return (( nLocalId <= mVhcBaseLocalId ) && (( mVhcBaseLocalId - nLocalId ) < mSpawnedVehicles.size() ) && mSpawnedVehicles[mVhcBaseLocalId-nLocalId] ); }
    PSpawnedVehicle* SpawnVehicle( const PVehicleInformation* const nVhcInfo, const PVhcCoordinates* const nVhcPos );
    PSpawnedVehicle* GetVehicle( const u32 nLocalId );
    bool UnspawnVehicle( const u32 nLocalId );

    PSpawnedVhcList* GetSpawnedVehicles() const;
};

#endif

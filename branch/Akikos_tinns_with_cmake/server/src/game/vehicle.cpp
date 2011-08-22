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
	vehicle.cpp - Vehicle handling

	Authors:
	- Namikon
	- Akiko

	MODIFIED: 08 Jan 2006 Namikon
	REASON: - initial release by Namikon
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/vehicle.h"
#include "include/worlds.h"


// PVhcCoordinates
void PVhcCoordinates::SetInterpolate( const PVhcCoordinates& Pos1, const PVhcCoordinates& Pos2, f32 nCoef )
{
  if (( nCoef < 0 ) || ( nCoef > 1 ) )
  {
    Console->Print( RED, BLACK, "[Error] PVhcCoordinates::Interpolate : Invalid nCoef value: %f", nCoef );
    nCoef = 0;
  }
  f32 rCoef = 1 - nCoef;

  mY = ( u16 )( rCoef * Pos1.mY + nCoef * Pos2.mY );
  mZ = ( u16 )( rCoef * Pos1.mZ + nCoef * Pos2.mZ );
  mX = ( u16 )( rCoef * Pos1.mX + nCoef * Pos2.mX );
  mUD = ( u8 )( rCoef * Pos1.mUD + nCoef * Pos2.mUD );
  mLR = ( u16 )( rCoef * Pos1.mLR + nCoef * Pos2.mLR );
  mRoll = ( u16 )( rCoef * Pos1.mRoll + nCoef * Pos2.mRoll );
}

void PVhcCoordinates::SetPosition( u16 nY, u16 nZ, u16 nX, u8 nUD, u16 nLR, u16 nRoll, u8 nAct, u16 nUnknown, u8 nFF )
{
  mY = nY;
  mZ = nZ;
  mX = nX;
  mUD = nUD;
  mLR = nLR;
  mRoll = nRoll;
  mAct = nAct;
  mUnknown = nUnknown;
  mFF = nFF;
}

// PVehicleInformation
//Tmp
const u8 VhcTypes[] = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                         11, 12, 13, 50, 60, 62, 64, 65, 70
                      }; // (adv.) assault gliders discarded because not ok
#define VHC_DISABLE_NOVULARI
#ifndef VHC_DISABLE_NOVULARI
const u8 VhcTypesNum = 19;
#else
const u8 VhcTypesNum = 18;
#endif

bool PVehicleInformation::Load( u32 nVehicleId )
{
  u8 i;
  u8 nVId = nVehicleId % 100; //Tmp
  for ( i = 0; ( i < VhcTypesNum ) && ( VhcTypes[i] < nVId ); i++ ) ; //Tmp
  if (( i < VhcTypesNum ) && ( VhcTypes[i] == nVId ) ) //Tmp
  {
    mVehicleId = nVehicleId;
    mOwnerCharId = nVehicleId / 100; // tmp
    mHealth = 200; // Tmp
    mVehicleType = nVId; // tmp
    mStatus = 0;
    return true;
  }
  else
    return false;
}

bool PVehicleInformation::Save()
{
  // Shall we Destroy() when mStatus == 2, or keep destroyed vhc in DB ?
  return true;
}

bool PVehicleInformation::Destroy()
{
  if ( mStatus == 2 )
  {
    // Delete from DB
    mVehicleId = 0;
    return true;
  }
  else
    return false;
}

bool PVehicleInformation::SetStatus( u8 nStatus )
{
  if (( mStatus != 2 ) && ( nStatus <= 2 ) )
  {
    mStatus = nStatus;
    return true;
  }
  else
    return false;
}

// PSpawnedVehicule
const u8 PSpawnedVehicle::mSeatsFlags[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

PSpawnedVehicle::PSpawnedVehicle( u32 nLocalId, PVehicleInformation const* nVhcInfo, u32 nLocation, PVhcCoordinates const* nVhcPos )
{
  mLocalId = nLocalId;
  mInfo = *nVhcInfo;
  mLocation = nLocation;
  mCoords = *nVhcPos;
  for ( int i = 0; i < 8; ++i )
    mSeatUserId[i] = 0;

  mVhcDef = GameDefs->Vhcs()->GetDef( mInfo.mVehicleType );
  if ( ! mVhcDef )
  {
    Console->Print( RED, BLACK, "[ERROR] Invalid vhc type %d for vhc id %d (local %d) owner char id %d", mInfo.mVehicleType, mInfo.mVehicleId, mLocalId, mInfo.mOwnerCharId );
    Console->Print( RED, BLACK, "[NOTICE] Setting vhc type to %d for vhc id %d (local %d) owner char id %d", VhcTypes[0], mInfo.mVehicleId, mLocalId, mInfo.mOwnerCharId );
    mInfo.mVehicleType = VhcTypes[0];
    mVhcDef = GameDefs->Vhcs()->GetDef( mInfo.mVehicleType );
  }

  mNbFreeSeats = GetNumSeats();
  if ( mNbFreeSeats > 8 )
  {
    Console->Print( RED, BLACK, "[ERROR] Vhc type %d has more than 8 seats (%d)", mInfo.mVehicleType, mNbFreeSeats );
    mNbFreeSeats = 8;
  }
  for ( int i = 0; i < mNbFreeSeats; ++i )
    mFreeSeatsFlags |= mSeatsFlags[i];

  //Temp
  for ( int i = 0; i < 4; ++i )
  {
    minmax[i][0] = 0xffff;
    minmax[i][1] = 0;
  }
}

void PSpawnedVehicle::SetLocation( u32 nLocation )
{
  mLocation = nLocation;
}

void PSpawnedVehicle::SetPosition( PVhcCoordinates const* nVhcPos )
{
  mCoords = *nVhcPos;
  // Temp
  if(gDevDebug)
  {
    if ( mCoords.mUD < minmax[0][0] ) minmax[0][0] = mCoords.mUD;
    if ( mCoords.mUD > minmax[0][1] ) minmax[0][1] = mCoords.mUD;
    if ( mCoords.mLR < minmax[1][0] ) minmax[1][0] = mCoords.mLR;
    if ( mCoords.mLR > minmax[1][1] ) minmax[1][1] = mCoords.mLR;
    if ( mCoords.mRoll < minmax[2][0] ) minmax[2][0] = mCoords.mRoll;
    if ( mCoords.mRoll > minmax[2][1] ) minmax[2][1] = mCoords.mRoll;
    Console->Print( "%s Min/Max: UD:%d/%d(%d) LR:%d/%d(%d) Roll:%d/%d(%d)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), minmax[0][0], minmax[0][1], mCoords.mUD, minmax[1][0], minmax[1][1], mCoords.mLR, minmax[2][0], minmax[2][1], mCoords.mRoll );
  }
}

int PSpawnedVehicle::GetNumSeats() const
{
  return mVhcDef->GetNumSeats();
}

bool PSpawnedVehicle::SetSeatUser( u8 nSeatId, u32 nCharId )
{
  if ( nSeatId < mVhcDef->GetNumSeats() )
  {
    if ( ! mSeatUserId[nSeatId] )
    {
      mSeatUserId[nSeatId] = nCharId;
      mFreeSeatsFlags &= ~mSeatsFlags[nSeatId];
      --mNbFreeSeats;
      return true;
    }
  }
  return false;
}

bool PSpawnedVehicle::UnsetSeatUser( u8 nSeatId, u32 nCharId )
{
  if ( nSeatId < mVhcDef->GetNumSeats() )
  {
    if ( mSeatUserId[nSeatId] == nCharId )
    {
      mSeatUserId[nSeatId] = 0;
      mFreeSeatsFlags |= mSeatsFlags[nSeatId];
      ++mNbFreeSeats;
      return true;
    }
  }
  return false;
}

bool PSpawnedVehicle::IsCharInside( u32 nCharId ) const
{
  for ( int i = 0; i < mVhcDef->GetNumSeats(); ++i )
  {
    if ( mSeatUserId[i] == nCharId )
      return true;
  }
  return false;
}

u8 PSpawnedVehicle::GetFirstFreeSeat() const
{
  for ( int i = 0; i < mVhcDef->GetNumSeats(); ++i )
  {
    if ( ! mSeatUserId[i] )
      return i;
  }

  return 255;
}
/*u8 PSpawnedVehicle::GetFreeSeats() const
{
  u8 bitField = 0;

  for(int i = mVhcDef->GetNumSeats() - 1; i >= 0 ; --i)
  {
    bitField <<= 1;
    if ( ! mSeatUserId[i] )
      bitField |= 1;
  }

  return bitField;
}*/

// PVehicles
PVehicles::PVehicles()
{

}

PVehicles::~PVehicles()
{

}

bool PVehicles::RegisterSpawnedVehicle( PSpawnedVehicle* nSpawnedVehicle )
{
  std::pair<PSpawnedVhcMap::iterator, bool> Result = mSpawnedVehicles.insert( std::make_pair( nSpawnedVehicle->GetVehicleId(), nSpawnedVehicle ) );
  return Result.second;
}

bool PVehicles::UnregisterSpawnedVehicle( u32 nVehicleId )
{
  PSpawnedVhcMap::iterator it = mSpawnedVehicles.find( nVehicleId );
  if ( it != mSpawnedVehicles.end() )
  {
    mSpawnedVehicles.erase( it );
    return true;
  }
  else
  {
    return false;
  }
}

//u32 PVehicles::CreateVehicle(u32 nOwnerChar, u8 mVehicleType) {}
//bool PVehicles::RegisterVehicleOwner(u32 nVehicleId, u32 nOwnerChar) {}
//bool PVehicles::DestroyVehicle(u32 nVehicleId) {}

bool PVehicles::IsValidVehicle( u32 nVehicleId, bool nCheckOwner, u32 nOwnerId ) const
{
  // Look in DB
  // tmp
  u32 tVehicleId = nVehicleId; tVehicleId = tVehicleId;
  bool tCheckOwner = nCheckOwner; tCheckOwner = tCheckOwner;
  u32 tOwnerId = nOwnerId; tOwnerId = tOwnerId;
  return true; // tmp
}

PSpawnedVehicle* PVehicles::GetSpawnedVehicle( u32 nVehicleId ) const
{
  PSpawnedVhcMap::const_iterator it = mSpawnedVehicles.find( nVehicleId );
  if ( it != mSpawnedVehicles.end() )
  {
    return ( it->second );
  }
  else
  {
    return NULL;
  }
}

bool PVehicles::GetVehicleInfo( u32 nVehicleId, PVehicleInformation* nInfo ) const
{
  PSpawnedVehicle* tVhc = GetSpawnedVehicle( nVehicleId );
  if ( tVhc )
  {
    *nInfo = tVhc->GetInformation();
    return true;
  }
  else
  {
    return nInfo->Load( nVehicleId );
  }
}

PVhcInfoList* PVehicles::GetCharVehicles( u32 nCharId, u16 nMaxCount, u16 nStartIndex )
{
  PVhcInfoList* Entries = new PVhcInfoList();
  PVehicleInformation* InfoEntry;
  // Tmp implementation
  u16 LimitIndex = nStartIndex + nMaxCount;
  if ( !nMaxCount || ( VhcTypesNum < LimitIndex ) )
  {
    LimitIndex = VhcTypesNum;
  }

  for ( u16 i = nStartIndex; ( i < LimitIndex ) ; ++i )
  {
    InfoEntry = new PVehicleInformation();
    if ( GetVehicleInfo( nCharId * 100 + VhcTypes[i], InfoEntry ) )
    {
      Entries->push( InfoEntry );
    }
    else
    {
      delete InfoEntry;
    }
  }

  return Entries;
}

PSpawnedVehicle* PVehicles::SpawnVehicle( u32 nVehicleId, u32 nLocation, PVhcCoordinates const* nVhcPos )
{
  PSpawnedVehicle* newVhc = NULL;
  PWorld* cWorld;
  PVehicleInformation nVhcInfo;

  if (( nLocation != PWorlds::mNcSubwayWorldId ) && IsValidVehicle( nVehicleId ) && !IsSpawned( nVehicleId ) )
  {
    cWorld = Worlds->LeaseWorld( nLocation );

    if ( cWorld && GetVehicleInfo( nVehicleId, &nVhcInfo ) )
    {
      newVhc = cWorld->GetSpawnedVehicules()->SpawnVehicle( &nVhcInfo, nVhcPos );
      if ( newVhc )
      {
        if ( !RegisterSpawnedVehicle( newVhc ) )
        {
          Console->Print( RED, BLACK, "[Error] PVehicles::SpawnVehicle : Could not register spawned vhc" );
        }
        if( gDevDebug )
          Console->Print( "%d Spawned vhc %d (local 0x%04x) type %d (requested: %d)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), newVhc->GetVehicleId(), newVhc->GetLocalId(), newVhc->GetInformation().GetVehicleType(), nVhcInfo.GetVehicleType() );
      }
      else
        Console->Print( RED, BLACK, "[Error] PVehicles::SpawnVehicle : Could not create vhc" );
    }

    Worlds->ReleaseWorld( nLocation );
  }

  return newVhc;
}

bool PVehicles::UnspawnVehicle( u32 nVehicleId )
{
  PWorld* cWorld;
  bool Result = false;

  PSpawnedVhcMap::iterator it = mSpawnedVehicles.find( nVehicleId );
  if ( it != mSpawnedVehicles.end() )
  {
    cWorld = Worlds->LeaseWorld( it->second->GetLocation() );

    if ( cWorld )
    {
      if ( !it->second->Save() )
      {
        Console->Print( RED, BLACK, "[Error] PVehicles::UnspawnVehicle : Could not save vhc %d", nVehicleId );
      }

      Result = cWorld->GetSpawnedVehicules()->UnspawnVehicle( it->second->GetLocalId() );
      if ( Result )
      {
        if ( !UnregisterSpawnedVehicle( nVehicleId ) )
        {
          Console->Print( RED, BLACK, "[Error] PVehicles::UnspawnVehicle : Could not unregister vhc %d", nVehicleId );
        }
      }
    }

    Worlds->ReleaseWorld( it->second->GetLocation() );
  }

  return Result;
}

//PSpawnedVehicles
PSpawnedVehicles::PSpawnedVehicles()
{
  mNextFreeHint = 0;
  mLocation = 0;
}

PSpawnedVehicles::~PSpawnedVehicles()
{

}

PSpawnedVehicle* PSpawnedVehicles::SpawnVehicle( PVehicleInformation const* nVhcInfo, PVhcCoordinates const* nVhcPos )
{
  PSpawnedVehicle* newVhc = NULL;
  u32 nSize;

  if ( nVhcInfo->GetStatus() == 0 ) // only if in garage
  {
    nSize = mSpawnedVehicles.size();
    while (( mNextFreeHint < nSize ) && mSpawnedVehicles[mNextFreeHint] )
    {
      ++mNextFreeHint;
    }
    if ( mNextFreeHint > nSize )
    {
      mNextFreeHint = nSize;
    }
    if ( mNextFreeHint == nSize )
    {
      mSpawnedVehicles.push_back( static_cast<PSpawnedVehicle *>(NULL) );
    }

    if ( mNextFreeHint < mMaxLocalVhc )
    {
      newVhc = new PSpawnedVehicle( mVhcBaseLocalId - mNextFreeHint, nVhcInfo, mLocation, nVhcPos );
      mSpawnedVehicles[mNextFreeHint++] = newVhc;
      newVhc->SetStatus( 1 );
    }
  }

  return newVhc;
}

PSpawnedVehicle* PSpawnedVehicles::GetVehicle( u32 nLocalId )
{
  if (( nLocalId <= mVhcBaseLocalId ) && ( nLocalId > ( mVhcBaseLocalId - mSpawnedVehicles.size() ) ) )
  {
    return mSpawnedVehicles[mVhcBaseLocalId - nLocalId];
  }
  else
  {
    return NULL;
  }
}

PSpawnedVehicle* PSpawnedVehicles::GetVehicleByGlobalId( u32 nVehicleId ) const
{
  for ( PSpawnedVhcVector::const_iterator it = mSpawnedVehicles.begin(); it != mSpawnedVehicles.end(); it++ )
  {
    if (( *it )->GetVehicleId() == nVehicleId )
    {
      return ( *it );
    }
  }

  return NULL;
}

bool PSpawnedVehicles::UnspawnVehicle( u32 nLocalId )
{
  u16 Index;
  PSpawnedVehicle* tVhc;

  if (( nLocalId <= mVhcBaseLocalId ) && ( nLocalId > ( mVhcBaseLocalId - mSpawnedVehicles.size() ) ) )
  {
    Index = mVhcBaseLocalId - nLocalId;
    tVhc = mSpawnedVehicles[Index];
    if ( tVhc )
    {
      if ( tVhc->GetInformation().GetStatus() != 2 )
      {
        tVhc->SetStatus( 0 );
      }
      delete tVhc;
      mSpawnedVehicles[Index] = NULL;
      if ( mNextFreeHint > Index )
      {
        mNextFreeHint = Index;
      }
      return true;
    }
  }

  return false;
}

PSpawnedVhcList* PSpawnedVehicles::GetSpawnedVehicles() const
{
  PSpawnedVhcList* Entries = new PSpawnedVhcList();

  for ( PSpawnedVhcVector::const_iterator i = mSpawnedVehicles.begin(); i !=  mSpawnedVehicles.end(); ++i )
  {
    if ( *i )
    {
      Entries->push( *i );
    }
  }

  return Entries;
}

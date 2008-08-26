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
  subway.h - subway class

	MODIFIED: 9 Nov 2007 Hammag
	REASON: - creation
	
*/

/*
(Real time)
Full cycle = 328860 ms
Network delay = TBD

Offset =MOD(M86-122778+(M85*29896);328860)

*/

#include "main.h"
#include "subway.h"
#include "chars.h" // for PCharCoordinates only

// Determines relative position of cabs, but how ???
const u16 PSubway::mSubwayInitData [] = {0x4396, 0x4387, 0x4370, 0x4352, 0x4334, 0x4316, 0x42f0, 0x42b4, 0x4270, 0x41f0, 0x0000};
const u32 PSubway::mCabLoopTime = 328860;
const u32 PSubway::mCab0TimeOffset = 122778;
const s32 PSubway::mTimingAdjust = 0;
const u32 PSubway::mCabIntervalTime = 29896;
const u32 PSubway::mOpenDoorOffset [] = { 0, 42500, 83000, 146800, 170700, 226200, 262800, 303200 };
const u32 PSubway::mOpenDoorDuration [] = { 8500, 6900, 12000, 7500, 8800, 9000, 8500, 9300 };
const char* PSubway::mSubwayStationName[] = { "Pla => Oz", "Vr => Oz", "Pp => Oz", "Oz end", "Oz => Pla", "Pp => Pla", "Vr => Pla", "Pla end" };
PCharCoordinates PSubway::mCabExitPositions [2][mStationsNumber];


PSubway::PSubway()
{
  for(u8 i=0; i<mCabsNumber; i++)
  {
    mSubways[i].mVhcId = mCabsBaseId+i;
    mSubways[i].mPosition = mSubwayInitData[i];
    mSubways[i].mDoorOpened = 0;
    for(u8 j=0; j<4; j++)
    {
      mSubways[i].mSeatUsersId[j] = 0;
    }
  }
  
  mCabExitPositions[0][0].SetPosition(32000-5205, 32000-608, 32000-4766, 0x80, 0);
  mCabExitPositions[1][0].SetPosition(32000-5764, 32000-608, 32000-4766, 0x80, 0);

  mCabExitPositions[0][1].SetPosition(32000+7290, 32000-600, 32000-500, 0x80, 45);
  mCabExitPositions[1][1].SetPosition(32000+7290, 32000-600, 32000-0, 0x80, 45); 

  mCabExitPositions[0][2].SetPosition(32000-2300, 32000-600, 32000+7985, 0x80, 0);
  mCabExitPositions[1][2].SetPosition(32000-2850, 32000-600, 32000+7985, 0x80, 0); 

  mCabExitPositions[0][3].SetPosition(32000-1700, 32000-596, 32000+1840, 0x80, 90);
  mCabExitPositions[1][3].SetPosition(32000-1250, 32000-596, 32000+1840, 0x80, 90); 

  mCabExitPositions[0][4].SetPosition(32000-1700, 32000-596, 32000+1570, 0x80, 0);
  mCabExitPositions[1][4].SetPosition(32000-1250, 32000-596, 32000+1570, 0x80, 0);    
  
  mCabExitPositions[0][5].SetPosition(32000-2300, 32000-600, 32000+8625, 0x80, 90);
  mCabExitPositions[1][5].SetPosition(32000-2850, 32000-600, 32000+8625, 0x80, 90);   
 
  mCabExitPositions[0][6].SetPosition(32000+7815, 32000-600, 32000-500, 0x80, 135);
  mCabExitPositions[1][6].SetPosition(32000+7815, 32000-600, 32000-0, 0x80, 135); 
 
  mCabExitPositions[0][7].SetPosition(32000-5205, 32000-608, 32000-4092, 0x80, 0);
  mCabExitPositions[1][7].SetPosition(32000-5764, 32000-608, 32000-4092, 0x80, 0); 
}

u32 PSubway::GetTimeOffset(u32 nVhcId, u32 nTime)
{
  u8 tIndex;
  
  if(!GetInfoIndex(nVhcId, &tIndex))
  {
    Console->Print(RED, BLACK, "[Error] PSubway::GetTimeOffset : invalid cab VhcId %d", nVhcId);
    return 0;  
  }

  return ((nTime + mCabLoopTime - mCab0TimeOffset + mTimingAdjust + (tIndex * mCabIntervalTime)) % mCabLoopTime);
}

u8 PSubway::GetStation(u32 nVhcId, u32 nTime, u32* TimeOffset)
{  
  s8 i;
  
  u32 tTimeOffset = GetTimeOffset(nVhcId, nTime);
  if(TimeOffset)
  {
    *TimeOffset = tTimeOffset;
  }
   
  for(i = mStationsNumber-1; (i >= 0) && (tTimeOffset < mOpenDoorOffset[i]); --i);

  return i;
}

bool PSubway::IsDoorOpen(u32 nVhcId, u32 nTime)
{
  u32 TimeOffset;
  u8 tStation;
  
  if(!GetInfoIndex(nVhcId))
  {
    Console->Print(RED, BLACK, "[Error] PSubway::IsDoorOpen : invalid cab VhcId %d", nVhcId);
    return false;  
  }
  else
  {
    tStation = GetStation(nVhcId, nTime, &TimeOffset);
    return ((TimeOffset-mOpenDoorOffset[tStation]) <= mOpenDoorDuration[tStation]);
  }
}

std::string* PSubway::GetStationName(u8 nStationId)
{
  if(nStationId < mStationsNumber)
  {
    return new std::string(mSubwayStationName[nStationId]);
  }
  else
  {
    Console->Print(RED, BLACK, "[Error] PSubway::GetStationName : invalid subway station %d", nStationId);
    return new std::string("Error");
  }
}

bool PSubway::GetStationExitPosition(PCharCoordinates* nPosition, u8 nStationId, f32 nCoef)
{
  if(nStationId < mStationsNumber)
  {
    nPosition->SetInterpolate(mCabExitPositions[0][nStationId], mCabExitPositions[1][nStationId], nCoef);
    return true;
  }
  else
  {
    Console->Print(RED, BLACK, "[Error] PSubway::SetStationExitPosition : invalid subway station %d", nStationId);
    return false;
  } 
}

bool PSubway::GetInfoIndex(u32 nVhcId, u8 *Index)
{
  s32 tIndex = nVhcId - mCabsBaseId;
  if ((tIndex >= 0) && (tIndex < mCabsNumber))
  {
    if(Index)
    {
      *Index = (u8)tIndex;
    }
    return true;
  }
  else
    return false;
}

bool PSubway::UpdateInfo(u32 nVhcId, u16 nPosition, u8 nDoorOpened)
{
  u8 tIndex;
  if(GetInfoIndex(nVhcId, &tIndex))
  {
    mSubways[tIndex].mPosition = nPosition;
//if(mSubways[tIndex].mDoorOpened != nDoorOpened)
//  Console->Print("[DEBUG] Subway cab %08x : door now %s ", nVhcId, nDoorOpened ? "opened" : "closed" );
  
    mSubways[tIndex].mDoorOpened = nDoorOpened;

    return true;
  }
  else
    return false;
}

u16 PSubway::GetPosition(u32 nVhcId)
{
  u8 tIndex;
  if(GetInfoIndex(nVhcId, &tIndex))
  {
    return mSubways[tIndex].mPosition;
  }
  else
    return 0;
}

u8 PSubway::GetFreeSeat(u32 nVhcId)
{
  u8 tIndex;
  u8 tSeatFound = 0;
  if(GetInfoIndex(nVhcId, &tIndex))
  {
    for(u8 j=0; j<4; j++)
    {
      if(! mSubways[tIndex].mSeatUsersId[j])
      {
        tSeatFound = j+1;
        break;
      }
    }
    return tSeatFound;
  }
  else
    return 0;
}

bool PSubway::SetSeatUser(u32 nVhcId, u8 nSeat, u32 nCharId)
{
  u8 tIndex;

  if(GetInfoIndex(nVhcId, &tIndex) && (nSeat >= 1) && (nSeat <= 4))
  {
    if(! mSubways[tIndex].mSeatUsersId[nSeat-1])
    {
      mSubways[tIndex].mSeatUsersId[nSeat-1] = nCharId;
//Console->Print("[DEBUG] Char %d using seat %d of subway cab %08x, ", nCharId, nSeat, nVhcId);
      return true;
    }
  }

  return false;
}

bool PSubway::UnsetSeatUser(u32 nVhcId, u8 nSeat, u32 nCharId)
{
  u8 tIndex;

  if(GetInfoIndex(nVhcId, &tIndex) && (nSeat >= 1) && (nSeat <= 4))
  {
    if(mSubways[tIndex].mSeatUsersId[nSeat-1] == nCharId)
    {
      mSubways[tIndex].mSeatUsersId[nSeat-1] = 0;
//Console->Print("[DEBUG] Char %d leaving seat %d of subway cab %08x, ", nCharId, nSeat, nVhcId);
      return true;
    }
  }

  return false;
}

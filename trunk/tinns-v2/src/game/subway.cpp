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

#include "main.h"

#include "subway.h"

const u16 SubwayInitPos [] = {0x4396, 0x4387, 0x4370, 0x4352, 0x4334, 0x4316, 0x42f0, 0x42b4, 0x4270, 0x41f0, 0x0000};
  
PSubway::PSubway()
{
  for(u8 i=0; i<SUBWAY_VHC_NB; i++)
  {
    mSubways[i].mVhcId = SUBWAY_VHC_BASE_ID+i;
    mSubways[i].mPosition = SubwayInitPos[i];
    mSubways[i].mDoorOpened = 0;
    for(u8 j=0; j<4; j++)
    {
      mSubways[i].mSeatUsersId[j] = 0;
    }
  }
}

bool PSubway::GetInfoIndex(u16 nVhcId, u8 *Index)
{
  int tIndex = nVhcId - SUBWAY_VHC_BASE_ID;
  if ((tIndex >= 0) && (tIndex < SUBWAY_VHC_NB))
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

bool PSubway::UpdateInfo(u16 nVhcId, u16 nPosition, u8 nDoorOpened)
{
  u8 tIndex;
  if(GetInfoIndex(nVhcId, &tIndex))
  {
    mSubways[tIndex].mPosition = nPosition;
//if(mSubways[tIndex].mDoorOpened != nDoorOpened)
//  Console->Print("[DEBUG] Subway cab %04x : door now %s ", nVhcId, nDoorOpened ? "opened" : "closed" );
  
    mSubways[tIndex].mDoorOpened = nDoorOpened;

    return true;
  }
  else
    return false;
}

u16 PSubway::GetPosition(u16 nVhcId)
{
  u8 tIndex;
  if(GetInfoIndex(nVhcId, &tIndex))
  {
    return mSubways[tIndex].mPosition;
  }
  else
    return 0;
}

u8 PSubway::GetDoorOpened(u16 nVhcId)
{
  u8 tIndex;
  if(GetInfoIndex(nVhcId, &tIndex))
  {
    return mSubways[tIndex].mDoorOpened;
  }
  else
    return 0;
}

u8 PSubway::GetFreeSeat(u16 nVhcId)
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

bool PSubway::SetSeatUser(u16 nVhcId, u8 nSeat, u32 nCharId)
{
  u8 tIndex;

  if(GetInfoIndex(nVhcId, &tIndex) && (nSeat >= 1) && (nSeat <= 4))
  {
    if(! mSubways[tIndex].mSeatUsersId[nSeat-1])
    {
      mSubways[tIndex].mSeatUsersId[nSeat-1] = nCharId;
Console->Print("[DEBUG] Char %d using seat %d of subway cab %04x, ", nCharId, nSeat, nVhcId);
      return true;
    }
  }

  return false;
}

bool PSubway::UnsetSeatUser(u16 nVhcId, u8 nSeat, u32 nCharId)
{
  u8 tIndex;

  if(GetInfoIndex(nVhcId, &tIndex) && (nSeat >= 1) && (nSeat <= 4))
  {
    if(mSubways[tIndex].mSeatUsersId[nSeat-1] == nCharId)
    {
      mSubways[tIndex].mSeatUsersId[nSeat-1] = 0;
Console->Print("[DEBUG] Char %d leaving seat %d of subway cab %04x, ", nCharId, nSeat, nVhcId);
      return true;
    }
  }

  return false;
}

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

#ifndef SUBWAY_H
#define SUBWAY_H

class PSubway
{
  friend class PMsgBuilder;

  struct PSubwayInfo
  {
    u16 mVhcId;
    u16 mPosition;
    u8 mDoorOpened;
    u32 mSeatUsersId[4];
  };

  public:
    static const u16 mCabsNumber = 11;
    static const u32 mCabsBaseId = 0x03f2;
    static const u32 mCabsBaseHealth = 100; //should take that from .def instead...
    static const u8 mStationsNumber = 8;
    
  private:
    static const u16 mSubwayInitData [];
    static const u32 mCabLoopTime;
    static const u32 mCab0TimeOffset;
    static const s32 mTimingAdjust;
    static const u32 mCabIntervalTime;
    static const u32 mOpenDoorOffset [];
    static const u32 mOpenDoorDuration [];
    static const char* mSubwayStationName [];
    static PCharCoordinates mCabExitPositions [2][mStationsNumber];
    
    PSubwayInfo mSubways[mCabsNumber];

public:    
    bool GetInfoIndex(u32 nVhcId, u8 *Index = NULL);
    
  public:
    PSubway();
    //~PSubway();
    
    inline bool IsValidSubwayCab(u32 nVhcId) {return GetInfoIndex(nVhcId); }
    bool UpdateInfo(u32 nVhcId, u16 nPosition, u8 nDoorOpened);
    u16 GetPosition(u32 nVhcId);
    
    u32 GetTimeOffset(u32 nVhcId, u32 nTime);
    u8 GetStation(u32 nVhcId, u32 nTime, u32* TimeOffset = NULL);
    bool IsDoorOpen(u32 nVhcId, u32 nTime);
    std::string* GetStationName(u8 nStationId);
    bool GetStationExitPosition(PCharCoordinates* nPosition, u8 nStationId, f32 nCoef = 0.5);
    
    u8 GetFreeSeat(u32 nVhcId);
    bool SetSeatUser(u32 nVhcId, u8 nSeat, u32 nCharId);
    bool UnsetSeatUser(u32 nVhcId, u8 nSeat, u32 nCharId);
};

#endif

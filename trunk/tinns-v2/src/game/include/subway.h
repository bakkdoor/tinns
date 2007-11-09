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

#define SUBWAY_VHC_BASE_ID 0x03f2
#define SUBWAY_VHC_NB 11

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
  
  private:
    PSubwayInfo mSubways[SUBWAY_VHC_NB];
    
    bool GetInfoIndex(u16 nVhcId, u8 *Index = NULL);
    
  public:
    PSubway();
    //~PSubway();
    
    inline bool IsValidSubwayCab(u16 nVhcId) {return GetInfoIndex(nVhcId); }
    bool UpdateInfo(u16 nVhcId, u16 nPosition, u8 nDoorOpened);
    u16 GetPosition(u16 nVhcId);
    u8 GetDoorOpened(u16 nVhcId);
    u8 GetFreeSeat(u16 nVhcId);
    bool SetSeatUser(u16 nVhcId, u8 nSeat, u32 nCharId);
    bool UnsetSeatUser(u16 nVhcId, u8 nSeat, u32 nCharId);
};

#endif

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

	udp_vhc.h - decoder classes for UDP vehicle related messages
  
	CREATION: 5 Sep 2006 Hammag

	MODIFIED:
	REASON: - 

*/

#ifndef UDPVHC_H
#define UDPVHC_H

class PUdpVhcMove : public PUdpMsgAnalyser
{ 
  private:
    u16 mVhcLocalId;
    u16 mNewY;
    u16 mNewZ;
    u16 mNewX;
    u16 mNewLR;
    u16 mNewRoll;
    u16 mUnk1; // always 0x0001 ?
    u8 mMoveType;
    u8 mNewUD;
    u8 mFF; // always 0xff ?
    u8 mAction; // &1 = Left, &2 = Right, &4 = Forward, &8 = Backward	
  public:
    PUdpVhcMove(PMsgDecodeData* nDecodeData);
    //~PUdpVhcMove();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpVhcUse : public PUdpMsgAnalyser
{
  private:
    u32 mVehicleID;
    u8 mVehicleSeat;
    
  public:
    PUdpVhcUse(PMsgDecodeData* nDecodeData);
    //~PUdpVhcMove();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpSubwayUpdate : public PUdpMsgAnalyser
{
  private:
    u32 mVehicleID;
    u16 mPosition;
    u8 mDoorOpened;
    
  public:
    PUdpSubwayUpdate(PMsgDecodeData* nDecodeData);
    //~PUdpSubwayUpdate();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpRequestSeatInfo : public PUdpMsgAnalyser
{
  private:
    u32 mVehicleID;
    
  public:
    PUdpRequestSeatInfo(PMsgDecodeData* nDecodeData);
    //~PUdpRequestSeatInfo();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

#endif

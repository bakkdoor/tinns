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

	udp_charmove.h - decoder classes for UDP char movement messages

	CREATION: 5 Sep 2006 Hammag

	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/

#ifndef UDPCHARMOVE_H
#define UDPCHARMOVE_H

#include "include/msgdecoder.h"
#include "udpanalyser.h"

class PUdpCharPosUpdate : public PUdpMsgAnalyser
{
  private:
    u8 mInfoBitfield;
    u16 mNewY;
    u16 mNewZ;
    u16 mNewX;
    u8 mNewUD;
    u8 mNewLR;
    u8 mNewAct;
    u8 mNewUnknown;
    u32 mChairItemID; // u16 or u32 ???
    u8 mChairItemSeat;

  public:
    PUdpCharPosUpdate(PMsgDecodeData* nDecodeData);
    //~PUdpCharMoves();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpCharExitChair : public PUdpMsgAnalyser
{
  public:
    PUdpCharExitChair(PMsgDecodeData* nDecodeData);
    //~PUdpCharExitChair();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();

    //nClient is optionnal, nVhc is to limit to that vhc, nForce to force exit event when cab door closed
    static bool DoLeaveChair(PChar* nChar, PClient* nClient = NULL, PSpawnedVehicle* nVhc = NULL, bool nForce = false); 
};

class PUdpCharJump : public PUdpMsgAnalyser
{
  public:
    PUdpCharJump(PMsgDecodeData* nDecodeData);
    //~PUdpCharJump();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpCharTargeting : public PUdpMsgAnalyser
{
  public:
    PUdpCharTargeting(PMsgDecodeData* nDecodeData);
    //~PUdpCharTargeting();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};
#endif

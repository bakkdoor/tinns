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

	msgbuilder.h - a classes to build NC messages

	CREATION: 30 Aug 2006 Hammag

	MODIFIED:
	REASON: -

*/

#ifndef MSGBUILDER_H
#define MSGBUILDER_H

class PClient;
class PMessage;

class PMsgBuilder
{
	private:

	public:
	  PMsgBuilder() {};
    ~PMsgBuilder() {};

// Following methods for broadcasted messages do NOT include UDP_ID increment
// when needed, nor UDP_ID / SessionID setting
// as these must be set on a destination client basis
  PMessage* BuildCharHelloMsg(PClient* nClient);
  PMessage* BuildCharHealthUpdateMsg (PClient* nClient);
  PMessage* BuildCharPosUpdateMsg (PClient* nClient);
  PMessage* BuildCharPosMoveMsg (PClient* nClient, u16 nNewX, u16 nNewY, u16 nNewZ);
  PMessage* BuildCharSittingMsg (PClient* nClient, u16 nData);
  PMessage* BuildCharExitChairMsg (PClient* nClient);
  PMessage* BuildCharJumpingMsg (PClient* nClient);
  PMessage* BuildCharEnteringVhcMsg (PClient* nClient, u16 nVehicleID, u8 nVehicleSeat);
  PMessage* BuildDoorOpenMsg (u32 nRawItemID, bool nDoubleDoor = false);
  PMessage* BuildCharUseChairMsg (PClient* nClient, u32 nRawChairID);
  PMessage* BuildCharShowGlowCircleMsg (PClient* nClient);
  PMessage* BuildCharVanishMsg (PClient* nClient);

  // Following methods for unicast messages DO include UDP_ID increment and
  // UDP_ID / SessionID setting when needed (at least for now)
  PMessage* BuildReqInfoAnswerMsg (PClient* nClient, u16 nReqType, u32 nInfoId, void* nResponse, u16 nResponseLength);

  PMessage* BuildPacket0Msg (PClient* nClient);
  PMessage* BuildPingMsg (PClient* nClient, u32 nClientTime);
  PMessage* BuildBaselineMsg (PClient* nClient);
  PMessage* BuildCharInfo3Msg (PClient* nClient);

  PMessage* BuildZoning1Msg (PClient* nClient, u16 nData);
  PMessage* BuildZoningTCPReadyMsg ();
  PMessage* BuildZoning2Msg (PClient* nClient);
  PMessage* BuildGenrepZoningMsg (PClient* nClient, u32 nLocation, u16 nEntity);

  PMessage* BuildGenrepAddToListMsg (PClient* nClient, u32 nLocation, u16 nEntity);

  PMessage* BuildAptLiftUseMsg (PClient* nClient, u32 nLocation, u16 nEntity);
  PMessage* BuildAptLiftFailedMsg (PClient* nClient);
  PMessage* BuildChangeLocationMsg (PClient* nClient, u32 nLocation, u16 nEntity, u8 nLevel = 0, u32 nRawItemID = 0);

  PMessage* BuildSubskillIncMsg (PClient* nClient, u8 nSubskill, u16 nSkillPoints);
  PMessage* BuildChatAddMsg (PClient* nClient, u32 nAddedCharID, u8 nMode); // mode = 1 for Direct, 2 for Buddy

  PMessage* BuildText100Msg (PClient* nClient, u8 nTxtMsgId, u32 nRawObjectID);

  PMessage* BuildFurnitureActivateMsg (PClient* nClient, u32 nRawObjectID, u8 nActionValue);
  PMessage* BuildCharUseFurnitureMsg (PClient* nClient, u32 nRawObjectID);
  PMessage* BuildCharUseGogoMsg (PClient* nClient);
  PMessage* BuildCharUseGenrepMsg (PClient* nClient, u32 nRawObjectID, u32 nLocation, u16 nEntity);
  PMessage* BuildCharUseLiftMsg (PClient* nClient, u32 nRawObjectID, u16 nAptPlace);
  PMessage* BuildCharUseVhcMsg (PClient* nClient, u32 nRawObjectID, u16 nVhcType);
};

#endif

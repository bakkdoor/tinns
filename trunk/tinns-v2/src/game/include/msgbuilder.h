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
  PMessage* BuildCharPosUpdate2Msg (PClient* nClient, u8 InfoBitfield = 0x7f);
  PMessage* BuildCharSittingMsg (PClient* nClient, u16 nData);
  PMessage* BuildCharExitChairMsg (PClient* nClient);
  PMessage* BuildCharJumpingMsg (PClient* nClient);
  PMessage* BuildCharEnteringVhcMsg (PClient* nClient, u16 nVehicleID, u8 nVehicleSeat);
  PMessage* BuildDoorOpenMsg (u32 nRawItemID, bool nDoubleDoor = false);
  PMessage* BuildCharUseChairMsg (PClient* nClient, u32 nRawChairID);
  PMessage* BuildCharShowGlowCircleMsg (PClient* nClient);
  PMessage* BuildCharVanishMsg (PClient* nClient);
  //PMessage* BuildRemoveWorldObjectMsg (u32 nWOID);
  //PMessage* BuiltSpawnObjectMsg (PClient* nClient, u16 nActorID, u16 nFunctionID, u32 nWOID);

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
  PMessage* BuildGenrepDenyBrokenMsg (PClient* nClient);

  PMessage* BuildAptLiftUseMsg (PClient* nClient, u32 nLocation, u16 nEntity);
  PMessage* BuildAptLiftFailedMsg (PClient* nClient);
  PMessage* BuildChangeLocationMsg (PClient* nClient, u32 nLocation, u16 nEntity, u8 nLevel = 0, u32 nRawItemID = 0);
  PMessage* BuildEntityPositionMsg (PClient* nClient, u16 pX, u16 pY, u16 pZ);
  
  PMessage* BuildCharAptLocInfoMsg (PClient* nClient);

  PMessage* BuildSubskillIncMsg (PClient* nClient, u8 nSubskill, u16 nSkillPoints);
  PMessage* BuildChatAddMsg (PClient* nClient, u32 nAddedCharID, u8 nMode); // mode = 1 for Direct, 2 for Buddy

  PMessage* BuildText100Msg (PClient* nClient, u8 nTxtMsgId, u32 nRawObjectID);
  PMessage* BuildTextIniMsg (PClient* nClient, u8 nTxtGroupID, u16 nTxtID);

  PMessage* BuildFurnitureActivateMsg (PClient* nClient, u32 nRawObjectID, u8 nActionValue);
  PMessage* BuildCharUseFurnitureMsg (PClient* nClient, u32 nRawObjectID);
  PMessage* BuildCharUseGogoMsg (PClient* nClient);
  PMessage* BuildCharUseGenrepMsg (PClient* nClient, u32 nRawObjectID, u32 nLocation, u16 nEntity);
  PMessage* BuildCharUseLiftMsg (PClient* nClient, u32 nRawObjectID, u16 nAptPlace);
  PMessage* BuildCharUseVhcMsg (PClient* nClient, u32 nRawObjectID, u16 nVhcType);

  PMessage* BuildCharMoneyUpdateMsg (PClient* nClient, u32 nCredits);
  PMessage* BuildCharUseQBSlotMsg1 (PClient* nClient, u8 nValue);
  PMessage* BuildCharUseQBSlotMsg2 (PClient* nClient, u16 nV1 = 100, u16 nV2 = 100, u16 nV3 = 100, u16 nV4 = 100, u16 nV5 = 100, u16 nV6 = 100, u16 nV7 = 0);
  PMessage* BuildCharUseQBSlotMsg3 (PClient* nClient, u8 nSlot);
  PMessage* BuildCharUseQBSlotMsg4 (PClient* nClient, u16 nValue1);

  PMessage* BuildCharOpenContainerMsg (PClient* nClient, u32 nContainerID); // TEMP Solution! Need to change this when we have an itemsystem
  PMessage* BuildItemMoveMsg (PClient* nClient, u8 nSource, u8 nSrcX, u8 nSrcY, u8 nDestination, u8 nDestX, u8 nDestY, u8 nItemCnt);
  PMessage* BuildStartHackGameMsg(PClient* nClient, u32 nWorldObjID, u8 nHackDifficult);
};

#endif

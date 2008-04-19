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
class PContainerEntry;
class PContainer;
class PSpawnedVehicle;

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
  PMessage* BuildCharSittingMsg (PClient* nClient);
  PMessage* BuildCharExitSeatMsg (PClient* nClient);
  PMessage* BuildCharJumpingMsg (PClient* nClient);
  PMessage* BuildDoorOpenMsg (u32 nRawItemID, bool nDoubleDoor = false);
  PMessage* BuildCharUseSeatMsg (PClient* nClient, u32 nRawObjectId, u8 nSeatId = 0);
  PMessage* BuildCharShowGlowCircleMsg (PClient* nClient);
  PMessage* BuildCharVanishMsg (PClient* nClient);

  PMessage* BuildSpawnWorldObjectMsg (u16 nModelID, u16 nFunctionID, u32 nWOID, u16 nPosX, u16 nPosY, u16 nPosZ, u8 nRotX, u8 nRotY, u8 nRotZ);
  PMessage* BuildRemoveWorldObjectMsg (u32 nWOID);
  
  PMessage* BuildSubwaySingleUpdateMsg (u32 nVehicleID, u16 nPosition, u8 nDoorOpened);
  PMessage* BuildVhcHealthUpdateMsg (PSpawnedVehicle* nVehicle);
  PMessage* BuildVhcPosUpdateMsg (PSpawnedVehicle* nVehicle);
  
  // Following methods for unicast messages DO include UDP_ID increment and
  // UDP_ID / SessionID setting when needed (at least for now)
  PMessage* BuildReqInfoAnswerMsg (PClient* nClient, u16 nReqType, u32 nInfoId, void* nResponse, u16 nResponseLength);

  PMessage* BuildPacket0Msg (PClient* nClient);
  PMessage* BuildPingMsg (PClient* nClient, u32 nClientTime);
  PMessage* BuildBaselineMsg (PClient* nClient);
  //PMessage* BuildCharInfo3Msg (PClient* nClient); // Removed because same as Zoning2Msg

  PMessage* BuildZoning1Msg (PClient* nClient, u16 nEntity, u8 nUnknown = 0);
  PMessage* BuildZoningTCPReadyMsg ();
  PMessage* BuildSendZoneTCPMsg (u32 nLocation, std::string* nWorldName);
  PMessage* BuildZoning2Msg (PClient* nClient);
  PMessage* BuildGenrepZoningMsg (PClient* nClient, u32 nLocation, u16 nEntity);

  PMessage* BuildGenrepAddToListMsg (PClient* nClient, u32 nLocation, u16 nEntity);
  PMessage* BuildGenrepDenyBrokenMsg (PClient* nClient);

  PMessage* BuildAptLiftUseMsg (PClient* nClient, u32 nLocation, u16 nEntity, u8 nEntityType = 0);
  PMessage* BuildAptLiftFailedMsg (PClient* nClient);
  PMessage* BuildChangeLocationMsg (PClient* nClient, u32 nLocation, u16 nEntity, u8 nEntityType = 0, u32 nRawItemID = 0);
  PMessage* BuildEntityPositionMsg (PClient* nClient, u16 pX, u16 pY, u16 pZ);
  
  PMessage* BuildCharAptLocInfoMsg (PClient* nClient);

  PMessage* BuildSubskillIncMsg (PClient* nClient, u8 nSubskill, u16 nSkillPoints);
  PMessage* BuildChatAddMsg (PClient* nClient, u32 nAddedCharID, u8 nMode); // mode = 1 for Direct, 2 for Buddy

  PMessage* BuildText100Msg (PClient* nClient, u8 nTxtMsgId, u32 nRawObjectID);
  PMessage* BuildTextIniMsg (PClient* nClient, u8 nTxtGroupID, u16 nTxtID);

  PMessage* BuildCharInteractionMenuMsg (PClient* nClient, u32 nRawTargetID);
    
  PMessage* BuildFurnitureActivateMsg (PClient* nClient, u32 nRawObjectID, u8 nActionValue);
  PMessage* BuildCharUseFurnitureMsg (PClient* nClient, u32 nRawObjectID);
  PMessage* BuildCharUseVhcTerminalMsg (PClient* nClient, u32 nRawObjectID);
  PMessage* BuildCharUseGogoMsg (PClient* nClient);
  PMessage* BuildCharUseGenrepMsg (PClient* nClient, u32 nRawObjectID, u32 nLocation, u16 nEntity);
  PMessage* BuildCharUseLiftMsg (PClient* nClient, u32 nRawObjectID, u16 nAptPlace);
  PMessage* BuildCharUseVhcMsg (PClient* nClient, u32 nRawObjectID, u16 nVhcType);

  PMessage* BuildCharMoneyUpdateMsg (PClient* nClient, u32 nCredits);
  PMessage* BuildUndefineduseMsg (PClient* nClient, u8 nValue);
  inline PMessage* BuildCharUseQBSlotMsg1 (PClient* nClient, u8 nValue) {return BuildUndefineduseMsg(nClient, nValue);}
  PMessage* BuildCharUseQBSlotMsg2 (PClient* nClient, u16 nV1 = 100, u16 nV2 = 100, u16 nV3 = 100, u16 nV4 = 100, u16 nV5 = 100, u16 nV6 = 100, u16 nV7 = 0);
  PMessage* BuildCharUseQBSlotMsg3 (PClient* nClient, u8 nSlot);
  PMessage* BuildCharUseQBSlotMsg4 (PClient* nClient, u16 nValue1);
  PMessage* BuildContainerContentList (PContainer* nContainer, u8 nLocType);
  PMessage* BuildContainerContentEntry (PContainerEntry* nEntry, u8 nLocType);

  PMessage* BuildCharOpenContainerMsg (PClient* nClient, u32 nContainerID, PContainer* nContainer); // TEMP Solution! Need to change this when we have an itemsystem
  PMessage* BuildItemMoveMsg (PClient* nClient, u8 nSource, u8 nSrcX, u8 nSrcY, u8 nDestination, u8 nDestX, u8 nDestY, u8 nItemCnt);
  PMessage* BuildBoxItemMoveMsg (PClient* nClient, PContainerEntry* nEntry, u8 nSrcX, u8 nSrcY, u8 nDestination, u8 nDestX, u8 nDestY, u8 nItemCnt);
    
  PMessage* BuildStartHackGameMsg (PClient* nClient, u32 nWorldObjID, u8 nHackDifficult);
  
  PMessage* BuildSubwaySpawnMsg(PClient* nClient, bool IsSecondMessage);
  //PMessage* BuildSubwayFullUpdateMsg(PClient* nClient);
  PMessage* BuildVhcInfoMsg (PClient* nClient, PSpawnedVehicle* nVehicle);
  
  PMessage* BuildDBRequestStatusMsg(PClient* nClient, std::string* nCommandName, u8 nStatus, u16 nErrCode);
  PMessage* BuildDBAnswerMsg(PClient* nClient, std::string* nCommandName, std::string* nAnswerData, u16 nRows, u16 nCols);
    
  PMessage* BuildTraderItemListMsg(PClient* nClient, u32 nTraderNpcID);
};

#endif

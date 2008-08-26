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

 udp_quickaccessbelt.cpp - decoder classes for UDP Quickaccessbelt related messages

 CREATION: 30 Dec 2006 Namikon

 MODIFIED: 1 Sept 2007 Hammage
 REASON: Put analysis code in Analyse() and change to use new item management methods

*/

#include "main.h"
#include "udp_quickaccessbelt.h"
#include "container.h"
#include "inventory.h"

/**** PUdpItemSlotUse ****/

PUdpItemSlotUse::PUdpItemSlotUse(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdpItemSlotUse::Analyse()
{
  mDecodeData->mName << "=Active QuickBelt Slot";

  mTargetSlot = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 8);
  // TODO : Check on mTargetSlot value + put set to INV_WORN_QB_HAND for hand
  if(mTargetSlot == 255) // H "slot 0" Hand
    mTargetSlot = INV_WORN_QB_HAND;

  //if(mTargetSlot == 11) //  ALT-H ?
    
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpItemSlotUse::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* tChar = nClient->GetChar();
  PItem* targetItem = NULL;
  bool tUsable = false;
  bool tUsableInHand = false;
  u16 ItemVal1 = 0;

Console->Print("Client trying to activate item in slot %d.", mTargetSlot);

  u8 currentActiveSlot = tChar->GetQuickBeltActiveSlot();
  if(mTargetSlot == INV_WORN_QB_HAND)
  {
Console->Print("Want to use Hand");
    tUsable = true;
    tUsableInHand = true;
  }
  else
  {
    targetItem = tChar->GetInventory()->GetContainer(INV_LOC_WORN)->GetItem(INV_WORN_QB_START + mTargetSlot);
    if(targetItem)
    { // TODO : do the real check;
Console->Print("Want to use existing item");
      tUsable = true;
      tUsableInHand = true;
      ItemVal1 = targetItem->GetValue1();
    }
  }

//TODO:
// if tUsableInHand
//  must check if weapon and allowed in zone
//  and change tUsableInHand to false if needed

  if(tUsable && !tUsableInHand)
  {
    // active item in belt, but don't take in hand
Console->Print(YELLOW, BLACK, "Debug: activation of QB item slot %d (%s)", mTargetSlot, targetItem->GetName().c_str());    
  }
  else
  {
    if(!tUsable)
    {
Console->Print("Want to use non-usable item");  
      mTargetSlot = INV_WORN_QB_NONE; // if not usable, unequip active one
    }
      
    if(mTargetSlot == currentActiveSlot) // if same as active, unequip active one 
    {
Console->Print("Want to use active slot");
      mTargetSlot = INV_WORN_QB_NONE;
    }
    
    if(mTargetSlot != currentActiveSlot) // now do somthing only if not same as active
    {
      if(tChar->SetQuickBeltActiveSlot(mTargetSlot))
      {
        PMessage* tmpMsg;
        tmpMsg = MsgBuilder->BuildCharHelloMsg(nClient);
        ClientManager->UDPBroadcast(tmpMsg, nClient);
      
        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg1(nClient, 59);
        nClient->getUDPConn()->SendMessage(tmpMsg);

        if(ItemVal1 > 0)
        {
          tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg2(nClient);
          nClient->getUDPConn()->SendMessage(tmpMsg);
        }
        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg3(nClient, mTargetSlot);
        nClient->getUDPConn()->SendMessage(tmpMsg);

        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg4(nClient, ItemVal1);
        nClient->getUDPConn()->SendMessage(tmpMsg);

Console->Print(YELLOW, BLACK, "Debug: activation of QB item slot %d", mTargetSlot);
      }
else Console->Print(RED, BLACK, "Error: activation of QB slot %d refused by PChar", mTargetSlot);
    }
else
{
Console->Print("Same slot %d as active - do nothing", mTargetSlot);
}
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

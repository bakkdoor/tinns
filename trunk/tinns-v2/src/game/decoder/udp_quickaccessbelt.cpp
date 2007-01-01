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

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_quickaccessbelt.h"

/**** PUdpItemSlotUse ****/

PUdpItemSlotUse::PUdpItemSlotUse(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdpItemSlotUse::Analyse()
{
    mDecodeData->mName << "=Using item from QuickAccessBelt";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpItemSlotUse::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();
    u8 SlotNumber = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 8);

    Console->Print("Client trying to activate item in slot %d.", SlotNumber);

    u16 ItemVal1 = 0;
    u16 qb_item = tChar->GetInventory()->QB_GetSlot(SlotNumber);
    u16 active_item = tChar->GetItemInHand();
    // Case 1: No item active, select slot X
    // Ex: active_item = 0
    // Act: Activate Slot X

    // Case 2: Slot X active, press same slot again
    // Ex: active_item = 28, qb_item = 28
    // Act: Deactivate Slot X

    // Case 3: Slot X active, press other slot Y
    // Ex: active_item = 28, qb_item = 35
    // Act: Activate Slot Y

    if((active_item == 0) || (qb_item != active_item))
    {
        if(qb_item == 0) // Slot is empty, unequip current item in use
            return true;

        const PDefItems *def = GameDefs->GetItemsDef(qb_item);  // Search for this itemID in items.def
        if(def != NULL)
        {
            ItemVal1 = (u16)def->GetValue1();
        }
        else
        {
            Console->Print("ERROR: Unable to get itemdata for itemID %d", qb_item);
            return false;
        }
        // Done, now set this itemnumber as "item in hand"
        tChar->SetItemInHand(qb_item);
    }
    else // Should only match for case 2 ( qb_item == active_item ), but better us a generic else here
    {
        tChar->SetItemInHand(0);
        ItemVal1 = 0;
        SlotNumber = 10; // Slot for "nothing"
    }

    PMessage* tmpMsg;
    tmpMsg = MsgBuilder->BuildCharHelloMsg(nClient);
    ClientManager->UDPBroadcast(tmpMsg, nClient);
    tmpMsg = NULL;

    tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg1(nClient, 59);
    nClient->getUDPConn()->SendMessage(tmpMsg);
    tmpMsg = NULL;
    if(ItemVal1 > 0)
    {
        tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg2(nClient);
        nClient->getUDPConn()->SendMessage(tmpMsg);
        tmpMsg = NULL;
    }
    tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg3(nClient, SlotNumber);
    nClient->getUDPConn()->SendMessage(tmpMsg);
    tmpMsg = NULL;
    tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg4(nClient, ItemVal1);
    nClient->getUDPConn()->SendMessage(tmpMsg);
    tmpMsg = NULL;

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

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

 udp_itemmove.cpp - decoder classes for UDP item move related messages

 CREATION: 30 Dec 2006 Namikon

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_itemmove.h"

/**** PUdpItemSlotUse ****/

PUdpItemMove::PUdpItemMove(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x1e";
}

PUdpMsgAnalyser* PUdpItemMove::Analyse()
{
    mDecodeData->mName << "=Moving item";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpItemMove::DoAction()
{
    PMessage* tmpMsg = NULL;
    bool DoneAnalyzing = false; // Make sure only ONE action is performed here
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();

    u8 src_loc = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 8);
    u8 src_x = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 9);
    u8 src_y = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 10);
    u8 dst_loc = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 11);
    u8 dst_x = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 12);
    u8 dst_y = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 13);
    u8 itemcnt = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 14);

    // Case #1: Simplest thing, client trying to change slots
    if(src_loc == 2 && dst_loc == 2)
    {
        if(tChar->GetInventory()->QB_IsFree(dst_x) == false)  // Destination slot is in use? Break here
            return true;

        tChar->GetInventory()->QB_Move(src_x, dst_x); // Destination slot is free, now switch item internal
        // Now built the clientmessage to move item on client
        tmpMsg = MsgBuilder->BuildItemMoveMsg(nClient, src_loc, src_x, src_y, dst_loc, dst_x, dst_y, itemcnt);
        DoneAnalyzing = true;
    }
// *_loc:
// 1: Ground
// 2: QB
// 3: Inv
// 4: Current open container/trashcan/box/w.e.
// 18:GoGuardian

    //if (gDevDebug)
    //{
        if(src_loc == 1 && dst_loc == 2)
            Console->Print("Client trying to move %d items from ground to Quickaccessbelt slot %d", itemcnt, dst_x);
        else if(src_loc == 1 && dst_loc == 3)
            Console->Print("Client trying to move %d items from ground to Inventory X %d Y %d", itemcnt, dst_x, dst_y);
        else if(src_loc == 2 && dst_loc == 1)
            Console->Print("Client trying to move %d items from Quickaccessbelt slot %d to ground", itemcnt, src_x);
        else if(src_loc == 2 && dst_loc == 2)
            Console->Print("Client trying to move %d items from Quickaccessbelt slot %d to Quickaccessbelt slot %d", itemcnt, src_x, dst_x);
        else if(src_loc == 2 && dst_loc == 3)
            Console->Print("Client trying to move %d items from Quickaccessbelt slot %d to Inventory X %d Y %d", itemcnt, src_x, dst_x, dst_y);
        else if(src_loc == 3 && dst_loc == 1)
            Console->Print("Client trying to move %d items from Inventory to Ground", itemcnt);
        else if(src_loc == 3 && dst_loc == 2)
            Console->Print("Client trying to move %d items from Inventory to Quickaccessbelt slot %d", itemcnt, dst_x);
        else
            Console->Print("Client trying to move %d items from LOC: %d X: %d Y: %d to LOC: %d X: %d Y: %d", itemcnt, src_loc, src_x, src_y, dst_loc, dst_x, dst_y);
    //}

    if(DoneAnalyzing == true && tmpMsg != NULL) // Send message if we got one
        nClient->getUDPConn()->SendMessage(tmpMsg);

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

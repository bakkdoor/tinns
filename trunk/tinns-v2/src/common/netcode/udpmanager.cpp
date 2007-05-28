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
	udpmanager.cpp - Manager for UDP Messages. Watches UDP_ID's and keeps
                     message history for possible OutOfOrder requests
*/

#include "main.h"

void ConnectionUDP::UpdateMessageBuffer()
{
    int erasednum = 0;
    // Delete all old messages
    for(PMessageMap::iterator it=UDPMessages.begin(); it!=UDPMessages.end(); it++)
    {
        if((int)it->first < (int)(mLastUDPID - MAX_RETENTION))
        {
            erasednum++;
            delete it->second;
            UDPMessages.erase(it);
        }
    }
    /* // Debug output
    if(erasednum > 0)
        Console->Print("[UpdateMessageBuffer] Done updating messagequeue, %d entries deleted", erasednum);
    */
}

void ConnectionUDP::ResetMessageBuffer()
{
    if(mUDP_ID != 0)
    {
        Console->Print("%s MessageQueue got erased but UDP_ID is still >0", Console->ColorText(RED, BLACK, "[WARNING]"));
    }
    for(PMessageMap::iterator it=UDPMessages.begin(); it!=UDPMessages.end(); it++)
    {
        delete it->second;
        UDPMessages.erase(it);
    }
    mLastUDPID = 0;
//    Console->Print("[udpmanager] Erased messagebuffer");
}

void ConnectionUDP::ReSendUDPMessage(u16 nUDP_ID)
{
    // ReSend packet with given UDP_ID
    if(nUDP_ID > mLastUDPID)
    {
        Console->Print("%s Cannot resend packet with UDP_ID %d, msgnumber is higher than last known udpID", Console->ColorText(RED, BLACK, "[PANIC]"), nUDP_ID);
    }
    else
    {
        // UDP_ID seems to be valid, now search for it
        PMessageMap::const_iterator it = UDPMessages.find(nUDP_ID);
        if(it == UDPMessages.end())
        {
            int dynRetention = (int)mLastUDPID - nUDP_ID;
            if(dynRetention > MAX_RETENTION)
            {
                Console->Print("%s Packet with UDP_ID %d not found. Increase #define MAX_RETENTION to at least %d", Console->ColorText(RED, BLACK, "[WARNING]"), nUDP_ID, dynRetention);
            }
            else
            {
                Console->Print("%s Packet with UDP_ID %d is missing in the packet queue!", Console->ColorText(RED, BLACK, "[PANIC]"), nUDP_ID);
            }
            Console->Print("Trying to cancel OOO notice by sending dummy packet");
            PMessage* tmpMsg = new PMessage(14);
            //u16 tmpSessionID = mLastUDPID + SESSION_UDP_OFFSET;
            *tmpMsg << (u8)0x13;
            *tmpMsg << nUDP_ID;
            *tmpMsg << (u16)(nUDP_ID + SESSION_UDP_OFFSET);
            *tmpMsg << (u8)0x08;
            *tmpMsg << (u8)0x03;
            *tmpMsg << nUDP_ID;
            *tmpMsg << (u8)0x1F;
            *tmpMsg << (u16)0xFFFF;     // Should do nothing, CharID 65535 should never exist
            *tmpMsg << (u16)0x3C01;     // This value IS wrong way, so that nothing can happen at all
            SendMessage(tmpMsg, true);
        }
        else if(it->second)
        {
            Console->Print("[OOO-Buster] ReSending UDP packet with ID %d", nUDP_ID);
            // Build new message, including the missing UDP packets as content
            u16 MsgSize = it->second->GetSize();
            PMessage* tmpMsg = new PMessage(MsgSize + 5); // Create new message
            *tmpMsg << (u8)0x13;
            *tmpMsg << nUDP_ID;
            *tmpMsg << (u16)(nUDP_ID + SESSION_UDP_OFFSET);
//            *tmpMsg << mUDP_ID;
//            *tmpMsg << mSessionID;
//            *tmpMsg << *it->second; // This should work, but it doesnt! Causes segfault after sending a few packets
            for(int x = 0; x < MsgSize; x++)
            {
                *tmpMsg << it->second->U8Data(x);
            }
            SendMessage(tmpMsg, true);  // Add message to outgoing VIP queue
        }
    }
}

void ConnectionUDP::InsertUDPMessage(PMessage* nMsg)
{
    if (!nMsg)
        return;

    if(nMsg->U8Data(0) != 0x13) return; // Only add real UDP messages here
//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28
// 13 07 00 F2 00 05 03 02 00 11 11 05 03 03 00 11 11 05 03 04 00 11 11 05 03 05 00 11 11 05 03 06 00 11 11 05 03 07 00 11 11

    // Grab submessages from packet, check if message is 0x03 commandset. If not, dont add message
    PMessage* tWorkMsg = NULL;

    u16 tCurPos = 5;
    u8 tSubMsgLen = 0;
    u16 tmpUDPID = 0;
    //nMsg->Dump();
    while(tCurPos < (nMsg->GetSize() - 1)) // Loop while we still have more frames. (-1: GetSize starts at 1, pos pointer at 0)
    {
        //Console->Print("tCurPos = %d  nMsg->GetSize = %d", tCurPos, nMsg->GetSize());
        tSubMsgLen = nMsg->U8Data(tCurPos) + 1;          // Get the lenght of the frame, and add the lenght byte
        if(nMsg->U8Data(tCurPos+1) != 0x03)
        {
            //Console->Print("Ignoring UDP message, no 0x03 commandset");
            tCurPos += tSubMsgLen;                       // Set pointer to the end of this frame
            continue;                                    // Skip if frame is not an 0x03 commandset
        }
        //Console->Print("MsgLen: %d", tSubMsgLen);
        tWorkMsg = nMsg->GetChunk(tCurPos, tSubMsgLen); // get the frame.
        //Console->Print("Msg:");
        //tWorkMsg->Dump();
        tmpUDPID = nMsg->U16Data(tCurPos + 2);                     // Get the UDP ID of this frame
        //Console->Print("UDP ID: %d", tmpUDPID);
        PMessageMap::const_iterator it = UDPMessages.find(tmpUDPID);                 // Try to find the UDP ID in the queue
        if(it->second)                                   // If we already have this UDP msg, print error
        {
            Console->Print("%s Packet *NOT* added to history buffer, UdpID %d already sent! (This may cause an OOO)", Console->ColorText(RED, BLACK, "[WARNING]"), tmpUDPID);
            nMsg->Dump();
        }
        else                                            // We dont have this msg? Add it!
        {
            //Console->Print("Added UDP ID %d to messagebuffer", tmpUDPID);
            UDPMessages.insert(std::make_pair(tmpUDPID, tWorkMsg));
            mLastUDPID++;
        }
        tCurPos += tSubMsgLen;                      // Set pointer to the end of this frame
    }
}

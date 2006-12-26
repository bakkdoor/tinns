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
            UDPMessages.erase(it);
        }
    }
//    if(erasednum > 0)
//        Console->Print("[udpmanager] Done updating messagequeue, %d entries deleted", erasednum);
}

void ConnectionUDP::ResetMessageBuffer()
{
    if(mUDP_ID != 0)
    {
        Console->Print("%s MessageQueue got erased but UDP_ID is still >0", Console->ColorText(RED, BLACK, "[WARNING]"));
    }
    UDPMessages.clear();
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
            u16 tmpSessionID = mLastUDPID + 37917;
            *tmpMsg << (u8)0x13;
            *tmpMsg << mLastUDPID;
            *tmpMsg << tmpSessionID;
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
            PMessage* tmpMsg = it->second;
            tmpMsg->SetNextByteOffset(1);

            // The packet is sent unchanged, except the first UDP and Session ID
            *tmpMsg << mUDP_ID;
            *tmpMsg << mSessionID;

            SendMessage(tmpMsg, true);  // Add message to outgoing VIP queue
        }
    }
}

void ConnectionUDP::InsertUDPMessage(PMessage* nMsg)
{
    if (!nMsg)
        return;

    // Check if message is 0x03 commandset. If not, dont add message
    if(nMsg->U8Data(6) == 0x03) // Since we only add single messages here and no multiframe, we can check for pos 6
    {
        u16 tmpUDPID = nMsg->U16Data(1);
        PMessageMap::const_iterator it = UDPMessages.find(tmpUDPID);
        int tmpOffset = tmpUDPID - mLastUDPID;
        if(it->second)
        {
            Console->Print("%s Packet *NOT* added to history buffer, UdpID %d already sent! (This may cause an OOO)", Console->ColorText(RED, BLACK, "[WARNING]"), tmpUDPID);
        }
        else
        {
            if(tmpOffset > 1)    // Check if UDP_ID got increased correctly. If not, add missing UDP_IDs and add original packet after
            {
                Console->Print("%s UdpID is out of order, expected %d, got %d (%d too high)!", Console->ColorText(RED, BLACK, "[WARNING]"), mLastUDPID + 1, tmpUDPID, tmpOffset);
                Console->Print("Adding %d fake Udp messages to history buffer...", tmpOffset);
                while(tmpOffset > 1)  // Add as many packets until tmpOffset is 1 again. This one packet is added after
                {
                    mLastUDPID++;
                    u16 tmpSessionID = mLastUDPID + 37917;

                    PMessage* tmpMsg = new PMessage(14);
                    *tmpMsg << (u8)0x13;
                    *tmpMsg << mLastUDPID;
                    *tmpMsg << tmpSessionID;
                    *tmpMsg << (u8)0x08;
                    *tmpMsg << (u8)0x03;
                    *tmpMsg << mLastUDPID;
                    *tmpMsg << (u8)0x1F;
                    *tmpMsg << (u16)0xFFFF;     // Should do nothing, CharID 65535 should never exist
                    *tmpMsg << (u16)0x3C01;     // This value IS wrong way, so that nothing can happen at all

                    UDPMessages.insert(std::make_pair(mLastUDPID, tmpMsg));
                    tmpOffset--;

                    Console->Print("Added fake Udp message with ID %d to history buffer. New offset is: %d (Remaining fake packets to add: %d)", mLastUDPID, tmpOffset, (tmpOffset - 1));
                }
            }
            else if(tmpOffset < 1)
            {
                // If we're honest, this cant happen. But well, better cover even the worst of all cases
                Console->Print("%s Sending 0x03 commandset without preincreased UDP_ID!", Console->ColorText(YELLOW, BLACK, "[WARNING]"));
                return;
            }

            // Insert new message. The UDP_ID is stored as index-value, so be able to search for it
            UDPMessages.insert(std::make_pair(tmpUDPID, nMsg));
            mLastUDPID++;       // Could also be mLastUDPID = tmpUDPID, but its a bit more "safer" to just increment it
            //Console->Print("Added Udp message with ID %d to queue", mLastUDPID);
        }
    }
}

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

	udp_terminal.cpp - decoder classes for UDP terminal related messages

	CREATION: 8 Jan 2007 Namikon

	MODIFIED:
	REASON: -

*/

#include "main.h"
#include "udp_terminal.h"
#include "terminal.h"
/*******************************************************************************************/
/**** PUdpReceiveDB ****/
/*******************************************************************************************/
PUdpReceiveDB::PUdpReceiveDB(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x17";
}

PUdpMsgAnalyser* PUdpReceiveDB::Analyse()
{
    mDecodeData->mName << "=ReceiveDB request from client";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpReceiveDB::DoAction()
{
    Console->Print("ReceiveDB request from client");
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
/*******************************************************************************************/
/**** PUdpUpdateDB ****/
/*******************************************************************************************/
PUdpUpdateDB::PUdpUpdateDB(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x18";
}

PUdpMsgAnalyser* PUdpUpdateDB::Analyse()
{
    mDecodeData->mName << "=UpdateDB request from client";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpUpdateDB::DoAction()
{
    Console->Print("UpdateDB request from client");
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
/*******************************************************************************************/
/**** PUdpTryAccessDB ****/
/*******************************************************************************************/
PUdpTryAccessDB::PUdpTryAccessDB(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x19";
}

PUdpMsgAnalyser* PUdpTryAccessDB::Analyse()
{
    mDecodeData->mName << "=TryAccess request from client";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpTryAccessDB::DoAction()
{
    // Note:
    // For some reason, the lenght bytes for the string values arent always correct.
    // So i decided to get the lenght from the string itself. Should do the trick..
    // However, its strange. On retail servers those values seems to match always...

    PClient* tClient = mDecodeData->mClient;
    PMessage* tMessage = mDecodeData->mMessage;
    u8 tSubMsgStart = mDecodeData->Sub0x13Start;
    char tArea[100];
    char tOption1[100];
    char tOption2[100];
    char tOption3[100];

    u8 tMsgLen = 0;
    u16 tCmdNr = 0;
    u16 tCmdLen = 0;
    u16 tOpt1Len = 0;
    u16 tOpt2Len = 0;
    u16 tOpt3Len = 0;

    // Terminate all chars to prevent errors
    tArea[0] = '\0';
    tOption1[0] = '\0';
    tOption2[0] = '\0';
    tOption3[0] = '\0';

    // Get lenght of first string (command) and check if there is more data to grab
    tMsgLen = tMessage->U8Data(tSubMsgStart);
    tCmdLen = tMessage->U16Data(tSubMsgStart+22); // Doesnt work. Look at the note at the beginning of this func

    // Get commandnumber (Required for some checks
    tCmdNr = tMessage->U16Data(tSubMsgStart+20);

    // Get area to check from incomming packet
    strcpy(tArea, tMessage->GetMessageData()+tSubMsgStart+24);

    // Better way to get the lenght
    tCmdLen = strlen(tArea) + 1; // Dont forget the '\0' char at string-end

tMessage->Dump();
Console->Print("tMsglen %d tCmdLen %d tCmdNr %d area %s", tMsgLen, tCmdLen, tCmdNr, tArea);
    if(tMsgLen > (tCmdLen + 23)) // Packetlenght is greater than the message with packetdata, get option1
    {
        Console->Print("Got First option!");
        // This line assembles like this:
        // - tSubMsgStart points to the lenght byte of the packet (u8)
        // - 23 is the offset to add to reach the first string start
        // - tCmdLen is the lenght of the DCB* command
        // - 2 is the lenght byte (u16) of the second string

        //tOpt1Len = tMessage->U8Data(tSubMsgStart+24+tCmdLen);
        strcpy(tOption1, tMessage->GetMessageData()+tSubMsgStart+24+tCmdLen+2);
        tOpt1Len = strlen(tOption1) + 1; // Dont forget the '\0' char at string-end

        Console->Print("tOpt1Len %d tOption1 %s", tOpt1Len, tOption1);

        if(tMsgLen > (tCmdLen + 23 + +2 + tOpt1Len)) // Check if the packet is still longer than the data we grabbed yet
        {
            Console->Print("Got second option!");
            // This line assembles like this:
            // - tSubMsgStart points to the lenght byte of the packet (u8)
            // - 24 is the offset to add to reach the first string start
            // - tCmdLen is the lenght of the DCB* command
            // - 2 is the lenght byte (u16) of the second string
            // - tOpt1Len is the lenght of the second string

            //tOpt2Len = tMessage->U8Data(tSubMsgStart+24+tCmdLen+2+tOpt1Len);
            strcpy(tOption2, tMessage->GetMessageData()+tSubMsgStart+24+tCmdLen+2+tOpt1Len+2);
            tOpt2Len = strlen(tOption2) + 1; // Dont forget the '\0' char at string-end

            Console->Print("tOpt2Len %d tOption2 %s", tOpt2Len, tOption2);

            if(tMsgLen > (tCmdLen + 23 + 2 + tOpt1Len + 2 + tOpt2Len)) // Check if the packet is still longer than the data we grabbed yet
            {
                Console->Print("Got third option!");
                // This line assembles like this:
                // - tSubMsgStart points to the lenght byte of the packet (u8)
                // - 24 is the offset to add to reach the first string start
                // - tCmdLen is the lenght of the DCB* command
                // - 2 is the lenght byte (u16) of the second string
                // - tOpt1Len is the lenght of the second string

                //tOpt3Len = tMessage->U8Data(tSubMsgStart+24+tCmdLen+2+tOpt1Len+2+tOpt2Len);
                strcpy(tOption3, tMessage->GetMessageData()+tSubMsgStart+24+tCmdLen+2+tOpt1Len+2+tOpt2Len+2);
                tOpt3Len = strlen(tOption3) + 1; // Dont forget the '\0' char at string-end

                Console->Print("tOpt3Len %d tOption3 %s", tOpt3Len, tOption3);

                if(tMsgLen > (tCmdLen + 23 + 2 +tOpt1Len + 2 + tOpt2Len + 2 + tOpt3Len)) // Check if the packet is STILL longer than the data we grabbed yet
                {
                    Console->Print("%s TryAccess command with more than 3 options received", Console->ColorText(RED, BLACK, "[Warning]"));
                }
            }
        }
    }

    if(gDevDebug) Console->Print("TryAccess request to area %s. Options: 1[%s] 2[%s] 3[%s]", tArea, tOption1, tOption2, tOption3);

    // Let the terminal class handle the request
    Terminal->CheckAccess(tClient, tArea, tCmdNr, tOption1, tOption2, tOption3);

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
/*******************************************************************************************/
/**** PUdpQueryDB ****/
/*******************************************************************************************/
PUdpQueryDB::PUdpQueryDB(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x1b";
}

PUdpMsgAnalyser* PUdpQueryDB::Analyse()
{
    mDecodeData->mName << "=QueryDB request from client";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpQueryDB::DoAction()
{
    Console->Print("QueryDB request from client");
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
/*******************************************************************************************/
/**** PUdpTeminal0x1f ****/
/*******************************************************************************************/
PUdpTeminal0x1f::PUdpTeminal0x1f(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdpTeminal0x1f::Analyse()
{
    mDecodeData->mName << "=Open Terminal";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpTeminal0x1f::DoAction()
{
    Console->Print("Open Terminal");
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

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
    PClient* tClient = mDecodeData->mClient;
    PMessage* tMessage = mDecodeData->mMessage;
    u8 tSubMsgStart = mDecodeData->Sub0x13Start;
    char tArea[100];

    // Get area to check from incomming packet
    strcpy(tArea, tMessage->GetMessageData()+tSubMsgStart+24);

    Console->Print("TryAccess request to area %s", tArea);
    u8 tSuccessFull = 0;

    if(Terminal->CheckAccess(tClient, tArea))
        tSuccessFull = 1;

    u8 i = (u8)strlen(tArea);
    Console->Print("MessageLengh: %d", i);

    tClient->IncreaseUDP_ID();
    PMessage* tmpMsg = new PMessage(i+17);

    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)tClient->GetUDP_ID();
    *tmpMsg << (u16)tClient->GetSessionID();
    *tmpMsg << (u8)(11+i);
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u16)tClient->GetUDP_ID();
    *tmpMsg << (u8)0x2b;
    *tmpMsg << (u8)0x1a;
    *tmpMsg << (u16)(i+1);
    *tmpMsg << (u8)tSuccessFull;
    *tmpMsg << (u8)0x00;
    *tmpMsg << (u8)0x00;
    *tmpMsg << tArea;

    tmpMsg->Dump();
    tClient->getUDPConn()->SendMessage(tmpMsg);

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

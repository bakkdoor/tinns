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

 udp_hack.cpp - decoder classes for UDP hacking related messages

 CREATION: 30 Dec 2006 Namikon

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_hack.h"

/**** PUdpHackInit ****/

PUdpHackInit::PUdpHackInit(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x00";
}

PUdpMsgAnalyser* PUdpHackInit::Analyse()
{
    mDecodeData->mName << "=Hackgame announced";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackInit::DoAction()
{
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

/**** PUdpHackStart ****/

PUdpHackStart::PUdpHackStart(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x20";
}

PUdpMsgAnalyser* PUdpHackStart::Analyse()
{
    mDecodeData->mName << "=Start hackgame";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackStart::DoAction()
{
    u32 mRawItemID = mDecodeData->mMessage->U32Data(mDecodeData->Sub0x13Start+24);
    // TODO: Start hackgame here, depending on itemID
    if(gDevDebug) Console->Print("Client %d wants to hack itemID %d ***not managed yet***", mDecodeData->mClient->GetID(), mRawItemID);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

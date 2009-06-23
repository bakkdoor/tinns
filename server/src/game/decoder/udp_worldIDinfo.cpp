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

 udp_worldIDinfo.cpp - decoder classes for UDP "Request for more information about WorldID xx"

 CREATION: 20 Jun 2009 Namikon

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_worldIDinfo.h"

/**** PWorldIDInfoReq ****/

PWorldIDInfoReq::PWorldIDInfoReq(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x27";
}

PUdpMsgAnalyser* PWorldIDInfoReq::Analyse()
{
    mDecodeData->mName << "=WorldID Info Req.";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PWorldIDInfoReq::DoAction()
{
    // TODO: Handle client request here!
    // Note: It *seems* that the client sends this "eek, giev info about ID xxx"
    // if the NPC/WorldID is "not in the world", means the coods are pointing
    // the NPC somewhere below ground or inside a wall. However, not 100% sure about that
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

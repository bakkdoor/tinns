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

 udp_pvptrade.cpp - decoder classes for UDP "Request for more information about WorldID xx"

 CREATION: 20 Jun 2009 Namikon

 MODIFIED: 4 Jul 2009 Hammag
 REASON: Corrected class name

*/

#include "main.h"
#include "udp_pvptrade.h"

/**** PPvPTrade ****/

PUdpPvPTrade::PUdpPvPTrade(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x3e";
}

PUdpMsgAnalyser* PUdpPvPTrade::Analyse()
{
    mDecodeData->mName << "=Switch PvP Trading";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpPvPTrade::DoAction()
{
    // TODO: Handle client PvP trade switch on/off
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

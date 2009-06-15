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
	udp_0x08.cpp - decoder classes for UDP 0x08 messages

	CREATION: 05 Jan 2007 Namikon

	MODIFIED:
	REASON: -
*/

#include "main.h"
#include "udp_0x08.h"


PUdp0x08::PUdp0x08(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x08";
}

PUdpMsgAnalyser* PUdp0x08::Analyse()
{
    mDecodeData->mName << "=Client crash";
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

    return this;
}

bool PUdp0x08::DoAction()
{
    // Client crashed, close connection from our side
    GameServer->ClientDisconnected(mDecodeData->mClient);
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;

    return true;
}

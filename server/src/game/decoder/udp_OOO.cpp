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

	udp_OOO.cpp - decoder classe for UDP Out Of Order message

	CREATION: 5 Sep 2006 Hammag

	MODIFIED:
	REASON: -

*/

#include "main.h"
#include "udp_OOO.h"

/**** PUdpOOO ****/

PUdpOOO::PUdpOOO(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x01";
}

PUdpMsgAnalyser* PUdpOOO::Analyse()
{
  mDecodeData->mName << "=Out Of Order";
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

  return this;
}

bool PUdpOOO::DoAction()
{
    u16 MissingUDP_ID = mDecodeData->mMessage->U16Data(mDecodeData->Sub0x13Start+5);

    mDecodeData->mClient->getUDPConn()->ReSendUDPMessage(MissingUDP_ID);
    //Console->Print("%s Out of Order packet received ! (Client is missing UDPID %d) ***not managed yet***", Console->ColorText(YELLOW, BLACK, "[Notice]"), MissingUDP_ID);

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

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

	udp_appartment.cpp - decoder classes for Navray apt location request messages

	CREATION: 23 Dec 2006 Namikon

	MODIFIED: 13 may 2007 Hammag
	REASON: corrected this header

*/

#include "main.h"
#include "udp_appartment.h"

/**** PUdpAptLocInfo ****/

PUdpAptLocInfo::PUdpAptLocInfo(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x0a";
}

PUdpMsgAnalyser* PUdpAptLocInfo::Analyse()
{
  mDecodeData->mName << "=Requesting appartment location for NavRay";

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpAptLocInfo::DoAction()
{
Console->Print("Got request for app data");
  PMessage* tmpMsg = MsgBuilder->BuildCharAptLocInfoMsg (mDecodeData->mClient);

  if (tmpMsg)
    mDecodeData->mClient->getUDPConn()->SendMessage(tmpMsg);

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}

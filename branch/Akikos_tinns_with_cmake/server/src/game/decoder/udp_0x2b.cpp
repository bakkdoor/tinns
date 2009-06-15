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

	udp_udp0x2b.cpp - decoder classes for UDP 0x13x03x2b messages

	CREATION: 8 Jan 2007 Namikon

	MODIFIED:
	REASON: -

*/

#include "main.h"
#include "udp_0x2b.h"
#include "udp_terminal.h"

/**** PUdp0x2b ****/

PUdp0x2b::PUdp0x2b(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x2b";
}

PUdpMsgAnalyser* PUdp0x2b::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = NULL;
  mDecodeData->mState = DECODE_MORE;
  u8 MsgType = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 5);
  switch(MsgType)
  {
    case 0x17: // ReceiveDB Terminal Command
    {
      nextAnalyser = new PUdpReceiveDB(mDecodeData);
      break;
    }
    case 0x18: // UpdateDB Terminal Command
    {
      nextAnalyser = new PUdpUpdateDB(mDecodeData);
      break;
    }
    case 0x19: // TryAccess Terminal Command
    {
      nextAnalyser = new PUdpTryAccessDB(mDecodeData);
      break;
    }
    case 0x1b: // DB Query and Command
    {
      nextAnalyser = new PUdpQueryDB(mDecodeData);
      break;
    }
    case 0x1f: // Citycom?
    {
      nextAnalyser = new PUdpTeminal0x1f(mDecodeData);
      break;
    }

    default:
    {
      mDecodeData->mUnknownType = MsgType;
      break;
    }
  }

  if (! nextAnalyser)
  {
    nextAnalyser = new PUdpMsgUnknown(mDecodeData);
  }

  return nextAnalyser;
}

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

	udpanalyser.cpp - Analyser class top (used only by PUdpMsgDecoder)
  Also includes the PUdpMsgUnknown derived class

	CREATION: 23 Aug 2006 Hammag

	MODIFIED: 30 Aug 2006 Hammag
	REASON: - moved these two class in file distinct from PUdpMsgDecoder.
	        - made implementation

*/

#include "main.h"
#include "udpanalyser.h"

#include "udp_sync.h"
#include "udp_0x13.h"
#include "udp_0x08.h"

/**** PUdpMsgAnalyser ****/

int PUdpMsgAnalyser::smAnaCount = 0; // temp for check

PUdpMsgAnalyser::PUdpMsgAnalyser(PMsgDecodeData* nDecodeData)
{
  mDecodeData = nDecodeData;
  if (++smAnaCount > 2) // temp for check
    Console->Print(RED, BLACK, "Analyser instances count: %d", smAnaCount);  // temp for check
}

PUdpMsgAnalyser::~PUdpMsgAnalyser()
{
  --smAnaCount;
}

PUdpMsgAnalyser* PUdpMsgAnalyser::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser;
  u8 MsgType;

  mDecodeData->mState = DECODE_MORE;
//mDecodeData->mTraceKnownMsg = true; // Don't want to trace all known messages
mDecodeData->mTraceUnknownMsg = true; // Want to show all unknown messages

  *(mDecodeData->mMessage) >> MsgType;
  switch(MsgType)
  {
    case 0x01:
    {
      nextAnalyser = new PUdpSync0(mDecodeData);
      break;
    }
    case 0x03:
    {
      nextAnalyser = new PUdpSync1(mDecodeData);
      break;
    }
    case 0x08:
    {
        nextAnalyser = new PUdp0x08(mDecodeData);
        break;
    }
    case 0x13:
    {
      nextAnalyser = new PUdp0x13(mDecodeData);
      break;
    }
    default:
    {
      mDecodeData->mUnknownType = MsgType;
      nextAnalyser = new PUdpMsgUnknown(mDecodeData);
      break;
    }
  }
 return nextAnalyser;
}

bool PUdpMsgAnalyser::DoAction()
{
  return false; // no action at this level
}


/**** Unkown UDP message ****/

PUdpMsgUnknown::PUdpMsgUnknown(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  char hexstr[16];

  nDecodeData->mState = DECODE_UNKNOWN;
  snprintf(hexstr, 16, "/0x%02x", mDecodeData->mUnknownType);
  mDecodeData->mName << hexstr;
  nDecodeData->mName << "=Unknown";
}

/*PUdpMsgUnknown::~PUdpMsgUnknown()
{
}*/

/*bool PUdpMsgUnknown::DoAction()
{
  return false;
}*/

/**** Ignore UDP message ****/

PUdpMsgIgnore::PUdpMsgIgnore(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  char hexstr[16];

  nDecodeData->mState = DECODE_FINISHED;
  snprintf(hexstr, 16, "/0x%02x", mDecodeData->mUnknownType);
  mDecodeData->mName << hexstr;
  nDecodeData->mName << "=Ignore";
}

/*PUdpMsgUnknown::~PUdpMsgUnknown()
{
}*/

/*bool PUdpMsgUnknown::DoAction()
{
  return false;
}*/

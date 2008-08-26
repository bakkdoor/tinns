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

	udp_udp0x22.cpp - decoder classes for UDP 0x13x03x22 messages
  
	CREATION: 6 Sep 2006 Hammag

	MODIFIED:
	REASON: - 

*/

#include "main.h"
#include "udp_0x22.h"

#include "udp_reqinfo.h"
#include "udp_zoning.h"
#include "udp_entityposreq.h"

/**** PUdp0x22 ****/

PUdp0x22::PUdp0x22(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x22";
} 

PUdpMsgAnalyser* PUdp0x22::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = NULL;
  mDecodeData->mState = DECODE_MORE;    
  u8 MsgType = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 5); 
  switch(MsgType)
  {
    case 0x03: // Zoning phase 2
    {       
      nextAnalyser = new PUdpZoning2(mDecodeData);
      break;
    }
    case 0x06: // Char/Clan/Rank/Map Info request
    {       
      nextAnalyser = new PUdpReqInfo(mDecodeData);
      break;
    }
    case 0x0b: // Entity position request
    {
      nextAnalyser = new PUdpEntityPosRequest(mDecodeData);
      break;
    } 
    case 0x0d: // Zoning phase 1
    {        
      nextAnalyser = new PUdpZoning1(mDecodeData);
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

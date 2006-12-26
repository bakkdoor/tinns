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

	udp_udp0x1f.cpp - decoder classes for UDP 0x13x03x1f messages

	CREATION: 6 Sep 2006 Hammag

	MODIFIED:
	REASON: -

*/

#include "main.h"
#include "udp_0x1f.h"

#include "udp_zoning.h"
#include "udp_charmove.h"
#include "udp_vhc.h"
#include "udp_subskill.h"
#include "udp_chat.h"
#include "udp_useobject.h"
#include "udp_appartment.h"

/**** PUdp0x1f ****/

PUdp0x1f::PUdp0x1f(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdp0x1f::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser = NULL;
  mDecodeData->mState = DECODE_MORE;
  u8 MsgType = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 7);
  u8 MsgSubType = mDecodeData->mMessage->U8Data(mDecodeData->Sub0x13Start + 8);

  switch(MsgType)
  {
    case 0x02:
    {
      nextAnalyser = new PUdpCharJump(mDecodeData);
      break;
    }
    case 0x17:
    {
      nextAnalyser = new PUdpUseObject(mDecodeData);
      break;
    }
    case 0x1b:
    {
      nextAnalyser = new PUdpChatLocal(mDecodeData);
      break;
    }
    /* case 0x1e:  // Inventory item move
    {
      nextAnalyser = new PUdpXXXX(mDecodeData);
      break;
    }
    case 0x1f: // Slot use
    {
      nextAnalyser = new PUdpXXXX(mDecodeData);
      break;
    } */
    case 0x22:
    {
      nextAnalyser = new PUdpCharExitChair(mDecodeData);
      break;
    }
    case 0x25:
    {
      mDecodeData->mName << "/0x25";
      if (MsgSubType == 0x04) // Is it sure this is a requirement ?
      {
        nextAnalyser = new PUdpSubskillInc(mDecodeData);
      }
      else
      {
        mDecodeData->mUnknownType = MsgSubType;
        mDecodeData->mTraceUnknownMsg = true;
      }
      break;
    }
    case 0x33:
    {
      nextAnalyser = new PUdpChatListAdd(mDecodeData);
      break;
    }
    case 0x38:
    {
      nextAnalyser = new PUdpAppartmentAccess(mDecodeData);
      break;
    }
    case 0x39:
    {
      nextAnalyser = new PUdpChatListRemove(mDecodeData);
      break;
    }
    case 0x3b:
    {
      nextAnalyser = new PUdpChatGlobal(mDecodeData);
      break;
    }
    case 0x3d:
    {
      mDecodeData->mName << "/0x3d";
      switch(MsgSubType)
      {
        case 0x02:
        {
          nextAnalyser = new PUdpAddGenrepToList(mDecodeData);
          break;
        }
        case 0x03:
        {
          nextAnalyser = new PUdpAptGRZoning(mDecodeData);
          break;
        }
        case 0x04:
        {
          nextAnalyser = new PUdpGenrepZoning(mDecodeData);
          break;
        }
        case 0x0a:
        {
          nextAnalyser = new PUdpAptLocInfo(mDecodeData);
          break;
        }
        case 0x0f:
        {
          nextAnalyser = new PUdpVhcUse(mDecodeData);
          break;
        }
        default:
        {
          mDecodeData->mUnknownType = MsgSubType;
          break;
        }
      }
      break;
    }
    case 0x4c:
    {
      nextAnalyser = new PUdpChatChannels(mDecodeData);
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

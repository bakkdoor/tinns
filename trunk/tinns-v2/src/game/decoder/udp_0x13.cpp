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
	udp_0x13.cpp - decoder classes for UDP 0x13 messages

	CREATION: 31 Aug 2006 Hammag

	MODIFIED:
	REASON: -
*/

#include "main.h"
#include "udp_0x13.h"

#include "udp_OOO.h"
#include "udp_zoning.h"
#include "udp_0x1f.h"
#include "udp_0x22.h"
#include "udp_ping.h"
#include "udp_sync.h"
#include "udp_charmove.h"
#include "udp_packet0.h"
#include "udp_vhc.h"
#include "udp_0x2b.h"

/**** PUdp0x13 ****/

PUdp0x13::PUdp0x13(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x13";
}

PUdpMsgAnalyser* PUdp0x13::Analyse()
{
  PMessage* TmpMsg = mDecodeData->mMessage;
  PUdpMsgAnalyser* nextAnalyser = NULL;

  if (! mDecodeData->mHandling0x13Sub) // First decoding pass
  {
//Console->Print(" --- New 0x13 msg");
//TmpMsg->Dump();
  	(*TmpMsg) >> (mDecodeData->mClientState->UDP.mSequence); // "LastPacket"
  	(*TmpMsg) >> (mDecodeData->mClientState->UDP.mServerPacketNum); //PID
  	mDecodeData->Sub0x13StartNext = 0;
  }

  if(TmpMsg->EOM())
  {
Console->Print(RED, BLACK, "PUdp0x13::Analyse(): Emptied 0x13 msg handling !!!");
    mDecodeData->mState = DECODE_FINISHED;
    mDecodeData->mHandling0x13Sub = false;
    nextAnalyser = this;
    mDecodeData->Sub0x13Start = mDecodeData->Sub0x13StartNext = 0;
  }
  else
  {
    if (mDecodeData->mHandling0x13Sub && mDecodeData->Sub0x13StartNext)
    {
        TmpMsg->SetNextByteOffset(mDecodeData->Sub0x13StartNext);
//Console->Print("Multi 0x13 msg continuing at %d", TmpMsg->GetNextByteOffset());
    }
    mDecodeData->Sub0x13Start = TmpMsg->GetNextByteOffset();
    u8 PSize = TmpMsg->U8Data(mDecodeData->Sub0x13Start);
    u16 EndOffset = mDecodeData->Sub0x13StartNext = mDecodeData->Sub0x13Start + 1 + PSize;

    if (EndOffset >= TmpMsg->GetSize())
    {
      mDecodeData->mHandling0x13Sub = false;
      //mDecodeData->Sub0x13Start = mDecodeData->Sub0x13StartNext = 0;
//Console->Print("Simple 0x13 msg");
      if (EndOffset > TmpMsg->GetSize())
      {
Console->Print(RED, BLACK, "PUdp0x13::Analyse(): Size error in 0x13 msg handling !!!");
        mDecodeData->mState = DECODE_ERROR;
        mDecodeData->mErrorDetail = "Submessage too long.";
        return this;
      }
    }
    else
    {
//Console->Print("Multi 0x13 msg. End at %d for total length %d", EndOffset, TmpMsg->GetSize());
      mDecodeData->mHandling0x13Sub = true;
    }

    mDecodeData->mState = DECODE_MORE;
    u8 MsgType = TmpMsg->U8Data(mDecodeData->Sub0x13Start + 1);
    /*u16 PSeq = 0; // This will have to be handled to detected missing granted-delivery messages
		if (MsgType == 0x03)
			PSeq = *(u16*)&Buf[Offset+2];*/
//Console->Print("0x13 Type: %d", MsgType);
    switch(MsgType)
    {
      case 0x03:
      {
        mDecodeData->mName << "/0x03";
        u8 MsgSubType = TmpMsg->U8Data(TmpMsg->GetNextByteOffset()+4);
        switch(MsgSubType)
        {
          case 0x01: // Out of order
          {
            nextAnalyser = new PUdpOOO(mDecodeData);
            break;
          }
          case 0x08: // Client zoning completed (!!! does not happen on login)
          {
            nextAnalyser = new PUdpEndOfZoning(mDecodeData);
            break;
          }
          case 0x1f:
          {
            nextAnalyser = new PUdp0x1f(mDecodeData);
            break;
          }
          case 0x22:
          {
            nextAnalyser = new PUdp0x22(mDecodeData);
            break;
          }
          case 0x2b: // Citycom (General Terminal?)
          {
            nextAnalyser = new PUdp0x2b(mDecodeData);
            break;
          }
          default:
          {
            mDecodeData->mUnknownType = MsgSubType;
            break;
          }
          /*default: // Temporary
          {
            nextAnalyser = new PUdp0x13Old(mDecodeData);
            break;
          }*/
        }
        break;
      }

      case 0x0b: // Ping
      {
        nextAnalyser = new PUdpPing(mDecodeData);
        break;
      }

      case 0x0c: // Baseline
      {
        nextAnalyser = new PUdpSync2(mDecodeData);
        break;
      }

      case 0x20: // Char move
      {
        mDecodeData->mName << "/0x20";
        u8 MsgSubType = TmpMsg->U8Data(TmpMsg->GetNextByteOffset()+4);
mDecodeData->mTraceUnknownMsg = false; // temp stop being bugged with unknown move msg
        switch(MsgSubType)
        {
          case 0x20:
          {
            nextAnalyser = new PUdpCharAttitudeUpdate(mDecodeData);
            break;
          }
          case 0x7f:
          {
            nextAnalyser = new PUdpCharPosUpdate(mDecodeData);
            break;
          }
          case 0x80:
          {
            nextAnalyser = new PUdpCharSitting(mDecodeData);
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

      case 0x27: // sent when char sit. Same occurence but less info than 0x13/0x20/0x80
      {
        mDecodeData->mUnknownType = MsgType;
        nextAnalyser = new PUdpMsgIgnore(mDecodeData); // Silently ignore this message
        break;
      }

      case 0x2a: // "Packet0"
      {
        nextAnalyser = new PUdpPacket0(mDecodeData);
        break;
      }

      case 0x2d: // sent when targeting another char
      {
        nextAnalyser = new PUdpCharTargeting(mDecodeData);
        break;
      }

      case 0x32: // Vhc move
      {
        nextAnalyser = new PUdpVhcMove(mDecodeData);
        break;
      }

      default:
      {
        mDecodeData->mUnknownType = MsgType;
        break;
      }

      /*default: // Temporary
      {
        nextAnalyser = new PUdp0x13Old(mDecodeData);
        break;
      }*/
    }

    if (! nextAnalyser)
    {
      nextAnalyser = new PUdpMsgUnknown(mDecodeData);
    }

  }
  return nextAnalyser;
}

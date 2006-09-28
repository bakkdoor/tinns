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

	msgdecoder.cpp - top class for NC messages decoding

	CREATION: 23 Aug 2006 Hammag

	MODIFIED:
	REASON: -

*/

#include "main.h"
#include "msgdecoder.h"

#include "udp_0x13.h"


// PUdpMsgDecoder

/*
    PMsgDecodeData mDecodeData;
    PUdpMsgAnalyser* mCurrentAnalyser;
*/
    
void PUdpMsgDecoder::Init(PMessage* nMessage, PClient* nClient)
{
  mDecodeData.mMessage = nMessage;
  mDecodeData.mClient = nClient;
  mDecodeData.mState = (nMessage && nClient) ? DECODE_MORE : DECODE_UNDEF;
  mDecodeData.mUnknownType = 0;
  mDecodeData.mHandling0x13Sub = false;
	mDecodeData.Sub0x13Start = mDecodeData.Sub0x13StartNext = 0;
  mPacketName.erase();
  mDecodeData.mName.str(mPacketName);
  mDecodeData.mErrorDetail.erase();
	mDecodeData.mTraceKnownMsg = false;
	mDecodeData.mTraceUnknownMsg = false;
	mDecodeData.mTraceDump = false;
  if (mCurrentAnalyser)
  {
    delete mCurrentAnalyser;
    mCurrentAnalyser = NULL;
  }
}
  
PUdpMsgDecoder::PUdpMsgDecoder()
{
  mCurrentAnalyser = NULL;
  Reset();
}

PUdpMsgDecoder::PUdpMsgDecoder(PMessage* nMessage, PClient* nClient)
{
  mCurrentAnalyser = NULL;
  Init(nMessage, nClient);
}

PUdpMsgDecoder::~PUdpMsgDecoder()
{
  if (mCurrentAnalyser)
  {
    delete mCurrentAnalyser;
  }
}

bool PUdpMsgDecoder::Analyse()
{
  PUdpMsgAnalyser* nextAnalyser;

  if ((mDecodeData.mState & DECODE_MORE) || mDecodeData.mHandling0x13Sub)
  {
    if (mCurrentAnalyser)
    {
      delete mCurrentAnalyser;
      mCurrentAnalyser = NULL;
    }
    if (mDecodeData.mHandling0x13Sub)
    {
      mPacketName.erase();
      mDecodeData.mName.str(mPacketName);
      mCurrentAnalyser = new PUdp0x13(&mDecodeData);
      mDecodeData.mState = DECODE_MORE;
    }
    else
    {
      mCurrentAnalyser = new PUdpMsgAnalyser(&mDecodeData);
    }

    while (mDecodeData.mState & DECODE_MORE)
    {
      nextAnalyser = mCurrentAnalyser->Analyse();
      if (mCurrentAnalyser != nextAnalyser)
      {
        delete mCurrentAnalyser;
        mCurrentAnalyser = nextAnalyser;
      }
    }
  }
  return (!(mDecodeData.mState & (DECODE_MORE | DECODE_ERROR | DECODE_UNKNOWN)));
}

bool PUdpMsgDecoder::Analyse(PMessage* nMessage, PClient* nClient)
{
  Init(nMessage, nClient);
  return Analyse();
}

bool PUdpMsgDecoder::DoAction()
{
  if (mDecodeData.mState & DECODE_ACTION_READY)
  {
    return mCurrentAnalyser->DoAction();
  }
  else
  {
    return false;
  }
}

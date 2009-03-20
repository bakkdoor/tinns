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

 udp_outfitter.cpp - decoder classes for UDP outfitter related messages

 CREATION: 20 Mar 2009 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_outfitter.h"


/**** PUdpOutfitter ****/

PUdpOutfitter::PUdpOutfitter(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x2e";
}

PUdpMsgAnalyser* PUdpOutfitter::Analyse()
{
    mDecodeData->mName << "=Outfitter validation";

	u16 tUnknown;
	PMessage* nMsg = mDecodeData->mMessage;
	nMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 8);

	(*nMsg) >> mSkin;
	(*nMsg) >> tUnknown;
	(*nMsg) >> mHead;
	(*nMsg) >> mTorso;
	(*nMsg) >> mLegs;
	(*nMsg) >> mHeadColor;
	(*nMsg) >> mTorsoColor;
	(*nMsg) >> mLegsColor;
	(*nMsg) >> mHeadDarkness;
	(*nMsg) >> mTorsoDarkness;
	(*nMsg) >> mLegsDarkness;

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpOutfitter::DoAction()
{
	PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();
	
	/* Some validity checks against char model and equipment can be done here */
	/* as well as payment */

	tChar->SetCurrentLook(mSkin, mHead, mTorso, mLegs);
	tChar->SetCurrentBodyColor(mHeadColor, mTorsoColor, mLegsColor, mHeadDarkness, mTorsoDarkness, mLegsDarkness);

    PMessage* tmpMsg = MsgBuilder->BuildCharHelloMsg(nClient);
    ClientManager->UDPBroadcast(tmpMsg, nClient);
	

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
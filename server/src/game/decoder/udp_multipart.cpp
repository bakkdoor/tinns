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

 udp_multipart.cpp - decoder classes for UDP multipart messages (0x07)

 CREATION: 25 Oct 2009 Namikon

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_multipart.h"

/**** PUdpMultiPart ****/

PUdpMultiPart::PUdpMultiPart( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x07";
}

PUdpMsgAnalyser* PUdpMultiPart::Analyse()
{
    mDecodeData->mName << "=Multipart Message";

    PMessage* TmpMsg = mDecodeData->mMessage;

    u8 tChunkSize = 0;

    ( *TmpMsg ) >> tChunkSize;
    TmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 5 );
    ( *TmpMsg ) >> mChunkNr;
    ( *TmpMsg ) >> mChunkTotal;
    ( *TmpMsg ) >> mSequence;

    tChunkSize -= 9;
    int tCurPos = mDecodeData->Sub0x13Start + 10;

    mChunk = new PMessage(tChunkSize);
    mChunk = TmpMsg->GetChunk(tCurPos, tChunkSize);

    //if (gDevDebug) Console->Print("Received MULTIPART fragment. ChunkNr: %d of %d, SequenceNr: %d ChunkSize: %d", mChunkNr, mChunkTotal, mSequence, mChunk->GetSize());
    //if (gDevDebug) mChunk->Dump();

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpMultiPart::DoAction()
{
    if ( mDecodeData->mState & DECODE_ACTION_READY )
    {
        MultiPartHandler->AddMultiPartChunk(mDecodeData->mClient, mChunk, mChunkNr, mChunkTotal, mSequence);
        return true;
    }
    else
        return false;
}

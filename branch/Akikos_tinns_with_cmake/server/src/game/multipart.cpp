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
 multipart.cpp - Handling of incomming multipart messages

 CREATION: 31 Aug 2006 Hammag

 MODIFIED:
 REASON: -
*/

#include "main.h"

PMultiPart::PMultiPart()
{
}

PMultiPart::~PMultiPart()
{

}

void PMultiPart::AssembleChunk(u16 nSequence)
{
    // Get the sequence number
    PMultipartMap::iterator it;
    it = MsgMap.find(nSequence);

    // Prepare messagebuffer
    PMessage* tmpMsg = new PMessage();

    // Get number of chunks in this sequence
    int tNumChunks = it->second.smChunkTotal;
    int tCurrentChunk = 0;

    vecMsgChunk::iterator vIt;
    bool tLoopDetect = false; // Dont go into an endless loop!

    // Assemble multiframe message. And (if required) put them together in correct order!
    while(tCurrentChunk < tNumChunks)
    {
        tLoopDetect = false;
        for (vIt = it->second.smvChunk.begin(); vIt != it->second.smvChunk.end(); vIt++)
        {
            // Is entry Nr. x the next chunk number we need to assemble?
            if(it->second.smvChunk[tCurrentChunk].smChunkNr == tCurrentChunk)
            {
                // Push chunk into buffer
                *tmpMsg << *(it->second.smvChunk[tCurrentChunk].smChunk);
                // proceed with next chunk
                tCurrentChunk++;

                // Break here if we have assembled all chunks
                if(tCurrentChunk == tNumChunks)
                    break;

                tLoopDetect = true;
            }
        }
        if(tLoopDetect == false)
        {
            Console->Print("%s [PMultiPart::AssembleChunk] Unable to assemble Sequence %d. Missing ChunkNumer %d", Console->ColorText(RED, BLACK, "ERROR"), it->first, tCurrentChunk);
            delete tmpMsg;
            it->second.smTimeStamp = 0;
        }
    }
    if (gDevDebug) Console->Print("%s [PMultiPart::AssembleChunk] Assembling of Sequence %d was a success", Console->ColorText(GREEN, BLACK, "Success"), it->first);
    it->second.smTimeStamp = 0;
    //tmpMsg->Dump();

    // Note: At this point, the reassembled message should be pushed back to the messagedecoder. But since the size of the packets there is always u8
    // it wont work with these oversized packets.
    // I decided to process the Assembled packet right here, since the only function in clients known to send oversized frames is UpdateDB
    ProcessPacket(it->second.smClient, tmpMsg);
}

void PMultiPart::ProcessPacket(PClient *nClient, PMessage *tmpMsg)
{
    mCommandName = "";
    for(int i = 0; i < mMaxOptions; i++)
        mOptions[i] = "";
    mOptionsCount = 0;
    mDBId = 0;

    // Make sure we have an UpdateDB call, otherwise break and error
    u8 MsgSubType = tmpMsg->U8Data(0);
    u8 TerminalAction = tmpMsg->U8Data(1);
    if(MsgSubType != 0x21 || TerminalAction != 0x18)
    {
        Console->Print("%s [PMultiPart::ProcessPacket] Unsupportet multipart message. Type %02x", Console->ColorText(RED, BLACK, "ERROR"), MsgSubType);
        return;
    }

    u16 tCmdLen, Unknown3, OptionSize;

    tmpMsg->SetNextByteOffset( 14 );
    ( *tmpMsg ) >> tCmdLen;
    ( *tmpMsg ) >> mDBId;
    ( *tmpMsg ) >> Unknown3;
    ( *tmpMsg ) >> mCommandName; // null terminated string

    mOptionsCount = 0;
    while (( tmpMsg->GetNextByteOffset() < tmpMsg->GetSize()  ) && ( mOptionsCount < mMaxOptions ) )
    {
        ( *tmpMsg ) >> OptionSize;
        if (( tmpMsg->GetNextByteOffset() < tmpMsg->GetSize() ) && OptionSize )
        {
            ( *tmpMsg ) >> mOptions[mOptionsCount++];
            //if(mOptions[mOptionsCount-1].size() != (OptionSize-1)) Warning (but no pb)!
        }
        else
        {
            break;
        }
    }
    /*if ( gDevDebug )
    {
        Console->Print( "%s Fragmented UpdateDB request from client; DBID: %d", Console->ColorText( YELLOW, BLACK, "[DEBUG]" ), mDBId );
        Console->Print( "%s Command: '%s'", Console->ColorText( YELLOW, BLACK, "[DEBUG]" ), mCommandName.c_str() );
        for ( u8 i = 0; i < mOptionsCount; ++i )
            Console->Print( "%s Option %d: '%s'", Console->ColorText( YELLOW, BLACK, "[DEBUG]" ), i, mOptions[i].c_str() );
    }*/
    Terminal->HandleUpdateDB(nClient, 0, &mCommandName, mOptions, mOptionsCount, mDBId, 0);
}

void PMultiPart::Update()
{
    for ( PMultipartMap::iterator i = MsgMap.begin(); i != MsgMap.end(); i++ )
    {
        // Check if sequence is complete
        if(i->second.smChunkTotal == i->second.smvChunk.size())
        {
            if (gDevDebug) Console->Print("[PMultiPart::Update] Sequence number %d is complete. Assembling...", i->first);
            AssembleChunk(i->first); // timestamp value is set in this function to 0, so it gets deleted after automaticly
        }

        // Check if sequence is timed out
        if(i->second.smTimeStamp + MAX_SEQUENCE_LIFE < time(NULL))
        {
            if (gDevDebug) Console->Print("[PMultiPart::Update] Sequence number %d timed out. Deleting...", i->first);
            MsgMap.erase(i);
        }
    }
}

// Adds an multipart frame
void PMultiPart::AddMultiPartChunk(PClient *nClient, PMessage *nChunk, u16 nChunkNumber, u16 nChunkTotal, u8 nSequence)
{
    // Search for Sequence ID in our map
    PMultipartMap::iterator it;
    it = MsgMap.find(nSequence);

    if ( it != MsgMap.end() )
    {   // We've found our sequence buffer
        // Prepare the chunk
        s_MessageChunk tChunk;
        tChunk.smChunkNr = nChunkNumber;
        tChunk.smChunk = nChunk;

        // And push it into the vector
        it->second.smvChunk.push_back(tChunk);
        // Update lifetimer
        it->second.smTimeStamp = time(NULL);
    }
    else
    {
        // Prepare our chunk
        s_MessageChunk tChunk;
        tChunk.smChunkNr = nChunkNumber;
        tChunk.smChunk = nChunk;

        // Generate new Sequence entry
        s_SequenceEntry tSeq;
        // Push our temp chunk into the vector
        tSeq.smvChunk.push_back(tChunk);
        // Add current timestamp
        tSeq.smTimeStamp = time(NULL);
        // Set chunktotal
        tSeq.smChunkTotal = nChunkTotal;
        // Store client
        tSeq.smClient = nClient;

        // Finally, push that into our map
        MsgMap.insert(make_pair(nSequence, tSeq));
    }
}

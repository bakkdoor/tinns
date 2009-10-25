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
 multipart.h - Handling of incomming multipart messages

 CREATION: 31 Aug 2006 Hammag

 MODIFIED:
 REASON: -
*/

#ifndef MULTIPART_H
#define MULTIPART_H

#define MAX_SEQUENCE_LIFE   10 // In seconds

typedef struct // A single chunk of any multipart sequence
{
    u16 smChunkNr;
    //u16 smChunkTotal;
    PMessage* smChunk;
} s_MessageChunk;

typedef vector<s_MessageChunk> vecMsgChunk; // The vector of an specific multipart sequence, containing all chunks
typedef struct
{
    time_t smTimeStamp;      // To keep track of sequence's lifetimer
    u16 smChunkTotal;       // Total amount of chunks for this sequence
    vecMsgChunk smvChunk;       // The chunkvector
    PClient *smClient;      // Required to call terminal class
} s_SequenceEntry;

typedef map<u8, s_SequenceEntry> PMultipartMap; // Map of all vectors, indexed by sequencenumber <u8>


class PMultiPart
{
  private:
    PMultipartMap MsgMap;
    void AssembleChunk(u16 nSequence);
    void ProcessPacket(PClient *nClient, PMessage *tmpMsg);

// Required to process packets here
    static const u8 mMaxOptions = 7;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    u8 mOptionsCount;
    u16 mDBId;
// ---------------------------------

  public:
    PMultiPart();
    ~PMultiPart();

    void Update(); // Check for complete/timed out sequences
    // Adds an multipart frame
    void AddMultiPartChunk(PClient *nClient, PMessage *nChunk, u16 nChunkNumber, u16 nChunkTotal, u8 nSequence);
};

#endif
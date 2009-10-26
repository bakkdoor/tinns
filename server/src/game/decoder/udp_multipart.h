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

 udp_multipart.h - decoder classes for UDP multipart messages (0x07)

 CREATION: 25 Oct 2009 Namikon

 MODIFIED:
 REASON: -

*/

#ifndef UDPMULTIPART_H
#define UDPMULTIPART_H

class PUdpMultiPart : public PUdpMsgAnalyser
{
  private:
    u16 mChunkNr;
    u16 mChunkTotal;
    u8 mSequence;
    PMessage* mChunk;

  public:
    PUdpMultiPart( PMsgDecodeData* nDecodeData );
    //~PUdpPing();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

#endif
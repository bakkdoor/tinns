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

 udp_itemuse.h - decoder classes for UDP inventory item use messages

 CREATION: 9 May 2009 Hammag

 MODIFIED:
 REASON: -

*/

#ifndef ITEMUSE_H
#define ITEMUSE_H

class PUdpItemUse : public PUdpMsgAnalyser
{
/*  private:
    u16 mWeaponId;
    u32 mTargetRawItemID;
    u8 mUnknown2;
    u8 mTargetedHeight;
    u8 mScore; // ??? looks quite random...
*/
  public:
    PUdpItemUse( PMsgDecodeData* nDecodeData );
    //~PUdpItemUse();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

#endif

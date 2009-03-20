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

 udp_outfitter.h - decoder classes for UDP outfitter related messages

 CREATION: 20 Mar 2009 Hammag

 MODIFIED:
 REASON: -

*/

#ifndef UDPOUTFITTER_H
#define UDPOUTFITTER_H

class PUdpOutfitter : public PUdpMsgAnalyser
{
	private:
		u32 mSkin;
		u8 mHead;
		u8 mTorso;
		u8 mLegs;
		u8 mHeadColor;
		u8 mTorsoColor;
		u8 mLegsColor;
		u8 mHeadDarkness;
		u8 mTorsoDarkness;
		u8 mLegsDarkness;

    public:
        PUdpOutfitter(PMsgDecodeData* nDecodeData);
        //~PUdpOutfitter();
        PUdpMsgAnalyser* Analyse();
        bool DoAction();
};

#endif
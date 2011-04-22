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

 udp_helditemaction.h - decoder classes for held item related messages

 CREATION: 20 Mar 2009 Hammag

 MODIFIED:
 REASON: -

*/

#ifndef HELDITEMACTION_H
#define HELDITEMACTION_H

class PUdpHeldItemBasicAction : public PUdpMsgAnalyser
{
  public:
    PUdpHeldItemBasicAction( PMsgDecodeData* nDecodeData );
    //~PUdpHeldItemBasicAction();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpHeldItemAimedAction : public PUdpMsgAnalyser
{

  private:
    u16 mWeaponId;
    u32 mTargetRawItemID;
    u8 mAiming; // 0: minimal
    u8 mTargetedHeight; // 0: bottom to 26: top
    u8 mScore; // ??? looks quite random...

  public:
    PUdpHeldItemAimedAction( PMsgDecodeData* nDecodeData );
    //~PUdpHeldItemAimedAction();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpHeldItemLaunchingAction : public PUdpMsgAnalyser
{
  private:
    u16 mWeaponId;
    u16 mSourceY;
    u16 mSourceZ;
    u16 mSourceX;
    u8 mSourceUD;
    u8 mSourceLR;
    u16 mUnknown1;
    u32 mUnknown2; // client timestamp ? => TODO: compare with data from ping request
    u32 mTargetRawItemID;

  public:
    PUdpHeldItemLaunchingAction( PMsgDecodeData* nDecodeData );
    //~PUdpHeldItemLaunchingAction();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};


class PUdpItemAddonActivation : public PUdpMsgAnalyser
{
  private:
    u8 mAddonIdx;

  public:
    PUdpItemAddonActivation( PMsgDecodeData* nDecodeData );
    //~PUdpItemAddonActivation();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};
#endif

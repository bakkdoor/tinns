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
 def_vehiclesits.cpp

    CREATED: 04 Apr 2009 Hammag
*/

#include "main.h"
#include "def_vehiclesits.h"

PDefVhcSeat::PDefVhcSeat()
{
}

// Not implemented yet
bool PDefVhcSeat::LoadFromDef( PTokenList *Tokens ) { Tokens = Tokens; return false; }

/*
class PDefVhcSeat : public PDef
{
  private :
    //int mIndex; //Field 1
    int mType;
    std::string mName; //Field 3
    //BasePosX Y Z
    //BaseAngleX Y Z
    float mLeavePos[3]; //X Y Z - Fields 10-12
    float mLeaveAngle[3]; //X Y Z - Fields 13-15
    //FirePosX Y Z //Field 16
    //XLock Ylock MinX MaxX TurnSpeed ForceExternalCam ShowActor 
    //SitBone RotXBone RotYBone WeaponBone
    int mWeaponId; //Field 30
    int mTL;
    float mDamageFactor;
    //SitAnimType SitYOffset //Field 33
    //SitFlags

  public :
    PDefVhcSeat();
    //~PDefVhcSeat();
*/
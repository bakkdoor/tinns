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
 def_vehiclesits.h

    CREATED: 03 Apr 2009 Hammag
*/

#ifndef DEF_VEHICLESEATS_H
#define DEF_VEHICLESEATS_H

#include "def.h"

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

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetType() const { return mType; }
    inline const std::string &GetName() const { return mName; }
    inline float GetLeavePos(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < 3)) ? mLeavePos[nIdx] : 0) ; }
    inline float GetLeaveAngle(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < 3)) ? mLeaveAngle[nIdx] : 0) ; }
    inline int GetWeaponId() const { return mWeaponId; }
    inline int GetTL() const { return mTL; }
    inline float GetDamageFactor() const { return mDamageFactor; }
};
/*
mType:
0 - Driver			(Ground vhc)
1 - Gunner
2 - Passenger
3 - Driver/Gunner
4 - Pilot  			(Flying vhc)
5 - Pilot/Gunner		(Flying vhc)
6 - Pilot 			(Senkrechtstarter ????)

mFlags:
1 - Winkel f�r Spieler und Waffe wird auf Fahrzeugrichtung gelockt. Zielrichtung darf nur leicht von der Fahrzeugrichtung abweichen
2 - Spieler wird gelockt
4 - Waffe ist nicht um X Achse drehbar
8 - Waffe ist nicht um Y Achse drehbar
16 - Spielermodel um 180 Grad gedreht
*/
#endif


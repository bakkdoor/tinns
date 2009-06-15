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
 def_ammo.h

    CREATED: 02 Apr 2009 Hammag
*/

#ifndef DEF_AMMO_H
#define DEF_AMMO_H

#include "def.h"

class PDefAmmo : public PDef
{
  private :
    //int mIndex;
	int mDamageId; // related to damage.def
	int mWeaponShotId; // related to shots.def ?
	int mMagSize;
    int mShotId; // related to shots.def ??? strange: hardly ever set in ammo.def

  public :
    PDefAmmo();
    //~PDefAmmo();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetDamageId() const { return mDamageId; }
	inline int GetWeaponShotId() const { return mWeaponShotId; }
    inline int GetMagSize() const { return mMagSize; }
	inline int GetShotId() const { return mShotId; }
};

#endif

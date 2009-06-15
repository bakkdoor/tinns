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
 def_shots.h

    CREATED: 03 Apr 2009 Hammag
*/

#ifndef DEF_SHOTS_H
#define DEF_SHOTS_H

#include "def.h"

class PDefShot : public PDef
{
  private :
    //int mIndex;
    int mDamageId;
    int mMass;
    int mRadius;
    float mSpeed;
    //int mHitSound;
    //int mCustomType;
    //std::string mCustomFrameFx;
    //std::string mCustomHitFx;

  public :
    PDefShot();
    //~PDefShot();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetDamageId() const { return mDamageId; }
    inline int GetMass() const { return mMass; }
    inline int GetRadius() const { return mRadius; }
    inline int GetSpeed() const { return mSpeed; }
};

#endif


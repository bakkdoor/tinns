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
 def_weapons.cpp

    CREATED: 29 Mar 2009 Hammag
*/

#include "main.h"

PDefWeapon::PDefWeapon()
{
  mIndex = -1;
  mName = "unknown";
  mItemIndex = mAmmoUse = mPSIuse = mWeaponType = mDiscardable = mMaxRange = 0;
  mAggressiveWeapon = mAmmoStartFlags = mShotCnt = mBaseWeaponId = 0;
  mAmmoTypes[0] = mAmmoTypes[1] =mAmmoTypes[2] =mAmmoTypes[3] = 0;
  mAmmoTypes[4] = mAmmoTypes[5] =mAmmoTypes[6] =mAmmoTypes[7] = 0;
  mItemModGroup = mItemModGroupFlags = mRareWeaponFx = 0;
  mStaminaUse = mSkillFactor = mDamageMultiplicator = 0;
}

PDefWeapon::~PDefWeapon()
{
}

bool PDefWeapon::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        continue;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        mName = *i; break;
      case 3 :
        continue; //int mFpsmodel;
      case 4 :
        continue; //int mAttachmodel;
      case 5 :
        continue; //int mMunactor;
      case 6 :
        continue; //float mDroptime; // useful ?
      case 7 :
        mItemIndex = atoi( i->c_str() ); break;
      case 8 :
        continue; //int effectcolor[3]; // R,G,B
      case 9 :
        continue; //int dynamiclight;
      case 10 :
        continue; //float lighttime;
      case 11 :
        continue; //int soundindex;
      case 12 :
        continue; //float mShotTime; // useful ?
      case 13 :
        mAmmoUse = atoi( i->c_str() ); break;
      case 14 :
        mPSIuse = atoi( i->c_str() ); break;
      case 15 :
        mStaminaUse = atof( i->c_str() ); break;
      case 16 :
        continue; //float upthrow;
      case 17 :
        continue; //int weaponHold; // ???
      case 18 :
        mWeaponType = atoi( i->c_str() ); break;
      case 19 :
        mDiscardable = atoi( i->c_str() ); break;
      case 20 :
        mSkillFactor = atof( i->c_str() ); break;
      case 21 :
        continue; //int mintgtRad;
      case 22 :
        continue; //int maxtgtRad;
      case 23 :
        mMaxRange = atoi( i->c_str() ); break;
      case 24 :
        mAggressiveWeapon = atoi( i->c_str() ); break;
      case 25 :
        mDamageMultiplicator = atof( i->c_str() ); break;
      case 26 :
      case 27 :
      case 28 :
      case 29 :
      case 30 :
      case 31 :
      case 32 :
      case 33 :
        mAmmoTypes[Idx - 26] = atoi( i->c_str() ); break;
      case 34 :
        mAmmoStartFlags = atoi( i->c_str() ); break; // ???
      case 35 :
        continue; //int customclasstype; // ?
      case 36 :
        continue; //int unknown // ?
      case 37 :
        mShotCnt = atoi( i->c_str() ); break;
      case 38 :
        continue; //float shotduration; // maybe useful later
      case 39 :
        continue; //std::string shotfx;
      case 40 :
        continue; //float attachposx;
      case 41 :
        continue; //float attachposy;
      case 42 :
        continue; //float attachposz;
      case 43 :
        continue; //float fpposx;
      case 44 :
        continue; //float fpposy;
      case 45 :
        continue; //float fpposz;
      case 46 :
        mBaseWeaponId = atoi( i->c_str() ); break;
      case 47 :
        continue; //int weaponcolor;
      case 48 :
        continue; //int reloadsound;
      case 49 :
        mItemModGroup = atoi( i->c_str() ); break;
      case 50 :
        mItemModGroupFlags = atoi( i->c_str() ); break;
      case 51 :
        mRareWeaponFx = atoi( i->c_str() ); break;
    }

    if ( Idx >= 51 )
      break;
  }

  return ((Idx == 50) || (Idx == 51));
}

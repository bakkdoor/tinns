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
 def_weapons.h

    CREATED: 29 Mar 2009 Hammag
*/

#ifndef DEF_WEAPONS_H
#define DEF_WEAPONS_H

#ifdef _MSC_VER
#pragma once
#endif

// siiifiiiiififiiffiiifiiiifiiiiiiiiiiiifsffffffiiiiii

class PDefWeapon
{
  private :
    int mIndex;
    std::string mName;
    //int mFpsmodel;
    //int mAttachmodel;
    //int mMunactor;
    //float mDroptime; // useful ?
    int mItemIndex;
    //int effectcolor[3]; // R,G,B
    //int dynamiclight;
    //float lighttime;
    //int soundindex;
    //float mShotTime; // useful ?
    int mAmmoUse;
    int mPSIuse;
    float mStaminaUse;
    //float upthrow;
    //int weaponHold; // ???
    int mWeaponType;
    int mDiscardable;
    float mSkillFactor;
    //int mintgtRad;
    //int maxtgtRad;
    int mMaxRange;
    int mAggressiveWeapon;
    float mDamageMultiplicator;
    int mAmmoTypes[8];
    int mAmmoStartFlags; // ???
    //int customclasstype; // ?
    //int unknown // ?
    int mShotCnt;
    //float shotduration; // maybe useful later
    //std::string shotfx;
    //float attachposx;
    //float attachposy;
    //float attachposz;
    //float fpposx;
    //float fpposy;
    //float fpposz;
    int mBaseWeaponId;
    //int weaponcolor;
    //int reloadsound;
    int mItemModGroup;
    int mItemModGroupFlags;
    int mRareWeaponFx;

  public :
    PDefWeapon();
    ~PDefWeapon();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetIndex() const { return mIndex; }
    inline const std::string &GetName() const { return mName; }
    inline int GetItemID() const { return mItemIndex; }
    inline int GetAmmoUse() const { return mAmmoUse; }
    inline int GetPsiUse() const { return mPSIuse; }
    inline float GetStaminaUse() const { return mStaminaUse; }
    inline int GetWeaponType() const { return mWeaponType; }
    inline bool IsDiscardable() const { return mDiscardable; }
    inline float GetSkillFactor() const { return mSkillFactor; }
    inline int GetMaxRange() const { return mMaxRange; }
    inline bool IsAggressiveWeapon() const { return mAggressiveWeapon; }
    inline float GetDamageMultiplicator() const { return mDamageMultiplicator; }
    inline int GetAmmoType(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < 8)) ? mAmmoTypes[nIdx] : 0) ; }
    inline int GetAmmoStartFlags() const { return mAmmoStartFlags; } 
    inline int GetShotCnt() const { return mShotCnt; }
    inline int GetBaseWeaponId() const { return mBaseWeaponId; }
    inline int GetItemModGroup() const { return mItemModGroup; }
    inline int GetItemModGroupFlags() const { return mItemModGroupFlags; }
    inline int GetRareWeaponFx() const { return mRareWeaponFx; }
};

#endif

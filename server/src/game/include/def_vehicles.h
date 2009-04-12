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
 def_vehicles.h

    CREATED: 03 Apr 2009 Hammag
*/

#ifndef DEF_VEHICLES_H
#define DEF_VEHICLES_H

#include "def.h"

class PDefVhc : public PDef
{
  private :
    //int mIndex; // field 1
    int mModel;
    std::string mName;
    /*
    float mLeftFront; // field 4
    float mFront;
    float mRightBack;
    float mBack;
    float mSideFriction;
    float mDownFriction;
    float mForwardFriction;
    float mAcceleration;
    float mBrakeFactor;
    float mTurnSpeed;
    float mFullTurnDelay;
    int mAnimClass; // field 15
    */
    int mSeatId[8]; // fields 16 - 23
    /*
    float mSpeedTiltFactor; // field 24
    float mSpeedGlideFactor;
    float mMinHover;
    float mMaxHover;
    float mHoverLoopLen;
    int mWheelCnt;
    float mWheelSpeed;
    float mMaxDive;
    int mEffectsID;
    int mShowDebugBouncer; // field 33
    */
    int mHealth;
    int mArmor;
    //int mSoundStartindex; // field 36

    // Additionnal info
    int mNumSeats;
    
  public :
    PDefVhc();
    //~PDefVhc();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetModel() const { return mModel; }
    inline const std::string &GetName() const { return mName; }
    inline int GetSeatId( int nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < 8 ) ) ? mSeatId[nIdx] : -1) ; }
    inline int GetHealth() const { return mHealth; }
    inline int GetArmor() const { return mArmor; }
    inline int GetNumSeats() const { return mNumSeats; }
};

#endif


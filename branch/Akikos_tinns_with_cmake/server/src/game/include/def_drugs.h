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
 def_drug.h

    CREATED: 31 Mar 2009 Hammag
*/

#ifndef DEF_DRUG_H
#define DEF_DRUG_H

#include "def.h"

class PDefDrug : public PDef
{
  protected :
    //int mIndex;
	int mType;
	//int mUseSound;
	int mDuration;
	int mChangeNum;
    int mChangeType[8]; // 1: bonus, 2:malus, ... other ?
    float mChangeScale[8];
	int mChangeTarget[8];

  public :
    PDefDrug();
    //~PDefDrug();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetType() const { return mType; }
	inline int GetDuration() const { return mDuration; }
    inline int GetChangeNum() const { return mChangeNum; }
    inline int GetChangeType( int nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < mChangeNum ) ) ? mChangeType[nIdx] : 0 ) ; }
    inline float GetChangeScale( int nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < mChangeNum ) ) ? mChangeScale[nIdx] : 0 ) ; }
    inline int GetChangeTarget( int nIdx ) const { return ((( nIdx >= 0 ) && ( nIdx < mChangeNum ) ) ? mChangeTarget[nIdx] : 0 ) ; }
};

//type of drug:
//1 normal Drug
//2 Skill ver�ndernder PSI Spruch K�mpfer
//3 Skill ver�ndernder PSI Spruch Supporter
//4 Skill ver�ndernder PSI Spruch Resists
//6+A274  PSI entferne Sprcuh

// drugeffects  < 1000  Subskill
//  <2000 Skill
//  <2100 Energy  (permanent)
//  <2200 Maxenergy
//  <2300 Armor
//  <2400 Subskills
//  3000-3500 Actionmods

#endif

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
 def_damage.h

    CREATED: 31 Mar 2009 Hammag
*/

#ifndef DEF_DAMAGE_H
#define DEF_DAMAGE_H

#include "def.h"

class PDefDamage : public PDef
{
  private :
    //int mIndex;
    //int mSoundIndex; // no use
    int mDamageNum;
    int mDamageValue[4];
    int mDamageEffect[4];
    int mDamageType[4];
    /*	int mEffectNum; // Are effects needed ? (server or client triggered ?)
    	int mEffectId[4]; // ?
    	int mEffectTarget[4]; // float ?
    	int mEffectValue[4]; // float ?
    	int mEffectamorId[4]; // ????
    */
  public :
    PDefDamage();
    //~PDefDamage();

    bool LoadFromDef ( PTokenList *Tokens );

    inline int GetDamageNum() const { return mDamageNum; }
    inline int GetDamageValue ( int nIdx ) const { return ( ( ( nIdx >= 0 ) && ( nIdx < mDamageNum ) ) ? mDamageValue[nIdx] : 0 ) ; }
    inline int GetDamageEffect ( int nIdx ) const { return ( ( ( nIdx >= 0 ) && ( nIdx < mDamageNum ) ) ? mDamageEffect[nIdx] : 0 ) ; }
    inline int GetDamageType ( int nIdx ) const { return ( ( ( nIdx >= 0 ) && ( nIdx < mDamageNum ) ) ? mDamageType[nIdx] : 0 ) ; }
    /*	inline int GetEffectNum() const { return mEffectNum; }
        inline int GetEffectId(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectId[nIdx] : 0) ; }
    	inline int GetEffectTarget(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectTarget[nIdx] : 0) ; }
    	inline int GetEffectValue(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectValue[nIdx] : 0) ; }
    	inline int GetEffectamorId(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mEffectNum)) ? mEffectamorId[nIdx] : 0) ; }
    	*/
};

#endif

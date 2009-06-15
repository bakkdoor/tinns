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
 def_npcgroupspawn.h

    CREATED: 03 Apr 2009 Hammag
*/

#ifndef DEF_NPCGROUPSPAWN_H
#define DEF_NPCGROUPSPAWN_H

#include "def.h"

class PDefNpcGroupSpawn : public PDef
{
  private :
    //int mIndex;
    int mIgnoreNearPC;
    int mNumNpc; // Size of the list. But that should be the size of the group...
    int mNpcType[8];
    std::string mScript[8];
    std::string mScriptParameter[8];
    int mFunctionValue[8];
    int mSpawnChance[8];

  public :
    PDefNpcGroupSpawn();
    //~PDefNpcGroupSpawn();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetIgnoreNearPC() const { return mIgnoreNearPC; }
    inline int GetNumNpc() const { return mNumNpc; }
    inline int GetNpcType(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mNpcType[nIdx] : 0) ; }
    inline const std::string& GetScript(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mScript[nIdx] : EmptyString) ; }
    inline const std::string& GetScriptParameter(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mScriptParameter[nIdx] : EmptyString) ; }
    inline int GetFunctionValue(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mFunctionValue[nIdx] : 0) ; }
    inline int GetSpawnChance(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < mNumNpc)) ? mSpawnChance[nIdx] : 0) ; }
};

#endif


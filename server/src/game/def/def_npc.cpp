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
 def_npc.cpp

    CREATED: 04 Apr 2009 Hammag
*/

#include "main.h"
#include "def_npc.h"

PDefNpc::PDefNpc()
{
}

// Not implemented yet
bool PDefNpc::LoadFromDef( PTokenList *Tokens ) { Tokens = Tokens; return false; }

/*
class PDefNpc : public PDef
{
  private :
    //int mIndex;
    int mModel; // <0: from characters.def, >0: from models.ini, [players] section (?)
    //int mSoundId; // always 0: not used
    int mNGT; // ? 1 - 4
    int mGender; // 0 - 1
    int mFaction;
    int mHealth;
    int mArmorId; // -1 or ref to NpcArmor
    int mWeaponId;
    std::string mDialogScript;
    int mCombat; // ? 20 - 800
    int mLoot; // 0 or ref to ItemContained
    int mMovementEnd; // 0 - 4
    int mFunctionType; // 0 - 4
    float mModelScaling; // 0 - 0.9
    int mMoneyLoose; // 0 - 190
    float mSkillScale; // 0 - 120
    std::string mStandardScript; 
    std::string mStandardParameter;
    int mMass; // 1 - 10000
    //int mStartHour; // not used
    //int mEndHour; // not used
    //std::string mTempScript; // not used
    int mFlags; // values: 513, 259, 256, 128, 64, 48, 35, 34, 33, 32, 2, 1, 0

  public :
    PDefNpc();
    //~PDefNpc();
*/
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
 def_actionmod.cpp

    CREATED: 04 Apr 2009 Hammag
*/

#include "main.h"
#include "def_actionmod.h"

PDefActionMod::PDefActionMod()
{
}

// Not implemented yet
bool PDefActionMod::LoadFromDef( PTokenList *Tokens ) { Tokens = Tokens; return false; }

/*
class PDefActionMod : public PDef
{
  private :
    //int mIndex;
    float mStartValue;
    int mNumOfSsq;
    int mSsqId[8];
    float mModFactor[8];

  public :
    PDefActionMod();
*/

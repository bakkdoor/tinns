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
 def_itemres.cpp

    CREATED: 04 Apr 2009 Hammag
*/

#include "main.h"
#include "def_itemres.h"

PDefItemRestriction::PDefItemRestriction()
{
}

// Not implemented yet
bool PDefItemRestriction::LoadFromDef( PTokenList *Tokens ) { Tokens = Tokens; return false; }

/*
class PDefItemRestriction : public PDef
{
  private :
    //int mIndex;
	int mNumRestrictions;
    int mSsq[6]; // Skill/Subs
	int mMinValue[6];

  public :
    PDefItemRestriction();
*/
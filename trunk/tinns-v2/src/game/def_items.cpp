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
        def_item.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL
        
        MODIFIED: 10 Jul Hammag
        REASON: - Full Item Def implementation
*/

#include "main.h"

PDefItems::PDefItems()
{
	mIndex = 0;
  //mModel = 0;
  mType = 0;
  mValue1 = 0;
  mValue2 = 0;
  mValue3 = 0;
  //mBmNum = 0;
  //mmBmNumIndex = 0;
  mSizeX = 1;
  mSizeY = 1;	
  //mSmallbmnum = 0;
  mWeight = 0;
  mStackable = 0;
  mFillWeight = 0;
  mQualifier = 0;
  //mGfxmods = 0;
  mItemGroupID = 0;	
  mTextDescID = 0;
  mBasePrice = 0;
  mTechlevel = 0;
  mItemflags = 0;
  // std::mShortname = "";
}

PDefItems::~PDefItems()
{

}

bool PDefItems::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0:	// setentry
					continue;
			case 1:	// index
				mIndex = std::atoi(i->c_str()); break;
			case 2: // name
				mName = *i; break;
			case 3:
        // mModel = std::atoi(i->c_str()); // used for IG display
        break;
			case 4:
        mType = std::atoi(i->c_str()); break;          
			case 5:
        mValue1 = std::atoi(i->c_str()); break;
			case 6:
        mValue2 = std::atoi(i->c_str()); break;
			case 7:
        mValue3 = std::atoi(i->c_str()); break;
			case 8:
        // mBmNum = std::atoi(i->c_str()); // used IG for inventory display
        break;
			case 9:
        // mmBmNumIndex = std::atoi(i->c_str()); // used IG for inventory display
        break;
			case 10:
        mSizeX = std::atoi(i->c_str()); break;
			case 11:
        mSizeY = std::atoi(i->c_str()); break;
			case 12:
        // mSmallbmnum = std::atoi(i->c_str()); // used IG for inventory display
        break;
			case 13:
        mWeight = std::atof(i->c_str()); break;
			case 14:
        mStackable = std::atoi(i->c_str()); break;
			case 15:
        mFillWeight = std::atof(i->c_str()); break;
			case 16:
        mQualifier = std::atoi(i->c_str()); break;
			case 17:
        // mGfxmods = std::atoi(i->c_str()); // used IG ?
        break;
			case 18:
        mItemGroupID = std::atoi(i->c_str()); break;
			case 19:
        mTextDescID = std::atoi(i->c_str()); break;
			case 20:
        mBasePrice = std::atoi(i->c_str()); break;
			case 21:
        mTechlevel = std::atoi(i->c_str()); break;
			case 22:
        mItemflags = std::atoi(i->c_str()); break;
			case 23:
        // mShortname = *i; // used IG for display on inventories Icons
        break;
		}
	}

	return true;
}


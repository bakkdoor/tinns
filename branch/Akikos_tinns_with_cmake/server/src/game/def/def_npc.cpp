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

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_npc.h"


PDefNpc::PDefNpc()
{
  mFlags = 0;
}

bool PDefNpc::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        mModel = atoi( i->c_str() ); break;
      case 4 :
        mNGT = atoi( i->c_str() ); break;
      case 5 :
        mGender = atoi( i->c_str() ); break;
      case 6 :
        mFaction = atoi( i->c_str() ); break;
      case 7 :
        mHealth = atoi( i->c_str() ); break;
      case 8 :
        mArmorId = atoi( i->c_str() ); break;
      case 9 :
        mWeaponId = atoi( i->c_str() ); break;
      case 10 :
        mDialogScript = *i; break;
      case 11 :
        mCombat = atoi( i->c_str() ); break;
      case 12 :
        mLoot = atoi( i->c_str() ); break;
      case 13 :
        mMovementEnd = atoi( i->c_str() ); break;
      case 14 :
        mFunctionType = atoi( i->c_str() ); break;
      case 15 :
        mModelScaling = atof( i->c_str() ); break;
      case 16 :
        mMoneyLoose = atoi( i->c_str() ); break;
      case 17 :
        mSkillScale = atof( i->c_str() ); break;
      case 18 :
        mStandardScript = *i; break;
      case 19 :
        mStandardParameter = *i; break;
      case 20 :
        mMass = atoi( i->c_str() ); break;
      case 24 :
        mFlags = atoi( i->c_str() ); break;
      default :
        break;
    }

    if ( Idx >= 24 )
      break;
  }

  return ((Idx >= 20));
}

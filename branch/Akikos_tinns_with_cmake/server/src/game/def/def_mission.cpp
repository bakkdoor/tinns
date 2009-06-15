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
	def_mission.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_mission.h"


PDefMission::PDefMission()
{
}

bool PDefMission::LoadFromDef ( PTokenList *Tokens )
{
  int Idx = 0;
  int arrayIdx, arraySubIdx;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi ( i->c_str() ); break;
      case 2 :
        mSourceId = atoi ( i->c_str() ); break;
      case 3:
        mStartDialog = *i; break;
      case 4 :
        mDescTextId = atoi ( i->c_str() ); break;
      case 33 :
        mEndMoney = atoi ( i->c_str() ); break;
      case 34 :
        mEndXp = atoi ( i->c_str() ); break;
      case 35 :
        mMaxTime = atoi ( i->c_str() ); break;
      case 36 :
        mDifficulty = atoi ( i->c_str() ); break;
      case 37 :
        mMinFactionValue = atoi ( i->c_str() ); break;
      case 38 :
        mPoints = atoi ( i->c_str() ); break;
      case 39 :
        mFlags = atoi ( i->c_str() ); break;
      default :
        if ( ( Idx >= 5 ) && ( Idx <= 16 ) )
        {
          arrayIdx = ( Idx - 5 ) / 3;
          switch ( ( Idx - 5 ) % 3 )
          {
            case 0:
              mNpcType[arrayIdx] = atoi ( i->c_str() ); break;
            case 1:
              mNpcDialog[arrayIdx] = *i; break;
            case 2:
              mNpcDialogStartState[arrayIdx] = atoi ( i->c_str() ); break;
          }
        }
        else if ( ( Idx >= 17 ) && ( Idx <= 32 ) )
        {
          arrayIdx = ( Idx - 17 ) / 4;
          arraySubIdx = ( Idx - 17 ) % 4;
          if ( arraySubIdx == 0 )
            mTargetType[arrayIdx] = atoi ( i->c_str() );
          else
            mTargetValue[arrayIdx][ arraySubIdx - 1 ] = atoi ( i->c_str() );

          break;
        }
        break;
    }

    if ( Idx >= 39 )
      break;
  }

  return ( ( Idx >= 38 ) );
}

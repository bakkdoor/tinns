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
	def_vehiclesits.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_vehiclesits.h"


PDefVhcSeat::PDefVhcSeat()
{
  mTL = mWeaponId = mType = 0;
  mName = "undefined";
  for(int i=0; i<3; ++i)
    mLeaveAngle[i] = mLeavePos[i] = 0;
  mDamageFactor = 0;
}

bool PDefVhcSeat::LoadFromDef( PTokenList *Tokens )
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
        mType = atoi( i->c_str() ); break;
      case 3 :
        mName = *i; break;
      case 30 :
        mWeaponId = atoi( i->c_str() ); break;
      case 31 :
        mTL = atoi( i->c_str() ); break;
      case 32 :
        mDamageFactor = atof( i->c_str() ); break;
      default :
        if( (Idx >= 10) && (Idx <= 12) )
        {
          mLeavePos[Idx - 10] = atof( i->c_str() );
        }
        else if( (Idx >= 13) && (Idx <= 15) )
        {
          mLeaveAngle[Idx - 13] = atof( i->c_str() );
        }
        break;
    }

    if ( Idx >= 34 )
      break;
  }

  return ((Idx >= 34));
}

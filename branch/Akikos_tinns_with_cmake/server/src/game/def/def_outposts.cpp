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
	def_outposts.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_outposts.h"


PDefOutpost::PDefOutpost()
{
  for ( int i = 0;  i < 8; ++i )
    mInfluenceZone[i] = 0;
}

bool PDefOutpost::LoadFromDef ( PTokenList *Tokens )
{
  int Idx=0;
  for ( PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0: // setentry
        break;
      case 1: // index
        mIndex = atoi ( i->c_str() ); break;
      case 2:
        mName = *i; break;
      case 3:
        mType = atoi ( i->c_str() ); break;
      case 4:
        mStandardFaction = atoi ( i->c_str() ); break;
      case 5:
        mRevenue = atof ( i->c_str() ); break;
      case 6:
        mConquestReward = atof ( i->c_str() ); break;
      case 7:
        mMaxSecurity = atoi ( i->c_str() ); break;
      default :
        if ( ( Idx - 8 ) < 8 )
        {
          mInfluenceZone[Idx - 8] = atoi ( i->c_str() );
        }
        break;
    }

    if ( Idx == 15 )
      break;
  }

  return ( Idx >= 7 );
}

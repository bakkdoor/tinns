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
	def_recycles.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_recycles.h"


PDefRecycle::PDefRecycle()
{
}

bool PDefRecycle::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 4;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        //mIndex = atoi ( i->c_str() );
        break;
      case 2 : // We use resultitem as index
        mIndex = mResultItemId = atoi ( i->c_str() ); break;
      case 3:
        mBuildTime = atoi ( i->c_str() ); break;
      case 4 :
      {
        mNumParts = atoi ( i->c_str() );
        if ( mNumParts > 8 )
          mNumParts = 8;
        maxFields = 4 + mNumParts;
        break;
      }
      default :
        if ( ( Idx >= 5 ) && ( Idx <= maxFields ) )
        {
              mPartId[ Idx - 5 ] = atoi ( i->c_str() ); break;
        }
        break;
    }

    if ( Idx >= 12 )
      break;
  }

  return ( ( Idx >= 5 ) );
}

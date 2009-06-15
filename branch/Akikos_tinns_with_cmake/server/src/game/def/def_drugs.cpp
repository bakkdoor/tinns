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
	def_drug.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_drugs.h"


PDefDrug::PDefDrug()
{
}

bool PDefDrug::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 5;
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
        break;
      case 4:
        mDuration = atoi( i->c_str() ); break;
      case 5 :
      {
        mChangeNum = atoi( i->c_str() );
        if ( mChangeNum > 8 )
          mChangeNum = 8;
        maxFields = 5 + 3 * mChangeNum;
        break;
      }
      default :
        if (( Idx >= 6 ) && ( Idx <= maxFields ) )
        {
          switch (( Idx - 6 ) % 3 )
          {
            case 0:
              mChangeType[( Idx - 6 ) / 3] = atoi( i->c_str() ); break;
            case 1:
              mChangeScale[( Idx - 6 ) / 3] = atof( i->c_str() ); break;
            case 2:
              mChangeTarget[( Idx - 6 ) / 3] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return (( Idx == maxFields ) );
}

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
	def_blueprintpieces.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_blueprintpieces.h"


PDefBlueprintPieces::PDefBlueprintPieces()
{
}


bool PDefBlueprintPieces::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int maxFields = 3;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        break;
      case 1 :
        mIndex = atoi( i->c_str() ); break;
      case 2 :
        mMaxPieceNum = atoi( i->c_str() ); break;
      case 3 :
      {
        mPieceNum = atoi( i->c_str() );
        if ( mPieceNum > 20 )
          mPieceNum = 20;
        maxFields = 3 + mPieceNum;
        break;
      }
      default :
        if (( Idx >= 4 ) && ( Idx <= maxFields ) )
        {
          mPieceId[ Idx - 4 ] = atoi( i->c_str() );
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  return (( Idx == maxFields ) );
}

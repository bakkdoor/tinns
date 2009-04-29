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
 def_shots.cpp

    CREATED: 04 Apr 2009 Hammag
*/

#include "main.h"
#include "def_shots.h"

PDefShot::PDefShot()
{ }

bool PDefShot::LoadFromDef( PTokenList *Tokens )
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
        mDamageId = atoi( i->c_str() ); break;
      case 3 :
        mMass = atoi( i->c_str() ); break;
      case 4 :
        mRadius = atoi( i->c_str() ); break;
      case 5 :
        mSpeed = atof( i->c_str() ); break;
      default :
        break;
    }

    if ( Idx >= 5 )
      break;
  }

  return (( Idx >= 5 ) );
}

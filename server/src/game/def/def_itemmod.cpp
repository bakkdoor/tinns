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
 def_itemmod.cpp

    CREATED: 04 Avr 2009 Hammag
*/

#include "main.h"
#include "def_itemmod.h"

PDefItemMod::PDefItemMod()
{
  for ( int i = 0; i < 4; ++i )
  {
    mChangeTarget[i] = 0;
    mChangeScale[i] = mChangeValue[0] = 0;
  }
}

bool PDefItemMod::LoadFromDef ( PTokenList *Tokens )
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
        mIndex = atoi ( i->c_str() ); break;
      case 2 :
        mType = atoi ( i->c_str() ); break;
      case 3:
        break;
      case 4 :
      {
        mChangeNum = atoi ( i->c_str() );
        if ( mChangeNum > 4 )
          mChangeNum = 4;
        maxFields = 4 + 3 * mChangeNum;
        break;
      }
      case 17 :
        mName = *i; break;
      default :
        if ( ( Idx >= 5 ) && ( Idx <= maxFields ) )
        {
          switch ( ( Idx - 5 ) % 3 )
          {
            case 0:
              mChangeTarget[ ( Idx - 5 ) / 3] = atoi ( i->c_str() ); break;
            case 1:
              mChangeValue[ ( Idx - 5 ) / 3] = atof ( i->c_str() ); break;
            case 2:
              mChangeScale[ ( Idx - 5 ) / 3] = atof ( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= 18 )
      break;
  }

  return ( ( Idx >= 17 ) );
}

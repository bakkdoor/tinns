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
	def_weather.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_weather.h"


PDefWeather::PDefWeather()
{
  mNumWeathers = mSectionId = 0;
  for(int i=0; i<8; ++i)
      mDuration[i] = mWeatherId[i] = 0;
}

bool PDefWeather::LoadFromDef( PTokenList *Tokens )
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
        mSectionId = atoi( i->c_str() ); break;
      case 3 :
        mNumWeathers = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 4) && (Idx <= (3 + 2*mNumWeathers))  && (Idx <= 19) )
        {
          if(Idx & 1)
          {
            mDuration[int((Idx - 4)/2)] = atoi( i->c_str() );
          }
          else
          {
            mWeatherId[int((Idx - 4)/2)] = atoi( i->c_str() );
          }
        }
        break;
    }

    if ( Idx >= 19 )
      break;
  }

  return ((Idx >= (3 + 2*mNumWeathers)));
}

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
	def_damage.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_damage.h"


PDefDamage::PDefDamage()
{
}

bool PDefDamage::LoadFromDef( PTokenList *Tokens )
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
        break;
      case 3 :
      {
        mDamageNum = atoi( i->c_str() );
        if ( mDamageNum > 4 )
          mDamageNum = 4;
        maxFields = 3 + 3 * mDamageNum;
        break;
      }
      default :
        if (( Idx >= 4 ) && ( Idx <= maxFields ) )
        {
          switch (( Idx - 4 ) % 3 )
          {
            case 0:
              mDamageValue[( Idx - 4 ) / 3] = atoi( i->c_str() ); break;
            case 1:
              mDamageEffect[( Idx - 4 ) / 3] = atoi( i->c_str() ); break;
            case 2:
              mDamageType[( Idx - 4 ) / 3] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= maxFields )
      break;
  }

  // Try to fix wrong size field
  if( Idx < maxFields )
  {
    mDamageNum = ( Idx - 4 ) / 3;
    if( (( Idx - 4 ) % 3) == 2 )
      ++mDamageNum;
  }
  return ( Idx >= 3 );
}
/*
class PDefDamage : public PDef
{
  private :
    //int mIndex;
    //int mSoundIndex; // no use
    int mDamageNum;
    int mDamageValue[4];
    int mDamageEffect[4];
    int mDamageType[4];
    // int mEffectNum; // Are effects needed ? (server or client triggered ?)
    // int mEffectId[4]; // ?
    // int mEffectTarget[4]; // float ?
    // int mEffectValue[4]; // float ?
    // int mEffectamorId[4]; // ????

  public :
    PDefDamage();
    //~PDefDamage();
*/

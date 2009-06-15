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
	def_itemcontainer.cpp

	CREATED: 04 Apr 2009 Hammag

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/def_itemcontainer.h"


PDefItemContainer::PDefItemContainer()
{
  mNumItems = 0;
  for(int i=0; i<6; ++i)
  {
      mChance[i] = mItemId[i] = mCumulatedChance[i] = 0;
      mQuality[i] = 0;
  }
}

bool PDefItemContainer::LoadFromDef( PTokenList *Tokens )
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
        mNumItemsAtOnce = atoi( i->c_str() ); break;
      case 3 :
        mRespawnTime = atoi( i->c_str() ); break;
      case 4 :
        mNumItems = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 5) && (Idx <= 22) )
        {
          switch ( ((Idx-5) % 3) )
          {
            case 0:
              mItemId[int((Idx - 5)/3)] = atoi( i->c_str() ); break;
            case 1:
              mQuality[int((Idx - 5)/3)] = atof( i->c_str() ); break;
            case 2:
              mChance[int((Idx - 5)/3)] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= 22 )
      break;
  }

  if (Idx >= 22)
  {
    BuildCumulatedChance();
    return true;
  }
  else
    return false;
}

void PDefItemContainer::BuildCumulatedChance()
{
  mCumulatedChance[0] = mChance[0];
  for(int i = 1; i < mNumItems; ++i)
  {
    mCumulatedChance[i] = mCumulatedChance[i-1] + mChance[i];
  }
}

int PDefItemContainer::GetRandomItemIdx() const
{
  int i = -1;
  
  if(mNumItems > 0)
  {
    int randomValue = GetRandom( mCumulatedChance[mNumItems-1], 1 );
    for(i = 0; i < mNumItems; ++i)
    {
      if(randomValue <= mCumulatedChance[i])
        break;
    }
  }
  return i;
}

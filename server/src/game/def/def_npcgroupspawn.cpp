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
 def_npcgroupspawn.cpp

    CREATED: 04 Apr 2009 Hammag
*/

#include "main.h"
#include "def_npcgroupspawn.h"

PDefNpcGroupSpawn::PDefNpcGroupSpawn()
{
  mNumNpc = 0;
  for(int i=0; i<8; ++i)
    mSpawnChance[i] = mFunctionValue[i] = mNpcType[i] = 0;

}


bool PDefNpcGroupSpawn::LoadFromDef( PTokenList *Tokens )
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
        mIgnoreNearPC = atoi( i->c_str() ); break;
      case 3 :
        mNumNpc = atoi( i->c_str() ); break;
      default :
        if( (Idx >= 4) && (Idx <= (3 + 5*mNumNpc)) && (Idx <= (43)) )
        {
          switch ( ((Idx - 4) % 5) )
          {
            case 0:
              mNpcType[int((Idx - 4)/5)] = atoi( i->c_str() ); break;
            case 1:
              mScript[int((Idx - 4)/5)] = *i; break;
            case 2:
              mScriptParameter[int((Idx - 4)/5)] = *i ; break;
            case 3:
              mFunctionValue[int((Idx - 4)/5)] = atoi( i->c_str() ); break;
            case 4:
              mSpawnChance[int((Idx - 4)/5)] = atoi( i->c_str() ); break;
          }
        }
        break;
    }

    if ( Idx >= 43 )
      break;
  }

  return ((Idx >= (3 + 5*mNumNpc)));
}

/*
class PDefNpcGroupSpawn : public PDef
{
  private :
    //int mIndex;
    int mIgnoreNearPC;
    int mNumNpc; // Size of the list. But that should be the size of the group...
    int mNpcType[8];
    std::string mScript[8];
    std::string mScriptParameter[8];
    int mFunctionValue[8];
    int mSpawnChance[8];

  public :
    PDefNpcGroupSpawn();
    //~PDefNpcGroupSpawn();
*/


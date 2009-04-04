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
 def_respawn.cpp

 MODIFIED: 22 Sep 2006 Hammag
 REASON: - Creation

*/

#include "main.h"

PDefRespawn::PDefRespawn()
{
}

bool PDefRespawn::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0: // setentry
        continue;

      case 1: // index
        mIndex = atoi( i->c_str() ); break;

      case 2: // value
        mWorldID = atoi( i->c_str() ); break;

      case 3: // value
        mEntityID = atoi( i->c_str() ); break;

      case 4: // value
        mHazardLevel = atoi( i->c_str() ); break;

      case 5: // name
        mName = *i; break;

      case 6: // world name
        mFlag = *i; break;
    }
  }

  return true;
}

// class PDefRespawnMap defined in gamedefs.h
int PDefRespawnsMap::GetRespawnEntity( u32 nWorldID, u16 nGROrder ) const
{
  u16 tOrder = 0;

  for ( std::map<int, PDefRespawn*>::const_iterator it = mDefs.begin(); it != mDefs.end(); it++ )
  {
    if (( u32 )( it->second->GetWorldID() ) == nWorldID )
    {
      ++tOrder;
      if (( nWorldID == 1 ) || ( nWorldID == 2 ) ) // hack for zones 1 & 2
      {
        if (( 3 - tOrder ) == nGROrder )
        {
          return ( it->second->GetEntityID() );
        }
      }
      else if ( tOrder == nGROrder )
      {
        return ( it->second->GetEntityID() );
      }
    }
  }
  return 0;
}

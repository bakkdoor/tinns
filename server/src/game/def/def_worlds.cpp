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
        def_worlds.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown
        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL

        MODIFIED: 07 Oct 2006 Hammag
        REASON: - Added quotes and spaces trim to mName and mDatFile.
*/

#include "main.h"

PDefWorld::PDefWorld()
{
}

bool PDefWorld::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  int StartPos = 0;
  int TailLen = 0;
  int Len;

  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0: // setentry
        continue;
      case 3: // music
        continue;

      case 1: // index
        mIndex = atoi( i->c_str() ); break;

      case 2: // name
      {
        Len = ( *i ).length();

        if (( *i )[0] == '\"' )
          StartPos = 1;

        while (( *i )[StartPos] == ' ' )
          ++StartPos;

        if (( *i )[Len-1] == '\"' )
          TailLen = 1;

        while (( Len - TailLen > 0 ) && (( *i )[Len-TailLen-1] == ' ' ) )
          ++TailLen;

        if ( StartPos >= Len - TailLen )
          mName = "";
        else
          mName = ( *i ).substr( StartPos, Len - StartPos - TailLen );

        break;
      }

      case 4: // datfile
      {
        Len = ( *i ).length();

        while (( StartPos = ( *i ).find( "\\", StartPos ) ) > -1 )
          ( *i )[StartPos] = '/';
        StartPos = 0;

        if (( *i )[0] == '\"' )
          StartPos = 1;

        while (( *i )[StartPos] == ' ' )
          ++StartPos;

        if (( StartPos <= Len - 2 ) && (( *i ).substr( StartPos, 2 ) == "./" ) )
          StartPos += 2;

        if (( *i )[Len-1] == '\"' )
          TailLen = 1;

        while (( Len - TailLen > 0 ) && (( *i )[Len-TailLen-1] == ' ' ) )
          ++TailLen;

        if ( StartPos >= Len - TailLen )
          mDatFile = "";
        else
          mDatFile = ( *i ).substr( StartPos, Len - StartPos - TailLen );

        break;
      }

      case 5:
        mFlags = atoi( i->c_str() ); break;
    }
  }

  return true;
}


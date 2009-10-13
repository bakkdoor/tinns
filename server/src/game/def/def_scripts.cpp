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
 def_scripts.cpp

    CREATED: 12 Oct 2009 Namikon
*/

#include "main.h"


PDefScripts::PDefScripts()
{
    mIdentifier = "";
    mLuaFile = "";
}

bool PDefScripts::LoadFromDef( PTokenList *Tokens )
{
  int Idx = 0;
  for ( PTokenList::iterator i = Tokens->begin(); i != Tokens->end(); i++, Idx++ )
  {
    switch ( Idx )
    {
      case 0 : // setentry
        continue;
      case 1 : // index
        mIndex = atoi(i->c_str()); break;
      case 2 :
        mIdentifier = *i;
        CleanUpString(&mIdentifier);
        break;
      case 3 :
        mLuaFile = *i;
        CleanUpString(&mLuaFile);
        break;
      case 4 :
        //mScriptHeader = *i; break;
        continue;
    }
  }

  return true;
}

void PDefScripts::CleanUpString(std::string *nString)
{
    if(nString->length() > 3)
    {
        size_t tfound;
        string t_replacechr ("\"");

        tfound = nString->find(t_replacechr);
        while(tfound != string::npos)
        {
            nString->replace(tfound, 1, " ");
            tfound = nString->find( t_replacechr, tfound +1 );
        }
        Trim(nString);
    }
    else
    {
        *nString = "";
    }
}
/*
bool PDefScriptsMap::Load(const char* nName, const char* nFilename)
{
  mName = nName;
  if(mName.empty())
  {
    Console->Print( "%s Defs name not defined", Console->ColorText( RED, BLACK, "[ERROR]" ) );
    return (false);
  }

  if(! *nFilename)
  {
    Console->Print( "%s Filename not defined for %s defs", Console->ColorText( RED, BLACK, "[ERROR]" ), mName.c_str() );
    return (false);
  }

  PDefParser parser;
  int nDefs = 0, nErrors = 0, nDup = 0;

  if ( parser.Parse( nFilename ) )
  {
    const PDefTokenList &t = parser.GetTokens();

    for ( PDefTokenList::const_iterator i = t.begin(); i != t.end(); i++ )
    {
      PDefScripts *it = new PDefScripts();
      bool loadfail = !it->LoadFromDef( *i ), insertfail = false;

      if ( !loadfail )
        insertfail = !mDefs.insert( std::make_pair( it->GetIndex(), it ) ).second;
      if ( loadfail || insertfail )
      {
        if ( insertfail )
        {
          ++nDup;
          if ( gDevDebug ) Console->Print( "%s ini error (new duplicate id %i discarded)", mName.c_str(), it->GetIndex(), it->GetName().c_str() );
        }
        else
        {
          Console->Print( "%s ini load error @ %i", mName.c_str(), nDefs + nErrors );
          ++nErrors;
        }
        delete it;
      }
      else
        ++nDefs;
    }
  }
  else
  {
    Console->Print( "%s Error loading %s ini defs", Console->ColorText( RED, BLACK, "[ERROR]" ), mName.c_str() );
    return ( false );
  }

  if ( nErrors > 0 )
    Console->Print( "%s Loaded %i %s ini defs, %i error(s).", Console->ColorText( RED, BLACK, "[ERROR]" ), nDefs, mName.c_str(), nErrors );
  else
    Console->Print( "%s Loaded %i %s ini defs, %i error(s).", Console->ColorText( GREEN, BLACK, "[Success]" ), nDefs, mName.c_str(), nErrors );

  if ( nDup )
    Console->Print( "%s %d duplicate entries ignored in %s ini defs.", Console->ColorText( YELLOW, BLACK, "[Notice]" ), nDup, mName.c_str() );

  return ( true );
}
*/

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
 defmap.h
 Definiton of template class for def maps.

    CREATED: 30 Mar 2009 Hammag
*/

template <class T> class PDefMap
{
  protected:
    std::string mName;
    std::map<int, T*> mDefs;

    void Clear();

  public:
    PDefMap () {};
    ~PDefMap ();

    bool Load(const char* nName, const char* nFilename);
    inline int GetNumDefs() const { return mDefs.size(); }
    const T* GetDef( int Index ) const;
};

template <class T> PDefMap<T>::~PDefMap()
{
  Clear();
}

template <class T> void PDefMap<T>::Clear()
{
  for ( typename std::map<int, T*>::iterator i = mDefs.begin(); i != mDefs.end(); i++ )
    delete i->second;
  mDefs.clear();
}

template <class T> bool PDefMap<T>::Load(const char* nName, const char* nFilename)
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
  int nDefs = 0, nErrors = 0;

  if ( parser.Parse( nFilename ) )
  {
    const PDefTokenList &t = parser.GetTokens();

    for ( PDefTokenList::const_iterator i = t.begin(); i != t.end(); i++ )
    {
      T* entry = new T();
      bool insertfail = false;
      bool loadfail = ! entry->LoadFromDef( *i );

      if ( !loadfail )
        insertfail = ! mDefs.insert( std::make_pair( entry->GetIndex(), entry ) ).second;

      if ( loadfail || insertfail )
      {
        if ( insertfail )
          Console->Print( RED, BLACK, "%s def error (duplicate id %i): %s",  mName.c_str(), entry->GetIndex(), entry->GetName().c_str() );
        else
          Console->Print( RED, BLACK, "%s def load error @ %i",  mName.c_str(), nDefs + nErrors );
        ++nErrors;
        delete entry;
      }
      else
        ++nDefs;
    }
  }
  else
  {
    Console->Print( "%s Error loading %s defs", Console->ColorText( RED, BLACK, "[ERROR]" ),  mName.c_str() );
    return (false);
  }

  if ( nErrors > 0 )
    Console->Print( "%s Loaded %i %s defs, %i error(s).", Console->ColorText( RED, BLACK, "[ERROR]" ), nDefs,  mName.c_str(), nErrors );
  else
    Console->Print( "%s Loaded %i %s defs, %i error(s).", Console->ColorText( GREEN, BLACK, "[Success]" ), nDefs,  mName.c_str(), nErrors );

  return ( true );
}

template <class T> const T* PDefMap<T>::GetDef( int Index ) const
{
  T* Result;

  typename std::map<int, T*>::const_iterator i = mDefs.find( Index );
  Result = ( ( i != mDefs.end() ) ? i->second : NULL );

  return ( Result );
}

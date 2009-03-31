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
        def_item.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL

        MODIFIED: 10 Jul Hammag
        REASON: - Full Item Def implementation
*/

#include "main.h"
#define GAMEDEFS_DEFITEMSMAXSEQ 100


PDefItems::PDefItems()
{
  mIndex = 0;
  //mModel = 0;
  mType = 0;
  mValue1 = 0;
  mValue2 = 0;
  mValue3 = 0;
  //mBmNum = 0;
  //mmBmNumIndex = 0;
  mSizeX = 1;
  mSizeY = 1;
  //mSmallbmnum = 0;
  mWeight = 0;
  mStackable = 0;
  mFillWeight = 0;
  mQualifier = 0;
  mGfxMods = 0;
  mItemGroupID = 0;
  mTextDescID = 0;
  mBasePrice = 0;
  mTechlevel = 0;
  mItemflags = 0;
  // std::mShortname = "";
}

PDefItems::~PDefItems()
{

}

bool PDefItems::LoadFromDef( PTokenList *Tokens )
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
      case 2: // name
        mName = *i; break;
      case 3:
        // mModel = atoi(i->c_str()); // used for IG display
        break;
      case 4:
        mType = atoi( i->c_str() ); break;
      case 5:
        mValue1 = atoi( i->c_str() ); break;
      case 6:
        mValue2 = atoi( i->c_str() ); break;
      case 7:
        mValue3 = atoi( i->c_str() ); break;
      case 8:
        // mBmNum = atoi(i->c_str()); // used IG for inventory display
        break;
      case 9:
        // mmBmNumIndex = atoi(i->c_str()); // used IG for inventory display
        break;
      case 10:
        mSizeX = atoi( i->c_str() ); break;
      case 11:
        mSizeY = atoi( i->c_str() ); break;
      case 12:
        // mSmallbmnum = atoi(i->c_str()); // used IG for inventory display
        break;
      case 13:
        mWeight = atof( i->c_str() ); break;
      case 14:
        mStackable = atoi( i->c_str() ); break;
      case 15:
        mFillWeight = atof( i->c_str() ); break;
      case 16:
        mQualifier = atoi( i->c_str() ); break;
      case 17:
        mGfxMods = atoi( i->c_str() ); break;
      case 18:
        mItemGroupID = atoi( i->c_str() ); break;
      case 19:
        mTextDescID = atoi( i->c_str() ); break;
      case 20:
        mBasePrice = atoi( i->c_str() ); break;
      case 21:
        mTechlevel = atoi( i->c_str() ); break;
      case 22:
        mItemflags = atoi( i->c_str() ); break;
      case 23:
        // mShortname = *i; // used IG for display on inventories Icons
        break;
    }
  }

  return true;
}


PDefItemsMap::PDefItemsMap()
{
  mMapItCache = NULL;
  mMapItCacheCount = 0;
}

PDefItemsMap::~PDefItemsMap()
{
  delete [] mMapItCache;
}

void PDefItemsMap::BuildMapItCache()
{
  int CacheEntryIdx = 0;
  int EntrySeqIdx = 1;

  mMapItCacheCount = 1 + ( mDefs.size() / GAMEDEFS_DEFITEMSMAXSEQ );

  if ( mMapItCacheCount )
  {
    if( mMapItCache )
    {
      delete [] mMapItCache;
    }
    mMapItCache = new std::map<int, PDefItems*>::const_iterator[mMapItCacheCount];
    mMapItCache[CacheEntryIdx++] = mDefs.begin();

    std::map<int, PDefItems*>::const_iterator It = mDefs.begin();
    while ( It != mDefs.end() )
    {
      if ( EntrySeqIdx++ == GAMEDEFS_DEFITEMSMAXSEQ )
      {
        mMapItCache[CacheEntryIdx++] = It;
        EntrySeqIdx = 1;
      }
      It++;
    }
  }
}

bool PDefItemsMap::Load(const char* nName, const char* nFilename)
{
  if( PDefMap<PDefItems>::Load( nName, nFilename) )
  {
    BuildMapItCache();
    return ( true );
  }
  else
    return ( false );
}

const PDefItems* PDefItemsMap::GetDefBySeqIndex( int SeqIndex ) const
{
  int CacheEntryIdx = SeqIndex / GAMEDEFS_DEFITEMSMAXSEQ;
  if ( CacheEntryIdx >= mMapItCacheCount )
    return NULL;

  std::map<int, PDefItems*>::const_iterator It = mMapItCache[CacheEntryIdx];
  int EntrySeqIdx = CacheEntryIdx * GAMEDEFS_DEFITEMSMAXSEQ;

  while (( EntrySeqIdx < SeqIndex ) && ( It != mDefs.end() ) )
  {
    EntrySeqIdx++;
    It++;
  }

  if (( EntrySeqIdx == SeqIndex ) && ( It != mDefs.end() ) )
  {
    return It->second;
  }
  else
    return NULL;
}

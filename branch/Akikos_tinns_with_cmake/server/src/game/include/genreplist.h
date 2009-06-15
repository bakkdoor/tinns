/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  genreplist.h - classe for character genrep list

	MODIFIED: 20 Sep 2006 Hammag
	REASON: - creation
	
*/

#ifndef GENREPLIST_H
#define GENREPLIST_H

#define GENREPLIST_ALLOC_SIZE 4 // atomicity of list entries allocation

class PGenrepList
{
  private:
    enum { // genrep DB Table fields
        g_id = 0,
        g_worldid,
        g_stationid,
        g_charid
    };
    struct PGenrepEntry
    {
      u16 mWorldID;
      u16 mStationID;
    };
    
    u32 mOwnerCharID;
    u8 mListMaxSize;
    u8 mListSize;
    PGenrepEntry* mGenrepList;
    
    void IncreaseMaxSize(u8 nNewMax = 0);
    u8 FindEntry(u16 nWorldID, u16 nStationID);
    
  public:
    PGenrepList(u32 nOwnerCharID);
    ~PGenrepList();
    bool AddGenrep(u16 nWorldID, u16 nStationID);
    //bool RemoveChar(u32 nBuddyCharID);    
    inline u8 Count() { return mListSize; }
    u16 GetListDataSize() { return (sizeof(PGenrepEntry) * mListSize); }
    const void* GetListData() { return (const void*)mGenrepList; }
    bool SQLLoad();
//    bool SQLSave();
};

#endif

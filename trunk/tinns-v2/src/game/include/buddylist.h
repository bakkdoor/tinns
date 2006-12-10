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
  buddylist.h - classe for chat buddylist

	MODIFIED: 19 Sep 2006 Hammag
	REASON: - creation

*/

#ifndef BUDDYLIST_H
#define BUDDYLIST_H

#define BUDDYLIST_ALLOC_SIZE 4 // atomicity of list entries allocation

class PBuddyList
{
  private:
    enum { // buddy_list DB Table fields
        bud_id = 0,
        bud_charid,
        bud_buddyid
    };

    u32 mOwnerCharID;
    u8 mListMaxSize;
    u8 mListSize;
    u32* mCharIDList;

    void IncreaseMaxSize(u8 nNewMax = 0);
    u8 FindEntry(u32 CharID);

  public:
    PBuddyList(u32 nOwnerCharID);
    ~PBuddyList();
    bool AddChar(u32 nBuddyCharID);
    bool RemoveChar(u32 nBuddyCharID);
    inline u8 Count() { return mListSize; }
    u16 GetListDataSize() { return (sizeof(u32) * mListSize); }
    const void* GetListData() { return (const void*)mCharIDList; }
    bool SQLLoad();

    bool IsInBuddy(u32 CharID);
//    bool SQLSave();
};

#endif

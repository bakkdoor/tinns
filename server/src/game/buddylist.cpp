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
  buddylist.cpp - classe for chat buddylist

 MODIFIED: 19 Sep 2006 Hammag
 REASON: - creation

 TODO: - link with a online char list, updated when needed (chars in/out, or list modified)
*/

#include "main.h"
#include "buddylist.h"

PBuddyList::PBuddyList(u32 nOwnerCharID)
{
    mOwnerCharID = nOwnerCharID;
    mListMaxSize = mListSize = 0;
    mCharIDList = NULL;
}

PBuddyList::~PBuddyList()
{

    if (mCharIDList)
        delete[] mCharIDList;
}

bool PBuddyList::AddChar(u32 nBuddyCharID)
{
    char query[256];

    if ((FindEntry(nBuddyCharID) >= mListSize) && (mListSize < 255))
    {
        if (mListSize == mListMaxSize)
        {
            IncreaseMaxSize();
        }
        mCharIDList[mListSize++] = nBuddyCharID;

        snprintf(query, 256, "INSERT INTO buddy_list (bud_id,bud_charid,bud_buddyid) VALUES (NULL,'%u','%u');", mOwnerCharID, nBuddyCharID);
        if ( MySQL->GameQuery(query) )
        {
            Console->Print(RED, BLACK, "PBuddyList::AddChar could not add some buddylisdt entry in the database");
            Console->Print("Query was:");
            Console->Print("%s", query);
            MySQL->ShowGameSQLError();
            return false;
        }

        return true;
    }
    else
        return false;
}

bool PBuddyList::RemoveChar(u32 nBuddyCharID)
{
    char query[256];
    u8 rEntry, i;

    if ((rEntry = FindEntry(nBuddyCharID)) < mListSize)
    {
        --mListSize;
        for (i = rEntry; i < mListSize; i++)
        {
            mCharIDList[i] = mCharIDList[i+1];
        }
        // Remove from DB here
        snprintf(query, 256, "DELETE FROM buddy_list WHERE ((bud_charid='%u') AND (bud_buddyid='%u')) LIMIT 1;", mOwnerCharID, nBuddyCharID);
        if ( MySQL->GameQuery(query) )
        {
            Console->Print(RED, BLACK, "PBuddyList::AddChar could not delete some buddylist entry from the database");
            Console->Print("Query was:");
            Console->Print("%s", query);
            MySQL->ShowGameSQLError();
            return false;
        }

        return true;
    }
    else
        return false;
}

bool PBuddyList::SQLLoad()
{
    char query[256];
    MYSQL_RES *result;
    MYSQL_ROW row;
    u8 EntriesNum;

    snprintf(query, 256, "SELECT * FROM buddy_list WHERE (bud_charid='%u')", mOwnerCharID);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PBuddyList::SQLLoad could not load buddylist entry from the database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }
    //Console->Print(GREEN, BLACK, "PBuddyList::SQLLoad Loading buddylist for char %d", mOwnerCharID);
    mListSize = 0;
    if((EntriesNum = mysql_num_rows(result)))
    {
        IncreaseMaxSize(EntriesNum);

        while((row = mysql_fetch_row(result)))
        {
            mCharIDList[mListSize++] = std::atoi(row[bud_buddyid]);
            //Console->Print(GREEN, BLACK, "PBuddyList::SQLLoad Buddylist entry %d : char %d", mListSize, mCharIDList[mListSize-1]);
        }
    }
    MySQL->FreeGameSQLResult(result);
    return true;
}

void PBuddyList::IncreaseMaxSize(u8 nNewMax)
{
    u16 tmpSize;

    if (!nNewMax)
    {
        tmpSize = mListMaxSize + BUDDYLIST_ALLOC_SIZE;
    }
    else if (nNewMax > mListMaxSize)
    {
        tmpSize = nNewMax / BUDDYLIST_ALLOC_SIZE;
        if (nNewMax % BUDDYLIST_ALLOC_SIZE)
            ++tmpSize;
        tmpSize *= BUDDYLIST_ALLOC_SIZE;
    }
    else
        return;

    mListMaxSize = (tmpSize < 256) ? tmpSize : 255;

    u32* tmpList = new u32[mListMaxSize];
    if (mCharIDList)
    {
        if (mListSize)
        {
            memcpy(tmpList, mCharIDList, sizeof(u32) * mListSize);
        }

        delete[] mCharIDList;
    }
    mCharIDList = tmpList;
}

u8 PBuddyList::FindEntry(u32 CharID)
{
    u8 i = 255;

    if (mCharIDList)
    {
        for (i = 0; i < mListSize; i++)
        {
            if (mCharIDList[i] == CharID)
                break;
        }
    }

    return i;
}

bool PBuddyList::IsInBuddy(u32 CharID)
{
    if (mCharIDList)
    {
        for (u8 i = 0; i < mListSize; i++)
        {
            if (mCharIDList[i] == CharID)
                return true;
        }
    }
    return false;
}

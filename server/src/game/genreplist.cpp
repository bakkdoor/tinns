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
  genreplist.cpp - classe for character genrep list

 MODIFIED: 20 Sep 2006 Hammag
 REASON: - creation

*/

#include "main.h"
#include "genreplist.h"

PGenrepList::PGenrepList(u32 nOwnerCharID)
{
    mOwnerCharID = nOwnerCharID;
    mListMaxSize = mListSize = 0;
    mGenrepList = NULL;
}

PGenrepList::~PGenrepList()
{
    if (mGenrepList)
        delete[] mGenrepList;
}

bool PGenrepList::AddGenrep(u16 nWorldID, u16 nStationID)
{
    char query[256];

    if ((FindEntry(nWorldID, nStationID) >= mListSize) && (mListSize < 255))
    {
        if (mListSize == mListMaxSize)
        {
            IncreaseMaxSize();
        }
        mGenrepList[mListSize].mWorldID = nWorldID;
        mGenrepList[mListSize++].mStationID = nStationID;

        snprintf(query, 256, "INSERT INTO genrep (g_id,g_worldid,g_stationid,g_charid) VALUES (NULL,'%u','%u','%u');", nWorldID, nStationID, mOwnerCharID);
        if ( MySQL->GameQuery(query) )
        {
            Console->Print(RED, BLACK, "PGenrepList::AddGenrep could not add some genrep entry in the database");
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

/*bool PGenrepList::RemoveChar(u32 nBuddyCharID)
{
  char query[256];
  u8 rEntry, i;

  if ((rEntry = FindEntry(nBuddyCharID)) < mListSize)
  {
    --mListSize;
    for (i = rEntry; i < mListSize; i++)
    {
      mGenrepList[i] = mGenrepList[i+1];
    }
    // Remove from DB here
    snprintf(query, 256, "DELETE FROM buddy_list WHERE ((bud_charid='%u') AND (bud_buddyid='%u')) LIMIT 1;", mOwnerCharID, nBuddyCharID);
    if ( MySQL->GameQuery(query) )
    {
        Console->Print(RED, BLACK, "PBuddyList::AddChar could not add some buddylist entry in the database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }

    return true;
  }
  else
    return false;
}*/

bool PGenrepList::SQLLoad()
{
    char query[256];
    MYSQL_RES *result;
    MYSQL_ROW row;
    u8 EntriesNum;

    snprintf(query, 256, "SELECT * FROM genrep WHERE (g_charid='%u')", mOwnerCharID);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PGenrepList::SQLLoad could not load genreplist from the database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }
    //Console->Print(GREEN, BLACK, "PGenrepList::SQLLoad Loading Genrep list for char %d", mOwnerCharID);
    mListSize = 0;
    if((EntriesNum = mysql_num_rows(result)))
    {
        IncreaseMaxSize(EntriesNum);

        while((row = mysql_fetch_row(result)))
        {
            mGenrepList[mListSize].mWorldID = atoi(row[g_worldid]);
            mGenrepList[mListSize++].mStationID = atoi(row[g_stationid]);
            //Console->Print(GREEN, BLACK, "PGenrepList::SQLLoad Genrep list entry %d : world %d station %d", mListSize, mGenrepList[mListSize-1].mWorldID,mGenrepList[mListSize-1].mStationID);
        }
    }
    MySQL->FreeGameSQLResult(result);
    return true;
}

void PGenrepList::IncreaseMaxSize(u8 nNewMax)
{
    u16 tmpSize;

    if (!nNewMax)
    {
        tmpSize = mListMaxSize + GENREPLIST_ALLOC_SIZE;
    }
    else if (nNewMax > mListMaxSize)
    {
        tmpSize = nNewMax / GENREPLIST_ALLOC_SIZE;
        if (nNewMax % GENREPLIST_ALLOC_SIZE)
            ++tmpSize;
        tmpSize *= GENREPLIST_ALLOC_SIZE;
    }
    else
        return;

    mListMaxSize = (tmpSize < 256) ? tmpSize : 255;

    PGenrepEntry* tmpList = new PGenrepEntry[mListMaxSize];
    if (mGenrepList)
    {
        if (mListSize)
        {
            memcpy(tmpList, mGenrepList, sizeof(PGenrepEntry) * mListSize);
        }
        delete[] mGenrepList;
    }
    mGenrepList = tmpList;
}

u8 PGenrepList::FindEntry(u16 nWorldID, u16 nStationID)
{
    u8 i = 255;

    if (mGenrepList)
    {
        for (i = 0; i < mListSize; i++)
        {
            if ((mGenrepList[i].mWorldID == nWorldID) && (mGenrepList[i].mStationID == nStationID))
                break;
        }
    }

    return i;
}

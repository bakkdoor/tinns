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

 udp_reqinfo.cpp - decoder classes for UDP Char/Clan/Map info request messages

 CREATION: 6 Sep 2006 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "udp_reqinfo.h"

/**** PUdpCharInfo ****/

PUdpReqInfo::PUdpReqInfo(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    mRequestType = 0;
    nDecodeData->mName << "/0x06";
}

PUdpMsgAnalyser* PUdpReqInfo::Analyse()
{
    PMessage* tmpMsg = mDecodeData->mMessage;

    mDecodeData->mName << "=Info Request";

    tmpMsg->SetNextByteOffset(mDecodeData->Sub0x13Start + 7);
    (*tmpMsg) >> mRequestType;
    (*tmpMsg) >> mInfoId;

    switch(mRequestType)
    {
        case 0:
        {
            mDecodeData->mName << "/0x00=Char Name";
            break;
        }
        case 1:
        {
            mDecodeData->mName << "/0x01=Clan Long Name";
            break;
        }
        case 4:
        {
            mDecodeData->mName << "/0x04=Clan Short Name";
            break;
        }
        case 5:
        {
            mDecodeData->mName << "/0x05=Clan Rank Name";
            break;
        }
        case 7:
        {
            mDecodeData->mName << "/0x07=World Map";
            break;
        }
        default:
        {
            mDecodeData->mUnknownType = mRequestType;
            return new PUdpMsgUnknown(mDecodeData);
        }
    }

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpReqInfo::DoAction()
{
    MYSQL_RES* result = NULL;
    MYSQL_ROW row;
    char query[255];
    char txtmsg[32];
    void* Answer = NULL;
    int len;

    query[0]='\0';

    switch (mRequestType)
    {
        case 0: //Name Request
        //Console->Print("Client %d (char %d): Character Name Request for CharID %i", mDecodeData->mClient->GetID(), mDecodeData->mClient->GetCharID(), mInfoId);
        sprintf (query, "SELECT c_name FROM characters WHERE c_id = %i", mInfoId);
        break;
        case 1: //Clan Long Name
        //Console->Print("Client %d : Clan Long Name Request for ClanID %i", mDecodeData->mClient->GetID(), mInfoId);
        sprintf (query, "SELECT cl_lname FROM clans WHERE cl_id = %i", mInfoId);
        break;
        case 4: //Clan Short name
        //Console->Print("Client %d : Clan Short Name Request for ClanID %i", mDecodeData->mClient->GetID(), mInfoId);
        sprintf (query, "SELECT cl_sname FROM clans WHERE cl_id = %i", mInfoId);
        break;
        case 5: //Clan Rank
        //Console->Print("Client %d : Clan Rank Name Request for ClanID %i", mDecodeData->mClient->GetID(), mInfoId);
        //sprintf (query, "SELECT cl_sname FROM clans WHERE cl_id = %i", mInfoId);
        sprintf(txtmsg, "Undef");
        Answer = txtmsg;
        break;
        case 7: //World Map
        //Console->Print("Client %d : World Map Request", mDecodeData->mClient->GetID());
        // Process request
        break;
        default:
        Console->Print(RED, BLACK, "Client %d : Unhandled Info Request Type %i, Id %i", mDecodeData->mClient->GetID(), mRequestType, mInfoId);
        return false;
        break;
    }

    if (query[0])
    {
        result = MySQL->GameResQuery(query);
        if(!result)
        {
            Console->Print(RED, BLACK, "%s Cannot get do SQL query %s ; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"), query);
            MySQL->ShowGameSQLError();
            sprintf(txtmsg, "Error");
            Answer = txtmsg;
        }
        else
        {
            if(mysql_num_rows(result) == 0)
            {
                sprintf(txtmsg, "Unknown");
                Answer = txtmsg;
            }
            else
            {
                row = mysql_fetch_row(result);
                Answer = row[0];
            }
        }
    }

    if (mRequestType == 7)
    {
        u8 WorldMapData[] = { // Temp map for test, with Attack None/GR All security setting
                                0xd6, 0x07, 0x05, // BT@Simmons
                                0xfe, 0x07, 0x0b, // CM // No effect: not on OP zone
                                0xea, 0x07, 0x03, // Next // No effect: not on OP zone
                                0x12, 0x08, 0x0f, // TG@Gravis !!! Kick them out !!!
                            };
        len = sizeof(WorldMapData);
        Answer = (void*) WorldMapData;
    }
    else
    {
        if (!Answer)
        {
            Console->Print(RED, BLACK, "PUdpReqInfo::DoAction() : Answer not defined !!!", Console->ColorText(RED, BLACK, "[Error]"));
            return false;
        }
        len = (int)(strlen((char*)Answer)+1);
    }

    if(result)
    {
        MySQL->FreeGameSQLResult(result);
    }

    PMessage* tmpMsg = MsgBuilder->BuildReqInfoAnswerMsg(mDecodeData->mClient, mRequestType, mInfoId, Answer, len);
    mDecodeData->mClient->getUDPConn()->SendMessage(tmpMsg);

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return (query[0]);
}

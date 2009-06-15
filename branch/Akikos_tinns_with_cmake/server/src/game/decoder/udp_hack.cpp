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
	udp_hack.cpp - decoder classes for UDP hacking related messages

	CREATION: 30 Dec 2006 Namikon

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "udp_hack.h"

#include "include/worlds.h"
#include "include/furnituretemplate.h"
#include "include/doortemplate.h"


/**** PUdpHackInit ****/

PUdpHackInit::PUdpHackInit(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x00";
}

PUdpMsgAnalyser* PUdpHackInit::Analyse()
{
    mDecodeData->mName << "=Hackgame announced";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackInit::DoAction()
{
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

/**** PUdpHackSuccess ****/

PUdpHackSuccess::PUdpHackSuccess(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x29";
}

PUdpMsgAnalyser* PUdpHackSuccess::Analyse()
{
    mDecodeData->mName << "=Hackgame was successfull";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackSuccess::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();
    PWorld* CurrentWorld = Worlds->GetWorld(tChar->GetLocation());
    const PFurnitureItemTemplate* tFurnitureTemplate = NULL;
    const PDefWorldModel* tFurnitureModel = NULL;

    u32 mRawItemID = mDecodeData->mMessage->U32Data(mDecodeData->Sub0x13Start+8);
    if(gDevDebug) Console->Print("DEBUG: Client %d successfully hacked object %d", mDecodeData->mClient->GetID(), mRawItemID);

    bool tHandleDynamicActor = false;
    // First try to find out if we have an dynamic worldactor
    if(WorldActors->IsDynamicActor(mRawItemID) == true)
    {
        // Now get the get the function value: (What kind of hackable object)
        int tFunctionVal = WorldActors->GetWorldActorFunctionID(mRawItemID);

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc. Here: Hack difficult
        tFurnitureModel = GameDefs->WorldModels()->GetDef(tFunctionVal);
        tHandleDynamicActor = true;
    }
    else
    {
        // Dat files have smaller IDs
        u32 ItemID = mRawItemID/1024 -1;

        // Now grab the template from .dat file
        tFurnitureTemplate = CurrentWorld->GetFurnitureItemTemplate(ItemID);

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc
        tFurnitureModel = CurrentWorld->GetFurnitureItemModel(ItemID);
    }


    if(tHandleDynamicActor == false && tFurnitureTemplate != NULL)
    {
        if (tFurnitureTemplate->GetLinkedObjectID())
        {
            PMessage* tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor(tFurnitureTemplate->GetLinkedObjectID())->IsDoubleDoor());
            ClientManager->UDPBroadcast(tmpMsg, nClient);
        }
        else
        {
            PMessage* tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg(nClient, mRawItemID);
            nClient->SendUDPMessage(tmpMsg);
        }
    }
    else
    {
        if (WorldActors->GetLinkedObjectID(mRawItemID))
        {
            PMessage* tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+WorldActors->GetLinkedObjectID(mRawItemID), CurrentWorld->GetDoor(WorldActors->GetLinkedObjectID(mRawItemID))->IsDoubleDoor());
            ClientManager->UDPBroadcast(tmpMsg, nClient);
        }
        else
        {
            PMessage* tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg(nClient, mRawItemID);
            nClient->SendUDPMessage(tmpMsg);
        }
    }

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}


/**** PUdpHackFail ****/

PUdpHackFail::PUdpHackFail(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x2C";
}

PUdpMsgAnalyser* PUdpHackFail::Analyse()
{
    mDecodeData->mName << "=Hackgame failed";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackFail::DoAction()
{
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

/**** PUdpHackStart ****/

PUdpHackStart::PUdpHackStart(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x20";
}

PUdpMsgAnalyser* PUdpHackStart::Analyse()
{
    mDecodeData->mName << "=Start hackgame";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpHackStart::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();
    PWorld* CurrentWorld = Worlds->GetWorld(tChar->GetLocation());
    const PFurnitureItemTemplate* tFurnitureTemplate = NULL;
    const PDefWorldModel* tFurnitureModel = NULL;

    u32 mRawItemID = mDecodeData->mMessage->U32Data(mDecodeData->Sub0x13Start+24);
    //if(gDevDebug) Console->Print("Client %d wants to hack itemID %d ***not managed yet***", mDecodeData->mClient->GetID(), mRawItemID);

    // First try to find out if we're hacking an dynamic actor
    if(WorldActors->IsDynamicActor(mRawItemID) == true)
    {
        // Now get the get the function value: (What kind of hackable object)
        int tFunctionVal = WorldActors->GetWorldActorFunctionID(mRawItemID);

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc. Here: Hack difficult
        tFurnitureModel = GameDefs->WorldModels()->GetDef(tFunctionVal);
    }
    else
    {
        // Dat files have smaller IDs
        u32 ItemID = mRawItemID/1024 -1;

        // Now grab the template from .dat file
        tFurnitureTemplate = CurrentWorld->GetFurnitureItemTemplate(ItemID);

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc
        tFurnitureModel = CurrentWorld->GetFurnitureItemModel(ItemID);
    }

    if(tFurnitureModel) // We have an valid worldobject? Fine. Then start the hackgame
    {
        u8 tHackDifficult = tFurnitureModel->GetHackDifficulty();
        u8 tHackPenalty = tFurnitureModel->GetHackPenalty();

        // Print it!
        if (gDevDebug) Console->Print("Client trying to hack itemID %d. Hack difficult: %d Hack penalty %d", mRawItemID, tHackDifficult, tHackPenalty);

        PMessage* tmpMsg = MsgBuilder->BuildStartHackGameMsg(nClient, mRawItemID, tHackDifficult);
        nClient->SendUDPMessage(tmpMsg);
        tmpMsg = NULL;
    }

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

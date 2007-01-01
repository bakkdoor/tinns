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

 udp_useobject.cpp - decoder classes for object use related messages

 CREATION: 17 Sep 2006 Hammag

 MODIFIED:
 REASON: -

  TODO: use only RawObjectIDs to avoid complication & errors

*/

#include "main.h"
#include "udp_useobject.h"
#include "worlds.h"
#include "furnituretemplate.h"
#include "doortemplate.h"
#include "appartements.h"

/**** PUdpVhcMove ****/

PUdpUseObject::PUdpUseObject(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x17";
}

PUdpMsgAnalyser* PUdpUseObject::Analyse()
{
    mDecodeData->mName << "=Object use";

    mRawItemID = mDecodeData->mMessage->U32Data(mDecodeData->Sub0x13Start+8);
    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;

    return this;
}

bool PUdpUseObject::DoAction()
{
    PClient* nClient = mDecodeData->mClient;
    PChar* tChar = nClient->GetChar();
    u32 ItemID;
    char DbgMessage[128];
    PMessage* tmpMsg;

    /*PMessage* cMsg = mDecodeData->mMessage;
    u32 ClientTime = cMsg->U32Data(mDecodeData->Sub0x13Start+2);

    tmpMsg = MsgBuilder->BuildPingMsg(mDecodeData->mClient, ClientTime);
    mDecodeData->mClient->getUDPConn()->SendMessage(tmpMsg);*/

    if (gDevDebug)
    {
        Console->Print("Char at y=%f (0x%04x) z=%f (0x%04x) x=%f (0x%04x)", (f32)(tChar->Coords.mY - 32000), tChar->Coords.mY, (f32)(tChar->Coords.mZ - 32000), tChar->Coords.mZ, (f32)(tChar->Coords.mX - 32000), tChar->Coords.mX);
        if (mRawItemID & 1023)
            Console->Print("using item %d (0x%08x)", mRawItemID, mRawItemID);
        else
            Console->Print("using item %d (0x%08x) [%d (0x%08x)]", mRawItemID, mRawItemID, mRawItemID/1024 -1, mRawItemID/1024 -1);
    }

    if(nClient->GetDebugMode(DBG_ITEMID))
    {
        if (mRawItemID & 1023)
            snprintf(DbgMessage, 128, "using item [raw: %d (0x%08x)]", mRawItemID, mRawItemID);
        else
            snprintf(DbgMessage, 128, "using item %d (0x%08x) [raw: %d (0x%08x)]", mRawItemID/1024 -1, mRawItemID/1024 -1, mRawItemID, mRawItemID);
        Chat->send(nClient, CHAT_GM, "Debug", DbgMessage);
    }

    PWorld* CurrentWorld = Worlds->GetWorld(tChar->GetLocation());

    if (mRawItemID & 1023) // non-furniture objects
    {
        if (mRawItemID > 0x80) // maybe door
        {
            if(nClient->GetDebugMode(DBG_ITEMID))
            {
                snprintf(DbgMessage, 128, "Door : %d", mRawItemID - 0x80);
                Chat->send(nClient, CHAT_GM, "Debug", DbgMessage);
            }
            const PDoorTemplate* tDoor = CurrentWorld->GetDoor(mRawItemID - 0x80);
            if (tDoor)
            {
                if (tDoor->IsTriggeredDoor())
                {
                    tmpMsg = MsgBuilder->BuildText100Msg(nClient, 6, mRawItemID);
                    nClient->getUDPConn()->SendMessage(tmpMsg);
                }
                else
                {
                    if (gDevDebug)
                        Console->Print("Opening %s door %d", (tDoor->IsDoubleDoor()?"double":"simple"), mRawItemID - 0x80);
                    tmpMsg = MsgBuilder->BuildDoorOpenMsg(mRawItemID, tDoor->IsDoubleDoor());
                    ClientManager->UDPBroadcast(tmpMsg, nClient);
                }
                mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
            }
        }
        if (!(mDecodeData->mState & DECODE_ACTION_DONE)) // else might be PC, NPC, VHC
        {
            if (gDevDebug)
                Console->Print("Clicing on char, npc or vhc %d", mRawItemID);
            mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
        }
    }
    else // furniture objects
    {
        ItemID = mRawItemID/1024 -1;

        const PFurnitureItemTemplate* tFurnitureTemplate = CurrentWorld->GetFurnitureItemTemplate(ItemID);
        const PDefWorldModel* tFurnitureModel = CurrentWorld->GetFurnitureItemModel(ItemID);
        if (tFurnitureModel) // valid active furniture (else invalid or passive furniture)
        {
            if(nClient->GetDebugMode(DBG_ITEMID))
            {
                snprintf(DbgMessage, 128, "Item : %s", tFurnitureModel->GetName().c_str());
                Chat->send(nClient, CHAT_GM, "Debug", DbgMessage);
            }

            if(tFurnitureModel->GetUseFlags() & ufChair)
            {
                if (CurrentWorld->CharUseChair(nClient->GetLocalID(), ItemID))
                {
                    u32 ChairInUse = tChar->GetChairInUse();
                    if (gDevDebug)
                        Console->Print("Localchar %d was previously using chair %d.", nClient->GetLocalID(), ChairInUse);
                    if((ChairInUse) && (ChairInUse != ItemID))
                    {
                        CurrentWorld->CharLeaveChair(nClient->GetLocalID(), ChairInUse);
                    }
                    tChar->SetChairInUse(ItemID);

                    tFurnitureTemplate->GetFrontPos(&(tChar->Coords.mX), &(tChar->Coords.mY), &(tChar->Coords.mZ));
                    (tChar->Coords).mLR = tFurnitureTemplate->GetFrontLR();

                    tmpMsg = MsgBuilder->BuildCharUseChairMsg(nClient, mRawItemID);
                    ClientManager->UDPBroadcast(tmpMsg, nClient);
                }
                else
                {
                    tmpMsg = MsgBuilder->BuildText100Msg(nClient, 1, mRawItemID); // "Already in use" msg
                    nClient->getUDPConn()->SendMessage(tmpMsg);
                }

                mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
            }
            else
            {
                if (gDevDebug)
                    Console->Print("Item function type: %d value: %d", tFurnitureModel->GetFunctionType(), tFurnitureModel->GetFunctionValue());
                switch(tFurnitureModel->GetFunctionType())
                {

                    case 6: //Respawn Station (GenRep)
                    {
                        u32 nLocation = tChar->GetLocation();
                        u16 nEntity;
                        if (Worlds->IsPotentialAppartement(nLocation))
                        {
                            nLocation = 0xffffffff; // (u32)-1;
                            nEntity = 0xffff; //(u16)-1;
                        }
                        else
                        {
                            //nEntity = MySQL->GetWorldItemOption(mRawItemID/256, nLocation, 1);
                            // This is a kind of nearly-not-hardcoded-hack ...
                            int nEntityInt = GameDefs->GetRespawnEntity(tChar->GetLocation(), tFurnitureTemplate->GetLinkedObjectID());
                            nEntity = (nEntityInt < 0 ? 0xffff : (u16)nEntityInt);
                        }

                        tmpMsg = MsgBuilder->BuildCharUseGenrepMsg(nClient, mRawItemID, nLocation, nEntity);
                        nClient->getUDPConn()->SendMessage(tmpMsg);
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    case 7: //GoGuardian
                    {
                        tmpMsg = MsgBuilder->BuildCharUseGogoMsg(nClient);
                        nClient->getUDPConn()->SendMessage(tmpMsg);
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    case 9: //Appartement Eingang
                    {
                        tmpMsg = MsgBuilder->BuildCharUseLiftMsg(nClient, mRawItemID, tFurnitureModel->GetFunctionValue());
                        nClient->getUDPConn()->SendMessage(tmpMsg);
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    case 10: //Appartement Ein/Ausgang
                    {
                        tmpMsg = MsgBuilder->BuildCharUseLiftMsg(nClient, mRawItemID, 0);
                        nClient->getUDPConn()->SendMessage(tmpMsg);
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    case 15: //HOLOMATCH EXIT
                    { // temp hack + wrong entity... guess it works same as function 20
                        u32 HoloNum = tChar->GetLocation() - 90000; // value 1 to 16, to transalte to 540..547 550..557 for worldmodel.def

                        const PDefWorldModel* tHoloExitModel = GameDefs->GetWorldModelDef( 539 + HoloNum + (HoloNum > 8 ? 2 : 0 ));
                        if ( tHoloExitModel && (tHoloExitModel->GetFunctionType() == 14)) // we use the Holomatch entry. Takes care of bad zone id
                        {
                            const PDefAppPlace* nAppPlace = GameDefs->GetAppPlaceDef(tHoloExitModel->GetFunctionValue());
                            if(nAppPlace)
                            {
                                u32 Location = nAppPlace->GetExitWorldID();
                                u16 Entity = nAppPlace->GetExitWorldEntity();
                                u8 SewerLevel = 0;

                                tmpMsg = MsgBuilder->BuildChangeLocationMsg(nClient, Location, Entity, SewerLevel, 0); //mRawItemID
                                nClient->getUDPConn()->SendMessage(tmpMsg);
                                if (gDevDebug)
                                    Console->Print("%s Client[%d] Char[%s] moving to zone %d (%s)", Console->ColorText(GREEN, BLACK, "[Debug]"), nClient->GetID(), tChar->GetName().c_str(), Location, nAppPlace->GetName().c_str());
                                /*if (gDevDebug)*/
                                Console->Print("%s Location=%d Entity=%d Level=%d", Console->ColorText(GREEN, BLACK, "[Debug]"), Location, Entity, SewerLevel);
                            }
                            else
                            {
                                Console->Print("%s Client[%d] Char[%s] invalid destination %d (appplaces.def)", Console->ColorText(RED, BLACK, "[Warning]"), nClient->GetID(), tChar->GetName().c_str(), tFurnitureModel->GetFunctionValue());
                            }
                        }
                        else
                        {
                            Console->Print("%s Client[%d] Char[%s] invalid holoentry used %d", Console->ColorText(RED, BLACK, "[Warning]"), nClient->GetID(), tChar->GetName().c_str(), tFurnitureModel->GetFunctionValue());
                        }
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }
                    case 18: // "WORLDCHANGEACTOR"
                    case 20: // "DATFILE WORLDCHANGE ACTOR" // yet to find out how to get the right destination entity
                    case 29: //Underground Exit
                    {
                        const PDefAppPlace* nAppPlace;
                        if (tFurnitureModel->GetFunctionType() == 29)
                        {
                            nAppPlace = GameDefs->GetAppPlaceDef(tChar->GetLocation()); // special for UG exit
                        }
                        else
                        {
                            nAppPlace = GameDefs->GetAppPlaceDef(tFurnitureModel->GetFunctionValue());
                        }
                        if(nAppPlace)
                        {
                            u32 Location = nAppPlace->GetExitWorldID();
                            u16 Entity = nAppPlace->GetExitWorldEntity();
                            u8 SewerLevel = 0;
                            //SewerLevel = nAppPlace->GetSewerLevel();
                            //SewerLevel = (tFurnitureModel->GetFunctionType() == 18 ? 0 : 1);

                            tmpMsg = MsgBuilder->BuildChangeLocationMsg(nClient, Location, Entity, SewerLevel, 0); //mRawItemID
                            nClient->getUDPConn()->SendMessage(tmpMsg);
                            if (gDevDebug)
                                Console->Print("%s Client[%d] Char[%s] moving to zone %d, %s", Console->ColorText(GREEN, BLACK, "[Debug]"), nClient->GetID(), tChar->GetName().c_str(), Location, nAppPlace->GetName().c_str());
                            if (gDevDebug)
                                Console->Print("%s Location=%d Entity=%d Level=%d", Console->ColorText(GREEN, BLACK, "[Debug]"), Location, Entity, SewerLevel);
                        }
                        else
                        {
                            Console->Print("%s Client[%d] Char[%s] invalid destination %d (appplaces.def)", Console->ColorText(RED, BLACK, "[Warning]"), nClient->GetID(), tChar->GetName().c_str(), tFurnitureModel->GetFunctionValue());
                            // send a "refused" msg ?
                        }
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        //nextAnalyser = new PUdpCharJump(mDecodeData);
                        break;
                    }

                    case 2: //Terminal
                    case 3: //Outfitter
                    case 16: //HOLOMATCH REFRESH
                    case 17: //HOLOMATCH HEAL & Recreation units
                    case 26: //Outpost Switch
                    case 28: //Fahrzeug Depot Interface
                    case 31: //Venture Warp Station
                    {
                        tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg(nClient, mRawItemID);
                        nClient->getUDPConn()->SendMessage(tmpMsg);
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    case 11: //Appartement Klingel/Öffner
                    {
                        if (Appartements->CanFreelyEnter(tChar, tChar->GetLocation()))
                        {
                            if (tFurnitureTemplate->GetLinkedObjectID())
                            {
                                tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor(tFurnitureTemplate->GetLinkedObjectID())->IsDoubleDoor());
                                ClientManager->UDPBroadcast(tmpMsg, nClient);
                            }
                            else
                            {
                                tmpMsg = MsgBuilder->BuildText100Msg(nClient, 0, mRawItemID);
                                nClient->getUDPConn()->SendMessage(tmpMsg);
                            }
                        }
                        else
                        {
                            tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(nClient, mRawItemID, 5); // Ring
                            ClientManager->UDPBroadcast(tmpMsg, nClient);
                        }
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    case 12: //Standard Button
                    {
                        if (tFurnitureTemplate->GetLinkedObjectID())
                        {
                            tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor(tFurnitureTemplate->GetLinkedObjectID())->IsDoubleDoor());
                            ClientManager->UDPBroadcast(tmpMsg, nClient);
                        }
                        else
                        {
                            tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg(nClient, mRawItemID);
                            nClient->getUDPConn()->SendMessage(tmpMsg);
                        }
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }
                    case 13: //Hack Button
                    {
                        if (tFurnitureTemplate->GetLinkedObjectID())
                        {
                            if(nClient->GetAccount()->GetLevel() >= PAL_GM) // Allow GameMasters and higher to just bypass HackButtons
                            {
                                tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor(tFurnitureTemplate->GetLinkedObjectID())->IsDoubleDoor());
                                ClientManager->UDPBroadcast(tmpMsg, nClient);
                            }
                            else
                            {
                                tmpMsg = MsgBuilder->BuildTextIniMsg(nClient, 6, 106); // Damn, locked!
                                nClient->getUDPConn()->SendMessage(tmpMsg);
                            }

                        }
                        else
                        {
                            tmpMsg = MsgBuilder->BuildText100Msg(nClient, 0, mRawItemID);
                            nClient->getUDPConn()->SendMessage(tmpMsg);
                        }
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    case 23: //EINTRITTSGELD BUTTON
                    {
                        if (tFurnitureTemplate->GetLinkedObjectID())
                        {
                            u32 OldCash = tChar->GetCash();
                            u32 DoorFee = (u32)tFurnitureModel->GetFunctionValue();
                            if(OldCash >= DoorFee)
                            {
                                u32 NewCash = tChar->SetCash(OldCash - DoorFee);
                                PMessage* tmpMsg_cash = MsgBuilder->BuildCharMoneyUpdateMsg(nClient, NewCash);
                                nClient->getUDPConn()->SendMessage(tmpMsg_cash);
                                tmpMsg_cash = NULL;
                                tmpMsg = MsgBuilder->BuildDoorOpenMsg(0x80+tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor(tFurnitureTemplate->GetLinkedObjectID())->IsDoubleDoor());
                                ClientManager->UDPBroadcast(tmpMsg, nClient);
                            }
                            else
                            {
                                tmpMsg = MsgBuilder->BuildTextIniMsg(nClient, 6, 12);  // You dont have enough money!
                                nClient->getUDPConn()->SendMessage(tmpMsg);
                            }
                        }
                        else
                        {
                            tmpMsg = MsgBuilder->BuildText100Msg(nClient, 0, mRawItemID);
                            nClient->getUDPConn()->SendMessage(tmpMsg);
                        }
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }
                    case 1: //Itemcontainer
                    {
                        // TODO: Add check if container is already open
                        tmpMsg = MsgBuilder->BuildCharOpenContainerMsg(nClient, mRawItemID);
                        nClient->getUDPConn()->SendMessage(tmpMsg);
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }
                    case 4: //Trader
                    case 5: //Mineral
                    case 8: //Hackterminal

                    case 14: //HOLOMATCH ENTRANCE
                    case 19: //CLANTERMINAL
                    //case 21: //LOCATION FOR 20
                    //case 22: //
                    case 24: //TUTORIALEXIT
                    case 25: //EXPLOSIVE
                    case 27: //Old goguardian
                    //case 30: //Static FX (Value=Type. 1=Fire 2=Smoke 3=Steam 4=Sparkle)
                    {
                        tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg(nClient, mRawItemID);
                        nClient->getUDPConn()->SendMessage(tmpMsg);
                        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                        break;
                    }

                    default:
                    break;
                }
            }
        }
        else
        {
            if (gDevDebug)
                Console->Print("Item not known from world template (maybe seen as PASSIVE ?)");
        }
    }

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

/**** PUdpCloseItemContainer ****/

PUdpCloseItemContainer::PUdpCloseItemContainer(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
    nDecodeData->mName << "/0x27";
}

PUdpMsgAnalyser* PUdpCloseItemContainer::Analyse()
{
    mDecodeData->mName << "=Closing item container";

    mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
    return this;
}

bool PUdpCloseItemContainer::DoAction()
{
    // TODO: free container that is currently in use by localchar
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}


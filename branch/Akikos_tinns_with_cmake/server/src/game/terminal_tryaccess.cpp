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
	terminal_tryaccess.cpp - Management class for Terminal actions (Citycom, keys, vehicledepot, ...)
				 Sub-File for terminal "TryAccess" command

	MODIFIED: 12 Jan 2007 Namikon
	REASON: - Created
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/terminal.h"


void PTerminal::SendTryAccessAnswer(PClient* nClient, char *nArea, bool nAllowed)
{
    u8 i = (u8)strlen(nArea);
    if(gDevDebug) Console->Print("MessageLengh: %d", i);

    nClient->IncreaseUDP_ID();
    PMessage* tmpMsg = new PMessage(i+17);

    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u16)nClient->GetSessionID();
    *tmpMsg << (u8)(11+i);
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u8)0x2b;
    *tmpMsg << (u8)0x1a;
    *tmpMsg << (u16)(i+1);

    if(nAllowed) *tmpMsg << (u8)0x01;
    else *tmpMsg << (u8)0x00;

    *tmpMsg << (u8)0x00;
    *tmpMsg << (u8)0x00;
    *tmpMsg << nArea;

    if(gDevDebug) tmpMsg->Dump();
    nClient->SendUDPMessage(tmpMsg);
}

void PTerminal::CheckAccess(PClient* nClient, char *nArea, u16 nCmdNr, char *nOption1, char *nOption2, char *nOption3)
{
    if(!nClient)
        return;

    if(gDevDebug) Console->Print("PTerminal::CheckAccess Received TryAccess request: %s CmdNr: %d (Options: %s %s %s)", nArea, nCmdNr, nOption1, nOption2, nOption3);

/* Todo:
DBAccess
DCBAcceptProclaimedWar
DCBAdminChangeLeader
DCBAdminChangeManifesto
DCBAdminChangeRank
DCBAdminChangeRepresentative
DCBAdminDismissMember
DCBAdminInviteMember
DCBAdminLeaderAccess
DCBAdminLevelAccess
DCBAdminMemberAccess
DCBAdminMoneyAccess
DCBAdminWarAccess
DCBBuyingForChecker
DCBCancelProclaimedWar
DCBChangeApartment
DCBChangeApartmentAccessCode
DCBChangeClanDescription
DCBChangeClanLeader
DCBChangeClanMinSympathy
DCBChangeClanName
DCBChangeClanRepresentative
DCBChangeClanShortName
DCBChangeEnemyStatement
DCBChangeInitiatorStatement
DCBChangeName
DCBCheckClanLeader
DCBCheckClanMember
DCBCheckMemberIDGreaterThanZero
DCBCheckStockxAmount
DCBClanAdminAccess
DCBClanForumControl
DCBDeleteCapitulatedWar
DCBDeleteClan
DCBDeleteExistingClanRank
DCBDeleteProclaimedWar
DCBDeleteThisFuckingClan
DCBEnemyCapitulate
DCBFactionForumAccess
DCBFactionForumControl
DCBGetOutOfMySight
DCBGetRepresentativeAccessMessage
DCBGetValue
DCBGMAccess
DCBGMChecker
DCBGMDeleter
DCBGMSelfProtector
DCBIgnoreProclaimedWar
DCBInitiatorCapitulate
DCBInviteRunner
DCBKillAndDestroyIt
DCBLetsBuyItCom
DCBLetsSellItCom
DCBLetsTrade
DCBLevelChecker
DCBLinkToClanForum
DCBLinkToFactionForum
DCBProclaimWar
DCBRepresentativeAccess
DCBResetPosition
DCBSellingForChecker
DCBSetJobToken
DCBSetNewRunnerWorld
DCBSetNewRunnerWorld
DCBSetParamsForTransaction
DCBSubmitClanTransaction
DCBSubmitFactionTransaction
DCBSubmitNewClanRank
DCBSubmitNewRank
DCBTryKickRunner
DCBTryWarp2GM
DCBUpdateManifesto
DCBWarpRequestRunnerToWorld
*/
    bool tAllowed = false;
    int nAccessLevel = nClient->GetAccountLevel();

    // Start
    if (!strcmp ("DCBLinker", nArea)) //Initial GM check for HELP menu
    {
        if(nAccessLevel >= PAL_VOLUNTEER)
            tAllowed = true;
    }
    else if (!strcmp ("DCBGMLinker", nArea)) //GM Check for all HELP menus
    {
        switch (nCmdNr) // Switch accesslevel to check. Maybe move this to commands.conf?
        {
            case 310:
                if(nAccessLevel > PAL_REGPLAYER)
                    tAllowed = true;
            break;

            case 315:
                if(nAccessLevel >= 15)
                    tAllowed = true;
            break;

            case 320:
                if(nAccessLevel >= 20)
                    tAllowed = true;
            break;

            case 330:
                if(nAccessLevel >= PAL_VOLUNTEER)
                    tAllowed = true;
            break;

            case 340:
                if(nAccessLevel >= PAL_GM)
                    tAllowed = true;
            break;

            case 350:
                if(nAccessLevel >= PAL_ADMIN)
                    tAllowed = true;
            break;

            default:
                if(nAccessLevel >= PAL_ADMIN)
                    tAllowed = true;
            break;
        }
    }
    else if (!strcmp ("DCBWarpRequestToRunner", nArea)) //Warp To Runner command check
    {
        if(nAccessLevel >= PAL_VOLUNTEER)
            tAllowed = true;
    }
    else if (!strcmp ("DCBTryWarpRunner2Me", nArea)) //Warp To GM command check
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    else if (!strcmp ("DCBTryWarp2Runner", nArea)) //Warp To Runner command check
    {
        if(nAccessLevel >= PAL_VOLUNTEER)
            tAllowed = true;
    }
    else if (!strcmp ("DCBWarpRequestToGamemaster", nArea)) //Warp To GM command check
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    else if (!strcmp ("DCBSetJobDeleted", nArea)) //Delete Job
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    else if (!strcmp ("DCBSetJobFree", nArea)) //Give up Job
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    else if (!strcmp ("DCBSetJobFinished", nArea)) //Finish Job
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    else if (!strcmp ("DCBGetStatistics", nArea)) //Get Statistics
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    else if (!strcmp ("DCBLinkToForum", nArea)) //Link To forum
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    else if (!strcmp ("DCBGMSubmitter", nArea)) //Submit
    {
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
    }
    /****************************\
    *	CityCom Access Rights	 *
    \****************************/
    else if (!strcmp ("DCBSetup", nArea)) //Citycom access check
    {
        tAllowed = true;
    }
    //Time Checker
    else if (!strcmp ("DCBNeocronTimeChecker", nArea))
    {
        tAllowed = true;
    }
    //Contact list description check
    else if (!strcmp ("DCBOverflowChecker", nArea))
    {
        tAllowed = true;
    }
    //Forum set
    else if (!strcmp ("DCBSetCategoryValuesForForum", nArea))
    {
        tAllowed = true;
    }
    else //Don't know, check if ADMIN rights. Warn ADMIN and let him use it.
    {
        Console->Print("%s Unhandled access check: %s", Console->ColorText(YELLOW, BLACK, "[Warning]"), nArea);
        if(nAccessLevel >= PAL_ADMIN)
            tAllowed = true;
    }
    // Stop

    // Now lets see if client is allowed to use given function
    SendTryAccessAnswer(nClient, nArea, tAllowed);
}

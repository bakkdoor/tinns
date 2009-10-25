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
 > Sub-File for terminal "TryAccess" command

 MODIFIED: 12 Jan 2007 Namikon
 REASON: - Created
 MODIFIED: 20 Oct 2009 Namikon
 REASON: - Rewritten

*/

#include "main.h"
#include "terminal.h"
#include "msgbuilder.h"

bool PTerminal::DoStockXCheck(PClient* nClient, int nAmountEntered, int nNewAmount)
{
    MYSQL_RES *result = NULL;
    char qry[100];
    int tDepotAmount = 0;

    // First check if we BUY or SELL stockx
    snprintf(qry, 100, "SELECT sxd_amount FROM stockx_depots WHERE sxd_playerid = %d", nClient->GetChar()->GetID());
    result = MySQL->GameResQuery(qry);
    if (!result)
    {
        Console->Print("%s [PTerminal::DoStockXCheck] Cannot query DB. Query was: %s", Console->ColorText(RED, BLACK, "Error"), qry);
        MySQL->ShowGameSQLError();
        return false;
    }

    if(mysql_num_rows(result) > 0)
        tDepotAmount = atoi(mysql_fetch_row(result)[0]);

    MySQL->FreeGameSQLResult(result);

    if(nNewAmount == tDepotAmount) // We are going to SELL stockx
    {
        if(nAmountEntered > tDepotAmount) // Want to sell more than we have?
        {
            return false;
        }
    }
    else if((nNewAmount - nAmountEntered) == tDepotAmount) // We are going to BUY stockx
    {
        if(nNewAmount > 10000) // Hardcoded limit of 10k per faction
        {
            return false;
        }
    }
    else // Strange unknown thing, return false just to be sure!
    {
        return false;
    }

    // Everything went fine
    return true;
}

bool PTerminal::HandleTryAccess(PClient* nClient, u16 mTerminalSessionId, std::string *nCommandName, std::string *nOptions, u8 nNumOptions, u16 nDBID, u8 nUnknown, bool nCheckOnly)
{
    // Empty buffer
    EraseVars();

    // Stop gcc annoying about unused var
    u8 dummy2 = nUnknown;
    u16 dummy3 = mTerminalSessionId;
    dummy3 = dummy2;

    char mSQLQuery[500];
    bool tGetResultFromSQL = false;
    bool tAllowed = false;
    int nAccessLevel = nClient->GetAccountLevel();
    int tCharID = nClient->GetChar()->GetID();

    switch(nDBID)
    {
        case 13: // Access to Neocronicle as GameMaster
        if(nAccessLevel >= PAL_VOLUNTEER)
            tAllowed = true;
        break;

        case 63: // Access to Outpost security status; ClanLevel >= 3
            if(!ChkOpt(nNumOptions, 1)) break;
            tGetResultFromSQL = true;
            snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) INNER JOIN outposts ON (o_clan = cl_id) WHERE (o_outnum = %d AND cll_charid = %d AND cll_level >= 3)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 64: // Clan: ClanWars
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 14)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 72: // Clan: AdminMemberAccess & AdminMoneyAccess
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 9)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 73: // Clan: AdminInviteMember
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 10)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 74: // Clan: AdminLevelAccess & AdminChangeRank
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 12)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 88: // Clan: Money transfer Check if Option1 (Entered value) is lower or equal Option2 (Either my money or clan money)
        if(!ChkOpt(nNumOptions, 2)) break;
        if((atoi(nOptions[0].c_str()) <= atoi(nOptions[1].c_str())) && (atoi(nOptions[0].c_str()) > 0 )) // This is an not-very-well check, we do the real check later when its about to transfer the money!
            tAllowed = true;
        break;

        case 95: // Clan: Check if given TextString is valid (Opt1 = String)
        if(!ChkOpt(nNumOptions, 1)) break;
        if(atoi(nOptions[0].c_str()) >= 0 && atoi(nOptions[0].c_str()) <= 15)
            tAllowed = true;
        break;

        case 110: // GMTool Access check. If GM, allowed
        if(nAccessLevel > PAL_VOLUNTEER)
            tAllowed = true;
        break;

        case 184: // Clan: Delete (Opt1: ClanID Opt2: CharID Opt3: ClanMemberCount) Allow clandelete only when Leader is the only one left
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        // The result of this query is 0 (false) if wrong clan, is 1 (true) when clan is correct and leader is the only member left and <membercount> (false, not 1) if there are members left
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM characters INNER JOIN clans ON (c_clan = cl_id) WHERE cl_id = %d AND cl_leader = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        break;

        case 201: // Clan: Invite Member. Check if target ID has already a clan and check if he is online right now. Location doesnt matter for us :)
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM characters WHERE c_id = %d AND c_clan = 0 AND c_online = 1", atoi(nOptions[0].c_str()));
        break;

        case 202: // CityCom and Check if given ID is > 0
        if(!ChkOpt(nNumOptions, 1)) break;
        if(atoi(nOptions[0].c_str()) > 0)
            tAllowed = true;
        break;

        case 208: // Clan: AdminDismissMember & ClanAdminAccess & AdminInviteMember // General ClanAdmin access in CityCom
        if(!ChkOpt(nNumOptions, 1)) break;
        if(nClient->GetChar()->GetClan() == atoi(nOptions[0].c_str()))
            tAllowed = true;
        break;

        case 209: // Give money to clan. Level 0+ ? Just check if clan = clan here..
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 214: // Take money from clan. Level 9+
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 9)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 215:
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 10)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 216:
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 12)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 217: // Access clanwars lv 14+
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clanlevels INNER JOIN clans ON (cll_clanid = cl_id) WHERE (cl_id = %d AND cll_charid = %d AND cll_level >= 14)", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 218: // Leader only. Access to "misc" section of clans
        if(!ChkOpt(nNumOptions, 1)) break;
        tGetResultFromSQL = true;
        snprintf(mSQLQuery, 500, "SELECT count(*) FROM clans WHERE cl_id = %d AND cl_leader = %d", atoi(nOptions[0].c_str()), tCharID);
        break;

        case 239: // StockX: Buy/Sell/Trade
        if(!ChkOpt(nNumOptions, 2)) break;
        // We checked stockx earlier in #240. Now just proof that the calculated values are correct. EG: Option 1 > Option 2
        if(atoi(nOptions[0].c_str()) > atoi(nOptions[1].c_str()))
            tAllowed = true;
        break;

        case 240: // StockX. Need special handling
        if(!ChkOpt(nNumOptions, 2)) break;
        tAllowed = DoStockXCheck(nClient, atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        break;

        case 259: // CityComSetup: SetCategorys for Forum | Check if Runner is ProNC or ProDoY
        if(nClient->GetChar()->GetFaction() < 7 || nClient->GetChar()->GetFaction() == 11) // 1-7 ProNC, 8-15 ProDoY 11: CityMercs
            tAllowed = true;
        break;

        case 264: // Check if player has a faction. Deny access if not (Opt1: FactionID)
        if(!ChkOpt(nNumOptions, 1)) break;
        if(atoi(nOptions[0].c_str()) > 0)
            tAllowed = true;
        break;

        case 269: // Check if player has a clan. Deny access if not (Opt1: ClanID)
        if(!ChkOpt(nNumOptions, 1)) break;
        if(atoi(nOptions[0].c_str()) > 0)
            tAllowed = true;
        break;

        case 310: // Warp to another GameMaster (GM Helper level)
        if(nAccessLevel > PAL_VOLUNTEER)
            tAllowed = true;
        break;

        case 315: // Warp players around, from/to GM kick players
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
        break;

        case 320:
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
        break;

        case 330:
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
        break;

        case 340:
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
        break;

        case 350: // Delete Clans, StockX
        if(nAccessLevel >= PAL_ADMIN)
            tAllowed = true;
        break;

        case 438: // Check if Contactlist has already 20 entries. If so, allow = false
        if(!ChkOpt(nNumOptions, 1)) break;
        if(atoi(nOptions[0].c_str()) < 21)
            tAllowed = true;
        break;

        case 470: // Something with clanranking... have to verify this
        Console->Print("%s [PTerminal::TryAccess] Accesscode 470 is not supportet yet; \"DCBResetPosition\": Option1 [%s]", Console->ColorText(YELLOW, BLACK, "Notice"), nOptions[0].c_str());
        if(nAccessLevel >= PAL_ADMIN)
            tAllowed = true;
        break;

        case 471: // Something with TimeStamp. NC had a problem with GameTime back then, maybe this is a check if ingametime is correct
        if(!ChkOpt(nNumOptions, 2)) break;
        if(atoi(nOptions[0].c_str()) != atoi(nOptions[1].c_str()))
            tAllowed = true;
        break;

        case 586: // Check if player is the current Representive of clan (Opt1: PlayerID Opt2: ClanRepID)
        if(!ChkOpt(nNumOptions, 2)) break;
        if(atoi(nOptions[0].c_str()) == atoi(nOptions[1].c_str()))
            tAllowed = true;
        Console->Print("This ID is marked as CHECK IF ITS WORKING %d", nDBID);
        break;

        case 621: // In text.ini (Section 4 text 330) it says something about clanwars.. Not sure. file: politics\administration.tsc(31)
        Console->Print("%s [PTerminal::TryAccess] Accesscode 621 is not supportet yet; \"GetOutOfMySight\": CharID [%d] FactionRank [%d]", Console->ColorText(YELLOW, BLACK, "Notice"), atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        if(nAccessLevel >= PAL_ADMIN)
            tAllowed = true;
        break;

        case 622: // TakeMoney from Faction wallet. Check if Player is allowed to do this. For now, only GMs may do this
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
        break;

        case 624: // Same as 622, Take/Give money from/to Faction wallet. GM only
        if(nAccessLevel >= PAL_GM)
            tAllowed = true;
        break;

        default:
        Console->Print("%s [PTerminal::TryAccess] Unknown DBID: %d", Console->ColorText(RED,BLACK,"Error"), nDBID);
        return false;
    }


    // ----------------
    if(tGetResultFromSQL == true)
    {
        MYSQL_RES *result = NULL;
        //MYSQL_ROW row;
        int tNumRows = 0;

        result = MySQL->GameResQuery(mSQLQuery);
        if (!result)
        {
            Console->Print("Cannot query DB. DBID: %d query was: %s", nDBID, mSQLQuery);
            MySQL->ShowGameSQLError();
            return false;
        }

        tNumRows = mysql_num_rows(result);
        if(tNumRows == 0)
            tAllowed = false;
        else
        {
            if(atoi(mysql_fetch_row(result)[0]) == 1)
                tAllowed = true;
        }
        MySQL->FreeGameSQLResult(result);
    }
    // ----------------


    if (gDevDebug) Console->Print("[PTerminal::TryAccess] DBID is: [%d] LoopBack: [%s] Allowed: [%d]", nDBID, nCommandName->c_str(), tAllowed);
    if(!nCheckOnly)
    {
        char tCmd[100];
        memset(tCmd, '\0', 100);
        strncpy(tCmd, nCommandName->c_str(), 100);

        PMessage* tmpMsg = MsgBuilder->BuildTryAccessAnswerMsg(nClient, tCmd, tAllowed);
        nClient->SendUDPMessage(tmpMsg);
    }

    return tAllowed;
}

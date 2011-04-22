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
	terminal_upcatedb.cpp - Management class for Terminal actions (Citycom, keys, vehicledepot, ...)
				Sub-File for terminal "UpdateDB" command

	MODIFIED: 12 Jan 2007 Namikon
	REASON: - Created
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"
#include "include/terminal.h"
#include "include/msgbuilder.h"


bool PTerminal::HandleUpdateDB(PClient* nClient, u16 mTerminalSessionId, std::string *nCommandName, std::string *nOptions, u8 nNumOptions, u16 nDBID, u8 nUnknown)
{
    ostringstream tLongSQL; // omg my eyes... Stringstream is one of the worst inventions ever! Stick with printf syntax!!111
    char tShortSQL[1024]; // Use this for small updates that will not exceed 1kb
    memset(tShortSQL, '\0', 1024);
    bool tSuccess = false;

    //Console->Print("UpdateDB ID: %d", nDBID);
    switch (nDBID)
    {
    case 5:
        tLongSQL << "INSERT INTO neochronicle (nc_icon, nc_author, nc_datetime, nc_name, nc_content)";
        tLongSQL << " VALUES (" << atoi(nOptions[0].c_str()) << ", \"" << nOptions[1] << "\", \"" << nOptions[2] << "\", \"" << nOptions[3] << "\", \"" << nOptions[4] << "\")";

        break;
    case 6:
        // UPDATE when Neocronicle DB is changed! author must be CHAR not INT
        break;
    case 7: // Delete neocronicle
        snprintf(tShortSQL, 1024, "DELETE FROM neochronicle WHERE nc_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 12:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 53: // Runner writing to public board
        snprintf(tShortSQL, 1024, "INSERT INTO forum_posts (fp_forumid, fp_replyid, fp_fromid, fp_datetime, fp_name, fp_content) VALUES ((SELECT f_area FROM forums WHERE f_name = \"%s\"), %d, %d, \"%s\", \"%s\", \"%s\")", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), nOptions[3].c_str(), nOptions[4].c_str(), nOptions[5].c_str());
        break;
    case 58:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 62: // Set new outpost security
        snprintf(tShortSQL, 1024, "UPDATE outposts SET o_security = %d WHERE o_outnum = %d AND o_clan = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        break;

    case 66: // Claninfo stuff that is somehow never used... Print error if it is
    case 67:
        Console->Print("%s [Pterminal::HandleUpdateDB] QueryID %d should never happen. Please contact Linux Addited forums!", Console->ColorText(RED,BLACK,"Notice"), nDBID);
        break;

    case 77: // Delete old clanlevel
        snprintf(tShortSQL, 1024, "DELETE FROM clanlevels WHERE cll_clanid = %d AND cll_level = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        break;
    case 78: // Add new clanlevel
        snprintf(tShortSQL, 1024, "INSERT INTO clanlevels (cll_clanid, cll_desc, cll_level) VALUES (%d, \"%s\", %d)", atoi(nOptions[0].c_str()), nOptions[1].c_str(), atoi(nOptions[2].c_str()));
        break;
    case 84: // Take/Give money to/from clan
        tLongSQL << "INSERT INTO moneytransactions (mt_clanid, mt_player, mt_amount, mt_date, mt_comment) ";
        tLongSQL << "VALUES (" << atoi(nOptions[0].c_str()) << ", " << atoi(nOptions[1].c_str()) << ", " << atoi(nOptions[2].c_str()) << ", \"" << nOptions[3] << "\", \"" << nOptions[4] << "\")";
        break;
    case 94: // Update runner description
        snprintf(tShortSQL, 1024, "UPDATE characters SET c_desc = \"%s\" WHERE c_id = %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()));
        break;
    case 97:
        // Just ignore that... KK required some extra updates here
        tSuccess = true;
        //Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 120: // Create support call
        snprintf(tShortSQL, 1024, "INSERT INTO support (su_player, su_worldid, su_type, su_desc) VALUES (%d, %d, %d, \"%s\")", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), nOptions[3].c_str());
        break;
    case 121:
        snprintf(tShortSQL, 1024, "UPDATE support SET su_inwork = 1, su_supporterid = %d WHERE su_id = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        break;
    case 122:
        snprintf(tShortSQL, 1024, "UPDATE support SET su_inwork = 0, su_supporterid = 0 WHERE su_id = %d AND su_supporterid = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        break;
    case 123:
        snprintf(tShortSQL, 1024, "UPDATE support SET su_inwork = 1, su_finished = 1, su_supporterid = %d WHERE su_id = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        break;
    case 124:
        snprintf(tShortSQL, 1024, "DELETE FROM support WHERE su_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 133:
        tSuccess = true;
        //Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 139:
        snprintf(tShortSQL, 1024, "UPDATE characters SET c_location = %d WHERE c_id = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
        break;
    case 142:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 149: // Changeleader 2nd step. Update clanappartment owner
        snprintf(tShortSQL, 1024, "UPDATE apartments SET apt_owner = %d WHERE apt_id = %d AND apt_owner = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        break;
    case 160: // Missions, not yet
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 161: // Missions, not yet
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 162: // Missions, not yet
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 163: // Missions, not yet
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 164: // Missions, not yet
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 165: // Missions, not yet
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 170: // Enter new bug
        snprintf(tShortSQL, 1024, "INSERT INTO bug_report (br_type, br_status, br_title, br_desc, br_location, br_fromid, br_datetime) VALUES (%d, %d, \"%s\", \"%s\", %d, %d, NOW())", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), nOptions[2].c_str(), nOptions[3].c_str(), atoi(nOptions[4].c_str()), atoi(nOptions[5].c_str()));
        break;
    case 172: // Delete bug id %d
        snprintf(tShortSQL, 1024, "DELETE FROM bug_report WHERE br_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 180:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 182:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 235:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 252:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 263: // Runner writing to Faction Board
        snprintf(tShortSQL, 1024, "INSERT INTO forum_posts (fp_forumid, fp_factionid, fp_replyid, fp_fromid, fp_datetime, fp_name, fp_content) VALUES ((SELECT f_area FROM forums WHERE f_name = \"%s\"), %d, %d, %d, \"%s\", \"%s\", \"%s\")", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()), nOptions[4].c_str(), nOptions[5].c_str(), nOptions[6].c_str());
        break;
    case 268: // Runner writing to ClanBoard
        snprintf(tShortSQL, 1024, "INSERT INTO forum_posts (fp_forumid, fp_clanid, fp_replyid, fp_fromid, fp_datetime, fp_name, fp_content) VALUES ((SELECT f_area FROM forums WHERE f_name = \"%s\"), %d, %d, %d, \"%s\", \"%s\", \"%s\")", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()), nOptions[4].c_str(), nOptions[5].c_str(), nOptions[6].c_str());
        break;
    case 370: // GM ClanRepair
        snprintf(tShortSQL, 1024, "INSERT INTO clanlevels (cll_clanid, cll_level, cll_desc) VALUES (%d, %d, \"%s\")", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), nOptions[2].c_str());
        break;
    case 385: // Clandelete 1/7 Clanlevels
        snprintf(tShortSQL, 1024, "DELETE FROM clanlevels WHERE cl_clanid = %d", atoi(nOptions[0].c_str()));
        break;
    case 386: // Clandelete 2/7 Outposts
        snprintf(tShortSQL, 1024, "UPDATE outposts SET o_clan = 0 WHERE o_clan = %d", atoi(nOptions[0].c_str()));
        break;
    case 387: // Clandelete 3/7 Free users from clans
        snprintf(tShortSQL, 1024, "UPDATE characters SET c_clan = 0 WHERE c_clan = %d", atoi(nOptions[0].c_str()));
        break;
    case 388: // Clandelete 4/7 The clan itself
        snprintf(tShortSQL, 1024, "DELETE FROM clans WHERE cl_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 389: // Clandelete 5/7 ?
        tSuccess = true;
        break;
    case 390: // Clandelete 6/7 Clanappartment
        snprintf(tShortSQL, 1024, "DELETE FROM apartments WHERE apt_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 391: // Clandelete 7/7 ?
        tSuccess = true;
        break;
    case 402: // GM writing to publicforum
        snprintf(tShortSQL, 1024, "INSERT INTO forum_posts (fp_forumid, fp_replyid, fp_fromid, fp_datetime, fp_name, fp_content) VALUES ((SELECT f_area FROM forums WHERE f_name = \"%s\"), %d, %d, \"%s\", \"%s\", \"%s\")", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), nOptions[3].c_str(), nOptions[4].c_str(), nOptions[5].c_str());
        break;
    case 404: // GM deleting forum entry step 1
        snprintf(tShortSQL, 1024, "DELETE FROM forum_posts WHERE fp_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 405: // GM deleting forum entry step 2
        snprintf(tShortSQL, 1024, "DELETE FROM forum_posts WHERE fp_replyid = %d", atoi(nOptions[0].c_str()));
        break;
    case 412: // GM writing to faction forum
        snprintf(tShortSQL, 1024, "INSERT INTO forum_posts (fp_forumid, fp_factionid, fp_replyid, fp_fromid, fp_datetime, fp_name, fp_content) VALUES ((SELECT f_area FROM forums WHERE f_name = \"%s\"), %d, %d, %d, \"%s\", \"%s\", \"%s\")", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()), nOptions[4].c_str(), nOptions[5].c_str(), nOptions[6].c_str());
        break;
    case 422: // Mark email as replied
        snprintf(tShortSQL, 1024, "UPDATE emails SET e_replied = 1 WHERE e_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 423: // Write new email
        snprintf(tShortSQL, 1024, "INSERT INTO emails (e_fromid,e_toid,e_datetime,e_subject,e_body) VALUES (%d, %d, \"%s\", \"%s\", \"%s\")", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), nOptions[2].c_str(), nOptions[3].c_str(), nOptions[4].c_str());
        break;
    case 424: // Delete email
        snprintf(tShortSQL, 1024, "DELETE FROM emails WHERE e_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 426: // Mark email as read
        snprintf(tShortSQL, 1024, "UPDATE emails SET e_new = 0 WHERE e_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 432: // Add new contact
        snprintf(tShortSQL, 1024, "INSERT INTO contacts (c_listid,c_conid,c_type,c_desc) VALUES (%d, %d, %d, \"%s\")", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), nOptions[3].c_str());
        break;
    case 433: // Edit contact
        snprintf(tShortSQL, 1024, "UPDATE contacts SET c_type = %d, c_desc = \"%s\" WHERE c_id = %d", atoi(nOptions[0].c_str()), nOptions[1].c_str(), atoi(nOptions[2].c_str()));
        break;
    case 434: // Delete contact
        snprintf(tShortSQL, 1024, "DELETE FROM contacts WHERE c_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 447:
        snprintf(tShortSQL, 1024, "INSERT INTO neochronicle (nc_icon, nc_author, nc_datetime, nc_name, nc_content, nc_lang) VALUES (%d, \"%s\", \"%s\", \"%s\", \"%s\", %d)", atoi(nOptions[0].c_str()), nOptions[1].c_str(), nOptions[2].c_str(), nOptions[3].c_str(), nOptions[4].c_str(), atoi(nOptions[5].c_str()));
        break;
    case 450:
        snprintf(tShortSQL, 1024, "INSERT INTO guides (g_chapter, g_part, g_title, g_content, g_language) VALUES (%d, %d, \"%s\", \"%s\", %d)", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), nOptions[2].c_str(), nOptions[3].c_str(), atoi(nOptions[4].c_str()));
        break;
    case 515: // Create new clanwar
        snprintf(tShortSQL, 1024, "INSERT INTO clanwars (cw_initclan, cw_enemyclan, cw_starttime) VALUES (%d, %d, \"%s\")", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), nOptions[2].c_str());
        break;
    case 516:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 517:
        // Special: Check if Clan of our char is = nOption2
        if(nClient->GetChar()->GetClan() == atoi(nOptions[2].c_str()))
            snprintf(tShortSQL, 1024, "UPDATE clanwars SET cw_status = %d WHERE cw_id = %d AND cw_initclan = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        // else: SQL Query is empty = failed as result
        break;
    case 518:
        if(nClient->GetChar()->GetClan() == atoi(nOptions[2].c_str()))
            snprintf(tShortSQL, 1024, "UPDATE clanwars SET cw_status = %d WHERE cw_id = %d AND cw_enemyclan = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        // else: SQL Query is empty = failed as result
        break;
    case 519:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 525: // Update clan short
        tLongSQL << "UPDATE clans SET cl_shortdesc = \"" << nOptions[0] << "\" WHERE cl_id = " << atoi(nOptions[1].c_str()) << " AND cl_leader = " << atoi(nOptions[2].c_str());

        break;
    case 526: // Update clan name
        tLongSQL << "UPDATE clans SET cl_name = \"" << nOptions[0] << "\" WHERE cl_id = " << atoi(nOptions[1].c_str()) << " AND cl_leader = " << atoi(nOptions[2].c_str());

        break;
    case 527: // Update clanapp password
        //UPDATE apartments SET apt_password = x WHERE apt_owner = x AND apt_id = (SELECT clans.cl_appid WHERE clans.cl_id = x)
        tLongSQL << "UPDATE apartments SET apt_password = \"" << nOptions[0] << "\" WHERE apt_owner = " << atoi(nOptions[2].c_str()) << " AND apt_id = (SELECT cl_appid FROM clans WHERE cl_id = " << atoi(nOptions[1].c_str()) << ")";
        break;
    case 528:
        tLongSQL << "UPDATE clans SET cl_minsympathy = " << atoi(nOptions[0].c_str()) << " WHERE cl_id = " << atoi(nOptions[1].c_str()) << " AND cl_leader = " << atoi(nOptions[2].c_str());
        break;
    case 529:
        tLongSQL << "UPDATE clans SET cl_description = \"" << nOptions[0] << "\" WHERE cl_id = " << atoi(nOptions[1].c_str()) << " AND cl_leader = " << atoi(nOptions[2].c_str());
        break;
    case 543:
        // Only delete if: option 2 is >= 7 (days); option0 is 5 or 6; option 4 = our clanid
        if(atoi(nOptions[2].c_str()) >= 7)
            if(atoi(nOptions[0].c_str()) == 5 || atoi(nOptions[0].c_str()) == 6)
                if(nClient->GetChar()->GetClan() == atoi(nOptions[4].c_str()))
                    snprintf(tShortSQL, 1024, "DELETE FROM clanwars WHERE (cw_status = 5 OR cw_status = 6) AND cw_id = %d AND cw_initclan = %d", atoi(nOptions[3].c_str()), atoi(nOptions[4].c_str()));

        //Console->Print("%s", tShortSQL);
        break;
    case 548:
        // Check clanmembership
        if(nClient->GetChar()->GetClan() == atoi(nOptions[2].c_str()))
            snprintf(tShortSQL, 1024, "UPDATE clanwars SET cw_statement_initiator = \"%s\" WHERE cw_id = %d AND cw_initclan = %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        break;
    case 549:
        // Check clanmembership
        if(nClient->GetChar()->GetClan() == atoi(nOptions[2].c_str()))
            snprintf(tShortSQL, 1024, "UPDATE clanwars SET cw_statement_enemy = \"%s\" WHERE cw_id = %d AND cw_enemyclan = %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        break;
    case 555:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 556:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 561:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 577:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 578:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 579:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 580:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 581:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 585:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 590:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 591:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 600: // Just ok, skip it
        tSuccess = true;
        break;
    case 601: // Just ok, skip it
        tSuccess = true;
        break;
    case 630:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    default:
        Console->Print("%s [Pterminal::HandleUpdateDB]  Unknown QueryID %d", Console->ColorText(RED,BLACK,"Warning"), nDBID);
        return false;
    }

// -------
    if(tLongSQL.str().length() > 0)
    {
        if(MySQL->GameQuery(tLongSQL.str().c_str()))
        {
            Console->Print("Cannot update DB. query was: %s", tLongSQL.str().c_str());
            MySQL->ShowGameSQLError();
            tSuccess = false;
        }
        else
            tSuccess = true;
    }
    else if(strlen(tShortSQL) > 0)
    {
        if(MySQL->GameQuery(tShortSQL))
        {
            Console->Print("Cannot update DB. query was: %s", tShortSQL);
            MySQL->ShowGameSQLError();
            tSuccess = false;
        }
        else
            tSuccess = true;
    }

    // Notice client about UpdateDB result
    char tCmd[100];
    memset(tCmd, '\0', 100);
    strncpy(tCmd, nCommandName->c_str(), 100);

    PMessage* tmpMsg = MsgBuilder->BuildTryAccessAnswerMsg(nClient, tCmd, tSuccess);
    nClient->SendUDPMessage(tmpMsg);
    return true;
}

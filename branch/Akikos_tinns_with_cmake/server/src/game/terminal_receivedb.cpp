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
	terminal_receivedb.cpp - Management class for Terminal actions (Citycom, keys, vehicledepot, ...)
				 Sub-File for terminal "ReceiveDB" command

	MODIFIED: 12 Jan 2007 Namikon
	REASON: - Created
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/terminal.h"
#include "include/msgbuilder.h"

bool PTerminal::HandleReceiveDB(PClient* nClient, u16 mTerminalSessionId, std::string *nCommandName, std::string *nOptions, u8 nNumOptions, u16 nDBID, u8 nUnknown)
{
    EraseVars();
    int nAccessLevel = nClient->GetAccountLevel();
    //Console->Print("DBID: %d", nDBID);

    switch (nDBID)
    {
    case 8:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 6;
        if(!strncmp(nOptions[0].c_str(), "date", 4))
            //snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, c_name, nc_name, nc_approved FROM `neochronicle` INNER JOIN characters ON (nc_author = c_id) ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
            snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, nc_author, nc_name, nc_approved FROM `neochronicle` ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        else
            //snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, c_name, nc_name, nc_approved FROM `neochronicle` INNER JOIN characters ON (nc_author = c_id) ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
            snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, nc_author, nc_name, nc_approved FROM `neochronicle` ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 9:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 6;
        if(!strncmp(nOptions[0].c_str(), "date", 4))
            //snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, c_name, nc_name, nc_approved FROM `neochronicle` INNER JOIN characters ON (nc_author = c_id) WHERE nc_approved = 1 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
            snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, nc_author, nc_name, nc_approved FROM `neochronicle` WHERE nc_approved = 1 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        else
            //snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, c_name, nc_name, nc_approved FROM `neochronicle` INNER JOIN characters ON (nc_author = c_id) WHERE nc_approved = 1 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
            snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, nc_author, nc_name, nc_approved FROM `neochronicle` WHERE nc_approved = 1 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 10:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 6;
        if(!strncmp(nOptions[0].c_str(), "date", 4))
            //snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, c_name, nc_name, nc_approved FROM `neochronicle` INNER JOIN characters ON (nc_author = c_id) WHERE nc_approved = 0 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
            snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, nc_author, nc_name, nc_approved FROM `neochronicle` WHERE nc_approved = 0 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        else
            //snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, c_name, nc_name, nc_approved FROM `neochronicle` INNER JOIN characters ON (nc_author = c_id) WHERE nc_approved = 0 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));
            snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, nc_author, nc_name, nc_approved FROM `neochronicle` WHERE nc_approved = 0 ORDER BY nc_datetime DESC LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 11:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 6;
        //snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, c_name, nc_name, nc_content FROM `neochronicle` INNER JOIN characters ON (nc_author = c_id) WHERE nc_id = %d", atoi(nOptions[0].c_str()));
        snprintf (mSQLQuery, 500, "SELECT nc_id, nc_icon, nc_datetime, nc_author, nc_name, nc_content FROM `neochronicle` WHERE nc_id = %d", atoi(nOptions[0].c_str()));
        //snprintf (mSQLQuery, 500, "SELECT na_id, \'\', na_datetime, na_author, na_name, na_content FROM `neocron articles` WHERE na_approval = 1 AND na_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 14:
        // 0: ID 1: Short 2: Clanname 3: FactionID 4: Faction Symp
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 6; // TODO: replace 0 with average faction sympathy
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_shortdesc, cl_name, cl_faction, \"0\", \"0\" FROM `clans` WHERE cl_name LIKE \"%%%s%%\" LIMIT %d, %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 16:
        if(!ChkOpt(nNumOptions, 3)) break;
        if (!strcmp (nOptions[0].c_str(), "Neocronicle"))
        {
            mResultFields = 5;
            snprintf (mSQLQuery, 500, "SELECT na_id, \'0\', na_datetime, na_author, na_name FROM `neocron articles` WHERE na_approval = 1 LIMIT %s, %s", nOptions[1].c_str(), nOptions[2].c_str());
        }
        else
        {
            mResultFields = 7;
            snprintf (mSQLQuery, 500, "SELECT na_id, \'0\', na_datetime, na_author, na_name, na_approval, \"0\" FROM `neocron articles` LIMIT %s, %s", nOptions[1].c_str(), nOptions[2].c_str());
        }

        break;
    case 18:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT c_name FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 20:
    case 21:
        // Select either c_"name" or c_"minsympathy". Names taken from option, saves if/else
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT `cl_%s` FROM `clans` WHERE `cl_id` = %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()));

        break;
    case 25:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 6; // TODO: replace 0 with average faction sympathy
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_shortdesc, cl_name, cl_faction, \"0\", \"0\" FROM clans WHERE cl_name > \"%s\" AND cl_name <=\"%s\" LIMIT %d, %d", nOptions[0].c_str(), nOptions[1].c_str(), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 26:
        // 0: short 1: name 2: money 3: faction 4: fac symp 5: symp_to_join 6: cappid
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT cl_shortdesc, cl_name, cl_money, cl_faction, \"0\", cl_minsympathy, cl_appid FROM `clans` WHERE `cl_id` = %d", atoi(nOptions[0].c_str()));

        break;
    case 27:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_shortdesc, cl_description, count(DISTINCT o_outnum), count(DISTINCT c_id) FROM outposts INNER JOIN clans ON (o_clan = cl_id) INNER JOIN characters ON (c_clan = clans.cl_id) WHERE (cl_id = %d) GROUP BY cl_id", atoi(nOptions[0].c_str()));

        break;
    case 28:
        // Faction missions? Never seen them even in NC2...
        Console->Print("%s [Pterminal::HandleReceiveDB] QueryID %d has not been written yet; Factionmissions..? Never seen them", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 29:
        Console->Print("%s [Pterminal::HandleReceiveDB] QueryID %d Ordercol was missing. Its: [%s]", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID, nOptions[0].c_str());
        break;
    case 34:
        // Name, Add. Info, Profession, Description, Overal Ranking, Soullight, Money, Runner Kills, Creature Kills
        // Note: Add. Info is autogenerated clientside!
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 8;
        snprintf (mSQLQuery, 500, "SELECT c_name, c_profession, \"0\", \"1\", \"127\", c_cash, \"0\", \"0\" FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 38:
        // Faction missions? Never seen them even in NC2...
        Console->Print("%s [Pterminal::HandleReceiveDB] QueryID %d has not been written yet; Factionmissions..? Never seen them", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 39:
    // 0:id 1:name 2:clan 3:fac.symp
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_clan, \"75\" FROM characters WHERE c_name LIKE \"%s\"", nOptions[0].c_str());

        break;
    case 41:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT o_outnum, o_outnum, \"0\", \"0\", \"0\" FROM outposts WHERE o_clan = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 46:
        // charid, charname, charclanlevel, clanlevelname, charfactionsymp, char online
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, cll_level, cll_desc, \"0\", c_online FROM clanlevels INNER JOIN characters ON (cll_charid = c_id) WHERE (cll_clanid = %d) AND (cll_level >= %d) LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 47:
        if(!ChkOpt(nNumOptions, 5)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, 0, cll_desc, 0, c_online FROM characters INNER JOIN clanlevels ON (c_id = cll_charid) WHERE cll_level >= %d AND cll_level <= %d AND c_clan = %d ORDER BY cll_level DESC LIMIT %d, %d", atoi(nOptions[2].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[0].c_str()), atoi(nOptions[3].c_str()), atoi(nOptions[4].c_str()));

        break;
    case 48:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT cll_desc, cll_level FROM clanlevels WHERE cll_clanid = %d AND cll_level = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 49:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 50:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 2; //Fieldnum
        snprintf (mSQLQuery, 500, "SELECT f_showname, f_name FROM forums WHERE f_area = %d", atoi(nOptions[0].c_str()));

        break;
    case 51:
        if(!ChkOpt(nNumOptions, 9)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT fp_id, fp_name, c_name, fp_datetime FROM `forum_posts`, forums, characters WHERE fp_forumid = f_id AND c_id = fp_fromid AND f_name = \'%s\' AND fp_replyid = 0 LIMIT %d, %d", nOptions[0].c_str(), atoi(nOptions[7].c_str()), atoi(nOptions[8].c_str()));

        break;
    case 52:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
       snprintf (mSQLQuery, 500, "SELECT c_name, fp_name, fp_datetime, fp_content FROM forum_posts INNER JOIN forums ON (fp_forumid = f_id) INNER JOIN characters ON (fp_fromid = c_id) WHERE (f_name = \"%s\") AND (fp_id = %d OR fp_replyid = %d)", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 54:
    // id, name, rank, rankname, f.symp
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, cll_level, cll_desc, 0 FROM clanlevels INNER JOIN characters ON (cll_charid = c_id) WHERE (c_id = %d)", atoi(nOptions[0].c_str()));

        break;
    case 60: // Query for OutPost state. Result is <outpost ID> <clan ID>
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT o_outnum, o_clan FROM outposts LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 61: // It should display the clanname, but it doesnt. seems to be clientside bug
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT o_outnum, o_clan, o_clan, o_security FROM outposts WHERE o_outnum = %d", atoi(nOptions[0].c_str()));

        break;
    case 75:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT cll_desc, cll_level FROM clanlevels WHERE cll_clanid = %d ORDER BY cll_level DESC LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 76:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT count(c_id) FROM clanlevels INNER JOIN characters ON (cll_charid = c_id) WHERE (cll_clanid = %d AND cll_desc = \"%s\")", atoi(nOptions[0].c_str()),nOptions[1].c_str());

        break;
    case 85:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 6; // No idea why this Fac.Symp is that high... 1000000 is 100 ingame. But only for this query
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, cll_level, cll_desc, 1000000, c_online FROM clanlevels INNER JOIN characters ON (cll_charid = c_id) WHERE (c_online = %d OR c_online = 1) AND c_clan = %d LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[0].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 86:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        if(!strncmp(nOptions[1].c_str(), "date", 4))
            snprintf (mSQLQuery, 500, "SELECT mt_id, mt_amount, mt_date, c_name FROM moneytransactions INNER JOIN characters ON (mt_player = c_id) WHERE mt_clanid = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));
        else
            snprintf (mSQLQuery, 500, "SELECT mt_id, mt_amount, mt_date, c_name FROM moneytransactions INNER JOIN characters ON (mt_player = c_id) WHERE mt_clanid = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 87:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_name, mt_date, mt_amount, mt_comment FROM moneytransactions INNER JOIN characters ON (mt_player = c_id) WHERE mt_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 92:
        if(!ChkOpt(nNumOptions, 1)) break;
        // Location of player
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT c_location FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 93:
        if(!ChkOpt(nNumOptions, 1)) break;
        // Name, Add. Info, Profession, Description
        // Note: Add. Info is autogenerated clientside!
        mResultFields = 3;
        snprintf (mSQLQuery, 500, "SELECT c_name, c_profession, \"0\" FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 96:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT f_showname FROM forums WHERE f_name = \"%s\"", nOptions[0].c_str());

        break;
    case 98:
        // ID, Name, Online, 0, Soullight
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_online, \"0\", c_soullight FROM characters WHERE c_online = %d AND c_soullight <= -%d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));
        break;
    case 99:
        // Terrorist info
        // id, name, faction, clan, clanlevel, location
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_faction, c_clan, cll_level, c_location FROM clanlevels INNER JOIN characters ON (cll_charid = c_id) WHERE (c_id = %d AND c_soullight <= -%d)", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 108:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT cl_name FROM clans WHERE cl_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 109:
        if(!ChkOpt(nNumOptions, 1)) break;
        // The only case where KK used an DBId twice for completely different things...
        if(nNumOptions < 3)
        {
            mResultFields = 5;
            snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_clan, cll_level, cll_desc FROM characters INNER JOIN clanlevels ON (cll_charid = c_id AND cll_clanid = c_clan) WHERE c_id = %d", atoi(nOptions[0].c_str()));
        }
        else if(nNumOptions == 3)
        {
            mResultFields = 2;
            snprintf (mSQLQuery, 500, "SELECT c_id, apt_location FROM apartments INNER JOIN characters ON (apt_owner = c_id) WHERE (c_id = %d) LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        }

        break;
    case 116:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT wr_id FROM worlds WHERE wr_group = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 125:
        // Return position in Helpqueue. todo yet... for now its 255, we're always busy! =)
        //if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT 255");

        break;
    case 126:
        // Return position in Helpqueue. todo yet... for now its 255, we're always busy! =)
        // This is with Datecheck, somehow..
        //if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT 255");

        break;
    case 127:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT c_name, apt_location FROM apartments INNER JOIN characters ON (apt_owner = c_id) WHERE (c_id = %d)", atoi(nOptions[0].c_str()));

        break;
    case 128:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT su_id FROM support WHERE su_supporterid = %d AND su_finished = 0", atoi(nOptions[0].c_str()));

    case 131:
        mResultFields = 6;
        //snprintf(mSQLQuery, 500, "SELECT br_id, br_type, c_online, c_name, br_location, br_datetime FROM `bug report`, characters WHERE c_id = br_fromid AND br_status = %d LIMIT %d, %d",
        // atoi(nOptions[0].c_str())+atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));
        snprintf(mSQLQuery, 500, "SELECT su_id, su_type, 0, c_name, su_worldid, su_datetime FROM support INNER JOIN characters ON (su_player = c_id) WHERE su_inwork = %d AND su_finished = %d LIMIT %d, %d",
         atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 132:
    // reqid, runnerid, runnername, 0, date/time, type, 0, in work, finished, desc
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 10;
        //snprintf(mSQLQuery, 500, "SELECT br_id, br_fromid, c_name, \"0\", br_datetime, br_type, \"0\", br_status, br_status, br_desc FROM `bug report` INNER JOIN characters ON (br_fromid = c_id) WHERE br_id = %d", atoi(nOptions[0].c_str()));
        snprintf(mSQLQuery, 500, "SELECT su_id, su_player, c_name, 0, su_datetime, su_type, su_supporterid, su_inwork, su_finished, su_desc FROM support INNER JOIN characters ON (su_player = c_id) WHERE su_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 134:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
        snprintf(mSQLQuery, 500, "SELECT br_id FROM bug_report WHERE br_fromid = %d", atoi(nOptions[0].c_str()));

        break;
    case 136:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT c_name FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 137:
        if(!ChkOpt(nNumOptions, 1)) break;
    // runnerid, runnername, location, online
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_location, c_online FROM characters WHERE c_name = \"%s\"", nOptions[0].c_str());
        break;
    case 138:
        if(!ChkOpt(nNumOptions, 1)) break;
    // runnerid, runnername, location, online
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT br_type, br_location, br_datetime, br_supid, br_status, br_desc FROM bug_report WHERE br_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 143:
        if(!ChkOpt(nNumOptions, 1)) break;
    // runnerid, runnername, location, online
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT a.a_id, a.a_username, b.c_id, 0, b.c_location, b.c_online FROM `%s`.accounts AS a, `%s`.characters AS b WHERE b.c_name = \"%s\" AND a.a_id = b.a_id",
                                  Config->GetOption("info_sql_database").c_str(), Config->GetOption("game_sql_database").c_str(), nOptions[0].c_str());
        break;
    case 144:
        mResultFields = 2; // Replace "1" with Online/Offline!
        snprintf (mSQLQuery, 500, "SELECT c_location, c_online FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 145:
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT a_email FROM `%s`.accounts WHERE a_id = %d", Config->GetOption("info_sql_database").c_str(), atoi(nOptions[0].c_str()));

        break;
    case 146:
        mResultFields = 4;
        // Too bad we named the row different...
        char tSortRow[10];
        if(!strncmp(nOptions[1].c_str(), "world", 5))
            strcpy(tSortRow, "location");
        else
            strcpy(tSortRow, "name");
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_location, c_online FROM characters WHERE c_name LIKE \"%s\" ORDER BY `c_%s` LIMIT %d, %d", nOptions[0].c_str(), tSortRow, atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 147:
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_location, c_online FROM characters WHERE c_online = %d ORDER BY `c_%s` LIMIT %d, %d", atoi(nOptions[0].c_str()), nOptions[1].c_str(), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 148:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, \"0\", \"0\", c_id FROM clans INNER JOIN characters ON (cl_leader = c_id) WHERE (cl_id = %d)", atoi(nOptions[0].c_str()));

        break;
    case 150:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 151:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 153:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 154:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 155:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 156:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 157:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 158:
        // Mission stuff. Will be added later!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 175:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT br_id, br_type, br_title, br_datetime FROM bug_report LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 176:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 8;
        snprintf (mSQLQuery, 500, "SELECT br_type, br_status, br_title, br_desc, 0, br_datetime, br_location, br_fromid FROM bug_report WHERE br_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 181:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 3;
        snprintf (mSQLQuery, 500, "SELECT c_name, apt_location, apt_password FROM apartments INNER JOIN characters ON (c_id = apt_owner) WHERE apt_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 183:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_name, (SELECT count(*) FROM characters WHERE c_clan = %d), c_name, apt_id, apt_location FROM clans INNER JOIN apartments ON (cl_appid = apt_id) INNER JOIN characters ON (c_clan = cl_id) AND (apt_owner = c_id) WHERE (cl_id = %d) GROUP BY c_id", atoi(nOptions[0].c_str()), atoi(nOptions[0].c_str()));

        break;
    case 186:
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT c_id FROM characters WHERE c_name = \"%s\"", nOptions[0].c_str());

        break;
    case 220:
        mResultFields = 4; //Fieldnum
        snprintf (mSQLQuery, 500, "SELECT v_id, v_type, v_status, v_condition FROM vehicles WHERE v_owner = %i LIMIT %i, %i", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 221:
        //VehicleControl order. "VehicleControl", "Type", "Status", "Condition", "Repairprice?"
        mResultFields = 4; //Fieldnum
        snprintf (mSQLQuery, 500, "SELECT v_type, v_status, v_condition, \"0\" FROM vehicles WHERE v_id = %s", nOptions[0].c_str());

        break;
    case 225:
        // Statistics. Maybe later...
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 226:
        // Statistics. Maybe later...
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 230:
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT st_id, st_factid, st_curval, st_curval-st_oldval FROM stockx");

        break;
    case 231:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT st_id, st_factid, st_curval, (st_curval - st_oldval) FROM stockx WHERE (st_id = %d)", atoi(nOptions[0].c_str()));

        break;
    case 232:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT sxd_id, st_factid, st_curval, sxd_amount, (sxd_amount * st_curval) FROM stockx_depots INNER JOIN stockx ON (sxd_st_id = st_id) WHERE (sxd_playerid = %d) LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 233:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 7;
        snprintf (mSQLQuery, 500, "SELECT st_factid, sxd_amount, (st_curval - st_oldval), st_curval, sxd_paid, (sxd_amount * st_curval), ((sxd_amount * st_curval) - sxd_paid) FROM stockx_depots INNER JOIN stockx ON (sxd_st_id = st_id) where sxd_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 234:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 3;
        snprintf (mSQLQuery, 500, "SELECT sxd_id, sxd_amount, sxd_paid FROM stockx_depots INNER JOIN stockx ON (sxd_st_id = st_id) WHERE sxd_playerid = %d AND st_factid = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 250:
        // Influence from CharID? Is stockx gm controlled??
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 251:
        // What is this "influence" ? Check this on live servers!
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 260:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 2; //Fieldnum
        snprintf (mSQLQuery, 500, "SELECT f_showname, f_name FROM forums WHERE f_area = %d+%d+%d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 261:
        // Maybe even rewrite BB system.. its wrong written anyways....
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 262:
        // This is wrong! 262 is the receiveDB request for FACTION threadlist. It should be limitet to that faction then
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_name, fp_name, fp_datetime, fp_content FROM `forum_posts`, forums, characters WHERE fp_forumid = f_id AND c_id = fp_fromid AND (fp_id = %s OR fp_replyid = %s)", nOptions[0].c_str(), nOptions[1].c_str());

       //TODO Check this. Same (?) for ID 52,262,267,401 and 411
        break;
    case 266:
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT fp_id, fp_name, c_name, fp_datetime FROM `forum_posts`, forums, characters WHERE fp_forumid = f_id AND c_id = fp_fromid AND f_name = \'%s\' AND fp_replyid = 0 AND fp_clanid = %d LIMIT %d, %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 267:
        mResultFields = 4;
       snprintf (mSQLQuery, 500, "SELECT c_name, fp_name, fp_datetime, fp_content FROM `forum_posts`, forums, characters WHERE fp_forumid = f_id AND c_id = fp_fromid AND (fp_id = %s OR fp_replyid = %s)", nOptions[0].c_str(), nOptions[1].c_str());

        break;
    case 270:
        Console->Print("%s [Pterminal::HandleReceiveDB] QueryID %d  Tablename/Name is: [%s] ThreadID is: [%d]", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID, atoi(nOptions[0].c_str()), nOptions[1].c_str());
        break;
    case 360:
        // Gamemaster name (char), Accountname, Location, Online, CharID
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT a.a_id, c.c_id, a.a_username, c.c_name, c.c_online, c.c_location FROM %s.characters AS c, %s.accounts AS a WHERE a.a_priv >= 50 AND c.c_online = %d AND c.a_id = a.a_id ORDER BY c.c_name LIMIT %d, %d",
                                    Config->GetOption("game_sql_database").c_str(), Config->GetOption("info_sql_database").c_str(), atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 365:
        // ID, short, name faction
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_shortdesc, cl_name, cl_faction FROM clans LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 366:
        // ID, short, name faction WHERE clanname = %s
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_shortdesc, cl_name, cl_faction FROM clans WHERE cl_name LIKE \"%s\" LIMIT %d, %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 367:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 8;
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_shortdesc, cl_name, cl_leader, cl_faction, cl_appid, cl_money, (SELECT count(*) FROM characters WHERE c_clan = %d) FROM clans WHERE cl_id = %d", atoi(nOptions[0].c_str()), atoi(nOptions[0].c_str()));

        break;
    case 368:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT cll_desc, cll_level FROM clanlevels WHERE cll_clanid = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 373:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT apt_id, apt_location, apt_type, apt_password FROM apartments WHERE apt_id = %d AND apt_owner = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 375:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_location, c_online FROM characters LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 376:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_location, c_online FROM characters WHERE c_name LIKE \"%s\" LIMIT %d, %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 377:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT DISTINCT a_id FROM characters WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 378:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT a_username, a_email, \"Since begin\", \"Never\", FROM_UNIXTIME(a_bandate) FROM `%s`.accounts WHERE a_id = %d", Config->GetOption("info_sql_database").c_str(), atoi(nOptions[0].c_str()));

        break;
    case 379:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, c_location, c_online, c_clan, cll_level FROM characters INNER JOIN clanlevels ON (cll_charid = c_id) WHERE c_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 380: // clean
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT cl_name, cll_desc FROM clans INNER JOIN clanlevels ON (cll_clanid = cl_id) WHERE cl_id = %d AND cll_level = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 381:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, 0, c_online FROM characters WHERE a_id = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 384: // clean
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT apt_id, apt_location, 0, apt_password FROM apartments WHERE apt_owner = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 400:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 4;
       snprintf (mSQLQuery, 500, "SELECT fp_id, fp_name, c_name, fp_datetime FROM forum_posts INNER JOIN forums ON (fp_forumid = f_id) INNER JOIN characters ON (fp_fromid = c_id) WHERE (f_name = \"%s\") LIMIT %d, %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 401:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
       snprintf (mSQLQuery, 500, "SELECT c_name, fp_name, fp_datetime, fp_content FROM forum_posts INNER JOIN forums ON (fp_forumid = f_id) INNER JOIN characters ON (fp_fromid = c_id) WHERE (f_name = \"%s\") AND (fp_id = %d OR fp_replyid = %d)", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 406:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1;
       snprintf (mSQLQuery, 500, "SELECT count(fp_id) FROM forum_posts INNER JOIN forums ON (fp_forumid = f_id) WHERE f_name = \"%s\"", nOptions[0].c_str());

        break;
    case 410:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 4;
       snprintf (mSQLQuery, 500, "SELECT fp_id, fp_name, c_name, fp_datetime FROM forum_posts INNER JOIN forums ON (fp_forumid = f_area) INNER JOIN characters ON (fp_fromid = c_id) WHERE (f_name = \"%s\" AND fp_factionid = %d) LIMIT %d, %d", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 411:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 4;
       snprintf (mSQLQuery, 500, "SELECT c_name, fp_name, fp_datetime, fp_content FROM forum_posts INNER JOIN forums ON (fp_forumid = f_area) INNER JOIN characters ON (fp_fromid = c_id) WHERE (f_name = \"%s\") AND (fp_id = %d OR fp_replyid = %d) AND (fp_factionid = %d)", nOptions[0].c_str(), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;

    case 420:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT e_id, e_subject, c_name, e_new, e_replied, e_datetime FROM emails, characters WHERE e_fromid = c_id AND e_toid = %i LIMIT %i, %i", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 421:
        if(!ChkOpt(nNumOptions, 1)) break;
        //MailControl order. "MailControl", "Mail ID", "Subject", "From", boolean Replied, "Date/Time", "FromID", "Content"
        mResultFields = 7;
        snprintf (mSQLQuery, 500, "SELECT e_id, e_subject, c_name, e_replied, e_datetime, e_fromid, e_body FROM emails, characters WHERE e_fromid = c_id AND e_id = %s", nOptions[0].c_str());

        break;
    case 425:
        if(!ChkOpt(nNumOptions, 1)) break;
        //Send mail order. "SendMail", "id"
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT c_id FROM characters WHERE c_name = \'%s\'", nOptions[0].c_str());

        break;
    case 430:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT a.c_id, a.c_conid, b.c_name, a.c_desc FROM contacts as a, characters as b WHERE a.c_conid = b.c_id AND a.c_listid = %s AND c_type = %s", nOptions[0].c_str(), nOptions[1].c_str());

        break;
    case 431:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT a.c_id, a.c_conid, b.c_name, a.c_desc FROM contacts as a, characters as b WHERE a.c_conid = b.c_id AND a.c_id = %s", nOptions[0].c_str());

        break;
    case 435:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM contacts WHERE c_listid = %s AND c_type = %s", nOptions[0].c_str(), nOptions[1].c_str());

        break;
    case 436:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name FROM characters WHERE c_name LIKE \'%%%s%%\' LIMIT %s, %s", nOptions[0].c_str(), nOptions[1].c_str(), nOptions[2].c_str());

        break;
    case 437:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 1; //Fieldnum
        snprintf (mSQLQuery, 500, "SELECT c_name FROM characters WHERE c_id = %s", nOptions[0].c_str());

        break;
    case 445:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT nc_id, nc_name FROM neochronicle WHERE nc_lang = %d ORDER BY nc_id DESC LIMIT %d, %d", atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 446:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT nc_id, nc_lang, nc_datetime, nc_name, nc_content FROM neochronicle WHERE nc_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 453:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT g_id, 0, g_part, g_title, g_language FROM guides WHERE g_chapter = %d AND g_language = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 455:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 3;
        // Opt 2 is Language
        snprintf (mSQLQuery, 500, "SELECT g_id, g_part, g_title FROM guides WHERE g_chapter = %d AND g_language = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 456:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT g_id, g_language, g_title, g_content FROM guides WHERE g_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 458: // Statisics
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 459: // Statisics
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 510: // ClanWars: Overview
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT cw_id, cl_name, cw_starttime, cw_status FROM clanwars INNER JOIN clans ON (cw_initclan = cl_id) or (cw_enemyclan = cl_id) WHERE (cw_enemyclan = %d OR cw_initclan = %d) AND (cl_id != %d) LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[0].c_str()), atoi(nOptions[0].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));
        break;
    case 511: // ClanWars: Selfdeclared wars
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT cw_id, cl_name, cw_starttime, cw_status FROM clanwars INNER JOIN clans ON (cw_enemyclan = cl_id) WHERE cw_initclan = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        break;
    case 512: // ClanWars: Foreigndeclared wars
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 4;
        snprintf (mSQLQuery, 500, "SELECT cw_id, cl_name, cw_starttime, cw_status FROM clanwars INNER JOIN clans ON (cw_initclan = cl_id) WHERE cw_enemyclan = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));
        break;
    case 513:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_name FROM clans WHERE cl_faction = %d AND cl_id != %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));
        break;
    case 514:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT cl_id, cl_name FROM clans WHERE cl_id = %d", atoi(nOptions[0].c_str()));
        break;
    case 520:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 10;
        snprintf (mSQLQuery, 500, "SELECT cl_faction, cl_shortdesc, cl_name, cl_description, 0, cl_appid, 80, cl_money, (SELECT count(*) FROM outposts WHERE o_clan = %d), (SELECT count(*) FROM characters WHERE c_clan = %d) FROM clans WHERE cl_id = %d", atoi(nOptions[0].c_str()), atoi(nOptions[0].c_str()), atoi(nOptions[0].c_str()));

        break;
    case 521:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, 0, cll_desc, 0, c_online FROM characters INNER JOIN clanlevels ON (c_id = cll_charid) WHERE c_clan = %d AND cll_level >= %d AND cll_level < 15 LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 522:
        if(!ChkOpt(nNumOptions, 5)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT c_id, c_name, 0, cll_desc, 0, c_online FROM characters INNER JOIN clanlevels ON (c_id = cll_charid) WHERE cll_level >= %d AND cll_level <= %d AND c_clan = %d ORDER BY cll_level DESC LIMIT %d, %d", atoi(nOptions[2].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[0].c_str()), atoi(nOptions[3].c_str()), atoi(nOptions[4].c_str()));

        break;
    case 523:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT cl_shortdesc, cl_name, cl_leader, cl_description, cl_minsympathy FROM clans WHERE cl_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 540:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT apt_id, apt_location FROM apartments WHERE apt_owner = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 541:
        if(!ChkOpt(nNumOptions, 3)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT v_id, v_type FROM vehicles WHERE v_owner = %d LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 542:
        if(!ChkOpt(nNumOptions, 2)) break;
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT v_id, v_type, v_world, v_status, v_condition FROM vehicles WHERE v_owner = %d and v_id = %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()));

        break;
    case 544:
        if(!ChkOpt(nNumOptions, 4)) break;
        mResultFields = 1;
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM clanwars WHERE (cw_initclan = %d AND cw_enemyclan = %d) OR (cw_enemyclan = %d AND cw_initclan = %d)", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()), atoi(nOptions[3].c_str()));

        break;
    case 546:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 2;
        snprintf (mSQLQuery, 500, "SELECT cw_starttime, (TO_DAYS(NOW()) - TO_DAYS(cw_starttime)) FROM clanwars WHERE cw_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 547:
        if(!ChkOpt(nNumOptions, 1)) break;
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT cw_initclan, cw_enemyclan, cw_starttime, cw_status, cw_statement_initiator, cw_statement_enemy FROM clanwars WHERE cw_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 557:
        Console->Print("%s [Pterminal::HandleReceiveDB] QueryID %d means that you want to edit an DialogScript from the server. This is not supportet yet!", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 558:
        Console->Print("%s [Pterminal::HandleReceiveDB] QueryID %d means that you want to edit an DialogScript from the server. This is not supportet yet!", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 562:
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT cb_inscription FROM datacubes WHERE cb_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 569:
        // Faction representatives. Not stored yet... is it?
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 570:
        // Clan representatives. Stored but this query wants votes too... Maybe later
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 571:
        // Clan representatives. Stored but this query wants votes too... Maybe later
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 572:
        // Runner voting. Not stored yet
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 573:
        // Next election date
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 574:
        mResultFields = 6;
        snprintf (mSQLQuery, 500, "SELECT count(c_id) FROM characters WHERE c_faction = %d", atoi(nOptions[0].c_str()));

        break;
    case 575:
        // Runner satisfaction with faction
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 576:
        // Check if vote from runner is already stored
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 582:
        // Clan representative details. Faction rank etc... not stored yet
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 583:
        // Clan representative details. Faction rank etc... not stored yet
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 584:
        // Clan representative details. Faction rank etc... not stored yet
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 587:
        // Clan representative stuff again...
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 603:
        mResultFields = 1;
        //snprintf (mSQLQuery, 500, "SELECT count(*) FROM `bug report` WHERE br_status = 0");
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM support WHERE su_inwork = 0 AND su_finished = 0");

        break;
    case 604:
        mResultFields = 1;
        //snprintf (mSQLQuery, 500, "SELECT count(*) FROM `bug report` WHERE br_status = 1");
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM support WHERE su_inwork = 1 AND su_finished = 0");

        break;
    case 605:
        mResultFields = 1;
        //snprintf (mSQLQuery, 500, "SELECT count(*) FROM `bug report` WHERE br_status = 2");
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM support WHERE su_finished = 1");

        break;
    case 606:
        mResultFields = 1;
        //snprintf (mSQLQuery, 500, "SELECT count(*) FROM `bug report`");
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM support");

        break;
    case 607: // Dont know how to select data from sql where no data to JOIN is there anymore :/
    // This should query ID, RunnerID and Date from bug reports where runnerID is unknown
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 608:
        mResultFields = 5;
        snprintf (mSQLQuery, 500, "SELECT br_id, br_fromid, br_location, br_status, br_datetime FROM bug_report WHERE br_id = %d", atoi(nOptions[0].c_str()));

        break;
    case 609:
        mResultFields = 1;
        //snprintf (mSQLQuery, 500, "SELECT count(*) FROM `bug report` WHERE br_supid = %s", nOptions[0].c_str());
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM support WHERE su_supporterid = %d", atoi(nOptions[0].c_str()));

        break;
    case 610:
        mResultFields = 6;
        //snprintf (mSQLQuery, 500, "SELECT br_id, \"0\", c_name, br_location, br_status = 2, br_datetime FROM `bug report`, characters WHERE c_id = br_fromid AND br_supid = %s AND br_status > 0 LIMIT %s, %s", nOptions[0].c_str(), nOptions[1].c_str(), nOptions[2].c_str());
        snprintf (mSQLQuery, 500, "SELECT su_id, su_supporterid, c_name, su_worldid, su_finished, su_datetime FROM support INNER JOIN characters ON (su_supporterid = c_id) WHERE su_supporterid = %d AND su_finished = 1 LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[1].c_str()), atoi(nOptions[2].c_str()));

        break;
    case 611:// Well, its meant to show date specific stuff. I will rewrite this soon...
        snprintf (mSQLQuery, 500, "SELECT count(*) FROM support WHERE su_supporterid = %d", atoi(nOptions[0].c_str()));
        break;
    case 612:// Well, its meant to show date specific stuff. I will rewrite this soon...
        snprintf (mSQLQuery, 500, "SELECT su_id, su_supporterid, c_name, su_worldid, su_finished, su_datetime FROM support INNER JOIN characters ON (su_supporterid = c_id) WHERE su_supporterid = %d AND su_finished = 1 LIMIT %d, %d", atoi(nOptions[0].c_str()), atoi(nOptions[3].c_str()), atoi(nOptions[4].c_str()));
        break;
    case 625: // Faction council stuff
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 626: // Faction council stuff
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 627: // Faction council stuff
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 628: // Faction council stuff
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    case 629: // Faction council stuff
        Console->Print("%s [Pterminal::HandleReceiveDB]  Nothing to do; Function not written yet for QueryID %d", Console->ColorText(YELLOW,BLACK,"Notice"), nDBID);
        break;
    default:
        Console->Print("%s [Pterminal::HandleReceiveDB]  Unknown QueryID %d", Console->ColorText(RED,BLACK,"Warning"), nDBID);
        return false;
    }

    if(strlen(mSQLQuery) == 0)
        return false;
// ---------------
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    int tNumRows = 0;

    result = MySQL->GameResQuery(mSQLQuery);
    if (!result)
    {
        Console->Print("Cannot query DB. DBID: %d query was: %s", nDBID, mSQLQuery);
        MySQL->ShowGameSQLError();
        return false;
    }

    PMessage* tDBResult = new PMessage();
    tNumRows = mysql_num_rows(result);

    // Loop all result rows
    for (int i = 0; i < tNumRows ; i++)
    {
        row = mysql_fetch_row( result ) ;
        // Loop all result fields and push answers into Message
        for (int t = 0; t < mResultFields; t++)
        {
            *tDBResult << ( u16 )(strlen(row[t]) + 1);
            *tDBResult << row[t];
        }
    }

    // Build result messages
    char tCmd[100];
    memset(tCmd, '\0', 100);
    strncpy(tCmd, nCommandName->c_str(), 100);

    PMessage* tmpMsg_allowed = MsgBuilder->BuildTryAccessAnswerMsg( nClient, tCmd, true );
    PMessage* tmpMsg_result = MsgBuilder->BuildReceiveDBAnswerMsg( nClient, tDBResult, nCommandName, tNumRows, mResultFields);

    nClient->SendUDPMessage(tmpMsg_allowed);
    nClient->FragmentAndSendUDPMessage(tmpMsg_result, 0x68);

    return true;
}

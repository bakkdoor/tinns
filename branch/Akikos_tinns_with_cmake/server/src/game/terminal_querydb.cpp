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
   terminal_querydb.cpp - Management class for Terminal actions (Citycom, keys, vehicledepot, ...)
   > Sub-File for terminal "ReceiveDB" command

   MODIFIED: 18 Oct 2009 Namikon
   REASON: - Created

*/

#include "main.h"
#include "terminal.h"
#include "msgbuilder.h"


bool PTerminal::HandleQueryDB(PClient* nClient, std::string *nDBCommandName, std::string *nCommandName, std::string *nOptions, u8 nNumOptions)
{
    EraseVars();

    bool tOk = false;

    if(!strcmp(nDBCommandName->c_str(), "ACM"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CHANGELEADER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CHANGERANK"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CHECKRUNNER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "CSM"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "DELETECLAN"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "DISMISSMEMBER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "DISMISSVEHICLE"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "GIVEMONEY"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "JOINCLAN"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "KICKPLAYER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "QUITCLAN"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "REPAIRVEHICLE"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "SPAWNVEHICLE"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "TAKEFACTIONMONEY"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "TAKEMONEY"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "TRADESTOCKX"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "WARPPLAYER"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else if(!strcmp(nDBCommandName->c_str(), "WARPPLAYERTOWORLD"))
    {
        Console->Print("%s [PTerminal::HandleQueryDB] Nothing to do; Function not written yet: %s", Console->ColorText(YELLOW,BLACK,"Notice"), nDBCommandName->c_str());
    }
    else
    {
        // Ok this REALLY should'nt happen.. But who knows..?
        Console->Print("%s [PTerminal::HandleQueryDB] Unknown ServerMessage: %s", Console->ColorText(RED,BLACK,"Error"), nDBCommandName->c_str());
        return false;
    }

    return tOk;
}

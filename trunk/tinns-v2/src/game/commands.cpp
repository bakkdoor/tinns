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
	commands.cpp - handles ingame commands (@<command>)

	Authors:
	- Namikon
    - bakkdoor
    - Hammag


    File rewritten on 20th Dec 2006 by Namikon
    Note: Later, when we add multithread support to tinns, the FlushArgs() functions
          have to be placed ONLY in con/destructor and nowhere else.
          Besides that, no global GameCommands, we should do that with tmpCmd = new PCommands;
*/
#include "main.h"

#include "client.h"
#include "msgbuilder.h"
#include "worlds.h"

PCommands::PCommands()
{
    FlushArgs();
}

PCommands::~PCommands()
{
    FlushArgs();
}

void PCommands::HandleGameCommand(char *packet, PClient *Client)
{
    // Start over with an empty class
    FlushArgs();

    // Grab client class
    source = Client;

    int posPacket = 1;
    int tmpPos = 0;

    //First of all, split the command itself. We only know here that it
    // *seems* to be an command, we have no idea if the command is valid
    do {
        // Copy command from packet to final var
        Command[tmpPos] = packet[posPacket];

        // Increment both pointers
        posPacket++;
        tmpPos++;
    } while (packet[posPacket] != ' ' && packet[posPacket] != '\0');

    // Terminate command
    Command[tmpPos] = '\0';


    // To save time, only copy org packet into dumb when we really need it.
    // On the other side, dont try to get the Args if a dumb is made.
    if(RequirePacketDumb() == true)
    {
        int loopy = 0;
        while (packet[loopy] != '\0' && loopy < (MAXDUMBSIZE - 1))
        {
            OrgPacketDumb[loopy] = packet[loopy];
            loopy++;
        }
        OrgPacketDumb[loopy] = '\0';
        DumbMade = true;
        ArgC = 0; // Just to be sure...
    }
    else
    {
        // Search next arg (Maybe someone typed more than one whitespace)
        while(packet[posPacket] == ' ' && packet[posPacket] != '\0')
        {
            posPacket++;
        }

        // ok, we have the command, now read the args
        // Loop till entire chatpacket is parsed or MAXARGS is reached
        while(packet[posPacket] != '\0' && ArgC <= MAXARGS)
        {
            // First set tmpPos for next Arg to 0
            tmpPos = 0;

            // Now loop until next space ' ' or end '\0' is reached
            while(packet[posPacket] != ' ' && packet[posPacket] != '\0')
            {
                // Copy arg from chatpacket to final arg var
                ArgV[ArgC][tmpPos] = packet[posPacket];

                // Increment tmpPos and posPacket
                posPacket++;
                tmpPos++;
            }
            // Reached either the end of packet or an whitespace
            // Terminate current ArgV
            ArgV[ArgC][tmpPos] = '\0';

            // Heh, we got one! Now more to next arg
            ArgC++;

            // Search next arg (Maybe someone typed more than one whitespace)
            while(packet[posPacket] == ' ' && packet[posPacket] != '\0')
            {
                posPacket++;
            }
        }
        if(packet[posPacket] != '\0' && ArgC == MAXARGS && gDevDebug)
            Console->Print("%s MAXARGS reached, cant parse more args for command!", Console->ColorText(YELLOW, BLACK, "[Warning]"));
    }
    // Check if client is allowed to perform command
    if(CheckPermission() == false && IsDevCommand() == false)
        return;

    if(strcmp(Command, "debug") == 0)
    {
        doCmddebug();
    }
    else if(strcmp(Command, "settime") == 0)
    {
        doCmdsettime();
    }
    else if(strcmp(Command, "warp") == 0)
    {
        doCmdwarp();
    }
    else if(strcmp(Command, "rawf") == 0)
    {
        doCmdrawf();
    }
    else if(strcmp(Command, "delworlditem") == 0)
    {
        doCmddelworlditem();
    }
    else if(strcmp(Command, "addworlditem") == 0)
    {
        doCmdaddworlditem();
    }
    else if(strcmp(Command, "adddoor") == 0)
    {
        doCmdadddor();
    }
    else if(strcmp(Command, "online") == 0) // Was: connectedList
    {
        doCmdconlist();
    }
    else if(strcmp(Command, "skin") == 0)
    {
        doCmdskin();
    }
    else if(strcmp(Command, "effect") == 0)
    {
        doCmdeffect();
    }
    else if(strcmp(Command, "speed") == 0)
    {
        doCmdspeed();
    }
    else if(strcmp(Command, "color") == 0)
    {
        doCmdcolor();
    }
    else if(strcmp(Command, "brightness") == 0)
    {
        doCmdbrightness();
    }
    else if(strcmp(Command, "remove") == 0)
    {
        doCmdremove();
    }
    else if(strcmp(Command, "rehash") == 0)
    {
        doCmdrehash();
    }
    else if(strcmp(Command, "uptime") == 0)
    {
        doCmduptime();
    }
    else if(strcmp(Command, "version") == 0)
    {
        doCmdversion();
    }
    else if(strcmp(Command, "kick") == 0)
    {
        doCmdkick();
    }
    else if(strcmp(Command, "info") == 0)
    {
        doCmdinfo();
    }
    else if(strcmp(Command, "setlevel") == 0)
    {
        doCmdsetlevel();
    }
    else if(strcmp(Command, "warpto") == 0)
    {
        doCmdwarpto();
    }
    else if(strcmp(Command, "recall") == 0)
    {
        doCmdrecall();
    }
    else if(strcmp(Command, "broadcast") == 0)
    {
        doCmdbroadcast();
    }
    else if(strcmp(Command, "t") == 0)
    {
        doCmd_dev_t();
    }
    else if(strcmp(Command, "h") == 0)
    {
        doCmd_dev_h();
    }
    else if(strcmp(Command, "v") == 0)
    {
        doCmd_dev_v();
    }
    else if(strcmp(Command, "ban") == 0)
    {
        doCmdban();
    }
    else if(strcmp(Command, "unban") == 0)
    {
        doCmdunban();
    }
    else if(strcmp(Command, "listbans") == 0)
    {
        doCmdlistbans();
    }
    else if(strcmp(Command, "shun") == 0)
    {
        doCmdshun();
    }
    else if(strcmp(Command, "unshun") == 0)
    {
        doCmdunshun();
    }
    else if(strcmp(Command, "jail") == 0)
    {
        doCmdjail();
    }
    else if(strcmp(Command, "unjail") == 0)
    {
        doCmdunjail();
    }
    else if(strcmp(Command, "teleport") == 0)
    {
        doCmdteleport();
    }

    // Else: unknown command. Ignore
}

bool PCommands::CheckPermission()
{
    if(CmdAccess->GetOptionInt(Command) > source->GetAccount()->GetLevel())
        return false;
    else
        return true;
}

bool PCommands::IsAdmin()
{
    if(source->GetAccount()->GetLevel() >= PAL_ADMIN)
        return true;
    else
        return false;
}


bool PCommands::GetTarget(int ArgNum)
{
    if(ArgNum > ArgC)
        return false;

    if(IsArgNumeric(ArgNum) == true)
        target = GetClientByID(GetArgInt(ArgNum));
    else
    {
        char tmp[50];
        GetArgText(ArgNum, tmp, 50);
        target = GetClientByNick(tmp);
    }

    if(target == NULL)
        return false;
    else
        return true;
}

bool PCommands::IsArgNumeric(int ArgNum)
{
    if(ArgNum > ArgC)
        return false;

    int tmpPos = 0;
    while(ArgV[ArgNum - 1][tmpPos] != '\0')
    {
        if(!isdigit(ArgV[ArgNum - 1][tmpPos]))
            return false;

        tmpPos++;
    }
    return true;
}

void PCommands::FlushArgs()
{
    // Empty all variables
    for(int i = 0; i < MAXARGS; i++)
        ArgV[i][0] = '\0';

    Command[0] = '\0';
    OrgPacketDumb[0] = '\0';

    ArgC = 0;
    DumbMade = false;
    source = NULL;
    target = NULL;

}

int PCommands::GetArgInt(int ArgNum)
{
    if(ArgNum > ArgC)
        return 0;

    int tmpIntRet = 0;
    tmpIntRet = std::atoi(ArgV[ArgNum - 1]);
    return tmpIntRet;
}

bool PCommands::GetArgText(int ArgNum, char* output, int output_size)
{
    if(ArgNum > ArgC)
        return false;

    int i = 0;

    // Copy arg to given char array
    while(ArgV[ArgNum - 1][i] != '\0')
    {
        output[i] = ArgV[ArgNum - 1][i];
        i++;
        if((i + 1) == output_size)
        {
            output[i] = '\0';
            return true;
        }
    }
    output[i] = '\0';
    return true;
}

/// ------- Begin commandset -------
void PCommands::doCmddebug()
{
    PDebugMode nWhat = DBG_ALL;
    int nHow = -1;
    char* DbgTarget = "all";
    char DbgMessage[80];

    if(ArgC < 1)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@debug [loc[ation] | it[emid]] [0|1]");
        return;
    }
    Console->Print("D1");
    if(IsArgNumeric(1) == true)
    {
        Console->Print("D2");
        if(GetArgInt(1) == 0)
        {
            Console->Print("D3");
            nHow = 0;
        }
        else
        {
            Console->Print("D4");
            nHow = 1;
        }
        Console->Print("D5");
    }
    else
    {
        Console->Print("D6");
        char tmp[10];
        Console->Print("D7");
        if(GetArgText(1, tmp, 10))
        {
            Console->Print("D8");
            if(strncmp(tmp, "loc", 3) == 0)
            {
                Console->Print("D9");
                nWhat = DBG_LOCATION;
                DbgTarget = "location";
                Console->Print("D10");
            }
            else if(strncmp(tmp, "it", 2) == 0)
            {
                Console->Print("D11");
                nWhat = DBG_ITEMID;
                DbgTarget = "itemid";
                Console->Print("D12");
            }
        }
    }

    if (nWhat != DBG_ALL)
    {
        if(ArgC == 1)
        {
            nHow = (source->GetDebugMode(nWhat) ? 0 : 1); // toggle if no arg
        }
        else if(ArgC > 1 && GetArgInt(2) == 0)
        {
            nHow = 0;
        }
        else if(ArgC > 1 && GetArgInt(2) == 1)
        {
            nHow = 1;
        }
    }

    if (nHow != -1)
    {
        source->SetDebugMode(nWhat, nHow);
        snprintf(DbgMessage, 80, "Debug %s is now %s", DbgTarget, (nHow ? "ENABLED" : "DISABLED"));
        Chat->send(source, CHAT_DIRECT, "System", DbgMessage);
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@debug [loc[ation] | it[emid]] [0|1]");
    }
}

void PCommands::doCmdsettime()
{
    if(ArgC < 1 && GetArgInt(1) == 0)
    {
       Chat->send(source, CHAT_DIRECT, "Usage", "@settime <raw timevalue>");
       return;
    }

    int newtime = 0;
    newtime = GetArgInt(1);

    GameServer->SetGameTime(newtime);
    Console->Print("IngameCommand: CharID %d set ingametime to %d", source->GetCharID(), newtime);
}

void PCommands::doCmdwarp()
{
    int SpawnPointID = 0;
    int zoneID = 0;

    bool SyntaxError = false;
    if(ArgC < 1)
    {
        Console->Print("D1");
        SyntaxError = true;
    }
    else
    {
        if(IsArgNumeric(1) == true)
        {
            zoneID = GetArgInt(1);
            if(zoneID == 0)
            {
                Console->Print("D2");
                SyntaxError = true;
            }
        }
        else
        {
            Console->Print("D3");
            SyntaxError = true;
        }

        if(ArgC > 1)
        {
            if(IsArgNumeric(2) == true)
            {
                SpawnPointID = GetArgInt(2);
            }
            else
            {
                Console->Print("D4");
                SyntaxError = true;
            }
        }
    }
    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@warp <zoneid> [<spawn location>]");
        return;
    }

    if (!Worlds->IsValidWorld(zoneID))
    {
        Chat->send(source, CHAT_DIRECT, "System", "Invalid zoneID");
        return;
    }

    if (source->ChangeCharLocation(zoneID))
    {
        if (gDevDebug) Console->Print("IngameCommand: Warping player %d to zone %d (%s)", source->GetCharID(), zoneID, Worlds->GetWorld(zoneID)->GetName().c_str());

        PMessage* tmpMsg = MsgBuilder->BuildAptLiftUseMsg (source, zoneID, SpawnPointID);
        source->getUDPConn()->SendMessage(tmpMsg);
    }
    else
    {
        Console->Print(RED, BLACK, "IngameCommand: Can't change location when trying to warp player %d to zone %d", source->GetCharID(), zoneID);
    }
}

void PCommands::doCmdrawf()
{
    bool SyntaxError = false;
    bool DebugMode = false;
    int Prot = 0;  // 0: Error 1: UDP 2: TCP

    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    char file_to_send[50], protocol_to_use[10];
    GetArgText(1, file_to_send, 50);
    GetArgText(2, protocol_to_use, 10);

    if(strcmp(protocol_to_use, "udp") == 0)
    {
        Prot = 1;
    }
    else if(strcmp(protocol_to_use, "tcp") == 0)
    {
        Prot = 2;
    }
    else if(strcmp(protocol_to_use, "debug") == 0)
    {
        DebugMode = true;
    }
    else
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@rawf <file> <udp/tcp/debug>");
        return;
    }

    ifstream::pos_type size;
    char *buffer;

    ifstream hexdump (file_to_send, ios::in|ios::binary|ios::ate);
    if (hexdump.is_open())
    {
        if (gDevDebug) Console->Print("IngameCommand: Sending packet file %s", file_to_send);

        size = hexdump.tellg();
        buffer = new char [size];
        hexdump.seekg (0, ios::beg);

        hexdump.read (buffer, size);
        hexdump.close();
        if(DebugMode == true)
        {
            int k;
            Console->Print("Byte dump of %s:", file_to_send);
            for(k=0;k<size;k++)
            {
                Console->Print("Byte %d: %#x", k, buffer[k]);
            }
        }
        else
        {
            if(Prot == 1)
                source->getUDPConn()->write(buffer, size);
            else if(Prot == 2)
                source->getTCPConn()->write(buffer, size);
            else // Should never happen...
                Console->Print("%s unable to determine protocol in PCommands::doCmdrawf", Console->ColorText(RED, BLACK, "[Error]"));
        }

        delete[] buffer;
    }
    else
    {
        char output[100];
        Console->Print("IngameCommand: Unable to send file %s", file_to_send);
        sprintf(output, "File not found: %s", file_to_send);
        Chat->send(source, CHAT_DIRECT, "System", output);
    }
}

void PCommands::doCmddelworlditem()
{
    bool SyntaxError = false;

    PChar *Char = Database->GetChar(source->GetCharID());
    char query[255], answer[255];
    int worlditemID = 0;
    int Location = Char->GetLocation();

    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == true)
    {
        worlditemID = GetArgInt(1);
        if(worlditemID == 0)
        {
            SyntaxError = true;
        }
    }
    else
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@delworlditem <ItemID>");
        return;
    }

    int checkval = MySQL->GetWorldItemType((unsigned short)worlditemID, Location);
    if(checkval < 1 || checkval > 5)
    {
        sprintf(answer, "Cannot delete WorlditemID %d, not in database!", worlditemID);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        return;
    }

    sprintf(query, "DELETE FROM world_items WHERE wi_worlditem_id = %d AND wi_worlditem_map = %d", worlditemID, Location);
    if(MySQL->GameQuery(query))
    {
        sprintf(answer, "Unable to delete worlditemID %d, see console for MySQL error", worlditemID);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        MySQL->ShowGameSQLError();
        return;
    }
    else
    {
        sprintf(answer, "Successfully deleted worlditemID %d, location %d from Database", worlditemID, Location);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        return;
    }
}

void PCommands::doCmdaddworlditem()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    PChar *Char = Database->GetChar(source->GetCharID());
    char query[255], answer[255];
    int worlditemID = 0, worlditemType = 0, worlditemOp1 = 0, worlditemOp2 = 0;
    int Location = Char->GetLocation();

    if(IsArgNumeric(1) == false || IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }
    else
    {
        if(GetArgInt(1) == 0 || GetArgInt(2) == 0)
        {
            SyntaxError = true;
        }
        else
        {
            worlditemID = GetArgInt(1);
            worlditemType = GetArgInt(2);
        }
    }
    if(ArgC > 2)
    {
        worlditemOp1 = GetArgInt(3);
        if(ArgC > 3)
        {
            worlditemOp2 = GetArgInt(4);
        }
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@addworlditem <ItemID> <ItemType> [<Option1> <Option2>]");
        return;
    }

    if(worlditemType < 1 || worlditemType > 19)
    {
        Chat->send(source, CHAT_DIRECT, "System", "Invalid Worlditemtype (1-19)");
        return;
    }

    if (Location > 100000)
    {
        Chat->send(source, CHAT_DIRECT, "System", "You cant add worlditems in appartments at the moment");
        return;
    }
    else
    {
        int checkval = MySQL->GetWorldItemType((unsigned short)worlditemID, Location);
        if(checkval > 0 && checkval < 5)
        {
            sprintf(answer, "WorlditemID %d is already in the Database! Use @delworlditem <id> to remove it first", worlditemID);
            Chat->send(source, CHAT_DIRECT, "System", answer);
            return;
        }
        sprintf (query, "INSERT INTO world_items (wi_worlditem_id, wi_worlditem_map, wi_type, wi_option1, wi_option2) VALUES (%d, %d, %d, %d, %d)", worlditemID, Location, worlditemType, worlditemOp1, worlditemOp2);
        if(MySQL->GameQuery(query))
        {
            sprintf(answer, "Error while adding WorlditemID %d to MySQL DB. See console output for MySQL error", worlditemID);
            Chat->send(source, CHAT_DIRECT, "System", answer);
            MySQL->ShowGameSQLError();
            return;
        }
        sprintf(answer, "Added worlditemID %d, location %d as type %d", worlditemID, Location, worlditemType);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        Console->Print("IngameCommand: Added Worlditem ID %d, Location %d to Database", worlditemID, Char->GetLocation());
        return;
    }
}

void PCommands::doCmdadddor()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    int doorID = 0, doorType = 0;

    if(IsArgNumeric(1) == false || IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }
    else
    {
        doorID = GetArgInt(1);
        doorType = GetArgInt(2);
    }

    if(doorID == 0 || doorType == 0 && (doorType == 1 || doorType == 2) || SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@adddoor <doorid> <1=singledoor / 2=doubledoor>");
        return;
    }
    else
    {
        char query[1024], answer[1024];
        PChar *Char = Database->GetChar(source->GetCharID());
        Console->Print("IngameCommand: Adding Worlddoor ID %d, Location %d to Database", doorID, Char->GetLocation());
        sprintf(query, "INSERT INTO world_doors (`wd_world_id`, `wd_type`, `wd_world_map`) VALUES ('%d', '%d', '%d');", doorID, doorType, Char->GetLocation());
        if(MySQL->GameQuery(query) > 0)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Failed to add door; Check console for MySQL error");
            Console->Print("Failed to add door! MySQL returned %s", mysql_error(MySQL->GetGameHandle()));
            return;
        }
        if(doorType == 2)
        {
            sprintf(answer, "Succesfully added doorID %d in location %d as doubledoor", doorID, Char->GetLocation());
        }
        else if(doorType == 1)
        {
            sprintf(answer, "Succesfully added doorID %d in location %d as singledoor", doorID, Char->GetLocation());
        }
        Chat->send(source, CHAT_DIRECT, "System", answer);
    }

}

void PCommands::doCmdconlist()
{
    if (gDevDebug) Console->Print("IngameCommand: Sending connected-player-list to charID %d", source->GetCharID());
    Chat->sendConnectedList(source, false);
}

void PCommands::doCmdskin()
{
// -------------------------------------------------------
/*
Usage:  @skin #<chardef idx>
              for use with the index id from characters.def
  or  @skin <model>|-|# [<head>[ <torso>[ <legs>]]]
              <model> is the model id found after the model name in characters.def
              # resets to real char skin (optionnaly modified by following args)
              - means current skin
              incrementaly optional <head>, <torso> and <legs> are values 0-9
*/
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    u32 Skinval1, Skinval2, Skinval3, Skinval4;
    PChar *SkinChar = Database->GetChar(source->GetCharID());
    std::stringstream SkinChat;
    char SkinStr[128];

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@skin ( #<chardef idx> ) | ( <model> | # [<head>[ <torso>[ <legs>]]] )");
        return;
    }

    char tmpval[30];
    GetArgText(1, tmpval, 30);
    if((tmpval[0] == '#') && (tmpval[1] != '\0'))
    {
        Skinval1 = atoi(tmpval+1);
        SkinChar->SetCurrentLookFromCharType(atoi(tmpval+1));
        SkinChat << "Skin set to the skin of char type ";
        SkinChat << (int)Skinval1;
    }
    else
    {
        if ((tmpval[0] == '#') && (tmpval[1] == '\0'))
        {
            SkinChar->GetRealLook(Skinval1, Skinval2, Skinval3, Skinval4);
        }
        else if ((tmpval[0] == '-') && (tmpval[1] == '\0'))
        {
            SkinChar->GetCurrentLook(Skinval1, Skinval2, Skinval3, Skinval4);
        }
        else
        {
            Skinval1 = GetArgInt(1);
        }

        char tmp_arg2[30];
        char tmp_arg3[30];
        char tmp_arg4[30];

        GetArgText(2, tmp_arg2, 30);
        GetArgText(3, tmp_arg3, 30);
        GetArgText(4, tmp_arg4, 30);

        if(tmp_arg2[0] != '\0')
        {
            if(tmp_arg2[0] != '-')
            {
                tmp_arg2[1] = '\0';
                Skinval2 = GetArgInt(2);
            }
            if(tmp_arg3[0] != '\0')
            {
                if(tmp_arg3[0] != '-')
                {
                    tmp_arg3[1] = '\0';
                    Skinval3 = GetArgInt(3);
                }
                if(tmp_arg4[0] != '\0')
                {
                    if(tmp_arg4[0] != '-')
                    {
                        tmp_arg4[1] = '\0';
                        Skinval4 = GetArgInt(4);
                    }
                }
            }
        }

        SkinChar->SetCurrentLook(Skinval1, Skinval2, Skinval3, Skinval4);

        SkinChat << "Skin set to model ";
        SkinChat << (int)Skinval1 << " with head " << (int)Skinval2 << ", torso " << (int)Skinval3 << ", legs " << (int)Skinval4;
    }

    snprintf(SkinStr, 127, "%s", SkinChat.str().c_str());
    SkinStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", SkinStr);

    PMessage* tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
}

void PCommands::doCmdeffect()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == false)
        SyntaxError = true;

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@effect <effect: 0=none, 1 .. 17> [<density: 0=max .. 255=min>]");
        return;
    }

    u8 val1, val2;
    char effStr[128];
    PMessage* tmpMsg;

    val1 = (u8)(GetArgInt(1) & 0xff);
    val2 = (u8)(GetArgInt(2) & 0xff);

    source->GetChar()->SetBodyEffect(val1, val2);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Body effect set to value %d with density %d (but you can see it yourself)", val1, val2);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}

void PCommands::doCmdspeed()
{
// Speed override setting.
// Usage: @speed <newspeed> | #
//  with <speed> = 0 (no move).. 254 , 255 or # meaning "no speed override"

    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@speed <newspeed: 0 .. 254 > | 255 | #");
        return;
    }
    char tmpval[30];
    GetArgText(1, tmpval, 30);

    u8 val1;
    char effStr[128];
    PMessage* tmpMsg;

    val1 = ((tmpval[0] == '#') ? 255 : (u8)(atoi(tmpval) & 0xff));
    source->GetChar()->SetSpeedOverride(val1);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Speed override set to value %d ", val1);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}

void PCommands::doCmdcolor()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@color -|<head color: 0..255> [-|<torso color>]  [-|<legs color>]");
        return;
    }

    char tmp_v1[30];
    char tmp_v2[30];
    char tmp_v3[30];

    GetArgText(1, tmp_v1, 30);
    GetArgText(2, tmp_v2, 30);
    GetArgText(3, tmp_v3, 30);

    u8 val1, val2, val3, val4, val5, val6;
    char effStr[128];
    PMessage* tmpMsg;

    source->GetChar()->GetCurrentBodyColor(val1, val2, val3, val4, val5, val6);
    if(tmp_v1[0] != '-')
    val1 = (u8)(atoi(tmp_v1) & 0xff);
    if((tmp_v2[0] != '-') && (tmp_v2[0] != '\0'))
    val2 = (u8)(atoi(tmp_v2) & 0xff);
    if((tmp_v3[0] != '-') && (tmp_v3[0] != '\0'))
    val3 = (u8)(atoi(tmp_v3) & 0xff);
    source->GetChar()->SetCurrentBodyColor(val1, val2, val3, val4, val5, val6);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Body color set to values %d %d %d", val1, val2, val3);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}

void PCommands::doCmdbrightness()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@brightness -|<head brightness: 0..255> [-|<torso brightness>]  [-|<legs brightness>]");
        return;
    }

    char tmp_v1[30];
    char tmp_v2[30];
    char tmp_v3[30];

    GetArgText(1, tmp_v1, 30);
    GetArgText(2, tmp_v2, 30);
    GetArgText(3, tmp_v3, 30);

    u8 val1, val2, val3, val4, val5, val6;
    char effStr[128];
    PMessage* tmpMsg;

    source->GetChar()->GetCurrentBodyColor(val1, val2, val3, val4, val5, val6);
    if(tmp_v1[0] != '-')
    val4 = (u8)(atoi(tmp_v1) & 0xff);
    if((tmp_v2[0] != '-') && (tmp_v2[0] != '\0'))
    val5 = (u8)(atoi(tmp_v2) & 0xff);
    if((tmp_v3[0] != '-') && (tmp_v3[0] != '\0'))
    val6 = (u8)(atoi(tmp_v3) & 0xff);
    source->GetChar()->SetCurrentBodyColor(val1, val2, val3, val4, val5, val6);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Body brightness set to values %d %d %d", val4, val5, val6);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}

void PCommands::doCmdremove()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@remove <raw item id>");
        return;
    }

    char tmp_v1[30];
    GetArgText(1, tmp_v1, 30);

    u32 TargetID;
    char delStr[128];
    PMessage* tmpMsg;

    TargetID = (u32)(atoi(tmp_v1) & 0xffffffff);
    tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(source, TargetID, 5);

    ClientManager->UDPBroadcast(tmpMsg, source);
    tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(source, TargetID, 9);

    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(delStr, 127, "Item %d removed.", TargetID);
    delStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", delStr);
}

void PCommands::doCmdrehash()
{
    Console->Print("%s %s is rehashing the server", Console->ColorText(YELLOW, BLACK, "[Notice]", source->GetAccount()->GetName().c_str()));
    Chat->send(source, CHAT_DIRECT, "System", "Rehashing server...");
    Database->Rehash();
}

void PCommands::doCmduptime()
{
    // get difference between var uptime and current time
    std::time_t Uptime = GameServer->GetStartTime();
    u32 TimeDiff = std::time(NULL) - Uptime;
    u32 UpYears = 0, UpMonths = 0, UpWeeks = 0, UpDays = 0, UpHours = 0, UpMinutes = 0, UpSeconds = 0;
    char tmpY[21], tmpM[21], tmpW[21], tmpD[21], tmpH[21], tmpMi[21], tmpS[21];

    // 1 min = 60 sec                           = 60
    // 1 hour = 60 * 60 sec                     = 3600
    // 1 day = 24 * 60 * 60 sec                 = 86400
    // 1 week = 7 * 24 * 60 * 60 sec            = 604800
    // 1 month = 30 * 7 * 24 * 60 * 60 sec      = 18144000
    // 1 year = 12 * 30 * 7 * 24 * 60 * 60 sec  = 217728000

    // Split Years
    while(TimeDiff >= 217728000)
    {
        TimeDiff = TimeDiff - 217728000;
        UpYears++;
    }

    // Split Months
    while(TimeDiff >= 18144000)
    {
        TimeDiff = TimeDiff - 18144000;
        UpMonths++;
    }

    // Split Weeks
    while(TimeDiff >= 604800)
    {
        TimeDiff = TimeDiff - 604800;
        UpWeeks++;
    }

    // Split Days
    while(TimeDiff >= 86400)
    {
        TimeDiff = TimeDiff - 86400;
        UpDays++;
    }

    // Split Hours
    while(TimeDiff >= 3600)
    {
        TimeDiff = TimeDiff - 3600;
        UpHours++;
    }

    // Split Minutes
    while(TimeDiff >= 60)
    {
        TimeDiff = TimeDiff - 60;
        UpMinutes++;
    }

    // What's left are seconds
    UpSeconds = TimeDiff;

    // Now create output strings. Add 's' if > 1
    // Years
    if(UpYears > 1)
        snprintf(tmpY, 20, "%d years ", UpYears);
    else if(UpYears > 0)
        snprintf(tmpY, 20, "%d year ", UpYears);
    else
        tmpY[0] = '\0';

    // Months
    if(UpMonths > 1)
        snprintf(tmpM, 20, "%d months ", UpMonths);
    else if(UpMonths > 0)
        snprintf(tmpM, 20, "%d month ", UpMonths);
    else
        tmpM[0] = '\0';

    // Weeks
    if(UpWeeks > 1)
        snprintf(tmpW, 20, "%d weeks ", UpWeeks);
    else if(UpWeeks > 0)
        snprintf(tmpW, 20, "%d week ", UpWeeks);
    else
        tmpW[0] = '\0';

    // Days
    if(UpDays > 1)
        snprintf(tmpD, 20, "%d days ", UpDays);
    else if(UpDays > 0)
        snprintf(tmpD, 20, "%d day ", UpDays);
    else
        tmpD[0] = '\0';

    // Hours
    if(UpHours > 1)
        snprintf(tmpH, 20, "%d hours ", UpHours);
    else if(UpHours > 0)
        snprintf(tmpH, 20, "%d hour ", UpHours);
    else
        tmpH[0] = '\0';

    // Minutes
    if(UpMinutes > 1)
        snprintf(tmpMi, 20, "%d minutes and ", UpMinutes);
    else if(UpMinutes > 0)
        snprintf(tmpMi, 20, "%d minute and ", UpMinutes);
    else
        tmpMi[0] = '\0';

    // Seconds
    if(UpSeconds > 1 || UpSeconds == 0)
        snprintf(tmpS, 20, "%d seconds.", UpSeconds);
    else
        snprintf(tmpS, 20, "%d second.", UpSeconds);

    // Terminate all chars, just to be sure
    tmpY[20] = '\0';
    tmpM[20] = '\0';
    tmpW[20] = '\0';
    tmpD[20] = '\0';
    tmpH[20] = '\0';
    tmpMi[20] = '\0';
    tmpS[20] = '\0';

    //Copy the single msg's into one
    char tmpChatMsg[300];
    snprintf(tmpChatMsg, 299, "The server has been running for %s%s%s%s%s%s%s", tmpY, tmpM, tmpW, tmpD, tmpH, tmpMi, tmpS);
    tmpChatMsg[299] = '\0';

    // Send it out
    Chat->send(source, CHAT_DIRECT, "System", tmpChatMsg);
}

void PCommands::doCmdversion()
{
    char tmpChatMsg[300];
    snprintf(tmpChatMsg, 299, "You are on TinNS server %s runnig version %s - SVN Rev. %s", Config->GetOption("server_name").c_str(), ServerVersion, SVNRevision);
    tmpChatMsg[299] = '\0';

    Chat->send(source, CHAT_DIRECT, "System", tmpChatMsg);
}

void PCommands::doCmdkick()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@kick <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }

    // Make sure only people with a higher level than victim can kick victim
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant kick %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

// *************** Checks done, proceed with command
    int final_bantime = std::time(NULL) + 60;               // Ban 60 seconds (Anti-Rejoin)
    target->GetAccount()->SetBannedStatus(final_bantime);
    target->GetAccount()->SQLSave();
    GameServer->ClientDisconnected(target);                 // Kick

    Console->Print("%s %s (Lv %d) kicked %s (Lv %d)", Console->ColorText(YELLOW, BLACK, "[GameCommand]"), Database->GetChar(source->GetCharID())->GetName().c_str(), source->GetAccount()->GetLevel(), Database->GetChar(target->GetCharID())->GetName().c_str(), target->GetAccount()->GetLevel());
    char successmsg[100];
    snprintf(successmsg, 99, "Kicked %s", Database->GetChar(target->GetCharID())->GetName().c_str());
    successmsg[99] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", successmsg);
}

void PCommands::doCmdinfo()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@info <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    // *************** Checks done, proceed with command
    /*
        DIRECT> System: PlayerInformation
        DIRECT> Info: CharID     : %d     // victim->GetCharID();
        DIRECT> Info: AccountID  : %d     // victim->GetAccount()->GetID();
        DIRECT> Info: LoginName  : %s     // victim->GetAccount()->GetName();
        DIRECT> Info: AccessLevel: %d     // victim->GetAccount()->GetLevel();
        DIRECT> Info: Current Loc: %d     // Database->GetChar(source->GetCharID())->GetLocation();
        DIRECT> Info: IP address : %s     // victim->GetAddress():

        Maybe for future addons...
        DIRECT> System: CharInformation
        DIRECT> Info: Faction      : %d     // Database->GetChar(source->GetCharID())->GetFaction();
        DIRECT> Info: Cash         : %d     // Database->GetChar(source->GetCharID())->GetCash();
        DIRECT> Info: Soullight    : %d     // Database->GetChar(source->GetCharID())->GetSoullight();
    */
    char tmpInfo_head[151];
    char tmpInfo_cID[151];
    char tmpInfo_aID[151];
    char tmpInfo_Login[151];
    char tmpInfo_AxxLv[151];
    char tmpInfo_Loc[151];
    char tmpInfo_IP[151];

    snprintf(tmpInfo_head, 150,     "PlayerInformation");
    snprintf(tmpInfo_cID, 150,      "CharID     : %d",  target->GetCharID());
    snprintf(tmpInfo_aID, 150,      "AccountID  : %d",  target->GetAccount()->GetID());
    snprintf(tmpInfo_Login, 150,    "LoginName  : %s",  target->GetAccount()->GetName().c_str());
    snprintf(tmpInfo_AxxLv, 150,    "AccessLevel: %d",  target->GetAccount()->GetLevel());
    snprintf(tmpInfo_Loc, 150,      "Current Loc: %d",  Database->GetChar(target->GetCharID())->GetLocation());
    snprintf(tmpInfo_IP, 150,       "IP address : %s",  target->GetAddress());

    tmpInfo_head[150] = '\0';
    tmpInfo_cID[150] = '\0';
    tmpInfo_aID[150] = '\0';
    tmpInfo_Login[150] = '\0';
    tmpInfo_AxxLv[150] = '\0';
    tmpInfo_Loc[150] = '\0';
    tmpInfo_IP[150] = '\0';

    Chat->send(source, CHAT_DIRECT, "System", tmpInfo_head);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_cID);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_aID);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_Login);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_AxxLv);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_Loc);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_IP);
}

void PCommands::doCmdsetlevel()
{
    int destLevel = 0;
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }
    else
    {
        destLevel = GetArgInt(2);
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@setlevel <charID or nickname> <newlevel 1-99>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant set new level for %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    target->GetAccount()->SetLevel(destLevel);
    char tmpMsg[60], tmpMsg2[60];

    snprintf(tmpMsg, 59, "Set level for player %s to %d", Database->GetChar(target->GetCharID())->GetName().c_str(), destLevel);
    snprintf(tmpMsg2, 59, "**POOF** Your new accesslevel is now %d", destLevel);

    tmpMsg[59] = '\0';
    tmpMsg2[59] = '\0';

    Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
    Chat->send(target, CHAT_DIRECT, "System", tmpMsg2);
}

void PCommands::doCmdwarpto()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@warpto <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }

    int destZone = 0;
    // Warp GM/Admin to target player
    destZone = Database->GetChar(target->GetCharID())->GetLocation();
    /*if (gDevDebug)*/ Console->Print("%s Warping %d to location of %d (> %d)", Console->ColorText(YELLOW, BLACK, "[Notice]"), source->GetCharID(), target->GetCharID(), destZone);
    if (!Worlds->IsValidWorld(destZone))
    {
        Console->Print("%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText(RED, BLACK, "[PANIC]"), destZone);
        return;
    }
// *************** Checks done, proceed with command
    if (source->ChangeCharLocation(destZone))
    {
        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (source, destZone, 0);
        source->getUDPConn()->SendMessage(tmpMsg_zone);
        tmpMsg_zone = NULL;

        /*if (gDevDebug)*/ Console->Print("%s Warp successfull, now placing char near target", Console->ColorText(YELLOW, BLACK, "[Notice]"));
        // World changed, now place GM/Admin to position of target player "victim"
        PMessage* tmpMsg_posupdate;

        // Get position of target player "victim"
        u16 nNewX, nNewY, nNewZ;
        nNewX = target->GetChar()->Coords.mX;
        nNewY = target->GetChar()->Coords.mY;
        nNewZ = target->GetChar()->Coords.mZ;

        // And now built new message for GM/Admin, with new X/Y/Z Coords
        tmpMsg_posupdate = MsgBuilder->BuildCharPosMoveMsg(source, nNewX, nNewY, nNewZ);

        // Finish packet (Add UDP_ID stuff etc)
        source->IncreaseUDP_ID();
        tmpMsg_posupdate->U16Data(0x01) = source->GetUDP_ID();
        tmpMsg_posupdate->U16Data(0x03) = source->GetSessionID();
        tmpMsg_posupdate->U16Data(0x07) = source->GetUDP_ID();

        // Send packet
        source->getUDPConn()->SendMessage(tmpMsg_posupdate);
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), source->GetCharID(), destZone);
    }
}

void PCommands::doCmdrecall()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@recall <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    // Check accountlevel (Only higher's can perform commands on lower's)
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant warp %s to you, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    // Warp target player to GM/Admin
    int destZone = Database->GetChar(source->GetCharID())->GetLocation();
    if (!Worlds->IsValidWorld(destZone))
    {
        Console->Print("%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText(RED, BLACK, "[PANIC]"), destZone);
        return;
    }
// *************** Checks done, proceed with command
    if (target->ChangeCharLocation(destZone))
    {
        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (target, destZone, 0);
        target->getUDPConn()->SendMessage(tmpMsg_zone);
        tmpMsg_zone = NULL;

        // World changed, now place victim to position of GM/Admin
        PMessage* tmpMsg_posupdate;

        // Get position of GM/Admin
        u16 nNewX, nNewY, nNewZ;
        nNewX = source->GetChar()->Coords.mX;
        nNewY = source->GetChar()->Coords.mY;
        nNewZ = source->GetChar()->Coords.mZ;

        // And now built new message for Victim, with new X/Y/Z Coords
        tmpMsg_posupdate = MsgBuilder->BuildCharPosMoveMsg(target, nNewX, nNewY, nNewZ);

        // Finish packet (Add UDP_ID stuff etc)
        target->IncreaseUDP_ID();
        tmpMsg_posupdate->U16Data(0x01) = target->GetUDP_ID();
        tmpMsg_posupdate->U16Data(0x03) = target->GetSessionID();
        tmpMsg_posupdate->U16Data(0x07) = target->GetUDP_ID();

        // Send packet
        target->getUDPConn()->SendMessage(tmpMsg_posupdate);
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), target->GetCharID(), destZone);
    }
}

void PCommands::doCmdbroadcast()
{
    if(DumbMade == false)
    {
        Console->Print("%s Missing packetdumb in PCommands::doCmdbroadcast for declared broadcast function!", Console->ColorText(RED, BLACK, "[PANIC]"));
        return;
    }

    Chat->sendBroadcast(OrgPacketDumb+11);
}

void PCommands::doCmd_dev_t()
{
    if(IsAdmin() == false)
        return;

    char Arg1[30], Arg2[30];

    Arg1[0] = '\0';
    Arg2[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
        if(ArgC > 1)
        {
            GetArgText(2, Arg2, 30);
        }
    }

    u32 val1;
    u8 val2;
    char tmpStr[128];
    static PMessage* tmpMsg = NULL;

    if (!tmpMsg)
      tmpMsg = MsgBuilder->BuildCharHelloMsg(source);

    if(Arg1[0] != '\0' && Arg2[0] != '\0')
    {
        val1 = atoi(Arg1);
        val2 = (u8)(atoi(Arg2) & 0xff);
        tmpMsg->U8Data(16 + val1) = val2;
        snprintf(tmpStr, 127, "Data #%d set to value 0x%02x", val1, val2);
    }
    else
    {
      if (tmpMsg)
        delete tmpMsg;
      tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
      snprintf(tmpStr, 127, "Data reset to normal values");
    }

    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);

    PMessage* SendMsg = new PMessage(tmpMsg->GetMaxSize());
    (*SendMsg) = (*tmpMsg);
    ClientManager->UDPBroadcast(SendMsg, source);
}

void PCommands::doCmd_dev_h()
{
    if(IsAdmin() == false)
        return;

    char Arg1[30], Arg2[30];

    Arg1[0] = '\0';
    Arg2[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
        if(ArgC > 1)
        {
            GetArgText(2, Arg2, 30);
        }
    }

    if(source->GetAccount()->GetLevel() < PAL_ADMIN)
        return;

    u8 val1, val2;
    char tmpStr[128];

    if(Arg1[0] != '\0')
    {
      val1 = (u8)(atoi(Arg1) & 0xff);
      val2 = (u8)(atoi(Arg2) & 0xff);
    }
    else
    {
      val1 = 0x30;
      val2 = 0x01;
    }

    PMessage* tmpMsg = new PMessage(14);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //source->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // source->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)source->GetLocalID();
	*tmpMsg << (u8)0x30;
	*tmpMsg << (u8)val1; //Head Heath (% ?)
	*tmpMsg << (u8)val1; //Body Heath
	*tmpMsg << (u8)val1; //Feet Heath
	*tmpMsg << (u8)0x01;

    (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
    ClientManager->UDPBroadcast(tmpMsg, source);

    snprintf(tmpStr, 127, "Data set to values 0x%02x 0x%02x", val1, val2);
    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);
}

void PCommands::doCmd_dev_v()
{
    if(IsAdmin() == false)
        return;

    char Arg1[30];

    Arg1[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
    }

    char tmpStr[128];
    int testmode = 0; // change here only for u8/u16/u32 testvalue use

    if(Arg1[0] != '\0')
    {
        switch(testmode)
        {
            case 0: source->testval8 = (u8)(atoi(Arg1) & 0xff); break;
            case 1: source->testval16 = (u16)(atoi(Arg1) & 0xffff); break;
            case 2: source->testval32 = (u32)(atoi(Arg1) & 0xffffffff); break;
        }
    }
    else
    {
        source->testval8 = 0;
        source->testval16 = 0;
        source->testval32 = 0;
    }

    switch(testmode)
    {
        case 0: snprintf(tmpStr, 127, "Test value u8 set to 0x%02x (%d)", source->testval8, source->testval8); break;
        case 1: snprintf(tmpStr, 127, "Test value u16 set to 0x%04x (%d)", source->testval16, source->testval16); break;
        case 2: snprintf(tmpStr, 127, "Test value u32 set to 0x%08x (%d)", source->testval32, source->testval32); break;
        default: tmpStr[0] = '\0';
    }
    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);
}

void PCommands::doCmdban()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@ban <charID or nickname> <xS(econds)>/<xM(inutes)>/<xH(ours)>/<xD(ays)>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant ban %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }
    char tmpTimeVal[10];
    GetArgText(2, tmpTimeVal, 10);

    int loop_i = 0;
    char tmp_atoi[10];

    while(isdigit(tmpTimeVal[loop_i]) != 0 && loop_i < 10)
    {
        tmp_atoi[loop_i] = tmpTimeVal[loop_i];
        loop_i++;
    }
    char timefactor[1];
    timefactor[0] = tmpTimeVal[loop_i];
    int timevalue = atoi(tmp_atoi);
    int time_to_ban = 0;

    if(strcmp(timefactor, "s") == 0 || strcmp(timefactor, "S") == 0 )
    {
        time_to_ban = timevalue;
    }
    else if(strcmp(timefactor, "m") == 0 || strcmp(timefactor, "M") == 0 )
    {
        time_to_ban = timevalue * 60;
    }
    else if(strcmp(timefactor, "h") == 0 || strcmp(timefactor, "H") == 0 )
    {
        time_to_ban = timevalue * 60 * 60;
    }
    else if(strcmp(timefactor, "d") == 0 || strcmp(timefactor, "D") == 0 )
    {
        time_to_ban = timevalue * 60 * 60 * 24;
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@ban <charID or nickname> <xS(econds)>/<xM(inutes)>/<xH(ours)>/<xD(ays)>");
        return;
    }
    int final_bantime = std::time(NULL) + time_to_ban;
    target->GetAccount()->SetBannedStatus(final_bantime);
    target->GetAccount()->SQLSave();
    GameServer->ClientDisconnected(target);  // Now kick the player (Hes banned :) )
}

void PCommands::doCmdunban()
{
    Chat->send(source, CHAT_DIRECT, "System", "Unban is not yet possible, sorry.");
}

void PCommands::doCmdlistbans()
{
    Chat->send(source, CHAT_DIRECT, "System", "Sorry, namikon was too lazy to code that ^^");
    Chat->send(source, CHAT_DIRECT, "System", "Please wait until the ban is removed automaticly or edit your sqlDB");
    return;
}

void PCommands::doCmdshun()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@shun <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant shun %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }
    if(target->GetAccount()->IsShunned() == false)
    {
        target->GetAccount()->SetShun(true);
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "System", "Player is already shunned");
        return;
    }
}

void PCommands::doCmdunshun()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@shun <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant unshun %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }
    if(target->GetAccount()->IsShunned() == true)
    {
        target->GetAccount()->SetShun(false);
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "System", "Player is not shunned");
        return;
    }
}

void PCommands::doCmdjail()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@jail <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    // Check accountlevel (Only higher's can perform commands on lower's)
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant jail %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    u32 currLoc = target->GetChar()->GetLocation();
    if(currLoc == 550 || currLoc == 551 || currLoc == 552)
    {
        Chat->send(source, CHAT_DIRECT, "System", "Target player is already jailed!");
        return;
    }

// *************** Checks done, proceed with command
    int destZone = 550; // DarkMetal #1
    if (target->ChangeCharLocation(destZone))
    {
        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (target, destZone, 0);
        target->getUDPConn()->SendMessage(tmpMsg_zone);
        tmpMsg_zone = NULL;
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), target->GetCharID(), destZone);
    }
}

void PCommands::doCmdunjail()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@unjail <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    // Check accountlevel (Only higher's can perform commands on lower's)
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant unjail %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    u32 currLoc = target->GetChar()->GetLocation();
    if(currLoc != 550 && currLoc != 551 && currLoc != 552)
    {
        Chat->send(source, CHAT_DIRECT, "System", "Target player is not jailed!");
        return;
    }

// *************** Checks done, proceed with command
    int destZone = 503; // TH Sec2. Maybe set to appartment later ?
    if (target->ChangeCharLocation(destZone))
    {
        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (target, destZone, 0);
        target->getUDPConn()->SendMessage(tmpMsg_zone);
        tmpMsg_zone = NULL;
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), target->GetCharID(), destZone);
    }
}

void PCommands::doCmdteleport()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@teleport <charID or nickname> <destination worldID>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }
    // Check accountlevel (Only higher's can perform commands on lower's)
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant teleport %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    int destZone = GetArgInt(2);
    if (!Worlds->IsValidWorld(destZone))
    {
        Chat->send(source, CHAT_DIRECT, "System", "Invalid zoneID");
        return;
    }
// *************** Checks done, proceed with command
    if (target->ChangeCharLocation(destZone))
    {
        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (target, destZone, 0);
        target->getUDPConn()->SendMessage(tmpMsg_zone);
        tmpMsg_zone = NULL;
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), target->GetCharID(), destZone);
    }
}

/// ------- End commandset -------
PClient* PCommands::GetClientByID(int charid)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(it->second)
        {
            PClient* target = it->second;
            if((int)target->GetCharID() == charid)
                return it->second;
        }
    }
    return NULL;
}

PClient* PCommands::GetClientByNick(const char *nick)
{
    for(PClientMap::iterator it=ClientManager->getClientListBegin(); it!=ClientManager->getClientListEnd(); it++)
    {
        if(it->second)
        {
            PClient* target = it->second;
            if(!strcasecmp(Database->GetChar(target->GetCharID())->GetName().c_str(), nick))
                return it->second;
        }
    }
    return NULL;
}
/*  \/ - Unused?
bool PCommands::SendRawFile(PClient *Client, char *FileName, int protocoll) {
       ConnectionTCP *Socket = source->getTCPConn();
       ConnectionUDP *UDPSocket = source->getUDPConn();

       ifstream::pos_type size;
       char *buffer;

       ifstream hexdump (FileName, ios::in|ios::binary|ios::ate);
       if (hexdump.is_open()) {
            Console->Print("SendRawFile: Sending raw file %s", FileName);

            size = hexdump.tellg();
            buffer = new char [size];
            hexdump.seekg (0, ios::beg);
            hexdump.read (buffer, size);
            hexdump.close();

            if(protocoll == 1) {
               source->SetUDP_ID(source->GetUDP_ID()+1);
               *(u16*)&buffer[1] = source->GetUDP_ID();
               *(u16*)&buffer[3] = source->GetSessionID();
               *(u16*)&buffer[7] = source->GetUDP_ID();

                UDPSocket->write(buffer, size);
            } else if (protocoll == 2) {
                Socket->write(buffer, size);
            } else {
                Console->Print("SendRawFile: Error, wasnt able to get the required connection");
                return false;
                // ^- this message should never happen, but i feel better with it :P
            }

            delete[] buffer;
       } else {
            Console->Print("SendRawFile: Unable to send file %s", FileName);
            return false;
       }
       return true;
}
*/
bool PCommands::RequirePacketDumb()
{
    bool tmpNeedDumb = false;

    if(strcmp(Command, "broadcast") == 0)
        tmpNeedDumb = true;
    // Add more checks here

    return tmpNeedDumb;
}

bool PCommands::IsDevCommand()
{
    bool tmpIsDev = false;

    if(strcmp(Command, "t") == 0)
        tmpIsDev = true;
    else if(strcmp(Command, "h") == 0)
        tmpIsDev = true;
    else if(strcmp(Command, "v") == 0)
        tmpIsDev = true;
    // Add more checks here

    return tmpIsDev;
}

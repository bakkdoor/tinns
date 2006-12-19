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

	MODIFIED: 11 Dec 2005 Namikon
	REASON: - initial release
        MODIFIED: 15 Dec 2005 Namikon
        REASON: - Added @rawf command; Reads a rawdump from a file and sends that to the user.
                  Usage: @rawf <path> udp/tcp Ex: "@rawf dumps/1.hex tcp"
                  <path> is the path where the tinns binary is
        MODIFIED: 16 Dec 2005 bakkdoor
        REASON: - Added "connectedList". Sends the complete list of all connected clients to the user
        MODIFIED: 18 Dec 2005 Namikon
        REASON: - Added @sendchat command. This isnt really necessary, was just an debug command for testing
                  the new channelIDs in chat.h
        MODIFIED: 22 Dec 2005 Namikon
        REASON: - Experimental @warp command added (was: ToDo)
        MODIFIED: 23 Dec 2005 Namikon
        REASON: - Fixed warping. (was: Experimental)
                  Warping now works for all availeable city maps (NC/TH/TG/MB).
                  You can begin inside the subway and walk out of it, but you cant warp/sync in yet.
                  Same goes for wasteland syncs
        MODIFIED: 26 Dec Namikon
        REASON: - Added "adddoor" to add doors to MySQL Database.
                  Usage: "@adddoor <doorID> <1=singledoor / 2=doubledoor>"
        MODIFIED: 01 Jan Namikon
        REASON: - Added "delworlditem" and "addworlditem" to remove and add worlditems to DB (was: ToDo)
                - Improved char -> int conversation in 2 functions
                - Added 4th commandline argument to handle option2 in worlditems
        MODIFIED: 02 Jan Namikon
        REASON: - Added command @debug to enable/disable debug output of world item IDs (DIRECT> System: ID: <id> LOC: <world>)
        MODIFIED: 29 Jul Hammag
        REASON: - Added command @skin to play with skins :-P
                  Usage:  @skin #<chardef idx>
                                  for use with the index id from characters.def
                      or  @skin <model>|-|# [<head>[ <torso>[ <legs>]]]
                                  <model> is the model id found after the model name in characters.def
                                  # resets to real char skin
                                  - means current skin
                                  incrementaly optional <head>, <torso> and <legs> are values 0-9
                      As a side effect, just doing @skin - will display you current skin infos (and change to the same ...)

        MODIFIED: 9 Sep Hammag
        REASON: - Added command @effect to play with skins effect :-P
                  Usage:  @effect <effect> [<density>]
                              with <effect> = 0 to 17 and <density> = 0 (max density) to 255 (min density)
                              <density> is meaningfull for some of the effects only
                  Note: you won't see the effect on yourself. Only other chars will see it ...
                  Effects found:
                  0 = no effect
                  1 = poison smoke
                  2 = electricity
                  3 = deflector (instant white flash)
                  4 = heal bubbles
                  5 = be a light in the darkness !!!
                  6 = flames
                  7 = stun particles
                  8 = group heal
                  9 = group deflector (?)
                  10= damage boost
                  11= group damage boost
                  12= shelter (instant yellow flash)
                  13= group deflector (?)
                  14= stealth
                  15= anti poison aura
                  16= I don't remember this one: swarming yellow & blue bubbles
                  17= true sight
        REASON: - Added command @speed to play with speed
                  Usage:  @speed <newspeed> | #
                              with <speed> = 0 (no move).. 254 , 255 or # meaning "no speed override"
        MODIFIED: 22 Sep Hammag
        REASON: - Added command @color to set skin color
                  Usage: @color -|<head color: 0..255> [-|<torso color>]  [-|<legs color>]
                            with - meaning "no change"
                - Added command @brightness to set skin color brightness
                  Usage: @brightness -|<head brightness: 0..255> [-|<torso brightness>]  [-|<legs brightness>]
                            with 0 for max brightness, 255 for max darkness, and - meaning "no change"
        MODIFIED: 09 Oct Hammag
        REASON: - Modified and extended @debug command
        MODIFIED: 08 Nov Hammag
        REASON: - Modified @warp zone check so that we can warp to any really valid zone (even apartment) and only to valid zones
        MODIFIED: 08 Dec Namikon
        REASON: - Added @rehash
        MODIFIED: 10 Dec Namikon
        REASON: - Added @uptime
        MODIFIED: 11 Dec Hammag
        REASON: - Added @version

	ToDo:
	- Fix Subwaysyncy
	- Fix Wastelandsyncy
*/

#include "main.h"

#include "client.h"
#include "msgbuilder.h"
#include "worlds.h"

char output[2048];

void HandleGameCommand(char *ChatText, PClient *Client) {
     char Command[30], Arg1[30], Arg2[30], Arg3[30], Arg4[30];
     int i, j;

     i = 1;
     j = 0;

     do {
          Command[j] = ChatText[i];
          i++;
          j++;
     } while (ChatText[i] != ' ' && ChatText[i] != '\0');
     Command[j] = '\0';

     j = 0;
     i++;
     if(ChatText[i] != '\0') {
          do {
               Arg1[j] = ChatText[i];
               i++;
               j++;
          } while (ChatText[i] != ' ' && ChatText[i] != '\0');
     }
     Arg1[j] = '\0';

     j = 0;
     i++;
     if(ChatText[i] != '\0') {
          do {
               Arg2[j] = ChatText[i];
               i++;
               j++;
          } while (ChatText[i] != ' ' && ChatText[i] != '\0');
     }
     Arg2[j] = '\0';

     j = 0;
     i++;
     if(ChatText[i] != '\0') {
          do {
               Arg3[j] = ChatText[i];
               i++;
               j++;
          } while (ChatText[i] != ' ' && ChatText[i] != '\0');
     }
     Arg3[j] = '\0';

     j = 0;
     i++;
     if(ChatText[i] != '\0') {
          do {
               Arg4[j] = ChatText[i];
               i++;
               j++;
          } while (ChatText[i] != ' ' && ChatText[i] != '\0');
     }
     Arg4[j] = '\0';

// -------------------------------------------------------
   if(strcmp(Command, "debug") == 0) {
       if(CmdAccess->GetOptionInt("debug") > Client->GetAccount()->GetLevel()) return;

      PDebugMode nWhat = DBG_ALL;
      int nHow = -1;
      char* DbgTarget = "all";
      char DbgMessage[80];

      if(Arg1[0] == '0')
      {
        nHow = 0;
      }
      else if(Arg1[0] == '1')
      {
        nHow = 1;
      }
      else if(strncmp(Arg1, "loc", 3) == 0) {
        nWhat = DBG_LOCATION;
        DbgTarget = "location";
      }
      else if(strncmp(Arg1, "it", 2) == 0) {
        nWhat = DBG_ITEMID;
        DbgTarget = "itemid";
      }

      if (nWhat != DBG_ALL)
      {
        if(Arg2[0] == '\0')
        {
          nHow = (Client->GetDebugMode(nWhat) ? 0 : 1); // toggle if no arg
        }
        else if(Arg2[0] == '0')
        {
          nHow = 0;
        }
        else if(Arg2[0] == '1')
        {
          nHow = 1;
        }
      }

      if (nHow != -1)
      {
        Client->SetDebugMode(nWhat, nHow);
        snprintf(DbgMessage, 80, "Debug %s is now %s", DbgTarget, (nHow ? "ENABLED" : "DISABLED"));
        Chat->send(Client, CHAT_DIRECT, "System", DbgMessage);
      }
      else
      {
        Chat->send(Client, CHAT_GM, "Usage", "@debug [loc[ation] | it[emid]] [0|1]");
      }
   }
// -------------------------------------------------------
   if(strcmp(Command, "settime") == 0) {
       if(CmdAccess->GetOptionInt("settime") > Client->GetAccount()->GetLevel()) return;

       int newtime = 0;
       newtime = std::atoi(Arg1);

       if(newtime == 0) {
           Chat->send(Client, CHAT_DIRECT, "System", "Syntax error. @settime <raw timevalue>");
           return;
       }

       /*u8 timepacket[] = {0x13, 0x04, 0x00, 0x21, 0x94,
       0x10,//                 \		Time		  /
       0x03, 0x04, 0x00, 0x0D, 0x8B, 0xDD, 0x0B, 0x00, 0x47, 0xC0,
       0x22, 0x00, 0xE5, 0x0A, 0xBB, 0x00};

       //Client->IncreaseUDP_ID();
       Client->SetUDP_ID(Client->GetUDP_ID()+1);
       *(u16*)&timepacket[1] = Client->GetUDP_ID();
           //(u16*)&timepacket[3] = Client->GetSessionID();
       *(u16*)&timepacket[3] = 37917+Client->GetUDP_ID();
       *(u16*)&timepacket[7] = Client->GetUDP_ID();
       *(u32*)&timepacket[10] = newtime;
*/
       GameServer->SetGameTime(newtime);
       Console->Print("IngameCommand: CharID %d set ingametime to %d", Client->GetCharID(), newtime);

  //     Client->getUDPConn()->write(timepacket, sizeof(timepacket));
   }
// -------------------------------------------------------
   if(strcmp(Command, "warp") == 0) {
       if(CmdAccess->GetOptionInt("warp") > Client->GetAccount()->GetLevel()) return;

       int zoneID = std::atoi(Arg1);
       int SpawnPointID = std::atoi(Arg2);

       if(zoneID == 0) {
           Chat->send(Client, CHAT_DIRECT, "System", "Syntax error. @warp <zoneid> [<spawn location>]");
           return;
       }

       //const PDefWorld *def = GameDefs->GetWorldDef(zoneID);
       //if(!def)
       if (!Worlds->IsValidWorld(zoneID))
       {
           Chat->send(Client, CHAT_DIRECT, "System", "Invalid zoneID");
           return;
       }

       //PChar *Char = Database->GetChar(Client->GetCharID());

	   /* u8 ZonePacket[] = {0x13, 0xc3, 0x00, 0x63, 0xf8,
        0x0f, 0x03, 0xc3, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x04, 0x00, 0xda,
        0xcf, 0x03, 0x00, 0x01, 0x00, 0x15, 0x1b, 0x22, 0x01, 0x00, 0x00,
        0x1f, 0x49, 0x82, 0x81, 0x81, 0xe5, 0x6b, 0x04, 0xd5, 0x76, 0x01,
        0x00, 0x00, 0x00, 0x11, 0x11};
     u8 ZonePacket[] = {0x13, 0xc3, 0x00, 0x63, 0xf8, // shortened version from NeoX
        0x0f, 0x03, 0xc3, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x04, 0x00, 0xda,
        0xcf, 0x03, 0x00, 0x01, 0x00};

       //Client->IncreaseUDP_ID();
       Client->SetUDP_ID(Client->GetUDP_ID()+1);
       *(u16*)&ZonePacket[1] = Client->GetUDP_ID();
       *(u16*)&ZonePacket[3] = Client->GetSessionID();
       *(u16*)&ZonePacket[7] = Client->GetUDP_ID();
       *(u32*)&ZonePacket[15] = zoneID;

       *(u16*)&ZonePacket[10] = Client->GetLocalID(); // from NeoX
       *(u16*)&ZonePacket[19] = SpawnPointID; // from NeoX
       // ZonePacket[1] = (char)Test; // from NeoX */

       if (Client->ChangeCharLocation(zoneID))
       {
Console->Print("IngameCommand: Warping player %d to zone %d (%s)", Client->GetCharID(), zoneID, Worlds->GetWorld(zoneID)->GetName().c_str());
       //Client->getUDPConn()->write(ZonePacket, sizeof(ZonePacket));
        PMessage* tmpMsg = MsgBuilder->BuildAptLiftUseMsg (Client, zoneID, SpawnPointID);
        Client->getUDPConn()->SendMessage(tmpMsg);
       }
       else
       {
Console->Print(RED, BLACK, "IngameCommand: Can't change location when trying to warp player %d to zone %d", Client->GetCharID(), zoneID);
       }
   }
// -------------------------------------------------------
#define DISABLE_RAWF
   if(strcmp(Command, "rawf") == 0) {
#ifdef DISABLE_RAWF
Chat->send(Client, CHAT_DIRECT, "System", "Command disabled, request logged");
Console->Print(RED, BLACK, "Client %d requested file %s with proto %s", Client->GetCharID(), Arg1, Arg2);
return;
#else
       if(strcmp(Arg2, "udp") != 0 && strcmp(Arg2, "tcp") != 0) {
          //SendDChat(Client, "SYSTEM", "Syntax is @rawf <file> <udp/tcp>");
          Chat->send(Client, CHAT_DIRECT, "System", "Syntax is @rawf <file> <udp/tcp>");
          return;
       }

       ConnectionTCP *Socket = Client->getTCPConn();
       ConnectionUDP *UDPSocket = Client->getUDPConn();

       ifstream::pos_type size;
       char *buffer;

       ifstream hexdump (Arg1, ios::in|ios::binary|ios::ate);
       if (hexdump.is_open()) {
            Console->Print("IngameCommand: Sending packet file %s", Arg1);
//            This output is annoying so i commented it out. Was for debugging :)
//            FmtTxt("Sending RAW packetfile \"%s\" Using \"%s\"", Arg1, Arg2);
//            SendDChat(Client, "SYSTEM", output);

            size = hexdump.tellg();
	    buffer = new char [size];
  	    hexdump.seekg (0, ios::beg);

            hexdump.read (buffer, size);
            hexdump.close();
            if(strcmp(Arg2, "debug") == 0) {
         	int k;
                Console->Print("Byte dump of %s:", Arg1);
	        for(k=0;k<size;k++) {
	           Console->Print("Byte %d: %#x", k, buffer[k]);
	        }
            }
            if(strcmp(Arg2, "udp") == 0) {
                UDPSocket->write(buffer, size);
            } else if (strcmp(Arg2, "tcp") == 0) {
                Socket->write(buffer, size);
            } else {
                Console->Print("Error in @rawf function, wasnt able to get the required connection");
                // ^- this message should never happen, but i feel better with it :P
            }

            delete[] buffer;
       } else {
            Console->Print("IngameCommand: Unable to send file %s", Arg1);
            sprintf(output, "File not found: %s", Arg1);
            //SendDChat(Client, "SYSTEM", output);
            Chat->send(Client, CHAT_DIRECT, "System", output);

       }
#endif
   }
// -------------------------------------------------------
   if(strcmp(Command, "delworlditem") == 0) {
       if(CmdAccess->GetOptionInt("delworlditem") > Client->GetAccount()->GetLevel()) return;

       PChar *Char = Database->GetChar(Client->GetCharID());
       char query[255], answer[255];
       int worlditemID = 0;
       int Location = Char->GetLocation();

       if(Arg1) worlditemID = std::atoi(Arg1);

       if(worlditemID == 0)
       {
           Chat->send(Client, CHAT_DIRECT, "System", "Syntax: @delworlditem <ItemID>");
           return;
       }

       int checkval = MySQL->GetWorldItemType((unsigned short)worlditemID, Location);
       if(checkval < 1 || checkval > 5)
       {
           sprintf(answer, "Cannot delete WorlditemID %d, not in database!", worlditemID);
           Chat->send(Client, CHAT_DIRECT, "System", answer);
           return;
       }

       sprintf(query, "DELETE FROM world_items WHERE wi_worlditem_id = %d AND wi_worlditem_map = %d", worlditemID, Location);
       if(MySQL->GameQuery(query))
       {
           sprintf(answer, "Unable to delete worlditemID %d, see console for MySQL error", worlditemID);
           Chat->send(Client, CHAT_DIRECT, "System", answer);
           MySQL->ShowGameSQLError();
           return;
       }
       else
       {
           sprintf(answer, "Successfully deleted worlditemID %d, location %d from Database", worlditemID, Location);
           Chat->send(Client, CHAT_DIRECT, "System", answer);
           return;
       }
   }
// -------------------------------------------------------
   if(strcmp(Command, "addworlditem") == 0) {
       if(CmdAccess->GetOptionInt("addworlditem") > Client->GetAccount()->GetLevel()) return;
       PChar *Char = Database->GetChar(Client->GetCharID());
       char query[255], answer[255];
       int worlditemID = 0, worlditemType = 0, worlditemOp1 = 0, worlditemOp2 = 0;
       int Location = Char->GetLocation();

       if(Arg1) worlditemID = std::atoi(Arg1);
       if(Arg2) worlditemType = std::atoi(Arg2);
       if(Arg3) worlditemOp1 = std::atoi(Arg3);
       if(Arg4) worlditemOp2 = std::atoi(Arg4);

       if(worlditemID == 0 || worlditemType == 0)
       {
           Chat->send(Client, CHAT_DIRECT, "System", "Syntax: @addworlditem <ItemID> <ItemType> [<Option1> <Option2>]");
           return;
       }
       if(worlditemType < 1 || worlditemType > 19)
       {
           Chat->send(Client, CHAT_DIRECT, "System", "Invalid Worlditemtype (1-19)");
           return;
       }

       if (Location > 100000)
       {
           Chat->send(Client, CHAT_DIRECT, "System", "You cant add worlditems in appartments at the moment");
           return;
       }
       else
       {
           int checkval = MySQL->GetWorldItemType((unsigned short)worlditemID, Location);
           if(checkval > 0 && checkval < 5)
           {
               sprintf(answer, "WorlditemID %d is already in the Database! Use @delworlditem <id> to remove it first", worlditemID);
               Chat->send(Client, CHAT_DIRECT, "System", answer);
               return;
           }
           sprintf (query, "INSERT INTO world_items (wi_worlditem_id, wi_worlditem_map, wi_type, wi_option1, wi_option2) VALUES (%d, %d, %d, %d, %d)", worlditemID, Location, worlditemType, worlditemOp1, worlditemOp2);
           if(MySQL->GameQuery(query))
           {
               sprintf(answer, "Error while adding WorlditemID %d to MySQL DB. See console output for MySQL error", worlditemID);
               Chat->send(Client, CHAT_DIRECT, "System", answer);
               MySQL->ShowGameSQLError();
               return;
           }
           sprintf(answer, "Added worlditemID %d, location %d as type %d", worlditemID, Location, worlditemType);
           Chat->send(Client, CHAT_DIRECT, "System", answer);
           Console->Print("IngameCommand: Added Worlditem ID %d, Location %d to Database", worlditemID, Char->GetLocation());
           return;
       }
   }
// -------------------------------------------------------
   if(strcmp(Command, "adddoor") == 0) {
       if(CmdAccess->GetOptionInt("adddoor") > Client->GetAccount()->GetLevel()) return;
       int doorID = 0, doorType = 0;
       doorID = std::atoi(Arg1);
       doorType = std::atoi(Arg2);

       if(doorID == 0 || doorType == 0 && (doorType == 1 || doorType == 2))
       {
           Chat->send(Client, CHAT_DIRECT, "System", "Failed to add door; Syntax is: @adddoor <doorid> <1=singledoor / 2=doubledoor>");
           return;
       }
       else
       {
           char query[1024], answer[1024];
           PChar *Char = Database->GetChar(Client->GetCharID());
           Console->Print("IngameCommand: Adding Worlddoor ID %d, Location %d to Database", doorID, Char->GetLocation());
           sprintf(query, "INSERT INTO world_doors (`wd_world_id`, `wd_type`, `wd_world_map`) VALUES ('%d', '%d', '%d');", doorID, doorType, Char->GetLocation());
           if(MySQL->GameQuery(query) > 0)
           {
               Chat->send(Client, CHAT_DIRECT, "System", "Failed to add door; Check console for MySQL error");
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
           Chat->send(Client, CHAT_DIRECT, "System", answer);
       }
   }
// -------------------------------------------------------
   if(strcmp(Command, "connectedList") == 0) {
       if(CmdAccess->GetOptionInt("connectedList") > Client->GetAccount()->GetLevel()) return;
       Console->Print("IngameCommand: Sending connected-player-list to client %s", Arg1);
       Chat->sendConnectedList(Client, false);
   }
// -------------------------------------------------------
   if(strcmp(Command, "sendchat") == 0) {
       if(CmdAccess->GetOptionInt("sendchat") > Client->GetAccount()->GetLevel()) return;
      if(strcmp(Arg1, "buddy") == 0) {
             //SendChat(Client, CHAT_BUDDY, "NickName", "TextToSend");
             Chat->send(Client, CHAT_BUDDY, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "local") == 0) {
             //SendChat(Client, CHAT_LOCAL, "NickName", "TextToSend");
             Chat->send(Client, CHAT_LOCAL, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "clan") == 0) {
             //SendChat(Client, CHAT_CLAN, "NickName", "TextToSend");
             Chat->send(Client, CHAT_CLAN, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "team") == 0) {
             //SendChat(Client, CHAT_TEAM, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TEAM, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "direct") == 0) {
             //SendChat(Client, CHAT_DIRECT, "NickName", "TextToSend");
             Chat->send(Client, CHAT_DIRECT, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "zone") == 0) {
             //SendChat(Client, CHAT_ZONE, "NickName", "TextToSend");
             Chat->send(Client, CHAT_ZONE, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "frak") == 0) {
             //SendChat(Client, CHAT_FRAK, "NickName", "TextToSend");
             Chat->send(Client, CHAT_FRAK, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "tradecs") == 0) {
             //SendChat(Client, CHAT_TRADECS, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TRADECS, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "trademb") == 0) {
             //SendChat(Client, CHAT_TRADEMB, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TRADEMB, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "tradenc") == 0) {
             //SendChat(Client, CHAT_TRADENC, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TRADENC, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "tradeth") == 0) {
             //SendChat(Client, CHAT_TRADETH, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TRADETH, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "tradewl") == 0) {
             //SendChat(Client, CHAT_TRADEWL, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TRADEWL, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "ooc") == 0) {
             //SendChat(Client, CHAT_OOC, "NickName", "TextToSend");
             Chat->send(Client, CHAT_OOC, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "help") == 0) {
             //SendChat(Client, CHAT_HELP, "NickName", "TextToSend");
             Chat->send(Client, CHAT_HELP, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "clansearch") == 0) {
             //SendChat(Client, CHAT_CLANSEARCH, "NickName", "TextToSend");
             Chat->send(Client, CHAT_CLANSEARCH, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "servicecs") == 0) {
             //SendChat(Client, CHAT_SERVICECS, "NickName", "TextToSend");
             Chat->send(Client, CHAT_SERVICECS, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "servicesmb") == 0) {
             //SendChat(Client, CHAT_SERVICESMB, "NickName", "TextToSend");
             Chat->send(Client, CHAT_SERVICESMB, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "servicesnc") == 0) {
             //SendChat(Client, CHAT_SERVICESNC, "NickName", "TextToSend");
             Chat->send(Client, CHAT_SERVICESNC, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "servicesth") == 0) {
             //SendChat(Client, CHAT_SERVICESTH, "NickName", "TextToSend");
             Chat->send(Client, CHAT_SERVICESTH, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "serviceswl") == 0) {
             //SendChat(Client, CHAT_SERVICESWL, "NickName", "TextToSend");
             Chat->send(Client, CHAT_SERVICESWL, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "team10") == 0) {
             //SendChat(Client, CHAT_TEAM10, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TEAM10, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "team30") == 0) {
             //SendChat(Client, CHAT_TEAM30, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TEAM30, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "team50") == 0) {
             //SendChat(Client, CHAT_TEAM50, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TEAM50, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "team70") == 0) {
             //SendChat(Client, CHAT_TEAM70, "NickName", "TextToSend");
             Chat->send(Client, CHAT_TEAM70, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "admin") == 0) {
             //SendChat(Client, CHAT_ADMIN, "NickName", "TextToSend");
             Chat->send(Client, CHAT_ADMIN, "Nickname", "TextToSend");
      } else if(strcmp(Arg1, "gm") == 0) {
             //SendChat(Client, CHAT_GM, "NickName", "TextToSend");
             Chat->send(Client, CHAT_GM, "Nickname", "TextToSend");
      }

   }
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
  if(strcmp(Command, "skin") == 0)
  {
if(CmdAccess->GetOptionInt("skin") > Client->GetAccount()->GetLevel()) return;
    u32 Skinval1, Skinval2, Skinval3, Skinval4;
    PChar *SkinChar = Database->GetChar(Client->GetCharID());
    std::stringstream SkinChat;
    char SkinStr[128];

    if(Arg1[0] == '\0')
    {
      Chat->send(Client, CHAT_GM, "Usage", "@skin ( #<chardef idx> ) | ( <model> | # [<head>[ <torso>[ <legs>]]] )");
    }
    else
    {
      if ((Arg1[0] == '#') && (Arg1[1] != '\0'))
      {
        Skinval1 = atoi(Arg1+1);
        SkinChar->SetCurrentLookFromCharType(atoi(Arg1+1));
        SkinChat << "Skin set to the skin of char type ";
        SkinChat << (int)Skinval1;
      }
      else
      {
        if ((Arg1[0] == '#') && (Arg1[1] == '\0'))
        {
          SkinChar->GetRealLook(Skinval1, Skinval2, Skinval3, Skinval4);
        }
        else if ((Arg1[0] == '-') && (Arg1[1] == '\0'))
        {
          SkinChar->GetCurrentLook(Skinval1, Skinval2, Skinval3, Skinval4);
        }
        else
        {
          Skinval1 = atoi(Arg1);
        }

        if(Arg2[0] != '\0')
        {
          if(Arg2[0] != '-')
          {
            Arg2[1] = '\0';
            Skinval2 = atoi(Arg2);
          }
          if(Arg3[0] != '\0')
          {
            if(Arg3[0] != '-')
            {
              Arg3[1] = '\0';
              Skinval3 = atoi(Arg3);
            }
            if(Arg4[0] != '\0')
            {
              if(Arg4[0] != '-')
              {
                Arg4[1] = '\0';
                Skinval4 = atoi(Arg4);
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
      Chat->send(Client, CHAT_DIRECT, "System", SkinStr);

      PMessage* tmpMsg = MsgBuilder->BuildCharHelloMsg(Client);
      ClientManager->UDPBroadcast(tmpMsg, Client);
    }
  }
// -------------------------------------------------------
// Body effect setting. See Changes at the begining of this file

if(strcmp(Command, "effect") == 0)
  {
      if(CmdAccess->GetOptionInt("effect") > Client->GetAccount()->GetLevel()) return;
    u8 val1, val2;
    char effStr[128];
    PMessage* tmpMsg;

    if(Arg1[0] != '\0')
    {
      val1 = (u8)(atoi(Arg1) & 0xff);
      val2 = (u8)(atoi(Arg2) & 0xff);
      Client->GetChar()->SetBodyEffect(val1, val2);

      tmpMsg = MsgBuilder->BuildCharHelloMsg(Client);
      ClientManager->UDPBroadcast(tmpMsg, Client);
      snprintf(effStr, 127, "Body effect set to value %d with density %d (but you can see it yourself)", val1, val2);
      effStr[127] = '\0';
      Chat->send(Client, CHAT_DIRECT, "System", effStr);
    }
    else
    {
      Chat->send(Client, CHAT_GM, "Usage", "@effect <effect: 0=none, 1 .. 17> [<density: 0=max .. 255=min>]");
    }
  }

// -------------------------------------------------------
// Speed override setting.
// Usage: @speed <newspeed> | #
//  with <speed> = 0 (no move).. 254 , 255 or # meaning "no speed override"

if(strcmp(Command, "speed") == 0)
  {
      if(CmdAccess->GetOptionInt("speed") > Client->GetAccount()->GetLevel()) return;
    u8 val1;
    char effStr[128];
    PMessage* tmpMsg;

    if(Arg1[0] != '\0')
    {
      val1 = ((Arg1[0] == '#') ? 255 : (u8)(atoi(Arg1) & 0xff));
      Client->GetChar()->SetSpeedOverride(val1);

      tmpMsg = MsgBuilder->BuildCharHelloMsg(Client);
      ClientManager->UDPBroadcast(tmpMsg, Client);
      snprintf(effStr, 127, "Speed override set to value %d ", val1);
      effStr[127] = '\0';
      Chat->send(Client, CHAT_DIRECT, "System", effStr);
    }
    else
    {
      Chat->send(Client, CHAT_GM, "Usage", "@speed <newspeed: 0 .. 254 > | 255 | #");
    }
  }

// Skin color setting.
if(strcmp(Command, "color") == 0)
  {
      if(CmdAccess->GetOptionInt("color") > Client->GetAccount()->GetLevel()) return;
    u8 val1, val2, val3, val4, val5, val6;
    char effStr[128];
    PMessage* tmpMsg;

    if(Arg1[0] != '\0')
    {
      Client->GetChar()->GetCurrentBodyColor(val1, val2, val3, val4, val5, val6);
      if(Arg1[0] != '-')
        val1 = (u8)(atoi(Arg1) & 0xff);
      if((Arg2[0] != '-') && (Arg2[0] != '\0'))
        val2 = (u8)(atoi(Arg2) & 0xff);
      if((Arg3[0] != '-') && (Arg3[0] != '\0'))
        val3 = (u8)(atoi(Arg3) & 0xff);
      Client->GetChar()->SetCurrentBodyColor(val1, val2, val3, val4, val5, val6);

      tmpMsg = MsgBuilder->BuildCharHelloMsg(Client);
      ClientManager->UDPBroadcast(tmpMsg, Client);
      snprintf(effStr, 127, "Body color set to values %d %d %d", val1, val2, val3);
      effStr[127] = '\0';
      Chat->send(Client, CHAT_DIRECT, "System", effStr);
    }
    else
    {
      Chat->send(Client, CHAT_GM, "Usage", "@color -|<head color: 0..255> [-|<torso color>]  [-|<legs color>]");
    }
  }

// Skin brightness setting.
if(strcmp(Command, "brightness") == 0)
  {
      if(CmdAccess->GetOptionInt("brightness") > Client->GetAccount()->GetLevel()) return;
    u8 val1, val2, val3, val4, val5, val6;
    char effStr[128];
    PMessage* tmpMsg;

    if(Arg1[0] != '\0')
    {
      Client->GetChar()->GetCurrentBodyColor(val1, val2, val3, val4, val5, val6);
      if(Arg1[0] != '-')
        val4 = (u8)(atoi(Arg1) & 0xff);
      if((Arg2[0] != '-') && (Arg2[0] != '\0'))
        val5 = (u8)(atoi(Arg2) & 0xff);
      if((Arg3[0] != '-') && (Arg3[0] != '\0'))
        val6 = (u8)(atoi(Arg3) & 0xff);
      Client->GetChar()->SetCurrentBodyColor(val1, val2, val3, val4, val5, val6);

      tmpMsg = MsgBuilder->BuildCharHelloMsg(Client);
      ClientManager->UDPBroadcast(tmpMsg, Client);
      snprintf(effStr, 127, "Body brightness set to values %d %d %d", val4, val5, val6);
      effStr[127] = '\0';
      Chat->send(Client, CHAT_DIRECT, "System", effStr);
    }
    else
    {
      Chat->send(Client, CHAT_GM, "Usage", "@brightness -|<head brightness: 0..255> [-|<torso brightness>]  [-|<legs brightness>]");
    }
  }

if(strcmp(Command, "remove") == 0)
  {
      if(CmdAccess->GetOptionInt("remove") > Client->GetAccount()->GetLevel()) return;
    u32 TargetID;
    char delStr[128];
    PMessage* tmpMsg;

    if(Arg1[0] != '\0')
    {
      TargetID = (u32)(atoi(Arg1) & 0xffffffff);
      tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(Client, TargetID, 5);
      ClientManager->UDPBroadcast(tmpMsg, Client);
      tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(Client, TargetID, 9);
      ClientManager->UDPBroadcast(tmpMsg, Client);
      snprintf(delStr, 127, "Item %d removed.", TargetID);
      delStr[127] = '\0';
      Chat->send(Client, CHAT_DIRECT, "System", delStr);
    }
    else
    {
      Chat->send(Client, CHAT_GM, "Usage", "@remove <raw item id>");
    }
  }
if(strcmp(Command, "rehash") == 0)
{
    if(CmdAccess->GetOptionInt("rehash") > Client->GetAccount()->GetLevel()) return;
    Chat->send(Client, CHAT_DIRECT, "System", "Rehashing server...");
    Database->Rehash();
}
if(strcmp(Command, "uptime") == 0)
{
    if(CmdAccess->GetOptionInt("uptime") > Client->GetAccount()->GetLevel()) return;
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
    Chat->send(Client, CHAT_DIRECT, "System", tmpChatMsg);
}
if(strcmp(Command, "version") == 0)
{
    if(CmdAccess->GetOptionInt("version") > Client->GetAccount()->GetLevel()) return;
    char tmpChatMsg[300];
    snprintf(tmpChatMsg, 299, "You are on TinNS server %s runnig version %s - SVN Rev. %s", Config->GetOption("server_name").c_str(), ServerVersion, SVNRevision);
    tmpChatMsg[299] = '\0';

    Chat->send(Client, CHAT_DIRECT, "System", tmpChatMsg);
}

/** ========================================================================== **/

if(strcmp(Command, "kick") == 0)
{
    if(CmdAccess->GetOptionInt("kick") > Client->GetAccount()->GetLevel()) return;

    int destCharID;
    PClient* victim;

    if(Arg1[0] == '\0')
    {
        Chat->send(Client, CHAT_DIRECT, "Usage", "@kick <charID or nickname>");
        return;
    }
    // Assuming player is using charID to kick
    destCharID = std::atoi(Arg1);

    if(destCharID == 0)
        victim = GetClientByNick(Arg1); // If we're wrong (used nickname), get PClient for victim by nickname
    else
        victim = GetClientByID(destCharID); // Now get PClient for victim over charID

    if(victim == NULL) // If victim isnt found, return error
    {
        Chat->send(Client, CHAT_DIRECT, "System", "No such player");
        return;
    }

    // Make sure only people with a higher level than victim can kick victim
    if(Client->GetAccount()->GetLevel() <= victim->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant kick %s, target level is higher or equal to yours!", Database->GetChar(victim->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(Client, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

// *************** Checks done, proceed with command
    Console->Print("%s %s (Lv %d) kicked %s (Lv %d)", Console->ColorText(YELLOW, BLACK, "[GameCommand]"), Database->GetChar(Client->GetCharID())->GetName().c_str(), Client->GetAccount()->GetLevel(), Database->GetChar(victim->GetCharID())->GetName().c_str(), victim->GetAccount()->GetLevel());
    //victim->GameDisconnect();  // causes SegFault here. Need to find a better way to disconnect player
}

if(strcmp(Command, "info") == 0)
{
    if(CmdAccess->GetOptionInt("info") > Client->GetAccount()->GetLevel()) return;

    int destCharID;
    PClient* victim;

    if(Arg1[0] == '\0')
    {
        Chat->send(Client, CHAT_DIRECT, "Usage", "@info <charID or nickname>");
        return;
    }
    destCharID = std::atoi(Arg1);

    if(destCharID == 0)
        victim = GetClientByNick(Arg1); // If we're wrong (used nickname), get PClient for victim by nickname
    else
        victim = GetClientByID(destCharID); // Now get PClient for victim over charID

    if(victim == NULL) // If victim isnt found, return error
    {
        Chat->send(Client, CHAT_DIRECT, "System", "No such player");
        return;
    }
    // *************** Checks done, proceed with command
    /*
        DIRECT> System: PlayerInformation
        DIRECT> Info: CharID     : %d     // victim->GetCharID();
        DIRECT> Info: AccountID  : %d     // victim->GetAccount()->GetID();
        DIRECT> Info: LoginName  : %s     // victim->GetAccount()->GetName();
        DIRECT> Info: AccessLevel: %d     // victim->GetAccount()->GetLevel();
        DIRECT> Info: Current Loc: %d     // Database->GetChar(Client->GetCharID())->GetLocation();
        DIRECT> Info: IP address : %s     // victim->GetAddress():

        Maybe for future addons...
        DIRECT> System: CharInformation
        DIRECT> Info: Faction      : %d     // Database->GetChar(Client->GetCharID())->GetFaction();
        DIRECT> Info: Cash         : %d     // Database->GetChar(Client->GetCharID())->GetCash();
        DIRECT> Info: Soullight    : %d     // Database->GetChar(Client->GetCharID())->GetSoullight();
    */
    char tmpInfo_head[151];
    char tmpInfo_cID[151];
    char tmpInfo_aID[151];
    char tmpInfo_Login[151];
    char tmpInfo_AxxLv[151];
    char tmpInfo_Loc[151];
    char tmpInfo_IP[151];

    snprintf(tmpInfo_head, 150,     "PlayerInformation");
    snprintf(tmpInfo_cID, 150,      "CharID     : %d",  victim->GetCharID());
    snprintf(tmpInfo_aID, 150,      "AccountID  : %d",  victim->GetAccount()->GetID());
    snprintf(tmpInfo_Login, 150,    "LoginName  : %s",  victim->GetAccount()->GetName().c_str());
    snprintf(tmpInfo_AxxLv, 150,    "AccessLevel: %d",  victim->GetAccount()->GetLevel());
    snprintf(tmpInfo_Loc, 150,      "Current Loc: %d",  Database->GetChar(Client->GetCharID())->GetLocation());
    snprintf(tmpInfo_IP, 150,       "IP address : %s",  victim->GetAddress());

    tmpInfo_head[150] = '\0';
    tmpInfo_cID[150] = '\0';
    tmpInfo_aID[150] = '\0';
    tmpInfo_Login[150] = '\0';
    tmpInfo_AxxLv[150] = '\0';
    tmpInfo_Loc[150] = '\0';
    tmpInfo_IP[150] = '\0';

    Chat->send(Client, CHAT_DIRECT, "System", tmpInfo_head);
    Chat->send(Client, CHAT_DIRECT, "Info", tmpInfo_cID);
    Chat->send(Client, CHAT_DIRECT, "Info", tmpInfo_aID);
    Chat->send(Client, CHAT_DIRECT, "Info", tmpInfo_Login);
    Chat->send(Client, CHAT_DIRECT, "Info", tmpInfo_AxxLv);
    Chat->send(Client, CHAT_DIRECT, "Info", tmpInfo_Loc);
    Chat->send(Client, CHAT_DIRECT, "Info", tmpInfo_IP);
}

if(strcmp(Command, "setlevel") == 0)
{
    if(CmdAccess->GetOptionInt("setlevel") > Client->GetAccount()->GetLevel()) return;

    int destCharID, destLevel;
    PClient* victim;

    if(Arg1[0] == '\0' || Arg2[0] == '\0')
    {
        Chat->send(Client, CHAT_DIRECT, "Usage", "@setlevel <charID or nickname> <newlevel 1-99>");
        return;
    }
    destCharID = std::atoi(Arg1);
    destLevel = std::atoi(Arg2);

    if(destCharID == 0)
        victim = GetClientByNick(Arg1); // If we're wrong (used nickname), get PClient for victim by nickname
    else
        victim = GetClientByID(destCharID); // Now get PClient for victim over charID

    if(victim == NULL) // If victim isnt found, return error
    {
        Chat->send(Client, CHAT_DIRECT, "System", "No such player");
        return;
    }

    // Check accountlevel (Only higher's can perform commands on lower's)
    if(Client->GetAccount()->GetLevel() <= victim->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant set level for %s, target level is higher or equal to yours!", Database->GetChar(victim->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(Client, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

// *************** Checks done, proceed with command
    victim->GetAccount()->SetLevel(destLevel);
    char tmpMsg[60], tmpMsg2[60];

    snprintf(tmpMsg, 59, "Set level for player %s to %d", Database->GetChar(victim->GetCharID())->GetName().c_str(), destLevel);
    snprintf(tmpMsg2, 59, "**POOF** Your new AccessLevel is now %d", destLevel);

    tmpMsg[59] = '\0';
    tmpMsg2[59] = '\0';

    Chat->send(Client, CHAT_DIRECT, "System", tmpMsg);
    Chat->send(victim, CHAT_DIRECT, "System", tmpMsg2);
}

if(strcmp(Command, "warpto") == 0)
{
    if(CmdAccess->GetOptionInt("warpto") > Client->GetAccount()->GetLevel()) return;

    int destCharID, destZone;
    PClient* victim;

    if(Arg1[0] == '\0')
    {
        Chat->send(Client, CHAT_DIRECT, "Usage", "@warpto <charID or nickname>");
        return;
    }
    destCharID = std::atoi(Arg1);

    if(destCharID == 0)
        victim = GetClientByNick(Arg1); // If we're wrong (used nickname), get PClient for victim by nickname
    else
        victim = GetClientByID(destCharID); // Now get PClient for victim over charID

    if(victim == NULL) // If victim isnt found, return error
    {
        Chat->send(Client, CHAT_DIRECT, "System", "No such player");
        return;
    }

    // Warp GM/Admin to target player
    destZone = Database->GetChar(victim->GetCharID())->GetLocation();
    if (!Worlds->IsValidWorld(destZone))
    {
        Console->Print("%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText(RED, BLACK, "[PANIC]"), destZone);
        return;
    }
// *************** Checks done, proceed with command
    if (Client->ChangeCharLocation(destZone))
    {
        // World changed, now place GM/Admin to position of target player "victim"
        PMessage* tmpMsg;

        // Get position of target player "victim"
        u16 nNewX, nNewY, nNewZ;
        nNewX = victim->GetChar()->Coords.mX;
        nNewY = victim->GetChar()->Coords.mY;
        nNewZ = victim->GetChar()->Coords.mZ;

        // And now built new message for GM/Admin, with new X/Y/Z Coords
        tmpMsg = MsgBuilder->BuildCharPosMoveMsg(Client, nNewX, nNewY, nNewZ);

        // Finish packet (Add UDP_ID stuff etc)
        Client->IncreaseUDP_ID();
        tmpMsg->U16Data(0x01) = Client->GetUDP_ID();
        tmpMsg->U16Data(0x03) = Client->GetSessionID();
        tmpMsg->U16Data(0x07) = Client->GetUDP_ID();

        // Send packet
        Client->getUDPConn()->SendMessage(tmpMsg);
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), Client->GetCharID(), destZone);
    }
}

if(strcmp(Command, "recall") == 0)
{
    if(CmdAccess->GetOptionInt("recall") > Client->GetAccount()->GetLevel()) return;

    int destCharID, destZone;
    PClient* victim;

    if(Arg1[0] == '\0')
    {
      Chat->send(Client, CHAT_DIRECT, "Usage", "@recall <charID or nickname>");
      return;
    }
    destCharID = std::atoi(Arg1);

    if(destCharID == 0)
        victim = GetClientByNick(Arg1); // If we're wrong (used nickname), get PClient for victim by nickname
    else
        victim = GetClientByID(destCharID); // Now get PClient for victim over charID

    if(victim == NULL) // If victim isnt found, return error
    {
        Chat->send(Client, CHAT_DIRECT, "System", "No such player");
        return;
    }

    // Check accountlevel (Only higher's can perform commands on lower's)
    if(Client->GetAccount()->GetLevel() <= victim->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant warp %s to you, target level is higher or equal to yours!", Database->GetChar(victim->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(Client, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    // Warp target player to GM/Admin
    destZone = Database->GetChar(Client->GetCharID())->GetLocation();
    if (!Worlds->IsValidWorld(destZone))
    {
        Console->Print("%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText(RED, BLACK, "[PANIC]"), destZone);
        return;
    }
// *************** Checks done, proceed with command
    if (victim->ChangeCharLocation(destZone))
    {
        // World changed, now place victim to position of GM/Admin
        PMessage* tmpMsg;

        // Get position of GM/Admin
        u16 nNewX, nNewY, nNewZ;
        nNewX = Client->GetChar()->Coords.mX;
        nNewY = Client->GetChar()->Coords.mY;
        nNewZ = Client->GetChar()->Coords.mZ;

        // And now built new message for Victim, with new X/Y/Z Coords
        tmpMsg = MsgBuilder->BuildCharPosMoveMsg(victim, nNewX, nNewY, nNewZ);

        // Finish packet (Add UDP_ID stuff etc)
        victim->IncreaseUDP_ID();
        tmpMsg->U16Data(0x01) = victim->GetUDP_ID();
        tmpMsg->U16Data(0x03) = victim->GetSessionID();
        tmpMsg->U16Data(0x07) = victim->GetUDP_ID();

        // Send packet
        victim->getUDPConn()->SendMessage(tmpMsg);
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), Client->GetCharID(), destZone);
    }
}

/****************************/
if(strcmp(Command, "broadcast") == 0)
{
    if(CmdAccess->GetOptionInt("broadcast") > Client->GetAccount()->GetLevel()) return;
    int i = 0, j = 11;
    char tmpStr[255];

    do
    {
        tmpStr[i] = ChatText[j];
        i++;
        j++;
    } while (ChatText[j] != '\0');
    tmpStr[i] = '\0';

    Chat->sendBroadcast(tmpStr);
}

/******* temp tests *******/
if(strcmp(Command, "t") == 0) // testing apprence status
  {
    if(Client->GetAccount()->GetLevel() < PAL_ADMIN)
        return;

    u32 val1;
    u8 val2;
    char tmpStr[128];
    static PMessage* tmpMsg = NULL;

    if (!tmpMsg)
      tmpMsg = MsgBuilder->BuildCharHelloMsg(Client);

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
      tmpMsg = MsgBuilder->BuildCharHelloMsg(Client);
      snprintf(tmpStr, 127, "Data reset to normal values");
    }

    tmpStr[127] = '\0';
    Chat->send(Client, CHAT_DIRECT, "System", tmpStr);

    PMessage* SendMsg = new PMessage(tmpMsg->GetMaxSize());
    (*SendMsg) = (*tmpMsg);
    ClientManager->UDPBroadcast(SendMsg, Client);
  }

if(strcmp(Command, "h") == 0) // testing apprence status
  {
    if(Client->GetAccount()->GetLevel() < PAL_ADMIN)
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
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)Client->GetLocalID();
	*tmpMsg << (u8)0x30;
	*tmpMsg << (u8)val1; //Head Heath (% ?)
	*tmpMsg << (u8)val1; //Body Heath
	*tmpMsg << (u8)val1; //Feet Heath
	*tmpMsg << (u8)0x01;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
   ClientManager->UDPBroadcast(tmpMsg, Client);

  snprintf(tmpStr, 127, "Data set to values 0x%02x 0x%02x", val1, val2);
  tmpStr[127] = '\0';
  Chat->send(Client, CHAT_DIRECT, "System", tmpStr);
  }

 if(strcmp(Command, "v") == 0) // testing actions
  {
    if(Client->GetAccount()->GetLevel() < PAL_ADMIN)
        return;

    char tmpStr[128];
    int testmode = 0; // change here only for u8/u16/u32 testvalue use

    if(Arg1[0] != '\0')
    {
      switch(testmode)
      {
        case 0: Client->testval8 = (u8)(atoi(Arg1) & 0xff); break;
        case 1: Client->testval16 = (u16)(atoi(Arg1) & 0xffff); break;
        case 2: Client->testval32 = (u32)(atoi(Arg1) & 0xffffffff); break;
      }
    }
    else
    {
      Client->testval8 = 0;
      Client->testval16 = 0;
      Client->testval32 = 0;
    }

    switch(testmode)
    {
      case 0: snprintf(tmpStr, 127, "Test value u8 set to 0x%02x (%d)", Client->testval8, Client->testval8); break;
      case 1: snprintf(tmpStr, 127, "Test value u16 set to 0x%04x (%d)", Client->testval16, Client->testval16); break;
      case 2: snprintf(tmpStr, 127, "Test value u32 set to 0x%08x (%d)", Client->testval32, Client->testval32); break;
      default: tmpStr[0] = '\0';
    }
    tmpStr[127] = '\0';
    Chat->send(Client, CHAT_DIRECT, "System", tmpStr);
  }
/******* end temp *******/

// -------------------------------------------------------
}
/// ****************************** ///
///         FUNCTIONS PART         ///
/// ****************************** ///
PClient* GetClientByID(int charid)
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

PClient* GetClientByNick(const char *nick)
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

bool SendRawFile(PClient *Client, char *FileName, int protocoll) {
       ConnectionTCP *Socket = Client->getTCPConn();
       ConnectionUDP *UDPSocket = Client->getUDPConn();

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
               Client->SetUDP_ID(Client->GetUDP_ID()+1);
               *(u16*)&buffer[1] = Client->GetUDP_ID();
               *(u16*)&buffer[3] = Client->GetSessionID();
               *(u16*)&buffer[7] = Client->GetUDP_ID();

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

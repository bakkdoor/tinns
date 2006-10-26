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

	ToDo:
	- Fix Subwaysyncy
	- Fix Wastelandsyncy
*/

#include "main.h"

#include "client.h"
#include "msgbuilder.h"

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
       int zoneID = std::atoi(Arg1);
       int SpawnPointID = std::atoi(Arg2);

       if(zoneID == 0) {
           Chat->send(Client, CHAT_DIRECT, "System", "Syntax error. @warp <zoneid> [<spawn location>]");
           return;
       }

       const PDefWorld *def = GameDefs->GetWorldDef(zoneID);
       if(!def)
       {
           Chat->send(Client, CHAT_DIRECT, "System", "Invalid zoneID");
           return;
       }

       //PChar *Char = Database->GetChar(Client->GetCharID());

	   /* u8 ZonePacket[] = {0x13, 0xc3, 0x00, 0x63, 0xf8,
        0x0f, 0x03, 0xc3, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x04, 0x00, 0xda,
        0xcf, 0x03, 0x00, 0x01, 0x00, 0x15, 0x1b, 0x22, 0x01, 0x00, 0x00,
        0x1f, 0x49, 0x82, 0x81, 0x81, 0xe5, 0x6b, 0x04, 0xd5, 0x76, 0x01,
        0x00, 0x00, 0x00, 0x11, 0x11}; */
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
       // ZonePacket[1] = (char)Test; // from NeoX
       Console->Print("IngameCommand: Warping player %d to zone %d (%s)", Client->GetCharID(), zoneID, def->GetName().c_str());

       Client->ChangeCharLocation(zoneID);
       Client->getUDPConn()->write(ZonePacket, sizeof(ZonePacket));	
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
       Console->Print("IngameCommand: Sending connected-player-list to client %s", Arg1);
       Chat->sendConnectedList(Client, false);
   }
// -------------------------------------------------------
   if(strcmp(Command, "sendchat") == 0) {
//       SendChat(Client, Channel, NickName, TextToSend);
Console->Print("SendChat");
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

/******* temp tests *******/
if(strcmp(Command, "t") == 0) // testing apprence status
  {
    
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
/******* end temp *******/    

// -------------------------------------------------------   
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

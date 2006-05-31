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

	ToDo:
	- Fix Subwaysyncy
	- Fix Wastelandsyncy
*/

#include "main.h"

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
       PAccount *Account = Client->GetAccount();


       if(Account->IsAdminDebug() == true)
       {
           Account->SetAdminDebug(false);
           Chat->send(Client, CHAT_DIRECT, "System", "Debug mode is now DISABLED");
       }
       else
       {
           Account->SetAdminDebug(true);
           Chat->send(Client, CHAT_DIRECT, "System", "Debug mode is now ENABLED");
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
       int zoneID = 0;
       zoneID = std::atoi(Arg1);

       if(zoneID == 0) {
           Chat->send(Client, CHAT_DIRECT, "System", "Syntax error. @warp <zoneid>");
           return;
       }

       const PDefWorld *def = GameDefs->GetWorldDef(zoneID);
       if(!def)
       {
           Chat->send(Client, CHAT_DIRECT, "System", "Invalid zoneID");
           return;
       }

       PChar *Char = Database->GetChar(Client->GetCharID());

	   u8 ZonePacket[] = {0x13, 0xc3, 0x00, 0x63, 0xf8,
        0x0f, 0x03, 0xc3, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x04, 0x00, 0xda,
        0xcf, 0x03, 0x00, 0x01, 0x00, 0x15, 0x1b, 0x22, 0x01, 0x00, 0x00,
        0x1f, 0x49, 0x82, 0x81, 0x81, 0xe5, 0x6b, 0x04, 0xd5, 0x76, 0x01,
        0x00, 0x00, 0x00, 0x11, 0x11};

       //Client->IncreaseUDP_ID();
       Client->SetUDP_ID(Client->GetUDP_ID()+1);
       *(u16*)&ZonePacket[1] = Client->GetUDP_ID();
       *(u16*)&ZonePacket[3] = Client->GetSessionID();
       *(u16*)&ZonePacket[7] = Client->GetUDP_ID();
       *(u32*)&ZonePacket[15] = zoneID;

       Console->Print("IngameCommand: Warping player %d to zone %d (%s)", Client->GetCharID(), zoneID, def->GetName().c_str());

       Char->SetLocation(zoneID);
       Client->getUDPConn()->write(ZonePacket, sizeof(ZonePacket));
   }
// -------------------------------------------------------
   if(strcmp(Command, "rawf") == 0) {
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

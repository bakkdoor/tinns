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
        chat.h

        Authors:
        - Namikon
        - bakkdoor

        MODIFIED: 30 Nov 2005 Namikon/Akiko
        REASON: - initial release by Namikon

        MODIFIED: 11 Dec 2005 Namikon
        REASON: - Added function SendDChat(Client, <text>, <nick>) to send a Direct to player
                - Added Channel IDs for GameMaster (/gm) and Administrator (/admin) chat
                - Added channel const's for sending chat packets
                - Added function SendChat(Client, <channel>, <text>, <nick>) to send chat messages
		  For <channel>, use one of the u8 constants listet below

        MODIFIED: 17 Dec 2005 bakkdoor
        REASON: - introduced new structure for chatsystem
                - -> PChat class
*/

#ifndef CHAT_H
#define CHAT_H
#define LOCALCHAT_MAXDISTANCE 1433

class PChat
{
    private:

    public:
            PChat();
            ~PChat();

            // INCOMING CHAT
            bool HandleGameChat(PClient *Client, const u8 *Packet);
            bool cmpr(const u8 *Array1, const u8 *Array2);

            // this function is called by the other more specific functions and simply sends the data to the receiver-client
            bool send(PClient* receiver, const u8* Channel, const char* AuthorNickName, char* text, bool debugOut=false);

            // this is for debugging and sends the current connected playerlist to the receiver-client
            void sendConnectedList(PClient* receiver, bool debugOut=false);

            // specific channel functions:
            void sendBuddy(PClient* author, char* text, bool debugOut=false);
            void sendLocal(PClient* author, char* text, bool debugOut=false);
            void sendClan(PClient* author, char* text, bool debugOut=false);
            void sendTeam(PClient* author, char* text, bool debugOut=false);
            void sendDirect(PClient* author, PClient* receiver, char* text, bool debugOut=false);
            void sendZone(PClient* author, char* text, bool debugOut=false);
            void sendFrak(PClient* author, char* text, bool debugOut=false);
            void sendTradeCS(PClient* author, char* text, bool debugOut=false);
            void sendTradeMB(PClient* author, char* text, bool debugOut=false);
            void sendTradeNC(PClient* author, char* text, bool debugOut=false);
            void sendTradeTH(PClient* author, char* text, bool debugOut=false);
            void sendTradeWL(PClient* author, char* text, bool debugOut=false);
            void sendOOC(PClient* author, char* text, bool debugOut=false);
            void sendHelp(PClient* author, char* text, bool debugOut=false);
            void sendClanSearch(PClient* author, char* text, bool debugOut=false);
            void sendServicesCS(PClient* author, char* text, bool debugOut=false);
            void sendServicesMB(PClient* author, char* text, bool debugOut=false);
            void sendServicesNC(PClient* author, char* text, bool debugOut=false);
            void sendServicesTH(PClient* author, char* text, bool debugOut=false);
            void sendServicesWL(PClient* author, char* text, bool debugOut=false);
            void sendTeam10(PClient* author, char* text, bool debugOut=false);
            void sendTeam30(PClient* author, char* text, bool debugOut=false);
            void sendTeam50(PClient* author, char* text, bool debugOut=false);
            void sendTeam70(PClient* author, char* text, bool debugOut=false);
            void sendAdmin(PClient* author, char* text, bool debugOut=false);
            void sendGM(PClient* author, char* text, bool debugOut=false);
            void sendGMAdmin(PClient* author, char* text, bool debugOut=false);

            void sendBroadcast(char* text, bool debugOut=false);
            void sendOOCBroadcast(char* text, bool debugOut=false);
            void sendPlayerDirect(PClient* author, char* text, u32 destination, bool debugOut=false);
            void sendLocalchat(PClient* receiver, PClient* author, char* text, bool debugOut=false);
            bool chanEnabled(PClient* Client, u32 channel);

};

/** A WARNING: DO >N O T< (!!!) CHANGE >ANY< OF THE FOLLOWING VARIABLES UNLESS YOU KNOW EXACT WHAT YOU'RE DOING! **/
/** You can easily mess up the entire chat subsystem. If you're unsure, ask Namikon first! **/
/*
####################
   CHANNEL-CODES:
     (INCOMING)
####################
*/
static const u32 CHANNEL_BUDDY = 0x00000000;                // Buddy
static const u32 CHANNEL_CLAN = 0x00000002;                 // Clan
static const u32 CHANNEL_TEAM = 0x00000003;                 // Team
static const u32 CHANNEL_DIRECT = 0xFFFFFFFF;               // Direct UNKNOWN YET
static const u32 CHANNEL_CUS_ZONE = 0x00000105;             // Custom -> Zone
static const u32 CHANNEL_CUS_FRAKTION = 0x00000205;         // Custom -> Fraktion
static const u32 CHANNEL_CUS_TRADE_CANYON = 0x00002005;     // Custom -> Trade Canyon
static const u32 CHANNEL_CUS_TRADE_MB = 0x00000805;         // Custom -> Trade MB
static const u32 CHANNEL_CUS_TRADE_NC = 0x00000405;         // Custom -> Trade NC
static const u32 CHANNEL_CUS_TRADE_TH = 0x00001005;         // Custom -> Trade TH
static const u32 CHANNEL_CUS_TRADE_WASTE = 0x00004005;      // Custom -> Trade Wastelands
static const u32 CHANNEL_CUS_OOC = 0x04000005;              // Custom -> OOC
static const u32 CHANNEL_CUS_PLAYERHELP = 0x02000005;       // Custom -> Player 2 Player help
static const u32 CHANNEL_CUS_CLANSEARCH = 0x01000005;       // Custom -> Searching Clan
static const u32 CHANNEL_CUS_SERVICES_CANYON = 0x00040005;  // Custom -> Runner Services Canyon
static const u32 CHANNEL_CUS_SERVICES_MB = 0x00010005;      // Custom -> Runner Services MB
static const u32 CHANNEL_CUS_SERVICES_NC = 0x00008005;      // Custom -> Runner Services NC
static const u32 CHANNEL_CUS_SERVICES_TH = 0x00020005;      // Custom -> Runner Services TH
static const u32 CHANNEL_CUS_SERVICES_WASTE = 0x00080005;   // Custom -> Runner Services Wastelands
static const u32 CHANNEL_CUS_TEAM_10 = 0x00100005;          // Custom -> Searching Team ~10
static const u32 CHANNEL_CUS_TEAM_30 = 0x00200005;          // Custom -> Searching Team ~30
static const u32 CHANNEL_CUS_TEAM_50 = 0x00400005;          // Custom -> Searching Team ~50
static const u32 CHANNEL_CUS_TEAM_70 = 0x00800005;          // Custom -> Searching Team ~70
static const u32 CHANNEL_ADMIN = 0x000000FF;                // Admin chat
static const u32 CHANNEL_GMADMIN = 0x000000FE;                // Admin chat
static const u32 CHANNEL_GMCHAT = 0x000000FD;               // GameMaster chat
/*
####################
   CHANNEL-CODES:
     (OUTGOING)
####################
*/
static const u8 CHAT_BUDDY[] = {0x00, 0x10};
static const u8 CHAT_LOCAL[] =  {0x01, 0x10};
static const u8 CHAT_CLAN[] = {0x02, 0x10};
static const u8 CHAT_TEAM[] = {0x03, 0x10};
static const u8 CHAT_DIRECT[] = {0x04, 0x10};
static const u8 CHAT_ZONE[] = {0x05, 0x00};
static const u8 CHAT_FRAK[] = {0x05, 0x01};
static const u8 CHAT_TRADECS[] = {0x05, 0x05};
static const u8 CHAT_TRADEMB[] = {0x05, 0x03};
static const u8 CHAT_TRADENC[] = {0x05, 0x02};
static const u8 CHAT_TRADETH[] = {0x05, 0x04};
static const u8 CHAT_TRADEWL[] = {0x05, 0x06};
static const u8 CHAT_OOC[] = {0x05, 0x12};
static const u8 CHAT_HELP[] = {0x05, 0x11};
static const u8 CHAT_CLANSEARCH[] = {0x05, 0x10};
static const u8 CHAT_SERVICECS[] =  {0x05, 0x0A};
static const u8 CHAT_SERVICESMB[] = {0x05, 0x08};
static const u8 CHAT_SERVICESNC[] = {0x05, 0x07};
static const u8 CHAT_SERVICESTH[] = {0x05, 0x09};
static const u8 CHAT_SERVICESWL[] = {0x05, 0x0B};
static const u8 CHAT_TEAM10[] = {0x05, 0x0C};
static const u8 CHAT_TEAM30[] = {0x05, 0x0D};
static const u8 CHAT_TEAM50[] = {0x05, 0x0E};
static const u8 CHAT_TEAM70[] = {0x05, 0x0F};
static const u8 CHAT_ADMIN[] = {0xFF, 0x10};
static const u8 CHAT_GMADMIN[] = {0xFE, 0x10};
static const u8 CHAT_GM[] = {0xFD, 0x10};

/*
####################
   CHANNEL-CODES:
(EN/DISABLE-ABLE CHANNELS)
####################
*/
static const u32 C_ZONE = 1;
static const u32 C_FRAK = 2;
static const u32 C_TRADENC = 4;
static const u32 C_TRADEMB = 8;
static const u32 C_TRADETH = 16;
static const u32 C_TRADECS = 32;
static const u32 C_TRADEWL = 64;
static const u32 C_SERVICENC = 128;
static const u32 C_SERVICEMB = 256;
static const u32 C_SERVICETH = 512;
static const u32 C_SERVICECS = 1024;
static const u32 C_SERVICEWL = 2048;
static const u32 C_TEAM10 = 4096;
static const u32 C_TEAM30 = 8192;
static const u32 C_TEAM50 = 16384;
static const u32 C_TEAM70 = 32768;
static const u32 C_CLANSEARCH = 65536;
static const u32 C_HELP = 131072;
static const u32 C_OOC = 262144;
#endif

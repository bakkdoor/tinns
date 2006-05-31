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
	Zoning - makes changing of maps possible ingame

	Authors:
	- initial release by unknown person
	- Sting <>
	- Akiko <akiko@gmx.org>
	- Namikon

	MODIFIED: 23 August 2005 by Sting
        REASON: - TO match up with defs/pak_worldinfo.def from patch 200

	MODIFIED: 27 Aug 2005 by Akiko
	REASON: - reformated for better reading
		- thinking about a way to optimize the case statements
	MODIFIED: 28 Aug 2005 by Akiko
	REASON: - continued reformating
	MODIFIED: 26 Oct 2005 Akiko
	REASON: - fixed a bug -> loc is a int, and the location is a 16 bit value
		- added GPL
	MODIFIED: 27 Nov 2005 Akiko
	REASON: - fixed a very stupid bug (loc)
	MODIFIED: 04 Dec 2005 Akiko
	REASON: - copying int to char array (loc) correctly now
	MODIFIED: 07 Jan 2006 Namikon
	REASON: - Fixed zoning to / from wastelands to / from city sectors (packet1 was wrong, thanks Maxx!!)
	MODIFIED: 08 Jan 2006 Namikon
	REASON: - Added appartment handling
            - Fixed minimap
	MODIFIED: 17 Jan 2006 Namikon
	REASON: - File rewritten. Now, only 1 packet is send, like the real servers (that one fixed subway)
            - Fixed several worldnames
*/

#include "main.h"

void SendZone(PClient *Client, int loc)
{
	//TODO : FIX case for worldroute and 5 missing id 505

	ConnectionTCP *Socket = Client->getTCPConn();
	PChar *Char = Database->GetChar(Client->GetCharID());

	Socket->SetTimeOutValue(0xffff);

    std::string worldName;

	int i = Char->GetLocation();

	if(i > 100000)
	{
	    switch(loc)
	    {
	        // Plaza
	        case 1:
	        {
                worldName = "apps/plaza_app_1";
                break;
	        }

	        case 2:
	        {
                worldName = "apps/plaza_app_2";
                break;
	        }

	        case 3:
	        {
                worldName = "apps/plaza_app_3";
                break;
	        }

	        // Via
	        case 20:
	        {
                worldName = "apps/vr_app_1";
                break;
	        }

	        case 21:
	        {
                worldName = "apps/vr_app_2";
                break;
	        }

	        case 22:
	        {
                worldName = "apps/vr_app_3";
                break;
	        }

	        // Pepper
	        case 40:
	        {
                worldName = "apps/pepper_app_1";
                break;
	        }

	        case 41:
	        {
                worldName = "apps/pepper_app_2";
                break;
	        }

	        case 42:
	        {
                worldName = "apps/pepper_app_3";
                break;
	        }

	        // Outzone
	        case 60:
	        {
                worldName = "apps/out_app_1";
                break;
	        }

	        // Tech Haven
	        case 80:
	        {
                worldName = "apps/tech_app_1";
                break;
	        }

	        case 81:
	        {
                worldName = "apps/tech_app_2";
                break;
	        }

	        // Military
	        case 90:
	        {
                worldName = "apps/mili_app_1";
                break;
	        }

	        // Plaza Warehouse - Epic Run Diamond
	        case 92:
	        {
                worldName = "apps/warehouse_plaza";
                break;
	        }

	        // Canyon
	        case 100:
	        {
                worldName = "apps/canyon_app_1";
                break;
	        }

	        // Faction start appartments
	        // Plaza
	        case 200:
	        {
                worldName = "apps/plaza_app_1";
                break;
	        }

	        case 201:
	        {
                worldName = "apps/plaza_app_1";
                break;
	        }

	        case 202:
	        {
                worldName = "apps/plaza_app_1";
                break;
	        }

	        // Via
	        case 205:
	        {
                worldName = "apps/vr_app_1";
                break;
	        }

	        case 206:
	        {
                worldName = "apps/vr_app_1";
                break;
	        }

	        case 207:
	        {
                worldName = "apps/vr_app_1";
                break;
	        }

	        // Pepper
	        case 210:
	        {
                worldName = "apps/pepper_app_1";
                break;
	        }

	        case 211:
	        {
                worldName = "apps/pepper_app_1";
                break;
	        }

	        // Outzone
	        case 212:
	        {
                worldName = "apps/out_app_1";
                break;
	        }

	        // Clanappartments
	        // Plaza
	        case 500:
	        {
                worldName = "apps/plaza_clan_1";
                break;
	        }

	        case 501:
	        {
                worldName = "apps/plaza_clan_2";
                break;
	        }

            // Pepper
	        case 510:
	        {
                worldName = "apps/pepper_clan_1";
                break;
	        }

            // Tech Haven
	        case 550:
	        {
                worldName = "apps/tech_clan_1";
                break;
	        }

            // Canyon
	        case 570:
	        {
                worldName = "apps/canyon_clan_1";
                break;
	        }

            // Outzone
	        case 580:
	        {
                worldName = "apps/out_clan_1";
                break;
	        }

            // Military Base
	        case 590:
	        {
                worldName = "apps/mili_clan_1";
                break;
	        }
	    }
	}
	else
	{
        switch(loc)
        {
            //Subway - this seems to change your location, but then goes wrong?
            case 1000:
            {
                worldName = "subway";
                break;
            }

            //PLAZA
            case 1:
            {
                worldName = "plaza/plaza_p1";
                break;
            }
            case 2:
            {
                worldName = "plaza/plaza_p2";
                break;
            }
            case 101:
            {
                worldName = "plaza/plaza_p3";
                break;
            }
            case 102:
            {
                worldName = "plaza/plaza_p4";
                break;
            }
            case 104:
            {
                worldName = "plaza/plaza_concenter";
                break;
            }
            case 105:
            {
                worldName = "plaza/plaza_cityadmin";
                break;
            }

            //DiamondRealEstate HQ
            case 107:
            {
                worldName = "plaza/plaza_fraction1";
                break;
            }

            //NeXT HQ
            case 108:
            {
                worldName = "plaza/plaza_fraction2";
                break;
            }
            case 109:
            {
                worldName = "plaza/plaza_Jobcenter";
                break;
            }

            //VIAROSSO
            case 3:
            {
                worldName = "viarosso/viarosso_p1";
                break;
            }
            case 4:
            {
                worldName = "viarosso/viarosso_p2";
                break;
            }
            case 401:
            {
                worldName = "viarosso/viarosso_p3";
                break;
            }
            case 402:
            {
                worldName = "viarosso/viarosso_lab";
                break;
            }
            case 403:
            {
                worldName = "viarosso/viarosso_bank";
                break;
            }
            case 404:
            {
                worldName = "viarosso/viarosso_ncpd";
                break;
            }

            //Tangent HQ
            case 405:
            {
                worldName = "viarosso/viarosso_fraction1";
                break;
            }

            //BioTech HQ
            case 406:
            {
                worldName = "viarosso/viarosso_fraction2";
                break;
            }

            //ProtoPharm HQ
            case 407:
            {
                worldName = "viarosso/viarosso_fraction3";
                break;
            }

            //PEPPER PARK
            case 5:
            {
                worldName = "pepper/pepper_p1";
                break;
            }
            case 6:
            {
                worldName = "pepper/pepper_p2";
                break;
            }
            case 7:
            {
                worldName = "pepper/pepper_p3";
                break;
            }
            case 601:
            {
                worldName = "pepper/pepper_maze";
                break;
            }

            //Club Veronique
            case 602:
            {
                worldName = "pepper/pepper_redlight";
                break;
            }

            //Bump Asylum
            case 603:
            {
                worldName = "pepper/pepper_guerilla";
                break;
            }
            case 604:
            {
                worldName = "pepper/pepper_church";
                break;
            }

            //Pussy Club
            case 605:
            {
                worldName = "pepper/pepper_disco";
                break;
            }

            //Secret Black Dragon
            case 606:
            {
                worldName = "pepper/pepper_mafia3";
                break;
            }

            //Secret Passage
            case 607:
            {
                worldName = "pepper/pepper_sway";
                break;
            }
            case 608:
            {
                worldName = "pepper/pepper_holo1";
                break;
            }
            case 609:
            {
                worldName = "pepper/pepper_shooting1";
                break;
            }
            case 610:
            {
                worldName = "pepper/pepper_pub";
                break;
            }

            //Black Dragon HQ
            case 611:
            {
                worldName = "pepper/pepper_mafia2";
                break;
            }

            //Twister
            case 612:
            {
                worldName = "pepper/pepper_peep";
                break;
            }

            //Tsunami Syndicate HQ
            case 613:
            {
                worldName = "pepper/pepper_mafia1";
                break;
            }

            //Secret Passage 2
            case 614:
            {
                worldName = "pepper/pepper_sway2";
                break;
            }
            case 615:
            {
                worldName = "pepper/pepper_holo2";
                break;
            }

            //INDUSTRIAL AREA
            case 8:
            {
                worldName = "industry/industry_p1";
                break;
            }
            case 801:
            {
                worldName = "industry/industry_p2";
                break;
            }

            //OUTZONE
            case 9:
            {
                worldName = "outzone/outzone_p1";
                break;
            }
            case 10:
            {
                worldName = "outzone/lostpart_p1";
                break;
            }
            case 11:
            {
                worldName = "outzone/lostpart_p2";
                break;
            }
            case 12:
            {
                worldName = "outzone/lostpart_p3";
                break;
            }
            case 13:
            {
                worldName = "outzone/lostpart_p4";
                break;
            }
            case 20:
            {
                worldName = "outzone/rooftops";
                break;
            }
            case 21:
            {
                worldName = "outzone/outzone_powerplant";
                break;
            }
            case 22:
            {
                worldName = "outzone/outzone_jail";
                break;
            }
            case 23:
            {
                worldName = "outzone/outzone_abbey";
                break;
            }
            case 24:
            {
                worldName = "outzone/outzone_station";
                break;
            }

            //TUTORIAL
            case 40:
            {
                worldName = "tutorial/tutorial_p1";
                break;
            }
            case 41:
            {
                worldName = "tutorial/tutorial_p2";
                break;
            }
            case 42:
            {
                worldName = "tutorial/tutorial_p3";
                break;
            }
            case 43:
            {
                worldName = "tutorial/tutorial_p4";
                break;
            }
            case 44:
            {
                worldName = "tutorial/tutorial_p5";
                break;
            }
            case 45:
            {
                worldName = "tutorial/tutorial_p6";
                break;
            }
            case 46:
            {
                worldName = "outzone/lostpart_p1_tut";
                break;
            }

            //MINING FACILITY
            case 700:
            {
                worldName = "mining/mining1";
                break;
            }
            case 701:
            {
                worldName = "mining/mining2";
                break;
            }
            case 704:
            {
                worldName = "mining/mining3";
                break;
            }

            //TECHTOWN
            case 500:
            {
                worldName = "techtown/techtown_p1";
                break;
            }
            case 502:
            {
                worldName = "techtown/techtown_mine1";
                break;
            }
            case 503:
            {
                worldName = "techtown/techtown_p2";
                break;
            }
            case 504:
            {
                worldName = "techtown/techtown_p3";
                break;
            }

            //DARKMETAL
            case 550:
            {
                worldName = "Darkmetal/Darkmetal_p1";
                break;
            }
            case 551:
            {
                worldName = "Darkmetal/Darkmetal_p2";
                break;
            }

            //CANYON
            case 650:
            {
                worldName = "canyon/canyon_p1";
                break;
            }

            //MILITARY BASE
            case 750:
            {
                worldName = "military/military_p1";
                break;
            }
            case 751:
            {
                worldName = "military/military_p2";
                break;
            }
            case 752:
            {
                worldName = "military/military_p3";
                break;
            }
            case 753:
            {
                worldName = "military/military_pstairs";
                break;
            }

            //Wastleand "Lava" Caves
            case 853:
            {
                worldName = "dungeons/caves_p1";
                break;
            }
            case 854:
            {
                worldName = "dungeons/caves_p2";
                break;
            }
            case 855:
            {
                worldName = "dungeons/caves_p3";
                break;
            }

            //Main Sewers
            case 900:
            {
                worldName = "sewer/sewer_p1";
                break;
            }
            case 901:
            {
                worldName = "sewer/sewer_p2";
                break;
            }
            case 902:
            {
                worldName = "sewer/sewer_p3";
                break;
            }
            case 903:
            {
                worldName = "sewer/sewer_p4";
                break;
            }

            case 904:
            {
                worldName = "sewer/pipeway_p1";
                break;
            }
            case 905:
            {
                worldName = "sewer/pipeway_p2";
                break;
            }
            case 906:
            {
                worldName = "sewer/pipeway_p3";
                break;
            }
            case 907:
            {
                worldName = "sewer/pipeway_p4";
                break;
            }

            //City Sewers
            //Plaza Level 1
            case 920:
            {
                worldName = "citysewer/plazasewer_1a";
                break;
            }
            case 921:
            {
                worldName = "citysewer/plazasewer_2a";
                break;
            }
            case 922:
            {
                worldName = "citysewer/plazasewer_3a";
                break;
            }
            case 923:
            {
                worldName = "citysewer/plazasewer_4b";
                break;
            }
            case 924:
            {
                worldName = "citysewer/plazasewer_2b";
                break;
            }
            case 925:
            {
                worldName = "citysewer/plazasewer_3b";
                break;
            }
            case 926:
            {
                worldName = "citysewer/plazasewer_1c";
                break;
            }
            case 927:
            {
                worldName = "citysewer/plazasewer_4c";
                break;
            }
            case 928:
            {
                worldName = "citysewer/plazasewer_3c";
                break;
            }
            case 929:
            {
                worldName = "citysewer/plazasewer_4d";
                break;
            }
            case 930:
            {
                worldName = "citysewer/plazasewer_2d";
                break;
            }
            case 931:
            {
                worldName = "citysewer/plazasewer_3d";
                break;
            }
            case 940:
            {
                worldName = "citysewer/peppersewer_1a";
                break;
            }
            case 941:
            {
                worldName = "citysewer/peppersewer_2a";
                break;
            }
            case 942:
            {
                worldName = "citysewer/peppersewer_3a";
                break;
            }
            case 943:
            {
                worldName = "citysewer/peppersewer_1b";
                break;
            }
            case 944:
            {
                worldName = "citysewer/peppersewer_2b";
                break;
            }
            case 945:
            {
                worldName = "citysewer/peppersewer_3a2";
                break;
            }
            case 946:
            {
                worldName = "citysewer/peppersewer_1c";
                break;
            }
            case 947:
            {
                worldName = "citysewer/peppersewer_3c";
                break;
            }
            case 950:
            {
                worldName = "citysewer/peppersewer_1a";
                break;
            }
            case 951:
            {
                worldName = "citysewer/peppersewer_3b";
                break;
            }
            case 952:
            {
                worldName = "citysewer/peppersewer_3b";
                break;
            }
            case 953:
            {
                worldName = "citysewer/peppersewer_3c";
                break;
            }
            case 954:
            {
                worldName = "citysewer/peppersewer_3c";
                break;
            }
            case 955:
            {
                worldName = "citysewer/peppersewer_1c";
                break;
            }
            case 956:
            {
                worldName = "citysewer/peppersewer_1c";
                break;
            }
            case 957:
            {
                worldName = "citysewer/peppersewer_1c";
                break;
            }
            case 958:
            {
                worldName = "citysewer/peppersewer_1c";
                break;
            }
            case 959:
            {
                worldName = "citysewer/peppersewer_3b";
                break;
            }
            case 960:
            {
                worldName = "citysewer/peppersewer_2b";
                break;
            }

            //Plaza Level 2
            case 970:
            {
                worldName = "citysewer/plazasewer_1a";
                break;
            }
            case 971:
            {
                worldName = "citysewer/plazasewer_2a";
                break;
            }
            case 972:
            {
                worldName = "citysewer/plazasewer_3a";
                break;
            }
            case 973:
            {
                worldName = "citysewer/plazasewer_4b";
                break;
            }
            case 974:
            {
                worldName = "citysewer/plazasewer_2b";
                break;
            }
            case 975:
            {
                worldName = "citysewer/plazasewer_3b";
                break;
            }
            case 976:
            {
                worldName = "citysewer/plazasewer_1c";
                break;
            }
            case 977:
            {
                worldName = "citysewer/plazasewer_4c";
                break;
            }
            case 978:
            {
                worldName = "citysewer/plazasewer_3c";
                break;
            }
            case 979:
            {
                worldName = "citysewer/plazasewer_4d";
                break;}
            case 980:
            {
                worldName = "citysewer/plazasewer_2d";
                break;
            }
            case 981:
            {
                worldName = "citysewer/plazasewer_3d";
                break;
            }

            //MILITARY DUNGEONS
            case 999:
            {
                worldName = "military/militarydungeon1";
                break;
            }
            case 1001:
            {
                worldName = "military/militarydungeon2";
                break;
            }
            case 1002:
            {
                worldName = "military/militarydungeon3";
                break;
            }
            case 1003:
            {
                worldName = "military/militarydungeon4";
                break;
            }
            case 1004:
            {
                worldName = "military/militarydungeon5";
                break;
            }
            case 1005:
            {
                worldName = "military/militarydungeon6";
                break;
            }

            //TECHTOWN DUNGEONS
            case 1006:
            {
                worldName = "techtown/techmine1";
                break;
            }
            case 1007:
            {
                worldName = "techtown/techmine2";
                break;
            }
            case 1008:
            {
                worldName = "techtown/techmine3";
                break;
            }
            case 1009:
            {
                worldName = "techtown/techmine4";
                break;
            }
            case 1010:
            {
                worldName = "techtown/techmine5";
                break;
            }
            case 1011:
            {
                worldName = "techtown/techmine6";
                break;
            }

            //CANYON DUNGEONS
            case 1012:
            {
                worldName = "canyon/cavemap1";
                break;
            }
            case 1013:
            {
                worldName = "canyon/cavemap2";
                break;
            }
            case 1014:
            {
                worldName = "canyon/cavemap3";
                break;
            }
            case 1015:
            {
                worldName = "canyon/cavemap4";
                break;
            }
            case 1016:
            {
                worldName = "canyon/cavemap5";
                break;
            }
            case 1017:
            {
                worldName = "canyon/cavemap6";
                break;
            }

            //SWAMP DUNGEONS
            case 1018:
            {
                worldName = "swamp/caveman1";
                break;
            }
            case 1019:
            {
                worldName = "swamp/caveman2";
                break;
            }
            case 1020:
            {
                worldName = "swamp/caveman3";
                break;
            }
            case 1021:
            {
                worldName = "swamp/caveman4";
                break;
            }
            case 1022:
            {
                worldName = "swamp/caveman5";
              break;
            }
            case 1023:
            {
                worldName = "swamp/caveman6";
                break;
            }

            //DESERT DUNGEONS
            case 1024:
            {
                worldName = "desert/wurm1";
                break;
            }
            case 1025:
            {
                worldName = "desert/wurm2";
                break;
            }
            case 1026:
            {
                worldName = "desert/wurm3";
                break;
            }
            case 1027:
            {
                worldName = "desert/wurm4";
                break;
            }
            case 1028:
            {
                worldName = "desert/wurm5";
                break;
            }
            case 1029:
            {
                worldName = "desert/wurm6";
                break;
            }

            //DESERT "CITY" DUNGEONS
            case 1030:
            {
                worldName = "desert/desertdung1";
                break;
            }
            case 1031:
            {
                worldName = "desert/desertdung3";
                break;
            }

            //CANYON DUNGEONS
            case 1032:
            {
                worldName = "canyon/cavemap1";
                break;
            }
            case 1033:
            {
                worldName = "canyon/cavemap2";
                break;
            }
            case 1034:
            {
                worldName = "canyon/cavemap3";
                break;
            }

            //WASTELAND "STORAGE" HALLS
            case 1035:
            {
                worldName = "outzone/storage1";
                break;
            }
            case 1036:
            {
                worldName = "outzone/storage2";
                break;
            }
            case 1037:
            {
                worldName = "outzone/storage3";
                break;
            }

            //OUTZONE DUNGEONS
            case 1040:
            {
                worldName = "outzone/storage1";
                break;
            }
            case 1041:
            {
                worldName = "outzone/storage2";
                break;
            }
            case 1042:
            {
                worldName = "outzone/storage3";
                break;
            }
            case 1043:
            {
                worldName = "outzone/storage4";
                break;
            }
            case 1044:
            {
                worldName = "outzone/storage5";
                break;
            }
            case 1045:
            {
                worldName = "outzone/storage6";
                break;
            }

            //GRAVE
            case 1050:
            {
                worldName = "desert/egyptmap";
                break;
            }

            //Plaza Sewer X
            case 1060:
            {
                worldName = "sewer/sewer_p4";
                break;
            }
            case 1061:
            {
                worldName = "sewer/sewer_p4";
                break;
            }
            case 1062:
            {
                worldName = "sewer/sewer_p4";
                break;
            }
            case 1063:
            {
                worldName = "sewer/sewer_p4";
                break;
            }
            case 1064:
            {
                worldName = "sewer/sewer_p4";
                break;
            }
            case 1065:
            {
                worldName = "sewer/sewer_p4";
                break;
            }

            //Pepper Sewer X
            case 1070:
            {
                worldName = "bop/underground_min";
                break;
            }
            case 1071:
            {
                worldName = "bop/underground_min";
                break;
            }
            case 1072:
            {
                worldName = "bop/underground_min";
                break;
            }
            case 1073:
            {
                worldName = "bop/underground_min";
                break;
            }
            case 1075:
            {
                worldName = "bop/underground_lab";
                break;
            }
            case 1076:
            {
                worldName = "bop/underground_lab";
                break;
            }
            case 1077:
            {
                worldName = "bop/underground_lab";
                break;
            }
            case 1078:
            {
                worldName = "bop/underground_lab";
                break;
            }

            //DOY "Dungeons" X
            //These do not appear to be included anywhere
            //SO I have only put the context - maybe maps can be found later?
            //I have put Sector Names instead of maps, as no maps were listed

            //Remember, IF you copy these maps from NC2, use them AT YOUR OWN RISK!!!
            //They have NOT been tested!


            case 1100:
            {
                worldName = "doy/PYork1";
                break;
            }
            case 1101:
            {
                worldName = "doy/YorkW07";
                break;
            }
            case 1102:
            {
                worldName = "doy/YorkW06";
                break;
            }
            case 1103:
            {
                worldName = "doy/YorkW08";
                break;
            }
            case 1104:
            {
                worldName = "doy/YorkW04";
                break;
            }
            case 1105:
            {
                worldName = "doy/pYork3";
                break;
            }
            case 1106:
            {
                worldName = "doy/pYork4";
                break;
            }
            case 1107:
            {
                worldName = "doy/PYork2";
                break;
            }
            case 1108:
            {
                worldName = "doy/PYork5";
                break;
            }
            case 1109:
            {
                worldName = "doy/YorkW09";
                break;
            }

            //Twilight Guardian HQ
            case 1110:
            {
                worldName = "doy/DOY_TG_HQ";
                break;
            }
            case 1111:
            {
                worldName = "doy/DOY_FA_HQ";
                break;
            }
            case 1112:
            {
                worldName = "doy/DOY_BC_HQ";
                break;
            }
            case 1116:
            {
                worldName = "doy/DOY_TS_HQ";
                break;
            }
            case 1119:
            {
                worldName = "doy/DOY_BD_HQ";
                break;
            }

            //Sec 1
            case 1300:
            {
                worldName = "doy/dungeons/side/gassen1";
                break;
            }
            case 1301:
            {
                worldName = "doy/dungeons/side/gassen2";
                break;
            }
            case 1302:
            {
                worldName = "doy/dungeons/sewer/sewer1";
                break;
            }
            case 1303:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 2
            case 1304:
            {
                worldName = "doy/dungeons/side/gassen1";
                break;
            }
            case 1305:
            {
                worldName = "doy/dungeons/side/gassen2";
                break;
            }
            case 1306:
            {
                worldName = "doy/dungeons/sewer/sewer2";
                break;
            }
            case 1307:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 3
            case 1308:
            {
                worldName = "doy/dungeons/side/gassen1";
                break;
            }
            case 1309:
            {
                worldName = "doy/dungeons/side/gassen2";
                break;
            }
            case 1310:
            {
                worldName = "doy/dungeons/sewer/sewer1";
                break;
            }
            case 1311:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 4
            case 1312:
            {
                worldName = "doy/dungeons/side/gassen1";
                break;
            }
            case 1313:
            {
                worldName = "doy/dungeons/sewer/sewer2";
                break;
            }
            case 1314:
            {
                worldName = "doy/dungeons/bunker/bunker1";
                break;
            }
            case 1315:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 5
            case 1316:
            {
                worldName = "doy/dungeons/bunker/bunker1";
                break;
            }
            case 1317:
            {
                worldName = "doy/dungeons/bunker/bunker2";
                break;
            }
            case 1318:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 6
            case 1319:
            {
                worldName = "doy/dungeons/side/gassen2";
                break;
            }
            case 1320:
            {
                worldName = "doy/dungeons/sewer/sewer1";
                break;
            }
            case 1321:
            {
                worldName = "doy/dungeons/bunker/bunker1";
                break;
            }
            case 1322:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 7
            case 1323:
            {
                worldName = "doy/dungeons/side/gassen1";
                break;
            }
            case 1324:
            {
                worldName = "doy/dungeons/side/gassen2";
                break;
            }
            case 1325:
            {
                worldName = "doy/dungeons/sewer/sewer2";
                break;
            }
            case 1326:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 8
            case 1327:
            {
                worldName = "doy/dungeons/side/gassen1";
                break;
            }
            case 1328:
            {
                worldName = "doy/dungeons/side/gassen2";
                break;
            }
            case 1329:
            {
                worldName = "doy/dungeons/sewer/sewer2";
                break;
            }
            case 1330:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 9
            case 1331:
            {
                worldName = "doy/dungeons/bunker/bunker1";
                break;
            }
            case 1332:
            {
                worldName = "doy/dungeons/bunker/bunker2";
                break;
            }
            case 1333:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }

            //Sec 10
            case 1334:
            {
                worldName = "doy/dungeons/side/gassen2";
                break;
            }
            case 1335:
            {
                worldName = "doy/dungeons/sewer/sewer2";
                break;
            }
            case 1336:
            {
                worldName = "doy/dungeons/bunker/bunker2";
                break;
            }
            case 1337:
            {
                worldName = "doy/dungeons/storage/storage";
                break;
            }


            //Terrain  "SEC-S"
            case 2006:
            {
                worldName = "terrain/a_06";
                break;
            }
            case 2007:
            {
                worldName = "terrain/a_07";
                break;
            }
            case 2008:
            {
                worldName = "terrain/a_08";
                break;
            }
            case 2009:
            {
                worldName = "terrain/a_09";
                break;
            }
            case 2010:
            {
                worldName = "terrain/a_10";
                break;
            }
            case 2011:
            {
                worldName = "terrain/a_11";
                break;
            }
            case 2026:
            {
                worldName = "terrain/b_06";
                break;
            }
            case 2027:
            {
                worldName = "terrain/b_07";
                break;
            }
            case 2028:
            {
                worldName = "terrain/b_08";
                break;
            }
            case 2029:
            {
                worldName = "terrain/b_09";
                break;
            }
            case 2030:
            {
                worldName = "terrain/b_10";
                break;
            }
            case 2031:
            {
                worldName = "terrain/b_11";
                break;
            }

            case 2032:
            {
                worldName = "terrain/b_12";
                break;
            }

            case 2046:
            {
                worldName = "terrain/c_06";
                break;
            }
            case 2047:
            {
                worldName = "terrain/c_07";
                break;
            }
            case 2048:
            {
                worldName = "terrain/c_08";
                break;
            }
            case 2049:
            {
                worldName = "terrain/c_09";
                break;
            }
            case 2050:
            {
                worldName = "terrain/c_10";
                break;
            }
            case 2051:
            {
                worldName = "terrain/c_11";
                break;
            }
            case 2052:
            {
                worldName = "terrain/c_12";
                break;
            }
            case 2065:
            {
                worldName = "terrain/d_05";
                break;
            }
            case 2066:
            {
                worldName = "terrain/d_06";
                break;
            }
            case 2067:
            {
                worldName = "terrain/d_07";
                break;
            }
            case 2068:
            {
                worldName = "terrain/d_08";
                break;
            }
            case 2069:
            {
                worldName = "terrain/d_09";
                break;
            }
            case 2070:
            {
                worldName = "terrain/d_10";
                break;
            }
            case 2071:
            {
                worldName = "terrain/d_11";
                break;
            }
            case 2072:
            {
                worldName = "terrain/d_12";
                break;
            }
            case 2073:
            {
                worldName = "terrain/d_13";
                break;
            }
            case 2083:
            {
                worldName = "terrain/e_03";
                break;
            }
            case 2085:
            {
                worldName = "terrain/e_05";
                break;
            }
            case 2086:
            {
                worldName = "terrain/e_06";
                break;
            }
            case 2087:
            {
                worldName = "terrain/e_07";
                break;
            }
            case 2088:
            {
                worldName = "terrain/e_08";
                break;
            }
            case 2089:
            {
                worldName = "terrain/e_09";
                break;
            }
            case 2090:
            {
                worldName = "terrain/e_10";
                break;
            }
            case 2091:
            {
                worldName = "terrain/e_11";
                break;
            }
            case 2092:
            {
                worldName = "terrain/e_12";
                break;
            }
            case 2093:
            {
                worldName = "terrain/e_13";
                break;
            }
            case 2102:
            {
                worldName = "terrain/f_02";
                break;
            }
            case 2103:
            {
                worldName = "terrain/f_03";
                break;
            }
            case 2104:
            {
                worldName = "terrain/f_04";
                break;
            }
            case 2105:
            {
                worldName = "terrain/f_05";
                break;
            }
            case 2106:
            {
                worldName = "terrain/f_06";
                break;
            }
            case 2107:
            {
                worldName = "terrain/f_07";
                break;
            }
            case 2108:
            {
                worldName = "terrain/f_08";
                break;
            }
            case 2109:
            {
                worldName = "terrain/f_09";
                break;
            }
            case 2110:
            {
                worldName = "terrain/f_10";
                break;
            }
            case 2111:
            {
                worldName = "terrain/f_11";
                break;
            }
            case 2112:
            {
                worldName = "terrain/f_12";
                break;
            }
            case 2113:
            {
                worldName = "terrain/f_13";
                break;
            }
            case 2123:
            {
                worldName = "terrain/g_03";
                break;
            }
            case 2124:
            {
                worldName = "terrain/g_04";
                break;
            }
            case 2125:
            {
                worldName = "terrain/g_05";
                break;
            }
            case 2126:
            {
                worldName = "terrain/g_06";
                break;
            }
            case 2127:
            {
                worldName = "terrain/g_07";
                break;
            }
            case 2128:
            {
                worldName = "terrain/g_08";
                break;
            }
            case 2129:
            {
                worldName = "terrain/g_09";
                break;
            }
            case 2130:
            {
                worldName = "terrain/g_10";
                break;
            }
            case 2131:
            {
                worldName = "terrain/g_11";
                break;
            }
            case 2132:
            {
                worldName = "terrain/g_12";
                break;
            }
            case 2133:
            {
                worldName = "terrain/g_13";
                break;
            }
            case 2143:
            {
                worldName = "terrain/h_03";
                break;
            }
            case 2144:
            {
                worldName = "terrain/h_04";
                break;
            }
            case 2145:
            {
                worldName = "terrain/h_05";
                break;
            }
            case 2146:
            {
                worldName = "terrain/h_06";
                break;
            }
            case 2147:
            {
                worldName = "terrain/h_07";
                break;
            }
            case 2148:
            {
                worldName = "terrain/h_08";
                break;
            }
            case 2149:
            {
                worldName = "terrain/h_09";
                break;
            }
            case 2150:
            {
                worldName = "terrain/h_10";
                break;
            }
            case 2151:
            {
                worldName = "terrain/h_11";
                break;
            }
            case 2152:
            {
                worldName = "terrain/h_12";
                break;
            }
            case 2153:
            {
                worldName = "terrain/h_13";
                break;
            }
            case 2154:
            {
                worldName = "terrain/h_14";
                break;
            }
            case 2155:
            {
                worldName = "terrain/h_15";
                break;
            }
            case 2156:
            {
                worldName = "terrain/h_16";
                break;
            }
            case 2162:
            {
                worldName = "terrain/i_02";
                break;
            }
            case 2163:
            {
                worldName = "terrain/i_03";
                break;
            }
            case 2164:
            {
                worldName = "terrain/i_04";
                break;
            }
            case 2165:
            {
                worldName = "terrain/i_05";
                break;
            }
            case 2166:
            {
                worldName = "terrain/i_06";
                break;
            }
            case 2167:
            {
                worldName = "terrain/i_07";
                break;
            }
            case 2168:
            {
                worldName = "terrain/i_08";
                break;
            }
            case 2169:
            {
                worldName = "terrain/i_09";
                break;
            }
            case 2170:
            {
                worldName = "terrain/i_10";
                break;
            }
            case 2171:
            {
                worldName = "terrain/i_11";
                break;
            }
            case 2172:
            {
                worldName = "terrain/i_12";
                break;
            }
            case 2173:
            {
                worldName = "terrain/i_13";
                break;
            }
            case 2174:
            {
                worldName = "terrain/i_14";
                break;
            }
            case 2175:
            {
                worldName = "terrain/i_15";
                break;
            }
            case 2176:
            {
                worldName = "terrain/i_16";
                break;
            }
            case 2181:
            {
                worldName = "terrain/j_01";
                break;
            }
            case 2182:
            {
                worldName = "terrain/j_02";
                break;
            }
            case 2183:
            {
                worldName = "terrain/j_03";
                break;
            }
            case 2184:
            {
                worldName = "terrain/j_04";
                break;
            }
            case 2185:
            {
                worldName = "terrain/j_05";
                break;
            }
            case 2186:
            {
                worldName = "terrain/j_06";
                break;
            }
            case 2187:
            {
                worldName = "terrain/j_07";
                break;
            }
            case 2188:
            {
                worldName = "terrain/j_08";
                break;
            }
            case 2189:
            {
                worldName = "terrain/j_09";
                break;
            }
            case 2190:
            {
                worldName = "terrain/j_10";
                break;
            }
            case 2191:
            {
                worldName = "terrain/j_11";
                break;
            }
            case 2192:
            {
                worldName = "terrain/j_12";
                break;
            }
            case 2193:
            {
                worldName = "terrain/j_13";
                break;
            }
            case 2194:
            {
                worldName = "terrain/j_14";
                break;
            }
            case 2195:
            {
                worldName = "terrain/j_15";
                break;
            }
            case 2196:
            {
                worldName = "terrain/j_16";
                break;
            }
            case 2201:
            {
                worldName = "terrain/k_01";
                break;
            }
            case 2202:
            {
                worldName = "terrain/k_02";
                break;
            }
            case 2203:
            {
                worldName = "terrain/k_03";
                break;
            }
            case 2204:
            {
                worldName = "terrain/k_04";
                break;
            }
            case 2205:
            {
                worldName = "terrain/k_05";
                break;
            }
            case 2206:
            {
                worldName = "terrain/k_06";
                break;
            }
            case 2207:
            {
                worldName = "terrain/k_07";
                break;
            }
            case 2209:
            {
                worldName = "terrain/k_09";
                break;
            }
            case 2210:
            {
                worldName = "terrain/k_10";
                break;
            }
            case 2211:
            {
                worldName = "terrain/k_11";
                break;
            }
            case 2212:
            {
                worldName = "terrain/k_12";
                break;
            }
            case 2213:
            {
                worldName = "terrain/k_13";
                break;
            }
            case 2214:
            {
                worldName = "terrain/k_14";
                break;
            }
            case 2215:
            {
                worldName = "terrain/k_15";
                break;
            }
            case 2216:
            {
                worldName = "terrain/k_16";
                break;
            }

            //HACKNET
            case 3000:
            {
                worldName = "hacknet/nc_mainframe";
                break;
            }
            case 3001:
            {
                worldName = "hacknet/nc_firewall";
                break;
            }
            case 3002:
            {
                worldName = "hacknet/nc_faction_dns";
                break;
            }

            //Listed as "Area 51"
            case 28:
            {
                worldName = "terrainchange/cityexit2";
                break;
            }

            //Outpost Undergrounds
            //Simmons
            case 4006:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Jeriko
            case 4028:
            {
                worldName = "bop/underground_for";
                break;
            }
            //McPherson
            case 4031:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Tyron
            case 4047:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Krupp
            case 4049:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Ceres
            case 4052:
            {
                worldName = "bop/underground_min";
                break;
            }
            //Gravis
            case 4066:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Redrock
            case 4067:
            {
                worldName = "bop/underground_min";
                break;
            }
            //Crest
            case 4069:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Foster
            case 4071:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Grant
            case 4073:
            {
                worldName = "bop/underground_min";
                break;
            }
            //Shirkan
            case 4090:
            {
                worldName = "bop/underground_for";
                break;
            }
            //Regent
            case 4105:
            {
                worldName = "bop/underground_for";
                break;
            }
            //Devereaux
            case 4111:
            {
                worldName = "bop/underground_for";
                break;
            }
            //Cycrow
            case 4112:
            {
                worldName = "bop/underground_lab";
                break;
            }
            //Gabanium
            case 4128:
            {
                worldName = "bop/underground_min";
                break;
            }
            //Tezla
            case 4132:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Jankins
            case 4143:
            {
                worldName = "bop/underground_lab";
                break;
            }
            //Hawkins
            case 4147:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Malstrond
            case 4149:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Blackhill
            case 4151:
            {
                worldName = "bop/underground_for";
                break;
            }
            //Nemesis
            case 4153:
            {
                worldName = "bop/underground_lab";
                break;
            }
            //Soliko
            case 4155:
            {
                worldName = "bop/underground_lab";
                break;
            }
            //Eastgate
            case 4156:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Sieger
            case 4164:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Chester
            case 4165:
            {
                worldName = "bop/underground_lab";
                break;
            }
            //Emmerson
            case 4167:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Avenger
            case 4168:
            {
                worldName = "bop/underground_min";
                break;
            }
            //Tristar
            case 4171:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Cajun
            case 4175:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Yutano
            case 4189:
            {
                worldName = "bop/underground_min";
                break;
            }
            //Syncon
            case 4194:
            {
                worldName = "bop/underground_lab";
                break;
            }
            //Rockshore
            case 4204:
            {
                worldName = "bop/underground_fab";
                break;
            }
            //Drakkhan
            case 4205:
            {
                worldName = "bop/underground_for";
                break;
            }
            //Northstar
            case 4207:
            {
                worldName = "bop/underground_com";
                break;
            }
            //Tescom
            case 4212:
            {
                worldName = "bop/underground_com";
                break;
            }

            //Area MC5
            case 1086:
            {
                worldName = "area_mc5/h_14";
                break;
            }
            case 1084:
            {
                worldName = "area_mc5/mc_main";
                break;
            }
            case 1085:
            {
                worldName = "area_mc5/mc_unit";
                break;
            }
            case 1080:
            {
                worldName = "area_mc5/mc_energy";
                break;
            }
            case 1081:
            {
                worldName = "area_mc5/mc_energy";
                break;
            }
            case 1082:
            {
                worldName = "area_mc5/mc_energy";
                break;
            }
            case 1083:
            {
                worldName = "area_mc5/mc_energy";
                break;
            }

            //Area H14
            case 1093:
            {
                worldName = "area_mc5/mc_main";
                break;
            }
            case 1094:
            {
                worldName = "area_mc5/mc_unit";
                break;
            }
            case 1090:
            {
                worldName = "area_mc5/mc_energy";
                break;
            }
            case 1091:
            {
                worldName = "area_mc5/mc_energy";
                break;
            }
            case 1092:
            {
                worldName = "area_mc5/mc_energy";
                break;
            }

            //Neofrag Holomatch Levels - Need to verify maps still!
            case 90001:
            {
                worldName = "holomatch/neofrag1";
                break;
            }
            case 90002:
            {
                worldName = "holomatch/neofrag2";
                break;
            }
            case 90003:
            {
                worldName = "holomatch/neofrag3";
                break;
            }
            case 90004:
            {
                worldName = "holomatch/neofrag4";
                break;
            }
            case 90005:
            {
                worldName = "holomatch/neofrag5";
                break;
            }
            case 90006:
            {
                worldName = "holomatch/neofrag6";
                break;
            }
            case 90007:
            {
                worldName = "holomatch/neofrag6";
                break;
            }
            case 90008:
            {
                worldName = "holomatch/neofrag6";
                break;
            }
            case 90009:
            {
                worldName = "holomatch/neofrag1";
                break;
            }
            case 90010:
            {
                worldName = "holomatch/neofrag2";
                break;
            }
            case 90011:
            {
                worldName = "holomatch/neofrag3";
                break;
            }
            case 90012:
            {
                worldName = "holomatch/neofrag4";
                break;
            }
            case 90013:
            {
                worldName = "holomatch/neofrag5";
                break;
            }
            case 90014:
            {
                worldName = "holomatch/neofrag6";
                break;
            }
            case 90015:
            {
                worldName = "holomatch/neofrag6";
                break;
            }
            case 90016:
            {
                worldName = "holomatch/neofrag6";
                break;
            }

            //DoY Tunnels
            case 1500:
            {
                worldName = "doy/tunnels/tunnel21";
                break;
            }
            case 1501:
            {
                worldName = "doy/tunnels/tunnel11";
                break;
            }
            case 1502:
            {
                worldName = "doy/tunnels/tunnel22";
                break;
            }
            case 1503:
            {
                worldName = "doy/tunnels/tunnel22";
                break;
            }
            case 1504:
            {
                worldName = "doy/tunnels/tunnel11";
                break;
            }
            case 1505:
            {
                worldName = "doy/tunnels/tunnel12";
                break;
            }
            case 1506:
            {
                worldName = "doy/tunnels/tunnel23";
                break;
            }
            case 1507:
            {
                worldName = "doy/tunnels/tunnel12";
                break;
            }
            case 1508:
            {
                worldName = "doy/tunnels/tunnel23";
                break;
            }
            case 1509:
            {
                worldName = "doy/tunnels/tunnel22";
                break;
            }
            case 1510:
            {
                worldName = "doy/tunnels/tunnel11";
                break;
            }
            case 1511:
            {
                worldName = "doy/tunnels/tunnel12";
                break;
            }
            case 1512:
            {
                worldName = "doy/tunnels/tunnel11";
                break;
            }
            case 1513:
            {
                worldName = "doy/tunnels/tunnel22";
                break;
            }
            case 1514:
            {
                worldName = "doy/tunnels/tunnel13";
                break;
            }
            case 1515:
            {
                worldName = "doy/tunnels/tunnel22";
                break;
            }
            case 1516:
            {
                worldName = "doy/tunnels/tunnel23";
                break;
            }
            case 1517:
            {
                worldName = "doy/tunnels/tunnel12";
                break;
            }
            case 1518:
            {
                worldName = "doy/tunnels/tunnel21";
                break;
            }
            case 1519:
            {
                worldName = "doy/tunnels/tunnel12";
                break;
            }
            case 1520:
            {
                worldName = "doy/tunnels/tunnel21";
                break;
            }
            case 1521:
            {
                worldName = "doy/tunnels/tunnel11";
                break;
            }
            case 1522:
            {
                worldName = "doy/tunnels/tunnel21";
                break;
            }
            case 1523:
            {
                worldName = "doy/tunnels/tunnel13";
                break;
            }
            case 1524:
            {
                worldName = "doy/tunnels/tunnel22";
                break;
            }
            case 1525:
            {
                worldName = "doy/tunnels/tunnel21";
                break;
            }
            case 1526:
            {
                worldName = "doy/tunnels/tunnel11";
                break;
            }

            //Callash Event Level
            case 1600:
            {
                worldName = "bop/underground_lab";
                break;
            }

            /*
            All maps listed after this are NOT listed in the pak_worldinfo.def
            file when patched to 200

            Because of this, I have commented them all out.
            However, if you have any problems with maps, remove my comment fields.
            */

    /*
            case 25:
            {
                worldName = "terrainchange/outzone_st2t";

                break;
            }
            case 26:
            {
                worldName = "terrainchange/abbey_2_terrain";

                break;
            }
            case 27:
            {
                worldName = "terrainchange/cityexit1";

                break;
            }
            case 29:
            {
                worldName = "terrainchange/lpp4_2_terrain";

                break;
            }
            case 30:
            {
                worldName = "terrainchange/pepsway_exit";

                break;
            }
            case 505:
            {
                worldName = "terrainchange/techtown_enter1";

                break;
            }

            //hmmm.. here some id is wrong
            case 505:
            {
                worldName = "terrainchange/techtown_enter1a";

                break;
            }
            case 505:
            {
                worldName = "terrainchange/techtown_enter1b";

                break;
            }
            case 505:
            {
                worldName = "terrainchange/techtown_enter2";

                break;
            }
            // end of "wrong IDs"

            case 506:
            {
                worldName = "terrainchange/tt_enter1";

                break;
            }
            case 552:
            {
                worldName = "terrainchange/darkmetal_load";

                break;
            }
            case 702:
            {
                worldName = "terrainchange/outside1";

                break;
            }
            case 703:
            {
                worldName = "terrainchange/outside2";

                break;
            }
            case 705:
            {
                worldName = "terrainchange/mine2out";

                break;
            }
            case 706:
            {
                worldName = "pipemaze";

                break;
            }
            case 707:
            {
                worldName = "pipeout";

                break;
            }
            case 708:
            {
                worldName = "labor/labor";

                break;
            }
            case 709:
            {
                worldName = "terrainchange/laborout";

                break;
            }
            case 754:
            {
                worldName = "terrainchange/military_outside";

                break;
            }
            case 850:
            {
                worldName = "terrainchange/caveout_p1";

                break;
            }
            case 851:
            {
                worldName = "terrainchange/caveout_p2";

                break;
            }
            case 852:
            {
                worldName = "terrainchange/caveout_p3";

                break;
            }
    */

    //special case UNKNOW ID is worldroute but it doesnt work
    /*
            case ???:
            {
                worldName = "worldroute";

                break;
            }
    */

            //default
            default://by default all players go to plaza sect 1
            {
                worldName = "plaza/plaza_p1";
                break;
            }
        }//close switch
	}

    u8 packet1[] = {0xfe, 0x17, 0x00, 0x83, 0x0c, loc, (loc >> 8), (loc >> 16), (loc >> 24), 0x00, 0x00, 0x00, 0x00};
    packet1[1] = worldName.size() + 11;
	u8 packet3[] = {0x00};

	Socket->write(packet1, sizeof(packet1));
	Socket->write(worldName.c_str(), worldName.size());
	Socket->write(packet3, sizeof(packet3));
}


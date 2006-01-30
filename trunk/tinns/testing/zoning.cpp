/*

	Zoning - makes changing of maps possible ingame

	Authors:
	- initial release by unknown person
	- Sting <>
	- Akiko <akiko@gmx.org>

	MODIFIED: 23 August 2005 by Sting
        REASON: - TO match up with defs/pak_worldinfo.def from patch 200

	MODIFIED: 27 August 2005 by Akiko
	REASON: - reformated for better reading
		- thinking about a way to optimize the case statements
	MODIFIED: 28 August 2005 by Akiko
	REASON: - continued reformating

*/

#include "tinns.h"

void SendZone(PClient *Client, PGameState *State, int loc)
{
	//TODO : FIX case for worldroute,subway and 5 missing id 505

	PGameSocket *Socket = Client->GetGameSocket();
	//PSocket *UDPSocket = Client->GetUDPSocket();

	Socket->SetTimeOutValue(0xffff);

	//write first piece like an header
	u8 packet1[] = {0xfe, 0x19, 0x00, 0x83, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
	packet1[5] = loc;  //remember to set[5] for the minimap
	Socket->Write(packet1, sizeof(packet1));

	//////////////////////////
	switch(loc)
	{
		//Subway - this seems to change your location, but then goes wrong?
		case 1000:
		{
			u8 packet[]={"subway"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		
		//PLAZA
		case 1:
		{
			u8 packet[] = {"plaza/plaza_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2:
		{
			u8 packet[]={"plaza/plaza_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 101:
		{
			u8 packet[]={"plaza/plaza_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 102:
		{
			u8 packet[]={"plaza/plaza_p4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 104:
		{
			u8 packet[]={"plaza/plaza_concenter"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 105:
		{
			u8 packet[]={"plaza/plaza_cityadmin"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//DiamondRealEstate HQ
		case 107:
		{
			u8 packet[]={"plaza/plaza_fraction1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//NeXT HQ
		case 108:
		{
			u8 packet[]={"plaza/plaza_fraction2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 109:
		{
			u8 packet[]={"plaza/plaza_Jobcenter"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//VIAROSSO
		case 3:
		{
			u8 packet[]={"viarosso/viarosso_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 4:
		{
			u8 packet[]={"viarosso/viarosso_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 401:
		{	
			u8 packet[]={"viarosso/viarosso_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 402:
		{
			u8 packet[]={"viarosso/viarosso_lab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 403:
		{
			u8 packet[]={"viarosso/viarosso_bank"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 404:
		{
			u8 packet[]={"viarosso/viarosso_ncpd"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Tangent HQ
		case 405:
		{
			u8 packet[]={"viarosso/viarosso_fraction1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//BioTech HQ
		case 406:
		{
			u8 packet[]={"viarosso/viarosso_fraction2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//ProtoPharm HQ
		case 407:
		{
			u8 packet[]={"viarosso/viarosso_fraction3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//PEPPER PARK
		case 5:
		{
			u8 packet[]={"pepper/pepper_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 6:
		{
			u8 packet[]={"pepper/pepper_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 7:
		{
			u8 packet[]={"pepper/pepper_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 601:
		{
			u8 packet[]={"pepper/pepper_maze"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
 
		//Club Veronique
		case 602:
		{
			u8 packet[]={"pepper/pepper_redlight"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		
		//Bump Asylum
		case 603:
		{
			u8 packet[]={"pepper/pepper_guerilla"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 604:
		{
			u8 packet[]={"pepper/pepper_church"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
 
		//Pussy Club
		case 605:
		{
			u8 packet[]={"pepper/pepper_disco"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
 
		//Secret Black Dragon
		case 606:
		{
			u8 packet[]={"pepper/pepper_mafia3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
 
		//Secret Passage
		case 607:
		{
			u8 packet[]={"pepper/pepper_sway"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 608:
		{
			u8 packet[]={"pepper/pepper_holo1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 609:
		{
			u8 packet[]={"pepper/pepper_shooting1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 610:
		{
			u8 packet[]={"pepper/pepper_pub"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
 
		//Black Dragon HQ
		case 611:
		{
			u8 packet[]={"pepper/pepper_mafia2"};Socket->Write(packet, sizeof(packet));
			break;
		} 
		
		//Twister
		case 612:
		{
			u8 packet[]={"pepper/pepper_peep"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
 
		//Tsunami Syndicate HQ
		case 613:
		{
			u8 packet[]={"pepper/pepper_mafia1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
 
		//Secret Passage 2
		case 614:
		{
			u8 packet[]={"pepper/pepper_sway2"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 615:
		{
			u8 packet[]={"pepper/pepper_holo2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//INDUSTRIAL AREA
		case 8:
		{
			u8 packet[]={"industry/industry_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 801:
		{
			u8 packet[]={"industry/industry_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//OUTZONE
		case 9:
		{
			u8 packet[]={"outzone/outzone_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 10:
		{
			u8 packet[] = {"outzone/lostpart_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 11:
		{
			u8 packet[]={"outzone/lostpart_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 12:
		{
			u8 packet[]={"outzone/lostpart_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 13:
		{
			u8 packet[]={"outzone/lostpart_p4"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 20:
		{
			u8 packet[]={"outzone/rooftops"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 21:
		{
			u8 packet[]={"outzone/outzone_powerplant"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 22:
		{
			u8 packet[]={"outzone/outzone_jail"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 23:
		{
			u8 packet[]={"outzone/outzone_abbey"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 24:
		{
			u8 packet[]={"outzone/outzone_station"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//TUTORIAL
		case 40:
		{
			u8 packet[]={"tutorial/tutorial_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 41:
		{
			u8 packet[]={"tutorial/tutorial_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 42:
		{
			u8 packet[]={"tutorial/tutorial_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 43:
		{
			u8 packet[]={"tutorial/tutorial_p4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 44:
		{
			u8 packet[]={"tutorial/tutorial_p5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 45:
		{
			u8 packet[]={"tutorial/tutorial_p6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 46:
		{
			u8 packet[]={"outzone/lostpart_p1_tut"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//MINING FACILITY
		case 700:
		{
			u8 packet[]={"mining/mining1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 701:
		{
			u8 packet[]={"mining/mining2"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 704:
		{
			u8 packet[]={"mining/mining3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//TECHTOWN
		case 500:
		{
			u8 packet[]={"techtown/techtown_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 502:
		{
			u8 packet[]={"techtown/techtown_mine1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 503:
		{
			u8 packet[]={"techtown/techtown_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 504:
		{
			u8 packet[]={"techtown/techtown_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//DARKMETAL
		case 550:
		{
			u8 packet[]={"Darkmetal/Darkmetal_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 551:
		{
			u8 packet[]={"Darkmetal/Darkmetal_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		
		//CANYON
		case 650:
		{
			u8 packet[]={"canyon/canyon_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//MILITARY BASE
		case 750:
		{
			u8 packet[]={"military/military_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 751:
		{
			u8 packet[]={"military/military_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 752:
		{
			u8 packet[]={"military/military_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 753:
		{
			u8 packet[]={"military/military_pstairs"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Wastleand "Lava" Caves
		case 853:
		{
			u8 packet[]={"dungeons/caves_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 854:
		{
			u8 packet[]={"dungeons/caves_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 855:
		{
			u8 packet[]={"dungeons/caves_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Main Sewers
		case 900:
		{
			u8 packet[]={"sewer/sewer_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 901:
		{
			u8 packet[]={"sewer/sewer_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 902:
		{
			u8 packet[]={"sewer/sewer_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 903:
		{
			u8 packet[]={"sewer/sewer_p4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  

		case 904:
		{
			u8 packet[]={"sewer/pipeway_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 905:
		{
			u8 packet[]={"sewer/pipeway_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 906:
		{
			u8 packet[]={"sewer/pipeway_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 907:
		{
			u8 packet[]={"sewer/pipeway_p4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//City Sewers
		//Plaza Level 1
		case 920:
		{
			u8 packet[]={"citysewer/plazasewer_1a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 921:
		{
			u8 packet[]={"citysewer/plazasewer_2a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 922:
		{
			u8 packet[]={"citysewer/plazasewer_3a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 923:
		{
			u8 packet[]={"citysewer/plazasewer_4d2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 924:
		{
			u8 packet[]={"citysewer/plazasewer_2b"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 925:
		{
			u8 packet[]={"citysewer/plazasewer_3b"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 926:
		{
			u8 packet[]={"citysewer/plazasewer_1a2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 927:
		{
			u8 packet[]={"citysewer/plazasewer_4d3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 928:
		{
			u8 packet[]={"citysewer/plazasewer_3b2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 929:
		{
			u8 packet[]={"citysewer/plazasewer_4d"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 930:
		{
			u8 packet[]={"citysewer/plazasewer_2b2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 931:
		{
			u8 packet[]={"citysewer/plazasewer_3a2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 940:
		{
			u8 packet[]={"citysewer/peppersewer_1a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 941:
		{
			u8 packet[]={"citysewer/peppersewer_2a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 942:
		{
			u8 packet[]={"citysewer/peppersewer_3a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 943:
		{
			u8 packet[]={"citysewer/peppersewer_1b"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 944:
		{
			u8 packet[]={"citysewer/peppersewer_2b"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 945:
		{
			u8 packet[]={"citysewer/peppersewer_3a2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 946:
		{
			u8 packet[]={"citysewer/peppersewer_1b2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 947:
		{
			u8 packet[]={"citysewer/peppersewer_3a3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 950:
		{
			u8 packet[]={"citysewer/peppersewer_1a1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 951:
		{
			u8 packet[]={"citysewer/peppersewer_3a4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 952:
		{
			u8 packet[]={"citysewer/peppersewer_3b"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 953:
		{
			u8 packet[]={"citysewer/peppersewer_3c"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 954:
		{
			u8 packet[]={"citysewer/peppersewer_3c1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 955:
		{
			u8 packet[]={"citysewer/peppersewer_1c"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 956:
		{
			u8 packet[]={"citysewer/peppersewer_1c1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 957:
		{
			u8 packet[]={"citysewer/peppersewer_1c2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 958:
		{
			u8 packet[]={"citysewer/peppersewer_1c3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 959:
		{
			u8 packet[]={"citysewer/peppersewer_3b1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 960:
		{
			u8 packet[]={"citysewer/peppersewer_2b1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		
		//Plaza Level 2
		case 970:
		{
			u8 packet[]={"citysewer/plazasewer_1c2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 971:
		{
			u8 packet[]={"citysewer/plazasewer_2d2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 972:
		{
			u8 packet[]={"citysewer/plazasewer_3d2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 973:
		{
			u8 packet[]={"citysewer/plazasewer_4b"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 974:
		{
			u8 packet[]={"citysewer/plazasewer_2d3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 975:
		{
			u8 packet[]={"citysewer/plazasewer_3c2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 976:
		{
			u8 packet[]={"citysewer/plazasewer_1c"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 977:
		{
			u8 packet[]={"citysewer/plazasewer_4c"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 978:
		{
			u8 packet[]={"citysewer/plazasewer_3c"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 979:
		{
			u8 packet[]={"citysewer/plazasewer_4b2"};
			Socket->Write(packet, sizeof(packet));
			break;}  
		case 980:
		{
			u8 packet[]={"citysewer/plazasewer_2d"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 981:
		{
			u8 packet[]={"citysewer/plazasewer_3d"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//MILITARY DUNGEONS
		case 999:
		{
			u8 packet[]={"military/militarydungeon1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1001:
		{
			u8 packet[] = {"military/militarydungeon2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1002:
		{
			u8 packet[] = {"military/militarydungeon3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1003:
		{
			u8 packet[]={"military/militarydungeon4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1004:
		{
			u8 packet[]={"military/militarydungeon5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1005:
		{
			u8 packet[]={"military/militarydungeon6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//TECHTOWN DUNGEONS
		case 1006:
		{
			u8 packet[]={"techtown/techmine1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1007:
		{
			u8 packet[]={"techtown/techmine2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1008:
		{
			u8 packet[]={"techtown/techmine3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1009:
		{
			u8 packet[]={"techtown/techmine4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1010:
		{
			u8 packet[]={"techtown/techmine5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1011:
		{
			u8 packet[]={"techtown/techmine6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//CANYON DUNGEONS
		case 1012:
		{
			u8 packet[]={"canyon/cavemap1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1013:
		{
			u8 packet[]={"canyon/cavemap2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1014:
		{
			u8 packet[]={"canyon/cavemap3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1015:
		{
			u8 packet[]={"canyon/cavemap4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1016:
		{
			u8 packet[]={"canyon/cavemap5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1017:
		{
			u8 packet[]={"canyon/cavemap6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//SWAMP DUNGEONS
		case 1018:
		{
			u8 packet[]={"swamp/caveman1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1019:
		{
			u8 packet[]={"swamp/caveman2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1020:
		{
			u8 packet[]={"swamp/caveman3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1021:
		{
			u8 packet[]={"swamp/caveman4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1022:
		{
			u8 packet[]={"swamp/caveman5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1023:
		{
			u8 packet[]={"swamp/caveman6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//DESERT DUNGEONS
		case 1024:
		{
			u8 packet[]={"desert/wurm1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1025:
		{
			u8 packet[]={"desert/wurm2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1026:
		{
			u8 packet[]={"desert/wurm3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1027:
		{
			u8 packet[]={"desert/wurm4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1028:
		{
			u8 packet[]={"desert/wurm5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1029:
		{
			u8 packet[]={"desert/wurm6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//DESERT "CITY" DUNGEONS
		case 1030:
		{
			u8 packet[]={"desert/desertdung1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1031:
		{
			u8 packet[]={"desert/desertdung3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//CANYON DUNGEONS
		case 1032:
		{
			u8 packet[]={"canyon/cavemap1a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1033:
		{
			u8 packet[]={"canyon/cavemap2a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1034:
		{
			u8 packet[]={"canyon/cavemap3a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//WASTELAND "STORAGE" HALLS
		case 1035:
		{
			u8 packet[]={"outzone/storage1a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1036:
		{
			u8 packet[]={"outzone/storage2a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1037:
		{
			u8 packet[]={"outzone/storage3a"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//OUTZONE DUNGEONS
		case 1040:
		{
			u8 packet[]={"outzone/storage1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1041:
		{
			u8 packet[]={"outzone/storage2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1042:
		{
			u8 packet[]={"outzone/storage3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1043:
		{
			u8 packet[]={"outzone/storage4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1044:
		{
			u8 packet[]={"outzone/storage5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1045:
		{
			u8 packet[]={"outzone/storage6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//GRAVE
		case 1050:
		{
			u8 packet[]={"desert/egyptmap"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Plaza Sewer X
		case 1060:
		{
			u8 packet[]={"sewer/sewer_p4_x3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1061:
		{
			u8 packet[]={"sewer/sewer_p4_x31"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1062:
		{
			u8 packet[]={"sewer/sewer_p4_x2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1063:
		{
			u8 packet[]={"sewer/sewer_p4_x21"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1064:
		{
			u8 packet[]={"sewer/sewer_p4_x1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1065:
		{
			u8 packet[]={"sewer/sewer_p4_x11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Pepper Sewer X
		case 1070:
		{
			u8 packet[]={"bop/underground_min_x1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1071:
		{
			u8 packet[]={"bop/underground_min_x11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1072:
		{
			u8 packet[]={"bop/underground_min_x2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1073:
		{
			u8 packet[]={"bop/underground_min_x21"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1075:
		{
			u8 packet[]={"bop/underground_lab_x1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1076:
		{
			u8 packet[]={"bop/underground_lab_x11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1077:
		{
			u8 packet[]={"bop/underground_lab_x2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//DOY "Dungeons" X
		//These do not appear to be included anywhere
		//SO I have only put the context - maybe maps can be found later?
		//I have put Sector Names instead of maps, as no maps were listed
		
		//Remember, IF you copy these maps from NC2, use them AT YOUR OWN RISK!!!
		//They have NOT been tested!

/*
		case 1100:
		{
			u8 packet[]={" DoY Sec-01 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1101:
		{
			u8 packet[]={" DoY Sec-03 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1102:
		{
			u8 packet[]={" DoY Sec-02 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1103:
		{
			u8 packet[]={" DoY Sec-05 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1104:
		{
			u8 packet[]={" DoY Sec-04 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1105:
		{
			u8 packet[]={" DoY Sec-06 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1106:
		{
			u8 packet[]={" DoY Sec-07 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1107:
		{
			u8 packet[]={" DoY Sec-10 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1108:
		{
			u8 packet[]={" DoY Sec-09 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1109:
		{
			u8 packet[]={" Doy Sec-08 "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Twilight Guardian HQ
		case 1110:
		{
			u8 packet[]={""};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1111:
		{
			u8 packet[]={""};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1112:
		{
			u8 packet[]={""};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1113:
		{
			u8 packet[]={""};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1114:
		{
			u8 packet[]={""};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Sec 1
		case 1300:
		{
			u8 packet[]={"doy/dungeons/side/gassen1e1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1301:
		{
			u8 packet[]={"doy/dungeons/side/gassen2e1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1302:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer1e1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1303:
		{
			u8 packet[]={"doy/dungeons/storage/storagee1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 2
		case 1304:
		{
			u8 packet[]={"doy/dungeons/side/gassen1e2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1305:
		{
			u8 packet[]={"doy/dungeons/side/gassen2e2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1306:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer2e1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1307:
		{
			u8 packet[]={"doy/dungeons/storage/storagee2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 3
		case 1308:
		{
			u8 packet[]={"doy/dungeons/side/gassen1e3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1309:
		{
			u8 packet[]={"doy/dungeons/side/gassen2e3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1310:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer1e2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1311:
		{
			u8 packet[]={"doy/dungeons/storage/storagee3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 4
		case 1312:
		{
			u8 packet[]={"doy/dungeons/side/gassen1m3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1313:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer2m1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1314:
		{
			u8 packet[]={"doy/dungeons/bunker/bunker1m1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1315:
		{
			u8 packet[]={"doy/dungeons/storage/storagem1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 5
		case 1316:
		{
			u8 packet[]={"doy/dungeons/bunker/bunker1h1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1317:
		{
			u8 packet[]={"doy/dungeons/bunker/bunker2h1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1318:
		{
			u8 packet[]={"doy/dungeons/storage/storageh1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 6
		case 1319:
		{
			u8 packet[]={"doy/dungeons/side/gassen2m1"};
			Socket->Write(packet,sizeof(packet));
			break;
		}
		case 1320:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer1m1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1321:
		{
			u8 packet[]={"doy/dungeons/bunker/bunker1m2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1322:
		{
			u8 packet[]={"doy/dungeons/storage/storagem2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 7
		case 1323:
		{
			u8 packet[]={"doy/dungeons/side/gassen1e4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1324:
		{
			u8 packet[]={"doy/dungeons/side/gassen2e4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1325:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer2e2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1326:
		{
			u8 packet[]={"doy/dungeons/storage/storagee4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 8
		case 1327:
		{
			u8 packet[]={"doy/dungeons/side/gassen1e5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1328:
		{
			u8 packet[]={"doy/dungeons/side/gassen2e5"}
			;Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1329:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer2e3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1330:
		{
			u8 packet[]={"doy/dungeons/storage/storagee5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 9
		case 1331:
		{
			u8 packet[]={"doy/dungeons/bunker/bunker1h2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1332:
		{
			u8 packet[]={"doy/dungeons/bunker/bunker2h2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1333:
		{
			u8 packet[]={"doy/dungeons/storage/storageh2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Sec 10
		case 1334:
		{
			u8 packet[]={"doy/dungeons/side/gassen2m2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1335:
		{
			u8 packet[]={"doy/dungeons/sewer/sewer2m2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1336:
		{
			u8 packet[]={"doy/dungeons/bunker/bunker2m1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1337:
		{
			u8 packet[]={"doy/dungeons/storage/storagem3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
*/
		
		//Terrain  "SEC-S"
		case 2006:
		{
			u8 packet[]={"terrain/a_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2007:
		{
			u8 packet[]={"terrain/a_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2008:
		{
			u8 packet[]={"terrain/a_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2009:
		{
			u8 packet[]={"terrain/a_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2010:
		{
			u8 packet[]={"terrain/a_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2011:
		{
			u8 packet[]={"terrain/a_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2026:
		{
			u8 packet[]={"terrain/b_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2027:
		{
			u8 packet[]={"terrain/b_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2028:
		{
			u8 packet[]={"terrain/b_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2029:
		{
			u8 packet[]={"terrain/b_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2030:
		{
			u8 packet[]={"terrain/b_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2031:
		{
			u8 packet[]={"terrain/b_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		/* This terrain is commented out in pak_worldinfo.defs
		case 2032:
		{
			u8 packet[]={"terrain/b_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		*/

		case 2046:
		{
			u8 packet[]={"terrain/c_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2047:
		{
			u8 packet[]={"terrain/c_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2048:
		{
			u8 packet[]={"terrain/c_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2049:
		{
			u8 packet[]={"terrain/c_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2050:
		{
			u8 packet[]={"terrain/c_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2051:
		{
			u8 packet[]={"terrain/c_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2052:
		{
			u8 packet[]={"terrain/c_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2065:
		{
			u8 packet[]={"terrain/d_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2066:
		{
			u8 packet[]={"terrain/d_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2067:
		{
			u8 packet[]={"terrain/d_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2068:
		{
			u8 packet[]={"terrain/d_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2069:
		{
			u8 packet[]={"terrain/d_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2070:
		{
			u8 packet[]={"terrain/d_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2071:
		{
			u8 packet[]={"terrain/d_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2072:
		{
			u8 packet[]={"terrain/d_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2073:
		{
			u8 packet[]={"terrain/d_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2083:
		{
			u8 packet[]={"terrain/e_03"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2085:
		{
			u8 packet[]={"terrain/e_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2086:
		{
			u8 packet[]={"terrain/e_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2087:
		{
			u8 packet[]={"terrain/e_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2088:
		{
			u8 packet[]={"terrain/e_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2089:
		{
			u8 packet[]={"terrain/e_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2090:
		{
			u8 packet[]={"terrain/e_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2091:
		{
			u8 packet[]={"terrain/e_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2092:
		{
			u8 packet[]={"terrain/e_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2093:
		{
			u8 packet[]={"terrain/e_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2102:
		{	
			u8 packet[]={"terrain/f_02"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2103:
		{
			u8 packet[]={"terrain/f_03"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2104:
		{
			u8 packet[]={"terrain/f_04"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2105:
		{
			u8 packet[]={"terrain/f_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2106:
		{
			u8 packet[]={"terrain/f_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2107:
		{
			u8 packet[]={"terrain/f_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2108:
		{
			u8 packet[]={"terrain/f_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2109:
		{
			u8 packet[]={"terrain/f_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2110:
		{
			u8 packet[]={"terrain/f_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2111:
		{
			u8 packet[]={"terrain/f_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2112:
		{
			u8 packet[]={"terrain/f_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2113:
		{
			u8 packet[]={"terrain/f_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2123:
		{
			u8 packet[]={"terrain/g_03"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2124:
		{
			u8 packet[]={"terrain/g_04"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2125:
		{
			u8 packet[]={"terrain/g_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2126:
		{
			u8 packet[]={"terrain/g_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2127:
		{
			u8 packet[]={"terrain/g_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2128:
		{
			u8 packet[]={"terrain/g_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2129:
		{
			u8 packet[]={"terrain/g_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2130:
		{
			u8 packet[]={"terrain/g_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2131:
		{
			u8 packet[]={"terrain/g_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2132:
		{
			u8 packet[]={"terrain/g_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2133:
		{
			u8 packet[]={"terrain/g_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2143:
		{
			u8 packet[]={"terrain/h_03"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2144:
		{
			u8 packet[]={"terrain/h_04"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2145:
		{
			u8 packet[]={"terrain/h_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2146:
		{
			u8 packet[]={"terrain/h_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2147:
		{
			u8 packet[]={"terrain/h_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2148:
		{
			u8 packet[]={"terrain/h_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2149:
		{
			u8 packet[]={"terrain/h_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2150:
		{
			u8 packet[]={"terrain/h_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2151:
		{
			u8 packet[]={"terrain/h_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2152:
		{
			u8 packet[]={"terrain/h_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2153:
		{
			u8 packet[]={"terrain/h_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2154:
		{
			u8 packet[]={"terrain/h_14"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2155:
		{
			u8 packet[]={"terrain/h_15"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2156:
		{
			u8 packet[]={"terrain/h_16"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2162:
		{
			u8 packet[]={"terrain/i_02"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2163:
		{
			u8 packet[]={"terrain/i_03"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2164:
		{
			u8 packet[]={"terrain/i_04"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2165:
		{
			u8 packet[]={"terrain/i_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2166:
		{
			u8 packet[]={"terrain/i_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2167:
		{
			u8 packet[]={"terrain/i_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2168:
		{
			u8 packet[]={"terrain/i_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2169:
		{
			u8 packet[]={"terrain/i_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2170:
		{
			u8 packet[]={"terrain/i_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2171:
		{
			u8 packet[]={"terrain/i_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2172:
		{
			u8 packet[]={"terrain/i_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2173:
		{
			u8 packet[]={"terrain/i_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2174:
		{
			u8 packet[]={"terrain/i_14"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2175:
		{
			u8 packet[]={"terrain/i_15"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2176:
		{
			u8 packet[]={"terrain/i_16"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2181:
		{
			u8 packet[]={"terrain/j_01"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2182:
		{
			u8 packet[]={"terrain/j_02"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2183:
		{
			u8 packet[]={"terrain/j_03"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2184:
		{
			u8 packet[]={"terrain/j_04"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2185:
		{
			u8 packet[]={"terrain/j_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2186:
		{
			u8 packet[]={"terrain/j_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2187:
		{
			u8 packet[]={"terrain/j_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2188:
		{
			u8 packet[]={"terrain/j_08"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2189:
		{
			u8 packet[]={"terrain/j_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2190:
		{
			u8 packet[]={"terrain/j_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2191:
		{
			u8 packet[]={"terrain/j_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2192:
		{
			u8 packet[]={"terrain/j_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2193:
		{
			u8 packet[]={"terrain/j_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2194:
		{
			u8 packet[]={"terrain/j_14"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2195:
		{
			u8 packet[]={"terrain/j_15"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2196:
		{
			u8 packet[]={"terrain/j_16"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2201:
		{
			u8 packet[]={"terrain/k_01"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2202:
		{
			u8 packet[]={"terrain/k_02"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2203:
		{
			u8 packet[]={"terrain/k_03"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2204:
		{
			u8 packet[]={"terrain/k_04"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2205:
		{
			u8 packet[]={"terrain/k_05"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2206:
		{
			u8 packet[]={"terrain/k_06"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2207:
		{
			u8 packet[]={"terrain/k_07"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2209:
		{
			u8 packet[]={"terrain/k_09"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2210:
		{
			u8 packet[]={"terrain/k_10"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2211:
		{
			u8 packet[]={"terrain/k_11"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2212:
		{
			u8 packet[]={"terrain/k_12"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2213:
		{
			u8 packet[]={"terrain/k_13"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2214:
		{
			u8 packet[]={"terrain/k_14"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2215:
		{
			u8 packet[]={"terrain/k_15"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 2216:
		{
			u8 packet[]={"terrain/k_16"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		/* Listed in pak_worldinfo.def, but maps not in NC1
		//The maps in NC2 are found in the doy/hacknet section
		//HACKNET
		case 3000:
		{
			u8 packet[]={" NC Mainframe "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 3001:
		{
			u8 packet[]={" NC Firewall "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 3002:
		{
			u8 packet[]={" NC Faction DNS "};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		*/
		
		//Listed as "Area 51"
		case 28:
		{
			u8 packet[]={"terrainchange/cityexit2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Outpost Undergrounds
		//Simmons
		case 4006:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Jeriko
		case 4028:
		{
			u8 packet[]={"bop/underground_for"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//McPherson
		case 4031:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Tyron
		case 4047:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Krupp
		case 4049:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Ceres
		case 4052:
		{
			u8 packet[]={"bop/underground_min"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Gravis
		case 4066:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Redrock
		case 4067:
		{
			u8 packet[]={"bop/underground_min"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Crest
		case 4069:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Foster
		case 4071:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Grant
		case 4073:
		{
			u8 packet[]={"bop/underground_min"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Shirkan
		case 4090:
		{
			u8 packet[]={"bop/underground_for"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Regent
		case 4105:
		{
			u8 packet[]={"bop/underground_for"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Devereaux
		case 4111:
		{
			u8 packet[]={"bop/underground_for"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Cycrow
		case 4112:
		{
			u8 packet[]={"bop/underground_lab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Gabanium
		case 4128:
		{
			u8 packet[]={"bop/underground_min"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Tezla
		case 4132:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Jankins
		case 4143:
		{
			u8 packet[]={"bop/underground_lab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Hawkins
		case 4147:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Malstrond
		case 4149:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Blackhill
		case 4151:
		{
			u8 packet[]={"bop/underground_for"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Nemesis
		case 4153:
		{
			u8 packet[]={"bop/underground_lab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Soliko
		case 4155:
		{
			u8 packet[]={"bop/underground_lab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Eastgate
		case 4156:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Sieger
		case 4164:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Chester
		case 4165:
		{
			u8 packet[]={"bop/underground_lab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Emmerson
		case 4167:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Avenger
		case 4168:
		{
			u8 packet[]={"bop/underground_min"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Tristar
		case 4171:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Cajun
		case 4175:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Yutano
		case 4189:
		{
			u8 packet[]={"bop/underground_min"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Syncon
		case 4194:
		{
			u8 packet[]={"bop/underground_lab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Rockshore
		case 4204:
		{
			u8 packet[]={"bop/underground_fab"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Drakkhan
		case 4205:
		{
			u8 packet[]={"bop/underground_for"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Northstar
		case 4207:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		//Tescom
		case 4212:
		{
			u8 packet[]={"bop/underground_com"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Area MC5
		case 1086:
		{
			u8 packet[]={"area_mc5/h14x1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1084:
		{
			u8 packet[]={"area_mc5/mc_main"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1085:
		{
			u8 packet[]={"area_mc5/mc_unit"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1080:
		{
			u8 packet[]={"area_mc5/mc_energyx1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1081:
		{
			u8 packet[]={"area_mc5/mc_energyx2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1082:
		{
			u8 packet[]={"area_mc5/mc_energyx3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1083:
		{
			u8 packet[]={"area_mc5/mc_energyx4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//Area H14
		case 1093:
		{
			u8 packet[]={"area_mc5/mc_mainy1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1094:
		{
			u8 packet[]={"area_mc5/mc_unity1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1090:
		{
			u8 packet[]={"area_mc5/mc_energyy1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1091:
		{
			u8 packet[]={"area_mc5/mc_energyy2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1092:
		{
			u8 packet[]={"area_mc5/mc_energyy3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//Neofrag Holomatch Levels - Need to verify maps still!
		case 90001:
		{
			u8 packet[]={"holomatch/neofrag1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90002:
		{
			u8 packet[]={"holomatch/neofrag2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90003:
		{
			u8 packet[]={"holomatch/neofrag3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90004:
		{
			u8 packet[]={"holomatch/neofrag4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90005:
		{
			u8 packet[]={"holomatch/neofrag5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90006:
		{
			u8 packet[]={"holomatch/neofrag6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90007:
		{
			u8 packet[]={"holomatch/neofrag6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90008:
		{
			u8 packet[]={"holomatch/neofrag6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90009:
		{
			u8 packet[]={"holomatch/neofrag1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90010:
		{
			u8 packet[]={"holomatch/neofrag2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90011:
		{
			u8 packet[]={"holomatch/neofrag3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90012:
		{
			u8 packet[]={"holomatch/neofrag4"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90013:
		{
			u8 packet[]={"holomatch/neofrag5"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90014:
		{
			u8 packet[]={"holomatch/neofrag6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90015:
		{
			u8 packet[]={"holomatch/neofrag6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 90016:
		{
			u8 packet[]={"holomatch/neofrag6"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//DoY Tunnels
		case 1500:
		{
			u8 packet[]={"doy/tunnels/tunnela"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1501:
		{
			u8 packet[]={"doy/tunnels/tunnelb"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1502:
		{
			u8 packet[]={"doy/tunnels/tunnelc"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1503:
		{
			u8 packet[]={"doy/tunnels/tunneld"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1504:
		{
			u8 packet[]={"doy/tunnels/tunnele"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1505:
		{
			u8 packet[]={"doy/tunnels/tunnelf"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1506:
		{
			u8 packet[]={"doy/tunnels/tunnelg"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1507:
		{
			u8 packet[]={"doy/tunnels/tunnelh"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1508:
		{
			u8 packet[]={"doy/tunnels/tunneli"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1509:
		{
			u8 packet[]={"doy/tunnels/tunnelj"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1510:
		{
			u8 packet[]={"doy/tunnels/tunnelk"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1511:
		{
			u8 packet[]={"doy/tunnels/tunnell"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1512:
		{
			u8 packet[]={"doy/tunnels/tunnelm"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1513:
		{
			u8 packet[]={"doy/tunnels/tunneln"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1514:
		{
			u8 packet[]={"doy/tunnels/tunnelo"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1515:
		{
			u8 packet[]={"doy/tunnels/tunnelp"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1516:
		{
			u8 packet[]={"doy/tunnels/tunnelq"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1517:
		{
			u8 packet[]={"doy/tunnels/tunnelr"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1518:
		{
			u8 packet[]={"doy/tunnels/tunnels"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1519:
		{
			u8 packet[]={"doy/tunnels/tunnelt"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1520:
		{
			u8 packet[]={"doy/tunnels/tunnelu"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1521:
		{
			u8 packet[]={"doy/tunnels/tunnelv"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1522:
		{
			u8 packet[]={"doy/tunnels/tunnelw"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1523:
		{
			u8 packet[]={"doy/tunnels/tunnelx"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1524:
		{
			u8 packet[]={"doy/tunnels/tunnely"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1525:
		{
			u8 packet[]={"doy/tunnels/tunnelz"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 1526:
		{
			u8 packet[]={"doy/tunnels/tunnelkos"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		
		//GM Event Map?
		case 1600:
		{
			u8 packet[]={"doy/events/callash1"};
			Socket->Write(packet, sizeof(packet));
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
			u8 packet[]={"terrainchange/outzone_st2t"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 26:
		{
			u8 packet[]={"terrainchange/abbey_2_terrain"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 27:
		{
			u8 packet[]={"terrainchange/cityexit1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 29:
		{
			u8 packet[]={"terrainchange/lpp4_2_terrain"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 30:
		{
			u8 packet[]={"terrainchange/pepsway_exit"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
		case 505:
		{
			u8 packet[]={"terrainchange/techtown_enter1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}

		//hmmm.. here some id is wrong
		case 505:
		{
			u8 packet[]={"terrainchange/techtown_enter1a"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 505:
		{
			u8 packet[]={"terrainchange/techtown_enter1b"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 505:
		{
			u8 packet[]={"terrainchange/techtown_enter2"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		// end of "wrong IDs"

		case 506:
		{
			u8 packet[]={"terrainchange/tt_enter1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 552:
		{
			u8 packet[]={"terrainchange/darkmetal_load"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 702:
		{
			u8 packet[]={"terrainchange/outside1"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 703:
		{
			u8 packet[]={"terrainchange/outside2"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 705:
		{
			u8 packet[]={"terrainchange/mine2out"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 706:
		{
			u8 packet[]={"pipemaze"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 707:
		{
			u8 packet[]={"pipeout"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 708:
		{
			u8 packet[]={"labor/labor"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 709:
		{
			u8 packet[]={"terrainchange/laborout"};
			Socket->Write(packet, sizeof(packet));
			break;
		} 
		case 754:
		{
			u8 packet[]={"terrainchange/military_outside"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 850:
		{
			u8 packet[]={"terrainchange/caveout_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 851:
		{
			u8 packet[]={"terrainchange/caveout_p2"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
		case 852:
		{
			u8 packet[]={"terrainchange/caveout_p3"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
*/

//special case UNKNOW ID is worldroute but it doesnt work
/*
		case ???:
		{
			u8 packet[]={"worldroute"};
			Socket->Write(packet, sizeof(packet));
			break;
		}  
*/

		//default
		default://by default all players go to plaza sect 1
		{
			u8 packet[]={"plaza/plaza_p1"};
			Socket->Write(packet, sizeof(packet));
			break;
		}
	}//close switch

	///////////////////
	//this close the packet
	u8 packet3[]={ 0x00};
	Socket->Write(packet3, sizeof(packet3));
}


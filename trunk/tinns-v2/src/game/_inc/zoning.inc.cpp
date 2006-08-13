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

/** TinNS include file. Do NOT compile **/


/** --------------------- Packet defs --------------------- **/
char ZonePacket[] = {0x13, 0x05, 0x00, 0x23, 0x94,
	0x0c,
	0x03, 0x05, 0x00, 0x1f, 0x01, 0x00, 0x25, 0x13, 0x01, 0x00,
	0x0e, 0x02,
	0x17,
	0x03, 0x06, 0x00, 0x23, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
	};

char GenZone[] = {0x13, 0x3b, 0x00, 0x6b, 0xaf,
    0x2c,
    0x03, 0x3b, 0x00, 0x23, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
    };
/** ------------------------------------------------------- **/

if ((Packet[4] == 0x22) && (Packet[5] == 0x0d))  // Zoning Part 1
{
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    Client->IncreaseTransactionID(); // from NeoX
        
		ZonePacket[0] = 0x13;
		ZonePacket[5] = 0x0c;
		ZonePacket[6] = 0x03;
    *(u16*)&ZonePacket[7] = Client->GetUDP_ID();
		ZonePacket[9] = 0x1f;  // from NeoX
		*(u16*)&ZonePacket[10] = Client->GetLocalID(); // from NeoX
		ZonePacket[12] = 0x25; // from NeoX
		ZonePacket[13] = 0x13; // from NeoX
    *(u16*)&ZonePacket[14] = Client->GetTransactionID(); // from NeoX
		ZonePacket[16] = 0x0e; // from NeoX
		ZonePacket[17] = 0x02; // from NeoX
    ZonePacket[18] = 0x17; // from NeoX
    ZonePacket[19] = 0x03; // from NeoX					
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    *(u16*)&ZonePacket[20] = Client->GetUDP_ID();
		ZonePacket[22] = 0x23; // from NeoX	
		ZonePacket[23] = 0x04; // from NeoX	
    *(u16*)&ZonePacket[1] = Client->GetUDP_ID();
    *(u16*)&ZonePacket[3] = Client->GetSessionID();
    u32 value = *(u32*)&Packet[11];
    Char->SetLocation(value);

    //int SizeZP = 42;
    Console->Print("Zoning Stage 1: New location: %d", value);  //<- Commented out, since it works now
    //*(u32*)&ZonePacket[SizeZP - 8] = *(u32*)&Packet[15];
    //*(u32*)&ZonePacket[34] = *(u32*)&Packet[15];
    *(u16*)&ZonePacket[34] = *(u16*)&Packet[15]; // from NeoX
    *(u16*)&ZonePacket[38] = Client->GetTransactionID(); // from NeoX
    //Client->getUDPConn()->write(ZonePacket, sizeof(ZonePacket));
    Client->getUDPConn()->write(ZonePacket, 42); // size from NeoX
    Client->getUDPConn()->flushSendBuffer();
    Console->Print("Zoning Stage 1: packet sent");
}
else if ((Packet[4] == 0x22) && (Packet[5] == 0x03))  // Zoning Part 2
{
    Console->Print("Zoning Stage 2: Sending Zone information");  //<- Commented out, since it works now
    u32 loc = Char->GetLocation();
    
    Console->Print("\t Sending Ready packet");
    static const u8 READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
		Client->getTCPConn()->write(READY, sizeof(READY)); // Added from NeoX
    Client->getTCPConn()->flushSendBuffer(); // Added from NeoX
		
		Console->Print("\t Sending Zone");
    SendZone(Client, loc);
    u8 up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
		*(u16*)&up[1] = Client->GetLocalID(); // Added from NeoX
		*(u16*)&up[5] = Client->getUDPConn()->getPort(); // Added from NeoX
    Client->getUDPConn()->write(up, sizeof(up));
    Client->getUDPConn()->flushSendBuffer();
    Client->SetUDP_ID(0); // ResetUDP_ID,  from NeoX
    State->UDP.mState = PGameState::UDP::GUS_SYNC1;//was GUS_SYNC1
}
else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x04))  // Zoning Genrep
{
    //Client->IncreaseUDP_ID();
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    *(unsigned short*)&GenZone[1] = Client->GetUDP_ID();
    *(unsigned short*)&GenZone[3] = Client->GetSessionID();
    *(unsigned short*)&GenZone[7] = Client->GetUDP_ID();

    Client->getUDPConn()->write(GenZone, sizeof(GenZone));
    Client->getUDPConn()->flushSendBuffer();
    
    //Client->IncreaseUDP_ID();
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    *(unsigned short*)&ZonePacket[7] = Client->GetUDP_ID();
    //Client->IncreaseUDP_ID();
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    *(unsigned short*)&ZonePacket[20] = Client->GetUDP_ID();
    *(unsigned short*)&ZonePacket[1] = Client->GetUDP_ID();
    *(unsigned short*)&ZonePacket[3] = Client->GetSessionID();

    Char->SetLocation(*(u32*)&Packet[11]);
    *(unsigned int*)&ZonePacket[34] = *(unsigned int*)&Packet[15];
    
    *(u16*)&ZonePacket[10] = Client->GetLocalID(); // from NeoX
    Client->IncreaseTransactionID(); // from NeoX
    *(u16*)&ZonePacket[14] = Client->GetTransactionID(); // from NeoX
    *(u16*)&ZonePacket[38] = Client->GetTransactionID();  // from NeoX
    
    Client->getUDPConn()->write(ZonePacket, sizeof(ZonePacket));
    Client->getUDPConn()->flushSendBuffer();
    Console->Print("Genrepzoning");
}

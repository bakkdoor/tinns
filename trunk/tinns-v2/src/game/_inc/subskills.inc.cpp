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
u8 SubSkillPacket[] = {
    0x13,
    0x00, 0x00, 0x00, 0x00,                         // 2x UDP_ID Sequence
    0x09, 0x03,
    0x00, 0x00,                                     // 1x UDP_ID Sequence
    0x1F, 0x01, 0x00, 0x25, 0x23, 0x28, 0x11, 0x03,
    0x00, 0x00,                                     // 1x UDP_ID Sequence
    0x1F, 0x01, 0x00, 0x25, 0x13,
    0x00, 0x00,                                     // 1x UDP_ID Sequence
    0x09,
    0x00,                                           // Skill ID
    0x00, 0x01,
    0x00, 0x05,                                     // Remaining Skill PTS
    0x00
};
/** ------------------------------------------------------- **/


// Player wants to increase subskill
// NC2 Version, isnt working here :(
Console->Print("Player wants to increase SubSkill ID %d",Packet[9]);

//Client->IncreaseUDP_ID();
Client->SetUDP_ID(Client->GetUDP_ID()+1);
*(u16*)&SubSkillPacket[1] = Client->GetUDP_ID();
*(u16*)&SubSkillPacket[3] = Client->GetSessionID();
*(u16*)&SubSkillPacket[7] = Client->GetUDP_ID();

*(u8*)&SubSkillPacket[27] = Packet[9];

Client->getUDPConn()->write(SubSkillPacket, sizeof(SubSkillPacket));

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

/** TODO: must be broadcast to zone **/

/** --------------------- Packet defs --------------------- **/
char ExitChair[] = {
    0x13, 0x49, 0x00, 0x1c, 0xd6,
    0x10,
    0x03, 0x49, 0x00, 0x1f, 0x01, 0x00, 0x22, 0x18, 0x83, 0x01,
    0x80, 0x5f, 0x82, 0x80, 0x80, 0xc3};
/** ------------------------------------------------------- **/

//Client->IncreaseUDP_ID();
Client->SetUDP_ID(Client->GetUDP_ID()+1);
*(unsigned short*)&ExitChair[1] = Client->GetUDP_ID();
*(unsigned short*)&ExitChair[3] = Client->GetSessionID();
*(unsigned short*)&ExitChair[7] = Client->GetUDP_ID();

*(u16*)&ExitChair[10] = Client->GetLocalID(); // from NeoX
*(u16*)&ExitChair[13] = Char->Coords.mY + 768;
*(u16*)&ExitChair[15] = Char->Coords.mZ + 768;
*(u16*)&ExitChair[17] = Char->Coords.mX + 768;
*(u8*)&ExitChair[19] = Char->Coords.mUD;
*(u8*)&ExitChair[20] = Char->Coords.mLR;
*(u8*)&ExitChair[21] = Char->Coords.mAct;

Client->getUDPConn()->write(ExitChair, sizeof(ExitChair));
Client->getUDPConn()->flushSendBuffer();
//Console->Print("leaving chair");

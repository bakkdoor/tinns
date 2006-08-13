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
char DirectAdd[] = {
    0x13, 0x7e, 0x00, 0xf2, 0xad,
    0x0c,
    0x03, 0x7e, 0x00, 0x1f, 0x01, 0x00, 0x32, 0x01, 0xd5, 0x3d,
    0x02, 0x00};
/** ------------------------------------------------------- **/

//Client->IncreaseUDP_ID();
Client->SetUDP_ID(Client->GetUDP_ID()+1);
*(u16*)&DirectAdd[1] = Client->GetUDP_ID();
*(u16*)&DirectAdd[3] = Client->GetSessionID();
*(u16*)&DirectAdd[7] = Client->GetUDP_ID();
*(u16*)&DirectAdd[10] = Client->GetLocalID(); // from NeoX

Client->getUDPConn()->write(DirectAdd, sizeof(DirectAdd));
Client->getUDPConn()->flushSendBuffer();
Console->Print("Adding player to direct");

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
char AptLiftUse[] = {
    0x13, 0xc3, 0x00, 0x63, 0xf8,
    0x0f,
    0x03, 0xc3, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x04, 0x00, 0xda,
    0xcf, 0x03, 0x00, 0x01, 0x00,
    0x15,
    0x1b, 0x22, 0x01, 0x00, 0x00, 0x1f, 0x49, 0x82, 0x81, 0x81,
    0xe5, 0x6b, 0x04, 0xd5, 0x76, 0x01, 0x00, 0x00, 0x00, 0x11,
    0x11};

char AptLiftFail[] = {
    0x13, 0x84, 0x00, 0x19, 0xec,
    0x08,
    0x03, 0x84, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x03};

/** ------------------------------------------------------- **/

Console->Print("User %d, Access apt with name/pw %s", Client->GetCharID(), Packet+14);
u32 i = MySQL->GetAptID(*(unsigned int*)&Packet[9], Packet+14);
if(i != 0)
{
    //Client->IncreaseUDP_ID();
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    *(u16*)&AptLiftUse[1] = Client->GetUDP_ID();
    *(u16*)&AptLiftUse[3] = Client->GetSessionID();
    *(u16*)&AptLiftUse[7] = Client->GetUDP_ID();
    *(u16*)&AptLiftUse[10] = Client->GetLocalID(); // from NeoX
    *(u32*)&AptLiftUse[15] = i;

    Char->SetLocation(i);

    Client->getUDPConn()->write(AptLiftUse, sizeof(AptLiftUse));
}
else // Wrong password or that entry doesnt have that apt
{
    //Client->IncreaseUDP_ID();
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    *(u16*)&AptLiftFail[1] = Client->GetUDP_ID();
    *(u16*)&AptLiftFail[3] = Client->GetSessionID();
    *(u16*)&AptLiftFail[7] = Client->GetUDP_ID();
    *(u16*)&AptLiftUse[10] = Client->GetLocalID(); // from NeoX
    
    Client->getUDPConn()->write(AptLiftFail, sizeof(AptLiftFail));
    Console->Print("Appartitemfail");
}
Client->getUDPConn()->flushSendBuffer();


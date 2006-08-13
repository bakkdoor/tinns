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
/*
  TODO :  - check for packet info with NeoX
          - broadcast packet to zone
*/  

/** --------------------- Packet defs --------------------- **/
char VehicleDriver[] = {
    0x13, 0x58, 0x00, 0x9a, 0xae,
    0x0c,
    0x03, 0x58, 0x00, 0x1f, 0x01, 0x00, 0x21, 0xfc, 0x03, 0x00,
    //            | Item ID |
    0x00, 0x00,
    0x19,
    0x32, 0xfb, 0x03, 0x03, 0xad, 0x80, 0xf7, 0x84, 0xc9, 0x91,
    0x90, 0xbf, 0xa1, 0x2e, 0x7f, 0x02, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x80, 0x00, 0x80};
/** ------------------------------------------------------- **/


Console->Print("Someone trying to get into a vehicle. Seat Nr. %d", Packet[16]);
if(Packet[16] == 0)
{
    //Client->IncreaseUDP_ID();
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    *(unsigned short*)&VehicleDriver[1] = Client->GetUDP_ID();
    *(unsigned short*)&VehicleDriver[3] = Client->GetSessionID();
    *(unsigned short*)&VehicleDriver[7] = Client->GetUDP_ID();
    *(unsigned short*)&VehicleDriver[13] = *(unsigned short*)&Packet[13];
    *(u16*)&VehicleDriver[10] = Client->GetLocalID(); // from NeoX
    
    Client->getUDPConn()->write(VehicleDriver, sizeof(VehicleDriver));
    Client->getUDPConn()->flushSendBuffer();
    Console->Print("Getting on vehicle");
}

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

/** ------------------------------------------------------- **/

if(type == 0x20) // Player update
{
    if(Packet[4] == 0x7F)
    {
        //Playerposition update
        u16 newmY = *(u16*)&Packet[5];
        u16 newmZ = *(u16*)&Packet[7];
        u16 newmX = *(u16*)&Packet[9];
        if ((Char->Coords.mY != newmY) || (Char->Coords.mZ != newmZ) || (Char->Coords.mX != newmX) || (Char->Coords.mLR != Packet[12]))
        {
          Char->Coords.mY = newmY;
          Char->Coords.mZ = newmZ;
          Char->Coords.mX = newmX;
          Char->Coords.mLR = Packet[12];
          Char->SetDirtyFlag();
        }
        Char->Coords.mUD = Packet[11];
        Char->Coords.mAct = Packet[13];
        // movement byte:
        // 0x00 NC has no focus (player alt+tab'ed out)
        // 0x20 Char does nothing
        // 0x22 kneeing
        // 0x28 left step
        // 0x30 right step
        // 0x40 walking (not running)
        // 0x60 forward
        // 0xA0 backward
        // bits:  00000000
        //        BFWRL.K.
//Console->Print("Char %d position update: X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Action(%#.2x)",Client->GetID(), Char->Coords.mX, Char->Coords.mY, Char->Coords.mZ, Char->Coords.mUD, Char->Coords.mLR, Char->Coords.mAct);
        
        outMsg = BuildCharHealthUpdateMsg(Client);
        ClientManager->UDPBroadcast(outMsg, Client);
        
        outMsg = BuildCharPosUpdateMsg(Client);
        ClientManager->UDPBroadcast(outMsg, Client, 5000);
                  
    }
    else if (Packet[4] == 0x20)
    {
        Char->Coords.mAct = Packet[5];
        //Console->Print("Movementupdate: [%2x]", Packet[5]);
    }
    else if (Packet[4] == 0x80)
    {
      // Sitting update
      outMsg = BuildCharSittingMsg(Client, *(u16*)&Packet[5]);
      ClientManager->UDPBroadcast(outMsg, Client);
    }
    else
    {}
    break;
}
else if(type == 0x32) // Vehicle update
{
    // Vehicle updates
    if(Packet[19] == 0)
    {
        Console->Print("Vehicle going nowhere");
    }
    else
    {
        if(Packet[19] & 1)
            Console->Print("Vehicle going left");
        if(Packet[19] & 2)
            Console->Print("Vehicle going right");
        if(Packet[19] & 4)
            Console->Print("Vehicle going forward");
        if(Packet[19] & 8)
            Console->Print("Vehicle going back");
        Char->SetDirtyFlag();
    }
    //Playerposition update
    Char->Coords.mY = *(u16*)&Packet[5];
    Char->Coords.mZ = *(u16*)&Packet[7];
    Char->Coords.mX = *(u16*)&Packet[9];
    Char->Coords.mUD = Packet[11];
    Char->Coords.mLR = Packet[12];
    Char->Coords.mAct = Packet[13];
    // movement byte:
    // 0x00 NC has no focus (player alt+tab'ed out)
    // 0x20 Char does nothing
    // 0x22 kneeing
    // 0x28 left step
    // 0x30 right step
    // 0x40 walking (not running)
    // 0x60 forward
    // 0xA0 backward
    break;
}

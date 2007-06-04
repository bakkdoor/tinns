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
#include "main.h"

void PCommands::doCmd_dev_t()
{
    if(IsAdmin() == false)
        return;
    
    char Arg1[30], Arg2[30];

    Arg1[0] = '\0';
    Arg2[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
        if(ArgC > 1)
        {
            GetArgText(2, Arg2, 30);
        }
    }

    u32 val1;
    u8 val2;
    char tmpStr[128];
    static PMessage* tmpMsg = NULL;
    bool SetUDP_IDNeeded = true;
    PChar* nChar = source->GetChar();
    (nChar->Coords).mY += 20;
    (nChar->Coords).mZ += 20;
    (nChar->Coords).mX += 20;
    
    if (!tmpMsg)
    {
      tmpMsg = MsgBuilder->BuildPacket0Msg(source);
      //tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
      SetUDP_IDNeeded = false;
    }

    if(Arg1[0] != '\0' && Arg2[0] != '\0')
    {
        val1 = atoi(Arg1);
        val2 = (u8)(atoi(Arg2) & 0xff);
        //tmpMsg->U8Data(16 + val1) = val2;
        tmpMsg->U8Data(10 + val1) = val2;
        snprintf(tmpStr, 127, "Data #%d set to value 0x%02x", val1, val2);
    }
    else
    {
      if (tmpMsg)
        delete tmpMsg;
      //tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
      tmpMsg = MsgBuilder->BuildPacket0Msg(source);
      SetUDP_IDNeeded = false;
      snprintf(tmpStr, 127, "Data reset to normal values");
    }

    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);

    PMessage* SendMsg = new PMessage(*tmpMsg);
    if(SetUDP_IDNeeded) {
      source->FillInUDP_ID(SendMsg);
    }
    SendMsg->Dump();
    //ClientManager->UDPBroadcast(SendMsg, source);
    source->SendUDPMessage(SendMsg);
}

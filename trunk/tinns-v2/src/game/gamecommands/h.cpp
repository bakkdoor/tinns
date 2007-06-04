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

void PCommands::doCmd_dev_h()
{
  if(IsAdmin() == false)
    return;

  if(source->GetAccountLevel() < PAL_ADMIN)
    return;

  u8 val1, val2, val3, val4;
  char tmpStr[128];

  if(ArgC > 0)
  {
    val1 = (u8)(GetArgInt(1) & 0xff);
    val2 = (u8)(GetArgInt(2) & 0xff);
    val3 = (u8)(GetArgInt(3) & 0xff);
    val4 = (ArgC > 3) ? (u8)(GetArgInt(4) & 0xff) : 0x01;
  }
  else
  {
    val1 = 0xff;
    val2 = 0xff;
    val3 = 0xff;
    val4 = 0x01;
  }

    PMessage* tmpMsg = new PMessage(14);  

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //source->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // source->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)source->GetLocalID();
	*tmpMsg << (u8)0x30;
	*tmpMsg << (u8)val1; //Head Heath (% ?) (45%)
	*tmpMsg << (u8)val2; //Body Heath (35%)
	*tmpMsg << (u8)val3; //Feet Heath (20%)
	*tmpMsg << (u8)val4;

    (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
    ClientManager->UDPBroadcast(tmpMsg, source);

    snprintf(tmpStr, 127, "Data set to values 0x%02x 0x%02x", val1, val2);
    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);
}

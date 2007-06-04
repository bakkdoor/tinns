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

void PCommands::doCmd_dev_v()
{
    if(IsAdmin() == false)
      return;
    
    char Arg1[30];

    Arg1[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
    }

    char tmpStr[128];
    int testmode = 0; // change here only for u8/u16/u32 testvalue use

    if(Arg1[0] != '\0')
    {
        switch(testmode)
        {
            case 0: source->testval8 = (u8)(atoi(Arg1) & 0xff); break;
            case 1: source->testval16 = (u16)(atoi(Arg1) & 0xffff); break;
            case 2: source->testval32 = (u32)(atoi(Arg1) & 0xffffffff); break;
        }
    }
    else
    {
        source->testval8 = 0;
        source->testval16 = 0;
        source->testval32 = 0;
    }

    switch(testmode)
    {
        case 0: snprintf(tmpStr, 127, "Test value u8 set to 0x%02x (%d)", source->testval8, source->testval8); break;
        case 1: snprintf(tmpStr, 127, "Test value u16 set to 0x%04x (%d)", source->testval16, source->testval16); break;
        case 2: snprintf(tmpStr, 127, "Test value u32 set to 0x%08x (%d)", source->testval32, source->testval32); break;
        default: tmpStr[0] = '\0';
    }
    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);

}

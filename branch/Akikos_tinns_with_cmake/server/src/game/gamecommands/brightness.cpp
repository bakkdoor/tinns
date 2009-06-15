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

void PCommands::doCmdbrightness()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@brightness -|<head brightness: 0..255> [-|<torso brightness>]  [-|<legs brightness>]");
        return;
    }

    char tmp_v1[30];
    char tmp_v2[30];
    char tmp_v3[30];

    GetArgText(1, tmp_v1, 30);
    GetArgText(2, tmp_v2, 30);
    GetArgText(3, tmp_v3, 30);

    u8 val1, val2, val3, val4, val5, val6;
    char effStr[128];
    PMessage* tmpMsg;

    source->GetChar()->GetCurrentBodyColor(val1, val2, val3, val4, val5, val6);
    if(tmp_v1[0] != '-')
    val4 = (u8)(atoi(tmp_v1) & 0xff);
    if((tmp_v2[0] != '-') && (tmp_v2[0] != '\0'))
    val5 = (u8)(atoi(tmp_v2) & 0xff);
    if((tmp_v3[0] != '-') && (tmp_v3[0] != '\0'))
    val6 = (u8)(atoi(tmp_v3) & 0xff);
    source->GetChar()->SetCurrentBodyColor(val1, val2, val3, val4, val5, val6);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Body brightness set to values %d %d %d", val4, val5, val6);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}

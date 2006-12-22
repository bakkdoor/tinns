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

void PCommands::doCmdspeed()
{
// Speed override setting.
// Usage: @speed <newspeed> | #
//  with <speed> = 0 (no move).. 254 , 255 or # meaning "no speed override"

    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@speed <newspeed: 0 .. 254 > | 255 | #");
        return;
    }
    char tmpval[30];
    GetArgText(1, tmpval, 30);

    u8 val1;
    char effStr[128];
    PMessage* tmpMsg;

    val1 = ((tmpval[0] == '#') ? 255 : (u8)(atoi(tmpval) & 0xff));
    source->GetChar()->SetSpeedOverride(val1);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Speed override set to value %d ", val1);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}

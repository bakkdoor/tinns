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

void PCommands::doCmdeffect()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == false)
        SyntaxError = true;

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@effect <effect: 0=none, 1 .. 17> [<density: 0=max .. 255=min>]");
        return;
    }

    u8 val1, val2;
    char effStr[128];
    PMessage* tmpMsg;

    val1 = (u8)(GetArgInt(1) & 0xff);
    val2 = (u8)(GetArgInt(2) & 0xff);

    source->GetChar()->SetBodyEffect(val1, val2);

    tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(effStr, 127, "Body effect set to value %d with density %d (but you can see it yourself)", val1, val2);
    effStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", effStr);
}

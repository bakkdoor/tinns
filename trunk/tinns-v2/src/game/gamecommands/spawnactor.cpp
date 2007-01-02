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

void PCommands::doCmdspawnactor()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == false || IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }

    u16 tmpActorID = (u16)GetArgInt(1);
    u16 tmpFunctionID = (u16)GetArgInt(2);

    if(tmpActorID == 0)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@spawnactor <actorID> <functionID>");
        return;
    }

    PMessage* tmpMsg = MsgBuilder->BuiltSpawnObjectMsg(source, tmpActorID, tmpFunctionID, mWOID++);
    ClientManager->UDPBroadcast(tmpMsg, source);
    tmpMsg = NULL;
}

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

void PCommands::doCmdremove()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@remove actor/<raw item id>");
        return;
    }

    char tmp_v1[30];
    GetArgText(1, tmp_v1, 30);

    if(strcmp(tmp_v1, "actor") == 0)
    {
        if(source->IsInRemoveActorMode() == false)
        {
            source->SetRemoveActorMode(true);
            Chat->send(source, CHAT_DIRECT, "System", "You are now in REMOVE ACTOR mode. Rightclick an actor to remove it, type command again to disable mode");
        }
        else
        {
            source->SetRemoveActorMode(false);
            Chat->send(source, CHAT_DIRECT, "System", "REMOVE ACTOR mode disabled");
        }
    return;
    }

    u32 TargetID;
    char delStr[128];
    PMessage* tmpMsg;

    TargetID = (u32)(atoi(tmp_v1) & 0xffffffff);
    tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(source, TargetID, 5);

    ClientManager->UDPBroadcast(tmpMsg, source);
    tmpMsg = MsgBuilder->BuildFurnitureActivateMsg(source, TargetID, 9);

    ClientManager->UDPBroadcast(tmpMsg, source);
    snprintf(delStr, 127, "Item %d removed.", TargetID);
    delStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", delStr);
}

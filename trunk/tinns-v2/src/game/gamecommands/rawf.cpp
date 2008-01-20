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

void PCommands::doCmdrawf()
{
    bool SyntaxError = false;
    bool DebugMode = false;
    int Prot = 0;  // 0: Error 1: UDP 2: TCP

    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    char file_to_send[50], protocol_to_use[10];
    GetArgText(1, file_to_send, 50);
    GetArgText(2, protocol_to_use, 10);

    if(strcmp(protocol_to_use, "udp") == 0)
    {
        Prot = 1;
    }
    else if(strcmp(protocol_to_use, "tcp") == 0)
    {
        Prot = 2;
    }
    else if(strcmp(protocol_to_use, "debug") == 0)
    {
        DebugMode = true;
    }
    else
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@rawf <file> <udp/tcp/debug>");
        return;
    }

    ifstream::pos_type size;
    char *buffer;

    ifstream hexdump (file_to_send, ios::in|ios::binary|ios::ate);
    if (hexdump.is_open())
    {
        if (gDevDebug) Console->Print("IngameCommand: Sending packet file %s", file_to_send);

        size = hexdump.tellg();
        buffer = new char [size];
        hexdump.seekg (0, ios::beg);

        hexdump.read (buffer, size);
        hexdump.close();
        if(DebugMode == true)
        {
            int k;
            Console->Print("Byte dump of %s:", file_to_send);
            for(k=0;k<size;k++)
            {
                Console->Print("Byte %d: %#x", k, buffer[k]);
            }
        }
        else
        {
            if(Prot == 1)
                source->getUDPConn()->write(buffer, size);
            else if(Prot == 2)
                source->getTCPConn()->write(buffer, size);
            else // Should never happen...
                Console->Print("%s unable to determine protocol in PCommands::doCmdrawf", Console->ColorText(RED, BLACK, "[Error]"));
        }

        delete[] buffer;
    }
    else
    {
        char output[100];
        Console->Print("IngameCommand: Unable to send file %s", file_to_send);
        snprintf(output, 100, "File not found: %s", file_to_send);
        Chat->send(source, CHAT_DIRECT, "System", output);
    }
}

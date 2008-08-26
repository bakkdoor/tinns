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

void PCommands::doCmdskin()
{
// -------------------------------------------------------
/*
Usage:  @skin #<chardef idx>
              for use with the index id from characters.def
  or  @skin <model>|-|# [<head>[ <torso>[ <legs>]]]
              <model> is the model id found after the model name in characters.def
              # resets to real char skin (optionnaly modified by following args)
              - means current skin
              incrementaly optional <head>, <torso> and <legs> are values 0-9
*/
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    u32 Skinval1, Skinval2, Skinval3, Skinval4;
    PChar *SkinChar = Chars->GetChar(source->GetCharID());
    std::stringstream SkinChat;
    char SkinStr[128];

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@skin ( #<chardef idx> ) | ( <model> | # [<head>[ <torso>[ <legs>]]] )");
        return;
    }

    char tmpval[30];
    GetArgText(1, tmpval, 30);
    if((tmpval[0] == '#') && (tmpval[1] != '\0'))
    {
        Skinval1 = atoi(tmpval+1);
        SkinChar->SetCurrentLookFromCharType(atoi(tmpval+1));
        SkinChat << "Skin set to the skin of char type ";
        SkinChat << (int)Skinval1;
    }
    else
    {
        if ((tmpval[0] == '#') && (tmpval[1] == '\0'))
        {
            SkinChar->GetRealLook(Skinval1, Skinval2, Skinval3, Skinval4);
        }
        else if ((tmpval[0] == '-') && (tmpval[1] == '\0'))
        {
            SkinChar->GetCurrentLook(Skinval1, Skinval2, Skinval3, Skinval4);
        }
        else
        {
            Skinval1 = GetArgInt(1);
        }

        char tmp_arg2[30];
        char tmp_arg3[30];
        char tmp_arg4[30];

        GetArgText(2, tmp_arg2, 30);
        GetArgText(3, tmp_arg3, 30);
        GetArgText(4, tmp_arg4, 30);

        if(tmp_arg2[0] != '\0')
        {
            if(tmp_arg2[0] != '-')
            {
                tmp_arg2[1] = '\0';
                Skinval2 = GetArgInt(2);
            }
            if(tmp_arg3[0] != '\0')
            {
                if(tmp_arg3[0] != '-')
                {
                    tmp_arg3[1] = '\0';
                    Skinval3 = GetArgInt(3);
                }
                if(tmp_arg4[0] != '\0')
                {
                    if(tmp_arg4[0] != '-')
                    {
                        tmp_arg4[1] = '\0';
                        Skinval4 = GetArgInt(4);
                    }
                }
            }
        }

        SkinChar->SetCurrentLook(Skinval1, Skinval2, Skinval3, Skinval4);

        SkinChat << "Skin set to model ";
        SkinChat << (int)Skinval1 << " with head " << (int)Skinval2 << ", torso " << (int)Skinval3 << ", legs " << (int)Skinval4;
    }

    snprintf(SkinStr, 127, "%s", SkinChat.str().c_str());
    SkinStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", SkinStr);

    PMessage* tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
    ClientManager->UDPBroadcast(tmpMsg, source);
}

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

/*

	udp_chat.h - decoder classes for UDP chat  messages
  
	CREATION: 15 Sep 2006 Hammag

	MODIFIED:
	REASON: - 

*/

#ifndef UDPCHAT_H
#define UDPCHAT_H

class PUdpChatLocal : public PUdpMsgAnalyser
{ 
  public:
    PUdpChatLocal(PMsgDecodeData* nDecodeData);
    //~PUdpChatLocal();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpChatGlobal : public PUdpMsgAnalyser
{
  private:
    
  public:
    PUdpChatGlobal(PMsgDecodeData* nDecodeData);
    //~PUdpChatGlobal();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpChatListAdd : public PUdpMsgAnalyser
{
  private:
    u8 mChatList; // 1 = Direct, 2 = Buddy List
    char* mAddedCharname;
    
  public:
    PUdpChatListAdd(PMsgDecodeData* nDecodeData);
    //~PUdpChatListAdd();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpChatListRemove : public PUdpMsgAnalyser
{
  private:
    u8 mChatList;
    u32 mRemovedCharID;
    
  public:
    PUdpChatListRemove(PMsgDecodeData* nDecodeData);
    //~PUdpChatListRemove();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

#endif

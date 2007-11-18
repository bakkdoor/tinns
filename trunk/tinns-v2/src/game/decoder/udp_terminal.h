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

	udp_terminal.h - decoder classes for UDP terminal related messages

	CREATION: 8 Jan 2007 Namikon

	MODIFIED:
	REASON: -

*/

#ifndef UDPTERMINAL_H
#define UDPTERMINAL_H



class PUdpReceiveDB : public PUdpMsgAnalyser
{
  private:
    static const u8 mMaxOptions = 3;
    u16 mTerminalSessionId;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    u8 mOptionsCount;
    
    u16 mUnknown1;
    u16 mUnknown2;
    
  public:
    PUdpReceiveDB(PMsgDecodeData* nDecodeData);
    //~PUdpReceiveDB();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpUpdateDB : public PUdpMsgAnalyser
{
  public:
    PUdpUpdateDB(PMsgDecodeData* nDecodeData);
    //~PUdpUpdateDB();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpTryAccessDB : public PUdpMsgAnalyser
{
  public:
    PUdpTryAccessDB(PMsgDecodeData* nDecodeData);
    //~PUdpTryAccessDB();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpQueryDB : public PUdpMsgAnalyser
{
  private:
    static const u8 mMaxOptions = 3;
    u16 mTerminalSessionId;
    std::string mDBCommandName;
    std::string mCommandName;
    std::string mOptions[mMaxOptions];
    u8 mOptionsCount;
  
  public:
    PUdpQueryDB(PMsgDecodeData* nDecodeData);
    //~PUdpQueryDB();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpTeminal0x1f : public PUdpMsgAnalyser
{
  private:
    u16 mTerminalSessionId;
    
  public:
    PUdpTeminal0x1f(PMsgDecodeData* nDecodeData);
    //~PUdpTeminal0x1f();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};


#endif

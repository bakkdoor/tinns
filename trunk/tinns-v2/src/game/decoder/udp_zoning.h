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

	udp_zoning.h - decoder classes for UDP Zoning messages
  
	CREATION: 6 Sep 2006 Hammag

	MODIFIED:
	REASON: - 

*/

#ifndef UDPZONING_H
#define UDPZONING_H

class PUdpZoning1 : public PUdpMsgAnalyser
{ 
  public:
    PUdpZoning1(PMsgDecodeData* nDecodeData);
    //~PUdpZoning1();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpZoning2 : public PUdpMsgAnalyser
{ 
  public:
    PUdpZoning2(PMsgDecodeData* nDecodeData);
    //~PUdpZoning2();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpGenrepZoning : public PUdpMsgAnalyser
{ 
  public:
    PUdpGenrepZoning(PMsgDecodeData* nDecodeData);
    //~PUdpGenrepZoning();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpAptGRZoning : public PUdpMsgAnalyser
{ 
  public:
    PUdpAptGRZoning(PMsgDecodeData* nDecodeData);
    //~PUdpAptGRZoning();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpAddGenrepToList : public PUdpMsgAnalyser
{
  private:
    u32 mLocation;
    u16 mEntity;
    
  public:
    PUdpAddGenrepToList(PMsgDecodeData* nDecodeData);
    //~PUdpAddGenrepToList();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpAppartmentAccess : public PUdpMsgAnalyser
{
  private:
    u16 mAppartmentPlace;
    char* mPassword;
    
  public:
    PUdpAppartmentAccess(PMsgDecodeData* nDecodeData);
    //~PUdpAppartmentAccess();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

#endif

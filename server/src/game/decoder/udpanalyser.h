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

	udpanalyser.h - Analyser class top (used only by PUdpMsgDecoder)
  Also includes the PUdpMsgUnknown derived class
  and the PUdpMsgIgnore derived class
  
	CREATION: 23 Aug 2006 Hammag

	MODIFIED: 30 Aug 2006 Hammag
	REASON: - moved these two class in file distinct from PUdpMsgDecoder.

*/

#ifndef UDPANALYSER_H
#define UDPANALYSER_H

class PUdpMsgAnalyser
{
  friend class PUdpMsgDecoder;
  
	protected:
    PMsgDecodeData* mDecodeData;

  public:	  
    PUdpMsgAnalyser(PMsgDecodeData* nDecodeData);
    virtual ~PUdpMsgAnalyser();
    
    virtual PUdpMsgAnalyser* Analyse();
    virtual bool DoAction();
    
    static int smAnaCount; // temp for check

};

class PUdpMsgUnknown : public PUdpMsgAnalyser
{ 
  public:
    PUdpMsgUnknown(PMsgDecodeData* nDecodeData);
    //~PUdpMsgUnknown();
    
    //bool DoAction();
};

class PUdpMsgIgnore : public PUdpMsgAnalyser
{ 
  public:
    PUdpMsgIgnore(PMsgDecodeData* nDecodeData);
    //~PUdpMsgUnknown();
    
    //bool DoAction();
};

#endif

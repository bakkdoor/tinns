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

	udp_popupresponse.cpp - decoder classes for some UDP Popup response messages

	CREATION: 14 Apr 2009 Hammag
*/


#ifndef UDPPOPUPRESP_H
#define UDPPOPUPRESP_H

class PUdpPopupResponse : public PUdpMsgAnalyser
{
  public:
    PUdpPopupResponse(PMsgDecodeData* nDecodeData);
    //~PUdpPopupResponse();
    PUdpMsgAnalyser* Analyse();
    //bool DoAction();
};

class PUdpVentureWarpConfirm : public PUdpMsgAnalyser
{
  private:
    u32 mRawItemId;
    u16 mStatus;
    u32 mUnknown1;
	u16 mUnknown2;
  public:
    PUdpVentureWarpConfirm(PMsgDecodeData* nDecodeData);
    //~PUdpVentureWarpConfirm();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

class PUdpVhcAccessResponse : public PUdpMsgAnalyser
{
  private:
    u32 mVhcAccessRequestId;
    u16 mStatus;
	u16 mUnknown;
  public:
    PUdpVhcAccessResponse(PMsgDecodeData* nDecodeData);
    //~PUdpVhcAccessResponse();
    PUdpMsgAnalyser* Analyse();
    bool DoAction();
};

#endif

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
	terminal.h - Management class for Terminal actions (Citycom, keys, vehicledepot, ...)

	MODIFIED: 08 Jan 2007 Namikon
	REASON: - Created

*/

#ifndef TERMINAL_H
#define TERMINAL_H

class PTerminal
{
    private:
        void SendTryAccessAnswer(PClient* nClient, char *nArea, bool nAllowed);
        char mSQLQuery[500];

        int mResultFields;
        void EraseVars();

        char mConPrefix[50];

        inline bool ChkOpt(u8 nNumOptions, u8 nReqOpt) { if(nNumOptions < nReqOpt) return false; else return true; };
        bool DoStockXCheck(PClient* nClient, int nAmountEntered, int nNewAmount);

    public:
        PTerminal();
        //~PTerminal();

        // Check accesslevel of Player for various Terminal actions
        bool CheckAccess(PClient* nClient, char *nArea, u16 nCmdNr, char *nOption1, char *nOption2, char *nOption3);
        u8 GetNewEmailCount(PClient* nClient, bool nNoticeClient = true);
        // Handle ReceiveDB queries
        bool HandleQueryDB(PClient* nClient, std::string *nDBCommandName, std::string *nCommandName, std::string *nOptions, u8 nNumOptions);
        bool HandleReceiveDB(PClient* nClient, u16 mTerminalSessionId, std::string *nCommandName, std::string *nOptions, u8 nNumOptions, u16 nDBID, u8 nUnknown);
        bool HandleTryAccess(PClient* nClient, u16 mTerminalSessionId, std::string *nCommandName, std::string *nOptions, u8 nNumOptions, u16 nDBID, u8 nUnknown, bool nCheckOnly = false);
        bool HandleUpdateDB(PClient* nClient, u16 mTerminalSessionId, std::string *nCommandName, std::string *nOptions, u8 nNumOptions, u16 nDBID, u8 nUnknown);
};

#endif

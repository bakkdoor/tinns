/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  appartements.h - appartements class

	MODIFIED: 20 Nov 2006 Hammag
	REASON: - creation
	
*/

#ifndef APPARTEMENTS_H
#define APPARTEMENTS_H

class PChar;

class PAppartements
{
      private:
        
      public:
        PAppartements();
        //~PAppartements();
        
        u32 CreateBaseAppartement(u32 nCharID, std::string nPassword, u8 nFactionID);
        void DeleteCharAppartements(u32 nCharID);
        bool CanFreelyEnter(PChar* nChar, u32 nLocation);
};

#endif

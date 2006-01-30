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
	def_items.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef DEF_ITEMS_H
#define DEF_ITEMS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefItems
{
	private :

		int mIndex;//1
        std::string mName;//2
		int mUnknow03;
		int mUnknow04;
		int mUnknow05;
		int mUnknow06;
		int mUnknow07;
		int mUnknow08;
		int mUnknow09;
		int mUnknow10;
		int mUnknow11;
		int mUnknow12;
		int mUnknow13;
		int mUnknow14;
		int mUnknow15;
		int mUnknow16;
        int mUnknow17;
		int mUnknow18;
		int mUnknow19;
		int mUnknow20;
		int mUnknow21;
		int mUnknow22;
		int mUnknow23;

	public :
		PDefItems();
		~PDefItems();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }

};

#endif


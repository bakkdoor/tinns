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
	def_worldfile.h (infos from worlds/worlds.ini)

	MODIFIED: 28 Sep 2007 Hammag
	REASON: - Creation
*/

#ifndef DEF_WORLDFILE_H
#define DEF_WORLDFILE_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefWorldFile
{
	private :
		int mIndex;
		std::string mName; // dat filename with ending extension and starting ./ or ./worlds/ REMOVED
		bool mFileInWorldsDir; // TRUE if worlds/ must be appendend before mName to get real file name (dat file at least)
		
	public :
		PDefWorldFile();
		~PDefWorldFile();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string GetBasicFileName() const { return (mFileInWorldsDir ? (std::string("worlds/") + mName) : mName); };
};


class PDefWorldFilesMap : public PDefMap<PDefWorldFile>
{
  public:
    bool Load(const char* nName, const char* nFilename);
    inline std::map<int, PDefWorldFile*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int, PDefWorldFile*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};

#endif

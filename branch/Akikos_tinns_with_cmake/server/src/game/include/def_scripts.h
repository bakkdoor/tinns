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
 def_scripts.h

    CREATED: 12 Oct 2009 Namikon
*/

#ifndef DEF_SCRIPTS_H
#define DEF_SCRIPTS_H

#include "def.h"

class PDefScripts : public PDef
{
  private :
    //int mIndex;
    std::string mIdentifier;
    std::string mLuaFile;
    std::string mScriptHeader;

  public :
    PDefScripts();
    //~PDefWeapon();

    bool LoadFromDef( PTokenList *Tokens );

    inline const std::string &GetIdentifier() const { return mIdentifier; }
    inline const std::string &GetLuaFile() const { return mLuaFile; }
    inline const std::string &GetScriptHeader() const { return mScriptHeader; }
};

class PDefScriptsMap : public PDefMap<PDefScripts>
{
  public:
    //bool Load(const char* nName, const char* nFilename);
    inline std::map<int, PDefScripts*>::const_iterator ConstIteratorBegin() const { return mDefs.begin(); }
    inline std::map<int, PDefScripts*>::const_iterator ConstIteratorEnd() const { return mDefs.end(); }
};

#endif

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
 def.h

    CREATED: 03 Apr 2009 Hammag
*/

#ifndef DEF_H
#define DEF_H

extern const std::string EmptyString;

/* mother class for PDef* classes */
/* provides default members required for def file loading */
class PDef
{
  protected :
    int mIndex;

  public :
    PDef() : mIndex(0) {};
    //~PDef() {};

    inline bool LoadFromDef( PTokenList *Tokens ) { Tokens = Tokens; return false; };

    inline int GetIndex() const { return mIndex; }
    inline const std::string &GetName() const { return EmptyString; }
};

#endif


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
	MODIFIED: 01 Jul 2006 hammag
	REASON: - add IPlongToString()
	
*/

#ifndef MISC_H
#define MISC_H

#ifdef _MSC_VER
	#pragma once
#endif

u32 IPStringToDWord(const char *IP);
char *IPlongToString(const u32 IP);
std::string GetAccessString(int level);
void GetSVNRev(char *version);

void Trim(char *t);
void Trim(std::string *stString);
void RTrim(char *t);
void RTrim(std::string *stString);
void LTrim(char *t);
void LTrim(std::string *stString);
#endif


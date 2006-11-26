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
	misc.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
	MODIFIED: 07 Jan 2006 Namikon
	REASON: - Added function to trim a string/char
	MODIFIED: 27 Aug 2006 Hammag
	REASON: - Merged misc function from all 3 servers
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

void PrintPacket(u8 *Packet, int PacketSize);

void Trim(char *t);
void Trim(std::string *stString);
void RTrim(char *t);
void RTrim(std::string *stString);
void LTrim(char *t);
void LTrim(std::string *stString);
std::string &Ssprintf(const char *fmt, ...);

u16 DistanceApprox(const u16 x1, const u16 y1, const u16 z1, const u16 x2, const u16 y2, const u16 z2);
f32 Distance(const f32 x1, const f32 y1, const f32 z1, const f32 x2, const f32 y2, const f32 z2);
f32 Distance(const f32 x1, const f32 y1, const f32 x2, const f32 y2); // 2D only version

#endif


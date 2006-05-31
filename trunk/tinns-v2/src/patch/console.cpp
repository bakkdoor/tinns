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

#include "main.h"

PConsole::PConsole()
{
	std::time(&mLastLogTime);
	mLogFile.open("log/patchserver.log");
}

PConsole::~PConsole()
{
    Print("%s Shutdown complete", Console->ColorText(GREEN, BLACK, "[Done]"));
	mLogFile.close();
}

void PConsole::Print(const char *Fmt, ...)
{
	static char Str[2048];
	va_list args;
	va_start(args, Fmt);
	vsnprintf(Str, 2047, Fmt, args);
	va_end(args);

	std::time(&mLastLogTime);
	std::tm *now = std::localtime(&mLastLogTime);

	static char datestr[64];
	std::sprintf(datestr, "%02i/%02i %02i:%02i:%02i ", now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	std::stringstream str;
	str << datestr << Str << std::endl;

	std::printf("%s", str.str().c_str());
	mLogFile << str.str();
	mLogFile.flush();
}

void PConsole::Print(COLORS foreground, COLORS background, const char *Fmt, ...)
{
	static char Str[2048];
	va_list args;
	va_start(args, Fmt);
	vsnprintf(Str, 2047, Fmt, args);
	va_end(args);

    char c_color[13];
    char c_reset[13];
    std::sprintf(c_color, "%c[%d;%d;%dm", 0x1B, 0, foreground + 30, background + 40);
    std::sprintf(c_reset, "%c[%d;%d;%dm", 0x1B, 0, 37, 40);

	std::time(&mLastLogTime);
	std::tm *now = std::localtime(&mLastLogTime);

	static char datestr[64];
	std::sprintf(datestr, "%02i/%02i %02i:%02i:%02i ", now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	std::stringstream str;
	str << datestr << c_color << Str << c_reset << std::endl;

	std::printf("%s", str.str().c_str());
	mLogFile << str.str();

	mLogFile.flush();
}

char *PConsole::ColorText(COLORS foreground, COLORS background, const char *Fmt, ...)
{
	static char Str[2048];
	va_list args;
	va_start(args, Fmt);
	vsnprintf(Str, 2047, Fmt, args);
	va_end(args);

    char c_color[13];
    char c_reset[13];
    std::sprintf(c_color, "%c[%d;%d;%dm", 0x1B, 0, foreground + 30, background + 40);
    std::sprintf(c_reset, "%c[%d;%d;%dm", 0x1B, 0, 37, 40);

    static char returnbuffer[2048];
	strcpy (returnbuffer, c_color);
	strcat (returnbuffer, Str);
	strcat (returnbuffer, c_reset);

	return returnbuffer;
}

void PConsole::LPrint(const char *Fmt, ...)
{
	static char Str[2048];
	va_list args;
	va_start(args, Fmt);
	vsnprintf(Str, 2047, Fmt, args);
	va_end(args);

	std::time(&mLastLogTime);
	std::tm *now = std::localtime(&mLastLogTime);

	static char datestr[64];
	std::sprintf(datestr, "%02i/%02i %02i:%02i:%02i ", now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
	std::stringstream str;
	str << datestr << Str;

	std::printf("%s", str.str().c_str());
	mLogFile << str.str();
	mLogFile.flush();
}

void PConsole::LPrint(COLORS foreground, COLORS background, const char *Fmt, ...)
{
    static char Str[2048];
	va_list args;
	va_start(args, Fmt);
	vsnprintf(Str, 2047, Fmt, args);
	va_end(args);

    char c_color[13];
    char c_reset[13];
    std::sprintf(c_color, "%c[%d;%d;%dm", 0x1B, 0, foreground + 30, background + 40);
    std::sprintf(c_reset, "%c[%d;%d;%dm", 0x1B, 0, 37, 40);

	std::stringstream str;
	str << c_color << Str << c_reset;

	std::printf("%s", str.str().c_str());
	mLogFile << str.str();

	mLogFile.flush();
}

void PConsole::LClose()
{
	std::stringstream str;
	str << std::endl;

	std::printf("%s", str.str().c_str());
	mLogFile << str.str();

	mLogFile.flush();
}

void PConsole::Update()
{
	// place a marker into the log each 15 minutes if no output has been generated
	std::time_t t;
	std::time(&t);
	if(std::difftime(t, mLastLogTime) >= 900)
		Print("--MARK--");
}

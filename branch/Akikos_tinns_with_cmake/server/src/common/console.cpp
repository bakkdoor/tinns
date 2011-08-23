#include "common/console.h"

#include <sstream>
#include <cstring>
#include <cstdarg>

PConsole::PConsole(const char *nLogFile)
{
	std::time(&mLastLogTime);
	mLogFile.open(nLogFile);
}

PConsole::~PConsole()
{
    Print("%s Shutdown complete", ColorText(GREEN, BLACK, "[Done]"));
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
	std::snprintf(datestr, 64, "%02i/%02i %02i:%02i:%02i ", now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
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
    std::snprintf(c_color, 13, "%c[%d;%d;%dm", 0x1B, 0, foreground + 30, background + 40);
    std::snprintf(c_reset, 13, "%c[%d;%d;%dm", 0x1B, 0, 37, 40);

	std::time(&mLastLogTime);
	std::tm *now = std::localtime(&mLastLogTime);

	static char datestr[64];
	std::snprintf(datestr, 64, "%02i/%02i %02i:%02i:%02i ", now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
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
  std::snprintf(c_color, 13, "%c[%d;%d;%dm", 0x1B, 0, foreground + 30, background + 40);
  std::snprintf(c_reset, 13, "%c[%d;%d;%dm", 0x1B, 0, 37, 40);

  static char returnbuffer[2048];
	strncpy (returnbuffer, c_color, 2048);
	strncat (returnbuffer, Str, 2047 - strlen(returnbuffer));
	strncat (returnbuffer, c_reset, 2047 - strlen(returnbuffer));

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
	std::snprintf(datestr, 64, "%02i/%02i %02i:%02i:%02i ", now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
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
    std::snprintf(c_color, 13, "%c[%d;%d;%dm", 0x1B, 0, foreground + 30, background + 40);
    std::snprintf(c_reset, 13, "%c[%d;%d;%dm", 0x1B, 0, 37, 40);

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

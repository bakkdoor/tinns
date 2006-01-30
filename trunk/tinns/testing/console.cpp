#include "tinns.h"

PConsole::PConsole()
{
	std::time(&mLastLogTime);
	mLogFile.open(PATH_LOG);
}

PConsole::~PConsole()
{
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

void PConsole::Update()
{
	// place a marker into the log each 15 minutes if no output has been generated
	std::time_t t;
	std::time(&t);
	if(std::difftime(t, mLastLogTime) >= 900)
		Print("--MARK--\n");
}

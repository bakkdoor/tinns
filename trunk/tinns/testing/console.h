#ifndef CONSOLE_H
#define CONSOLE_H

class PConsole
{
	private :
		std::ofstream mLogFile;
		std::time_t mLastLogTime;
	public :
		PConsole();
		~PConsole();
		void Print(const char *Fmt_, ...);
		void Update();
};

#endif


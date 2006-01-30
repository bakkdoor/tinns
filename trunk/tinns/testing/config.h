#ifndef CONFIG_H
#define CONFIG_H

#ifdef _MSC_VER
	#pragma once
#endif

class PConfig
{
	private :
		typedef std::map<std::string, std::string> OptionsMap;
		OptionsMap mOptions;
	public :
		PConfig();
		~PConfig();
		
		bool LoadOptions();
		const std::string &GetOption(const char *Name) const;
		int GetOptionInt(const char *Name) const;
};

#endif


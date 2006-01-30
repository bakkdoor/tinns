#include "tinns.h"

PConfig::PConfig()
{
}

PConfig::~PConfig()
{
}

bool PConfig::LoadOptions()
{
	Console->Print("Loading configuration file...");
	TiXmlDocument doc("./database/config.xml");
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			TiXmlElement *op = Root->FirstChildElement("option");
			while(op)
			{
				const char *name = op->Attribute("name");
				const char *value = op->Attribute("value");
				if(name && value)				
					mOptions.insert(std::make_pair(name, value));
				
				op = op->NextSiblingElement("option");
			}
		
			return true;
		}
	} 
	
	// no config file or no options
	Console->Print("Config error: No config found");
	return false;
}

const std::string &PConfig::GetOption(const char *Name) const
{
	static std::string NullString = "";
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return i->second;
	
	return NullString;
}

int PConfig::GetOptionInt(const char *Name) const
{
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return std::atoi(i->second.c_str());
		
	return 0;
}


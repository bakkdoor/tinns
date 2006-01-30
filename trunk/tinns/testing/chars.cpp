#include "tinns.h"

PChar::PChar()
{
	mID = 0;
	mAccount = 0;
	mProfession = 1;
	mFaction = 1;
	mModel = 0;
	mType = 0;
	mLocation = 1;
	
	mDirtyFlag = false;
}

bool PChar::Load(TiXmlElement *Node)
{
	if(!Node)
		return false;

	const char *name = Node->Attribute("name");
	const char *id = Node->Attribute("id");
	
	if(name && id)
	{
		SetID(std::atoi(id));
		SetName(name);
		
		std::stringstream fname;
		fname << "./database/playerchars/" << GetID() << ".xml" << '\0';
		
		bool Error=false;
		TiXmlDocument doc(fname.str().c_str());
		if(doc.LoadFile())
		{
			TiXmlElement *Root = doc.RootElement();
			if(Root)
			{
				TiXmlElement *profession = Root->FirstChildElement("profession");
				TiXmlElement *faction = Root->FirstChildElement("faction");
				TiXmlElement *model = Root->FirstChildElement("model");
				TiXmlElement *type = Root->FirstChildElement("type");
				TiXmlElement *location = Root->FirstChildElement("location");
				
				if(profession)
				{
					int profvalue = 0;
					profession->Attribute("value", &profvalue);
					if(GameDefs->GetCharDef(profvalue))
						mProfession = static_cast<u32>(profvalue);
					else
						mProfession = 1;
				}
				
				if(faction)
				{
					int facvalue = 0;
					faction->Attribute("value", &facvalue);
					if(GameDefs->GetFactionDef(facvalue))
						mFaction = static_cast<u32>(facvalue);
					else
						mFaction = 1;
				}
				
				if(model)
				{
					int modvalue = 0;
					model->Attribute("value", &modvalue);
					mModel = static_cast<u32>(modvalue);
				}
				
				if(type)
				{
					int typevalue = 0;
					type->Attribute("value", &typevalue);
					mType = static_cast<u32>(typevalue);
				}
				
				if(location)
				{
					int locvalue = 1;
					location->Attribute("value", &locvalue);//NEW corrected
					mLocation = static_cast<u32>(locvalue);
				}
				
			} else
				Error=true;
		} else
			Error = true;
		
		if(Error)
		{
			Console->Print("PChar: could not load char %s (%d)", mName.c_str(), mID);
			return false;
		}
		return true;
	}
	
	return false;
}

void PChar::Save()
{
	std::stringstream fname;
	fname << "./database/playerchars/" << GetID() << ".xml" << '\0';
	std::stringstream tempname;
	tempname << "./database/playerchars/" << GetID() << ".tmp" << '\0';

	std::stringstream bakname;
	bakname << "./database/playerchars/" << GetID() << ".bak" << '\0';
	
	std::remove(tempname.str().c_str());
	
	TiXmlDocument doc;
	TiXmlElement Root("playerchar");
	Root.SetAttribute("id", GetID());
	Root.SetAttribute("name", mName.c_str());
	
	TiXmlElement type("type");
	type.SetAttribute("value", mType);
	Root.InsertEndChild(type);
	
	TiXmlElement prof("profession");
	prof.SetAttribute("value", mProfession);
	Root.InsertEndChild(prof);
	
	TiXmlElement fac("faction");
	fac.SetAttribute("value", mFaction);
	Root.InsertEndChild(fac);
	
	TiXmlElement model("model");
	model.SetAttribute("value", mModel);
	Root.InsertEndChild(model);
	
	TiXmlElement loc("location");
	loc.SetAttribute("value", mLocation);
	Root.InsertEndChild(loc);
	
	doc.InsertEndChild(Root);
	if(doc.SaveFile(tempname.str().c_str()))
	{	
		std::remove(bakname.str().c_str());
		std::rename(fname.str().c_str(), bakname.str().c_str());
		std::rename(tempname.str().c_str(), fname.str().c_str());
	} else
	{
		Console->Print("PChar: could not save char %s (%d)", mName.c_str(), mID);
	}
	
	mDirtyFlag = false;
}

// ===================================

PChars::PChars()
{
	mLastID = 0;
}

PChars::~PChars()
{
	for(CharMap::iterator i=mChars.begin(); i!=mChars.end(); i++)
		delete i->second;	
}

void PChars::Load()
{
	Console->Print("Loading player chars...");
	int nChars=0;
	TiXmlDocument doc("./database/chars.xml");
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			TiXmlElement *List = Root->FirstChildElement("charlist");
			while(List)
			{
				int AccId = 0;
				List->Attribute("account", &AccId);
				if(AccId != 0)
				{
					PAccount *Account = Database->GetAccount(AccId);
					if(Account)
					{
						TiXmlElement *Ch = List->FirstChildElement("char");
						while(Ch)
						{
							PChar *info = new PChar();
							if(info->Load(Ch))
							{
								info->SetAccount(Account->GetID());
								mLastID = max(mLastID, info->GetID());								
								if(!mChars.insert(std::make_pair(info->GetID(), info)).second)
								{
									Console->Print("Duplicate char id found: %s (%i)", info->GetName().c_str(), info->GetID());
									delete info;
								} else
								{
									if(!Account->AddChar(info->GetID()))
										Console->Print("Could not add char '%s' to account '%s'", info->GetName().c_str(), Account->GetName().c_str());
									++nChars;
								}
							} else
							{
								Console->Print("Invalid char entry found in database. Ignored.");
								delete info;
							}
							
							
							Ch=Ch->NextSiblingElement("char");
						}
					} else
						Console->Print("Ignoring chars of inexistant account %i", AccId);
				}
				
				List = List->NextSiblingElement("charlist");
			}
		}	
	}
	
	Console->Print("Loaded %i player chars", nChars);
	mLastSave = std::clock();
}

void PChars::Save()
{
	// saves all dirty-flagged chars and maintains global character list
	int nChars = 0;
	typedef std::map<u32, std::list<PChar*>*> AccCharList;
	AccCharList CharList;
	for(CharMap::const_iterator i=mChars.begin(); i!=mChars.end(); i++)
	{
		PChar *Char = i->second;
		AccCharList::iterator Acc = CharList.find(Char->GetAccount());
		if(Acc==CharList.end())
		{
			std::list<PChar*> *List = new std::list<PChar*>();
			List->push_back(Char);
			CharList.insert(std::make_pair(Char->GetAccount(), List));
		} else
		{
			Acc->second->push_back(Char);
		}
		if(Char->IsDirty())
		{
			Char->Save();
			++nChars;
		}
	}
	
	TiXmlDocument CList("./database/chars.xml");
	TiXmlElement Root("chars");
	for(AccCharList::const_iterator i = CharList.begin(); i!= CharList.end(); i++)
	{
		TiXmlElement AccList("charlist");
		AccList.SetAttribute("account", i->first);
		for(std::list<PChar*>::const_iterator j=i->second->begin(); j!=i->second->end(); j++)
		{
			TiXmlElement CharElement("char");
			CharElement.SetAttribute("name", (*j)->GetName().c_str());
			CharElement.SetAttribute("id", (*j)->GetID());
			AccList.InsertEndChild(CharElement);
		}
		delete i->second;
		Root.InsertEndChild(AccList);
	}
	CList.InsertEndChild(Root);
	if(!CList.SaveFile())
	{
		Console->Print("ERROR: could not save char list");
	}
	Console->Print("%i characters saved", nChars);
}

PChar *PChars::GetChar(u32 CharID) const
{
	PChar *Result = 0;
	CharMap::const_iterator i = mChars.find(CharID);
	if(i != mChars.end())
		Result = i->second;
	
	return Result;	
}

PChar *PChars::GetChar(const std::string &Name) const
{
	PChar *Result = 0;
	for(CharMap::const_iterator i=mChars.begin(); i!=mChars.end(); i++)
	{
		if(!/*std::*/strcasecmp(i->second->GetName().c_str(), Name.c_str()))
		{
			Result = i->second;
			break;
		}
	}
	return Result;	
}

void PChars::Update()
{
	std::clock_t t = std::clock();
	
	// autosave characters every 10 minutes
	//NEW CHANGED TO 5 min
	if((t-mLastSave)/CLOCKS_PER_SEC >= 300)
	{
		bool NeedSave = false;
		for(CharMap::const_iterator i=mChars.begin(); i!=mChars.end(); i++)
		{
			if(i->second->IsDirty())
			{
				NeedSave=true;
				break;
			}
		}
		
		if(NeedSave)
		{
			Console->Print("Autosaving characters...");
			Save();
			Console->Print("Autosave done.");
		}
		mLastSave = t;
	}
}

PChar *PChars::CreateChar(u32 Account, const std::string &Name, u32 Type, u32 Model)
{
	PChar *Char = new PChar();
	Char->SetName(Name);
	Char->SetModel(Model);
	Char->SetType(Type);
	Char->SetAccount(Account);
	mChars.insert(std::make_pair(++mLastID, Char));
	Char->SetID(mLastID);
	Char->SetDirtyFlag(true);
	Save();
	
	return Char;
}

/*

	gamedefs - this part loads "all" the definitions found in defs/

	MODIFIED: 30 August 2005 Akiko
	REASON: - added this fency header
		- changed the path of every def file to fellow the TinNS way (loading by the config.xml)
		- reformated code for easier reading
	MODIFIED: 31 August 2005 Akiko
	REASON: - updated paths for the def files

	TODO: checking for def files the correct way, that means, first look into the
	      paks, then try to load plain text files from defs directory, then try
	      to load the pak files from the defs directory
*/

#include "tinns.h"

PGameDefs::PGameDefs()
{
}

PGameDefs::~PGameDefs()
{
	for (PDefCharacterMap::iterator i=mCharDefs.begin(); i!=mCharDefs.end(); i++)
		delete i->second;
	for (PDefSkillMap::iterator i=mSkillDefs.begin(); i!=mSkillDefs.end(); i++)
		delete i->second;
	for (PDefSubSkillMap::iterator i=mSubSkillDefs.begin(); i!=mSubSkillDefs.end(); i++)
		delete i->second;

	//NEW TO DO ADD CICLE TO DESTROY
}

bool PGameDefs::LoadCharDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_CHARACTERS;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefCharacter *c = new PDefCharacter();
			bool loadfail = !c->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mCharDefs.insert(std::make_pair(c->GetIndex(), c)).second;
			
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Char def error (duplicate id %i): %s", c->GetIndex(), c->GetName().c_str());
				else
					Console->Print("Char def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete c;
			}
			else
				++nDefs;
		}
	}
	else
	{
		Console->Print("Error loading character defs");

		return (false);
	}

	Console->Print("Loaded %i char defs, %i error(s).", nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadSkillDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_SKILLS;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefSkill *s = new PDefSkill();
			bool loadfail = !s->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mSkillDefs.insert(std::make_pair(s->GetIndex(), s)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Skill def error (duplicate id %i): %s", s->GetIndex(), s->GetName().c_str());
				else
					Console->Print("Skill def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete s;
			}
			else
				++nDefs;
		}
	}
	 else
	{
		Console->Print("Error loading skill defs");

		return (false);
	}
	
	Console->Print("Loaded %i skill defs, %i error(s).", nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadSubSkillDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_SUBSKILLS;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefSubSkill *s = new PDefSubSkill();
			bool loadfail = !s->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mSubSkillDefs.insert(std::make_pair(s->GetIndex(), s)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Subskill def error (duplicate id %i): %s", s->GetIndex(), s->GetName().c_str());
				else
					Console->Print("Subskill def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete s;
			}
			else
				++nDefs;
		}
	}
	else
	{
		Console->Print("Error loading subskill defs");

		return (false);
	}

	Console->Print("Loaded %i subskill defs, %i error(s).", nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadCharKindDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_CHARKINDS;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefCharKind *c = new PDefCharKind();
			bool loadfail = !c->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mCharKindDefs.insert(std::make_pair(c->GetIndex(), c)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Charkind def error (duplicate id %i): %s", c->GetIndex(), c->GetName().c_str());
				else
					Console->Print("Charkind def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete c;
			}
			else
				++nDefs;
		}
	}
	else
	{
		Console->Print("Error loading charkind defs");

		return (false);
	}

	Console->Print("Loaded %i charkind defs, %i error(s).", nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadWorldDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_WORLDINFO;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefWorld *w = new PDefWorld();
			bool loadfail = !w->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mWorldDefs.insert(std::make_pair(w->GetIndex(), w)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("World def error (duplicate id %i): %s", w->GetIndex(), w->GetName().c_str());
				else
					Console->Print("World def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete w;
			}
			else
				++nDefs;
		}
	}
	 else
	{
		Console->Print("Error loading world defs");

		return (false);
	}

	Console->Print("Loaded %i world defs, %i error(s).", nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadFactionDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_FRACTIONS;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefFaction *f = new PDefFaction();
			bool loadfail = !f->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mFactionDefs.insert(std::make_pair(f->GetIndex(), f)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Faction def error (duplicate id %i): %s", f->GetIndex(), f->GetName().c_str());
				else
					Console->Print("Faction def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete f;
			}
			else
				++nDefs;
		}
	}
	else
	{
		Console->Print("Error loading faction defs");

		return (false);
	}

	Console->Print("Loaded %i faction defs, %i error(s).", nDefs, nErrors);

	return (true);
}


/*********************************************///NEW

bool PGameDefs::LoadHackDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_HACK;

	if(parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefHack *h = new PDefHack();
			bool loadfail = !h->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mHackDefs.insert(std::make_pair(h->GetIndex(), h)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					//Console->Print("Hack def error (duplicate id %i): %s", h->GetIndex(), h->GetName().c_str());
					Console->Print("Hack def error (duplicate id %i)", h->GetIndex() );
				else
					Console->Print("Hack def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete h;
			}
			 else
				++nDefs;
		}
	}
	else
	{
		Console->Print("Error loading hack defs");

		return (false);
	}

	Console->Print("Loaded %i hack defs, %i error(s).", nDefs, nErrors);

	return (true);
}

///////////////////////////////
bool PGameDefs::LoadItemsDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs path") + "/" + DEF_ITEMS;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefItems *it = new PDefItems();
			bool loadfail = !it->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mItemsDefs.insert(std::make_pair(it->GetIndex(), it)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Items def error (duplicate id %i): %s", it->GetIndex(), it->GetName().c_str());
				else
					Console->Print("Items def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete it;
			}
			else
				++nDefs;
		}
	}
	else
	{
		Console->Print("Error loading items defs");

		return (false);
	}

	Console->Print("Loaded %i items defs, %i error(s).", nDefs, nErrors);

	return (true);
}

/************************************************///END NEW

const PDefCharacter *PGameDefs::GetCharDef(int Index) const
{
	PDefCharacter *Result = 0;
	
	PDefCharacterMap::const_iterator i = mCharDefs.find(Index);
	if (i!=mCharDefs.end())
		Result = i->second;
		
	return (Result);
}

const PDefSkill *PGameDefs::GetSkillDef(int Index) const
{
	PDefSkill *Result = 0;
	
	PDefSkillMap::const_iterator i = mSkillDefs.find(Index);
	if (i!=mSkillDefs.end())
		Result = i->second;
		
	return (Result);
}

const PDefSubSkill *PGameDefs::GetSubSkillDef(int Index) const
{
	PDefSubSkill *Result = 0;
	
	PDefSubSkillMap::const_iterator i = mSubSkillDefs.find(Index);
	if (i!=mSubSkillDefs.end())
		Result = i->second;
		
	return (Result);
}

const PDefWorld *PGameDefs::GetWorldDef(int Index) const
{
	PDefWorld *Result = 0;
	PDefWorldMap::const_iterator i = mWorldDefs.find(Index);
	if (i!=mWorldDefs.end())
		Result = i->second;
		
	return (Result);
}

const PDefFaction *PGameDefs::GetFactionDef(int Index) const
{
	PDefFaction *Result = 0;
	PDefFactionMap::const_iterator i = mFactionDefs.find(Index);
	if (i!=mFactionDefs.end())
		Result = i->second;
		
	return (Result);
}
/*************************************///NEW
const PDefHack *PGameDefs::GetHackDef(int Index) const
{
	PDefHack *Result = 0;
	PDefHackMap::const_iterator i = mHackDefs.find(Index);
	if (i!=mHackDefs.end())
		Result = i->second;
		
	return (Result);
}

const PDefItems *PGameDefs::GetItemsDef(int Index) const
{
	PDefItems *Result = 0;
	PDefItemsMap::const_iterator i = mItemsDefs.find(Index);
	if (i!=mItemsDefs.end())
		Result = i->second;
		
	return (Result);
}

/****************************************************///END NEW

void PGameDefs::Init()
{
	Console->Print("Initializing gamedefs...");
	LoadWorldDefs();
	LoadSubSkillDefs();
	LoadSkillDefs();
	LoadCharKindDefs();
	LoadCharDefs();
	LoadFactionDefs();

	LoadHackDefs();//NEW
	LoadItemsDefs();//NEW
}


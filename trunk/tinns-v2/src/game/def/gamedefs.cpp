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

	gamedefs - this part loads "all" the definitions found in defs/

	MODIFIED: 30 August 2005 Akiko
	REASON: - added this fency header
		- changed the path of every def file to fellow the TinNS way (loading by the config.xml)
		- reformated code for easier reading
	MODIFIED: 31 August 2005 Akiko
	REASON: - updated paths for the def files
	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
	MODIFIED: 06 Jan 2006
	REASON: - Added GetCharKindDef(), was missing somehow
	MODIFIED: 21 Sep 2005 Hammag
	REASON: - Added PDefWorldModel related stuff
	        - Added PDefAppPlace related stuff
	        - completed PGameDefs destructor
  
  MODIFIED: 22 Sep 2005 Hammag
	REASON: - Added PDefAppartement related stuff
	        - Added PDefRespawn related stuff
	
	
	WARNING:
	When adding new .def support, don't forget to add required stuff in
	  PGameDefs::~PGameDefs() and PGameDefs::Init()
	  (compiler won't complain if you don't add that)
	        
	        	
	NOTA: as PGameDefs uses the PDefParser class, which uses the PFileSystem and PFile classes,
	  files are searched in the same way as the NC client does:
	    - first try to load the file in unpacked form from the given directory,
	    - else tries to load the file in packed form from the given directory,
	        appending pak_ to its name,
	    - else tries to load the packed file from the .pak archive which name is
	        derived from the last part of the path.

*/

#include "main.h"

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
	for (PDefCharKindMap::iterator i=mCharKindDefs.begin(); i!=mCharKindDefs.end(); i++)
		delete i->second;
	for (PDefWorldMap::iterator i=mWorldDefs.begin(); i!=mWorldDefs.end(); i++)
		delete i->second;
	for (PDefFactionMap::iterator i=mFactionDefs.begin(); i!=mFactionDefs.end(); i++)
		delete i->second;
	for (PDefHackMap::iterator i=mHackDefs.begin(); i!=mHackDefs.end(); i++)
		delete i->second;
	for (PDefItemsMap::iterator i=mItemsDefs.begin(); i!=mItemsDefs.end(); i++)
		delete i->second;
	for (PDefWorldModelMap::iterator i=mWorldModelDefs.begin(); i!=mWorldModelDefs.end(); i++)
		delete i->second;		
	for (PDefAppPlaceMap::iterator i=mAppPlaceDefs.begin(); i!=mAppPlaceDefs.end(); i++)
		delete i->second;
	for (PDefAppartementMap::iterator i=mAppartementDefs.begin(); i!=mAppartementDefs.end(); i++)
		delete i->second;
  for (PDefRespawnMap::iterator i=mRespawnDefs.begin(); i!=mRespawnDefs.end(); i++)
		delete i->second;	
}

void PGameDefs::Init()
{
	Console->Print("Initializing gamedefs...");
	LoadWorldDefs();
	LoadSubSkillDefs();
	LoadSkillDefs();
	LoadCharKindDefs();
	LoadCharDefs();
	LoadFactionDefs();
	LoadHackDefs();
	LoadItemsDefs();
	LoadWorldModelDefs();
	LoadAppPlaceDefs();
	LoadAppartementDefs();
	LoadRespawnDefs();
}

/****** Loading methods ******/

bool PGameDefs::LoadCharDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_CHARACTERS;

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
					Console->Print(RED, BLACK, "Char def error (duplicate id %i): %s", c->GetIndex(), c->GetName().c_str());
				else
					Console->Print(RED, BLACK, "Char def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete c;
			}
			else
				++nDefs;
		}
	}
	else
	{
		Console->Print("%s Error loading character defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}

    if(nErrors > 0)
        Console->Print("%s Loaded %i char defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i char defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadSkillDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_SKILLS;

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
					Console->Print(RED, BLACK, "Skill def error (duplicate id %i): %s", s->GetIndex(), s->GetName().c_str());
				else
					Console->Print(RED, BLACK, "Skill def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete s;
			}
			else
				++nDefs;
		}
	}
	 else
	{
	    Console->Print("%s Error loading skill defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}

    if(nErrors > 0)
        Console->Print("%s Loaded %i skill defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i skill defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadSubSkillDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_SUBSKILLS;

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
					Console->Print(RED, BLACK, "Subskill def error (duplicate id %i): %s", s->GetIndex(), s->GetName().c_str());
				else
					Console->Print(RED, BLACK, "Subskill def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete s;
			}
			else
				++nDefs;
		}
	}
	else
	{
	    Console->Print("%s Error loading subskill defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i subskill defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i subskill defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);


	return (true);
}

bool PGameDefs::LoadCharKindDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_CHARKINDS;

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
					Console->Print(RED, BLACK, "Charkind def error (duplicate id %i): %s", c->GetIndex(), c->GetName().c_str());
				else
					Console->Print(RED, BLACK, "Charkind def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete c;
			}
			else
				++nDefs;
		}
	}
	else
	{
	    Console->Print("%s Error loading charkind defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i charkind defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i charkind defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadWorldDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_WORLDINFO;

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
					Console->Print(RED, BLACK, "World def error (duplicate id %i): %s", w->GetIndex(), w->GetName().c_str());
				else
					Console->Print(RED, BLACK, "World def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete w;
			}
			else
				++nDefs;
		}
	}
	 else
	{
	    Console->Print("%s Error loading world defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i world defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i world defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadFactionDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_FRACTIONS;

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
					Console->Print(RED, BLACK, "Faction def error (duplicate id %i): %s", f->GetIndex(), f->GetName().c_str());
				else
					Console->Print(RED, BLACK, "Faction def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete f;
			}
			else
				++nDefs;
		}
	}
	else
	{
   	    Console->Print("%s Error loading faction defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i faction defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i faction defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);


	return (true);
}

bool PGameDefs::LoadHackDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_HACK;

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
   	    Console->Print("%s Error loading hack defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i hack defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i hack defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadItemsDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_ITEMS;

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
   	    Console->Print("%s Error loading items defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i items defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i items defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadWorldModelDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_WORLDMODEL;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefWorldModel *it = new PDefWorldModel();
			bool loadfail = !it->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mWorldModelDefs.insert(std::make_pair(it->GetIndex(), it)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("WorldModel def error (duplicate id %i): %s", it->GetIndex(), it->GetName().c_str());
				else
					Console->Print("WorldModel def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete it;
			}
			else
				++nDefs;
		}
	}
	else
	{
   	    Console->Print("%s Error loading worldmodel defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i worldmodel defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i worldmodel defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadAppPlaceDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_APPPLACES;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefAppPlace *it = new PDefAppPlace();
			bool loadfail = !it->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mAppPlaceDefs.insert(std::make_pair(it->GetIndex(), it)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("AppPlace def error (duplicate id %i): %s", it->GetIndex(), it->GetName().c_str());
				else
					Console->Print("AppPlace def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete it;
			}
			else
				++nDefs;
		}
	}
	else
	{
   	    Console->Print("%s Error loading appplace defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i appplace defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i appplace defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadAppartementDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_APPARTEMENTS;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefAppartement *it = new PDefAppartement();
			bool loadfail = !it->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mAppartementDefs.insert(std::make_pair(it->GetIndex(), it)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Appartement def error (duplicate id %i): %s", it->GetIndex(), it->GetName().c_str());
				else
					Console->Print("Appartement def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete it;
			}
			else
				++nDefs;
		}
	}
	else
	{
   	    Console->Print("%s Error loading appartement defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i appartement defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i appartement defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

bool PGameDefs::LoadRespawnDefs()
{
	PDefParser parser;
	int nDefs = 0, nErrors = 0;
	const string DEF_FILE = Config->GetOption("defs_path") + "/" + DEF_RESPAWN;

	if (parser.Parse(DEF_FILE.c_str()))
	{
		const PDefTokenList &t = parser.GetTokens();

		for (PDefTokenList::const_iterator i=t.begin(); i!=t.end(); i++)
		{
			PDefRespawn *it = new PDefRespawn();
			bool loadfail = !it->LoadFromDef(*i), insertfail=false;

			if (!loadfail)
				insertfail = !mRespawnDefs.insert(std::make_pair(it->GetIndex(), it)).second;
			if (loadfail || insertfail)
			{
				if (insertfail)
					Console->Print("Respawn def error (duplicate id %i): %s", it->GetIndex(), it->GetName().c_str());
				else
					Console->Print("Respawn def load error @ %i", nDefs+nErrors);
				++nErrors;
				delete it;
			}
			else
				++nDefs;
		}
	}
	else
	{
   	    Console->Print("%s Error loading respawn defs", Console->ColorText(RED, BLACK, "[ERROR]"));

		return (false);
	}
    if(nErrors > 0)
        Console->Print("%s Loaded %i respawn defs, %i error(s).", Console->ColorText(RED, BLACK, "[ERROR]"), nDefs, nErrors);
    else
        Console->Print("%s Loaded %i respawn defs, %i error(s).", Console->ColorText(GREEN, BLACK, "[Success]"), nDefs, nErrors);

	return (true);
}

/****** Get methods ******/

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

const PDefCharKind *PGameDefs::GetCharKindDef(int Index) const
{
    PDefCharKind *Result = 0;
	PDefCharKindMap::const_iterator i = mCharKindDefs.find(Index);
	if (i!=mCharKindDefs.end())
		Result = i->second;

	return (Result);
}

const PDefWorldModel *PGameDefs::GetWorldModelDef(int Index) const
{
  PDefWorldModel *Result = 0;
	PDefWorldModelMap::const_iterator i = mWorldModelDefs.find(Index);
	if (i!=mWorldModelDefs.end())
		Result = i->second;

	return (Result);
}

const PDefAppPlace *PGameDefs::GetAppPlaceDef(int Index) const
{
  PDefAppPlace *Result = 0;
	PDefAppPlaceMap::const_iterator i = mAppPlaceDefs.find(Index);
	if (i!=mAppPlaceDefs.end())
		Result = i->second;

	return (Result);
}

const PDefAppartement *PGameDefs::GetAppartementDef(int Index) const
{
  PDefAppartement *Result = 0;
	PDefAppartementMap::const_iterator i = mAppartementDefs.find(Index);
	if (i!=mAppartementDefs.end())
		Result = i->second;

	return (Result);
}

const PDefRespawn *PGameDefs::GetRespawnDef(int Index) const
{
  PDefRespawn *Result = 0;
	PDefRespawnMap::const_iterator i = mRespawnDefs.find(Index);
	if (i!=mRespawnDefs.end())
		Result = i->second;

	return (Result);
}

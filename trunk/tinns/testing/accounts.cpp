#include "tinns.h"

PAccount::PAccount()
{
	mID = 0;
	for(int i=0; i<MAXCHARSPERACC; i++)
		mChars[i] = 0;
	mLevel = PAL_BANNED;
	mConsoleAllowed = false;
	mDirty = false;
}		

bool PAccount::AddChar(u32 CharID)
{
	for(int i=0; i<MAXCHARSPERACC; i++)
	{
		if(mChars[i]==0)
		{
			mChars[i]=CharID;
			return true;
		}
	}
	
	return false;	
}

void PAccount::RemoveChar(u32 CharID)
{
	for(int i=0; i<MAXCHARSPERACC; i++)
	{
		if(mChars[i]==CharID)
		{
			mChars[i]=0;
			break;
		}
	}
}

void PAccount::RemoveChar(int CharIndex)
{
	if(CharIndex < 0 || CharIndex >= MAXCHARSPERACC)
		return;
	
	mChars[CharIndex]=0;
}

u32 PAccount::GetChar(int Num) const
{
	if(Num < 0 || Num >= MAXCHARSPERACC)
		return 0;
		
	return mChars[Num];
}

void PAccount::SetID(u32 ID) 
{ 
	if(ID != mID)
		SetDirtyFlag();
	mID = ID; 
}

void PAccount::SetName(const std::string &Name)
{ 
	if(/*std::*/strcasecmp(mName.c_str(), Name.c_str()))
		SetDirtyFlag(); 
		
	mName = Name; 
}

void PAccount::SetPassword(const std::string &Pass) 
{ 
	if(mPassword != Pass)
		SetDirtyFlag(); 
	mPassword = Pass; 
}

void PAccount::SetLevel(PAccountLevel Level) 
{ 
	if(mLevel != Level)
		SetDirtyFlag(); 
		
	mLevel = Level; 
}

void PAccount::SetConsoleAllowed(bool Allowed) 
{ 
	if(Allowed != mConsoleAllowed)
		SetDirtyFlag();
		
	mConsoleAllowed = Allowed; 
}

std::string PAccount::GetLevelString() const
{
	switch(mLevel)
	{
		case PAL_BANNED : return "banned";
		case PAL_PLAYER : return "player";
		case PAL_VOLUNTEER : return "volunteer";
		case PAL_GM : return "gm";
		case PAL_ADMIN : return "admin";
	}
	
	return "banned";
}

bool PAccount::Load(const TiXmlElement *Node)
{
	if(!Node)
		return false;
		
	const char *name = Node->Attribute("name");
	const char *pass = Node->Attribute("password");
	const char *level = Node->Attribute("level");
	const char *id = Node->Attribute("id");

	if(name && pass && level && id)
	{
		SetID(std::atoi(id));
		SetName(name);
		SetPassword(pass);
		SetLevel(PAL_PLAYER);
		if(!/*std::*/strcasecmp(level, "admin"))
			SetLevel(PAL_ADMIN);
		else
		if(!/*std::*/strcasecmp(level, "gm"))
			SetLevel(PAL_GM);
		else
		if(!/*std::*/strcasecmp(level, "volunteer"))
			SetLevel(PAL_VOLUNTEER);
		else
		if(!/*std::*/strcasecmp(level, "player"))
			SetLevel(PAL_PLAYER);
		else
		if(!/*std::*/strcasecmp(level, "banned"))
			SetLevel(PAL_BANNED);
			
		int Con=0;
		Node->Attribute("console", &Con);
		SetConsoleAllowed(Con!=0);
		
		ClearDirtyFlag();
		
		return true;		
	}
	
	return false;	
}

void PAccount::Save(TiXmlElement *Node)
{
	if(!Node)
		return;

	Node->SetAttribute("name", GetName().c_str());
	Node->SetAttribute("password", GetPassword().c_str());
	Node->SetAttribute("id", GetID());
	Node->SetAttribute("level", GetLevelString().c_str());
	Node->SetAttribute("console", IsConsoleAllowed() ? "1" : "0");
	
	ClearDirtyFlag();
}

// ===========================================================================

PAccounts::PAccounts()
{
	mLastID = 1;
}

PAccounts::~PAccounts()
{
	Update();	// just to be sure
	for(AccountMap::iterator i=mAccounts.begin(); i!=mAccounts.end(); i++)
		delete i->second;	
}

void PAccounts::Load()
{
	Console->Print("Loading accounts...");
	int nAcc=0;
	bool HasAdminAcc = false;
	TiXmlDocument doc("./database/accounts.xml");
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			TiXmlElement *Acc = Root->FirstChildElement("account");
			while(Acc)
			{				
				PAccount *info = new PAccount();
				if(!info->Load(Acc))
				{
					Console->Print("Invalid account entry found in database. Ignored.");
					delete info;
				} else
				{
					if(!mAccounts.insert(std::make_pair(info->GetID(), info)).second)
					{
						Console->Print("Duplicate account id found: %s", info->GetName().c_str());
						delete info;
					} else
						++nAcc;					
					mLastID = max(mLastID, info->GetID());
					if(info->GetLevel()==PAL_ADMIN)
						HasAdminAcc=true;
				}
				Acc=Acc->NextSiblingElement("account");
			}
		}
	}
	
	Console->Print("Loaded %i accounts", nAcc);
	if(!HasAdminAcc)
		Console->Print("Warning: no admin account in database");
}

PAccount *PAccounts::Authenticate(const char *User, const u8 *Password, int PassLen, const u8 *Key, bool UseAutoAccounts)
{	
	PAccount *Account = 0;
	char Pass[128];
	Pass[0]=0;
	PAccount *Acc = 0;
	if(PassLen < 128)
	{	
		if(Key[0]>7) // TODO: >7 correct?
		{
			for(int i=0; i<PassLen; i+=2)
				Pass[i>>1] = (char)(((Password[i]&0xf0)>>4)
					+((Password[i+1]&0x0f)<<4)-Key[0]);
			Pass[PassLen>>1]=0;
		} else
		{
			for(int i=0; i<PassLen; i++)
				Pass[i] = (char)(Password[i]-Key[0]);
			Pass[PassLen]=0;
		}
		
		Acc = GetAccount(User);
		if(Acc)
		{
			if(Acc->GetPassword() == Pass)
				Account = Acc;
			else
				Console->Print("User %s entered wrong password", User);
		} else
			Console->Print("Unknown user %s", User);
	} else
		Console->Print("Accounts: malformed auth data");
	
	// auto accounts
	if(!Acc && UseAutoAccounts && Config->GetOptionInt("auto accounts"))
	{
		if(std::strlen(User) >= 3 && std::strlen(Pass) >= 4)
		{
			Account = CreateAccount(User, Pass);
			Console->Print("Account %s created", User);
		} else
			Console->Print("Could not autocreate account: user name or password too short");
	}
	
	return Account;
}

PAccount *PAccounts::Authenticate(const char *User, const char *Password) const
{	
	PAccount *Account = 0;
	PAccount *Acc = GetAccount(User);
	if(Acc)
	{
		if(Acc->GetPassword() == Password)
			Account = Acc;
		else
			Console->Print("User %s entered wrong password", User);
	} else
		Console->Print("Unknown user %s", User);
	
	return Account;
}

PAccount *PAccounts::GetAccount(u32 AccountID) const
{
	PAccount *Result = 0;
	AccountMap::const_iterator i = mAccounts.find(AccountID);
	if(i != mAccounts.end())
		Result = i->second;
	
	return Result;
}

PAccount *PAccounts::GetAccount(const std::string &Name) const
{
	PAccount *Result = 0;
	for(AccountMap::const_iterator i=mAccounts.begin(); i!=mAccounts.end(); i++)
	{
		if(!/*std::*/strcasecmp(i->second->GetName().c_str(), Name.c_str()))
		{
			Result = i->second;
			break;
		}
	}
	return Result;	
}

void PAccounts::Update()
{
	Console->Print("Updating account database...");
	TiXmlDocument doc("./database/accounts.xml");
	int NumUpd=0;
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			// update already exisiting accounts first
			TiXmlElement *e = Root->FirstChildElement("account");
			while(e)
			{
				int id;
				e->Attribute("id", &id);
				u32 id32 = static_cast<u32>(id);
				
				// WARNING: do not supply functions to change an account id from outside
				// because this piece of code won't work anymore!
				AccountMap::const_iterator i = mAccounts.find(id32);
				if(i!=mAccounts.end() && i->second->IsDirty())
				{
					i->second->Save(e);
					++NumUpd;					
				}
				
				e = e->NextSiblingElement("account");
			}
			
			// now add unsaved accounts
			for(AccountMap::iterator i=mAccounts.begin(); i!=mAccounts.end(); i++)
			{
				PAccount *acc = i->second;
				if(acc->IsDirty())
				{
					TiXmlElement elem("account");
					acc->Save(&elem);
					Root->InsertEndChild(elem);
					++NumUpd;					
				}
			}			
			
			if(!doc.SaveFile())
				Console->Print("PAccounts::Update: could not save account database");
		}
	} else
	{
		Console->Print("PAccounts::Update: could not load account database");
	}
	
	Console->Print("Update done, %i accounts updated", NumUpd);
}

PAccount *PAccounts::CreateAccount(const std::string &Name, const std::string &Password)
{
	PAccount *Acc = GetAccount(Name);
	if(Acc)
	{
		Console->Print("Accounts: account name %s already exists", Name.c_str());
		return 0;
	}
	
	Acc = new PAccount();
	Acc->SetID(++mLastID);
	Acc->SetName(Name);
	Acc->SetPassword(Password);
	Acc->SetLevel(PAL_PLAYER);
	Acc->SetConsoleAllowed(false);
	
	mAccounts.insert(std::make_pair(Acc->GetID(), Acc));
	
	Update();
	return Acc;
}

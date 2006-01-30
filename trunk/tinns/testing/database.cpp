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
	MODIFIED: 28 Sep 2005 Akiko
	REASON: - added this header
		- added GPL
		- reformated code
	MODIFIED: 29 Sep 2005 Akiko
	REASON: - fixed a syntax bug
*/


#include "tinns.h"

PDatabase::PDatabase() {
	mAccounts = 0;
	mChars = 0;
}

PDatabase::~PDatabase() {
	delete(mChars);
	delete(mAccounts);
}

void PDatabase::Init() {
	Console->Print("Initializing database...");
	mAccounts = new PAccounts();
	mAccounts->Load();
	
	mChars = new PChars();
	mChars->Load();
}

void PDatabase::Update() {
	mChars->Update();
}

PChar *PDatabase::CreateChar(u32 Account, const std::string &Name, u32 Type, u32 Model) {
	return(mChars->CreateChar(Account, Name, Type, Model));
}


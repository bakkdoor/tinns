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
        defparser.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL
*/

#include "main.h"

PDefParser::PDefParser()
{
}

PDefParser::~PDefParser()
{
	for(PDefTokenList::iterator i=mTokens.begin(); i!=mTokens.end(); i++)
		delete *i;
}

bool PDefParser::Parse(const char *File)
{
	PFile *f = Filesystem->Open("", File, Config->GetOption("nc_data_path"));
	enum { NOPE, MAYBE, LINE, BLOCK, MAYBE_END } Comment = NOPE;

	if(f)
	{
		while(!f->Eof())
		{
			std::string Str = f->ReadString();
			int Len = Str.length();
			if(Len <= 0)
				continue;

			if(Str.substr(0, 3)!="set")
				continue;

			//Console->Print("%s", Str.c_str());//NEW was commented , just to be sure of what we are reading

			PTokenList *List = new PTokenList();
			int Start=0, Tokens=0;
			bool Quote=false;
			for(int i=0; i<Len; i++)
			{
				if(Str[i]=='\r' || Str[i]=='\n' || Str[i]=='|' || Str[i]==';')
				{
					if(Comment < LINE)
					{
						int End=i-1;
						if(End >= Start)
						{
							List->push_back(std::string(Str.substr(Start, (End-Start)+1)));
							Tokens++;
							break;
						}
						Quote=false;
					}
					if(Comment != BLOCK)
						Comment=NOPE;
					break;
				}

				if(!Quote)
				{
					if(Comment < LINE && (Str[i]=='\t' || Str[i]==',' || Str[i]==' ' || Str[i]==';' || Str[i]=='|' || i==Len-1))
					{
						int End=i-1;
						if(End >= Start)
						{
							List->push_back(std::string(Str.substr(Start, (End-Start)+1)));
							Tokens++;
						}
						Start=i+1;
					}

					if(Str[i] <= ' ')
						continue;

					if(Str[i]=='*')
					{
						if(Comment==MAYBE)
							Comment=BLOCK;
						else
						if(Comment==BLOCK)
							Comment=MAYBE_END;
					} else
					if(Str[i]=='/')
					{
						if(Comment==MAYBE)
						{
							// second slash, skip rest of line
							Comment=LINE;
							break;
						} else
						if(Comment==MAYBE_END)
						{
							Comment=NOPE;	// comment block ends
							Start=i+1;
						} else
							if(Comment != BLOCK)
								Comment=MAYBE;	// first slash
					} else
					{
						if(Comment==MAYBE)
							Comment=NOPE;	// stand-alone slash
						else
						if(Comment==MAYBE_END)
							Comment=BLOCK;	// comment block did not end
					}
				}

				if(Str[i]=='"')
					Quote ^= true;
			}

			if(Tokens > 0)
				mTokens.push_back(List);
			else
				delete List;
		}
	} else
	{
		Console->Print("%s PDefParser::Parse: could not open file %s", Console->ColorText( RED, BLACK, "[ERROR]" ), File);
		return false;
	}

	return true;
}

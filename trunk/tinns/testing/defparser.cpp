#include "tinns.h"

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
	PFile *f = Filesystem->Open("", File);
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
		Console->Print("PDefParser::Parse: could not open file %s", File);
		return false;
	}
		
	return true;
}

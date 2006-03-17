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

#include "tinns.h"

u32 IPStringToDWord(const char *IP)
{
	if(!IP)
		return 0;

	u32 a, b, c, d;
	if(std::sscanf(IP, "%u.%u.%u.%u", &a, &b, &c, &d)!=4)
		return 0;

	return (d<<24)|(c<<16)|(b<<8)|a;
}

//NEW
//this function allow to print a packet
//just for tracking values
void PrintPacket(u8 *Packet, int PacketSize)
{
	Console->Print("inside : PrintPacket");

	if (PacketSize<1){Console->Print("Wrong packet");}//do nothing
	else
	{
		Console->Print("PacketSize is : %d",PacketSize);

		u8 value = 0;
		for (int i=0;i<PacketSize;i++)
		{
			value= *(u8*)&Packet[i];
			Console->Print("value[%d] is : %x",i,value);
		}
	}

}//end function

void GetSVNRev(char *version)
{
	FILE *f;

	if ((f = fopen(".svn/entries", "r")) != NULL) {
		char line[1024];
		int rev;
		while (fgets(line,1023,f))
		{
			if (strstr(line,"revision="))
                break;
		}
		fclose(f);

		if (sscanf(line," %*[^\"]\"%d%*[^\n]", &rev) == 1)
		{
			sprintf(version, "%d", rev);
			return;
		}
	}

    sprintf(version, "Unknown");
}

void Trim(char *t)
{
	RTrim(t);
	if (strlen(t))
		LTrim(t);
}

void Trim(std::string *stString)
{
	RTrim(stString);
	if (stString->length())
		LTrim(stString);
}

void RTrim(char *t)
{
	char *buf;

	if (!strlen(t)) return;

	buf = t;
	buf += strlen(t) -1;

	while (*buf == ' ' && strlen(t) )
		*buf-- = '\0';
}

void RTrim(std::string *stString)
{
	int i;							//Count VAR

	if (!stString->length())					//If There Is No Length
		return;

	i = stString->length()-1;					//Set The Counter To The String Length -1

	while (stString->substr(i,1) == " ")		//While There Is Still " "
	{
		*stString = stString->substr(0, i);		//Remove Off The End Of The String
		i--;									//Advance The Counter
	}

}

void LTrim(char *t)
{
	char tmp[128];				//Temp Char VAR
	char *buf;					//Pointer

	if (!strlen(t)) return;

	strcpy(tmp,t);
	buf=tmp;

	while (*buf == ' ' && strlen(buf))	*buf++;

	strcpy(t,buf);
}

void LTrim(std::string *stString)
{
	unsigned int i;							//Count VAR
	string buf;						//Temp String VAR

	if (!stString->length()) return;						//If The Length Is 0

	i = 0;													//Setup The Counter VAR

	while (stString->substr(i,1) == " ")					//While " "
		i++;												//Increase Counter

	if (i == stString->length())							//If The Whole String Is " "
	{
		*stString = "";										//Return Blank String
		return;
	}

	try
	{
		*stString = stString->substr(i,stString->length()-i);	//Send The String Back Without The Spaces
	}
	catch(...)
	{
		*stString = "";										//Return Blank String
		return;
	}

}

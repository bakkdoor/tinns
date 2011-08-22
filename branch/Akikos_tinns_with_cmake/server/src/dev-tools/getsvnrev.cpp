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
	getsvnrev - a simple tool to read out actual SVN revision-number

	Why?
		Well for example my server doesnt have any SVN libs installed. Therefore,
		the getsvnrev script fails. I need this little tool, and maybe someone else too ^^

	Usage: 
		call getsvnrev either without parameters or with the directory you
		want to know the SVN revision.
	
	Console output:
		If no .svn directory is found, the tool returns 0.
		Otherwise it will return the SVN revision of the target dir

	MODIFIED: 22 Dec 2006 Namikon
	REASON: - started this tool
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - more C++ stylish
		- missing libs (cstring, cstdlib)

	TODO:
		- Better way to get SVN rev than this (2nd number in file)
*/


#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
	std::string targetdir;
	FILE *f;

        if(argc == 2)
	{
		int i = 0;
		while(argv[1][i] != '\0') { i++; };
		if(argv[1][i-1] == '/')
		{
			targetdir = strcat(argv[1], ".svn/entries");
		}
		else
		{
			targetdir = strcat(argv[1], "/.svn/entries");
		}
        }
	else
	{
		targetdir = ".svn/entries";
	}


        if ((f = fopen(targetdir.c_str(), "r")) != NULL) {
                char line[255];
                int rev;
		bool firstnrfound = false;
		
		do
		{
		   fgets (line, 255, f);
		   rev = atoi(line);
		   if(rev > 0 && firstnrfound == false)
		   {
		  	firstnrfound = true;
			rev = 0;
                   }
		} while (rev == 0);
		
                fclose(f);

                std::cout << rev << std::endl;
        }
	else
	{
		std::cout << "0" << std::endl;
	}
	return(0);
}

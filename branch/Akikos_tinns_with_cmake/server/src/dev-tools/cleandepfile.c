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
  This tool is to be used on output from gcc -MD -MP
  in order to filter out standard files from dependency list
  
  It is needed because a simple egrep -v would not leave the file in
  valid format
*/

/*
	cleandepfile.c

	CREATED:  11 Dec 2006 Hammag
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - corrected warning (args)
*/

#include <string.h>
#include <stdio.h>

char in_buff[1024];

char* filter_out[] = {
  "/usr/include/",
  "/usr/lib/",
  "/usr/local/lib/"
};
#define filter_nb 3
int filter_len[filter_nb];

void init_filter(void)
{
  int i;
  
  for (i=0; i<filter_nb; i++)
  {
    filter_len[i] = strlen(filter_out[i]);
  } 
}

int check_filter(char* buffer)
{
  int i;
  
  for (i=0; i<filter_nb; i++)
  {
    if (!strncmp(buffer, filter_out[i], filter_len[i]))
    {
      return 0;
    }
  }
  return 1;
}

int main(void)
{
  int ret;
  int len;
  int inside = 0;
  
  init_filter();
  
  while(1)
  {
    ret = scanf("%1023s", in_buff);
    if (ret == EOF) break;
    if (ret == 0) continue;
    
    len = strlen(in_buff);
    switch (in_buff[len-1])
    {
      case ':':
      {
        if (check_filter(in_buff))
        {
          printf("\n%s", in_buff);
          inside = 1;
        }
        else
        {
          inside = 0;
        }
        break; 
      }
      
      case '\\':
      {
        if (len == 1)
          break;
        else
          in_buff[len-1] = 0;
      }
      default:
      {
        if (inside && check_filter(in_buff))
          printf(" \\\n  %s", in_buff);
        break; 
      }
    }
    
  }
  printf("\n");

  return 0; 
}

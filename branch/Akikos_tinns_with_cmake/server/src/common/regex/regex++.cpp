//
// regex.hpp 1.0 Copyright (c) 2003 Peter Petersen (pp@on-time.de)
// Simple C++ wrapper for PCRE
//
// This source file is freeware. You may use it for any purpose without
// restriction except that the copyright notice as the top of this file as
// well as this paragraph may not be removed or altered.
//



/*
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/

#include <string.h>
#include "include/regex++.h"

RegEx::RegEx(const char * regex, int options)
{
   const char * error;
   int          erroffset;

   re = pcre_compile(regex, options, &error, &erroffset, NULL);
   if (re == NULL)
      throw error;
   pe = pcre_study(re, 0, &error);
   pcre_fullinfo(re, pe, PCRE_INFO_CAPTURECOUNT, &substrcount);
   substrcount++;
   ovector = new int[3*substrcount];
   matchlist = NULL;
}

RegEx::~RegEx()
{
   ClearMatchList();
   delete ovector;
   if (pe)
      pcre_free(pe);
   pcre_free(re);
}

bool RegEx::Search(const char * subject, int len, int options)
{
   ClearMatchList();
   return pcre_exec(re, pe, lastsubject = subject, slen = (len >= 0) ? len : strlen(subject), 0, options, ovector, 3*substrcount) > 0;
}

bool RegEx::SearchAgain(int options)
{
   ClearMatchList();
   return pcre_exec(re, pe, lastsubject, slen, ovector[1], options, ovector, 3*substrcount) > 0;
}

const char * RegEx::Match(int i)
{
   if (i < 0)
      return lastsubject;
   if (matchlist == NULL)
      pcre_get_substring_list(lastsubject, ovector, substrcount, &matchlist);
   return matchlist[i];
}

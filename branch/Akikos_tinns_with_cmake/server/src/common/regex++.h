#pragma once

#include <iostream>

#ifndef _PCRE_H
#include "pcre.h"
#endif

class RegEx {
private:
	pcre * re;
	pcre_extra * pe;
	int substrcount;
	int * ovector;
	const char * lastsubject;
	int slen;
	const char * * matchlist;
      
	inline void ClearMatchList(void)
	{
		if (matchlist)
		{
			pcre_free_substring_list(matchlist);
			matchlist = NULL;
		}
	}

public:
	RegEx(const char * regex, int options = 0);
	~RegEx();
	inline int SubStrings(void) const { return substrcount; }
	bool Search(const char * subject, int len = -1, int options = 0);
	bool SearchAgain(int options = 0);
	const char * Match(int i = 1);
};

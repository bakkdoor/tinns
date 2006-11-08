/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  doortemplate.cpp - world door template class

	MODIFIED: 05 Nov 2006 Hammag
	REASON: - creation
	
*/


#include "main.h"
#include "doortemplate.h"

PDoorTemplate::PDoorTemplate()
{
  mDoorID = 0;
  mWorldmodelID = 0;
  mDefWorldModel = NULL;
  mIsDoubleDoor = false;
  mIsTriggeredDoor = false;
}

PDoorTemplate::~PDoorTemplate()
{
  
}

void PDoorTemplate::SetDoorParameters(char* nDoorParametersString)
{
  char* SepPos;
  int ParamNum = 0;
  
  while ( *nDoorParametersString && (SepPos = strchr(nDoorParametersString, ',')) && (ParamNum < 4))
  {
    *SepPos = 0;
    mDoorParameters[ParamNum++] = atoi(nDoorParametersString);
    nDoorParametersString = SepPos + 1;
  }
  if (*nDoorParametersString)
  {
    if (ParamNum < 4)
      mDoorParameters[ParamNum] = atoi(nDoorParametersString);
    else
      Console->Print(RED, BLACK, "[ERROR] More than 4 parameters in Sec2ElemType5 ParamString");
  }
}

void PDoorTemplate::SetDoorTypeName(char* nDoorTypeName)
{
  mDoorTypeName = nDoorTypeName;
  
  if (mDoorTypeName == "DDOOR")
  {
    mIsDoubleDoor = true;
  }
  else if (mDoorTypeName == "TRIGDD")
  {
    mIsDoubleDoor = true;
    mIsTriggeredDoor = true;
  }
}

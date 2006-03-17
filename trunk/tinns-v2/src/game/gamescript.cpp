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

PGameScript::PGameScript()
{
}

PGameScript::~PGameScript()
{
	for(HookMap::iterator it = mHooks.begin(); it != mHooks.end(); it++)
		delete it->second;
}

bool PGameScript::LoadScripts()
{
}

bool PGameScript::Rehash()
{
    Console->LPrint("Rehashing GameMonkey Scripts...");
	for(HookMap::iterator it = mHooks.end(); it != mHooks.begin(); it--)
        mHooks.erase(it);

    if(LoadScripts() == true)
    {
        Console->LPrint(GREEN, BLACK, "Done");
        Console->LClose();
        return true;
    }
    else
    {
        Console->LPrint(RED, BLACK, "Failed");
        Console->LClose();
        return false;
    }
}

void PGameScript::TriggerHook(PHookTypes hook)
{
}


bool ScriptSys::ExecuteFile(const char* a_fileName)
{
  FILE* scriptFile = NULL;
  char* fileString = NULL;
  int fileSize = 0;

  GM_ASSERT(m_machine);

  if( !(scriptFile = fopen(a_fileName, "rb")) )
  {
    return false;
  }

  fseek(scriptFile, 0, SEEK_END);
  fileSize = ftell(scriptFile);
  fseek(scriptFile, 0, SEEK_SET);
  fileString = new char [fileSize+1];
  fread(fileString, fileSize, 1, scriptFile);
  fileString[fileSize] = 0; // Terminating null
  fclose(scriptFile);

  int threadId = GM_INVALID_THREAD;
  int errors = m_machine->ExecuteString(fileString, &threadId, true, a_fileName);
  if(errors)
  {
    LogAnyMachineErrorMessages();
  }

  delete [] fileString;

  return true;
}


int GM_CDECL PGameScript::AddHook(gmThread *a_thread)
{
    GM_CHECK_NUM_PARAMS(2);
    GM_CHECK_STRING_PARAM(tmphooktype, 0);
    GM_CHECK_STRING_PARAM(tmpfunction, 1);

    mHooks.insert(std::make_pair(tmphooktype, tmpfunction));

    return GM_OK;
}

int PGameScript::AddTwoIntegers(int valueA, int valueB)
{
  int resultInt = 0;

  gmCall call;

  if(call.BeginGlobalFunction(&machine, "Add"))
  {
    call.AddParamInt(valueA);
    call.AddParamInt(valueB);
    call.End();
    call.GetReturnedInt(resultInt);
  }

  return resultInt;
}
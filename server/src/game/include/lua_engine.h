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

lua_engine.h - TinNS Lua engine for processing NPC scripts

CREATION: 13 Oct 2009 Namikon

*/

#ifndef PLUA_H
#define PLUA_H

extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

#define PLUAENGINE_DEFAULT      0
#define PLUAENGINE_GETANSWER    4096
#define PLUAENGINE_EXECUTENODE  8192

class PLuaEngine
{
    private:
        PClient* mTargetClient;

        bool mRunning;
        lua_State *mLua;

        std::vector<int> mReturnValues;


        // Return Values
        int mReturn_INT;
//        std::string mReturn_STR;
        // add more if needed...

        void CleanUp();
        bool ExecuteScript(std::string nLUAScript, int nNode, int nDialogClass = 0);

    public:
        PLuaEngine();
        ~PLuaEngine();

        // To check if LUA Script has syntax errors or whatever
        bool CheckLUAFile(std::string nLUAScript);
        void AddScriptResult(int nResult);
        void ProcessDialogScript(PClient* nClient, std::string mLUAFile, int nAnswer);
        inline PClient* GetBoundClient() { return mTargetClient; };

        inline void SetReturnINT(int nValue) { mReturn_INT = nValue; };
        // inline void SetReturnSTR(std::string nValue) { mReturn_STR = nValue; };
};


#endif

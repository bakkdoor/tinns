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

lua_engine.cpp - TinNS Lua engine for processing NPC scripts

CREATION: 13 Oct 2009 Namikon

*/

#include "main.h"
#include "include/msgbuilder.h"

// ******************************************************************
// ATTENTION  ATTENTION  ATTENTION  ATTENTION  ATTENTION  ATTENTION
// ******************************************************************
/*
   This function is the CALLBACK function for Neocron lua scripts.
   Every command inside any .lua file will result in a call to
   SendScriptMsg(xx), which ends up here.

   For currently known callbacks, look in the docs/ folder,
   file lua_callbacks.xls. It also contains the status and
   all parameters with return values that are required to handle
   script requests. If you're extending this function,
   please note it in the xls file!
*/
// ******************************************************************
int npcscript_callback(lua_State *nLua)
{
    PClient* tClient = LuaEngine->GetBoundClient();
    if(tClient == NULL)
    {
        Console->Print("%s [npcscript_callback] No client is set, unable to process actions!", Console->ColorText(RED, BLACK, "[Error]"));
        return 0;
    }

    // Get dialogclass and ScriptMsg from LUA call
    std::string tScriptMsg = lua_tostring(nLua, 1);
    int tDialogClass = lua_tointeger(nLua, 2);

    // Only execute any commands (getmoney, takemoney, etc) when we're told to do so!
    bool tExecCommands = false;
    if((tDialogClass&PLUAENGINE_EXECUTENODE) == PLUAENGINE_EXECUTENODE)
    {
        tExecCommands = true;
    }

    // Check which ScriptMsg we have
    if(tScriptMsg == "setanswer")
    {
        // its an setanswer
        // see if we're requested to return the node for any special answer number
        if((tDialogClass&PLUAENGINE_GETANSWER) == PLUAENGINE_GETANSWER)
        {
            // looks like that. Get the answernumber to check for and answernumber from lua
            int tAnswerNr = tDialogClass - PLUAENGINE_GETANSWER;
            int tLuaAnswerNr = lua_tointeger(nLua, 3);

            // Now check answernumber, and if positive, set the returnvalue in LuaEngine
            if(tAnswerNr == tLuaAnswerNr)
            {
                LuaEngine->SetReturnINT(lua_tointeger(nLua, 5));
                // We're done here; exit function
                return 1;
            }
        }
    }
    else if(tScriptMsg == "givemoney" && tExecCommands == true)
    // Give the player money
    // Script syntax: GIVEMONEY(amount)
    // Return Values: None
    {
        int tAmount = lua_tointeger(nLua, 3);

        /*int tNewMoneyValue = */tClient->GetChar()->AddCash(tAmount);

        // This issnt required, since the client adds the money already. It
        // isnt added twice, but you get the message twice and thats confusing :)
        //PMessage* tmpMsg = MsgBuilder->BuildCharMoneyUpdateMsg(tClient, tNewMoneyValue);
        //tClient->SendUDPMessage(tmpMsg);

        if (gDevDebug) Console->Print("GIVEMONEY from script; Added %d credits", tAmount);
    }
    else if(tScriptMsg == "takemoney" && tExecCommands == true)
    // Takes away money from player
    // Script syntax: TAKEMONEY(amount)
    // Return Values: 1 (Success) 0 (Failed)
    {
        int tAmount = lua_tointeger(nLua, 3);
        if (gDevDebug) Console->Print("TAKEMONEY from script");

        if(tClient->GetChar()->GetCash() < (u32)tAmount)
        {
            // Cannot take cash, player has less than amount required
            LuaEngine->AddScriptResult(0);
            if (gDevDebug) Console->Print("..FAILED player has not enough money (-%d)", tAmount);
        }
        else
        {
            // TakeMOney successfull. Reduce players credits and send money update
            int tNewMoneyValue = tClient->GetChar()->TakeCash(tAmount);
            PMessage* tmpMsg = MsgBuilder->BuildCharMoneyUpdateMsg(tClient, tNewMoneyValue);
            tClient->SendUDPMessage(tmpMsg);

            // TakeMoney successfull. Return 1
            LuaEngine->AddScriptResult(1);
            if (gDevDebug) Console->Print("..SUCCESS player had enough (-%d)", tAmount);
        }
    }
    else if(tScriptMsg == "rand" && tExecCommands == true)
    // Generate random number from 0 to tRange
    // Script syntax: RAND(range)
    // Return Values: 0 to range
    {
        int tRange = lua_tointeger(nLua, 3);

        int tRandomVal = GetRandom(tRange, 0);
        LuaEngine->AddScriptResult(tRandomVal);
        if (gDevDebug) Console->Print("RAND(%d) from script. Result: %d", tRange, tRandomVal);
    }
    else if(tScriptMsg == "takeitem" && tExecCommands == true)
    // Takes away an item from players inventory
    // Script syntax: TAKEITEM(itemID)
    // Return Values: 1 (Success) 0 (Failed)
    {
        int tItemID = lua_tointeger(nLua, 3);
        // Check if player has item ID tItemID and take it away
        // No clean way to take item away yet, fake answer until then.
        int tRes = GetRandom (1, 0);
        LuaEngine->AddScriptResult(tRes); // TakeItem was SUCCESSFULL

        if (gDevDebug) Console->Print("TAKEITEM(%d) from script. Random result: %d", tItemID, tRes);
    }
    else if(tScriptMsg == "giveitem" && tExecCommands == true)
    // Give an item to player, with given quality
    // Script syntax: GIVEITEM(itemID)
    // Return Values: None
    {
        int tItemID = lua_tointeger(nLua, 3);
        int tQuality = lua_tointeger(nLua, 4);
        // Give item tItemID in quality tQuality to player

        PItem* NewItem = new PItem( tItemID );
        if ( NewItem->GetItemID() )
        {
            NewItem->MakeItemStandard( tQuality, tQuality ); // Set Quality of item to tQuality
            tClient->GetChar()->GetInventory()->AddItem( NewItem );
            // TODO: Find a way to refresh inventory
            if (gDevDebug) Console->Print("GIVEITEM(%d,%d) from script. Result: %d", tItemID, tQuality);
        }
        else
        {
            // Error, invalid ItemID in script..?
            if (gDevDebug) Console->Print("GIVEITEM(%d,%d) from script. Fail: Unknown itemID", tItemID, tQuality);

            // Do nothing
        }
    }
    return 1;
}
// *************************************************************
bool PLuaEngine::CheckLUAFile(std::string nLUAScript)
{
    luaL_openlibs(mLua);
    if (luaL_loadstring(mLua, nLUAScript.c_str()) || lua_pcall(mLua, 0, 0, 0)) {
        Console->Print("[PLuaEngine::CheckLUAFile] Error in LUA Script: %s", lua_tostring(mLua, -1));
        return false;
    }

    // Check if we have the 2 main functions present
    lua_getglobal(mLua, "lSendAction");
    if(!lua_isfunction(mLua,-1))
    {
        Console->Print("[PLuaEngine::CheckLUAFile] LUA Script is missing function \"lSendAction)\"");
        lua_pop(mLua,1);
        return false;
    }
    // Check if we have the 2 main functions present
    lua_getglobal(mLua, "SetResult");
    if(!lua_isfunction(mLua,-1))
    {
        Console->Print("[PLuaEngine::CheckLUAFile] LUA Script is missing function \"SetResult)\"");
        lua_pop(mLua,1);
        return false;
    }

    // LUA File is ok
    return true;
}

void PLuaEngine::ProcessDialogScript(PClient* nClient, std::string nLUAFile, int nAnswer)
{
    mTargetClient = nClient;
    PChar* tChar = nClient->GetChar();

    // Get current node ID the player is on
    u16 tNodeID = tChar->GetDialogNode();

    // First, get the new node ID based on the answer the client gave us. DO NOT execute any commands from
    // Script yet. Resulting node is in mReturn_INT
    ExecuteScript(nLUAFile, tNodeID, nAnswer + PLUAENGINE_GETANSWER);

    //int tNextNode = LuaEngine->GetNextNodeFromAnswer(nLUAFile, tNodeID, nAnswer);
    // Now execute target Node. Process all requests by Script, and tell client the results (if any)
    if(LuaEngine->ExecuteScript(nLUAFile, mReturn_INT, PLUAENGINE_EXECUTENODE) == false)
    {
        Console->Print("ERROR while processing LUA Script, skip");
        return;
    }
    else
    {
        // Set the new node in client
        tChar->SetDialogNode((u16)mReturn_INT);
        // Now send the new node (together with the answer-buffer) to the client.
        // After this packet hits the client, it will show the given node
        PMessage* tmpMsg = MsgBuilder->BuildNPCDialogReplyMsg(nClient, tChar->GetDialogNode(), &mReturnValues);
        nClient->SendUDPMessage(tmpMsg);
    }

    // Cleanup everything
    mTargetClient = NULL;
    CleanUp();
}

void PLuaEngine::AddScriptResult(int nResult)
{
    if(!mRunning)
    {
        Console->Print("%s [PLuaEngine::AddScriptResult] Trying to set lua result while no lua script is running", Console->ColorText(YELLOW, BLACK, "[Warning]"));
        return;
    }

    // Insert new returnvalue to buffer
    mReturnValues.push_back(nResult);

    // Push result back to LUA Script
    lua_getglobal(mLua, "SetResult");
    lua_pushnumber(mLua, nResult);   // newstate

    if (lua_pcall(mLua, 1, 0, 0) != 0)
    {
        Console->Print("%s [npcscript_callback] Unable to return result '%s'", Console->ColorText(RED, BLACK, "[Error]"), lua_tostring(mLua, -1));
    }
}

PLuaEngine::PLuaEngine()
{
	// New lua function 
    mLua = luaL_newstate();
    mRunning = false;
}

void PLuaEngine::CleanUp()
{
    // CleanUp vars, empty vectors, etc
    mRunning = false;
    mReturnValues.erase(mReturnValues.begin(), mReturnValues.end());
}

PLuaEngine::~PLuaEngine()
{
    lua_close(mLua);
}

bool PLuaEngine::ExecuteScript(std::string nLUAScript, int nNode, int nDialogClass)
{
    // Make sure we have an bound client to work with
    if(mTargetClient == NULL)
    {
        Console->Print("%s [PLuaEngine::ExecuteScript] No Client has been bound. Cannot run lua script", Console->ColorText(RED, BLACK, "[Error]"));
        return false;
    }

    // Open LUA libs
    luaL_openlibs(mLua);

    // Try to load script and check for syntax errors
    if (luaL_loadstring(mLua, nLUAScript.c_str()) || lua_pcall(mLua, 0, 0, 0)) {
        Console->Print("%s [PLuaEngine::ExecuteScript] Unable to execute lua script: '%s'", Console->ColorText(RED, BLACK, "[Error]"), lua_tostring(mLua, -1));
        return false;
    }

    // Register callback function for NPC Script actions
    lua_register(mLua, "SendScriptMsg", npcscript_callback);

    // Get handle for lua main() function
    lua_getglobal(mLua, "lSendAction");

    // Error if function isnt found
    if(!lua_isfunction(mLua,-1))
    {
        Console->Print("%s [PLuaEngine::ExecuteScript] Unable to execute lua script; Script has no entry function (lSendAction)", Console->ColorText(RED, BLACK, "[Error]"));
        lua_pop(mLua,1);
        return false;
    }
    // Push our vars to the script
    // DialogClass is a loopback value for our callback function, to detect different
    // script runs (with or without execution of command)
    lua_pushnumber(mLua, nDialogClass);
    lua_pushnumber(mLua, nNode);

    /*
       run LUA Script (2 arguments, 0 result, 0 errorhandler)
       Only allow callback actions while lua is running (mRunning = true)
    */

    mRunning = true;
    int tLuaResult = lua_pcall(mLua, 2, 0, 0);
    mRunning = false;

    if (tLuaResult != 0)
    {
        Console->Print("%s [PLuaEngine::ExecuteScript] Unable to execute lua script: '%s'", Console->ColorText(RED, BLACK, "[Error]"), lua_tostring(mLua, -1));
        return false;
    }

    return true;
}

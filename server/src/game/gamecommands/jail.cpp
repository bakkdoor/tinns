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
#include "main.h"

void PCommands::doCmdjail()
{
  bool SyntaxError = false;
  if ( ArgC < 1 )
  {
    SyntaxError = true;
  }

  if ( SyntaxError == true )
  {
    Chat->send( source, CHAT_DIRECT, "Usage", "@jail <charID or nickname>" );
    return;
  }

  if ( IsArgNumeric( 1 ) == true )
  {
    target = GetClientByID( GetArgInt( 1 ) );
  }
  else
  {
    char tmp_destNick[50];
    GetArgText( 1, tmp_destNick, 50 );
    target = GetClientByNick( tmp_destNick );
  }

  if ( target == NULL ) // If victim isnt found, return error
  {
    Chat->send( source, CHAT_DIRECT, "System", "No such player" );
    return;
  }
  // Check accountlevel (Only higher's can perform commands on lower's)
  if ( source->GetAccountLevel() <= target->GetAccountLevel() )
  {
    char tmpMsg[200];
    snprintf( tmpMsg, 199, "Cant jail %s, target level is higher or equal to yours!", Chars->GetChar( target->GetCharID() )->GetName().c_str() );
    tmpMsg[199] = '\0';
    Chat->send( source, CHAT_DIRECT, "System", tmpMsg );
    return;
  }

  u32 currLoc = target->GetChar()->GetLocation();
  // Make sure that target player is not already in Regants
  if ( currLoc == 550 || currLoc == 551 )
  {
    Chat->send( source, CHAT_DIRECT, "System", "Target player is already jailed!" );
    return;
  }

// *************** Checks done, proceed with command

  int destZone = 550; // DarkMetal #1
  if ( target->ChangeCharLocation( destZone, true ) )
  {
    PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg( target, destZone, 0 );
    target->SendUDPMessage( tmpMsg_zone );
    tmpMsg_zone = NULL;

    char tmpMsg_success[81];
    snprintf( tmpMsg_success, 80, "Successfully jailed %s", target->GetChar()->GetName().c_str() );
    tmpMsg_success[80] = '\0';
    Chat->send( source, CHAT_DIRECT, "System", tmpMsg_success );
    return;
  }
  else
  {
    Console->Print( "%s Unable to change location for player %d to %d", Console->ColorText( RED, BLACK, "[PANIC]" ), target->GetCharID(), destZone );
  }
}

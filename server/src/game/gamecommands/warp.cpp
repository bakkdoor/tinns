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

void PCommands::doCmdwarp()
{
  int SpawnPointID = 0;
  int zoneID = 0;
  int ZoningType = 0;

  bool SyntaxError = false;
  if ( ArgC < 1 )
  {
    SyntaxError = true;
  }
  else
  {
    if ( IsArgNumeric( 1 ) == true )
    {
      zoneID = GetArgInt( 1 );
      if ( zoneID == 0 )
      {
        SyntaxError = true;
      }
    }
    else
    {
      SyntaxError = true;
    }

    if ( ArgC > 1 )
    {
      if ( IsArgNumeric( 2 ) == true )
      {
        SpawnPointID = GetArgInt( 2 );
        if ( ArgC > 2 )
        {
          ZoningType = 1;
        }
      }
      else
      {
        SyntaxError = true;
      }
    }
  }
  if ( SyntaxError == true )
  {
    Chat->send( source, CHAT_DIRECT, "Usage", "@warp <zoneid> [<spawn location> [<use .def: 1>]]" );
    return;
  }

  if ( !Worlds->IsValidWorld( zoneID ) )
  {
    Chat->send( source, CHAT_DIRECT, "System", "Invalid zoneID" );
    return;
  }

  if ( source->ChangeCharLocation( zoneID, true ) )
  {
    if ( gDevDebug ) Console->Print( "IngameCommand: Warping player %d to zone %d (%s)", source->GetCharID(), zoneID, Worlds->GetWorld( zoneID )->GetName().c_str() );

    PMessage* tmpMsg = MsgBuilder->BuildAptLiftUseMsg( source, zoneID, SpawnPointID, ZoningType );
    source->SendUDPMessage( tmpMsg );
  }
  else
  {
    Console->Print( RED, BLACK, "IngameCommand: Can't change location when trying to warp player %d to zone %d", source->GetCharID(), zoneID );
  }
}

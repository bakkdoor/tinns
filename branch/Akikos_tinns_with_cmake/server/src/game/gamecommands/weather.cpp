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

void PCommands::doCmdweather()
{
  bool SyntaxError = false;
  if ( ArgC < 1 )
  {
    SyntaxError = true;
  }

  if ( SyntaxError == true )
  {
    Chat->send( source, CHAT_DIRECT, "Usage", "@weather ?|<weather id> [<target char id>]" );
    return;
  }

  int wId = 0;
  char tmp_v1[6];
  GetArgText( 1, tmp_v1, 6 );

  if ( tmp_v1[0] != '?' )
  {
    wId = atoi( tmp_v1 );
    if ( wId > 0 )
    {
      if ( ArgC >= 2 )
      {
        if ( !GetTarget( 2 ) )
        {
          Chat->send( source, CHAT_DIRECT, "System", "Invalid target character" );
          return;
        }
      }
      else
        target = source;

      PMessage* tmpMsg = MsgBuilder->BuildWeatherControlMsg(( u16 )wId );
      ClientManager->UDPBroadcast( tmpMsg, target );

      char effStr[128];
      snprintf( effStr, 127, "Weather set to value %d", wId );
      effStr[127] = '\0';
      Chat->send( source, CHAT_DIRECT, "System", effStr );

      return;
    }
    else
      Chat->send( source, CHAT_DIRECT, "System", "Invalid weather id" );
  }

  Chat->send( source, CHAT_DIRECT, "System", "Valid weather values:" );
  Chat->send( source, CHAT_DIRECT, "-", "1:Bright sky 2:Single cloud 3:Cloudy" );
  Chat->send( source, CHAT_DIRECT, "-", "4:Light rain 5:Heavy rain 6:Thunderstorm" );
  Chat->send( source, CHAT_DIRECT, "-", "100:Sea bright sky" );

}

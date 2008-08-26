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

void PCommands::doCmduptime()
{
    // get difference between var uptime and current time
    std::time_t Uptime = GameServer->GetStartTime();
    u32 TimeDiff = std::time(NULL) - Uptime;
    u32 UpYears = 0, UpMonths = 0, UpWeeks = 0, UpDays = 0, UpHours = 0, UpMinutes = 0, UpSeconds = 0;
    char tmpY[21], tmpM[21], tmpW[21], tmpD[21], tmpH[21], tmpMi[21], tmpS[21];

    // 1 min = 60 sec                           = 60
    // 1 hour = 60 * 60 sec                     = 3600
    // 1 day = 24 * 60 * 60 sec                 = 86400
    // 1 week = 7 * 24 * 60 * 60 sec            = 604800
    // 1 month = 30 * 7 * 24 * 60 * 60 sec      = 18144000
    // 1 year = 12 * 30 * 7 * 24 * 60 * 60 sec  = 217728000

    // Split Years
    while(TimeDiff >= 217728000)
    {
        TimeDiff = TimeDiff - 217728000;
        UpYears++;
    }

    // Split Months
    while(TimeDiff >= 18144000)
    {
        TimeDiff = TimeDiff - 18144000;
        UpMonths++;
    }

    // Split Weeks
    while(TimeDiff >= 604800)
    {
        TimeDiff = TimeDiff - 604800;
        UpWeeks++;
    }

    // Split Days
    while(TimeDiff >= 86400)
    {
        TimeDiff = TimeDiff - 86400;
        UpDays++;
    }

    // Split Hours
    while(TimeDiff >= 3600)
    {
        TimeDiff = TimeDiff - 3600;
        UpHours++;
    }

    // Split Minutes
    while(TimeDiff >= 60)
    {
        TimeDiff = TimeDiff - 60;
        UpMinutes++;
    }

    // What's left are seconds
    UpSeconds = TimeDiff;

    // Now create output strings. Add 's' if > 1
    // Years
    if(UpYears > 1)
        snprintf(tmpY, 20, "%d years ", UpYears);
    else if(UpYears > 0)
        snprintf(tmpY, 20, "%d year ", UpYears);
    else
        tmpY[0] = '\0';

    // Months
    if(UpMonths > 1)
        snprintf(tmpM, 20, "%d months ", UpMonths);
    else if(UpMonths > 0)
        snprintf(tmpM, 20, "%d month ", UpMonths);
    else
        tmpM[0] = '\0';

    // Weeks
    if(UpWeeks > 1)
        snprintf(tmpW, 20, "%d weeks ", UpWeeks);
    else if(UpWeeks > 0)
        snprintf(tmpW, 20, "%d week ", UpWeeks);
    else
        tmpW[0] = '\0';

    // Days
    if(UpDays > 1)
        snprintf(tmpD, 20, "%d days ", UpDays);
    else if(UpDays > 0)
        snprintf(tmpD, 20, "%d day ", UpDays);
    else
        tmpD[0] = '\0';

    // Hours
    if(UpHours > 1)
        snprintf(tmpH, 20, "%d hours ", UpHours);
    else if(UpHours > 0)
        snprintf(tmpH, 20, "%d hour ", UpHours);
    else
        tmpH[0] = '\0';

    // Minutes
    if(UpMinutes > 1)
        snprintf(tmpMi, 20, "%d minutes and ", UpMinutes);
    else if(UpMinutes > 0)
        snprintf(tmpMi, 20, "%d minute and ", UpMinutes);
    else
        tmpMi[0] = '\0';

    // Seconds
    if(UpSeconds > 1 || UpSeconds == 0)
        snprintf(tmpS, 20, "%d seconds.", UpSeconds);
    else
        snprintf(tmpS, 20, "%d second.", UpSeconds);

    // Terminate all chars, just to be sure
    tmpY[20] = '\0';
    tmpM[20] = '\0';
    tmpW[20] = '\0';
    tmpD[20] = '\0';
    tmpH[20] = '\0';
    tmpMi[20] = '\0';
    tmpS[20] = '\0';

    //Copy the single msg's into one
    char tmpChatMsg[300];
    snprintf(tmpChatMsg, 299, "The server has been running for %s%s%s%s%s%s%s", tmpY, tmpM, tmpW, tmpD, tmpH, tmpMi, tmpS);
    tmpChatMsg[299] = '\0';

    // Send it out
    Chat->send(source, CHAT_DIRECT, "System", tmpChatMsg);
}

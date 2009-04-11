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
        misc.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL
        MODIFIED: 07 Jan 2006 Namikon
        REASON: - Added function to trim a string/char
       MODIFIED: 01 Jul 2006 hammag
       REASON: - add IPlongToString()
       MODIFIED: 27 Aug 2006 Hammag
       REASON: - Merged misc function from all 3 servers
       MODIFIED: 11 Dec 2006 Hammag
       REASON: - Commented out GetSVNRev() that is not used anymore

       TODO:
               - Put Network Utility function in a netutility.cpp in netcode
               - Put GetAccessString() as a static member of Accounts class
*/

#include "main.h"

u32 IPStringToDWord( const char *IP )
{
  if ( !IP )
    return 0;

  u32 a, b, c, d;
  if ( std::sscanf( IP, "%u.%u.%u.%u", &a, &b, &c, &d ) != 4 )
    return 0;

  return ( d << 24 ) | ( c << 16 ) | ( b << 8 ) | a;
}

char *IPlongToString( const u32 IP )
{
  struct in_addr in_IP;

  in_IP.s_addr = IP;
  return inet_ntoa( in_IP );
}

//NEW
//this function allow to print a packet
//just for tracking values
void PrintPacket( u8 *Packet, int PacketSize )
{
  Console->Print( "inside : PrintPacket" );

  if ( PacketSize < 1 ){Console->Print( "Wrong packet" );}//do nothing
  else
  {
    Console->Print( "PacketSize is : %d", PacketSize );

    u8 value = 0;
    for ( int i = 0;i < PacketSize;i++ )
    {
      value = *( u8* ) & Packet[i];
      Console->Print( "value[%d] is : %x", i, value );
    }
  }

}//end function

void Trim( char *t )
{
  RTrim( t );
  if ( strlen( t ) )
    LTrim( t );
}

void Trim( std::string *stString )
{
  RTrim( stString );
  if ( stString->length() )
    LTrim( stString );
}

void RTrim( char *t )
{
  char *buf;

  if ( !strlen( t ) ) return;

  buf = t;
  buf += strlen( t ) - 1;

  while ( *buf == ' ' && strlen( t ) )
    *buf-- = '\0';
}

void RTrim( std::string *stString )
{
  int i;       //Count VAR

  if ( !stString->length() )   //If There Is No Length
    return;

  i = stString->length() - 1;   //Set The Counter To The String Length -1

  while ( stString->substr( i, 1 ) == " " )  //While There Is Still " "
  {
    *stString = stString->substr( 0, i );  //Remove Off The End Of The String
    i--;         //Advance The Counter
  }

}

void LTrim( char *t )
{
  char tmp[2048];    //Temp Char VAR
  char *buf;     //Pointer

  if ( !strlen( t ) ) return;

  strncpy( tmp, t, 2048 );
  tmp[2047] = 0;
  buf = tmp;

  while ( *buf == ' ' && strlen( buf ) ) *buf++;

  strncpy( t, buf, 2048 ); // just because without length is forbidden !
}

void LTrim( std::string *stString )
{
  unsigned int i;       //Count VAR
  string buf;      //Temp String VAR

  if ( !stString->length() ) return;    //If The Length Is 0

  i = 0;             //Setup The Counter VAR

  while ( stString->substr( i, 1 ) == " " )     //While " "
    i++;            //Increase Counter

  if ( i == stString->length() )     //If The Whole String Is " "
  {
    *stString = "";          //Return Blank String
    return;
  }

  try
  {
    *stString = stString->substr( i, stString->length() - i ); //Send The String Back Without The Spaces
  }
  catch ( ... )
  {
    *stString = "";          //Return Blank String
    return;
  }

}

std::string GetAccessString( int level )
{
  switch ( level )
  {
    case 0: return "Unregistered";
    case 1: return "Registered";
    case 30: return "Volunteer";
    case 50: return "Gamemaster";
    case 100: return "Administrator";
  }
  return "Error";
}

std::string &Ssprintf( const char *fmt, ... )
{
  static std::string tmpstring;
  char buff[1024];
  va_list args;

  va_start( args, fmt );
  vsnprintf( buff, 1024, fmt, args );
  va_end( args );
  buff[1023] = '\0';
  tmpstring = buff;
  return tmpstring;
}

u16 DistanceApprox( const u16 x1, const u16 y1, const u16 z1, const u16 x2, const u16 y2, const u16 z2 )
{
  u16 DX, DY, DZ, DMax;
  u32 DMinSum, DApprox;

  DMax = DX = ( x1 >= x2 ) ? x1 - x2 : x2 - x1;
  DMinSum = DY = ( y1 >= y2 ) ? y1 - y2 : y2 - y1;
  DZ = ( z1 >= z2 ) ? z1 - z2 : z2 - z1;
  if ( DX < DY )
  {
    DMax = DY;
    DMinSum = DX;
  }
  if ( DMax < DZ )
  {
    DMinSum += DMax;
    DMax = DZ;
  }
  else
  {
    DMinSum += DZ;
  }

  DApprox = DMax + ( u32 )( 0.33 * DMinSum );
  if ( DApprox > 65535 )
  {
    DApprox = 65535;
  }

  // We use Dapprox = max(dx, dy) + K * min(dx, dy)
  // Dapprox = (DX > DY) ? DX + (DY >> 1) : DY + (DX >> 1); // Fastest, but max error on dist is around 10% real dist
  //Dapprox = (u16)((DX > DY) ? DX + 0.33*DY : DY + 0.33*DX); // max error on dist is around 5%, which should be enough
  /*
   // for dist approx debug
  float fDX, fDY, fDZ, fDist;
  fDX=(x1 - x2);
  fDY=(y1 - y2);
  fDZ=(z1 - z2);
  fDist=sqrt(fDX*fDX + fDY*fDY + fDZ*fDZ);
  if (fDist != 0) Console->Print("Dist: %f\tApprox: %d\tError: %d (%d%)", fDist, DApprox, (int)(DApprox-fDist), (int)(100*(DApprox-fDist)/fDist));
  */

  return ( u16 )DApprox;
}

/*** Portable pseudo-random number generator ***/
// until native standardized C++ lib support

u32 mInternalRand = 1;

void InitRandom( u32 nInitialisationValue )
{
  mInternalRand = nInitialisationValue;
}

u16 GetRandom( u16 MaxVal, u16 MinVal )
{
  mInternalRand = mInternalRand * 1103515245 + 12345; //from rand() manpage
  return ( u16 )( MinVal + (( mInternalRand >> 16 ) % 32768 % ( MaxVal - MinVal + 1 ) ) );
}

f32 GetRandomFloat()
{
  mInternalRand = mInternalRand * 1103515245 + 12345; //from rand() manpage
  return (( f32 )(( mInternalRand >> 16 ) % 32768 ) / ( f32 )32768 );
}

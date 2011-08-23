#include "common/misc.h"

#include <cstring>
#include <cstdarg>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common/console.h"

uint32_t IPStringToDWord( const char *IP )
{
  if ( !IP )
    return 0;

  uint32_t a, b, c, d;
  if ( std::sscanf( IP, "%u.%u.%u.%u", &a, &b, &c, &d ) != 4 )
    return 0;

  return ( d << 24 ) | ( c << 16 ) | ( b << 8 ) | a;
}

char *IPlongToString( const uint32_t IP )
{
  struct in_addr in_IP;

  in_IP.s_addr = IP;
  return inet_ntoa( in_IP );
}

//NEW
//this function allow to print a packet
//just for tracking values
void PrintPacket( uint8_t *Packet, int PacketSize )
{
  Console->Print( "inside : PrintPacket" );

  if ( PacketSize < 1 ){Console->Print( "Wrong packet" );}//do nothing
  else
  {
    Console->Print( "PacketSize is : %d", PacketSize );

    uint8_t value = 0;
    for ( int i = 0;i < PacketSize;i++ )
    {
      value = *( uint8_t* ) & Packet[i];
      Console->Print( "value[%d] is : %x", i, value );
    }
  }

}//end function

void CleanUpString(std::string *nString)
{
    if(!nString)
        return;

    // Skip if string is >" "<
    if(nString->length() > 3)
    {
        size_t tfound;
        std::string t_replacechr ("\"");

        tfound = nString->find(t_replacechr);
        while(tfound != std::string::npos)
        {
            nString->replace(tfound, 1, " ");
            tfound = nString->find( t_replacechr, tfound +1 );
        }
        // Skip if string consists of spaces now
        if(strncmp(nString->c_str(), "   ", 3) == 0)
        {
            // Set empty
            *nString = "";
        }
        else
        {
            // Trim spaces
            Trim(nString);
        }
    }
    else
    {
        *nString = "";
    }
}


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
  std::string buf;      //Temp String VAR

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

uint16_t DistanceApprox( const uint16_t x1, const uint16_t y1, const uint16_t z1, const uint16_t x2, const uint16_t y2, const uint16_t z2 )
{
  uint16_t DX, DY, DZ, DMax;
  uint32_t DMinSum, DApprox;

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

  DApprox = DMax + ( uint32_t )( 0.33 * DMinSum );
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

  return ( uint16_t )DApprox;
}

/*** Portable pseudo-random number generator ***/
// until native standardized C++ lib support

uint32_t mInternalRand = 1;

void InitRandom( uint32_t nInitialisationValue )
{
  mInternalRand = nInitialisationValue;
}

uint16_t GetRandom( uint16_t MaxVal, uint16_t MinVal )
{
  mInternalRand = mInternalRand * 1103515245 + 12345; //from rand() manpage
  return ( uint16_t )( MinVal + (( mInternalRand >> 16 ) % 32768 % ( MaxVal - MinVal + 1 ) ) );
}

float GetRandomFloat()
{
  mInternalRand = mInternalRand * 1103515245 + 12345; //from rand() manpage
  return (( float )(( mInternalRand >> 16 ) % 32768 ) / ( float )32768 );
}

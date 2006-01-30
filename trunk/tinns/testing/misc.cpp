#include "tinns.h"

u32 IPStringToDWord(const char *IP)
{
	if(!IP)
		return 0;
	
	u32 a, b, c, d;
	if(std::sscanf(IP, "%u.%u.%u.%u", &a, &b, &c, &d)!=4)
		return 0;
	
	return (d<<24)|(c<<16)|(b<<8)|a;
}

//NEW
//this function allow to print a packet
//just for tracking values
void PrintPacket(u8 *Packet, int PacketSize)
{	
	Console->Print("inside : PrintPacket");

	if (PacketSize<1){Console->Print("Wrong packet");}//do nothing
	else
	{
		Console->Print("PacketSize is : %d",PacketSize);

		u8 value = 0;
		for (int i=0;i<PacketSize;i++)
		{
			value= *(u8*)&Packet[i];
			Console->Print("value[%d] is : %x",i,value);
		}
	}
		 
}//end function


#ifndef MISC_H
#define MISC_H

#ifdef _MSC_VER
	#pragma once
#endif

u32 IPStringToDWord(const char *IP);

void PrintPacket(u8 *Packet, int PacketSize);

#endif


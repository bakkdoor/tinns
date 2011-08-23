#pragma once

#include <iostream>
#include <string>

uint32_t IPStringToDWord(const char *IP);
char *IPlongToString(const uint32_t IP);
std::string GetAccessString(int level);
//void GetSVNRev(char *version);

void PrintPacket(uint8_t *Packet, int PacketSize);

// Cleanup for strings read from .def
void CleanUpString(std::string *nString);
void Trim(char *t);
void Trim(std::string *stString);
void RTrim(char *t);
void RTrim(std::string *stString);
void LTrim(char *t);
void LTrim(std::string *stString);
std::string &Ssprintf(const char *fmt, ...);

uint16_t DistanceApprox(const uint16_t x1, const uint16_t y1, const uint16_t z1, const uint16_t x2, const uint16_t y2, const uint16_t z2);
float Distance(const float x1, const float y1, const float z1, const float x2, const float y2, const float z2);
float Distance(const float x1, const float y1, const float x2, const float y2); // 2D only version

void InitRandom(uint32_t nInitialisationValue);
uint16_t GetRandom(uint16_t MaxVal, uint16_t MinVal = 0); // u16 value between MinVal and MaxVal (inclusive) with max range 32768
float GetRandomFloat(); // f32 value between 0 and 1

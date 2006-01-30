#ifndef TYPES_H
#define TYPES_H

// basic type definitions

#ifdef WIN32
	typedef signed char s8;
	typedef unsigned char u8;
	typedef signed short s16;
	typedef unsigned short u16;
	typedef signed int s32;
	typedef unsigned int u32;
//	#ifdef _MSC_VER
//		typedef signed __int64 s64;
//		typedef unsigned __int64 u64;
//	#endif	// _MSC_VER
	typedef float f32;
	typedef double f64;
#else	// no WIN32
	typedef signed char s8;
	typedef unsigned char u8;
	typedef signed short s16;
	typedef unsigned short u16;
	typedef signed int s32;
	typedef unsigned int u32;
	//typedef signed int64 s64;
	//typedef unsigned int64 u64;
	typedef float f32;
	typedef double f64;
#endif // LINUX

#endif

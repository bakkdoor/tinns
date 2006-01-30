#ifndef SOCKET_H
#define SOCKET_H

#ifdef _MSC_VER
	#pragma once
#endif

static const int RECVBUFFERSIZE = 4096;
static const int SENDBUFFERSIZE = 4096;

class PSocket
{
	protected :
		NLsocket mSocket;
		u8 mReceiveBuffer[RECVBUFFERSIZE];
		u8 mSendBuffer[SENDBUFFERSIZE];
		int mRecvSize, mRecvRewind;
		int mSendSize;
		std::clock_t mLastActive;
		std::clock_t mTimeOutValue;
		NLaddress mRemoteAddr;
		char mLocalIP[32];
	public :
		PSocket(const NLsocket &Socket, const NLaddress *LocalAddr=0, const NLaddress *RemoteAddr=0);
		virtual ~PSocket();

		inline clock_t GetTimeOutValue() const { return mTimeOutValue; }
		inline void SetTimeOutValue(clock_t Value) { mTimeOutValue = Value; }
		
		virtual int Write(const void *Data, int Size);
		virtual int Write(u8 Data);
		virtual int Write(u16 Data);
		virtual int Write(u32 Data);
		virtual int Write(float Data);
		virtual int Write(double Data);
		virtual int Write(const char *String);

		// returns a pointer to the internal receive buffer
		// Size contains the number of octets to read (or 0 to read entire buffer)
		// number of octets available is returned in Size
		virtual const u8 *Read(int *Size);
		inline int GetRecvBufferSize() const { return mRecvSize; }
		inline int GetSendBufferSize() const { return mSendSize; }
		
		// ?TODO: RecvRewind(int);
		virtual void FlushSendBuffer();
		
		virtual bool Update();
		bool TimeOut() const;
		const char *GetLocalAddress();
};

// protocol level socket: Read() returns *complete* game packets
class PGameSocket : public PSocket
{
	public :
		PGameSocket(const NLsocket &Socket);
		// returns a pointer to the internal receive buffer
		// value in Size is ignored (automatically chosen through protocol)
		// Size can be NULL, number of octets available is returned in Size, if !NULL
		const u8 *Read(int *Size);
};

#endif


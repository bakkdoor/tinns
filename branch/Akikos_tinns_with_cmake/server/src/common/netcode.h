#pragma once

#include <iostream>
#include <chrono>
#include <queue>
#include <map>
#include <netinet/in.h>
#include "common/message.h"

#define RECVBUFFERSIZE 4096
#define SENDBUFFERSIZE 4096
#define DEFAULT_TCP_TIMEOUT 600
#define DEFAULT_UDP_TIMEOUT 60
#define SESSION_UDP_OFFSET 37917
#define MAX_RETENTION 20

class ConnectionTCP;
class ConnectionUDP;
class ServerSocket;

class ConnectionTCP {
private:
	int32_t mSockfd;
	struct sockaddr_in mRemoteAddr;
	time_t mLastActive;
	time_t mTimeOutValue;
	bool mbConnected;
	ServerSocket *mServerSocket; // pointer to ServerSocket
	std::queue<PMessage *> mQueueIn;
	std::queue<PMessage *> mQueueOut;
	PMessage *mReceiveBufferMsg;
	// old stuff
	PMessage *mSendBufferMsg;

public:
	ConnectionTCP(int32_t sockfd, struct sockaddr_in addr, ServerSocket *server);
	~ConnectionTCP();

	struct sockaddr_in getAddr()
	{
		return mRemoteAddr;
	}
	int32_t getSockfd()
	{
		return mSockfd;
	}
	char *getRemoteAddress();
	void setServer(ServerSocket *server)
	{
		if (server)
			mServerSocket = server;
	}
	bool timeOut() const;
	inline time_t GetTimeOutValue() const
	{
		return mTimeOutValue;
	}
	inline void SetTimeOutValue(time_t Value)
	{
		mTimeOutValue = Value;
	}
	bool update();
	bool isConnected()
	{
		return mbConnected;
	}
	inline void SendMessage(PMessage *nMessage)
	{
		mQueueOut.push(nMessage);
	}
	PMessage *GetMessage();
	void DeleteOutgoingMessages();
	// old stuff
	void flushSendBuffer();
	int32_t getRecvBufferSize();
	int32_t getSendBufferSize();
	const uint8_t *read(int32_t *size);
	int32_t write(const void *data, int32_t size);
	int32_t write(uint8_t data);
	int32_t write(uint16_t data);
	int32_t write(uint32_t data);
	int32_t write(float data);
	int32_t write(double data);
	int32_t write(const char *string);
};

class ConnectionUDP {
	typedef std::map<uint16_t, PMessage *> PMessageMap;
private:
	int32_t mSockfd;
	struct sockaddr_in mRemoteAddr;
	time_t mLastActive;
	time_t mTimeOutValue;
	int32_t mPort;
	ServerSocket *mServerSocket;
	std::queue<PMessage *> mQueueIn;
	std::queue<PMessage *> mQueueOut;
	std::queue<PMessage *> mVIPQueueOut;
	// UDP MessageBuffer
	uint16_t mUDP_ID;
	uint16_t mLastUDPID;
	uint16_t mSessionID;
	uint16_t mTransactionID;
	PMessageMap UDPMessages;
	PMessageMap::iterator GetMsgListBegin()
	{
		return UDPMessages.begin();
	}
	PMessageMap::iterator GetMsgListEnd()
	{
		return UDPMessages.end();
	}
	void InsertUDPMessage(PMessage *nMsg); // save message for later OOO handling
	void UpdateMessageBuffer(); // delete old packets, depending on define "MAX_RETENTION"
	void ResetMessageBuffer(); // done whe UDP_ID gets set to zero
	// old stuff
	PMessage *mSendBufferMsg;

public:
	ConnectionUDP(int32_t sockfd, int32_t port, int32_t remoteaddress, int32_t remoteport, ServerSocket *server);
	~ConnectionUDP();

	bool update();
	int32_t getPort()
	{
		return mPort;
	}
	struct sockaddr_in getAddr()
	{
		return mRemoteAddr;
	}
	int32_t getSockfd()
	{
		return mSockfd;
	}
	char *getRemoteAddress();
	bool timeOut() const;
	inline time_t getTimeOutValue() const
	{
		return mTimeOutValue;
	}
	inline void setTimeOutValue(time_t Value)
	{
		mTimeOutValue = Value;
	}
	void SendMessage(PMessage *nMessage, bool nVIP = false);
	inline int32_t GetReadyMessageNumber()
	{
		return mQueueIn.size();
	}
	PMessage *GetMessage();
	void DeleteOutgoingMessages();
	// MessageBuffer
	void ReSendUDPMessage(uint16_t nUDP_ID); // OOO
	inline uint16_t GetUDP_ID() const
	{
		return mUDP_ID;
	}
	inline uint16_t GetSessionID() const
	{
		return SESSION_UDP_OFFSET + mUDP_ID;
	}
	inline uint16_t GetTransactionID()
	{
		return mTransactionID;
	}
	void SetUDP_ID(uint16_t id);
	inline void IncreaseUDP_ID()
	{
		SetUDP_ID(mUDP_ID + 1);
	}
	inline void ResetTransactionID()
	{
		mTransactionID = 10170;
	}
	inline void IncreaseTransactionID(uint8_t nInc = 1)
	{
		mTransactionID += nInc;
	}
	// old stuff
	int32_t getRecvBufferSize();
	int32_t getSendBufferSize();
	void flushSendBuffer();
	const uint8_t *read(int32_t *size);
	int32_t write(const void *data, int32_t size);
	int32_t write(uint8_t data);
	int32_t write(uint16_t data);
	int32_t write(uint32_t data);
	int32_t write(float data);
	int32_t write(double data);
	int32_t write(const char *string);
};

class ServerSocket {
private:
	fd_set m_MainSetTCP; // master file descriptor list for tcp-connections
	fd_set m_ReadSetTCP; // temp file descriptor list for select() for tcp-connections
	fd_set m_MainSetUDP; // master file descriptor list for udp-connections
	fd_set m_MainSetGlobal; // master file descriptor list for udp+tcp connections
	struct sockaddr_in m_ServerAddr;
	int32_t m_ListenerTCP; // listen socket
	int32_t m_FdMaxTCP; // highest current file-descriptor (tcp)
	int32_t m_FdMaxUDP; // highest current file-descriptor (udp)
	bool m_bNewTCPConnection;
	struct timeval m_TimeOut;

public:
	ServerSocket();
	~ServerSocket();
	void settimeout(int32_t timeout_sec, int32_t timeout_usec);
	bool open(int32_t port);
	void update();
	bool newConnection();
	ConnectionTCP *getTCPConnection();
	ConnectionUDP *getUDPConnection(uint32_t remoteaddress, int32_t remoteport);
	bool isDataAvailable(int32_t sockfd);
	void delSocketFromSet(int32_t sockfd);
	void closeServer();
};

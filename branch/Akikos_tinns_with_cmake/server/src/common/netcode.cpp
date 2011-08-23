#include "common/netcode.h"

#include <cstring>
#include <fcntl.h>
#include <arpa/inet.h>
#include "common/console.h"
#include "common/config.h"

ConnectionTCP::ConnectionTCP(int sockfd, struct sockaddr_in addr, ServerSocket* server)
{
    mSockfd = sockfd;
    mRemoteAddr = addr;
    
    if(server)
    {
        mServerSocket = server;
    }

    // set new socket to non-blocking
    fcntl(mSockfd, F_SETFL, O_NONBLOCK);
            
    mbConnected = true; // client obviously is connected at creation...

    mTimeOutValue = DEFAULT_TCP_TIMEOUT;
    mLastActive = std::time(NULL);
      
    mReceiveBufferMsg = NULL;
    mSendBufferMsg = NULL;
}

ConnectionTCP::~ConnectionTCP()
{
    if (mServerSocket)
    {
      mServerSocket->delSocketFromSet(mSockfd);
    }
    close(mSockfd);
    
    if (mReceiveBufferMsg)
    {
      delete mReceiveBufferMsg;
    }
    
    while (!mQueueIn.empty())
    {
      delete mQueueIn.front();
      mQueueIn.pop();
    }

    if (mSendBufferMsg)
    {
      delete mSendBufferMsg;
    }
}

char* ConnectionTCP::getRemoteAddress()
{
    return inet_ntoa(mRemoteAddr.sin_addr);
}

bool ConnectionTCP::timeOut() const
{
	time_t now = std::time(NULL);
	if((now-mLastActive) >= mTimeOutValue)
		return true;

	return false;
}

PMessage* ConnectionTCP::GetMessage()
{
  PMessage* RetVal;
  
  if (mQueueIn.empty())
    RetVal = NULL;
  else
  {
    RetVal = mQueueIn.front();
    mQueueIn.pop();
  }
  return RetVal;
}

void ConnectionTCP::DeleteOutgoingMessages()
{
  while (! mQueueOut.empty())
  {
    delete mQueueOut.front();
    mQueueOut.pop();
  }  
}

bool ConnectionTCP::update() // non optimal read-algorithm atm, but well ... :p
{
  PMessage* tmpMsg;
  int numBytes;
  uint8_t const* DataStart;
  uint16_t DataSize;
  uint8_t* MsgStart;
  uint16_t MsgOffset;
  uint16_t MsgLen;
  
  //check if data is available for this socket and if yes, read into a new PMessage and put it on incoming queue
  if(mServerSocket->isDataAvailable(mSockfd))
  {
//Console->Print("ConnectionTCP::update() - IN Data avail");
    if (mReceiveBufferMsg == NULL)
    {
      mReceiveBufferMsg = new PMessage(RECVBUFFERSIZE);
    }
    
    DataSize = mReceiveBufferMsg->GetSize();
    numBytes = recv(mSockfd, (char*) mReceiveBufferMsg->GetMessageDataPointer(RECVBUFFERSIZE - DataSize) + DataSize, RECVBUFFERSIZE - DataSize, 0); // get the data

    if(numBytes > 0)
    {
//Console->Print(GREEN, BLACK, "ConnectionTCP::update() - Data received");
      mbConnected = true;
      mReceiveBufferMsg->ForceSize(DataSize + numBytes);
      mLastActive = std::time(NULL);
      
      while(mReceiveBufferMsg && mReceiveBufferMsg->GetSize())
      {
        DataStart = mReceiveBufferMsg->GetMessageData();
        DataSize = mReceiveBufferMsg->GetSize();
        MsgStart = (uint8_t*)memchr(DataStart, 0xfe, DataSize);
        
        if (MsgStart)
        {
          MsgOffset = MsgStart - DataStart;
          if (MsgOffset)
          {
            Console->Print(YELLOW, BLACK, "ConnectionTCP::update() Message head found 0x%04hx bytes after packet start. Inbetween data will be discarded.", MsgOffset);
          }
          if (MsgOffset + 3 <= DataSize)
          {
            MsgLen = *(uint16_t*)(DataStart + MsgOffset + 1);
//Console->Print(GREEN, BLACK, "ConnectionTCP::update() TCP Message body length 0x%04hx (buffer 0x%04hx)", MsgLen, mReceiveBufferMsg->GetSize());
            if (MsgOffset + 3 + MsgLen <= DataSize)
            {
              tmpMsg = mReceiveBufferMsg->GetChunk(MsgOffset, MsgLen + 3); // Change (MsgOffset, MsgLen + 3) to (MsgOffset + 3, MsgLen) to REMOVE head & length from message head
              mQueueIn.push(tmpMsg);
              
              if (MsgOffset + 3 + MsgLen < DataSize)
              {
                 tmpMsg = mReceiveBufferMsg->GetChunk(MsgOffset + 3 + MsgLen, DataSize - (MsgOffset + 3 + MsgLen));
              }
              else
              {
                tmpMsg = NULL;
              }
              
              delete mReceiveBufferMsg;
              mReceiveBufferMsg = tmpMsg;              
            }
          }
        }
        else
        {
          break;
        }
      }
    }
    else if(numBytes == 0) // disconnected !!
    {
        mbConnected = false; // set to not-connected and close the socket
        if (mServerSocket)
        {
          mServerSocket->delSocketFromSet(mSockfd);
        }
        close(mSockfd);        
        return false;
    }
    else
    {
      if(errno != EAGAIN) 
      {                    // an error has occured -> output it to the console
        perror("tcp-receive");
        Console->Print(RED, BLACK, "mSockfd:%d MaxRead:%d ", mSockfd, RECVBUFFERSIZE);
      }
    }           
  }

  // send data from outgoing queue
  flushSendBuffer(); // manage old write compatibility
	while(! mQueueOut.empty())
	{
//Console->Print("ConnectionUDP::update() - OUT Data avail");
	  tmpMsg = mQueueOut.front();
		int numBytes = send(mSockfd, (char*) tmpMsg->GetMessageData(), tmpMsg->GetSize(), 0);
		if(numBytes == -1) // error while sending data -> output error-msg to console
		{
		  if (errno == EAGAIN)
		  {
		    break;
		  }
		  else
		  {
		    perror("tcp-send");
		    //close(mSockfd);
        return false;
      }
    }
    else if(numBytes > 0)
		{
//Console->Print(GREEN, BLACK, "ConnectionTCP::update() - Data sent");
		    mLastActive = std::time(NULL);
        mQueueOut.pop(); // message written, we can remove it from queue
        delete tmpMsg; // and delete the message
		}
	}

	return true;
}

/**** Old I/F compatibility functions ****/

int ConnectionTCP::getRecvBufferSize()
{
  PMessage* tmpMsg;
  
  if (mQueueIn.empty())
    return 0;

  tmpMsg = mQueueIn.front();
	uint16_t _size = tmpMsg->GetSize()-tmpMsg->GetNextByteOffset();
	if (_size <= 0)
	{
	  mQueueIn.pop();
	  delete tmpMsg;
    if (mQueueIn.empty())
      return 0;
    tmpMsg = mQueueIn.front();
	  tmpMsg->SetNextByteOffset(0);
	  _size = tmpMsg->GetSize();
	}
	return _size;
}

int ConnectionTCP::getSendBufferSize()
{
  if(mSendBufferMsg == NULL)
    return 0;
  else
    return mSendBufferMsg->GetSize();
}

void ConnectionTCP::flushSendBuffer()
{
//Console->Print("ConnectionTCP::flushSendBuffer()");
    if((mSendBufferMsg != NULL) && (mSendBufferMsg->GetSize() > 0))
    {
      SendMessage(mSendBufferMsg);
      mSendBufferMsg = NULL;
//Console->Print(YELLOW, BLACK, "ConnectionTCP::flushSendBuffer() - Data flushed");
    }
}

const uint8_t* ConnectionTCP::read(int* size)
{
  PMessage* tmpMsg;
//Console->Print("ConnectionTCP::read() - trying to read up to %d bytes", *size);  
  if (mQueueIn.empty() || !size)
  {
//Console->Print("ConnectionTCP::read() - no more packet");
    return NULL;
  }

  tmpMsg = mQueueIn.front();
	uint16_t _size = tmpMsg->GetSize()-tmpMsg->GetNextByteOffset();
//Console->Print("ConnectionTCP::read() - %d bytes remaining in current packet", _size);
	if (_size <= 0)
	{
//Console->Print("ConnectionTCP::read() - trying next packet");
	  mQueueIn.pop();
	  delete tmpMsg;
    if (mQueueIn.empty())
    {
//Console->Print("ConnectionUDP::read() - no more packet");
      return NULL;
     }
    tmpMsg = mQueueIn.front();
	  _size = tmpMsg->GetSize();
	  tmpMsg->SetNextByteOffset(0);
	}
	
	if(*size==0)
	{
		*size=_size;
	}
	else
	{
		*size = std::min(*size, (int32_t)_size);
	}

	uint8_t const* ptr = tmpMsg->GetMessageData() + tmpMsg->GetNextByteOffset();
	tmpMsg->SetNextByteOffset(tmpMsg->GetNextByteOffset()+ *size);
//Console->Print(GREEN, BLACK, "ConnectionTCP::read() - %d bytes read", *size);
	return ptr;
}

int ConnectionTCP::write(const void* data, int size)
{
  // data is stored in mSendBufferMsg. Gets queued in next flushSendBuffer() or update()
  if (mSendBufferMsg == NULL)
  {
//Console->Print("ConnectionTCP::write() creating new mSendBufferMsg");
    mSendBufferMsg = new PMessage(SENDBUFFERSIZE);
  }
  
	mSendBufferMsg->Write(data, (uint16_t)size);
//Console->Print(GREEN, BLACK, "ConnectionUDP::write() %d bytes written to mSendBufferMsg (total size %d)", size, mSendBufferMsg->GetSize());
	return size;
}

int ConnectionTCP::write(const char *String)
{
	if(!String)
		return 0;

	return write(String, strlen(String));
}

int ConnectionTCP::write(uint8_t Data)
{
	return write(&Data, sizeof(uint8_t));
}

int ConnectionTCP::write(uint16_t Data)
{
	return write(&Data, sizeof(uint16_t));
}

int ConnectionTCP::write(uint32_t Data)
{
	return write(&Data, sizeof(uint32_t));
}

int ConnectionTCP::write(float Data)
{
	return write(&Data, sizeof(float));
}

int ConnectionTCP::write(double Data)
{
	return write(&Data, sizeof(double));
}

ConnectionUDP::ConnectionUDP(int sockfd, int port, int remoteadress, int remoteport, ServerSocket* server)
{
    mSockfd = sockfd;
    mPort = port;
    //Console->Print("local UDP port: %d", port);
    if(server)
    {
        mServerSocket = server;
    }

    mTimeOutValue = DEFAULT_UDP_TIMEOUT;
    mLastActive = std::time(NULL);

    mRemoteAddr.sin_family = AF_INET;       // host byte order
    mRemoteAddr.sin_port = htons(remoteport);     // short, network byte order
    mRemoteAddr.sin_addr.s_addr = remoteadress;   // TODO: Get IP of client

    //Bind client to server in a udp pseudo-connection
    /*if(connect(sockfd, (struct sockaddr *)&mRemoteAddr, sizeof(struct sockaddr )))
    {
      Console->Print("Error on pseudo-connecting udp socket to %s:%d", inet_ntoa(mRemoteAddr.sin_addr),ntohs(mRemoteAddr.sin_port));
      perror("udp connect"); //exception should be thrown here
    }
    else */
    {
        Console->Print("Client UDP %s:%d", inet_ntoa(mRemoteAddr.sin_addr), ntohs(mRemoteAddr.sin_port));
    }
    // set UDP-socket to non-blocking
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    //    mRemoteAddr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    //    memset(&(mRemoteAddr.sin_zero), '\0', 8); // zero the rest of the struct
    mSendBufferMsg = NULL;

	mUDP_ID = 0;
	mSessionID = SESSION_UDP_OFFSET;
    mLastUDPID = 0;
    mTransactionID = 0;
}


ConnectionUDP::~ConnectionUDP()
{
    if (mServerSocket)
    {
        mServerSocket->delSocketFromSet(mSockfd);
    }
    close(mSockfd);

    if (mSendBufferMsg)
    {
        delete mSendBufferMsg;
    }

    while (!mQueueIn.empty())
    {
        delete mQueueIn.front();
        mQueueIn.pop();
    }

    while (!mQueueOut.empty())
    {
        delete mQueueOut.front();
        mQueueOut.pop();
    }
    while (!mVIPQueueOut.empty())
    {
        delete mVIPQueueOut.front();
        mVIPQueueOut.pop();
    }
    for(PMessageMap::iterator it=UDPMessages.begin(); it!=UDPMessages.end(); it++)
    {
        delete it->second;
        UDPMessages.erase(it);
    }
}

void ConnectionUDP::InsertUDPMessage(PMessage* nMsg)
{
    if (!nMsg)
        return;

    if(nMsg->U8Data(0) != 0x13) return; // Only add real UDP messages here
//  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25 26 27 28
// 13 07 00 F2 00 05 03 02 00 11 11 05 03 03 00 11 11 05 03 04 00 11 11 05 03 05 00 11 11 05 03 06 00 11 11 05 03 07 00 11 11

    // Grab submessages from packet, check if message is 0x03 commandset. If not, dont add message
    PMessage* tWorkMsg = NULL;

    uint16_t tCurPos = 5;
    uint8_t tSubMsgLen = 0;
    uint16_t tmpUDPID = 0;
    //nMsg->Dump();
    while(tCurPos < (nMsg->GetSize() - 1)) // Loop while we still have more frames. (-1: GetSize starts at 1, pos pointer at 0)
    {
        //Console->Print("tCurPos = %d  nMsg->GetSize = %d", tCurPos, nMsg->GetSize());
        tSubMsgLen = nMsg->U8Data(tCurPos) + 1;          // Get the lenght of the frame, and add the lenght byte
        if(nMsg->U8Data(tCurPos+1) != 0x03)
        {
            //Console->Print("Ignoring UDP message, no 0x03 commandset");
            tCurPos += tSubMsgLen;                       // Set pointer to the end of this frame
            continue;                                    // Skip if frame is not an 0x03 commandset
        }
        //Console->Print("MsgLen: %d", tSubMsgLen);
        tWorkMsg = nMsg->GetChunk(tCurPos, tSubMsgLen); // get the frame.
        //Console->Print("Msg:");
        //tWorkMsg->Dump();
        tmpUDPID = nMsg->U16Data(tCurPos + 2);                     // Get the UDP ID of this frame
        //Console->Print("UDP ID: %d", tmpUDPID);
        PMessageMap::const_iterator it = UDPMessages.find(tmpUDPID);                 // Try to find the UDP ID in the queue
        if(it->second)                                   // If we already have this UDP msg, print error
        {
            Console->Print("%s Packet *NOT* added to history buffer, UdpID %d already sent! (This may cause an OOO)", Console->ColorText(RED, BLACK, "[WARNING]"), tmpUDPID);
            nMsg->Dump();
        }
        else                                            // We dont have this msg? Add it!
        {
            //Console->Print("Added UDP ID %d to messagebuffer", tmpUDPID);
            UDPMessages.insert(std::make_pair(tmpUDPID, tWorkMsg));
            mLastUDPID++;
        }
        tCurPos += tSubMsgLen;                      // Set pointer to the end of this frame
    }
}

void ConnectionUDP::UpdateMessageBuffer()
{
    int erasednum = 0;
    // Delete all old messages
    for(PMessageMap::iterator it=UDPMessages.begin(); it!=UDPMessages.end(); it++)
    {
        if((int)it->first < (int)(mLastUDPID - MAX_RETENTION))
        {
            erasednum++;
            delete it->second;
            UDPMessages.erase(it);
        }
    }
    /* // Debug output
    if(erasednum > 0)
        Console->Print("[UpdateMessageBuffer] Done updating messagequeue, %d entries deleted", erasednum);
    */
}

void ConnectionUDP::ResetMessageBuffer()
{
    if(mUDP_ID != 0)
    {
        Console->Print("%s MessageQueue got erased but UDP_ID is still >0", Console->ColorText(RED, BLACK, "[WARNING]"));
    }
    for(PMessageMap::iterator it=UDPMessages.begin(); it!=UDPMessages.end(); it++)
    {
        delete it->second;
        UDPMessages.erase(it);
    }
    mLastUDPID = 0;
//    Console->Print("[udpmanager] Erased messagebuffer");
}

void ConnectionUDP::ReSendUDPMessage(uint16_t nUDP_ID)
{
    // ReSend packet with given UDP_ID
    if(nUDP_ID > mLastUDPID)
    {
        Console->Print("%s Cannot resend packet with UDP_ID %d, msgnumber is higher than last known udpID", Console->ColorText(RED, BLACK, "[PANIC]"), nUDP_ID);
    }
    else
    {
        // UDP_ID seems to be valid, now search for it
        PMessageMap::const_iterator it = UDPMessages.find(nUDP_ID);
        if(it == UDPMessages.end())
        {
            int dynRetention = (int)mLastUDPID - nUDP_ID;
            if(dynRetention > MAX_RETENTION)
            {
                Console->Print("%s Packet with UDP_ID %d not found. Increase #define MAX_RETENTION to at least %d", Console->ColorText(RED, BLACK, "[WARNING]"), nUDP_ID, dynRetention);
            }
            else
            {
                Console->Print("%s Packet with UDP_ID %d is missing in the packet queue!", Console->ColorText(RED, BLACK, "[PANIC]"), nUDP_ID);
            }
            Console->Print("Trying to cancel OOO notice by sending dummy packet");
            PMessage* tmpMsg = new PMessage(14);
            //u16 tmpSessionID = mLastUDPID + SESSION_UDP_OFFSET;
            *tmpMsg << (uint8_t)0x13;
            *tmpMsg << nUDP_ID;
            *tmpMsg << (uint16_t)(nUDP_ID + SESSION_UDP_OFFSET);
            *tmpMsg << (uint8_t)0x08;
            *tmpMsg << (uint8_t)0x03;
            *tmpMsg << nUDP_ID;
            *tmpMsg << (uint8_t)0x1F;
            *tmpMsg << (uint16_t)0xFFFF;     // Should do nothing, CharID 65535 should never exist
            *tmpMsg << (uint16_t)0x3C01;     // This value IS wrong way, so that nothing can happen at all
            SendMessage(tmpMsg, true);
        }
        else if(it->second)
        {
            Console->Print("[OOO-Buster] ReSending UDP packet with ID %d", nUDP_ID);
            // Build new message, including the missing UDP packets as content
            uint16_t MsgSize = it->second->GetSize();
            PMessage* tmpMsg = new PMessage(MsgSize + 5); // Create new message
            *tmpMsg << (uint8_t)0x13;
            *tmpMsg << nUDP_ID;
            *tmpMsg << (uint16_t)(nUDP_ID + SESSION_UDP_OFFSET);
//            *tmpMsg << mUDP_ID;
//            *tmpMsg << mSessionID;
//            *tmpMsg << *it->second; // This should work, but it doesnt! Causes segfault after sending a few packets
            for(int x = 0; x < MsgSize; x++)
            {
                *tmpMsg << it->second->U8Data(x);
            }
            SendMessage(tmpMsg, true);  // Add message to outgoing VIP queue
        }
    }
}

bool ConnectionUDP::timeOut() const
{
    time_t now = std::time(NULL);
    if((now-mLastActive) >= mTimeOutValue)
        return true;

    return false;
}

char* ConnectionUDP::getRemoteAddress()
{
    return inet_ntoa(mRemoteAddr.sin_addr);
}

PMessage* ConnectionUDP::GetMessage()
{
    PMessage* RetVal;

    if (mQueueIn.empty())
        RetVal = NULL;
    else
    {
        RetVal = mQueueIn.front();
        mQueueIn.pop();
    }
    return RetVal;
}

void ConnectionUDP::DeleteOutgoingMessages()
{
    while (! mQueueOut.empty())
    {
        PMessage* tmpMsg;
        tmpMsg = mQueueOut.front();
        mQueueOut.pop();
        delete tmpMsg;
    }
    while (! mVIPQueueOut.empty())
    {
        PMessage* tmpMsg2;
        tmpMsg2 = mVIPQueueOut.front();
        mVIPQueueOut.pop();
        delete tmpMsg2;
    }
}

bool ConnectionUDP::update()
{
    PMessage* tmpMsg;
    int numBytes;
    bool gotVIPmsg;

    // send data from outgoing queue
    flushSendBuffer(); // manage old write compatibility

    // Update messagebuffer; Erase all messages older than MAX_RETENTION
    UpdateMessageBuffer();

    if (! mQueueOut.empty() || ! mVIPQueueOut.empty())
    {
        //Console->Print(GREEN, BLACK, "ConnectionUDP::update() - Sending messages -----------------");
        //  Console->Print("ConnectionUDP::update() - %d messages waiting in Output Queue", mQueueOut.size());
        while(! mQueueOut.empty() || ! mVIPQueueOut.empty())
        {
            //Console->Print("ConnectionUDP::update() - OUT Data avail");

            // First, take a look at the VIP Query. If not empty, send these packets first
            gotVIPmsg = false;
            if(! mVIPQueueOut.empty())
            {
                tmpMsg = mVIPQueueOut.front();
                gotVIPmsg = true;
                Console->Print("ConnectionUDP::update() - Got VIP (Very important packet) that is waiting to be sent");
            }
            else
            {
                tmpMsg = mQueueOut.front();

                // We ignore VIP packets for now. They are only meant to OOO packets
                InsertUDPMessage(tmpMsg);
            }
            //int numBytes = send(mSockfd, tmpMsg->GetMessageData(), tmpMsg->GetSize(), 0);
            int numBytes = sendto(mSockfd, (char*) tmpMsg->GetMessageData(), tmpMsg->GetSize(), 0, (struct sockaddr *)&mRemoteAddr, sizeof(struct sockaddr));
            if(numBytes > 0)
            {
                //Console->Print(GREEN, BLACK, "ConnectionUDP::update() - Data sent");
                mLastActive = std::time(NULL);
                if(gotVIPmsg == true)
                {
                    mVIPQueueOut.pop();
                }
                else
                {
                    mQueueOut.pop(); // message written, we can remove it from queue
                }
                //Console->Print(GREEN, BLACK, "ConnectionUDP::update() - Message sent:");
                //tmpMsg->DumpHead("OUT Msg:"); // ====
                //tmpMsg->Dump();
                delete tmpMsg; // and delete the message
            }
            else
            {
                if (errno == EAGAIN)
                {
                    break;
                }
                else // error while sending data -> output error-msg to console
                {
                    perror("udp-send2");
                    //close(mSockfd);
                    return false;
                }
            }
        }
        //if (! mQueueOut.empty())
        //  Console->Print(YELLOW, BLACK, "ConnectionUDP::update() - %d messages remaining in Output Queue", mQueueOut.size());
    }

    //check if data is available from this socket and if yes, read into a new PMessage and put it on incoming queue
    if(mServerSocket->isDataAvailable(mSockfd))
    {
        //Console->Print("ConnectionUDP::update() - IN Data avail");
        while (1)
        {
            tmpMsg = new PMessage(RECVBUFFERSIZE);
            socklen_t addrlen;
            addrlen = sizeof(mRemoteAddr);
            //struct sockaddr_in tempAddr;  // need to built in check,
            // if the incoming data is coming from the client or someone else!
            //numBytes = recv(mSockfd, tmpMsg->GetMessageDataPointer(RECVBUFFERSIZE), RECVBUFFERSIZE, 0); // get the data
            numBytes = recvfrom(mSockfd, (char*) tmpMsg->GetMessageDataPointer(RECVBUFFERSIZE), RECVBUFFERSIZE, 0, (struct sockaddr *)&mRemoteAddr, &addrlen);
            if(numBytes > 0)
            {
                //Console->Print(GREEN, BLACK, "ConnectionUDP::update() - Data received");
                mLastActive = std::time(NULL);
                tmpMsg->ForceSize(numBytes);
                mQueueIn.push(tmpMsg);
                //tmpMsg->DumpHead("IN Msg :"); // ====
            }
            else
            {
                delete tmpMsg;
                if(errno != EAGAIN)
                {                    // an error has occured -> output it to the console
                    perror("udp-receive");
                    Console->Print(RED, BLACK, "mSockfd:%d MaxRead:%d ", mSockfd, RECVBUFFERSIZE);
                }
                break;
            }
        }
        //Console->Print("ConnectionUDP::update() - %d messages ready in Input Queue", mQueueIn.size());
    }
    return true;
}

void ConnectionUDP::SendMessage(PMessage* nMessage, bool nVIP)
{
    if (nMessage)
    {
        if(nVIP == true)
            mVIPQueueOut.push(nMessage);
        else
            mQueueOut.push(nMessage);
    }
}

/**************** Old I/F compatibility stuff ******************/

int ConnectionUDP::getRecvBufferSize()
{
    PMessage* tmpMsg;

    if (mQueueIn.empty())
        return 0;

    tmpMsg = mQueueIn.front();
    uint16_t _size = tmpMsg->GetSize()-tmpMsg->GetNextByteOffset();
    if (_size <= 0)
    {
        mQueueIn.pop();
        delete tmpMsg;
        if (mQueueIn.empty())
            return 0;
        tmpMsg = mQueueIn.front();
        tmpMsg->SetNextByteOffset(0);
        _size = tmpMsg->GetSize();
    }
    return _size;
}

int ConnectionUDP::getSendBufferSize()
{
    if(mSendBufferMsg == NULL)
        return 0;
    else
        return mSendBufferMsg->GetSize();
}

void ConnectionUDP::flushSendBuffer()
{
    //Console->Print("ConnectionUDP::flushSendBuffer()");
    if((mSendBufferMsg != NULL) && (mSendBufferMsg->GetSize() > 0))
    {
        SendMessage(mSendBufferMsg);
        mSendBufferMsg = NULL;
        //Console->Print(YELLOW, BLACK, "ConnectionUDP::flushSendBuffer() - Data flushed");
    }
}

const uint8_t *ConnectionUDP::read(int *size)
{
    PMessage* tmpMsg;
    //Console->Print("ConnectionUDP::read() - trying to read up to %d bytes", *size);
    if (mQueueIn.empty() || !size)
    {
        //Console->Print("ConnectionUDP::read() - no more packet");
        return NULL;
    }

    tmpMsg = mQueueIn.front();
    uint16_t _size = tmpMsg->GetSize()-tmpMsg->GetNextByteOffset();
    //Console->Print("ConnectionUDP::read() - %d bytes remaining in current packet", _size);
    if (_size <= 0)
    {
        //Console->Print("ConnectionUDP::read() - trying next packet");
        mQueueIn.pop();
        delete tmpMsg;
        if (mQueueIn.empty())
        {
            //Console->Print("ConnectionUDP::read() - no more packet");
            return NULL;
        }
        tmpMsg = mQueueIn.front();
        _size = tmpMsg->GetSize();
        tmpMsg->SetNextByteOffset(0);
    }

    if(*size==0)
    {
        *size=_size;
    }
    else
    {
        *size = std::min(*size, (int32_t)_size);
    }

    uint8_t const *ptr = tmpMsg->GetMessageData() + tmpMsg->GetNextByteOffset();
    tmpMsg->SetNextByteOffset(tmpMsg->GetNextByteOffset()+ *size);
    //Console->Print(GREEN, BLACK, "ConnectionUDP::read() - %d bytes read", *size);
    return ptr;
}

int ConnectionUDP::write(const void* data, int size)
{
    // data is stored in mSendBufferMsg. Gets queued in next flushSendBuffer() or update()
    if (mSendBufferMsg == NULL)
    {
        //Console->Print("ConnectionUDP::write() creating new mSendBufferMsg");
        mSendBufferMsg = new PMessage(SENDBUFFERSIZE);
    }
    mSendBufferMsg->Write(data, (uint16_t)size);
    //Console->Print(GREEN, BLACK, "ConnectionUDP::write() %d bytes written to mSendBufferMsg (total size %d)", size, mSendBufferMsg->GetSize());
    return size;
}

int ConnectionUDP::write(const char *String)
{
    if(!String)
        return 0;

    return write(String, strlen(String));
}

int ConnectionUDP::write(uint8_t Data)
{
    return write(&Data, sizeof(uint8_t));
}

int ConnectionUDP::write(uint16_t Data)
{
    return write(&Data, sizeof(uint16_t));
}

int ConnectionUDP::write(uint32_t Data)
{
    return write(&Data, sizeof(uint32_t));
}

int ConnectionUDP::write(float Data)
{
    return write(&Data, sizeof(float));
}

int ConnectionUDP::write(double Data)
{
    return write(&Data, sizeof(double));
}

/// ***********************************************

void ConnectionUDP::SetUDP_ID(uint16_t id)
{
    if (mUDP_ID == 0xffff)
    {
        mUDP_ID = 0;
    }
    else
    {
        mUDP_ID = id;
    }
    if(mUDP_ID == 0)        // If UDPID is set to zero, erase message buffer too
        ResetMessageBuffer();
}

ServerSocket::ServerSocket()
{
    FD_ZERO(&m_MainSetTCP);    // clear the master and temp sets
    FD_ZERO(&m_ReadSetTCP);
    FD_ZERO(&m_MainSetUDP);
    FD_ZERO(&m_MainSetGlobal);
    
    m_FdMaxUDP=0;
    
    m_TimeOut.tv_sec = 0;
    m_TimeOut.tv_usec = 30;
    
    //m_LastUDPPort = 5000;
}

ServerSocket::~ServerSocket()
{
    close(m_ListenerTCP);
}

void ServerSocket::settimeout(int32_t timeout_sec, int32_t timeout_usec)
{
  if ((timeout_sec >= 0) && (timeout_usec >= 0))
  {
    m_TimeOut.tv_sec = timeout_sec;
    m_TimeOut.tv_usec = timeout_usec;
  }
    
}

bool ServerSocket::open(int port)
{
    // get the tcp listener
    if ((m_ListenerTCP = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("tcp-socket");
        return false;
    }

    int yes=1;

    // lose the pesky "address already in use" error message
    if (setsockopt(m_ListenerTCP, SOL_SOCKET, SO_REUSEADDR, &yes,
                                                        sizeof(int)) == -1) {
        perror("tcp-setsockopt");
        return false;
    }

    // bind
    memset(&m_ServerAddr,0,sizeof(struct sockaddr_in));
    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_addr.s_addr = INADDR_ANY;
    m_ServerAddr.sin_port = htons(port);

    // bind with tcp sockfd
    if (bind(m_ListenerTCP, (struct sockaddr *)&m_ServerAddr, sizeof(struct sockaddr)) == -1)
    {
        perror("tcp-bind");
        return false;
    }

    // set listening-socket to non-blocking
    fcntl(m_ListenerTCP, F_SETFL, O_NONBLOCK);

    // listen
    if (listen(m_ListenerTCP, 10) == -1)
    {
        perror("tcp-listen");
        return false;
    }

    FD_ZERO(&m_MainSetTCP);
    FD_ZERO(&m_ReadSetTCP);
    FD_ZERO(&m_MainSetUDP);
    FD_ZERO(&m_MainSetGlobal);
    
    m_FdMaxUDP=0;
    
    // add the listener to the master sets
    FD_SET(m_ListenerTCP, &m_MainSetTCP);
    FD_SET(m_ListenerTCP, &m_MainSetGlobal);

    // keep track of the biggest file descriptor
    m_FdMaxTCP = m_ListenerTCP; // so far, it's this one

    //Console->Print("Server running on port %d", port);

    return true;
}

void ServerSocket::update()
{
  struct timeval tmp_TimeOut;
  int fdMax;
  
    // copy fd_sets from main-set to temp. read-set
    m_ReadSetTCP = m_MainSetGlobal;

    fdMax = std::max(m_FdMaxTCP, m_FdMaxUDP);
    
    //FD_ZERO(&m_ReadSetTCP);
	//FD_SET (m_ListenerTCP, &m_ReadSetTCP);

    // select incoming data for tcp & udp
    tmp_TimeOut = m_TimeOut; //save m_TimeOut... will be modified by select
    if (select(fdMax+1, &m_ReadSetTCP, NULL, NULL, &tmp_TimeOut) == -1)
    {
        perror("select");
    }
    
    //Select exit condition logging
    /*
    if ((tmp_TimeOut.tv_sec == 0) && (tmp_TimeOut.tv_usec == 0))
    {
      Console->LPrint(GREEN, BLACK, "[Idle]");
      Console->LPrint(" Exiting select on timeout (remains %d sec and %d usec)", tmp_TimeOut.tv_sec, tmp_TimeOut.tv_usec);
      Console->LClose();
    }
    else     
    {
      Console->LPrint(YELLOW, BLACK, "[Active]");
      Console->LPrint(" Exiting select with remaining time %d sec and %d usec", tmp_TimeOut.tv_sec, tmp_TimeOut.tv_usec);
      Console->LClose();
    }
	  */

    // check for new tcp connections
    if (FD_ISSET(m_ListenerTCP, &m_ReadSetTCP)) // we got one!!
    {
        m_bNewTCPConnection = true;
    }
    else
    {
        m_bNewTCPConnection = false;
    }

}

bool ServerSocket::newConnection()
{
    return (m_bNewTCPConnection);
}

ConnectionTCP* ServerSocket::getTCPConnection()
{
    if(m_bNewTCPConnection)
    {
        struct sockaddr_in  RemoteAddr; // new incoming client address
        socklen_t addrlen = sizeof(RemoteAddr);
        int NewSockfd; // newly accept()ed socket descriptor

        // handle new connections
        if ((NewSockfd = accept(m_ListenerTCP, (struct sockaddr *)&RemoteAddr, &addrlen)) == -1)
        {
            m_bNewTCPConnection = false; // set back to normal
            perror("tcp-accept");
            return 0;
        }
        else
        {
            FD_SET(NewSockfd, &m_MainSetTCP); // add to master TCP set
            FD_SET(NewSockfd, &m_MainSetGlobal); // add to master global set
            if (NewSockfd > m_FdMaxTCP) // keep track of the maximum
            {
                m_FdMaxTCP = NewSockfd;
            }

            ConnectionTCP* tcpConn = new ConnectionTCP(NewSockfd, RemoteAddr, this);

            m_bNewTCPConnection = false; // set back to normal

            return tcpConn;
        }
    }

    return 0;
}

ConnectionUDP* ServerSocket::getUDPConnection(uint32_t remoteadress, int32_t remoteport)
{
    int udpSockfd;

    if ((udpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("udp-socket");
        return NULL;
    }

    struct sockaddr_in my_addr;

    uint16_t Port = Config->GetOptionInt("gameserver_udpport_min");
    uint16_t maxPort = Config->GetOptionInt("gameserver_udpport_max");

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(Port);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    //Search a free udp port to use (could be optimised for faster port allocation)
    while (bind(udpSockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
      if (errno == EADDRINUSE)
      {       
        my_addr.sin_port = htons(++Port);
        if (Port > maxPort)
        {
          Console->Print(RED, BLACK,"No more free UDP port in configured range");
          return NULL;
        }
      }
      else
      {  
        perror("udp-bind");
        return NULL;
      }
    }

    FD_SET(udpSockfd, &m_MainSetUDP); // add to master UDP set
    FD_SET(udpSockfd, &m_MainSetGlobal); // add to master global set
    if (udpSockfd > m_FdMaxUDP) // keep track of the maximum
    {
        m_FdMaxUDP = udpSockfd;
    }
            
    ConnectionUDP* udpConn = new ConnectionUDP(udpSockfd, Port, remoteadress, remoteport, this);

	  //m_LastUDPPort++;

    return udpConn;
}

bool ServerSocket::isDataAvailable(int sockfd)
{
    if(FD_ISSET(sockfd, &m_ReadSetTCP))
    {
        return true;
    }

    return false;
}

void ServerSocket::delSocketFromSet(int sockfd)
{
    FD_CLR(sockfd, &m_MainSetTCP);
    FD_CLR(sockfd, &m_MainSetUDP);
    FD_CLR(sockfd, &m_MainSetGlobal);
}

void ServerSocket::closeServer()
{
    close(m_ListenerTCP);
}

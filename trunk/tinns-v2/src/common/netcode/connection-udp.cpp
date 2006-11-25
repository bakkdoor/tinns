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
	connection-udp.cpp - a connection class for udp

	Authors:
	- bakkdoor

	MODIFIED: 09 Feb 2006 bakkdoor
	REASON: - introduced
	
	MODIFIED: 27 Jun 2006 hammag
	REASON: - put UDP socket in non-blocking mode in ConnectionUDP to avoid blocking
	          in update() as it can be called without data available.
	        - modified update() accordingly.
	        - modified flushSendBuffer() accordingly; >> !!! leads to data loss if packet could not be sent
	            and connection closed just after that. Is it a problem before UDP connection closure (ie logout) ???
	        - corrected recv *buf and len parameters in update() to avoid 
	          unread in-buffer data smashing.
	        - added initialisation of addrlen in update().

	MODIFIED: 01 Jul 2006 hammag
	REASON: - ConnectionUDP(): modified to set m_ServerSocket
	        - update(): modified to check m_ServerSocket->isDataAvailable()
	            before trying to read from socket
	        - ~ConnectionUDP(): added socket removal from fd set

  MODIFIED: 24 Jul 2006 hammag
	REASON: - changed member data prefix from "m_" to "m" in for homogeneity with the reste of TinNS code
	        - added private members data mQueueIn and mQueueOut
	        - added public members methods SendMessage(), GetMessage(), DeleteOutgoingMessages() and modified code accordingly
	        - changed old read/write methods implementation to message queues usage
	
	MODIFIED: 26 Jul 2006 hammag
	REASON: - enabled multiple network receive & send in on single update() call

	MODIFIED: 29 Jul 2006 hammag
	REASON: - changed order of network read/write in update(): now first write, then read

	MODIFIED: 05 Aug 2006 hammag
	REASON: - a pseudo-connection is now established on the udp socket, so that we don't need to care
	            for client IP/port in later processing
	REASON: - renamed "getLocalAddress()" to "getRemoteAddress()" as it is ... what it does !
	            
	
	TODO:   - split update so the main loop is : read - process - write - wait for something to read
	        - add try vector read & write with recvmsg/sendmsg
	        - add real priority management when needed. atm all outgoing messages are put in the same queue
	        - add pending outgoing message management in case of EAGAIN (request temp add to writeset & mngt by  the "scheduler")
          - best sizing of receive message could be done with ioctl:
             "int value;
              error = ioctl(tcp_socket, ioctl_type, &value);
              SIOCINQ
              Gets a pointer to an integer as argument. Returns the  size  of
              the next pending datagram in the integer in bytes, or 0 when no
              datagram is pending."
          - check incoming packets Source IP for matching with registered client IP, to avoid DOS & other things
          - maybe dont allow source port change (eg. because of NAT) more than once (at the begining only) for same reasons
          - check incoming messages UDP_ID to reject old messages and -if possible- request OOO granted messages (to be done at higher message decoding level thought)
          - manage an ageing queue of sent granted messages for resent if OOO notified by client
 	        - don't access Config object from netcode's objects. Use parameter members
              set by object owner.
*/

#include "main.h"

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
}

bool ConnectionUDP::update()
{
  PMessage* tmpMsg;
  int numBytes;

  // send data from outgoing queue
  flushSendBuffer(); // manage old write compatibility

  if (! mQueueOut.empty())
  {
//Console->Print(GREEN, BLACK, "ConnectionUDP::update() - Sending messages -----------------");
//  Console->Print("ConnectionUDP::update() - %d messages waiting in Output Queue", mQueueOut.size());
  	while(! mQueueOut.empty())
  	{
//Console->Print("ConnectionUDP::update() - OUT Data avail");
  	  tmpMsg = mQueueOut.front();
  	  //int numBytes = send(mSockfd, tmpMsg->GetMessageData(), tmpMsg->GetSize(), 0);
  		int numBytes = sendto(mSockfd, tmpMsg->GetMessageData(), tmpMsg->GetSize(), 0, (struct sockaddr *)&mRemoteAddr, sizeof(struct sockaddr));
      if(numBytes > 0)
  		{
//Console->Print(GREEN, BLACK, "ConnectionUDP::update() - Data sent");
  		    mLastActive = std::time(NULL);
          mQueueOut.pop(); // message written, we can remove it from queue
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
  
  //check if data is available for this socket and if yes, read into a new PMessage and put it on incoming queue
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
      numBytes = recvfrom(mSockfd, tmpMsg->GetMessageDataPointer(RECVBUFFERSIZE), RECVBUFFERSIZE, 0, (struct sockaddr *)&mRemoteAddr, &addrlen);  
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

/**************** Old I/F compatibility stuff ******************/

int ConnectionUDP::getRecvBufferSize()
{
  PMessage* tmpMsg;
  
  if (mQueueIn.empty())
    return 0;

  tmpMsg = mQueueIn.front();
	u16 _size = tmpMsg->GetSize()-tmpMsg->GetNextByteOffset();
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

const u8* ConnectionUDP::read(int* size)
{
  PMessage* tmpMsg;
//Console->Print("ConnectionUDP::read() - trying to read up to %d bytes", *size);  
  if (mQueueIn.empty() || !size)
  {
//Console->Print("ConnectionUDP::read() - no more packet");
    return NULL;
  }

  tmpMsg = mQueueIn.front();
	u16 _size = tmpMsg->GetSize()-tmpMsg->GetNextByteOffset();
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
		*size=min(*size, (s32)_size);
	}

	u8* ptr = (u8*) (tmpMsg->GetMessageData() + tmpMsg->GetNextByteOffset());
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
  
	mSendBufferMsg->Write(data, (u16)size);
//Console->Print(GREEN, BLACK, "ConnectionUDP::write() %d bytes written to mSendBufferMsg (total size %d)", size, mSendBufferMsg->GetSize());
	return size;
}

int ConnectionUDP::write(const char *String)
{
	if(!String)
		return 0;

	return write(String, strlen(String));
}

int ConnectionUDP::write(u8 Data)
{
	return write(&Data, sizeof(u8));
}

int ConnectionUDP::write(u16 Data)
{
	return write(&Data, sizeof(u16));
}

int ConnectionUDP::write(u32 Data)
{
	return write(&Data, sizeof(u32));
}

int ConnectionUDP::write(float Data)
{
	return write(&Data, sizeof(float));
}

int ConnectionUDP::write(double Data)
{
	return write(&Data, sizeof(double));
}

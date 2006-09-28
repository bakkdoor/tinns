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
	connection-tcp.cpp - a connection class for tcp

	Authors:
	- bakkdoor

	MODIFIED: 09 Feb 2006 bakkdoor
	REASON: - introduced
	
	MODIFIED: 27 Jun 2006 hammag
	REASON: - added m_ServerSocket check in ~ConnectionTCP()
	        - added m_ServerSocket check in update() in disconnection case
	        - corrected recv *buf and len parameters in update()
	          to avoid unread in-buffer data smashing
	          
	MODIFIED: 01 Jul 2006 hammag
	REASON: - update() : modified to deal correctly with would-block socket writes.
          - ConnectionTCP() : moved-in non-blocking setting from
	            ServerSocket::getTCPConnection() for better class coherency

  MODIFIED: 24 Jul 2006 hammag
	REASON: - ConnectionTCP() : added m_SendSize, m_RecvRewind and m_RecvSize missing initialisation
	          which caused nice segfaults on unlucky days :p
	
	MODIFIED: 24 Jul 2006 hammag
	REASON: - changed member data prefix from "m_" to "m" in for homogeneity with the reste of TinNS code
	        - added private members data mQueueIn and mQueueOut
	        - added public members methods SendMessage(), GetMessage(), DeleteOutgoingMessages() and modified code accordingly
	        - changed old read/write methods implementation to message queues usage
	NOTA:   - as previously, ConnectionTCP class takes care of separating messages in the TCP stream according
	            to NC TCP packet lentgh field. Additionnaly, it does look for the 0xFE signature byte in front of the packet lentgh field.
	
	MODIFIED: 26 Jul 2006 hammag
	REASON: - enabled multiple network send in on single update() call

	MODIFIED: 05 Aug 2006 hammag
	REASON: - renamed "getLocalAddress()" to "getRemoteAddress()" as it is ... what it does !
	
	TODO:   - add real priority management when needed. atm all outgoing messages are put in the same queue
	        - add pending outgoing message management in case of EAGAIN (request temp add to writeset & mngt by  the "scheduler")
	        - set TCP socket option SO_KEEPALIVE to be firewall-users friendly (else connexion may be lost when no zoning occurs for a long time, like in real NC ;-) )
	        - don't access Config object from netcode's objects. Use parameter members
              set by object owner.

*/

#include "main.h"

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

    mTimeOutValue = DEFAULT_TIMEOUT;
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
  u8* DataStart;
  u16 DataSize;
  u8* MsgStart;
  u16 MsgOffset;
  u16 MsgLen;
  
  //check if data is available for this socket and if yes, read into a new PMessage and put it on incoming queue
  if(mServerSocket->isDataAvailable(mSockfd))
  {
//Console->Print("ConnectionTCP::update() - IN Data avail");
    if (mReceiveBufferMsg == NULL)
    {
      mReceiveBufferMsg = new PMessage(RECVBUFFERSIZE);
    }
    
    DataSize = mReceiveBufferMsg->GetSize();
    numBytes = recv(mSockfd, mReceiveBufferMsg->GetMessageDataPointer(RECVBUFFERSIZE - DataSize) + DataSize, RECVBUFFERSIZE - DataSize, 0); // get the data

    if(numBytes > 0)
    {
//Console->Print(GREEN, BLACK, "ConnectionTCP::update() - Data received");
      mbConnected = true;
      mReceiveBufferMsg->ForceSize(DataSize + numBytes);
      mLastActive = std::time(NULL);
      
      while(mReceiveBufferMsg && mReceiveBufferMsg->GetSize())
      {
        DataStart = (u8*)mReceiveBufferMsg->GetMessageData();
        DataSize = mReceiveBufferMsg->GetSize();
        MsgStart = (u8*)memchr(DataStart, 0xfe, DataSize);
        
        if (MsgStart)
        {
          MsgOffset = MsgStart - DataStart;
          if (MsgOffset)
          {
            Console->Print(YELLOW, BLACK, "ConnectionTCP::update() Message head found 0x%04hx bytes after packet start. Inbetween data will be discarded.", MsgOffset);
          }
          if (MsgOffset + 3 <= DataSize)
          {
            MsgLen = *(u16*)(DataStart + MsgOffset + 1);
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
		int numBytes = send(mSockfd, tmpMsg->GetMessageData(), tmpMsg->GetSize(), 0);
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

const u8* ConnectionTCP::read(int* size)
{
  PMessage* tmpMsg;
//Console->Print("ConnectionTCP::read() - trying to read up to %d bytes", *size);  
  if (mQueueIn.empty() || !size)
  {
//Console->Print("ConnectionTCP::read() - no more packet");
    return NULL;
  }

  tmpMsg = mQueueIn.front();
	u16 _size = tmpMsg->GetSize()-tmpMsg->GetNextByteOffset();
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
		*size=min(*size, (s32)_size);
	}

	u8* ptr = (u8*) (tmpMsg->GetMessageData() + tmpMsg->GetNextByteOffset());
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
  
	mSendBufferMsg->Write(data, (u16)size);
//Console->Print(GREEN, BLACK, "ConnectionUDP::write() %d bytes written to mSendBufferMsg (total size %d)", size, mSendBufferMsg->GetSize());
	return size;
}

int ConnectionTCP::write(const char *String)
{
	if(!String)
		return 0;

	return write(String, strlen(String));
}

int ConnectionTCP::write(u8 Data)
{
	return write(&Data, sizeof(u8));
}

int ConnectionTCP::write(u16 Data)
{
	return write(&Data, sizeof(u16));
}

int ConnectionTCP::write(u32 Data)
{
	return write(&Data, sizeof(u32));
}

int ConnectionTCP::write(float Data)
{
	return write(&Data, sizeof(float));
}

int ConnectionTCP::write(double Data)
{
	return write(&Data, sizeof(double));
}

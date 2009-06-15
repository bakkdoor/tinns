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

	msgdecoder.h - top class for NC messages decoding

	CREATION: 23 Aug 2006 Hammag

	MODIFIED:
	REASON: -

*/

#ifndef MSGDECODER_H
#define MSGDECODER_H

struct PGameState;
class PClient;
class PMessage;
class PUdpMsgAnalyser;

#define DECODE_UNDEF 0
#define DECODE_ERROR 1 // one error occured on the current decoding step
#define DECODE_UNKNOWN 2 // the current decoded (sub)message is unkown
#define DECODE_MORE 4 // more decoding needed for the (sub)message
#define DECODE_FINISHED 8 // decoding if finished for the whole message
#define DECODE_ACTION_READY 16 // an action is ready to be done
#define DECODE_ACTION_DONE 32 // the action triggered hasn't to be triggered again
#define DECODE_ACTION_IGNORED 64 // the action wasn't performed for some (good) reason (to combine or not with DONE)
#define DECODE_ACTION_FAILED 128 // the action failed totally or partially (to combine or not with DONE)


struct PMsgDecodeData
{
	  PMessage* mMessage;
	  PClient* mClient;
	  u8 mState;
	  u8 mUnknownType;
	  bool mHandling0x13Sub;
	  u16 Sub0x13Start;
	  u16 Sub0x13StartNext;
	  std::stringstream mName;
	  std::string mErrorDetail;
	  bool mTraceKnownMsg;
	  bool mTraceUnknownMsg;
	  bool mTraceDump;
	  PGameState* mClientState; // Temporary until State is put back in Client object
};

// UDP Message decoder
// boolean methods return true if successful

class PUdpMsgDecoder
{
	private:
    PMsgDecodeData mDecodeData;
    PUdpMsgAnalyser* mCurrentAnalyser;
    std::string mPacketName;
    std::string mTmpName;

    void Init(PMessage* nMessage, PClient* nClient);

	public:
	  PUdpMsgDecoder();
    PUdpMsgDecoder(PMessage* nMessage, PClient* nClient);
    ~PUdpMsgDecoder();
    
    bool Analyse();
    bool Analyse(PMessage* nMessage, PClient* nClient); // Can be used on non initialized or already used object
    // Temporary form until State is put back in Client object
    inline void Init(PMessage* nMessage, PClient* nClient, PGameState *nClientState) { mDecodeData.mClientState = nClientState; Init(nMessage, nClient); }
    inline u8 GetState() { return mDecodeData.mState; }
    inline bool IsError() { return (mDecodeData.mState & DECODE_ERROR); }
    inline bool IsKnown() { return (!(mDecodeData.mState & DECODE_UNKNOWN)); }
    inline bool MoreSubMsg() { return mDecodeData.mHandling0x13Sub; }
    inline bool IsActionReady() { return (mDecodeData.mState & DECODE_ACTION_READY); }
    inline bool IsActionDone() { return (mDecodeData.mState & DECODE_ACTION_DONE); }
    inline bool IsTraceKnownMsg() { return mDecodeData.mTraceKnownMsg; }
	  inline bool IsTraceUnknownMsg() { return mDecodeData.mTraceUnknownMsg; }
	  inline bool IsTraceDump() { return mDecodeData.mTraceDump; }
	  inline void DumpMsg() { if (mDecodeData.mMessage) mDecodeData.mMessage->Dump(); }
    inline std::string const &GetName() { return (mTmpName = mDecodeData.mName.str()); }
    inline std::string const &GetError() { return mDecodeData.mErrorDetail; }
    bool DoAction();
    inline void Reset() { Init(NULL, NULL); }
};

#endif

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
 client.h

 Authors:
 - v00d00
 - Akiko
 - Namikon
 - bakkdoor

 MODIFIED: 30 Nov 2005 Akiko
 REASON: - added GPL
        - added stuff from Namikon
  MODIFIED: 13 Dec 2005 bakkdoor
  REASON: - added multiuser chat
  MODIFIED: 27 Jul 2006 Hammag
  REASON: - added mTransactionID member
          - re-added IncreaseUDP_ID() // but I don't understand well what SetUDP_ID() does ...
  MODIFIED: 05 Aug 2006 Hammag
  REASON: - changed TMP_UDP_PORT/SetTMPUDPPort()/GetTMPUDPPort() to mRemotePort/SetRemoteUDPPort()/GetRemoteUDPPort()
              which corresponds to the real purpose of these members
          - added GetID() as an alias os GetIndex() for better coherency with other classes
  MODIFIED: 09 Oct 2006 Hammag
  REASON: - added GetDebugMode() and SetDebugMode() methods


  TODO:   - check that SetUDP_ID, and the mSessionID(UDP_ID_HIGH) real use,
              and if UDP_ID and mSessionID must be synced (like in NeoX) or not

*/

#ifndef CLIENT_H
#define CLIENT_H

enum PClientConnection
{
  PCC_NONE = 0,
  PCC_GAME = 1
};

// AccountLevel handling is part of accounts.cpp
/*
enum PClientLevel
{
 PCL_BANNED = -1,
 PCL_UNREGPLAYER = 0,
 PCL_REGPLAYER = 1,
 PCL_VOLUNTEER = 30,
 PCL_GM = 50,
 PCL_ADMIN = 100
};
*/
#define DEBUG_MODES 3
enum PDebugMode
{
  DBG_LOCATION = 0,
  DBG_ITEMID = 1,
  DBG_SUBWAY = 2,
  DBG_ALL = DEBUG_MODES // must always be last, with DEBUG_MODES updated as needed
};


class PClient
{
  private :
    ConnectionTCP* m_TCPConnection;
    ConnectionUDP* m_UDPConnection;

    u32 mAccountID;
    int mAccountLevel;
    u32 mIndex;
    u32 mCharID;

//  u16 mUDP_ID;
//  u16 mSessionID;
//  u16 mTransactionID;

    // AccountLevel handling is part of accounts.cpp
    //PClientLevel mLevel;
    int mConnection;
    int mRemotePort;

    bool mDebugMode[DEBUG_MODES];
    // new multiuser-chat implementation //
    int m_ZoneID;
    //int[4] m_IP;

    //*******
    bool mActorRemoveMode;
    //*******
    bool mAwaitingWarpto;
    u16 mTargetX;
    u16 mTargetY;
    u16 mTargetZ;
    //*******
    bool mAcceptNPCUpdates;
    bool mZoning;
    bool mVhcZoning;
    

  protected :
  public :
    PClient( int Index );
    ~PClient();

    inline bool GetDebugMode( PDebugMode nDebugID ) { return mDebugMode[nDebugID]; }
    void SetDebugMode( PDebugMode nDebugID, bool nVal = true );

    inline bool IsAcceptingNPCUpdates() { return mAcceptNPCUpdates; }
    inline void SetAcceptNPCUpdates( bool nVal ) { mAcceptNPCUpdates = nVal; }
    inline bool IsZoning() { return mZoning; }
    inline void SetZoning( bool nVal = true ) { mZoning = nVal; if ( !nVal ) mVhcZoning = false; }
    inline bool IsVhcZoning() { return mVhcZoning; }
    inline void SetVhcZoning( bool nVal = true ) { mVhcZoning = nVal; }

    inline u32 GetIndex() const { return mIndex; } // better use GetID()
    inline u32 GetID() const { return mIndex; } // for better coherency with other classes
    inline u32 GetLocalID() const { return mIndex + 1; }
    inline u32 GetCharID() const { return mCharID; }
    PChar* GetChar() const;
    bool ChangeCharLocation( u32 nLocation, bool DoForce = false );

    inline int GetRemoteUDPPort() const { return mRemotePort; } // Temp solution

    inline bool IsInRemoveActorMode() { return mActorRemoveMode; }
    inline void SetRemoveActorMode( bool nNewValue ) { mActorRemoveMode = nNewValue; }

    inline void SetRemoteUDPPort( int port ) { mRemotePort = port; } // Temp solution
    inline void SetCharID( int id ) { mCharID = id; }//NEW added

    /*
        inline u16 GetUDP_ID() const { return mUDP_ID; }
        inline u16 GetSessionID() const { return SESSION_UDP_OFFSET + mUDP_ID ; }
        inline u16 GetTransactionID() {return mTransactionID; }
        void SetUDP_ID(int id);
        inline void IncreaseUDP_ID() { SetUDP_ID(mUDP_ID + 1); }
        inline void ResetTransactionID() { mTransactionID = 10170; }

        inline void IncreaseTransactionID(u8 nInc = 1) { mTransactionID += nInc; }
    */

    // All outgoing ID's and stuff is now part of the ConnectionUDP class itself!
    //    (which is not so good.... comment from Hammag)
    // However, we still have full access to it through these functions
    u16 GetUDP_ID();
    void SetUDP_ID( int id );
    void IncreaseUDP_ID();

    u16 GetSessionID();

    u16 GetTransactionID();
    void ResetTransactionID();
    void IncreaseTransactionID( u8 nInc = 1 );

    void FillInUDP_ID( PMessage* nMessage );

// ************************************************************************ //
    // AccountLevel handling is part of accounts.cpp
    //inline PClientLevel GetLevel() const { return mLevel; }

    inline void setTCPConnection( ConnectionTCP* conn ) { m_TCPConnection = conn; m_UDPConnection = 0; mConnection = PCC_GAME;  }
    inline void setUDPConnection( ConnectionUDP* conn ) { m_UDPConnection = conn; }

    inline ConnectionTCP* getTCPConn() { return m_TCPConnection; }
    inline ConnectionUDP* getUDPConn() { return m_UDPConnection; }

    inline void SendTCPMessage( PMessage* nMessage ) { if ( m_TCPConnection ) { m_TCPConnection->SendMessage( nMessage ); } else { delete nMessage; } }
    void FragmentAndSendUDPMessage( PMessage* nMessage, u8 nType );
    inline void SendUDPMessage( PMessage* nMessage, bool nVIP = false ) { if ( m_UDPConnection ) { m_UDPConnection->SendMessage( nMessage, nVIP ); }  else { delete nMessage; } }

    inline int GetConnection() const { return mConnection; }
    inline const char *GetAddress() const { return m_TCPConnection->getRemoteAddress(); }
    inline u32 GetAccountID() const { return mAccountID; }
    inline int GetAccountLevel() const { return mAccountLevel; }

    void GameDisconnect();

    void RefreshAccountInfo( PAccount* Account );
    inline void LoggedIn( PAccount* Account ) { RefreshAccountInfo( Account ); }
    void Update();

    // new multiuser-chat implementation //
    inline int getZoneID() const { return m_ZoneID; } // example: canyon 650 (for local-channel...every client with same AreaID get the chatmsg)
    //inline int* getIP() const { return (int*) m_IP; }

    inline void SetAwaitingWarpto( bool yesno, u16 NewX, u16 NewY, u16 NewZ )
    {
      mAwaitingWarpto = yesno;
      mTargetX = NewX;
      mTargetY = NewY;
      mTargetZ = NewZ;
    }
    bool GetCharAwaitingWarpto( u16* PosX = NULL, u16* PosY = NULL, u16* PosZ = NULL );

    // Char broadcasted effects
    void InitWarpCircle();
    void InitCharVanish();

    // used for dynamic ingame testing
    u8 testval8;
    u16 testval16;
    u32 testval32;
};

#endif


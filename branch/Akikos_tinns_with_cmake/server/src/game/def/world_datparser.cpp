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
 world_datparser.h
 Class to parse .dat world files

 MODIFIED: 29 Sep 2006 Hammag
 REASON: - Creation

MODIFIED: 21 Jun 2009 Namikon
REASON: - Added NPC Template stuff
        - reformatted for better reading

*/

#include "main.h"

#include "world_datparser.h"
#include "world_datstruct.h"

#include "worlddatatemplate.h"
#include "furnituretemplate.h"
#include "doortemplate.h"
#include "npctemplate.h"

#include <math.h>

const u16 nonDiscardUseFlags = ufTouchable | ufUsable | ufChair | ufToolTarget ; // furniture always to keep even if function type = 0

PWorldDatParser::PWorldDatParser()
{
    f = NULL;
}

PWorldDatParser::~PWorldDatParser()
{

}

int PWorldDatParser::LoadDatFile( const std::string& nFilename, PWorldDataTemplate* nWorld, const bool nDiscardPassiveObjects, const bool nTestAccesOnly )
{
    PWorldFileHeader FileHeader;
    PSectionHeader SectionHeader;
    PSec2ElemHeader Sec2ElemHeader;

    u32 FileLen;
    u32 NextSectionOffset = 0;
    u32 NextElementOffset;
    bool ProcessOK;

    mWorld = nWorld;
    mDiscardPassiveObjects = nDiscardPassiveObjects;

    if ( mNCDataPath == "" )
    {
        mNCDataPath = Config->GetOption( "nc_data_path" );
    }

    if ( gDevDebug )
        Console->Print( "Reading file %s/%s", mNCDataPath.c_str(), nFilename.c_str() );
    f = Filesystem->Open( "", nFilename.c_str(), mNCDataPath );

    if ( nTestAccesOnly )
    {
        ProcessOK = ( bool )f;
        Filesystem->Close( f );
        return ( ProcessOK ? 0 : -1 );
    }

    if ( f )
    {
        FileLen = f->GetSize();

        // Section 1
        if (gDevDebug)
            Console->Print( "Reading file header (section 1) ... " );
        f->Read( &FileHeader, sizeof( PWorldFileHeader ) );
        if (( FileHeader.mHeaderSize != 0x00000008 )
                || ( FileHeader.mHeaderSig != 0x000fcfcf )
                || ( FileHeader.mSection != 0x00000001 ) )
        {
            if (gDevDebug)
                Console->Print( "Read header: %08x / %08x / %08x",  FileHeader.mHeaderSize, FileHeader.mHeaderSig, FileHeader.mSection);
            Filesystem->Close( f );
            return -2;
        }
        NextSectionOffset += FileHeader.mHeaderSize + 4;

        // Other Sections
        //  Header
        while ( ! f->Eof() )
        {
            f->Seek( NextSectionOffset ); // Make sure we are at the computed offset
            if (gDevDebug)
                Console->Print( "Reading next section header ... " );
            if (( u32 )( f->Read( &SectionHeader, sizeof( PSectionHeader ) ) ) < sizeof( PSectionHeader ) )
            {
                Filesystem->Close( f );
                return -3;
            }

            if (( SectionHeader.mHeaderSize != 0x0000000c ) || ( SectionHeader.mHeaderSig != 0x0000ffcf ) )
            {
                Filesystem->Close( f );
                return -2;
            }

            if ( SectionHeader.mSection == 0 )
            {
                if (gDevDebug)
                    Console->Print( "Ending section reached" );
                break;
            }

            NextElementOffset = NextSectionOffset + SectionHeader.mHeaderSize + 4;
            NextSectionOffset = NextElementOffset + SectionHeader.mDataSize;
            if (gDevDebug)
                Console->Print( "Processing section %d (size %d)", SectionHeader.mSection, SectionHeader.mDataSize );

            if ( SectionHeader.mSection == 2 )
            {
                //int cnt=0;
                if ( gDevDebug )
                    Console->Print( "Element Type 3 size: %d or %d", sizeof( PSec2ElemType3a ), sizeof( PSec2ElemType3a ) + sizeof( PSec2ElemType3b ) );
                while ( NextElementOffset < NextSectionOffset )
                {
                    f->Seek( NextElementOffset ); // Make sure we are at the computed offset

                    //if ( gDevDebug )
                    //    Console->Print( "Reading next element header ... " );
                    if (( u32 )( f->Read( &Sec2ElemHeader, sizeof( PSec2ElemHeader ) ) ) < sizeof( PSec2ElemHeader ) )
                    {
                        Filesystem->Close( f );
                        return -3;
                    }
                    if (( Sec2ElemHeader.mHeaderSize != 0x0000000c ) || ( Sec2ElemHeader.mHeaderSig != 0x0ffefef1 ) )
                    {
                        Filesystem->Close( f );
                        return -2;
                    }
                    NextElementOffset += ( Sec2ElemHeader.mHeaderSize + 4 + Sec2ElemHeader.mDataSize );
                    //if (gDevDebug) Console->Print("Found element %d of type %d, size %d", ++cnt, Sec2ElemHeader.mElementType, Sec2ElemHeader.mDataSize);
                    switch ( Sec2ElemHeader.mElementType )
                    {
                    case 1000003:
                    {
                        ProcessOK = ProcessSec2ElemType3( Sec2ElemHeader.mDataSize );
                        break;
                    }
                    case 1000005:
                    {
                        ProcessOK = ProcessSec2ElemType5( Sec2ElemHeader.mDataSize );
                        break;
                    }
                    case 1000006:
                    {
                        ProcessOK = ProcessSec2NPCEntry( Sec2ElemHeader.mDataSize );
                        break;
                    }
                    default:
                    {
                        if (gDevDebug) Console->Print( "Ignoring SectionID %d, not supportet yet", Sec2ElemHeader.mElementType );
                        ProcessOK = true;
                        break;
                    }
                    }

                    if ( !ProcessOK )
                        return -4;
                }
            }
            else
            {
                if ( gDevDebug )
                    Console->Print( "Section %d ignored", SectionHeader.mSection );
                continue;
            }

        }

        Filesystem->Close( f );
    }
    else
    {
        return -1;
    }

    return 0;
}

bool PWorldDatParser::ProcessSec2ElemType3( u32 nSize ) // furniture
{
    PSec2ElemType3a DataA;
    PSec2ElemType3b DataB;
    const PDefWorldModel* nWorldModel;
    std::string nName;
    const u32 sza = sizeof( PSec2ElemType3a );
    const u32 szb = sizeof( PSec2ElemType3a ) + sizeof( PSec2ElemType3b );

    if (( nSize != szb ) && ( nSize != sza ) )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for Sec2ElemType3 (%d read vs %d or %d needed", nSize, sza, szb );
        return false;
    }
    if (( u32 )( f->Read( &DataA, sza ) ) < sza )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType3a" );
        return false;
    }
    if ( nSize == szb )
    {
        if (( u32 )( f->Read( &DataB, sizeof( PSec2ElemType3b ) ) ) < sizeof( PSec2ElemType3b ) )
        {
            Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType3b" );
            return false;
        }
    }
    else
    {
        DataB.mBoxLowerY = DataB.mBoxLowerZ = DataB.mBoxLowerX = 0;
        DataB.mBoxUpperY = DataB.mBoxUpperZ = DataB.mBoxUpperX = 0;
    }

    if ( DataA.mWorldmodelID )
    {
        nWorldModel = GameDefs->WorldModels()->GetDef( DataA.mWorldmodelID );
        if ( nWorldModel )
            nName = nWorldModel->GetName();
        else
            nName = "UNKNOWN";
    }
    else
    {
        nName = "PASSIVE";
        nWorldModel = NULL;
    }

    /* if (gDevDebug) {
    Console->Print("-------------------------------------------------------");
    Console->Print("%s (%d) : ID %d", nName.c_str(), DataA.mWorldmodelID, DataA.mObjectID);
    if (!nWorldModel) Console->Print("y:%f z:%f x:%f model %d", DataA.mPosY , DataA.mPosZ, DataA.mPosX, DataA.mModelID);
    Console->Print("Scale:%f Uk2:0x%08x Uk3:0x%08x", DataA.mScale, DataA.mUnknown2, DataA.mUnknown3);
    Console->Print("Uk4:0x%08x Uk5:0x%04x", DataA.mUnknown4, DataA.mUnknown5);
    //Console->Print("Ly:%f Lz:%f Lx:%f", DataB.mBoxLowerY, DataB.mBoxLowerZ, DataB.mBoxLowerX);
    //Console->Print("Uy:%f Uz:%f Ux:%f", DataB.mBoxUpperY, DataB.mBoxUpperZ, DataB.mBoxUpperX);
    }*/

    if (( !nWorldModel || ( !nWorldModel->GetFunctionType() && !( nWorldModel->GetUseFlags() & nonDiscardUseFlags ) ) ) && mDiscardPassiveObjects )
    {
        //if ( gDevDebug )
        //    Console->Print( "Discarded" );
        return true;
    }

    PFurnitureItemTemplate* nItem = new PFurnitureItemTemplate;
    nItem->mObjectID = DataA.mObjectID;

    // The commented out values are not loaded from dat file atm because they are not used yet.
    nItem->mPosY = DataA.mPosY; // float pos values are kept 0-centered
    nItem->mPosZ = DataA.mPosZ;
    nItem->mPosX = DataA.mPosX;
//  nItem->mRotY = DataA.mRotY;
    nItem->mRotZ = DataA.mRotZ;
//  nItem->mRotX = DataA.mRotX;
//  nItem->mScale = DataA.mScale;
//  nItem->mUnknown2 = DataA.mUnknown2;
    nItem->mModelID = DataA.mModelID;
//  nItem->mUnknown3 = DataA.mUnknown3;
//  nItem->mUnknown4 = DataA.mUnknown4;
    nItem->mWorldmodelID = DataA.mWorldmodelID;
//  nItem->mUnknown5 = DataA.mUnknown5;

//  nItem->mBoxLowerY = DataB.mBoxLowerY;
//  nItem->mBoxLowerZ = DataB.mBoxLowerZ;
//  nItem->mBoxLowerX = DataB.mBoxLowerX;
//  nItem->mBoxUpperY = DataB.mBoxUpperY;
//  nItem->mBoxUpperZ = DataB.mBoxUpperZ;
//  nItem->mBoxUpperX = DataB.mBoxUpperX;

    nItem->mDefWorldModel = nWorldModel;
    /*u16 func=nWorldModel->GetFunctionType();
    if((func==18) || (func==20) || (func==29))
    Console->Print("gate model: %d", DataA.mWorldmodelID);*/

    float Angle = ( 180 + DataA.mRotZ ) * 3.14159 / 180;
    float Radius = abs(( int )(( DataB.mBoxUpperX - DataB.mBoxLowerX ) / 2 ) );
    if ( Radius == 0 )
    {
        Radius = 10;
    }
    Radius *= DataA.mScale;
    Radius += 5;

    // int pos values are change to match char pos scale (32000 centered)
    nItem->mFrontPosY = ( u16 )( 32000 + DataA.mPosY + Radius * sinf( Angle ) );
    nItem->mFrontPosZ = ( u16 )( 32000 + DataA.mPosZ );
    nItem->mFrontPosX = ( u16 )( 32000 + DataA.mPosX + Radius * cosf( Angle ) );
    nItem->mFrontLR = ( u8 )( 0.5 * ( DataA.mRotZ + ( DataA.mRotZ < 0 ? 360 : 0 ) ) );

    mWorld->AddFurnitureItem( nItem );

    return true;
}

bool PWorldDatParser::ProcessSec2ElemType5( u32 nSize ) // doors
{
    PSec2ElemType5Start Data;
    char StringData[64];

    const PDefWorldModel* nWorldModel;
    std::string nName;
    char* ActorString;
    char* ParamString;

    const u32 sza = sizeof( PSec2ElemType5Start );

    if (( nSize < sza ) )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for Sec2ElemType5 (%d read vs %d needed", nSize, sza );
        return false;
    }
    if (( u32 )( f->Read( &Data, sza ) ) < sza )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType5start" );
        return false;
    }
    u32 szb = Data.mActorStringSize  + Data.mParamStringSize;

    if ( nSize != ( sza + szb ) )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for Sec2ElemType5 (%d read vs %d needed", nSize, sza + szb );
        return false;
    }
    else
    {
        if ( szb > 64 )
        {
            Console->Print( RED, BLACK, "[Warning] String data too long in Sec2ElemType5 End String. End will be ignored" );
            szb = 64;
        }
        if (( u32 )( f->Read( StringData, szb ) ) < szb )
        {
            Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in Sec2ElemType5 End Strings" );
            return false;
        }
    }

    if ( Data.mWorldmodelID )
    {
        nWorldModel = GameDefs->WorldModels()->GetDef( Data.mWorldmodelID );
        if ( nWorldModel )
            nName = nWorldModel->GetName();
        else
            nName = "UNKNOWN";
    }
    else
    {
        nName = "PASSIVE";
        nWorldModel = NULL;
    }

    StringData[Data.mActorStringSize - 1] = 0;
    ActorString = StringData;
    StringData[szb - 1] = 0;
    ParamString = StringData + Data.mActorStringSize;
    /*
        if ( gDevDebug )
        {
            Console->Print( "-------------------------------------------------------" );
            Console->Print( "Door %s (%d) : ID %d", nName.c_str(), Data.mWorldmodelID, Data.mDoorID );
            Console->Print( "y:%f z:%f x:%f", Data.mPosY , Data.mPosZ, Data.mPosX );
            Console->Print( "Uk1:0x%04x Uk1bis:0x%04x Uk5:0x%04x", Data.mUnknown1, Data.mUnknown1bis, Data.mUnknown5 );
            Console->Print( "Type=%s Param=%s", ActorString, ParamString );
        }
    */
// Let's keep knowledge of doors even without models !
    /*  if ((!nWorldModel || (!nWorldModel->GetFunctionType() && !(nWorldModel->GetUseFlags() & nonDiscardUseFlags))) && mDiscardPassiveObjects)
      {
    if (gDevDebug) Console->Print("Discarded");
    if (gDevDebug)
    {
    Console->Print("Door %s (%d) : ID %d", nName.c_str(), Data.mWorldmodelID, Data.mDoorID);
    Console->Print("Type=%s Param=%s", ActorString, ParamString);
    }
        return true;
      }*/

    PDoorTemplate* nDoor = new PDoorTemplate;
    nDoor->mDoorID = Data.mDoorID;

    //nDoor->mUnknown1 = Data.mUnknown1; //18 00
    //nDoor->mUnknown1bis = Data.mUnknown1bis; //00 00 ? varies
    nDoor->mPosY = Data.mPosY;
    nDoor->mPosZ = Data.mPosZ;
    nDoor->mPosX = Data.mPosX;
    //nDoor->mUnknown5 = Data.mUnknown5; //00 00 ? second byte varies
    nDoor->mWorldmodelID = Data.mWorldmodelID; //door type from worldmodel.def
    nDoor->mDefWorldModel = nWorldModel;

    nDoor->SetDoorTypeName( ActorString );
    nDoor->SetDoorParameters( ParamString );

    mWorld->AddDoor( nDoor );

    return true;
}

bool PWorldDatParser::ProcessSec2NPCEntry( u32 nSize )
{
    PSec2NPC_EntryPart1         tNPCPartA;
    PSec2NPC_EntryPart2         tNPCPartB;
    string                      tActorName;
    string                      tAngle;
    char                        tStrBuffer[64];

    u32 tSizeOfA = sizeof(tNPCPartA);
    u32 tSizeOfB = sizeof(tNPCPartB);

    // Are we able to read enough bytes from the file? means: CAN we safely read our entire struct from the file?
    if ( nSize < tSizeOfA )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for PSec2NPC_EntryPart1 (%d read vs %d needed", nSize, tSizeOfA );
        return false;
    }
    // yes we can! So read it now. If we reach EOF, break
    if (( u32 )( f->Read( &tNPCPartA, tSizeOfA ) ) < tSizeOfA )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry" );
        return false;
    }
    // Now we have the header. lets check if we have some waypoints for this NPC
    // Size of entire NPC entry in file
    u32 tCompleteNPCSize = tSizeOfA + tNPCPartA.mActorStringSize + tNPCPartA.mAngleStringSize;
    if ( tNPCPartA.mHasAdditionalCoords > 0 )
    {
        // It has additional coords, add 'em
        tCompleteNPCSize += tSizeOfB*tNPCPartA.mHasAdditionalCoords;
    }

    // Do a last check if we're on the correct size
    if ( nSize != tCompleteNPCSize )
    {
        Console->Print( RED, BLACK, "[ERROR] Wrong size for PSec2NPC_Entry (%d available vs %d expected", nSize, tCompleteNPCSize );
        Console->Print( RED, BLACK, "NPC ID was: %d", tNPCPartA.mNpcID);
        return false;
    }
    // We are. Continue reading!
    // Assign the 2 strings and watch out for EOF!
    memset(tStrBuffer, 0, 64);
    if (( u32 )( f->Read( tStrBuffer, tNPCPartA.mActorStringSize ) ) < tNPCPartA.mActorStringSize )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry mActorStringSize" );
        return false;
    }
    tActorName = tStrBuffer;

    memset(tStrBuffer, 0, 64);
    if (( u32 )( f->Read( tStrBuffer, tNPCPartA.mAngleStringSize ) ) < tNPCPartA.mAngleStringSize )
    {
        Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry mAngleStringSize" );
        return false;
    }
    tAngle = tStrBuffer;

    // We're done. Now create new NPC entry
    PNPCTemplate* tNPC = new PNPCTemplate;

    // and assing all that stuff
    tNPC->SetUnknown1(tNPCPartA.mUnknown1);
    tNPC->SetPosX(tNPCPartA.mPosX);
    tNPC->SetPosY(tNPCPartA.mPosY);
    tNPC->SetPosZ(tNPCPartA.mPosZ);
    tNPC->SetNPCTypeID(tNPCPartA.mNPCTypeID);
    tNPC->SetActorStrSize(tNPCPartA.mActorStringSize);
    tNPC->SetAngleStrSize(tNPCPartA.mAngleStringSize);
    tNPC->SetNpcID(tNPCPartA.mNpcID);
    tNPC->SetUnknown2a(tNPCPartA.mUnknown2a);
    tNPC->SetUnknown2b(tNPCPartA.mUnknown2b);
    tNPC->SetUnknown2c(tNPCPartA.mUnknown2c);
    tNPC->SetTradeID/*SetUnknown3*/(tNPCPartA.mTradeID/*mUnknown3*/);
    tNPC->SetUnknown4(tNPCPartA.mUnknown4);

    tNPC->SetActorName(tActorName);
    tNPC->SetAngle(tAngle);

    // Read additional Waypoints if available
    u8 tCurrWayP = 0;
    if ( tNPCPartA.mHasAdditionalCoords > 0 )
    {
        while ( tCurrWayP < tNPCPartA.mHasAdditionalCoords )
        {
            memset(&tNPCPartB, 0, tSizeOfB);
            if (( u32 )( f->Read( &tNPCPartB, tSizeOfB ) ) < tSizeOfB )
            {
                Console->Print( RED, BLACK, "[ERROR] Unexpected end of file in ProcessSec2NPCEntry while reading WayPoints" );
                return false;
            }
            tNPC->AddWayPoint(tNPCPartB.mPosX, tNPCPartB.mPosY, tNPCPartB.mPosZ, tCurrWayP);
            tCurrWayP++;
        }
    }

    if (gDevDebug) Console->Print("Added NPC ID %d", tNPCPartA.mNpcID);

    mWorld->AddNPC(tNPC);
    return true;
}

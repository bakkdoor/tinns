#include "main.h"
#include "include/npc.h"

// First try of an "AI" :P
void PNPCWorld::CheckForEnemies(PNPC* nNPC)
{
    //return;
    std::time_t tNow = time(NULL);
    // Is it time for next enemy check?

    // Temp: Skip that for IDs below 1000
    if(nNPC->GetRealWorldID() < 1000)
        return;
    // -------------

    if(tNow > nNPC->mNextEnemyCheck)
    {
        //Console->Print("[NPC AI] Checking enemy status for NPC %d", nNPC->GetRealWorldID());
        nNPC->mNextEnemyCheck = time(NULL) + NPC_ENEMYCHECK;
        // Loop all NPCs in my world

        // tNearestEnemy[0] = WorldID | tNearestEnemy[1] = Distance to us
        u32 tNearestEnemy[2] = {0,0};

        for ( PNPCMap::iterator it = mNPCs.begin(); it != mNPCs.end(); it++ )
        {
            if(it->second)
            {
                PNPC* tNPC = it->second;
                // Is target = me?
                if(tNPC->GetRealWorldID() == nNPC->GetRealWorldID())
                    continue;

                u16 tDistance = DistanceApprox(nNPC->mPosX, nNPC->mPosY, nNPC->mPosZ, tNPC->mPosX, tNPC->mPosY, tNPC->mPosZ);

                // Is NPC infight?
                if(nNPC->GetActionStatus() == NPC_ACTIONSTATE_ATTACK)
                {
                    // If not infight with current target, skip here
                    if(nNPC->mTarget != tNPC->GetRealWorldID())
                        continue;

                    // He IS with target infight. Check range
                    if(tDistance > 1500)
                        nNPC->StopAttack(); // Enemy ran away :( cancel fight
                    else
                    {
                        //nNPC->Attack(tNPC->GetRealWorldID());
                        continue;           // Enemy is still in range, good! keep attacking him
                    }

                }
                // Ok, NPC is not infight, so lets check if current iterator target is in range
                if(tDistance > 800)
                    continue;   // No, he's not

                // He is! Look closer at him. Get Faction standing
                int tFactionMe = nNPC->mFaction;
                int tFactionHim = tNPC->mFaction;
                //Console->Print("[NPC AI] Checking InRange NPC %d; MyFac: %d HisFac: %d", tNPC->GetRealWorldID(), tFactionMe, tFactionHim);

                // Hey, we're friends :)
                if(tFactionMe == tFactionHim)
                    continue;

                const PDefFaction* tFaction = NULL;
                int tStanding = 0;
                // Always check higher faction against lower faction
                if(tFactionMe > tFactionHim)
                {
                    tFaction = GameDefs->Factions()->GetDef(tFactionMe);
                    if(!tFaction)
                    {
                        Console->Print("Unknown faction: %d", tFactionMe);
                        continue;
                    }
                    tStanding = tFaction->GetRelation(tFactionHim);
                }
                else
                {
                    tFaction = GameDefs->Factions()->GetDef(tFactionHim);
                    if(!tFaction)
                    {
                        Console->Print("Unknown faction: %d", tFactionHim);
                        continue;
                    }
                    tStanding = tFaction->GetRelation(tFactionMe);
                }
                // Enemies? omg! Check distance!
                if(tStanding < 0)
                {
                    if(tNearestEnemy[1] > tDistance || tNearestEnemy[0] == 0)
                    {
                        // This enemy is even closer than the stored one, or we dont have any enemy yet
                        tNearestEnemy[0] = tNPC->GetRealWorldID();
                        tNearestEnemy[1] = tDistance;
                        Console->Print("NPC ID %d (Distance %d) is an Faction enemy, Standing: %d. Remembering him!", tNearestEnemy[0], tNearestEnemy[1], tStanding);
                    }
                }
            }
        }
        if(tNearestEnemy[0] > 0)
        {
            Console->Print("NPC ID %d now attacking NPC ID %d", nNPC->GetRealWorldID(), tNearestEnemy[0]);
            nNPC->Attack(tNearestEnemy[0]);
        }
    }
}

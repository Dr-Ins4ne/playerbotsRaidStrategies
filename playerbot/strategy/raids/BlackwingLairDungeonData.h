#pragma once

#include "DungeonTargetHelper.h"

#include <cfloat>
#include <cmath>

namespace ai
{
    namespace BlackwingLair
    {
        static const uint32 MAP_ID = 469;

        static const uint32 NPC_RAZORGORE = 12435;
        static const uint32 NPC_GRETHOK_CONTROLLER = 12557;
        static const uint32 NPC_VAELASTRASZ = 13020;

        static const float RAZORGORE_EGG_PHASE_NEAR_DISTANCE = 18.0f;
        static const float RAZORGORE_EGG_PHASE_RING_MIN = 9.0f;
        static const float RAZORGORE_EGG_PHASE_RING_MAX = 14.0f;

        static const float VAELASTRASZ_PULL_X = -7485.0f;
        static const float VAELASTRASZ_PULL_Y = -1023.0f;
        static const float VAELASTRASZ_PULL_Z = 408.6f;
        static const float VAELASTRASZ_RANGED_X = -7513.0f;
        static const float VAELASTRASZ_RANGED_Y = -1004.0f;
        static const float VAELASTRASZ_RANGED_Z = 408.5f;
        static const float VAELASTRASZ_POSITION_REACHED_DISTANCE = 4.0f;
        static const float VAELASTRASZ_RANGED_PULL_POSITION_DISTANCE = 12.0f;

        static const uint32 NPC_BROODLORD_LASHLAYER = 12017;

        static const float BROODLORD_STACK_X = -7555.6f;
        static const float BROODLORD_STACK_Y = -1067.2f;
        static const float BROODLORD_STACK_Z = 449.3f;
        static const float BROODLORD_STACK_DISTANCE = 3.0f;

        static Unit* FindBroodlord(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, NPC_BROODLORD_LASHLAYER);
        }

        static bool IsBotNearBroodlordStackPosition(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::IsBotNearPosition(
                ai,
                BROODLORD_STACK_X,
                BROODLORD_STACK_Y,
                BROODLORD_STACK_Z,
                BROODLORD_STACK_DISTANCE);
        }

        static Unit* FindRazorgore(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, NPC_RAZORGORE);
        }

        static Unit* FindGrethokController(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, NPC_GRETHOK_CONTROLLER);
        }

        static Unit* FindVaelastrasz(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, NPC_VAELASTRASZ);
        }

        static bool IsVaelastraszTargetingBot(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::IsCreatureTargetingBot(ai, NPC_VAELASTRASZ);
        }

        static float DistanceToVaelastraszPullPosition(Player* bot)
        {
            return DungeonTargetHelper::DistanceToPosition(bot, VAELASTRASZ_PULL_X, VAELASTRASZ_PULL_Y, VAELASTRASZ_PULL_Z);
        }

        static float DistanceToVaelastraszRangedPosition(Player* bot)
        {
            return DungeonTargetHelper::DistanceToPosition(bot, VAELASTRASZ_RANGED_X, VAELASTRASZ_RANGED_Y, VAELASTRASZ_RANGED_Z);
        }

        static bool IsRazorgoreFightVisible(PlayerbotAI* ai)
        {
            return FindRazorgore(ai) || FindGrethokController(ai);
        }

        // Phase 1 after Grethok dies: Razorgore is alive, but the raid should
        // not hard-switch onto Razorgore yet. Once the real kill phase starts
        // and Razorgore is the current target, this returns false.
        static bool IsRazorgoreEggPhase(PlayerbotAI* ai)
        {
            if (!FindRazorgore(ai))
                return false;

            if (FindGrethokController(ai))
                return false;

            if (DungeonTargetHelper::IsCurrentTargetEntry(ai, NPC_RAZORGORE))
                return false;

            return true;
        }
    }
}

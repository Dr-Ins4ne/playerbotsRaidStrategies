#pragma once

#include "DungeonTargetHelper.h"

namespace ai
{
    namespace BlackwingLair
    {
        static const uint32 MAP_ID = 469;

        static const uint32 NPC_RAZORGORE = 12435;
        static const uint32 NPC_GRETHOK_CONTROLLER = 12557;

        static const float RAZORGORE_EGG_PHASE_NEAR_DISTANCE = 18.0f;
        static const float RAZORGORE_EGG_PHASE_RING_MIN = 9.0f;
        static const float RAZORGORE_EGG_PHASE_RING_MAX = 14.0f;

        static Unit* FindRazorgore(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, NPC_RAZORGORE);
        }

        static Unit* FindGrethokController(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, NPC_GRETHOK_CONTROLLER);
        }

        static bool IsRazorgoreFightVisible(PlayerbotAI* ai)
        {
            return FindRazorgore(ai) || FindGrethokController(ai);
        }

        // Heuristic for phase 1 after Grethok dies: Razorgore is still alive, but
        // the raid should not be hard-switching onto Razorgore yet. Once the normal
        // kill phase starts and Razorgore becomes the current target, this returns false.
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

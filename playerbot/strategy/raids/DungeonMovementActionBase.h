#pragma once

#include "../actions/MovementActions.h"
#include "DungeonTargetHelper.h"

#include <cfloat>
#include <string>

namespace ai
{
    class DungeonMovementActionBase : public MovementAction
    {
    public:
        DungeonMovementActionBase(PlayerbotAI* ai, std::string name)
            : MovementAction(ai, name) {}

        bool isPossible() override
        {
            return MovementAction::isPossible() && ai->CanMove();
        }

    protected:
        bool IsBotReadyForDungeonMovement(uint32 mapId = 0) const
        {
            if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            if (!ai || !ai->CanMove())
                return false;

            if (mapId && bot->GetMapId() != mapId)
                return false;

            return true;
        }

        float DistanceToPosition(float x, float y, float z) const
        {
            return DungeonTargetHelper::DistanceToPosition(bot, x, y, z);
        }

        bool IsNearPosition(float x, float y, float z, float maxDistance) const
        {
            return DungeonTargetHelper::IsUnitNearPosition(bot, x, y, z, maxDistance);
        }

        bool MoveToDungeonPosition(uint32 mapId, float x, float y, float z, bool waitForReachOnReaction = true)
        {
            if (!IsBotReadyForDungeonMovement(mapId))
                return false;

            const float distance = DistanceToPosition(x, y, z);

            if (MoveTo(mapId, x, y, z, false, IsReaction(), false, true))
            {
                if (waitForReachOnReaction && IsReaction())
                    WaitForReach(distance);

                return true;
            }

            return false;
        }
    };
}

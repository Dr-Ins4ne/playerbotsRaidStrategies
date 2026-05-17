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

        bool MoveToDungeonPosition(
            uint32 mapId,
            float x,
            float y,
            float z,
            bool waitForReachOnReaction = true,
            bool normalizeZ = true,
            bool requirePath = true)
        {
            if (!IsBotReadyForDungeonMovement(mapId))
                return false;

            const uint32 targetMapId = mapId ? mapId : bot->GetMapId();

            WorldPosition botPos(bot);
            WorldPosition targetPos(targetMapId, x, y, z, 0.0f);

            if (normalizeZ)
                targetPos.setZ(targetPos.getHeight());

            const float distance = botPos.distance(targetPos);

            if (requirePath && !botPos.canPathTo(targetPos, bot))
            {
                if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
                    ai->TellPlayerNoFacing(GetMaster(), getName() + ": target position is not pathable");

                return false;
            }

            if (MoveTo(
                    targetMapId,
                    targetPos.getX(),
                    targetPos.getY(),
                    targetPos.getZ(),
                    false,
                    IsReaction(),
                    false,
                    true))
            {
                if (waitForReachOnReaction && IsReaction())
                    WaitForReach(distance);

                return true;
            }

            if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
                ai->TellPlayerNoFacing(GetMaster(), getName() + ": MoveToDungeonPosition failed");

            return false;
        }
    };
}
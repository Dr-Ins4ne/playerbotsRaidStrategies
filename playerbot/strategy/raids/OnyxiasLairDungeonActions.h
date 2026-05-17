#pragma once

#include "../actions/DungeonActions.h"
#include "../actions/ChangeStrategyAction.h"

#include "DungeonTargetHelper.h"

#include "playerbot/strategy/Action.h"
#include "playerbot/ServerFacade.h"

#include <algorithm>
#include <cmath>
#include <list>
#include <string>
#include <vector>

namespace ai
{
    class OnyxiaActionBase : public Action
    {
    public:
        OnyxiaActionBase(PlayerbotAI* ai, std::string name)
            : Action(ai, name) {}

    protected:
        static constexpr uint32 MAP_ONYXIA_LAIR = 249;

        static constexpr uint32 NPC_ONYXIA = 10184;
        static constexpr uint32 NPC_ONYXIAN_WHELP = 11262;

        static constexpr float ONYXIA_EGG_PIT_1_X = -27.0f;
        static constexpr float ONYXIA_EGG_PIT_1_Y = -186.6f;
        static constexpr float ONYXIA_EGG_PIT_1_Z = -89.0f;

        static constexpr float ONYXIA_EGG_PIT_2_X = -27.0f;
        static constexpr float ONYXIA_EGG_PIT_2_Y = -250.0f;
        static constexpr float ONYXIA_EGG_PIT_2_Z = -89.0f;

        static constexpr float ONYXIA_SAFE_CENTER_X = 13.0f;
        static constexpr float ONYXIA_SAFE_CENTER_Y = -205.0f;
        static constexpr float ONYXIA_SAFE_CENTER_Z = -85.77f;

        // Tune this if bots still touch eggs.
        static constexpr float ONYXIA_EGG_PIT_RADIUS = 20.0f;

        // One movement step toward the center.
        // Repeated trigger executions will continue walking them out.
        static constexpr float ONYXIA_EGG_PIT_MOVE_STEP = 10.0f;

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, entry);
        }

        Unit* FindNearestAliveCreature(uint32 entry)
        {
            return DungeonTargetHelper::FindNearestAliveCreature(ai, entry);
        }

        bool SetCurrentTarget(Unit* target)
        {
            return DungeonTargetHelper::SetCurrentTarget(ai, target);
        }

        static float Distance2d(float x1, float y1, float x2, float y2)
        {
            float dx = x1 - x2;
            float dy = y1 - y2;
            return std::sqrt(dx * dx + dy * dy);
        }

        bool IsNearOnyxiaEggPit(Player* bot) const
        {
            if (!bot)
                return false;

            if (bot->GetMapId() != MAP_ONYXIA_LAIR)
                return false;

            float x = bot->GetPositionX();
            float y = bot->GetPositionY();

            float d1 = Distance2d(x, y, ONYXIA_EGG_PIT_1_X, ONYXIA_EGG_PIT_1_Y);
            float d2 = Distance2d(x, y, ONYXIA_EGG_PIT_2_X, ONYXIA_EGG_PIT_2_Y);

            return d1 <= ONYXIA_EGG_PIT_RADIUS ||
                   d2 <= ONYXIA_EGG_PIT_RADIUS;
        }
    };


    class TargetOnyxiaWhelpAction : public OnyxiaActionBase
    {
    public:
        TargetOnyxiaWhelpAction(PlayerbotAI* ai)
            : OnyxiaActionBase(ai, "target onyxia whelp") {}

        bool Execute(Event& event) override
        {
            return SetCurrentTarget(FindNearestAliveCreature(NPC_ONYXIAN_WHELP));
        }
    };


    class TargetOnyxiaAction : public OnyxiaActionBase
    {
    public:
        TargetOnyxiaAction(PlayerbotAI* ai)
            : OnyxiaActionBase(ai, "target onyxia") {}

        bool Execute(Event& event) override
        {
            return SetCurrentTarget(FindAliveCreature(NPC_ONYXIA));
        }
    };


    class MoveOutOfOnyxiaEggPitAction : public MovementAction
    {
    public:
        MoveOutOfOnyxiaEggPitAction(PlayerbotAI* ai)
            : MovementAction(ai, "move out of onyxia egg pit") {}

        bool Execute(Event& event) override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (!bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            if (bot->GetMapId() != MAP_ONYXIA_LAIR)
                return false;

            float botX = bot->GetPositionX();
            float botY = bot->GetPositionY();

            float d1 = Distance2d(botX, botY, ONYXIA_EGG_PIT_1_X, ONYXIA_EGG_PIT_1_Y);
            float d2 = Distance2d(botX, botY, ONYXIA_EGG_PIT_2_X, ONYXIA_EGG_PIT_2_Y);

            if (d1 > ONYXIA_EGG_PIT_RADIUS && d2 > ONYXIA_EGG_PIT_RADIUS)
                return false;

            float toCenterX = ONYXIA_SAFE_CENTER_X - botX;
            float toCenterY = ONYXIA_SAFE_CENTER_Y - botY;
            float length = std::sqrt(toCenterX * toCenterX + toCenterY * toCenterY);

            if (length < 0.1f)
                return false;

            float step = std::min(ONYXIA_EGG_PIT_MOVE_STEP, length);

            float destX = botX + (toCenterX / length) * step;
            float destY = botY + (toCenterY / length) * step;
            float destZ = ONYXIA_SAFE_CENTER_Z;

            if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
            {
                bot->SummonCreature(
                    15631,
                    destX,
                    destY,
                    destZ,
                    0.0f,
                    TEMPSPAWN_TIMED_DESPAWN,
                    5000.0f);
            }

            if (MoveTo(bot->GetMapId(), destX, destY, destZ, false, IsReaction(), false, true))
            {
                if (IsReaction())
                    WaitForReach(step);

                return true;
            }

            return false;
        }

        bool isPossible() override
        {
            return MovementAction::isPossible() && ai->CanMove();
        }

    private:
        static constexpr uint32 MAP_ONYXIA_LAIR = 249;

        static constexpr float ONYXIA_EGG_PIT_1_X = -27.0f;
        static constexpr float ONYXIA_EGG_PIT_1_Y = -186.6f;
        static constexpr float ONYXIA_EGG_PIT_1_Z = -89.0f;

        static constexpr float ONYXIA_EGG_PIT_2_X = -27.0f;
        static constexpr float ONYXIA_EGG_PIT_2_Y = -250.0f;
        static constexpr float ONYXIA_EGG_PIT_2_Z = -89.0f;

        static constexpr float ONYXIA_SAFE_CENTER_X = 13.0f;
        static constexpr float ONYXIA_SAFE_CENTER_Y = -205.0f;
        static constexpr float ONYXIA_SAFE_CENTER_Z = -85.77f;

        static constexpr float ONYXIA_EGG_PIT_RADIUS = 20.0f;
        static constexpr float ONYXIA_EGG_PIT_MOVE_STEP = 10.0f;

    private:
        static float Distance2d(float x1, float y1, float x2, float y2)
        {
            float dx = x1 - x2;
            float dy = y1 - y2;
            return std::sqrt(dx * dx + dy * dy);
        }
    };


    class OnyxiaActionContext : public NamedObjectContext<Action>
    {
    public:
        OnyxiaActionContext()
        {
            creators["enable onyxia lair strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable onyxia lair strategy", "+onyxia lair"); };
            creators["disable onyxia lair strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable onyxia lair strategy", "-onyxia lair"); };

            creators["enable onyxia fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable onyxia fight strategy", "+onyxia"); };
            creators["disable onyxia fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable onyxia fight strategy", "-onyxia"); };

            creators["target onyxia whelp"] = [](PlayerbotAI* ai) { return new TargetOnyxiaWhelpAction(ai); };
            creators["target onyxia"] = [](PlayerbotAI* ai) { return new TargetOnyxiaAction(ai); };

            creators["move away from onyxia"] = [](PlayerbotAI* ai) { return new MoveAwayFromCreature(ai, "move away from onyxia", 10184, 10.0f); };
            creators["move out of onyxia egg pit"] = [](PlayerbotAI* ai) { return new MoveOutOfOnyxiaEggPitAction(ai); };
        }
    };
}
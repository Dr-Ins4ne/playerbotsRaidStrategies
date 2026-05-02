#pragma once
#include "DungeonActions.h"
#include "ChangeStrategyAction.h"
#include "playerbot/strategy/Action.h"
#include "playerbot/ServerFacade.h"
#include <vector>

namespace ai
{
    class OnyxiaActionBase : public Action
    {
    public:
        OnyxiaActionBase(PlayerbotAI* ai, std::string name) : Action(ai, name) {}

    protected:
        static const uint32 NPC_ONYXIA = 10184;
        static const uint32 NPC_ONYXIAN_WHELP = 11262;

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            std::list<ObjectGuid> targets = AI_VALUE(std::list<ObjectGuid>, "possible attack targets");

            for (ObjectGuid const& guid : targets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!unit)
                    continue;

                if (!unit->IsAlive())
                    continue;

                if (unit->GetEntry() == entry)
                    return unit;
            }

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget && currentTarget->IsAlive() && currentTarget->GetEntry() == entry)
                return currentTarget;

            return nullptr;
        }

        Unit* FindNearestAliveCreature(uint32 entry)
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return nullptr;

            Unit* best = nullptr;
            float bestDistance = 999999.0f;

            std::list<ObjectGuid> targets = AI_VALUE(std::list<ObjectGuid>, "possible attack targets");

            for (ObjectGuid const& guid : targets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!unit)
                    continue;

                if (!unit->IsAlive())
                    continue;

                if (unit->GetEntry() != entry)
                    continue;

                float distance = sServerFacade.GetDistance2d(bot, unit);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    best = unit;
                }
            }

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (!best && currentTarget && currentTarget->IsAlive() && currentTarget->GetEntry() == entry)
                return currentTarget;

            return best;
        }

        bool SetCurrentTarget(Unit* target)
        {
            if (!target)
                return false;

            context->GetValue<Unit*>("current target")->Set(target);
            ai->GetBot()->SetSelectionGuid(target->GetObjectGuid());

            return true;
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


    class OnyxiaActionContext : public NamedObjectContext<Action>
    {
    public:
        OnyxiaActionContext()
        {
            creators["enable onyxia lair strategy"] = [](PlayerbotAI* ai)
            {
                return new ChangeAllStrategyAction(ai, "enable onyxia lair strategy", "+onyxia lair");
            };

            creators["disable onyxia lair strategy"] = [](PlayerbotAI* ai)
            {
                return new ChangeAllStrategyAction(ai, "disable onyxia lair strategy", "-onyxia lair");
            };

            creators["enable onyxia fight strategy"] = [](PlayerbotAI* ai)
            {
                return new ChangeAllStrategyAction(ai, "enable onyxia fight strategy", "+onyxia");
            };

            creators["disable onyxia fight strategy"] = [](PlayerbotAI* ai)
            {
                return new ChangeAllStrategyAction(ai, "disable onyxia fight strategy", "-onyxia");
            };

            creators["target onyxia whelp"] = [](PlayerbotAI* ai)
            {
                return new TargetOnyxiaWhelpAction(ai);
            };

            creators["target onyxia"] = [](PlayerbotAI* ai)
            {
                return new TargetOnyxiaAction(ai);
            };

            creators["move away from onyxia"] = [](PlayerbotAI* ai)
            {
                return new MoveAwayFromCreature(ai, "move away from onyxia", 10184, 30.0f);
            };
        }
    };
}
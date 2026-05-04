#pragma once

#include "../actions/DungeonActions.h"
#include "../actions/ChangeStrategyAction.h"

#include "playerbot/strategy/Action.h"
#include "playerbot/ServerFacade.h"

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
        static const uint32 NPC_ONYXIA = 10184;
        static const uint32 NPC_ONYXIAN_WHELP = 11262;

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (IsValidCreature(currentTarget, entry))
                return currentTarget;

            Unit* unit = FindAliveCreatureInGuidList("possible attack targets", entry);
            if (unit)
                return unit;

            unit = FindAliveCreatureInGuidList("attackers", entry);
            if (unit)
                return unit;

            unit = FindAliveCreatureInGuidList("possible targets", entry);
            if (unit)
                return unit;

            return nullptr;
        }

        Unit* FindNearestAliveCreature(uint32 entry)
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return nullptr;

            Unit* best = nullptr;
            float bestDistance = 999999.0f;

            FindNearestAliveCreatureInGuidList("possible attack targets", entry, best, bestDistance);
            FindNearestAliveCreatureInGuidList("attackers", entry, best, bestDistance);
            FindNearestAliveCreatureInGuidList("possible targets", entry, best, bestDistance);

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (IsValidCreature(currentTarget, entry))
            {
                float distance = sServerFacade.GetDistance2d(bot, currentTarget);
                if (!best || distance < bestDistance)
                {
                    best = currentTarget;
                    bestDistance = distance;
                }
            }

            return best;
        }

        Unit* FindAliveCreatureInGuidList(std::string const& valueName, uint32 entry)
        {
            std::list<ObjectGuid> targets = AI_VALUE(std::list<ObjectGuid>, valueName);

            for (ObjectGuid const& guid : targets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (IsValidCreature(unit, entry))
                    return unit;
            }

            return nullptr;
        }

        void FindNearestAliveCreatureInGuidList(
            std::string const& valueName,
            uint32 entry,
            Unit*& best,
            float& bestDistance)
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return;

            std::list<ObjectGuid> targets = AI_VALUE(std::list<ObjectGuid>, valueName);

            for (ObjectGuid const& guid : targets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!IsValidCreature(unit, entry))
                    continue;

                float distance = sServerFacade.GetDistance2d(bot, unit);
                if (!best || distance < bestDistance)
                {
                    best = unit;
                    bestDistance = distance;
                }
            }
        }

        bool IsValidCreature(Unit* unit, uint32 entry)
        {
            return unit &&
                   unit->IsAlive() &&
                   unit->GetEntry() == entry;
        }

        bool SetCurrentTarget(Unit* target)
        {
            if (!target)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget &&
                currentTarget->IsAlive() &&
                currentTarget->GetObjectGuid() == target->GetObjectGuid())
            {
                // Important:
                // Returning false here prevents the action from constantly
                // consuming cycles when the bot already has the correct target.
                return false;
            }

            context->GetValue<Unit*>("current target")->Set(target);
            bot->SetSelectionGuid(target->GetObjectGuid());

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
                return new ChangeAllStrategyAction(
                    ai,
                    "enable onyxia lair strategy",
                    "+onyxia lair");
            };

            creators["disable onyxia lair strategy"] = [](PlayerbotAI* ai)
            {
                return new ChangeAllStrategyAction(
                    ai,
                    "disable onyxia lair strategy",
                    "-onyxia lair");
            };

            creators["enable onyxia fight strategy"] = [](PlayerbotAI* ai)
            {
                return new ChangeAllStrategyAction(
                    ai,
                    "enable onyxia fight strategy",
                    "+onyxia");
            };

            creators["disable onyxia fight strategy"] = [](PlayerbotAI* ai)
            {
                return new ChangeAllStrategyAction(
                    ai,
                    "disable onyxia fight strategy",
                    "-onyxia");
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
                return new MoveAwayFromCreature(
                    ai,
                    "move away from onyxia",
                    10184,
                    30.0f);
            };
        }
    };
}
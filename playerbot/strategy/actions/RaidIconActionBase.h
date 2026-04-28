#pragma once

#include "DungeonActions.h"
#include "playerbot/strategy/values/RtiTargetValue.h"
#include "Groups/Group.h"
#include <vector>

namespace ai
{
    class RaidIconActionBase : public Action
    {
    public:
        RaidIconActionBase(PlayerbotAI* ai, std::string name) : Action(ai, name) {}

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            std::list<ObjectGuid> attackTargets = AI_VALUE(std::list<ObjectGuid>, "possible attack targets");

            for (ObjectGuid const& guid : attackTargets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!unit)
                    continue;

                if (!unit->IsAlive())
                    continue;

                if (unit->GetEntry() == entry)
                    return unit;
            }

            std::list<ObjectGuid> possibleTargets = AI_VALUE(std::list<ObjectGuid>, "possible targets");

            for (ObjectGuid const& guid : possibleTargets)
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

        float GetHealthPct(Unit* unit)
        {
            if (!unit || !unit->GetMaxHealth())
                return 0.0f;

            return 100.0f * float(unit->GetHealth()) / float(unit->GetMaxHealth());
        }

        bool ShouldSkipDpsRtiSelection()
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return true;

            if (ai->IsHeal(bot))
                return true;

            if (ai->IsTank(bot))
                return true;

            return false;
        }

        bool SetCurrentTargetFromRti()
        {
            Unit* target = AI_VALUE(Unit*, "rti target");
            if (!target)
                return false;

            context->GetValue<Unit*>("current target")->Set(target);
            bot->SetSelectionGuid(target->GetObjectGuid());

            return true;
        }

        bool SetRti(std::string rti)
        {
            Value<std::string>* value = context->GetValue<std::string>("rti");
            if (!value)
                return false;

            if (value->Get() == rti)
                return false;

            value->Set(rti);

            SetCurrentTargetFromRti();

            return true;
        }

        Unit* GetHighestHpTarget(std::vector<Unit*> const& targets)
        {
            Unit* best = nullptr;
            float bestHp = -1.0f;

            for (Unit* target : targets)
            {
                if (!target || !target->IsAlive())
                    continue;

                float hp = GetHealthPct(target);
                if (hp > bestHp)
                {
                    bestHp = hp;
                    best = target;
                }
            }

            return best;
        }

        Unit* GetHighestHpTargetAbove(std::vector<Unit*> const& targets, float threshold)
        {
            Unit* best = nullptr;
            float bestHp = -1.0f;

            for (Unit* target : targets)
            {
                if (!target || !target->IsAlive())
                    continue;

                float hp = GetHealthPct(target);

                if (hp <= threshold)
                    continue;

                if (hp > bestHp)
                {
                    bestHp = hp;
                    best = target;
                }
            }

            return best;
        }

        bool SetTargetIcon(std::string icon, Unit* target)
        {
            if (!target)
                return false;

            Group* group = bot->GetGroup();
            if (!group)
                return false;

            if (bot->InBattleGround())
                return false;

            int index = RtiTargetValue::GetRtiIndex(icon);
            if (index < 0)
                return false;

            ObjectGuid currentGuid = group->GetTargetIcon(index);
            if (currentGuid == target->GetObjectGuid())
                return false;

#ifndef MANGOSBOT_TWO
            group->SetTargetIcon(index, target->GetObjectGuid());
#else
            group->SetTargetIcon(index, bot->GetObjectGuid(), target->GetObjectGuid());
#endif

            return true;
        }
    };
}
#pragma once

#include "playerbot/playerbot.h"
#include "playerbot/strategy/AiObjectContext.h"
#include "playerbot/strategy/values/RtiTargetValue.h"
#include "Groups/Group.h"

#include <list>
#include <string>
#include <vector>

namespace ai
{
    class DungeonTargetHelper
    {
    public:
        static Unit* FindAliveCreature(PlayerbotAI* ai, uint32 entry)
        {
            return FindCreature(ai, entry, true);
        }

        static Unit* FindCreature(PlayerbotAI* ai, uint32 entry, bool aliveOnly = true)
        {
            if (!ai)
                return nullptr;

            auto isValid = [entry, aliveOnly](Unit* unit) -> bool
            {
                if (!unit)
                    return false;

                if (aliveOnly && !unit->IsAlive())
                    return false;

                return unit->GetEntry() == entry;
            };

            Unit* currentTarget = GetUnitValue(ai, "current target");
            if (isValid(currentTarget))
                return currentTarget;

            Unit* unit = FindUnitInGuidList(ai, "possible attack targets", isValid);
            if (unit)
                return unit;

            unit = FindUnitInGuidList(ai, "attackers", isValid);
            if (unit)
                return unit;

            unit = FindUnitInGuidList(ai, "possible targets", isValid);
            if (unit)
                return unit;

            return nullptr;
        }

        static std::vector<Unit*> FindAliveCreatures(PlayerbotAI* ai, uint32 entry)
        {
            return FindCreatures(ai, entry, true);
        }

        static std::vector<Unit*> FindCreatures(PlayerbotAI* ai, uint32 entry, bool aliveOnly = true)
        {
            std::vector<Unit*> result;

            if (!ai)
                return result;

            auto isValid = [entry, aliveOnly](Unit* unit) -> bool
            {
                if (!unit)
                    return false;

                if (aliveOnly && !unit->IsAlive())
                    return false;

                return unit->GetEntry() == entry;
            };

            AddUnitsFromGuidList(ai, "possible attack targets", result, isValid);
            AddUnitsFromGuidList(ai, "attackers", result, isValid);
            AddUnitsFromGuidList(ai, "possible targets", result, isValid);

            Unit* currentTarget = GetUnitValue(ai, "current target");
            if (isValid(currentTarget) && !ContainsUnit(result, currentTarget))
                result.push_back(currentTarget);

            return result;
        }

        static float GetHealthPct(Unit* unit)
        {
            if (!unit || !unit->GetMaxHealth())
                return 0.0f;

            return 100.0f * float(unit->GetHealth()) / float(unit->GetMaxHealth());
        }

        static Unit* GetHighestHpTarget(std::vector<Unit*> const& targets)
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

        static Unit* GetHighestHpTargetAbove(std::vector<Unit*> const& targets, float threshold)
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

        static bool HasAura(PlayerbotAI* ai, Unit* unit, uint32 spellId)
        {
            return ai && unit && ai->HasAura(spellId, unit);
        }

        static bool HasAnyAura(PlayerbotAI* ai, Unit* unit, std::vector<std::string> const& auraNames)
        {
            if (!ai || !unit)
                return false;

            for (std::string const& aura : auraNames)
            {
                if (ai->HasAura(aura, unit))
                    return true;
            }

            return false;
        }

        static bool IsAliveGroupPlayer(Player* bot, Unit* unit, bool excludeSelf = true)
        {
            if (!bot || !unit)
                return false;

            if (!unit->IsAlive())
                return false;

            if (excludeSelf && unit == bot)
                return false;

            if (unit->GetTypeId() != TYPEID_PLAYER)
                return false;

            return bot->IsInGroup(unit);
        }

        static Unit* FindGroupPlayerWithAura(PlayerbotAI* ai, Player* bot, uint32 auraId, bool excludeSelf = true)
        {
            if (!ai || !bot)
                return nullptr;

            auto isValid = [ai, bot, auraId, excludeSelf](Unit* unit) -> bool
            {
                return IsAliveGroupPlayer(bot, unit, excludeSelf) &&
                       ai->HasAura(auraId, unit);
            };

            Unit* target = FindUnitInGuidList(ai, "nearest friendly players", isValid);
            if (target)
                return target;

            target = FindUnitInGuidList(ai, "possible attack targets", isValid);
            if (target)
                return target;

            target = FindUnitInGuidList(ai, "attackers", isValid);
            if (target)
                return target;

            target = FindUnitInGuidList(ai, "possible targets", isValid);
            if (target)
                return target;

            Unit* currentTarget = GetUnitValue(ai, "current target");
            if (isValid(currentTarget))
                return currentTarget;

            return nullptr;
        }

        static bool HasCorrectTargetIcon(
            PlayerbotAI* ai,
            Player* bot,
            std::string const& icon,
            Unit* target)
        {
            if (!ai || !bot || !target)
                return false;

            Group* group = bot->GetGroup();
            if (!group)
                return false;

            if (bot->InBattleGround())
                return false;

            int index = RtiTargetValue::GetRtiIndex(icon);
            if (index < 0)
                return false;

            return group->GetTargetIcon(index) == target->GetObjectGuid();
        }

        static bool ContainsUnit(std::vector<Unit*> const& units, Unit* unit)
        {
            if (!unit)
                return false;

            for (Unit* existing : units)
            {
                if (existing &&
                    existing->GetObjectGuid() == unit->GetObjectGuid())
                {
                    return true;
                }
            }

            return false;
        }

        static bool IsAliveCreature(Unit* unit, uint32 entry)
        {
            return unit &&
                   unit->IsAlive() &&
                   unit->GetEntry() == entry;
        }

        template <typename Predicate>
        static Unit* FindUnitInGuidList(PlayerbotAI* ai, std::string const& valueName, Predicate predicate)
        {
            if (!ai)
                return nullptr;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return nullptr;

            std::list<ObjectGuid> const& guids =
                context->GetValue<std::list<ObjectGuid>>(valueName)->Get();

            for (ObjectGuid const& guid : guids)
            {
                Unit* unit = ai->GetUnit(guid);
                if (predicate(unit))
                    return unit;
            }

            return nullptr;
        }

        template <typename Predicate>
        static void AddUnitsFromGuidList(
            PlayerbotAI* ai,
            std::string const& valueName,
            std::vector<Unit*>& result,
            Predicate predicate)
        {
            if (!ai)
                return;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return;

            std::list<ObjectGuid> const& guids =
                context->GetValue<std::list<ObjectGuid>>(valueName)->Get();

            for (ObjectGuid const& guid : guids)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!predicate(unit))
                    continue;

                if (ContainsUnit(result, unit))
                    continue;

                result.push_back(unit);
            }
        }

        static Unit* GetUnitValue(PlayerbotAI* ai, std::string const& valueName)
        {
            if (!ai)
                return nullptr;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return nullptr;

            return context->GetValue<Unit*>(valueName)->Get();
        }
    };
}
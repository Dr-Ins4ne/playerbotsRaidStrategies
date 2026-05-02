#pragma once

#include "playerbot/playerbot.h"
#include "playerbot/strategy/AiObjectContext.h"

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

        static float GetHealthPct(Unit* unit)
        {
            if (!unit || !unit->GetMaxHealth())
                return 0.0f;

            return 100.0f * float(unit->GetHealth()) / float(unit->GetMaxHealth());
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
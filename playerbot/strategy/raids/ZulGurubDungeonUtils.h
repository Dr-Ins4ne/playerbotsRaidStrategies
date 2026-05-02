#pragma once

#include "DungeonTargetHelper.h"

#include <string>
#include <vector>

namespace ai
{
    class ZulGurubDungeonUtils
    {
    public:
        static const uint32 NPC_HAKKAR = 14834;
        static const uint32 SPELL_HAKKAR_MIND_CONTROL = 24327;

        static constexpr float HAKKAR_TANK_X = -11787.0f;
        static constexpr float HAKKAR_TANK_Y = -1667.0f;
        static constexpr float HAKKAR_TANK_Z = 52.9f;
        static constexpr float HAKKAR_TANK_POSITION_RADIUS = 4.0f;

        static Unit* FindHakkar(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, NPC_HAKKAR);
        }

        static bool IsAtHakkarTankPosition(Player* bot)
        {
            if (!bot)
                return false;

            return bot->GetDistance(
                HAKKAR_TANK_X,
                HAKKAR_TANK_Y,
                HAKKAR_TANK_Z
            ) <= HAKKAR_TANK_POSITION_RADIUS;
        }

        static std::string GetCcSpell(Player* bot)
        {
            if (!bot)
                return "";

            switch (bot->getClass())
            {
                case CLASS_MAGE:
                    return "polymorph";

                case CLASS_WARLOCK:
                    return "fear";

                case CLASS_DRUID:
                    return "entangling roots";

                default:
                    return "";
            }
        }

        static bool IsSupportedCcClass(Player* bot)
        {
            return !GetCcSpell(bot).empty();
        }

        static bool IsAlreadyCrowdControlled(PlayerbotAI* ai, Unit* unit)
        {
            static const std::vector<std::string> ccAuras =
            {
                "polymorph",
                "fear",
                "entangling roots"
            };

            return DungeonTargetHelper::HasAnyAura(ai, unit, ccAuras);
        }

        static bool IsValidMindControlTarget(PlayerbotAI* ai, Player* bot, Unit* unit)
        {
            if (!DungeonTargetHelper::IsAliveGroupPlayer(bot, unit, true))
                return false;

            if (!DungeonTargetHelper::HasAura(ai, unit, SPELL_HAKKAR_MIND_CONTROL))
                return false;

            if (IsAlreadyCrowdControlled(ai, unit))
                return false;

            return true;
        }

        static Unit* FindMindControlledTargetInList(
            PlayerbotAI* ai,
            Player* bot,
            std::string const& valueName)
        {
            auto predicate = [ai, bot](Unit* unit) -> bool
            {
                return IsValidMindControlTarget(ai, bot, unit);
            };

            return DungeonTargetHelper::FindUnitInGuidList(ai, valueName, predicate);
        }

        static Unit* FindMindControlledTarget(PlayerbotAI* ai, Player* bot)
        {
            if (!ai || !bot)
                return nullptr;

            Unit* target = FindMindControlledTargetInList(ai, bot, "nearest friendly players");
            if (target)
                return target;

            target = FindMindControlledTargetInList(ai, bot, "possible attack targets");
            if (target)
                return target;

            target = FindMindControlledTargetInList(ai, bot, "attackers");
            if (target)
                return target;

            target = FindMindControlledTargetInList(ai, bot, "possible targets");
            if (target)
                return target;

            Unit* currentTarget = DungeonTargetHelper::GetUnitValue(ai, "current target");
            if (IsValidMindControlTarget(ai, bot, currentTarget))
                return currentTarget;

            return nullptr;
        }

        static bool CanCcMindControlledTarget(PlayerbotAI* ai, Player* bot)
        {
            if (!ai || !bot)
                return false;

            std::string spell = GetCcSpell(bot);
            if (spell.empty())
                return false;

            Unit* target = FindMindControlledTarget(ai, bot);
            if (!target)
                return false;

            return ai->CanCastSpell(spell, target, true);
        }

        static bool CastCcOnMindControlledTarget(PlayerbotAI* ai, Player* bot)
        {
            if (!ai || !bot)
                return false;

            if (bot->GetCurrentSpell(CURRENT_GENERIC_SPELL) ||
                bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                return false;

            std::string spell = GetCcSpell(bot);
            if (spell.empty())
                return false;

            Unit* target = FindMindControlledTarget(ai, bot);
            if (!target)
                return false;

            if (!ai->CanCastSpell(spell, target, true))
                return false;

            return ai->CastSpell(spell, target);
        }
    };
}
#pragma once

#include "../actions/DungeonActions.h"
#include "DungeonTargetHelper.h"
#include "playerbot/strategy/values/RtiTargetValue.h"
#include "Groups/Group.h"

#include <algorithm>
#include <list>
#include <string>
#include <utility>
#include <vector>

namespace ai
{
    class RaidIconActionBase : public Action
    {
    public:
        RaidIconActionBase(PlayerbotAI* ai, std::string name)
            : Action(ai, name) {}

    protected:
        // ---------------------------------------------------------------------
        // Generic creature / unit wrappers
        // ---------------------------------------------------------------------

        Unit* FindAliveCreature(uint32 entry)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, entry);
        }

        std::vector<Unit*> FindAliveCreatures(uint32 entry)
        {
            return DungeonTargetHelper::FindAliveCreatures(ai, entry);
        }

        float GetHealthPct(Unit* unit)
        {
            return DungeonTargetHelper::GetHealthPct(unit);
        }

        Unit* GetHighestHpTarget(std::vector<Unit*> const& targets)
        {
            return DungeonTargetHelper::GetHighestHpTarget(targets);
        }

        Unit* GetHighestHpTargetAbove(std::vector<Unit*> const& targets, float threshold)
        {
            return DungeonTargetHelper::GetHighestHpTargetAbove(targets, threshold);
        }

        bool HasAnyAura(Unit* target, std::vector<std::string> const& auraNames)
        {
            return DungeonTargetHelper::HasAnyAura(ai, target, auraNames);
        }


        // ---------------------------------------------------------------------
        // RTI / raid-icon target helpers
        // ---------------------------------------------------------------------

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
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            Unit* target = AI_VALUE(Unit*, "rti target");
            if (!target)
                return false;

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget &&
                currentTarget->IsAlive() &&
                currentTarget->GetObjectGuid() == target->GetObjectGuid())
            {
                return false;
            }

            context->GetValue<Unit*>("current target")->Set(target);
            bot->SetSelectionGuid(target->GetObjectGuid());

            return true;
        }

        bool SetRti(std::string const& rti)
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

        bool SetTargetIcon(std::string const& icon, Unit* target)
        {
            if (!target)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
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

        bool SetTargetIcon(std::string const& icon, uint32 entry)
        {
            return SetTargetIcon(icon, FindAliveCreature(entry));
        }

        bool SetTargetIconsByEntry(std::vector<std::pair<std::string, uint32>> const& assignments)
        {
            bool changed = false;

            for (std::pair<std::string, uint32> const& assignment : assignments)
            {
                changed |= SetTargetIcon(assignment.first, assignment.second);
            }

            return changed;
        }

        bool SetTargetIconsForCreatures(
            std::vector<std::string> const& icons,
            std::vector<Unit*> const& targets)
        {
            bool changed = false;

            size_t count = icons.size() < targets.size() ? icons.size() : targets.size();

            for (size_t i = 0; i < count; ++i)
            {
                changed |= SetTargetIcon(icons[i], targets[i]);
            }

            return changed;
        }

        bool SetTargetIconsForEntry(std::vector<std::string> const& icons, uint32 entry)
        {
            return SetTargetIconsForCreatures(icons, FindAliveCreatures(entry));
        }

        bool HasCorrectTargetIcon(std::string const& icon, Unit* target)
        {
            if (!target)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
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

        bool HasCorrectTargetIcon(std::string const& icon, uint32 entry)
        {
            return HasCorrectTargetIcon(icon, FindAliveCreature(entry));
        }

        Unit* GetTargetIconUnit(std::string const& icon)
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return nullptr;

            Group* group = bot->GetGroup();
            if (!group)
                return nullptr;

            int index = RtiTargetValue::GetRtiIndex(icon);
            if (index < 0)
                return nullptr;

            ObjectGuid guid = group->GetTargetIcon(index);

            // Do not use "if (!guid)" here.
            // Some ObjectGuid implementations do not define operator!.
            return ai->GetUnit(guid);
        }

        Unit* FindFirstAliveIconTarget(std::vector<std::string> const& icons)
        {
            for (std::string const& icon : icons)
            {
                Unit* target = GetTargetIconUnit(icon);
                if (!target || !target->IsAlive())
                    continue;

                return target;
            }

            return nullptr;
        }

        bool SelectDpsRti(std::string const& icon, Unit* target)
        {
            if (ShouldSkipDpsRtiSelection())
                return false;

            if (!target)
                return false;

            bool changed = false;
            changed |= SetTargetIcon(icon, target);
            changed |= SetRti(icon);

            return changed;
        }

        bool SelectDpsRti(std::string const& icon, uint32 entry)
        {
            return SelectDpsRti(icon, FindAliveCreature(entry));
        }

        bool SelectFirstAliveDpsRti(std::vector<std::string> const& icons)
        {
            if (ShouldSkipDpsRtiSelection())
                return false;

            for (std::string const& icon : icons)
            {
                Unit* target = GetTargetIconUnit(icon);
                if (!target || !target->IsAlive())
                    continue;

                return SelectDpsRti(icon, target);
            }

            return false;
        }


        // ---------------------------------------------------------------------
        // Generic aura / CC helpers
        // ---------------------------------------------------------------------

        Unit* FindFirstAliveIconTargetWithoutAuras(
            std::vector<std::string> const& icons,
            std::vector<std::string> const& auraNames)
        {
            for (std::string const& icon : icons)
            {
                Unit* target = GetTargetIconUnit(icon);
                if (!target || !target->IsAlive())
                    continue;

                if (HasAnyAura(target, auraNames))
                    continue;

                return target;
            }

            return nullptr;
        }

        bool TryCastOnUnit(std::string const& spell, Unit* target)
        {
            if (!target)
                return false;

            if (!ai->CanCastSpell(spell, target, true))
                return false;

            return ai->CastSpell(spell, target);
        }

        bool TryCastAnyOnUnit(std::vector<std::string> const& spells, Unit* target)
        {
            if (!target)
                return false;

            for (std::string const& spell : spells)
            {
                if (TryCastOnUnit(spell, target))
                    return true;
            }

            return false;
        }

        std::vector<std::string> GetCommonCrowdControlAuras()
        {
            return {
                "polymorph",
                "banish",
                "fear",
                "seduction",
                "freezing trap",
                "freezing trap effect",
                "hibernate",
                "entangling roots",
                "shackle undead",
                "repentance",
                "blind",
                "gouge",
                "kidney shot"
            };
        }

        std::vector<std::string> GetCommonCrowdControlSpells()
        {
            return {
                // Mage
                "polymorph",

                // Warlock
                "banish",
                "fear",
                "seduction",

                // Hunter
                "freezing trap",
                "scare beast",

                // Druid
                "hibernate",
                "entangling roots",

                // Priest
                "shackle undead",

                // Paladin
                "repentance",

                // Rogue / short-control fallback
                "blind",
                "gouge",
                "kidney shot"
            };
        }

        bool IsCommonCrowdControlled(Unit* target)
        {
            return HasAnyAura(target, GetCommonCrowdControlAuras());
        }

        bool TryCommonCrowdControl(Unit* target)
        {
            return TryCastAnyOnUnit(GetCommonCrowdControlSpells(), target);
        }
    };
}
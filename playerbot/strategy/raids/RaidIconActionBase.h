#pragma once

#include "../actions/DungeonActions.h"
#include "DungeonTargetHelper.h"

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

        bool ContainsUnit(std::vector<Unit*> const& units, Unit* unit)
        {
            return DungeonTargetHelper::ContainsUnit(units, unit);
        }

        // ---------------------------------------------------------------------
        // Current target / RTI wrappers
        // ---------------------------------------------------------------------

        Unit* GetCurrentTarget()
        {
            return DungeonTargetHelper::GetCurrentTarget(ai);
        }

        bool IsCurrentTarget(Unit* target)
        {
            return DungeonTargetHelper::IsCurrentTarget(ai, target);
        }

        bool SetCurrentTarget(Unit* target)
        {
            return DungeonTargetHelper::SetCurrentTarget(ai, target);
        }

        Unit* GetRtiTarget()
        {
            return DungeonTargetHelper::GetRtiTarget(ai);
        }

        bool SetCurrentTargetFromRti()
        {
            return DungeonTargetHelper::SetCurrentTargetFromRti(ai);
        }

        bool HasRti(std::string const& icon)
        {
            return DungeonTargetHelper::HasRti(ai, icon);
        }

        bool SetRti(std::string const& icon)
        {
            return DungeonTargetHelper::SetRti(ai, icon);
        }

        bool ShouldSkipDpsRtiSelection()
        {
            return DungeonTargetHelper::ShouldSkipDpsRtiSelection(ai);
        }

        bool NeedsDpsRtiSelection(std::string const& icon, Unit* target)
        {
            return DungeonTargetHelper::NeedsDpsRtiSelection(ai, icon, target);
        }

        bool IsDpsRtiSelected(std::string const& icon, Unit* target)
        {
            if (!target)
                return false;

            return HasCorrectTargetIcon(icon, target) &&
                   HasRti(icon) &&
                   IsCurrentTarget(target);
        }

        // ---------------------------------------------------------------------
        // Raid icon wrappers
        // ---------------------------------------------------------------------

        bool SetTargetIcon(std::string const& icon, Unit* target)
        {
            return DungeonTargetHelper::SetTargetIcon(ai, icon, target);
        }

        bool SetTargetIcon(std::string const& icon, uint32 entry)
        {
            return SetTargetIcon(icon, FindAliveCreature(entry));
        }

        bool HasCorrectTargetIcon(std::string const& icon, Unit* target)
        {
            return DungeonTargetHelper::HasCorrectTargetIcon(ai, icon, target);
        }

        bool HasCorrectTargetIcon(std::string const& icon, uint32 entry)
        {
            return HasCorrectTargetIcon(icon, FindAliveCreature(entry));
        }

        Unit* GetTargetIconUnit(std::string const& icon)
        {
            return DungeonTargetHelper::GetTargetIconUnit(ai, icon);
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

        // ---------------------------------------------------------------------
        // DPS RTI selection helpers
        // ---------------------------------------------------------------------

        bool SelectDpsRti(std::string const& icon, Unit* target)
        {
            if (ShouldSkipDpsRtiSelection())
                return false;

            if (!target || !target->IsAlive())
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

        bool SelectFirstAliveDpsRtiWithoutAuras(
            std::vector<std::string> const& icons,
            std::vector<std::string> const& auraNames)
        {
            if (ShouldSkipDpsRtiSelection())
                return false;

            Unit* target = FindFirstAliveIconTargetWithoutAuras(icons, auraNames);
            if (!target)
                return false;

            for (std::string const& icon : icons)
            {
                Unit* iconTarget = GetTargetIconUnit(icon);
                if (!iconTarget || !iconTarget->IsAlive())
                    continue;

                if (iconTarget->GetObjectGuid() != target->GetObjectGuid())
                    continue;

                return SelectDpsRti(icon, target);
            }

            return false;
        }

        // ---------------------------------------------------------------------
        // Generic cast helpers
        // ---------------------------------------------------------------------

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

        // ---------------------------------------------------------------------
        // Generic crowd-control helpers
        // ---------------------------------------------------------------------

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
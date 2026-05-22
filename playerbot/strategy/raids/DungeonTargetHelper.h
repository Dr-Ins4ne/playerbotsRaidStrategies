#pragma once

#include "playerbot/playerbot.h"
#include "playerbot/strategy/AiObjectContext.h"
#include "playerbot/strategy/values/RtiTargetValue.h"
#include "Groups/Group.h"

#include <cfloat>
#include <cmath>
#include <list>
#include <limits>
#include <string>
#include <vector>

namespace ai
{
    class DungeonTargetHelper
    {
    public:
        // ---------------------------------------------------------------------
        // Core unit helpers
        // ---------------------------------------------------------------------

        static Unit* GetVictim(Unit* unit)
        {
            if (!unit)
                return nullptr;

#ifdef CMANGOS
            return unit->GetVictim();
#else
            return unit->getVictim();
#endif
        }

        static bool IsUnitTargetingBot(PlayerbotAI* ai, Unit* unit)
        {
            if (!ai || !unit || !unit->IsAlive())
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            Unit* victim = GetVictim(unit);
            return victim && victim->GetObjectGuid() == bot->GetObjectGuid();
        }

        static bool IsCreatureTargetingBot(PlayerbotAI* ai, uint32 entry)
        {
            Unit* creature = FindAliveCreature(ai, entry);
            return IsUnitTargetingBot(ai, creature);
        }

        static float DistanceToPosition(Unit* unit, float x, float y, float z)
        {
            if (!unit)
                return FLT_MAX;

            float dx = unit->GetPositionX() - x;
            float dy = unit->GetPositionY() - y;
            float dz = unit->GetPositionZ() - z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }

        static bool IsUnitNearPosition(Unit* unit, float x, float y, float z, float maxDistance)
        {
            return DistanceToPosition(unit, x, y, z) <= maxDistance;
        }

        static bool IsBotNearPosition(PlayerbotAI* ai, float x, float y, float z, float maxDistance)
        {
            return ai && IsUnitNearPosition(ai->GetBot(), x, y, z, maxDistance);
        }

        static bool IsValidUnitForSelection(PlayerbotAI* ai, Unit* unit, bool aliveOnly = true)
        {
            if (!ai || !unit)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (!unit->IsInWorld())
                return false;

            if (unit->GetMapId() != bot->GetMapId())
                return false;

            if (aliveOnly && !unit->IsAlive())
                return false;

            return true;
        }

        static bool IsAliveCreature(Unit* unit, uint32 entry)
        {
            return unit &&
                   unit->IsAlive() &&
                   unit->GetEntry() == entry;
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

        static Unit* GetUnitValue(PlayerbotAI* ai, std::string const& valueName)
        {
            if (!ai)
                return nullptr;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return nullptr;

            Value<Unit*>* value = context->GetValue<Unit*>(valueName);
            if (!value)
                return nullptr;

            return value->Get();
        }

        template <typename Predicate>
        static Unit* FindUnitInGuidList(PlayerbotAI* ai, std::string const& valueName, Predicate predicate)
        {
            if (!ai)
                return nullptr;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return nullptr;

            Value<std::list<ObjectGuid>>* value = context->GetValue<std::list<ObjectGuid>>(valueName);
            if (!value)
                return nullptr;

            std::list<ObjectGuid> const& guids = value->Get();

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

            Value<std::list<ObjectGuid>>* value = context->GetValue<std::list<ObjectGuid>>(valueName);
            if (!value)
                return;

            std::list<ObjectGuid> const& guids = value->Get();

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

        // ---------------------------------------------------------------------
        // Creature lookup helpers
        // ---------------------------------------------------------------------

        static Unit* FindAliveCreature(PlayerbotAI* ai, uint32 entry)
        {
            return FindCreature(ai, entry, true);
        }

        static Unit* FindCreature(PlayerbotAI* ai, uint32 entry, bool aliveOnly = true)
        {
            if (!ai)
                return nullptr;

            auto isValid = [ai, entry, aliveOnly](Unit* unit) -> bool
            {
                if (!IsValidUnitForSelection(ai, unit, aliveOnly))
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

            auto isValid = [ai, entry, aliveOnly](Unit* unit) -> bool
            {
                if (!IsValidUnitForSelection(ai, unit, aliveOnly))
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

        static Unit* FindNearestAliveCreature(PlayerbotAI* ai, uint32 entry, Unit* reference = nullptr)
        {
            std::vector<Unit*> units = FindAliveCreatures(ai, entry);
            if (units.empty())
                return nullptr;

            Player* bot = ai ? ai->GetBot() : nullptr;
            if (!reference)
                reference = bot;

            if (!reference)
                return nullptr;

            Unit* best = nullptr;
            float bestDistance = std::numeric_limits<float>::max();

            for (Unit* unit : units)
            {
                if (!IsValidUnitForSelection(ai, unit, true))
                    continue;

                float distance = reference->GetDistance(unit);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    best = unit;
                }
            }

            return best;
        }

        // ---------------------------------------------------------------------
        // Health helpers
        // ---------------------------------------------------------------------

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

        // ---------------------------------------------------------------------
        // Aura helpers
        // ---------------------------------------------------------------------

        static bool HasAura(PlayerbotAI* ai, Unit* unit, uint32 spellId)
        {
            return ai && unit && ai->HasAura(spellId, unit);
        }

        static bool HasAura(PlayerbotAI* ai, Unit* unit, std::string const& auraName)
        {
            return ai && unit && ai->HasAura(auraName, unit);
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

        // ---------------------------------------------------------------------
        // Group-player helpers
        // ---------------------------------------------------------------------

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

            Unit* currentTarget = GetUnitValue(ai, "current target");
            if (isValid(currentTarget))
                return currentTarget;

            target = FindUnitInGuidList(ai, "possible targets", isValid);
            if (target)
                return target;

            target = FindUnitInGuidList(ai, "possible attack targets", isValid);
            if (target)
                return target;

            target = FindUnitInGuidList(ai, "attackers", isValid);
            if (target)
                return target;

            return nullptr;
        }

        // ---------------------------------------------------------------------
        // Current target helpers
        // ---------------------------------------------------------------------

        static Unit* GetCurrentTarget(PlayerbotAI* ai)
        {
            return GetUnitValue(ai, "current target");
        }

        static bool IsInvalidCurrentTarget(PlayerbotAI* ai)
        {
            if (!ai)
                return true;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return true;

            Value<bool>* value = context->GetValue<bool>("invalid target", "current target");
            if (!value)
                return true;

            return value->Get();
        }

        static bool IsCurrentTarget(PlayerbotAI* ai, Unit* target)
        {
            if (!target)
                return false;

            Unit* currentTarget = GetCurrentTarget(ai);
            return currentTarget &&
                   currentTarget->IsAlive() &&
                   currentTarget->GetObjectGuid() == target->GetObjectGuid();
        }

        static bool IsCurrentTargetEntry(PlayerbotAI* ai, uint32 entry)
        {
            Unit* currentTarget = GetCurrentTarget(ai);
            return currentTarget &&
                   currentTarget->IsAlive() &&
                   currentTarget->GetEntry() == entry &&
                   !IsInvalidCurrentTarget(ai);
        }

        static bool SetCurrentTarget(PlayerbotAI* ai, Unit* target)
        {
            if (!IsValidUnitForSelection(ai, target, true))
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return false;

            if (IsCurrentTarget(ai, target))
                return false;

            Value<Unit*>* value = context->GetValue<Unit*>("current target");
            if (!value)
                return false;

            value->Set(target);
            bot->SetSelectionGuid(target->GetObjectGuid());

            return true;
        }

        // ---------------------------------------------------------------------
        // RTI value helpers
        // ---------------------------------------------------------------------

        static bool HasRti(PlayerbotAI* ai, std::string const& icon)
        {
            if (!ai)
                return false;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return false;

            Value<std::string>* value = context->GetValue<std::string>("rti");
            if (!value)
                return false;

            return value->Get() == icon;
        }

        static Unit* GetRtiTarget(PlayerbotAI* ai)
        {
            return GetUnitValue(ai, "rti target");
        }

        static bool SetCurrentTargetFromRti(PlayerbotAI* ai)
        {
            Unit* target = GetRtiTarget(ai);
            if (!IsValidUnitForSelection(ai, target, true))
                return false;

            return SetCurrentTarget(ai, target);
        }

        static bool SetRti(PlayerbotAI* ai, std::string const& icon)
        {
            if (!ai)
                return false;

            AiObjectContext* context = ai->GetAiObjectContext();
            if (!context)
                return false;

            Value<std::string>* value = context->GetValue<std::string>("rti");
            if (!value)
                return false;

            bool changed = false;

            if (value->Get() != icon)
            {
                value->Set(icon);
                changed = true;
            }

            changed |= SetCurrentTargetFromRti(ai);

            return changed;
        }

        

        static bool SetRtiAndCurrentTarget(PlayerbotAI* ai, std::string const& icon, Unit* target)
        {
            bool changed = false;

            changed |= SetRti(ai, icon);

            if (IsValidUnitForSelection(ai, target, true))
                changed |= SetCurrentTarget(ai, target);

            return changed;
        }

        static bool ShouldSkipDpsRtiSelection(PlayerbotAI* ai)
        {
            if (!ai)
                return true;

            Player* bot = ai->GetBot();
            if (!bot)
                return true;

            if (ai->IsHeal(bot))
                return true;

            if (ai->IsTank(bot))
                return true;

            return false;
        }

        // ---------------------------------------------------------------------
        // Raid-icon helpers
        // ---------------------------------------------------------------------

        static bool CanUseTargetIcons(PlayerbotAI* ai)
        {
            if (!ai)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (!bot->GetGroup())
                return false;

            if (bot->InBattleGround())
                return false;

            return true;
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

        static bool HasCorrectTargetIcon(PlayerbotAI* ai, std::string const& icon, Unit* target)
        {
            Player* bot = ai ? ai->GetBot() : nullptr;
            return HasCorrectTargetIcon(ai, bot, icon, target);
        }

        static Unit* GetTargetIconUnit(PlayerbotAI* ai, std::string const& icon)
        {
            if (!ai)
                return nullptr;

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
            return ai->GetUnit(guid);
        }

        static bool SetTargetIcon(PlayerbotAI* ai, std::string const& icon, Unit* target)
        {
            if (!IsValidUnitForSelection(ai, target, true))
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

        static bool SetTargetIcon(PlayerbotAI* ai, std::string const& icon, uint32 entry)
        {
            Unit* target = FindAliveCreature(ai, entry);
            return SetTargetIcon(ai, icon, target);
        }

        static bool NeedsDpsRtiSelection(PlayerbotAI* ai, std::string const& icon, Unit* target)
        {
            if (ShouldSkipDpsRtiSelection(ai))
                return false;

            if (!IsValidUnitForSelection(ai, target, true))
                return false;

            if (!CanUseTargetIcons(ai))
                return false;

            return !HasCorrectTargetIcon(ai, icon, target) ||
                   !HasRti(ai, icon) ||
                   !IsCurrentTarget(ai, target);
        }

        static bool SelectDpsRti(PlayerbotAI* ai, std::string const& icon, Unit* target)
        {
            if (!NeedsDpsRtiSelection(ai, icon, target))
                return false;

            bool changed = false;
            changed |= SetTargetIcon(ai, icon, target);
            changed |= SetRtiAndCurrentTarget(ai, icon, target);

            return changed;
        }

        static bool SelectDpsRti(PlayerbotAI* ai, std::string const& icon, uint32 entry)
        {
            return SelectDpsRti(ai, icon, FindAliveCreature(ai, entry));
        }

        static Unit* FindFirstAliveIconTarget(PlayerbotAI* ai, std::vector<std::string> const& icons)
        {
            for (std::string const& icon : icons)
            {
                Unit* target = GetTargetIconUnit(ai, icon);
                if (!IsValidUnitForSelection(ai, target, true))
                    continue;

                return target;
            }

            return nullptr;
        }

        static Unit* FindFirstAliveIconTargetWithoutAuras(
            PlayerbotAI* ai,
            std::vector<std::string> const& icons,
            std::vector<std::string> const& auraNames)
        {
            for (std::string const& icon : icons)
            {
                Unit* target = GetTargetIconUnit(ai, icon);
                if (!IsValidUnitForSelection(ai, target, true))
                    continue;

                if (HasAnyAura(ai, target, auraNames))
                    continue;

                return target;
            }

            return nullptr;
        }

        static bool SelectFirstAliveDpsRti(PlayerbotAI* ai, std::vector<std::string> const& icons)
        {
            if (ShouldSkipDpsRtiSelection(ai))
                return false;

            for (std::string const& icon : icons)
            {
                Unit* target = GetTargetIconUnit(ai, icon);
                if (!IsValidUnitForSelection(ai, target, true))
                    continue;

                return SelectDpsRti(ai, icon, target);
            }

            return false;
        }

        // ---------------------------------------------------------------------
        // Spell convenience helpers
        // ---------------------------------------------------------------------

        static bool TryCastOnUnit(PlayerbotAI* ai, std::string const& spell, Unit* target)
        {
            if (!ai || !target)
                return false;

            if (!ai->CanCastSpell(spell, target, true))
                return false;

            return ai->CastSpell(spell, target);
        }

        static bool TryCastAnyOnUnit(PlayerbotAI* ai, std::vector<std::string> const& spells, Unit* target)
        {
            if (!ai || !target)
                return false;

            for (std::string const& spell : spells)
            {
                if (TryCastOnUnit(ai, spell, target))
                    return true;
            }

            return false;
        }
    };
}
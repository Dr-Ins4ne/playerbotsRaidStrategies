#pragma once

#include "DungeonTargetHelper.h"
#include "playerbot/playerbot.h"
#include "Spells/SpellAuraDefines.h"

#include <algorithm>
#include <string>
#include <vector>

namespace ai
{
    namespace AhnQirajTemple
    {
        static constexpr uint32 MAP_AHNQIRAJ_TEMPLE = 531;
        static constexpr uint32 NPC_PROPHET_SKERAM = 15263;

        static constexpr float SKERAM_PULL_X = -8361.0f;
        static constexpr float SKERAM_PULL_Y = 2074.0f;
        static constexpr float SKERAM_PULL_Z = 125.7f;
        static constexpr float SKERAM_PULL_TOLERANCE = 3.0f;

        static constexpr float SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE = 10.0f;

        static constexpr uint32 ITEM_GREATER_NATURE_PROTECTION_POTION = 13458;
        static constexpr uint32 SPELL_GREATER_NATURE_PROTECTION = 17546;

        inline bool IsSkeram(Unit* unit)
        {
            return unit && unit->IsAlive() && unit->GetEntry() == NPC_PROPHET_SKERAM;
        }

        inline std::vector<Unit*> GetSkeramUnits(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreatures(ai, NPC_PROPHET_SKERAM);
        }

        inline Unit* GetRealSkeram(PlayerbotAI* ai)
        {
            std::vector<Unit*> units = GetSkeramUnits(ai);

            Unit* best = nullptr;
            uint32 bestMaxHealth = 0;
            uint32 bestHealth = 0;

            for (Unit* unit : units)
            {
                if (!IsSkeram(unit))
                    continue;

                uint32 maxHealth = unit->GetMaxHealth();
                uint32 health = unit->GetHealth();

                if (!best || maxHealth > bestMaxHealth || (maxHealth == bestMaxHealth && health > bestHealth))
                {
                    best = unit;
                    bestMaxHealth = maxHealth;
                    bestHealth = health;
                }
            }

            return best;
        }

        inline bool IsRealSkeramTargetingBot(PlayerbotAI* ai)
        {
            if (!ai)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            Unit* real = GetRealSkeram(ai);
            if (!real)
                return false;

            Unit* victim = DungeonTargetHelper::GetVictim(real);
            if (!victim)
                return false;

            return victim->GetObjectGuid() == bot->GetObjectGuid();
        }

        inline bool IsBotNearSkeramPullPosition(PlayerbotAI* ai, float tolerance = SKERAM_PULL_TOLERANCE)
        {
            if (!ai)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (bot->GetMapId() != MAP_AHNQIRAJ_TEMPLE)
                return false;

            return bot->GetDistance(SKERAM_PULL_X, SKERAM_PULL_Y, SKERAM_PULL_Z) <= tolerance;
        }

        inline bool IsRealSkeram(PlayerbotAI* ai, Unit* unit)
        {
            if (!IsSkeram(unit))
                return false;

            Unit* real = GetRealSkeram(ai);
            return real && real->GetObjectGuid() == unit->GetObjectGuid();
        }

        inline bool IsSkeramImage(PlayerbotAI* ai, Unit* unit)
        {
            if (!IsSkeram(unit))
                return false;

            Unit* real = GetRealSkeram(ai);
            if (!real)
                return false;

            return real->GetObjectGuid() != unit->GetObjectGuid();
        }

        inline std::vector<Unit*> GetSkeramImages(PlayerbotAI* ai)
        {
            std::vector<Unit*> units = GetSkeramUnits(ai);
            Unit* real = GetRealSkeram(ai);

            std::vector<Unit*> images;
            for (Unit* unit : units)
            {
                if (!IsSkeram(unit))
                    continue;

                if (real && unit->GetObjectGuid() == real->GetObjectGuid())
                    continue;

                images.push_back(unit);
            }

            std::stable_sort(images.begin(), images.end(), [](Unit* left, Unit* right)
            {
                if (!left)
                    return false;

                if (!right)
                    return true;

                return left->GetHealth() > right->GetHealth();
            });

            return images;
        }

        inline bool HasSkeramImages(PlayerbotAI* ai)
        {
            return !GetSkeramImages(ai).empty();
        }

        inline std::string GetSkeramCcSpell(Player* bot)
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

        inline bool IsSkeramControlled(Unit* unit)
        {
            if (!unit || !unit->IsAlive())
                return false;

            return unit->HasAuraType(AuraType::SPELL_AURA_MOD_CHARM);
        }

        inline bool IsSkeramControlled(PlayerbotAI* ai, Unit* unit)
        {
            if (!unit || !unit->IsAlive())
                return false;

            if (IsSkeramControlled(unit))
                return true;

            static const std::vector<std::string> controlAuras = { "true fulfillment", "mind control", "charm" };
            return DungeonTargetHelper::HasAnyAura(ai, unit, controlAuras);
        }

        inline bool IsAlreadySkeramCrowdControlled(PlayerbotAI* ai, Unit* unit)
        {
            static const std::vector<std::string> ccAuras = { "polymorph", "fear", "entangling roots" };
            return DungeonTargetHelper::HasAnyAura(ai, unit, ccAuras);
        }

        inline bool IsValidSkeramControlledTarget(PlayerbotAI* ai, Player* bot, Unit* unit, bool requireNotCc = true)
        {
            if (!DungeonTargetHelper::IsAliveGroupPlayer(bot, unit, true))
                return false;

            if (!IsSkeramControlled(ai, unit))
                return false;

            if (requireNotCc && IsAlreadySkeramCrowdControlled(ai, unit))
                return false;

            return true;
        }

        inline Unit* FindSkeramControlledTargetInList(PlayerbotAI* ai, Player* bot, std::string const& valueName, bool requireNotCc = true)
        {
            auto predicate = [ai, bot, requireNotCc](Unit* unit) -> bool
            {
                return IsValidSkeramControlledTarget(ai, bot, unit, requireNotCc);
            };

            return DungeonTargetHelper::FindUnitInGuidList(ai, valueName, predicate);
        }

        inline Unit* FindSkeramControlledTarget(PlayerbotAI* ai, Player* bot, bool requireNotCc = true)
        {
            if (!ai || !bot)
                return nullptr;

            Unit* target = FindSkeramControlledTargetInList(ai, bot, "nearest friendly players", requireNotCc);
            if (target)
                return target;

            target = FindSkeramControlledTargetInList(ai, bot, "possible attack targets", requireNotCc);
            if (target)
                return target;

            target = FindSkeramControlledTargetInList(ai, bot, "attackers", requireNotCc);
            if (target)
                return target;

            target = FindSkeramControlledTargetInList(ai, bot, "possible targets", requireNotCc);
            if (target)
                return target;

            Unit* currentTarget = DungeonTargetHelper::GetUnitValue(ai, "current target");
            if (IsValidSkeramControlledTarget(ai, bot, currentTarget, requireNotCc))
                return currentTarget;

            return nullptr;
        }

        inline Unit* FindCastableSkeramControlledTarget(PlayerbotAI* ai, Player* bot)
        {
            if (!ai || !bot)
                return nullptr;

            std::string spell = GetSkeramCcSpell(bot);
            if (spell.empty())
                return nullptr;

            auto isCastableControlled = [ai, bot, spell](Unit* unit) -> bool
            {
                return IsValidSkeramControlledTarget(ai, bot, unit, true) && ai->CanCastSpell(spell, unit, true);
            };

            Unit* target = DungeonTargetHelper::FindUnitInGuidList(ai, "nearest friendly players", isCastableControlled);
            if (target)
                return target;

            target = DungeonTargetHelper::FindUnitInGuidList(ai, "attackers", isCastableControlled);
            if (target)
                return target;

            target = DungeonTargetHelper::FindUnitInGuidList(ai, "possible attack targets", isCastableControlled);
            if (target)
                return target;

            target = DungeonTargetHelper::FindUnitInGuidList(ai, "possible targets", isCastableControlled);
            if (target)
                return target;

            Unit* currentTarget = DungeonTargetHelper::GetUnitValue(ai, "current target");
            if (isCastableControlled(currentTarget))
                return currentTarget;

            return nullptr;
        }

        inline Unit* FindSkeramControlledTargetNearBot(PlayerbotAI* ai, Player* bot, float maxDistance)
        {
            if (!ai || !bot)
                return nullptr;

            auto isNearControlled = [ai, bot, maxDistance](Unit* unit) -> bool
            {
                return IsValidSkeramControlledTarget(ai, bot, unit, false) && bot->GetDistance(unit) <= maxDistance;
            };

            Unit* target = DungeonTargetHelper::FindUnitInGuidList(ai, "nearest friendly players", isNearControlled);
            if (target)
                return target;

            target = DungeonTargetHelper::FindUnitInGuidList(ai, "attackers", isNearControlled);
            if (target)
                return target;

            target = DungeonTargetHelper::FindUnitInGuidList(ai, "possible attack targets", isNearControlled);
            if (target)
                return target;

            target = DungeonTargetHelper::FindUnitInGuidList(ai, "possible targets", isNearControlled);
            if (target)
                return target;

            Unit* currentTarget = DungeonTargetHelper::GetUnitValue(ai, "current target");
            if (isNearControlled(currentTarget))
                return currentTarget;

            return nullptr;
        }

        inline bool CanCcSkeramControlledTarget(PlayerbotAI* ai, Player* bot)
        {
            return FindCastableSkeramControlledTarget(ai, bot) != nullptr;
        }

        inline bool CastCcOnSkeramControlledTarget(PlayerbotAI* ai, Player* bot)
        {
            if (!ai || !bot)
                return false;

            std::string spell = GetSkeramCcSpell(bot);
            if (spell.empty())
                return false;

            for (int type = CURRENT_MELEE_SPELL; type < CURRENT_CHANNELED_SPELL; ++type)
            {
                Spell* currentSpell = bot->GetCurrentSpell((CurrentSpellTypes)type);
                if (currentSpell && currentSpell->CanBeInterrupted())
                {
                    bot->InterruptSpell((CurrentSpellTypes)type);
                    ai->SpellInterrupted(currentSpell->m_spellInfo->Id);
                }
            }

            Unit* target = FindCastableSkeramControlledTarget(ai, bot);
            if (!target)
                return false;

            return ai->CastSpell(spell, target);
        }

        inline bool HasGreaterNatureProtection(PlayerbotAI* ai, Player* bot)
        {
            return ai && bot && ai->HasAura(SPELL_GREATER_NATURE_PROTECTION, bot);
        }

        inline bool IsGreaterNatureProtectionPotionReady(PlayerbotAI* ai, Player* bot)
        {
            if (!ai || !bot)
                return false;

            if (HasGreaterNatureProtection(ai, bot))
                return false;

            if (!ai->HasCheat(BotCheatMask::item) && !bot->HasItemCount(ITEM_GREATER_NATURE_PROTECTION_POTION, 1))
                return false;

            ItemPrototype const* proto = sObjectMgr.GetItemPrototype(ITEM_GREATER_NATURE_PROTECTION_POTION);
            if (!proto)
                return false;

            if (!ai->HasCheat(BotCheatMask::item) && bot->CanUseItem(proto) != EQUIP_ERR_OK)
                return false;

            for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
            {
                _Spell const& spellData = proto->Spells[i];

                if (!spellData.SpellId)
                    continue;

#ifdef MANGOSBOT_ZERO
                if (spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_USE && spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_NO_DELAY_USE)
#else
                if (spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
#endif
                    continue;

                if (!sServerFacade.IsSpellReady(bot, spellData.SpellId))
                    return false;

                if (!sServerFacade.IsSpellReady(bot, spellData.SpellId, ITEM_GREATER_NATURE_PROTECTION_POTION))
                    return false;
            }

            return true;
        }
    }
}

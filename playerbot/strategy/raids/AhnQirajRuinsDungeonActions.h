#pragma once

#include "../actions/DungeonActions.h"
#include "../actions/GenericSpellActions.h"
#include "../actions/MovementActions.h"
#include "RaidIconActionBase.h"
#include "DungeonTargetHelper.h"

#include "playerbot/ServerFacade.h"

namespace ai
{
    namespace AQ20
    {
        static constexpr uint32 MAP_ID = 509;

        static constexpr uint32 NPC_OSSIRIAN = 15339;
        static constexpr uint32 NPC_KURINNAXX = 15348;
        static constexpr uint32 NPC_AYAMISS = 15369;
        static constexpr uint32 NPC_BURU = 15370;

        static constexpr uint32 NPC_BURU_EGG = 15514;
        static constexpr uint32 NPC_HIVEZARA_LARVA = 15555;

        static constexpr uint32 GO_OSSIRIAN_CRYSTAL = 180619;

        static constexpr uint32 SPELL_KURINNAXX_MORTAL_WOUND = 25646;
        static constexpr uint32 SPELL_AYAMISS_POISON_STINGER = 25748;
        static constexpr uint32 SPELL_OSSIRIAN_STRENGTH = 25176;
    }

    // ------------------------------------------------------------
    // Small local strategy-change helper
    // ------------------------------------------------------------

    class AQ20ChangeCombatStrategyAction : public Action
    {
    public:
        AQ20ChangeCombatStrategyAction(PlayerbotAI* ai, std::string name, std::string strategy, bool enable)
            : Action(ai, name), strategy(strategy), enable(enable) {}

        bool Execute(Event& event) override
        {
            ai->ChangeStrategy(std::string(enable ? "+" : "-") + strategy, BotState::BOT_STATE_COMBAT);
            return true;
        }

        bool isUseful() override
        {
            return ai->HasStrategy(strategy, BotState::BOT_STATE_COMBAT) != enable;
        }

    private:
        std::string strategy;
        bool enable;
    };

    class AQ20EnableDungeonStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        AQ20EnableDungeonStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "enable ahnqiraj ruins strategy", "ahnqiraj ruins", true) {}
    };

    class AQ20DisableDungeonStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        AQ20DisableDungeonStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "disable ahnqiraj ruins strategy", "ahnqiraj ruins", false) {}
    };

    class KurinnaxxEnableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        KurinnaxxEnableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "enable kurinnaxx strategy", "kurinnaxx", true) {}
    };

    class KurinnaxxDisableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        KurinnaxxDisableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "disable kurinnaxx strategy", "kurinnaxx", false) {}
    };

    class BuruEnableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        BuruEnableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "enable buru strategy", "buru", true) {}
    };

    class BuruDisableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        BuruDisableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "disable buru strategy", "buru", false) {}
    };

    class AyamissEnableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        AyamissEnableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "enable ayamiss strategy", "ayamiss", true) {}
    };

    class AyamissDisableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        AyamissDisableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "disable ayamiss strategy", "ayamiss", false) {}
    };

    class OssirianEnableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        OssirianEnableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "enable ossirian strategy", "ossirian", true) {}
    };

    class OssirianDisableFightStrategyAction : public AQ20ChangeCombatStrategyAction
    {
    public:
        OssirianDisableFightStrategyAction(PlayerbotAI* ai)
            : AQ20ChangeCombatStrategyAction(ai, "disable ossirian strategy", "ossirian", false) {}
    };

    // ------------------------------------------------------------
    // Shared AQ20 action helper
    // ------------------------------------------------------------

    class AQ20RaidIconActionBase : public RaidIconActionBase
    {
    public:
        AQ20RaidIconActionBase(PlayerbotAI* ai, std::string name) : RaidIconActionBase(ai, name) {}

    protected:
        Unit* GetVictim(Unit* unit)
        {
            if (!unit)
                return nullptr;

#ifdef CMANGOS
            return unit->GetVictim();
#else
            return unit->getVictim();
#endif
        }

        Unit* FindNearestAliveCreature(uint32 entry, Unit* reference = nullptr)
        {
            std::vector<Unit*> units = FindAliveCreatures(entry);
            if (units.empty())
                return nullptr;

            if (!reference)
                reference = bot;

            Unit* best = nullptr;
            float bestDistance = 999999.0f;

            for (Unit* unit : units)
            {
                if (!unit || !unit->IsAlive())
                    continue;

                float distance = reference ? reference->GetDistance(unit) : bot->GetDistance(unit);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    best = unit;
                }
            }

            return best;
        }

        bool SetCurrentTarget(Unit* target)
        {
            if (!target || !target->IsAlive())
                return false;

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget && currentTarget->GetObjectGuid() == target->GetObjectGuid())
                return false;

            context->GetValue<Unit*>("current target")->Set(target);
            bot->SetSelectionGuid(target->GetObjectGuid());
            return true;
        }

        bool SelectRtiOrCurrent(std::string const& icon, Unit* target)
        {
            if (!target)
                return false;

            bool changed = false;
            changed |= SetTargetIcon(icon, target);
            changed |= SetRti(icon);
            changed |= SetCurrentTarget(target);
            return changed;
        }
    };

    // ------------------------------------------------------------
    // Kurinnaxx
    // ------------------------------------------------------------

    class MoveAwayFromKurinnaxxSandTrapAction : public MoveAwayFromHazard
    {
    public:
        MoveAwayFromKurinnaxxSandTrapAction(PlayerbotAI* ai)
            : MoveAwayFromHazard(ai, "move away from kurinnaxx sand trap") {}
    };

    class TauntKurinnaxxAction : public CastSpellAction
    {
    public:
        TauntKurinnaxxAction(PlayerbotAI* ai) : CastSpellAction(ai, "taunt") {}

        std::string GetTargetName() override { return "creature id"; }
        std::string GetTargetQualifier() override { return std::to_string(AQ20::NPC_KURINNAXX); }

        bool isUseful() override
        {
            if (!ai->IsTank(bot))
                return false;

            Unit* boss = DungeonTargetHelper::FindAliveCreature(ai, AQ20::NPC_KURINNAXX);
            if (!boss)
                return false;

            Unit* victim = nullptr;
#ifdef CMANGOS
            victim = boss->GetVictim();
#else
            victim = boss->getVictim();
#endif

            if (victim == bot)
                return false;

            if (ai->HasAura(AQ20::SPELL_KURINNAXX_MORTAL_WOUND, bot))
                return false;

            return CastSpellAction::isUseful();
        }
    };

    class KurinnaxxTankRetreatAction : public MoveAwayFromCreature
    {
    public:
        KurinnaxxTankRetreatAction(PlayerbotAI* ai)
            : MoveAwayFromCreature(ai, "kurinnaxx tank retreat", AQ20::NPC_KURINNAXX, 30.0f) {}

        bool isUseful() override
        {
            if (!ai->IsTank(bot))
                return false;

            Aura* aura = ai->GetAura(AQ20::SPELL_KURINNAXX_MORTAL_WOUND, bot);
            return aura && aura->GetStackAmount() >= 3;
        }
    };

    // ------------------------------------------------------------
    // Buru
    // ------------------------------------------------------------

    class MoveToBuruEggAction : public MovementAction
    {
    public:
        MoveToBuruEggAction(PlayerbotAI* ai) : MovementAction(ai, "move to buru egg") {}

        bool isUseful() override
        {
            Unit* boss = DungeonTargetHelper::FindAliveCreature(ai, AQ20::NPC_BURU);
            if (!boss)
                return false;

            Unit* victim = nullptr;
#ifdef CMANGOS
            victim = boss->GetVictim();
#else
            victim = boss->getVictim();
#endif

            return victim && victim->GetObjectGuid() == bot->GetObjectGuid();
        }

        bool Execute(Event& event) override
        {
            Unit* egg = FindNearestEgg();
            if (!egg)
                return false;

            MarkEgg(egg);

            if (bot->IsWithinDistInMap(egg, 8.0f))
                return true;

            return MoveNear(egg, 6.0f);
        }

    private:
        Unit* FindNearestEgg()
        {
            std::vector<Unit*> eggs = DungeonTargetHelper::FindAliveCreatures(ai, AQ20::NPC_BURU_EGG);
            Unit* best = nullptr;
            float bestDistance = 999999.0f;

            for (Unit* egg : eggs)
            {
                if (!egg || !egg->IsAlive())
                    continue;

                float distance = bot->GetDistance(egg);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    best = egg;
                }
            }

            return best;
        }

        bool MarkEgg(Unit* egg)
        {
            if (!egg || !bot->GetGroup())
                return false;

            int index = RtiTargetValue::GetRtiIndex("skull");
            if (index < 0)
                return false;

#ifndef MANGOSBOT_TWO
            bot->GetGroup()->SetTargetIcon(index, egg->GetObjectGuid());
#else
            bot->GetGroup()->SetTargetIcon(index, bot->GetObjectGuid(), egg->GetObjectGuid());
#endif

            context->GetValue<std::string>("rti")->Set("skull");
            context->GetValue<Unit*>("current target")->Set(egg);
            bot->SetSelectionGuid(egg->GetObjectGuid());
            return true;
        }
    };

    class MarkNearestBuruEggAction : public AQ20RaidIconActionBase
    {
    public:
        MarkNearestBuruEggAction(PlayerbotAI* ai) : AQ20RaidIconActionBase(ai, "mark nearest buru egg") {}

        bool Execute(Event& event) override
        {
            Unit* reference = bot;

            Unit* boss = FindAliveCreature(AQ20::NPC_BURU);
            if (boss)
            {
                Unit* victim = GetVictim(boss);
                if (victim)
                    reference = victim;
            }

            Unit* egg = FindNearestAliveCreature(AQ20::NPC_BURU_EGG, reference);
            if (!egg)
                return false;

            return SelectRtiOrCurrent("skull", egg);
        }
    };

    class SelectBuruEggAction : public AQ20RaidIconActionBase
    {
    public:
        SelectBuruEggAction(PlayerbotAI* ai) : AQ20RaidIconActionBase(ai, "select buru egg") {}

        bool Execute(Event& event) override
        {
            if (ai->IsHeal(bot))
                return false;

            Unit* egg = GetTargetIconUnit("skull");
            if (!egg || !egg->IsAlive())
                egg = FindNearestAliveCreature(AQ20::NPC_BURU_EGG);

            if (!egg)
                return false;

            return SelectRtiOrCurrent("skull", egg);
        }
    };

    class SelectBuruBossAction : public AQ20RaidIconActionBase
    {
    public:
        SelectBuruBossAction(PlayerbotAI* ai) : AQ20RaidIconActionBase(ai, "select buru boss") {}

        bool Execute(Event& event) override
        {
            Unit* boss = FindAliveCreature(AQ20::NPC_BURU);
            if (!boss)
                return false;

            return SelectRtiOrCurrent("skull", boss);
        }
    };

    // ------------------------------------------------------------
    // Ayamiss
    // ------------------------------------------------------------

    class SelectAyamissLarvaAction : public AQ20RaidIconActionBase
    {
    public:
        SelectAyamissLarvaAction(PlayerbotAI* ai) : AQ20RaidIconActionBase(ai, "select ayamiss larva") {}

        bool Execute(Event& event) override
        {
            if (ai->IsHeal(bot))
                return false;

            Unit* larva = FindNearestAliveCreature(AQ20::NPC_HIVEZARA_LARVA);
            if (!larva)
                return false;

            return SelectRtiOrCurrent("skull", larva);
        }
    };

    class SelectAyamissBossAction : public AQ20RaidIconActionBase
    {
    public:
        SelectAyamissBossAction(PlayerbotAI* ai) : AQ20RaidIconActionBase(ai, "select ayamiss boss") {}

        bool Execute(Event& event) override
        {
            if (ai->IsHeal(bot))
                return false;

            Unit* boss = FindAliveCreature(AQ20::NPC_AYAMISS);
            if (!boss)
                return false;

            // In phase 1 Ayamiss is flying; melee cannot usefully hit her.
            if (!ai->IsRanged(bot) && GetHealthPct(boss) > 70.0f)
                return false;

            return SelectRtiOrCurrent("cross", boss);
        }
    };

    class AyamissStingerRetreatAction : public MoveAwayFromCreature
    {
    public:
        AyamissStingerRetreatAction(PlayerbotAI* ai)
            : MoveAwayFromCreature(ai, "ayamiss stinger retreat", AQ20::NPC_AYAMISS, 35.0f) {}

        bool isUseful() override
        {
            Aura* aura = ai->GetAura(AQ20::SPELL_AYAMISS_POISON_STINGER, bot);
            return aura && aura->GetStackAmount() >= 20;
        }
    };

    // ------------------------------------------------------------
    // Ossirian
    // ------------------------------------------------------------

    class MoveToOssirianCrystalAction : public MovementAction
    {
    public:
        MoveToOssirianCrystalAction(PlayerbotAI* ai) : MovementAction(ai, "move to ossirian crystal") {}

        bool isUseful() override
        {
            Unit* boss = DungeonTargetHelper::FindAliveCreature(ai, AQ20::NPC_OSSIRIAN);
            if (!boss)
                return false;

            // Tank should drag Ossirian; others can ignore movement unless already near crystal.
            return ai->IsTank(bot);
        }

        bool Execute(Event& event) override
        {
            GameObject* crystal = FindNearestCrystal();
            if (!crystal)
                return false;

            if (bot->IsWithinDistInMap(crystal, 8.0f))
                return true;

            return MoveNear(crystal, 5.0f);
        }

    private:
        GameObject* FindNearestCrystal()
        {
            std::list<ObjectGuid> guids = AI_VALUE2(std::list<ObjectGuid>, "nearest game objects no los", AQ20::GO_OSSIRIAN_CRYSTAL);

            GameObject* best = nullptr;
            float bestDistance = 999999.0f;

            for (ObjectGuid const& guid : guids)
            {
                GameObject* go = ai->GetGameObject(guid);
                if (!go)
                    continue;

                if (!sServerFacade.isSpawned(go))
                    continue;

                float distance = bot->GetDistance(go);
                if (distance < bestDistance)
                {
                    bestDistance = distance;
                    best = go;
                }
            }

            return best;
        }
    };

    class UseOssirianCrystalAction : public Action
    {
    public:
        UseOssirianCrystalAction(PlayerbotAI* ai) : Action(ai, "use ossirian crystal") {}

        bool isUseful() override
        {
            return FindNearestUsableCrystal() != nullptr;
        }

        bool Execute(Event& event) override
        {
            GameObject* crystal = FindNearestUsableCrystal();
            if (!crystal)
                return false;

            WorldPacket data(CMSG_GAMEOBJ_USE);
            data << crystal->GetObjectGuid();
            bot->GetSession()->HandleGameObjectUseOpcode(data);
            return true;
        }

    private:
        GameObject* FindNearestUsableCrystal()
        {
            Unit* boss = DungeonTargetHelper::FindAliveCreature(ai, AQ20::NPC_OSSIRIAN);
            if (!boss)
                return nullptr;

            std::list<ObjectGuid> guids = AI_VALUE2(std::list<ObjectGuid>, "nearest game objects no los", AQ20::GO_OSSIRIAN_CRYSTAL);

            for (ObjectGuid const& guid : guids)
            {
                GameObject* go = ai->GetGameObject(guid);
                if (!go)
                    continue;

                if (!sServerFacade.isSpawned(go))
                    continue;

                if (!bot->IsWithinDistInMap(go, INTERACTION_DISTANCE, false))
                    continue;

                if (!bot->CanInteract(go))
                    continue;

                return go;
            }

            return nullptr;
        }
    };

    class SelectOssirianBossAction : public AQ20RaidIconActionBase
    {
    public:
        SelectOssirianBossAction(PlayerbotAI* ai) : AQ20RaidIconActionBase(ai, "select ossirian boss") {}

        bool Execute(Event& event) override
        {
            Unit* boss = FindAliveCreature(AQ20::NPC_OSSIRIAN);
            if (!boss)
                return false;

            return SelectRtiOrCurrent("skull", boss);
        }
    };

    // ------------------------------------------------------------
    // Action context
    // ------------------------------------------------------------

    class AhnQirajRuinsActionContext : public NamedObjectContext<Action>
    {
    public:
        AhnQirajRuinsActionContext()
        {
            creators["enable ahnqiraj ruins strategy"] = [](PlayerbotAI* ai) { return new AQ20EnableDungeonStrategyAction(ai); };
            creators["disable ahnqiraj ruins strategy"] = [](PlayerbotAI* ai) { return new AQ20DisableDungeonStrategyAction(ai); };

            creators["enable kurinnaxx strategy"] = [](PlayerbotAI* ai) { return new KurinnaxxEnableFightStrategyAction(ai); };
            creators["disable kurinnaxx strategy"] = [](PlayerbotAI* ai) { return new KurinnaxxDisableFightStrategyAction(ai); };
            creators["move away from kurinnaxx sand trap"] = [](PlayerbotAI* ai) { return new MoveAwayFromKurinnaxxSandTrapAction(ai); };
            creators["taunt kurinnaxx"] = [](PlayerbotAI* ai) { return new TauntKurinnaxxAction(ai); };
            creators["kurinnaxx tank retreat"] = [](PlayerbotAI* ai) { return new KurinnaxxTankRetreatAction(ai); };

            creators["enable buru strategy"] = [](PlayerbotAI* ai) { return new BuruEnableFightStrategyAction(ai); };
            creators["disable buru strategy"] = [](PlayerbotAI* ai) { return new BuruDisableFightStrategyAction(ai); };
            creators["move to buru egg"] = [](PlayerbotAI* ai) { return new MoveToBuruEggAction(ai); };
            creators["mark nearest buru egg"] = [](PlayerbotAI* ai) { return new MarkNearestBuruEggAction(ai); };
            creators["select buru egg"] = [](PlayerbotAI* ai) { return new SelectBuruEggAction(ai); };
            creators["select buru boss"] = [](PlayerbotAI* ai) { return new SelectBuruBossAction(ai); };

            creators["enable ayamiss strategy"] = [](PlayerbotAI* ai) { return new AyamissEnableFightStrategyAction(ai); };
            creators["disable ayamiss strategy"] = [](PlayerbotAI* ai) { return new AyamissDisableFightStrategyAction(ai); };
            creators["select ayamiss larva"] = [](PlayerbotAI* ai) { return new SelectAyamissLarvaAction(ai); };
            creators["select ayamiss boss"] = [](PlayerbotAI* ai) { return new SelectAyamissBossAction(ai); };
            creators["ayamiss stinger retreat"] = [](PlayerbotAI* ai) { return new AyamissStingerRetreatAction(ai); };

            creators["enable ossirian strategy"] = [](PlayerbotAI* ai) { return new OssirianEnableFightStrategyAction(ai); };
            creators["disable ossirian strategy"] = [](PlayerbotAI* ai) { return new OssirianDisableFightStrategyAction(ai); };
            creators["move to ossirian crystal"] = [](PlayerbotAI* ai) { return new MoveToOssirianCrystalAction(ai); };
            creators["use ossirian crystal"] = [](PlayerbotAI* ai) { return new UseOssirianCrystalAction(ai); };
            creators["select ossirian boss"] = [](PlayerbotAI* ai) { return new SelectOssirianBossAction(ai); };
        }
    };
}
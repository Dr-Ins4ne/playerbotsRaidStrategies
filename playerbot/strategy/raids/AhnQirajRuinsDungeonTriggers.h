#pragma once

#include "../triggers/DungeonTriggers.h"
#include "../triggers/GenericTriggers.h"
#include "AhnQirajRuinsDungeonActions.h"

namespace ai
{
    // ------------------------------------------------------------
    // Kurinnaxx
    // ------------------------------------------------------------

    class KurinaxxSandTrapTrigger : public CloseToGameObjectHazardTrigger
    {
    public:
        KurinaxxSandTrapTrigger(PlayerbotAI* ai)
            : CloseToGameObjectHazardTrigger(ai, "kurinnaxx sand trap close", 180630, 8.0f, 60) {}
    };

    class KurinnaxxMortalWoundHighTrigger : public DungeonCreatureTrigger
    {
    public:
        KurinnaxxMortalWoundHighTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "kurinnaxx mortal wound high", 1) {}

        bool IsActive() override
        {
            Unit* boss = FindAliveCreature(AQ20::NPC_KURINNAXX);
            if (!boss)
                return false;

            Unit* victim = nullptr;
#ifdef CMANGOS
            victim = boss->GetVictim();
#else
            victim = boss->getVictim();
#endif

            if (!victim)
                return false;

            Aura* aura = ai->GetAura(AQ20::SPELL_KURINNAXX_MORTAL_WOUND, victim);
            return aura && aura->GetStackAmount() >= 3;
        }
    };

    // ------------------------------------------------------------
    // Buru
    // ------------------------------------------------------------

    class BuruFocusedMeTrigger : public DungeonCreatureTrigger
    {
    public:
        BuruFocusedMeTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "buru focused me", 1) {}

        bool IsActive() override
        {
            Unit* boss = FindAliveCreature(AQ20::NPC_BURU);
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
    };

    class BuruEggAvailableTrigger : public DungeonCreatureTrigger
    {
    public:
        BuruEggAvailableTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "buru egg available", 1) {}

        bool IsActive() override
        {
            return IsAlive(AQ20::NPC_BURU_EGG);
        }
    };

    class BuruShellBrokenTrigger : public DungeonCreatureTrigger
    {
    public:
        BuruShellBrokenTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "buru shell broken", 1) {}

        bool IsActive() override
        {
            Unit* boss = FindAliveCreature(AQ20::NPC_BURU);
            return boss && GetHealthPct(boss) <= 20.0f;
        }
    };

    // ------------------------------------------------------------
    // Ayamiss
    // ------------------------------------------------------------

    class AyamissLarvaAliveTrigger : public DungeonCreatureTrigger
    {
    public:
        AyamissLarvaAliveTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "ayamiss larva alive", 1) {}

        bool IsActive() override
        {
            return IsAlive(AQ20::NPC_HIVEZARA_LARVA);
        }
    };

    class AyamissBossAvailableTrigger : public DungeonCreatureTrigger
    {
    public:
        AyamissBossAvailableTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "ayamiss boss available", 1) {}

        bool IsActive() override
        {
            return IsAlive(AQ20::NPC_AYAMISS);
        }
    };

    class AyamissPoisonStingerHighTrigger : public HasAuraTrigger
    {
    public:
        AyamissPoisonStingerHighTrigger(PlayerbotAI* ai)
            : HasAuraTrigger(ai, "ayamiss poison stinger high", 1) {}

        bool IsActive() override
        {
            Aura* aura = ai->GetAura(AQ20::SPELL_AYAMISS_POISON_STINGER, bot);
            return aura && aura->GetStackAmount() >= 20;
        }
    };

    // ------------------------------------------------------------
    // Ossirian
    // ------------------------------------------------------------

    class OssirianNeedsCrystalTrigger : public DungeonCreatureTrigger
    {
    public:
        OssirianNeedsCrystalTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "ossirian needs crystal", 1) {}

        bool IsActive() override
        {
            Unit* boss = FindAliveCreature(AQ20::NPC_OSSIRIAN);
            if (!boss)
                return false;

            return ai->HasAura(AQ20::SPELL_OSSIRIAN_STRENGTH, boss);
        }
    };

    class OssirianCrystalCloseTrigger : public Trigger
    {
    public:
        OssirianCrystalCloseTrigger(PlayerbotAI* ai)
            : Trigger(ai, "ossirian crystal close", 1) {}

        bool IsActive() override
        {
            Unit* boss = DungeonTargetHelper::FindAliveCreature(ai, AQ20::NPC_OSSIRIAN);
            if (!boss)
                return false;

            std::list<ObjectGuid> guids =
                AI_VALUE2(std::list<ObjectGuid>, "nearest game objects no los", AQ20::GO_OSSIRIAN_CRYSTAL);

            for (ObjectGuid const& guid : guids)
{
                GameObject* go = ai->GetGameObject(guid);
                if (!go)
                    continue;

                if (!sServerFacade.isSpawned(go))
                    continue;

                if (bot->IsWithinDistInMap(go, INTERACTION_DISTANCE, false) && bot->CanInteract(go))
                    return true;
            }

            return false;
        }
    };

    // ------------------------------------------------------------
    // AQ20 trigger context
    // ------------------------------------------------------------

    class AhnQirajRuinsTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        AhnQirajRuinsTriggerContext()
        {
            creators["enter aq20"] = [](PlayerbotAI* ai){return new EnterDungeonTrigger(ai, "enter aq20", "ahnqiraj ruins", AQ20::MAP_ID);};
            creators["leave aq20"] = [](PlayerbotAI* ai){return new LeaveDungeonTrigger(ai, "leave aq20", "ahnqiraj ruins", AQ20::MAP_ID);};
            creators["start kurinnaxx fight"] = [](PlayerbotAI* ai){return new StartBossFightTrigger(ai, "start kurinnaxx fight", "kurinnaxx", AQ20::NPC_KURINNAXX);};
            creators["end kurinnaxx fight"] = [](PlayerbotAI* ai){return new EndBossFightTrigger(ai, "end kurinnaxx fight", "kurinnaxx", AQ20::NPC_KURINNAXX);};
            creators["start buru fight"] = [](PlayerbotAI* ai){return new StartBossFightTrigger(ai, "start buru fight", "buru", AQ20::NPC_BURU);};
            creators["end buru fight"] = [](PlayerbotAI* ai){return new EndBossFightTrigger(ai, "end buru fight", "buru", AQ20::NPC_BURU);};
            creators["start ayamiss fight"] = [](PlayerbotAI* ai){return new StartBossFightTrigger(ai, "start ayamiss fight", "ayamiss", AQ20::NPC_AYAMISS);};
            creators["end ayamiss fight"] = [](PlayerbotAI* ai){return new EndBossFightTrigger(ai, "end ayamiss fight", "ayamiss", AQ20::NPC_AYAMISS);};
            creators["start ossirian fight"] = [](PlayerbotAI* ai){return new StartBossFightTrigger(ai, "start ossirian fight", "ossirian", AQ20::NPC_OSSIRIAN);};
            creators["end ossirian fight"] = [](PlayerbotAI* ai){return new EndBossFightTrigger(ai, "end ossirian fight", "ossirian", AQ20::NPC_OSSIRIAN);};
            creators["kurinnaxx sand trap close"] = &AhnQirajRuinsTriggerContext::kurinnaxx_sand_trap_close;
            creators["kurinnaxx mortal wound high"] = &AhnQirajRuinsTriggerContext::kurinnaxx_mortal_wound_high;

            creators["buru focused me"] = &AhnQirajRuinsTriggerContext::buru_focused_me;
            creators["buru egg available"] = &AhnQirajRuinsTriggerContext::buru_egg_available;
            creators["buru shell broken"] = &AhnQirajRuinsTriggerContext::buru_shell_broken;

            creators["ayamiss larva alive"] = &AhnQirajRuinsTriggerContext::ayamiss_larva_alive;
            creators["ayamiss boss available"] = &AhnQirajRuinsTriggerContext::ayamiss_boss_available;
            creators["ayamiss poison stinger high"] = &AhnQirajRuinsTriggerContext::ayamiss_poison_stinger_high;

            creators["ossirian needs crystal"] = &AhnQirajRuinsTriggerContext::ossirian_needs_crystal;
            creators["ossirian crystal close"] = &AhnQirajRuinsTriggerContext::ossirian_crystal_close;
        }

    private:
        static Trigger* kurinnaxx_sand_trap_close(PlayerbotAI* ai) { return new KurinaxxSandTrapTrigger(ai); }
        static Trigger* kurinnaxx_mortal_wound_high(PlayerbotAI* ai) { return new KurinnaxxMortalWoundHighTrigger(ai); }

        static Trigger* buru_focused_me(PlayerbotAI* ai) { return new BuruFocusedMeTrigger(ai); }
        static Trigger* buru_egg_available(PlayerbotAI* ai) { return new BuruEggAvailableTrigger(ai); }
        static Trigger* buru_shell_broken(PlayerbotAI* ai) { return new BuruShellBrokenTrigger(ai); }

        static Trigger* ayamiss_larva_alive(PlayerbotAI* ai) { return new AyamissLarvaAliveTrigger(ai); }
        static Trigger* ayamiss_boss_available(PlayerbotAI* ai) { return new AyamissBossAvailableTrigger(ai); }
        static Trigger* ayamiss_poison_stinger_high(PlayerbotAI* ai) { return new AyamissPoisonStingerHighTrigger(ai); }

        static Trigger* ossirian_needs_crystal(PlayerbotAI* ai) { return new OssirianNeedsCrystalTrigger(ai); }
        static Trigger* ossirian_crystal_close(PlayerbotAI* ai) { return new OssirianCrystalCloseTrigger(ai); }
    };
}
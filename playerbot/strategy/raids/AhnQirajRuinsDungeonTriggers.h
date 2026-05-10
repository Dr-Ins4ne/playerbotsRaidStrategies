#pragma once

#include "../triggers/DungeonTriggers.h"
#include "../triggers/GenericTriggers.h"
#include "AhnQirajRuinsDungeonActions.h"

namespace ai
{
    // ------------------------------------------------------------
    // Kurinnaxx
    // ------------------------------------------------------------

    class KurinnaxxSandTrapTrigger : public CloseToGameObjectHazardTrigger
    {
    public:
        KurinnaxxSandTrapTrigger(PlayerbotAI* ai)
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
            if (!boss || !boss->IsInWorld() || boss->GetMapId() != bot->GetMapId())
                return false;

            Unit* victim = nullptr;
    #ifdef CMANGOS
            victim = boss->GetVictim();
    #else
            victim = boss->getVictim();
    #endif

            if (!victim || !victim->IsInWorld() || victim->GetMapId() != bot->GetMapId())
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

    class BuruAddAliveTrigger : public DungeonCreatureTrigger
    {
    public:
        BuruAddAliveTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "buru add alive", 1) {}

        bool IsActive() override
        {
            return IsAlive(AQ20::NPC_HIVEZARA_HATCHLING);
        }
    };

    class BuruEggAvailableTrigger : public DungeonCreatureTrigger
    {
    public:
        BuruEggAvailableTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "buru egg available", 1) {}

        bool IsActive() override
        {
            Unit* boss = FindAliveCreature(AQ20::NPC_BURU);
            if (!boss)
                return false;

            if (GetHealthPct(boss) <= 20.0f)
                return false;

            if (IsAlive(AQ20::NPC_HIVEZARA_HATCHLING))
                return false;

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
            if (!boss)
                return false;

            if (IsAlive(AQ20::NPC_HIVEZARA_HATCHLING))
                return false;

            return GetHealthPct(boss) <= 20.0f;
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
            // Only non-healer melee bots should react to larvae.
            // This prevents ranged DPS and warlocks from running attack rti target on larva skull.
            if (ai->IsHeal(bot))
                return false;

            if (ai->IsRanged(bot))
                return false;

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
            if (ai->IsHeal(bot))
                return false;

            // Warlocks use the dedicated warlock-tank trigger/action.
            if (bot->getClass() == CLASS_WARLOCK)
                return false;

            Unit* boss = FindAliveCreature(AQ20::NPC_AYAMISS);
            if (!boss || !boss->IsInWorld() || boss->GetMapId() != bot->GetMapId())
                return false;

            // Melee should stay on larvae while larvae exist.
            if (!ai->IsRanged(bot) && IsAlive(AQ20::NPC_HIVEZARA_LARVA))
                return false;

            // In phase 1 Ayamiss is flying; melee cannot usefully hit her.
            if (!ai->IsRanged(bot) && GetHealthPct(boss) > 70.0f)
                return false;

            return true;
        }
    };

    class AyamissWarlockTankNeededTrigger : public DungeonCreatureTrigger
    {
    public:
        AyamissWarlockTankNeededTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "ayamiss warlock tank needed", 1) {}

        bool IsActive() override
        {
            if (bot->getClass() != CLASS_WARLOCK)
                return false;

            Unit* boss = FindAliveCreature(AQ20::NPC_AYAMISS);
            if (!boss || !boss->IsInWorld() || boss->GetMapId() != bot->GetMapId())
                return false;

            Aura* aura = ai->GetAura(AQ20::SPELL_AYAMISS_POISON_STINGER, bot);
            if (aura && aura->GetStackAmount() >= 18)
                return false;

            return true;
        }
    };

    class AyamissPoisonStingerHighTrigger : public HasAuraTrigger
    {
    public:
        AyamissPoisonStingerHighTrigger(PlayerbotAI* ai)
            : HasAuraTrigger(ai, "ayamiss poison stinger high", 1) {}

        bool IsActive() override
        {
            if (bot->getClass() != CLASS_WARLOCK)
                return false;

            Aura* aura = ai->GetAura(AQ20::SPELL_AYAMISS_POISON_STINGER, bot);
            return aura && aura->GetStackAmount() >= 18;
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
            creators["enter aq20"] = [](PlayerbotAI* ai) { return new EnterDungeonTrigger(ai, "enter aq20", "ahnqiraj ruins", AQ20::MAP_ID); };
            creators["leave aq20"] = [](PlayerbotAI* ai) { return new LeaveDungeonTrigger(ai, "leave aq20", "ahnqiraj ruins", AQ20::MAP_ID); };

            creators["start kurinnaxx fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start kurinnaxx fight", "kurinnaxx", AQ20::NPC_KURINNAXX); };
            creators["end kurinnaxx fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end kurinnaxx fight", "kurinnaxx", AQ20::NPC_KURINNAXX); };

            creators["start buru fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start buru fight", "buru", AQ20::NPC_BURU); };
            creators["end buru fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end buru fight", "buru", AQ20::NPC_BURU); };

            creators["start ayamiss fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start ayamiss fight", "ayamiss", AQ20::NPC_AYAMISS); };
            creators["end ayamiss fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end ayamiss fight", "ayamiss", AQ20::NPC_AYAMISS); };

            creators["start ossirian fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start ossirian fight", "ossirian", AQ20::NPC_OSSIRIAN); };
            creators["end ossirian fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end ossirian fight", "ossirian", AQ20::NPC_OSSIRIAN); };

            creators["kurinnaxx sand trap close"] = [](PlayerbotAI* ai) { return new KurinnaxxSandTrapTrigger(ai); };
            creators["kurinnaxx mortal wound high"] = [](PlayerbotAI* ai) { return new KurinnaxxMortalWoundHighTrigger(ai); };

            creators["buru focused me"] = [](PlayerbotAI* ai) { return new BuruFocusedMeTrigger(ai); };
            creators["buru add alive"] = [](PlayerbotAI* ai) { return new BuruAddAliveTrigger(ai); };
            creators["buru egg available"] = [](PlayerbotAI* ai) { return new BuruEggAvailableTrigger(ai); };
            creators["buru shell broken"] = [](PlayerbotAI* ai) { return new BuruShellBrokenTrigger(ai); };

            creators["ayamiss larva alive"] = [](PlayerbotAI* ai) { return new AyamissLarvaAliveTrigger(ai); };
            creators["ayamiss boss available"] = [](PlayerbotAI* ai) { return new AyamissBossAvailableTrigger(ai); };
            creators["ayamiss warlock tank needed"] = [](PlayerbotAI* ai) { return new AyamissWarlockTankNeededTrigger(ai); };
            creators["ayamiss poison stinger high"] = [](PlayerbotAI* ai) { return new AyamissPoisonStingerHighTrigger(ai); };

            creators["ossirian needs crystal"] = [](PlayerbotAI* ai) { return new OssirianNeedsCrystalTrigger(ai); };
            creators["ossirian crystal close"] = [](PlayerbotAI* ai) { return new OssirianCrystalCloseTrigger(ai); };
        }
    };
}
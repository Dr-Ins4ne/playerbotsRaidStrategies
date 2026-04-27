
#pragma once
#include "DungeonTriggers.h"
#include "GenericTriggers.h"
#include "HealthTriggers.h"

namespace ai
{

    // Trigger for Edge of Madness Brazier (Requires Gurubashi Mojo Madness: 19931)
    class ZGBrazierInSightTrigger : public ValueTrigger
    {
    public:
        ZGBrazierInSightTrigger(PlayerbotAI* ai) : ValueTrigger(ai, "zg brazier in sight", 1)
        {
            qualifier = "and::{"
                "action possible::use id::19931,"
                "has object::go usable filter::go trapped filter::entry filter::{gos in sight,zg braziers},"
                "not::has object::entry filter::{gos close,zg braziers}"
                "}";
        }
    };

    class ZGBrazierCloseTrigger : public ValueTrigger
    {
    public:
        ZGBrazierCloseTrigger(PlayerbotAI* ai) : ValueTrigger(ai, "zg brazier close", 1) { qualifier = "has object::go usable filter::entry filter::{gos close,zg braziers}"; }
    };

    // Trigger for Gahz'ranka Water Node (Requires Mudskunk Lure: 19974)
    class ZGWaterNodeInSightTrigger : public ValueTrigger
    {
    public:
        ZGWaterNodeInSightTrigger(PlayerbotAI* ai) : ValueTrigger(ai, "zg water node in sight", 1)
        {
            qualifier = "and::{"
                "action possible::use id::19974,"
                "has object::go usable filter::go trapped filter::entry filter::{gos in sight,zg water nodes},"
                "not::has object::entry filter::{gos close,zg water nodes}"
                "}";
        }
    };

    class ZGWaterNodeCloseTrigger : public ValueTrigger
    {
    public:
        ZGWaterNodeCloseTrigger(PlayerbotAI* ai) : ValueTrigger(ai, "zg water node close", 1) { qualifier = "has object::go usable filter::entry filter::{gos close,zg water nodes}"; }
    };




    class ThekalTriggerBase : public Trigger
    {
    public:
        ThekalTriggerBase(PlayerbotAI* ai, std::string name, int checkInterval = 1)
            : Trigger(ai, name, checkInterval) {}

    protected:
        static const uint32 NPC_THEKAL  = 14509;
        static const uint32 NPC_LORKHAN = 11347;
        static const uint32 NPC_ZATH    = 11348;
        static const uint32 NPC_TIGER   = 11361;

        static constexpr float HOLD_HP_PCT   = 12.0f;
        static constexpr float FINISH_HP_PCT = 12.0f;

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            std::list<ObjectGuid> targets = AI_VALUE(std::list<ObjectGuid>, "possible attack targets");

            for (ObjectGuid const& guid : targets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!unit)
                    continue;

                if (!unit->IsAlive())
                    continue;

                if (unit->GetEntry() == entry)
                    return unit;
            }

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget && currentTarget->IsAlive() && currentTarget->GetEntry() == entry)
                return currentTarget;

            return nullptr;
        }

        float GetHealthPct(Unit* unit)
        {
            if (!unit || !unit->GetMaxHealth())
                return 0.0f;

            return 100.0f * float(unit->GetHealth()) / float(unit->GetMaxHealth());
        }

        bool IsAlive(uint32 entry)
        {
            return FindAliveCreature(entry) != nullptr;
        }

        int AliveTrioCount()
        {
            int count = 0;

            if (IsAlive(NPC_THEKAL))
                ++count;

            if (IsAlive(NPC_LORKHAN))
                ++count;

            if (IsAlive(NPC_ZATH))
                ++count;

            return count;
        }

        bool IsTrioReadyToFinish()
        {
            Unit* thekal  = FindAliveCreature(NPC_THEKAL);
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            Unit* zath    = FindAliveCreature(NPC_ZATH);

            if (!thekal || !lorkhan || !zath)
                return false;

            return GetHealthPct(thekal)  <= FINISH_HP_PCT &&
                   GetHealthPct(lorkhan) <= FINISH_HP_PCT &&
                   GetHealthPct(zath)    <= FINISH_HP_PCT;
        }
    };


    class ThekalTigerAliveTrigger : public ThekalTriggerBase
    {
    public:
        ThekalTigerAliveTrigger(PlayerbotAI* ai)
            : ThekalTriggerBase(ai, "thekal tiger alive", 1) {}

        bool IsActive() override
        {
            return IsAlive(NPC_TIGER);
        }
    };


    class LorkhanCastingTrigger : public ThekalTriggerBase
    {
    public:
        LorkhanCastingTrigger(PlayerbotAI* ai)
            : ThekalTriggerBase(ai, "lorkhan casting", 1) {}

        bool IsActive() override
        {
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            return lorkhan && lorkhan->IsNonMeleeSpellCasted(true);
        }
    };


    class LorkhanNeedsCurseOfTonguesTrigger : public ThekalTriggerBase
    {
    public:
        LorkhanNeedsCurseOfTonguesTrigger(PlayerbotAI* ai)
            : ThekalTriggerBase(ai, "lorkhan needs curse of tongues", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();

            if (bot->getClass() != CLASS_WARLOCK)
                return false;

            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            if (!lorkhan)
                return false;

            return !ai->HasAura("curse of tongues", lorkhan);
        }
    };


    class ThekalTrioReadyToFinishTrigger : public ThekalTriggerBase
    {
    public:
        ThekalTrioReadyToFinishTrigger(PlayerbotAI* ai)
            : ThekalTriggerBase(ai, "thekal trio ready to finish", 1) {}

        bool IsActive() override
        {
            return IsTrioReadyToFinish();
        }
    };


    class ThekalTrioNeedsBalanceTrigger : public ThekalTriggerBase
    {
    public:
        ThekalTrioNeedsBalanceTrigger(PlayerbotAI* ai)
            : ThekalTriggerBase(ai, "thekal trio needs balance", 1) {}

        bool IsActive() override
        {
            if (IsAlive(NPC_TIGER))
                return false;

            if (IsTrioReadyToFinish())
                return false;

            return AliveTrioCount() == 3;
        }
    };


    class ThekalTrioBrokenTrigger : public ThekalTriggerBase
    {
    public:
        ThekalTrioBrokenTrigger(PlayerbotAI* ai)
            : ThekalTriggerBase(ai, "thekal trio broken", 1) {}

        bool IsActive() override
        {
            if (IsAlive(NPC_TIGER))
                return false;

            int alive = AliveTrioCount();

            // One or two remaining means something died early or phase transition started.
            // Burn remaining targets quickly.
            return alive > 0 && alive < 3;
        }
    };


    class ZulGurubTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        ZulGurubTriggerContext()
        {
            // Map 309 = Zul'Gurub
            creators["enter zul gurub"] = [](PlayerbotAI* ai){ return new EnterDungeonTrigger(ai, "enter zul gurub", "zul gurub", 309);};
            creators["leave zul gurub"] = [](PlayerbotAI* ai){ return new LeaveDungeonTrigger(ai, "leave zul gurub", "zul gurub", 309);};

            // Boss fight triggers with exact CMaNGOS NPC IDs
            creators["start jeklik fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start jeklik fight", "jeklik", 14517);};
            creators["end jeklik fight"] = [](PlayerbotAI* ai) {return new EndBossFightTrigger(ai, "end jeklik fight", "jeklik", 14517);};
            
            creators["start venoxis fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start venoxis fight", "venoxis", 14507);};
            creators["end venoxis fight"] = [](PlayerbotAI* ai) {return new EndBossFightTrigger(ai, "end venoxis fight", "venoxis", 14507);};
            
            creators["start marli fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start marli fight", "marli", 14510);};
            creators["end marli fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end marli fight", "marli", 14510);};
            
            creators["start mandokir fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start mandokir fight", "mandokir", 11382);};
            creators["end mandokir fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end mandokir fight", "mandokir", 11382);};
            
            creators["start gahzranka fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start gahzranka fight", "gahzranka", 15114);};
            creators["end gahzranka fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end gahzranka fight", "gahzranka", 15114);};
            
            creators["start thekal fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start thekal fight", "thekal", 14509);};
            creators["end thekal fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end thekal fight", "thekal", 14509);};
            

            // High Priest Thekal encounter
            creators["thekal tiger alive"] = [](PlayerbotAI* ai){return new ThekalTigerAliveTrigger(ai);};
            creators["lorkhan casting"] = [](PlayerbotAI* ai){return new LorkhanCastingTrigger(ai);};
            creators["lorkhan needs curse of tongues"] = [](PlayerbotAI* ai){return new LorkhanNeedsCurseOfTonguesTrigger(ai);};
            creators["thekal trio ready to finish"] = [](PlayerbotAI* ai){return new ThekalTrioReadyToFinishTrigger(ai);};
            creators["thekal trio needs balance"] = [](PlayerbotAI* ai){return new ThekalTrioNeedsBalanceTrigger(ai);};
            creators["thekal trio broken"] = [](PlayerbotAI* ai){return new ThekalTrioBrokenTrigger(ai);};
            
            creators["start arlokk fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start arlokk fight", "arlokk", 14515);};
            creators["end arlokk fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end arlokk fight", "arlokk", 14515);};
            
            creators["start jindo fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start jindo fight", "jindo", 11380);};
            creators["end jindo fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end jindo fight", "jindo", 11380);};
            
            creators["start hakkar fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start hakkar fight", "hakkar", 14834);};
            creators["end hakkar fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end hakkar fight", "hakkar", 14834);};

            // Hazards and Proximity triggers
            creators["venoxis poison cloud"] = [](PlayerbotAI* ai) {return new CloseToGameObjectHazardTrigger(ai, "venoxis poison cloud", 179905, 5.0f, 60);};
            creators["venoxis too close"] = [](PlayerbotAI* ai) {return new  CloseToCreatureTrigger(ai, "venoxis too close", 14507, 30.0f);};
            
            // Replaced Fire Prot with Major Nature Protection Potion (Item 13458, Aura 17546)
            creators["nature protection potion ready"] = [](PlayerbotAI* ai){ return new ItemBuffReadyTrigger(ai, "nature protection potion ready", 13458, 17546);};

            // Interactable context bindings
            creators["zg brazier in sight"] = &ZulGurubTriggerContext::zg_brazier_in_sight;
            creators["zg brazier close"] = &ZulGurubTriggerContext::zg_brazier_close;
            creators["zg water node in sight"] = &ZulGurubTriggerContext::zg_water_node_in_sight;
            creators["zg water node close"] = &ZulGurubTriggerContext::zg_water_node_close;
        }
        private:
        static Trigger* zg_brazier_in_sight(PlayerbotAI* ai) { return new ZGBrazierInSightTrigger(ai); };
        static Trigger* zg_brazier_close(PlayerbotAI* ai) { return new ZGBrazierCloseTrigger(ai); };
        static Trigger* zg_water_node_in_sight(PlayerbotAI* ai) { return new ZGWaterNodeInSightTrigger(ai); };
        static Trigger* zg_water_node_close(PlayerbotAI* ai) { return new ZGWaterNodeCloseTrigger(ai); };
    };

};
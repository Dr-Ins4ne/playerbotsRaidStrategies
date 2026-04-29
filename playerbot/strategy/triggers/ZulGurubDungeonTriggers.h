
#pragma once
#include "DungeonTriggers.h"
#include "GenericTriggers.h"
#include "HealthTriggers.h"
#include "playerbot/strategy/values/RtiTargetValue.h"
#include "Groups/Group.h"
#include "playerbot/strategy/actions/RaidIconActionBase.h"
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

    class HakkarTriggerBase : public DungeonCreatureTrigger
    {
    public:
        HakkarTriggerBase(PlayerbotAI* ai, std::string name, int checkInterval = 1)
            : DungeonCreatureTrigger(ai, name, checkInterval) {}

    protected:
        static const uint32 NPC_HAKKAR = 14834;

        // TODO: replace with your desired fixed Hakkar tank position.
        static constexpr float HAKKAR_TANK_X = -11875.0f;
        static constexpr float HAKKAR_TANK_Y = -1660.0f;
        static constexpr float HAKKAR_TANK_Z = 43.0f;

        static constexpr float HAKKAR_TANK_POSITION_RADIUS = 4.0f;

    protected:
        Unit* FindHakkar()
        {
            return FindAliveCreature(NPC_HAKKAR);
        }

        bool IsHakkarTargetingMe()
        {
            Unit* hakkar = FindHakkar();
            if (!hakkar)
                return false;

            Unit* victim = hakkar->GetVictim();
            return victim && victim->GetObjectGuid() == bot->GetObjectGuid();
        }

        bool IsAtHakkarTankPosition()
        {
            return bot->GetDistance(
                HAKKAR_TANK_X,
                HAKKAR_TANK_Y,
                HAKKAR_TANK_Z
            ) <= HAKKAR_TANK_POSITION_RADIUS;
        }

        bool IsValidMindControlTarget(Unit* unit)
        {
            if (!unit || !unit->IsAlive())
                return false;

            if (unit == bot)
                return false;

            if (unit->GetTypeId() != TYPEID_PLAYER)
                return false;

            if (!bot->IsInGroup(unit))
                return false;

            // Hakkar MC/charm state.
            if (!unit->HasAuraType(SPELL_AURA_MOD_CHARM))
                return false;

            // Do not spam CC if one of our intended controls is already present.
            if (ai->HasAura("polymorph", unit))
                return false;

            if (ai->HasAura("fear", unit))
                return false;

            if (ai->HasAura("entangling roots", unit))
                return false;

            return true;
        }

        Unit* FindMindControlledTargetInList(std::list<ObjectGuid> const& guids)
        {
            for (ObjectGuid const& guid : guids)
            {
                Unit* unit = ai->GetUnit(guid);
                if (IsValidMindControlTarget(unit))
                    return unit;
            }

            return nullptr;
        }

        Unit* FindHakkarMindControlledTarget()
        {
            Unit* target = FindMindControlledTargetInList(AI_VALUE(std::list<ObjectGuid>, "possible attack targets"));
            if (target)
                return target;

            target = FindMindControlledTargetInList(AI_VALUE(std::list<ObjectGuid>, "attackers"));
            if (target)
                return target;

            target = FindMindControlledTargetInList(AI_VALUE(std::list<ObjectGuid>, "possible targets"));
            if (target)
                return target;

            return nullptr;
        }
    };


    class HakkarAggroHolderOutOfPositionTrigger : public HakkarTriggerBase
    {
    public:
        HakkarAggroHolderOutOfPositionTrigger(PlayerbotAI* ai)
            : HakkarTriggerBase(ai, "hakkar aggro holder out of position", 1) {}

        bool IsActive() override
        {
            if (!bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            if (!ai->CanMove())
                return false;

            if (!IsHakkarTargetingMe())
                return false;

            return !IsAtHakkarTankPosition();
        }
    };


    class HakkarMindControlTargetNeedsCcTrigger : public HakkarTriggerBase
    {
    public:
        HakkarMindControlTargetNeedsCcTrigger(PlayerbotAI* ai)
            : HakkarTriggerBase(ai, "hakkar mind control target needs cc", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            switch (bot->getClass())
            {
                case CLASS_MAGE:
                case CLASS_WARLOCK:
                case CLASS_DRUID:
                    break;
                default:
                    return false;
            }

            return FindHakkarMindControlledTarget() != nullptr;
        }
    };

    class JeklikCastingTrigger : public DungeonCreatureTrigger
    {
    public:
        JeklikCastingTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "jeklik casting", 1) {}

        bool IsActive() override
        {
            return IsInterruptableCasting(NPC_JEKLIK);
        }

    private:
        static const uint32 NPC_JEKLIK = 14517;
    };

    class JeklikNeedsCurseOfTonguesTrigger : public DungeonCreatureTrigger
    {
    public:
        JeklikNeedsCurseOfTonguesTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "jeklik needs curse of tongues", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();
            if (!bot || bot->getClass() != CLASS_WARLOCK)
                return false;

            Unit* jeklik = FindAliveCreature(NPC_JEKLIK);
            if (!jeklik)
                return false;

            return !ai->HasAura("curse of tongues", jeklik);
        }

    private:
        static const uint32 NPC_JEKLIK = 14517;
    };


    class JeklikShouldDrainManaTrigger : public DungeonCreatureTrigger
    {
    public:
        JeklikShouldDrainManaTrigger(PlayerbotAI* ai)
            : DungeonCreatureTrigger(ai, "jeklik should drain mana", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();
            if (!bot || bot->getClass() != CLASS_WARLOCK)
                return false;

            Unit* jeklik = FindAliveCreature(NPC_JEKLIK);
            if (!jeklik)
                return false;

            // Only use Drain Mana if Jeklik actually has mana.
            if (!AI_VALUE2(bool, "has mana", "current target"))
                return false;

            // Prevent repeatedly interrupting our own Drain Mana channel.
            if (bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                return false;

            return true;
        }

    private:
        static const uint32 NPC_JEKLIK = 14517;
    };

    class ThekalTriggerBase : public DungeonCreatureTrigger
    {
    public:
        ThekalTriggerBase(PlayerbotAI* ai, std::string name, int checkInterval = 1)
            : DungeonCreatureTrigger(ai, name, checkInterval) {}

    protected:
        static const uint32 NPC_THEKAL  = 14509;
        static const uint32 NPC_LORKHAN = 11347;
        static const uint32 NPC_ZATH    = 11348;
        static const uint32 NPC_TIGER   = 11361;


        static constexpr float FINISH_HP_PCT = 7.0f;

    protected:

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

    class ThekalTargetsNeedMarkingTrigger : public ThekalTriggerBase
    {
    public:
        ThekalTargetsNeedMarkingTrigger(PlayerbotAI* ai)
            : ThekalTriggerBase(ai, "thekal targets need marking", 1) {}

        bool IsActive() override
        {
            Group* group = bot->GetGroup();
            if (!group)
                return false;

            if (bot->InBattleGround())
                return false;

            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            Unit* zath    = FindAliveCreature(NPC_ZATH);
            Unit* thekal  = FindAliveCreature(NPC_THEKAL);
            Unit* tiger   = FindAliveCreature(NPC_TIGER);

            if (!lorkhan && !zath && !thekal && !tiger)
                return false;

            if (lorkhan && !HasCorrectIcon(group, "skull", lorkhan))
                return true;

            if (zath && !HasCorrectIcon(group, "cross", zath))
                return true;

            if (thekal && !HasCorrectIcon(group, "square", thekal))
                return true;

            if (tiger && !HasCorrectIcon(group, "triangle", tiger))
                return true;

            return false;
        }

    private:
        bool HasCorrectIcon(Group* group, std::string icon, Unit* unit)
        {
            if (!group || !unit)
                return false;

            int index = RtiTargetValue::GetRtiIndex(icon);
            if (index < 0)
                return false;

            return group->GetTargetIcon(index) == unit->GetObjectGuid();
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
            return IsInterruptableCasting(NPC_LORKHAN);
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
            creators["jeklik casting"] = [](PlayerbotAI* ai){ return new JeklikCastingTrigger(ai);};
            creators["jeklik needs curse of tongues"] = [](PlayerbotAI* ai){return new JeklikNeedsCurseOfTonguesTrigger(ai);};
            creators["jeklik should drain mana"] = [](PlayerbotAI* ai){return new JeklikShouldDrainManaTrigger(ai);};


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
            creators["thekal targets need marking"] = [](PlayerbotAI* ai){return new ThekalTargetsNeedMarkingTrigger(ai);};
            
            creators["start arlokk fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start arlokk fight", "arlokk", 14515);};
            creators["end arlokk fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end arlokk fight", "arlokk", 14515);};
            
            creators["start jindo fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start jindo fight", "jindo", 11380);};
            creators["end jindo fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end jindo fight", "jindo", 11380);};
            
            creators["start hakkar fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start hakkar fight", "hakkar", 14834);};
            creators["end hakkar fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end hakkar fight", "hakkar", 14834);};
            creators["hakkar aggro holder out of position"] = [](PlayerbotAI* ai){return new HakkarAggroHolderOutOfPositionTrigger(ai);};
            creators["hakkar mind control target needs cc"] = [](PlayerbotAI* ai){return new HakkarMindControlTargetNeedsCcTrigger(ai);};
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
#pragma once
#include "../triggers/DungeonTriggers.h"
#include "../triggers/GenericTriggers.h"

namespace ai
{

    class MCRuneInSightTrigger : public ValueTrigger
    {
    public:
        MCRuneInSightTrigger(PlayerbotAI* ai) : ValueTrigger(ai, "mc rune in sight", 1)
        {
            qualifier = "and::{"
                "action possible::use id::17333,"
                "has object::go usable filter::go trapped filter::entry filter::{gos in sight,mc runes},"
                "not::has object::entry filter::{gos close,mc runes}"
                "}";
        }
    };

    class MCRuneCloseTrigger : public ValueTrigger
    {
    public:
        MCRuneCloseTrigger(PlayerbotAI* ai) : ValueTrigger(ai, "mc rune close", 1) { qualifier = "has object::go usable filter::entry filter::{gos close,mc runes}"; }
    };

    class GolemaggTriggerBase : public DungeonCreatureTrigger
    {
    public:
        GolemaggTriggerBase(PlayerbotAI* ai, std::string name, int checkInterval = 1)
            : DungeonCreatureTrigger(ai, name, checkInterval) {}

    protected:
        static const uint32 NPC_GOLEMAGG   = 11988;
        static const uint32 NPC_CORE_RAGER = 11672;

    protected:
        std::vector<Unit*> FindAliveCoreRagers()
        {
            std::vector<Unit*> ragers;

            AddAliveCoreRagersFromGuidList("possible attack targets", ragers);
            AddAliveCoreRagersFromGuidList("attackers", ragers);
            AddAliveCoreRagersFromGuidList("possible targets", ragers);

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (IsValidCoreRager(currentTarget) && !ContainsUnit(ragers, currentTarget))
                ragers.push_back(currentTarget);

            return ragers;
        }

        void AddAliveCoreRagersFromGuidList(std::string const& valueName, std::vector<Unit*>& ragers)
        {
            std::list<ObjectGuid> targets = AI_VALUE(std::list<ObjectGuid>, valueName);

            for (ObjectGuid const& guid : targets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!IsValidCoreRager(unit))
                    continue;

                if (ContainsUnit(ragers, unit))
                    continue;

                ragers.push_back(unit);
            }
        }

        bool IsValidCoreRager(Unit* unit)
        {
            return unit &&
                unit->IsAlive() &&
                unit->GetEntry() == NPC_CORE_RAGER;
        }

        bool ContainsUnit(std::vector<Unit*> const& units, Unit* unit)
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

        bool HasCorrectIcon(Group* group, std::string icon, Unit* unit)
        {
            if (!group || !unit)
                return false;

            int index = RtiTargetValue::GetRtiIndex(icon);
            if (index < 0)
                return false;

            return group->GetTargetIcon(index) == unit->GetObjectGuid();
        }

        bool CurrentTargetIsCoreRager()
        {
            Unit* target = AI_VALUE(Unit*, "current target");
            return IsValidCoreRager(target);
        }

        bool HasAggroOnCurrentTarget()
        {
            return AI_VALUE2(bool, "has aggro", "current target");
        }

        bool IsTooCloseToGolemagg(float maxDistance)
        {
            Unit* golemagg = FindAliveCreature(NPC_GOLEMAGG);
            if (!golemagg)
                return false;

            return sServerFacade.GetDistance2d(bot, golemagg) < maxDistance;
        }
    };

    class GolemaggAliveTrigger : public GolemaggTriggerBase
    {
    public:
        GolemaggAliveTrigger(PlayerbotAI* ai)
            : GolemaggTriggerBase(ai, "golemagg alive", 1) {}

        bool IsActive() override
        {
            return IsAlive(NPC_GOLEMAGG);
        }
    };

    class GolemaggTargetsNeedMarkingTrigger : public GolemaggTriggerBase
    {
    public:
        GolemaggTargetsNeedMarkingTrigger(PlayerbotAI* ai)
            : GolemaggTriggerBase(ai, "golemagg targets need marking", 1) {}

        bool IsActive() override
        {
            Group* group = bot->GetGroup();
            if (!group)
                return false;

            if (bot->InBattleGround())
                return false;

            Unit* golemagg = FindAliveCreature(NPC_GOLEMAGG);
            if (!golemagg)
                return false;

            if (!HasCorrectIcon(group, "skull", golemagg))
                return true;

            std::vector<Unit*> ragers = FindAliveCoreRagers();

            if (ragers.size() > 0 && !HasCorrectIcon(group, "cross", ragers[0]))
                return true;

            if (ragers.size() > 1 && !HasCorrectIcon(group, "square", ragers[1]))
                return true;

            return false;
        }
    };

    class GolemaggRagerTankTooCloseTrigger : public GolemaggTriggerBase
    {
    public:
        GolemaggRagerTankTooCloseTrigger(PlayerbotAI* ai)
            : GolemaggTriggerBase(ai, "golemagg rager tank too close", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (!bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            if (!ai->CanMove())
                return false;

            if (!ai->IsTank(bot))
                return false;

            if (!IsAlive(NPC_GOLEMAGG))
                return false;

            // Only the tank currently holding a Core Rager should move.
            // We do not globally move every tank.
            if (!CurrentTargetIsCoreRager())
                return false;

            if (!HasAggroOnCurrentTarget())
                return false;

            // If the rager tank is close to Golemagg, drag the rager away.
            return IsTooCloseToGolemagg(25.0f);
        }
    };

    class GolemaggAggroHolderOutOfPositionTrigger : public GolemaggTriggerBase
    {
    public:
        GolemaggAggroHolderOutOfPositionTrigger(PlayerbotAI* ai)
            : GolemaggTriggerBase(ai, "golemagg aggro holder out of position", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (!bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            if (!ai->CanMove())
                return false;

            if (!ai->IsTank(bot))
                return false;

            Unit* golemagg = FindAliveCreature(NPC_GOLEMAGG);
            if (!golemagg)
                return false;

            Unit* victim = golemagg->GetVictim();
            if (!victim || victim->GetObjectGuid() != bot->GetObjectGuid())
                return false;

            return !IsAtGolemaggTankPosition();
        }

    private:
        bool IsAtGolemaggTankPosition()
        {
            return bot->GetDistance(
                GOLEMAGG_TANK_X,
                GOLEMAGG_TANK_Y,
                GOLEMAGG_TANK_Z
            ) <= GOLEMAGG_TANK_POSITION_RADIUS;
        }

    private:
        static constexpr float GOLEMAGG_TANK_X = 780.6f;
        static constexpr float GOLEMAGG_TANK_Y = -1001.8f;
        static constexpr float GOLEMAGG_TANK_Z = -206.0f;

        // Reference only for now.
        static constexpr float GOLEMAGG_TANK_O = 1.0f;

        static constexpr float GOLEMAGG_TANK_POSITION_RADIUS = 5.0f;
    };

    class MajordomoTriggerBase : public DungeonCreatureTrigger
    {
    public:
        MajordomoTriggerBase(PlayerbotAI* ai, std::string name, int checkInterval = 1)
            : DungeonCreatureTrigger(ai, name, checkInterval) {}

    protected:
        static const uint32 NPC_MAJORDOMO_EXECUTUS = 12018;
        static const uint32 NPC_FLAMEWAKER_HEALER  = 11663;
        static const uint32 NPC_FLAMEWAKER_ELITE   = 11664;

    protected:
        bool IsMajordomoEncounterActive()
        {
            return IsAlive(NPC_MAJORDOMO_EXECUTUS) ||
                IsAlive(NPC_FLAMEWAKER_HEALER) ||
                IsAlive(NPC_FLAMEWAKER_ELITE);
        }
    };

    class MajordomoAddsAliveTrigger : public MajordomoTriggerBase
    {
    public:
        MajordomoAddsAliveTrigger(PlayerbotAI* ai)
            : MajordomoTriggerBase(ai, "majordomo adds alive", 1) {}

        bool IsActive() override
        {
            return IsAlive(NPC_FLAMEWAKER_HEALER) ||
                IsAlive(NPC_FLAMEWAKER_ELITE);
        }
    };

    class MajordomoTargetsNeedMarkingTrigger : public MajordomoTriggerBase
    {
    public:
        MajordomoTargetsNeedMarkingTrigger(PlayerbotAI* ai)
            : MajordomoTriggerBase(ai, "majordomo targets need marking", 1) {}

        bool IsActive() override
        {
            if (!bot->GetGroup())
                return false;

            if (bot->InBattleGround())
                return false;

            return IsMajordomoEncounterActive();
        }
    };

    class MajordomoNeedsCrowdControlTrigger : public MajordomoTriggerBase
    {
    public:
        MajordomoNeedsCrowdControlTrigger(PlayerbotAI* ai)
            : MajordomoTriggerBase(ai, "majordomo needs crowd control", 1) {}

        bool IsActive() override
        {
            if (!IsMajordomoEncounterActive())
                return false;

            if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            // Tanks should not stop tanking to cast CC.
            if (ai->IsTank(bot))
                return false;

            // Let the action decide whether this bot actually has a usable CC spell.
            return true;
        }
    };

    class MajordomoTeleportedToPitTrigger : public MajordomoTriggerBase
    {
    public:
        MajordomoTeleportedToPitTrigger(PlayerbotAI* ai)
            : MajordomoTriggerBase(ai, "majordomo teleported to pit", 1) {}

        bool IsActive() override
        {
            if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            if (!ai->CanMove())
                return false;

            if (!IsMajordomoEncounterActive())
                return false;

            return IsInMajordomoPit();
        }

    private:
        bool IsInMajordomoPit()
        {
            // TODO: Replace with measured coordinates from your server/core.
            // The safest first version is a loose Z check plus an XY box.
            float x = bot->GetPositionX();
            float y = bot->GetPositionY();
            float z = bot->GetPositionZ();

            bool insideBox =
                x >= MAJORDOMO_PIT_MIN_X &&
                x <= MAJORDOMO_PIT_MAX_X &&
                y >= MAJORDOMO_PIT_MIN_Y &&
                y <= MAJORDOMO_PIT_MAX_Y;

            bool belowPlatform = z <= MAJORDOMO_PIT_MAX_Z;

            return insideBox && belowPlatform;
        }

    private:
        // TODO: Measure and replace.
        static constexpr float MAJORDOMO_PIT_MIN_X = 0.0f;
        static constexpr float MAJORDOMO_PIT_MAX_X = 0.0f;
        static constexpr float MAJORDOMO_PIT_MIN_Y = 0.0f;
        static constexpr float MAJORDOMO_PIT_MAX_Y = 0.0f;
        static constexpr float MAJORDOMO_PIT_MAX_Z = 0.0f;
    };

    class MoltenCoreTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        MoltenCoreTriggerContext()
        {
            creators["enter molten core"] = [](PlayerbotAI* ai){ return new EnterDungeonTrigger(ai, "enter molten core", "molten core", 409);};
            creators["leave molten core"] = [](PlayerbotAI* ai){ return new LeaveDungeonTrigger(ai, "leave molten core", "molten core", 409);};

            creators["start lucifron fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start lucifron fight", "lucifron", 12118);};
            creators["end lucifron fight"] = [](PlayerbotAI* ai) {return new EndBossFightTrigger(ai, "end lucifron fight", "lucifron", 12118);};
            creators["start magmadar fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start magmadar fight", "magmadar", 11982);};
            creators["end magmadar fight"] = [](PlayerbotAI* ai) {return new EndBossFightTrigger(ai, "end magmadar fight", "magmadar", 11982);};
            creators["start gehennas fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start gehennas fight", "gehennas", 12259);};
            creators["end gehennas fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end gehennas fight", "gehennas", 12259);};
            creators["start garr fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start garr fight", "garr", 12057);};
            creators["end garr fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end garr fight", "garr", 12057);};
            creators["start baron geddon fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start baron geddon fight", "baron geddon", 12056);};
            creators["end baron geddon fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end baron geddon fight", "baron geddon", 12056);};
            creators["start shazzrah fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start shazzrah fight", "shazzrah", 12264);};
            creators["end shazzrah fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end shazzrah fight", "shazzrah", 12264);};
            creators["start sulfuron harbinger fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start sulfuron harbinger fight", "sulfuron harbinger", 12098);};
            creators["end sulfuron harbinger fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end sulfuron harbinger fight", "sulfuron harbinger", 12098);};
            creators["start golemagg fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start golemagg fight", "golemagg", 11988);};
            creators["end golemagg fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end golemagg fight", "golemagg", 11988);};
            creators["golemagg alive"] = [](PlayerbotAI* ai){return new GolemaggAliveTrigger(ai);};
            creators["golemagg targets need marking"] = [](PlayerbotAI* ai){return new GolemaggTargetsNeedMarkingTrigger(ai);};
            creators["golemagg rager tank too close"] = [](PlayerbotAI* ai){return new GolemaggRagerTankTooCloseTrigger(ai);};
            creators["golemagg aggro holder out of position"] = [](PlayerbotAI* ai){return new GolemaggAggroHolderOutOfPositionTrigger(ai);};
            
            creators["start majordomo fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start majordomo fight", "majordomo", 12018);};
            creators["end majordomo fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end majordomo fight", "majordomo", 12018);};
            creators["majordomo adds alive"] = [](PlayerbotAI* ai){return new MajordomoAddsAliveTrigger(ai);};
            creators["majordomo targets need marking"] = [](PlayerbotAI* ai){return new MajordomoTargetsNeedMarkingTrigger(ai);};
            creators["majordomo needs crowd control"] = [](PlayerbotAI* ai){return new MajordomoNeedsCrowdControlTrigger(ai);};
            creators["majordomo teleported to pit"] = [](PlayerbotAI* ai){return new MajordomoTeleportedToPitTrigger(ai);};
            
            
            creators["start ragnaros fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start ragnaros fight", "ragnaros", 11502);};
            creators["end ragnaros fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end ragnaros fight", "ragnaros", 11502);};


            creators["magmadar lava bomb"] = [](PlayerbotAI* ai) {return new CloseToGameObjectHazardTrigger(ai, "magmadar lava bomb", 177704, 5.0f, 60);};
            creators["magmadar too close"] = [](PlayerbotAI* ai) {return new  CloseToCreatureTrigger(ai, "magmadar too close", 11982, 30.0f);};
            creators["fire protection potion ready"] = [](PlayerbotAI* ai){ return new ItemBuffReadyTrigger(ai, "fire protection potion ready", 13457, 17543);};

            creators["mc rune in sight"] = &MoltenCoreTriggerContext::mc_rune_in_sight;
            creators["mc rune close"] = &MoltenCoreTriggerContext::mc_rune_close;
        }
        private:
        static Trigger* mc_rune_in_sight(PlayerbotAI* ai) { return new MCRuneInSightTrigger(ai); };
        static Trigger* mc_rune_close(PlayerbotAI* ai) { return new MCRuneCloseTrigger(ai); };
    };

  
   

};
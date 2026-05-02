#pragma once
#include "DungeonTriggers.h"
#include "GenericTriggers.h"

namespace ai
{
    class OnyxiaTriggerBase : public Trigger
    {
    public:
        OnyxiaTriggerBase(PlayerbotAI* ai, std::string name, int checkInterval = 1)
            : Trigger(ai, name, checkInterval) {}

    protected:
        static const uint32 NPC_ONYXIA = 10184;
        static const uint32 NPC_ONYXIAN_WHELP = 11262;

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

        bool IsAlive(uint32 entry)
        {
            return FindAliveCreature(entry) != nullptr;
        }
    };


    class OnyxiaAliveTrigger : public OnyxiaTriggerBase
    {
    public:
        OnyxiaAliveTrigger(PlayerbotAI* ai)
            : OnyxiaTriggerBase(ai, "onyxia alive", 1) {}

        bool IsActive() override
        {
            return IsAlive(NPC_ONYXIA);
        }
    };


    class OnyxiaWhelpAliveTrigger : public OnyxiaTriggerBase
    {
    public:
        OnyxiaWhelpAliveTrigger(PlayerbotAI* ai)
            : OnyxiaTriggerBase(ai, "onyxia whelp alive", 1) {}

        bool IsActive() override
        {
            return IsAlive(NPC_ONYXIAN_WHELP);
        }
    };


    class OnyxiaFlyingTrigger : public OnyxiaTriggerBase
    {
    public:
        OnyxiaFlyingTrigger(PlayerbotAI* ai)
            : OnyxiaTriggerBase(ai, "onyxia flying", 1) {}

        bool IsActive() override
        {
            Unit* onyxia = FindAliveCreature(NPC_ONYXIA);
            if (!onyxia)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            // Simple phase-2 approximation:
            // Onyxia is considered flying when she is significantly above the raid.
            // Adjust 8.0f if your map/core reports Z values differently.
            return onyxia->GetPositionZ() > bot->GetPositionZ() + 8.0f;
        }
    };

    class OnyxiaTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        OnyxiaTriggerContext()
        {
            // Onyxia's Lair map ID: 249
            creators["enter onyxia lair"] = [](PlayerbotAI* ai)
            {
                return new EnterDungeonTrigger(ai, "enter onyxia lair", "onyxia lair", 249);
            };

            creators["leave onyxia lair"] = [](PlayerbotAI* ai)
            {
                return new LeaveDungeonTrigger(ai, "leave onyxia lair", "onyxia lair", 249);
            };

            // Onyxia NPC ID: 10184
            creators["start onyxia fight"] = [](PlayerbotAI* ai)
            {
                return new StartBossFightTrigger(ai, "start onyxia fight", "onyxia", 10184);
            };

            creators["end onyxia fight"] = [](PlayerbotAI* ai)
            {
                return new EndBossFightTrigger(ai, "end onyxia fight", "onyxia", 10184);
            };

            creators["onyxia alive"] = [](PlayerbotAI* ai)
            {
                return new OnyxiaAliveTrigger(ai);
            };

            creators["onyxia whelp alive"] = [](PlayerbotAI* ai)
            {
                return new OnyxiaWhelpAliveTrigger(ai);
            };

            creators["onyxia flying"] = [](PlayerbotAI* ai)
            {
                return new OnyxiaFlyingTrigger(ai);
            };

            creators["onyxia too close"] = [](PlayerbotAI* ai)
            {
                return new CloseToCreatureTrigger(ai, "onyxia too close", 10184, 25.0f);
            };

            
        }
    };
}
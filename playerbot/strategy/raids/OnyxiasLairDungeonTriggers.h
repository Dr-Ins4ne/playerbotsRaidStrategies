#pragma once

#include "../triggers/DungeonTriggers.h"
#include "../triggers/GenericTriggers.h"

#include "DungeonTargetHelper.h"

#include <cmath>

namespace ai
{
    class OnyxiaTriggerBase : public Trigger
    {
    public:
        OnyxiaTriggerBase(PlayerbotAI* ai, std::string name, int checkInterval = 1)
            : Trigger(ai, name, checkInterval) {}

    protected:
        static constexpr uint32 MAP_ONYXIA_LAIR = 249;

        static constexpr uint32 NPC_ONYXIA = 10184;
        static constexpr uint32 NPC_ONYXIAN_WHELP = 11262;

        static constexpr float ONYXIA_EGG_PIT_1_X = -27.0f;
        static constexpr float ONYXIA_EGG_PIT_1_Y = -186.6f;
        static constexpr float ONYXIA_EGG_PIT_1_Z = -89.0f;

        static constexpr float ONYXIA_EGG_PIT_2_X = -27.0f;
        static constexpr float ONYXIA_EGG_PIT_2_Y = -250.0f;
        static constexpr float ONYXIA_EGG_PIT_2_Z = -89.0f;

        static constexpr float ONYXIA_SAFE_CENTER_X = 13.0f;
        static constexpr float ONYXIA_SAFE_CENTER_Y = -205.0f;
        static constexpr float ONYXIA_SAFE_CENTER_Z = -85.77f;

        // Tune this if bots still touch eggs.
        static constexpr float ONYXIA_EGG_PIT_RADIUS = 20.0f;

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            return DungeonTargetHelper::FindAliveCreature(ai, entry);
        }

        bool IsAlive(uint32 entry)
        {
            return FindAliveCreature(entry) != nullptr;
        }

        bool CurrentTargetIs(uint32 entry)
        {
            return DungeonTargetHelper::IsCurrentTargetEntry(ai, entry);
        }

        bool IsOnyxiaFlying()
        {
            Unit* onyxia = FindAliveCreature(NPC_ONYXIA);
            if (!onyxia)
                return false;

            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            // Simple phase-2 approximation:
            // Onyxia is considered flying when she is significantly above the bot/raid.
            // Adjust 8.0f if your core reports Z values differently.
            return onyxia->GetPositionZ() > bot->GetPositionZ() + 8.0f;
        }

        static float Distance2d(float x1, float y1, float x2, float y2)
        {
            float dx = x1 - x2;
            float dy = y1 - y2;
            return std::sqrt(dx * dx + dy * dy);
        }

        bool IsNearOnyxiaEggPit()
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (!bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            if (bot->GetMapId() != MAP_ONYXIA_LAIR)
                return false;

            float x = bot->GetPositionX();
            float y = bot->GetPositionY();

            float d1 = Distance2d(x, y, ONYXIA_EGG_PIT_1_X, ONYXIA_EGG_PIT_1_Y);
            float d2 = Distance2d(x, y, ONYXIA_EGG_PIT_2_X, ONYXIA_EGG_PIT_2_Y);

            return d1 <= ONYXIA_EGG_PIT_RADIUS ||
                   d2 <= ONYXIA_EGG_PIT_RADIUS;
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
            return IsOnyxiaFlying();
        }
    };


    class OnyxiaEggPitTooCloseTrigger : public OnyxiaTriggerBase
    {
    public:
        OnyxiaEggPitTooCloseTrigger(PlayerbotAI* ai)
            : OnyxiaTriggerBase(ai, "onyxia egg pit too close", 1) {}

        bool IsActive() override
        {
            if (!IsNearOnyxiaEggPit())
                return false;

            // Avoid spamming a new MoveTo every tick while the bot is already moving.
            if (AI_VALUE2(bool, "moving", "self target"))
                return false;

            return true;
        }
    };


    class OnyxiaFlyingMeleeNeedsWhelpTargetTrigger : public OnyxiaTriggerBase
    {
    public:
        OnyxiaFlyingMeleeNeedsWhelpTargetTrigger(PlayerbotAI* ai)
            : OnyxiaTriggerBase(ai, "onyxia flying melee needs whelp target", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            // Healers should not be forced into DPS targeting.
            if (ai->IsHeal(bot))
                return false;

            // Ranged DPS should keep Onyxia targeted in phase 2.
            if (ai->IsRanged(bot))
                return false;

            if (!IsOnyxiaFlying())
                return false;

            if (!IsAlive(NPC_ONYXIAN_WHELP))
                return false;

            // Only fire if the current target is wrong.
            return !CurrentTargetIs(NPC_ONYXIAN_WHELP);
        }
    };


    class OnyxiaFlyingRangedNeedsOnyxiaTargetTrigger : public OnyxiaTriggerBase
    {
    public:
        OnyxiaFlyingRangedNeedsOnyxiaTargetTrigger(PlayerbotAI* ai)
            : OnyxiaTriggerBase(ai, "onyxia flying ranged needs onyxia target", 1) {}

        bool IsActive() override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            // Healers should not be forced to target Onyxia.
            if (ai->IsHeal(bot))
                return false;

            if (!ai->IsRanged(bot))
                return false;

            if (!IsOnyxiaFlying())
                return false;

            if (!IsAlive(NPC_ONYXIA))
                return false;

            // Only fire if the current target is wrong.
            return !CurrentTargetIs(NPC_ONYXIA);
        }
    };


    class OnyxiaTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        OnyxiaTriggerContext()
        {
            creators["enter onyxia lair"] = [](PlayerbotAI* ai) { return new EnterDungeonTrigger(ai, "enter onyxia lair", "onyxia lair", 249); };
            creators["leave onyxia lair"] = [](PlayerbotAI* ai) { return new LeaveDungeonTrigger(ai, "leave onyxia lair", "onyxia lair", 249); };

            creators["start onyxia fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai, "start onyxia fight", "onyxia", 10184); };
            creators["end onyxia fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai, "end onyxia fight", "onyxia", 10184); };

            creators["onyxia alive"] = [](PlayerbotAI* ai) { return new OnyxiaAliveTrigger(ai); };
            creators["onyxia whelp alive"] = [](PlayerbotAI* ai) { return new OnyxiaWhelpAliveTrigger(ai); };
            creators["onyxia flying"] = [](PlayerbotAI* ai) { return new OnyxiaFlyingTrigger(ai); };
            creators["onyxia egg pit too close"] = [](PlayerbotAI* ai) { return new OnyxiaEggPitTooCloseTrigger(ai); };

            creators["onyxia flying melee needs whelp target"] = [](PlayerbotAI* ai) { return new OnyxiaFlyingMeleeNeedsWhelpTargetTrigger(ai); };
            creators["onyxia flying ranged needs onyxia target"] = [](PlayerbotAI* ai) { return new OnyxiaFlyingRangedNeedsOnyxiaTargetTrigger(ai); };

            creators["onyxia too close"] = [](PlayerbotAI* ai) { return new CloseToCreatureTrigger(ai, "onyxia too close", 10184, 5.0f); };
        }
    };
}
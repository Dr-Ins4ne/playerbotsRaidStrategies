#pragma once

#include "../triggers/DungeonTriggers.h"
#include "../triggers/GenericTriggers.h"
#include "playerbot/strategy/values/GuidPositionValues.h"

namespace ai
{
    class BlackwingLairEnterDungeonTrigger : public EnterDungeonTrigger
    {
    public:
        BlackwingLairEnterDungeonTrigger(PlayerbotAI* ai) : EnterDungeonTrigger(ai, "enter blackwing lair", "blackwing lair", 469) {}
    };

    class BlackwingLairLeaveDungeonTrigger : public LeaveDungeonTrigger
    {
    public:
        BlackwingLairLeaveDungeonTrigger(PlayerbotAI* ai) : LeaveDungeonTrigger(ai, "leave blackwing lair", "blackwing lair", 469) {}
    };

    class RazorgoreStartFightTrigger : public Trigger
    {
    public:
        RazorgoreStartFightTrigger(PlayerbotAI* ai) : Trigger(ai, "start razorgore fight", 1) {}
        bool IsActive() override;
    };

    class RazorgoreEndFightTrigger : public Trigger
    {
    public:
        RazorgoreEndFightTrigger(PlayerbotAI* ai) : Trigger(ai, "end razorgore fight", 2) {}
        bool IsActive() override;
    };

    class RazorgoreControllerAliveTrigger : public Trigger
    {
    public:
        RazorgoreControllerAliveTrigger(PlayerbotAI* ai) : Trigger(ai, "razorgore controller alive", 1) {}
        bool IsActive() override;
    };

    class RazorgoreEggPhaseTrigger : public Trigger
    {
    public:
        RazorgoreEggPhaseTrigger(PlayerbotAI* ai) : Trigger(ai, "razorgore egg phase", 1) {}
        bool IsActive() override;
    };

    class RazorgoreFarFromBossTrigger : public Trigger
    {
    public:
        RazorgoreFarFromBossTrigger(PlayerbotAI* ai) : Trigger(ai, "razorgore far from boss", 1) {}
        bool IsActive() override;
    };

    class SuppressionDeviceNeedStealthTrigger : public Trigger
    {
    public:
        SuppressionDeviceNeedStealthTrigger(PlayerbotAI* ai) : Trigger(ai, "suppression device need stealth", 1) {}

        bool IsActive() override
        {
            if (bot->getClass() != CLASS_ROGUE)
                return false;

            if (ai->HasAura("stealth", bot))
                return false;

            std::list<GuidPosition> gos = AI_VALUE(std::list<GuidPosition>, "go usable filter::go trapped filter::entry filter::{gos in sight,suppression devices}");
            return !gos.empty();
        }
    };

    class SuppressionDeviceInSightTrigger : public Trigger
    {
    public:
        SuppressionDeviceInSightTrigger(PlayerbotAI* ai) : Trigger(ai, "suppression device in sight", 1) {}

        bool IsActive() override
        {
            if (bot->getClass() != CLASS_ROGUE)
                return false;

            std::list<GuidPosition> gosInSight = AI_VALUE(std::list<GuidPosition>, "go usable filter::go trapped filter::entry filter::{gos in sight,suppression devices}");
            std::list<GuidPosition> gosClose = AI_VALUE(std::list<GuidPosition>, "entry filter::{gos close,suppression devices}");
            
            return !gosInSight.empty() && gosClose.empty();
        }
    };

    class SuppressionDeviceCloseTrigger : public Trigger
    {
    public:
        SuppressionDeviceCloseTrigger(PlayerbotAI* ai) : Trigger(ai, "suppression device close", 1) {}

        bool IsActive() override
        {
            if (bot->getClass() != CLASS_ROGUE)
                return false;

            std::list<GuidPosition> gos = AI_VALUE(std::list<GuidPosition>, "go usable filter::go trapped filter::entry filter::{gos close,suppression devices}");
            return !gos.empty();
        }
    };

    class BlackwingLairDungeonTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        BlackwingLairDungeonTriggerContext()
        {
            creators["enter blackwing lair"] = [](PlayerbotAI* ai) { return new BlackwingLairEnterDungeonTrigger(ai); };
            creators["leave blackwing lair"] = [](PlayerbotAI* ai) { return new BlackwingLairLeaveDungeonTrigger(ai); };
            creators["start razorgore fight"] = [](PlayerbotAI* ai) { return new RazorgoreStartFightTrigger(ai); };
            creators["end razorgore fight"] = [](PlayerbotAI* ai) { return new RazorgoreEndFightTrigger(ai); };
            creators["razorgore controller alive"] = [](PlayerbotAI* ai) { return new RazorgoreControllerAliveTrigger(ai); };
            creators["razorgore egg phase"] = [](PlayerbotAI* ai) { return new RazorgoreEggPhaseTrigger(ai); };
            creators["razorgore far from boss"] = [](PlayerbotAI* ai) { return new RazorgoreFarFromBossTrigger(ai); };
            creators["suppression device need stealth"] = [](PlayerbotAI* ai) { return new SuppressionDeviceNeedStealthTrigger(ai); };
            creators["suppression device in sight"] = [](PlayerbotAI* ai) { return new SuppressionDeviceInSightTrigger(ai); };
            creators["suppression device close"] = [](PlayerbotAI* ai) { return new SuppressionDeviceCloseTrigger(ai); };
        }
    };
}

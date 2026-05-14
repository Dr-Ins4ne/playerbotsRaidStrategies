#pragma once

#include "../triggers/DungeonTriggers.h"
#include "../triggers/GenericTriggers.h"
#include "playerbot/strategy/values/GuidPositionValues.h"
#include "playerbot/strategy/NamedObjectContext.h"
#include "BlackwingLairDungeonData.h"
#include "BlackwingLairDungeonActions.h"

namespace ai
{
    class BlackwingLairEnterDungeonTrigger : public EnterDungeonTrigger
    {
    public:
        BlackwingLairEnterDungeonTrigger(PlayerbotAI* ai)
            : EnterDungeonTrigger(ai, "enter blackwing lair", "blackwing lair", BlackwingLair::MAP_ID) {}
    };

    class BlackwingLairLeaveDungeonTrigger : public LeaveDungeonTrigger
    {
    public:
        BlackwingLairLeaveDungeonTrigger(PlayerbotAI* ai)
            : LeaveDungeonTrigger(ai, "leave blackwing lair", "blackwing lair", BlackwingLair::MAP_ID) {}
    };



    class RazorgoreControllerNeedsTargetTrigger : public Trigger
    {
    public:
        RazorgoreControllerNeedsTargetTrigger(PlayerbotAI* ai, std::string name = "razorgore controller needs target")
            : Trigger(ai, name, 1) {}

        bool IsActive() override;
    };

    // Compatibility wrapper for older strategy text. Despite the legacy name,
    // this uses the same "needs target" condition to avoid action-queue spam.
    class RazorgoreControllerAliveTrigger : public RazorgoreControllerNeedsTargetTrigger
    {
    public:
        RazorgoreControllerAliveTrigger(PlayerbotAI* ai)
            : RazorgoreControllerNeedsTargetTrigger(ai, "razorgore controller alive") {}
    };

    class RazorgoreEggPhaseTrigger : public Trigger
    {
    public:
        RazorgoreEggPhaseTrigger(PlayerbotAI* ai)
            : Trigger(ai, "razorgore egg phase", 1) {}

        bool IsActive() override;
    };

    class RazorgoreFarFromBossTrigger : public Trigger
    {
    public:
        RazorgoreFarFromBossTrigger(PlayerbotAI* ai)
            : Trigger(ai, "razorgore far from boss", 1) {}

        bool IsActive() override;
    };

 

    class VaelastraszTankNeedsPullPositionTrigger : public Trigger
    {
    public:
        VaelastraszTankNeedsPullPositionTrigger(PlayerbotAI* ai)
            : Trigger(ai, "vaelastrasz tank needs pull position", 1) {}

        bool IsActive() override;
    };

    class VaelastraszRangedNearPullPositionTrigger : public Trigger
    {
    public:
        VaelastraszRangedNearPullPositionTrigger(PlayerbotAI* ai)
            : Trigger(ai, "vaelastrasz ranged near pull position", 1) {}

        bool IsActive() override;
    };



    class BroodlordOutOfPositionTrigger : public Trigger
    {
    public:
        BroodlordOutOfPositionTrigger(PlayerbotAI* ai)
            : Trigger(ai, "broodlord out of position", 1) {}

        bool IsActive() override;
    };

    class SuppressionDeviceNeedStealthTrigger : public Trigger
    {
    public:
        SuppressionDeviceNeedStealthTrigger(PlayerbotAI* ai)
            : Trigger(ai, "suppression device need stealth", 1) {}

        bool IsActive() override;
    };

    class SuppressionDeviceInSightTrigger : public Trigger
    {
    public:
        SuppressionDeviceInSightTrigger(PlayerbotAI* ai)
            : Trigger(ai, "suppression device in sight", 1) {}

        bool IsActive() override;
    };

    class SuppressionDeviceCloseTrigger : public Trigger
    {
    public:
        SuppressionDeviceCloseTrigger(PlayerbotAI* ai)
            : Trigger(ai, "suppression device close", 2) {}

        bool IsActive() override;
    };

    class BlackwingLairTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        BlackwingLairTriggerContext()
        {
            creators["enter blackwing lair"] = [](PlayerbotAI* ai) { return new BlackwingLairEnterDungeonTrigger(ai); };
            creators["leave blackwing lair"] = [](PlayerbotAI* ai) { return new BlackwingLairLeaveDungeonTrigger(ai); };
            creators["start razorgore fight"] = [](PlayerbotAI* ai) { return new StartBossFightTrigger(ai,"start razorgore fight","razorgore",BlackwingLair::NPC_RAZORGORE); };
            creators["end razorgore fight"] = [](PlayerbotAI* ai) { return new EndBossFightTrigger(ai,"end razorgore fight","razorgore",BlackwingLair::NPC_RAZORGORE);};
            creators["start vaelastrasz fight"] = [](PlayerbotAI* ai){return new StartBossFightTrigger(ai,"start vaelastrasz fight","vaelastrasz",BlackwingLair::NPC_VAELASTRASZ);};
            creators["end vaelastrasz fight"] = [](PlayerbotAI* ai){return new EndBossFightTrigger(ai,"end vaelastrasz fight","vaelastrasz",BlackwingLair::NPC_VAELASTRASZ);};
            creators["start broodlord fight"] = [](PlayerbotAI* ai){return new StartBossFightTrigger(ai,"start broodlord fight","broodlord",BlackwingLair::NPC_BROODLORD_LASHLAYER);};
            creators["end broodlord fight"] = [](PlayerbotAI* ai){return new EndBossFightTrigger(ai,"end broodlord fight","broodlord",BlackwingLair::NPC_BROODLORD_LASHLAYER);};

            // Preferred anti-spam trigger. It becomes false once Grethok is the current target.
            creators["razorgore controller needs target"] = [](PlayerbotAI* ai) { return new RazorgoreControllerNeedsTargetTrigger(ai); };

            // Compatibility alias for older strategy text. Do not use a pure "alive" condition here,
            // because that would keep enqueuing high-priority actions every trigger tick.
            creators["razorgore controller alive"] = [](PlayerbotAI* ai) { return new RazorgoreControllerAliveTrigger(ai); };

            creators["razorgore egg phase"] = [](PlayerbotAI* ai) { return new RazorgoreEggPhaseTrigger(ai); };
            creators["razorgore far from boss"] = [](PlayerbotAI* ai) { return new RazorgoreFarFromBossTrigger(ai); };

            creators["vaelastrasz tank needs pull position"] = [](PlayerbotAI* ai) { return new VaelastraszTankNeedsPullPositionTrigger(ai); };
            creators["vaelastrasz ranged near pull position"] = [](PlayerbotAI* ai) { return new VaelastraszRangedNearPullPositionTrigger(ai); };
            

            creators["broodlord out of position"] = [](PlayerbotAI* ai){return new BroodlordOutOfPositionTrigger(ai);};
            
            creators["suppression device need stealth"] = [](PlayerbotAI* ai) { return new SuppressionDeviceNeedStealthTrigger(ai); };
            creators["suppression device in sight"] = [](PlayerbotAI* ai) { return new SuppressionDeviceInSightTrigger(ai); };
            creators["suppression device close"] = [](PlayerbotAI* ai) { return new SuppressionDeviceCloseTrigger(ai); };
        }
    };

}

#pragma once

#include "../actions/ChangeStrategyAction.h"
#include "../actions/MovementActions.h"
#include "DungeonMovementActionBase.h"
#include "RaidIconActionBase.h"
#include "playerbot/strategy/values/GuidPositionValues.h"
#include "playerbot/strategy/NamedObjectContext.h"
#include "BlackwingLairDungeonData.h"

#include <cfloat>
#include <cmath>
#include <memory>
#include <utility>

namespace ai
{
    const uint32 SPELL_DISARM_TRAP = 1842;

    class BlackwingLairEnableDungeonStrategyAction : public ChangeAllStrategyAction
    {
    public:
        BlackwingLairEnableDungeonStrategyAction(PlayerbotAI* ai)
            : ChangeAllStrategyAction(ai, "enable blackwing lair strategy", "+blackwing lair") {}
    };

    class BlackwingLairDisableDungeonStrategyAction : public ChangeAllStrategyAction
    {
    public:
        BlackwingLairDisableDungeonStrategyAction(PlayerbotAI* ai)
            : ChangeAllStrategyAction(ai, "disable blackwing lair strategy", "-blackwing lair") {}
    };

    class RazorgoreEnableFightStrategyAction : public Action
    {
    public:
        RazorgoreEnableFightStrategyAction(PlayerbotAI* ai)
            : Action(ai, "enable razorgore fight strategy") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class RazorgoreDisableFightStrategyAction : public Action
    {
    public:
        RazorgoreDisableFightStrategyAction(PlayerbotAI* ai)
            : Action(ai, "disable razorgore fight strategy") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class MarkRazorgoreControllerAction : public RaidIconActionBase
    {
    public:
        MarkRazorgoreControllerAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "mark razorgore controller") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class MoveNearRazorgoreAction : public MovementAction
    {
    public:
        MoveNearRazorgoreAction(PlayerbotAI* ai)
            : MovementAction(ai, "move near razorgore") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
        bool isUseful() override;
    };

    class MoveToVaelastraszTankPositionAction : public DungeonMovementActionBase
    {
    public:
        MoveToVaelastraszTankPositionAction(PlayerbotAI* ai)
            : DungeonMovementActionBase(ai, "move to vaelastrasz tank position") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
        bool isUseful() override;
    };

    class MoveToVaelastraszRangedPositionAction : public DungeonMovementActionBase
    {
    public:
        MoveToVaelastraszRangedPositionAction(PlayerbotAI* ai)
            : DungeonMovementActionBase(ai, "move to vaelastrasz ranged position") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
        bool isUseful() override;
    };

    class MoveToBroodlordStackPositionAction : public DungeonMovementActionBase
    {
    public:
        MoveToBroodlordStackPositionAction(PlayerbotAI* ai)
            : DungeonMovementActionBase(ai, "move to broodlord stack position") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class MoveToSuppressionDeviceAction : public MovementAction
    {
    public:
        MoveToSuppressionDeviceAction(PlayerbotAI* ai)
            : MovementAction(ai, "move to suppression device") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
        bool isUseful() override;
    };

    class StealthForSuppressionDeviceAction : public Action
    {
    public:
        StealthForSuppressionDeviceAction(PlayerbotAI* ai)
            : Action(ai, "stealth for suppression device") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
        bool isUseful() override;
    };

    class DeactivateSuppressionDeviceAction : public Action
    {
    public:
        DeactivateSuppressionDeviceAction(PlayerbotAI* ai)
            : Action(ai, "deactivate suppression device") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
        bool isUseful() override;
    };

    class DisarmSuppressionDeviceAction : public Action
    {
    public:
        DisarmSuppressionDeviceAction(PlayerbotAI* ai)
            : Action(ai, "disarm suppression device") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
        bool isUseful() override;
    };

    class BlackwingLairActionContext : public NamedObjectContext<Action>
    {
    public:
        BlackwingLairActionContext()
        {
            creators["enable blackwing lair strategy"] = [](PlayerbotAI* ai) { return new BlackwingLairEnableDungeonStrategyAction(ai); };
            creators["disable blackwing lair strategy"] = [](PlayerbotAI* ai) { return new BlackwingLairDisableDungeonStrategyAction(ai); };
            creators["enable razorgore fight strategy"] = [](PlayerbotAI* ai) { return new RazorgoreEnableFightStrategyAction(ai); };
            creators["disable razorgore fight strategy"] = [](PlayerbotAI* ai) { return new RazorgoreDisableFightStrategyAction(ai); };
            creators["enable vaelastrasz fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable vaelastrasz fight strategy", "+vaelastrasz"); };
            creators["disable vaelastrasz fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable vaelastrasz fight strategy", "-vaelastrasz"); };

            // Preferred name. This marks/selects Grethok without bypassing normal combat execution.
            creators["mark razorgore controller"] = [](PlayerbotAI* ai) { return new MarkRazorgoreControllerAction(ai); };

            // Compatibility alias for older strategy text. It intentionally uses the anti-spam mark/select action.
            creators["focus razorgore controller"] = [](PlayerbotAI* ai) { return new MarkRazorgoreControllerAction(ai); };

            creators["move near razorgore"] = [](PlayerbotAI* ai) { return new MoveNearRazorgoreAction(ai); };
            creators["move to vaelastrasz tank position"] = [](PlayerbotAI* ai) { return new MoveToVaelastraszTankPositionAction(ai); };
            creators["move to vaelastrasz ranged position"] = [](PlayerbotAI* ai) { return new MoveToVaelastraszRangedPositionAction(ai); };
            
            creators["enable broodlord fight strategy"] = [](PlayerbotAI* ai){return new ChangeCombatStrategyAction(ai, "enable broodlord fight strategy", "+broodlord");};
            creators["disable broodlord fight strategy"] = [](PlayerbotAI* ai){return new ChangeCombatStrategyAction(ai, "disable broodlord fight strategy", "-broodlord");};
            creators["move to broodlord stack position"] = [](PlayerbotAI* ai){return new MoveToBroodlordStackPositionAction(ai);};
            
            
            creators["move to suppression device"] = [](PlayerbotAI* ai) { return new MoveToSuppressionDeviceAction(ai); };
            creators["stealth for suppression device"] = [](PlayerbotAI* ai) { return new StealthForSuppressionDeviceAction(ai); };
            creators["deactivate suppression device"] = [](PlayerbotAI* ai) { return new DeactivateSuppressionDeviceAction(ai); };
            creators["disarm suppression device"] = [](PlayerbotAI* ai) { return new DisarmSuppressionDeviceAction(ai); };
        }
    };

}

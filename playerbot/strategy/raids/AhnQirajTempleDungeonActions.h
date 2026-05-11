#pragma once

#include "../actions/ChangeStrategyAction.h"
#include "../actions/DungeonActions.h"
#include "../actions/UseItemAction.h"

#include "AhnQirajTempleDungeonUtils.h"
#include "RaidIconActionBase.h"

#include "playerbot/strategy/NamedObjectContext.h"

namespace ai
{
    class SkeramMarkImagesAction : public RaidIconActionBase
    {
    public:
        SkeramMarkImagesAction(PlayerbotAI* ai) : RaidIconActionBase(ai, "skeram mark images") {}
        bool isUseful() override;
        bool Execute(Event& event) override;
    };

    class SkeramMarkRealAction : public RaidIconActionBase
    {
    public:
        SkeramMarkRealAction(PlayerbotAI* ai) : RaidIconActionBase(ai, "skeram mark real") {}
        bool isUseful() override;
        bool Execute(Event& event) override;
    };

    class SkeramSelectImageTargetAction : public RaidIconActionBase
    {
    public:
        SkeramSelectImageTargetAction(PlayerbotAI* ai) : RaidIconActionBase(ai, "skeram select image target") {}
        bool isUseful() override;
        bool Execute(Event& event) override;
    };

    class SkeramSelectRealTargetAction : public RaidIconActionBase
    {
    public:
        SkeramSelectRealTargetAction(PlayerbotAI* ai) : RaidIconActionBase(ai, "skeram select real target") {}
        bool isUseful() override;
        bool Execute(Event& event) override;
    };

    class SkeramPullToTankPositionAction : public MovementAction
    {
    public:
        SkeramPullToTankPositionAction(PlayerbotAI* ai) : MovementAction(ai, "skeram pull to tank position") {}
        bool Execute(Event& event) override;
        bool isUseful() override;
        bool isPossible() override;
    };

    class SkeramMoveToPullPositionAction : public MovementAction
    {
    public:
        SkeramMoveToPullPositionAction(PlayerbotAI* ai) : MovementAction(ai, "skeram move to pull position") {}
        bool Execute(Event& event) override;
        bool isUseful() override;
        bool isPossible() override;
    };

    class CrowdControlSkeramControlledTargetAction : public Action
    {
    public:
        CrowdControlSkeramControlledTargetAction(PlayerbotAI* ai) : Action(ai, "cc skeram controlled target") {}
        bool Execute(Event& event) override;
        bool isPossible() override;
    };

    class SkeramSelectControlledPlayerForFleeAction : public Action
    {
    public:
        SkeramSelectControlledPlayerForFleeAction(PlayerbotAI* ai) : Action(ai, "skeram select controlled player for flee") {}
        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class SkeramTankTargetRealAction : public Action
    {
    public:
        SkeramTankTargetRealAction(PlayerbotAI* ai) : Action(ai, "skeram tank target real") {}
        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class UseGreaterNatureProtectionPotionAction : public UseItemIdAction
    {
    public:
        UseGreaterNatureProtectionPotionAction(PlayerbotAI* ai);
        bool isUseful() override;
        bool isPossible() override;
    };

    class AhnQirajTempleActionContext : public NamedObjectContext<Action>
    {
    public:
        AhnQirajTempleActionContext()
        {
            creators["enable ahnqiraj temple strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable ahnqiraj temple strategy", "+ahnqiraj temple"); };
            creators["disable ahnqiraj temple strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable ahnqiraj temple strategy", "-ahnqiraj temple"); };
            creators["enable skeram fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable skeram fight strategy", "+skeram"); };
            creators["disable skeram fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable skeram fight strategy", "-skeram"); };
            creators["skeram mark images"] = [](PlayerbotAI* ai) { return new SkeramMarkImagesAction(ai); };
            creators["skeram mark real"] = [](PlayerbotAI* ai) { return new SkeramMarkRealAction(ai); };
            creators["skeram select image target"] = [](PlayerbotAI* ai) { return new SkeramSelectImageTargetAction(ai); };
            creators["skeram select real target"] = [](PlayerbotAI* ai) { return new SkeramSelectRealTargetAction(ai); };
            creators["skeram pull to tank position"] = [](PlayerbotAI* ai) { return new SkeramPullToTankPositionAction(ai); };
            creators["skeram move to pull position"] = [](PlayerbotAI* ai) { return new SkeramMoveToPullPositionAction(ai); };
            creators["cc skeram controlled target"] = [](PlayerbotAI* ai) { return new CrowdControlSkeramControlledTargetAction(ai); };
            creators["skeram select controlled player for flee"] = [](PlayerbotAI* ai) { return new SkeramSelectControlledPlayerForFleeAction(ai); };
            creators["skeram tank target real"] = [](PlayerbotAI* ai) { return new SkeramTankTargetRealAction(ai); };
            creators["use greater nature protection potion"] = [](PlayerbotAI* ai) { return new UseGreaterNatureProtectionPotionAction(ai); };
        }
    };
}

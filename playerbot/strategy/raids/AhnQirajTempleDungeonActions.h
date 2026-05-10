#pragma once

#include "../actions/GenericActions.h"
#include "../actions/ChangeStrategyAction.h"
#include "../actions/GenericSpellActions.h"
#include "../actions/DungeonActions.h"
#include "RaidIconActionBase.h"
#include "AhnQirajTempleDungeonUtils.h"
#include "playerbot/strategy/NamedObjectContext.h"

#include <vector>

namespace ai
{

    class CrowdControlSkeramControlledTargetAction : public Action
    {
    public:
        CrowdControlSkeramControlledTargetAction(PlayerbotAI* ai)
            : Action(ai, "cc skeram controlled target") {}

        bool Execute(Event& event) override;
        bool isPossible() override;
    };

    class MoveAwayFromSkeramControlledPlayerAction : public MovementAction
    {
    public:
        MoveAwayFromSkeramControlledPlayerAction(PlayerbotAI* ai)
            : MovementAction(ai, "move away from skeram controlled player") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
        bool isPossible() override;
    };
    class SkeramMarkImagesAction : public RaidIconActionBase
    {
    public:
        SkeramMarkImagesAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "skeram mark images") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class SkeramMarkRealAction : public RaidIconActionBase
    {
    public:
        SkeramMarkRealAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "skeram mark real") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class SkeramSelectImageTargetAction : public RaidIconActionBase
    {
    public:
        SkeramSelectImageTargetAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "skeram select image target") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class SkeramSelectRealTargetAction : public RaidIconActionBase
    {
    public:
        SkeramSelectRealTargetAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "skeram select real target") {}

        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    class SkeramPullToTankPositionAction : public MovementAction
    {
    public:
        SkeramPullToTankPositionAction(PlayerbotAI* ai)
            : MovementAction(ai, "skeram pull to tank position") {}

        bool Execute(Event& event) override;
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
            creators["skeram pull to tank position"] = [](PlayerbotAI* ai){return new SkeramPullToTankPositionAction(ai);};
            creators["cc skeram controlled target"] = [](PlayerbotAI* ai){return new CrowdControlSkeramControlledTargetAction(ai);};
            creators["move away from skeram controlled player"] = [](PlayerbotAI* ai){return new MoveAwayFromSkeramControlledPlayerAction(ai);};
        }
    };
}

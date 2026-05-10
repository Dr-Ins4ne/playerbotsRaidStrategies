#pragma once

#include "DungeonTriggers.h"
#include "../generic/AhnQirajTempleDungeonUtils.h"
#include "playerbot/strategy/NamedObjectContext.h"

namespace ai
{
    class AhnQirajTempleEnterDungeonTrigger : public EnterDungeonTrigger
    {
    public:
        AhnQirajTempleEnterDungeonTrigger(PlayerbotAI* ai)
            : EnterDungeonTrigger(ai, "enter aq40", "ahnqiraj temple", AhnQirajTemple::MAP_AHNQIRAJ_TEMPLE) {}
    };

    class AhnQirajTempleLeaveDungeonTrigger : public LeaveDungeonTrigger
    {
    public:
        AhnQirajTempleLeaveDungeonTrigger(PlayerbotAI* ai)
            : LeaveDungeonTrigger(ai, "leave aq40", "ahnqiraj temple", AhnQirajTemple::MAP_AHNQIRAJ_TEMPLE) {}
    };

    class SkeramStartFightTrigger : public StartBossFightTrigger
    {
    public:
        SkeramStartFightTrigger(PlayerbotAI* ai)
            : StartBossFightTrigger(ai, "start skeram fight", "skeram", AhnQirajTemple::NPC_PROPHET_SKERAM) {}
    };

    class SkeramEndFightTrigger : public EndBossFightTrigger
    {
    public:
        SkeramEndFightTrigger(PlayerbotAI* ai)
            : EndBossFightTrigger(ai, "end skeram fight", "skeram", AhnQirajTemple::NPC_PROPHET_SKERAM) {}
    };

    class SkeramImagesActiveTrigger : public Trigger
    {
    public:
        SkeramImagesActiveTrigger(PlayerbotAI* ai) : Trigger(ai, "skeram images active", 1) {}
        bool IsActive() override;
    };

    class SkeramNoImagesActiveTrigger : public Trigger
    {
    public:
        SkeramNoImagesActiveTrigger(PlayerbotAI* ai) : Trigger(ai, "skeram no images active", 1) {}
        bool IsActive() override;
    };

    class AhnQirajTempleTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        AhnQirajTempleTriggerContext()
        {
            creators["enter aq40"] = [](PlayerbotAI* ai) { return new AhnQirajTempleEnterDungeonTrigger(ai); };
            creators["leave aq40"] = [](PlayerbotAI* ai) { return new AhnQirajTempleLeaveDungeonTrigger(ai); };

            creators["start skeram fight"] = [](PlayerbotAI* ai) { return new SkeramStartFightTrigger(ai); };
            creators["end skeram fight"] = [](PlayerbotAI* ai) { return new SkeramEndFightTrigger(ai); };
            creators["skeram images active"] = [](PlayerbotAI* ai) { return new SkeramImagesActiveTrigger(ai); };
            creators["skeram no images active"] = [](PlayerbotAI* ai) { return new SkeramNoImagesActiveTrigger(ai); };
        }
    };
}

#pragma once

#include "playerbot/strategy/Multiplier.h"

class Action;

namespace ai
{
    class SkeramDamageControlMultiplier : public Multiplier
    {
    public:
        SkeramDamageControlMultiplier(PlayerbotAI* ai) : Multiplier(ai, "skeram damage control") {}
        float GetValue(Action* action) override;

    private:
        bool IsBlockedRealSkeramAction(Action* action) const;
    };

    class SkeramControlledPlayerDamageMultiplier : public Multiplier
    {
    public:
        SkeramControlledPlayerDamageMultiplier(PlayerbotAI* ai) : Multiplier(ai, "skeram controlled player damage control") {}
        float GetValue(Action* action) override;

    private:
        bool IsAllowedControlledPlayerAction(Action* action) const;
    };

    class SkeramTankAnchorMultiplier : public Multiplier
    {
    public:
        SkeramTankAnchorMultiplier(PlayerbotAI* ai) : Multiplier(ai, "skeram tank anchor") {}
        float GetValue(Action* action) override;

    private:
        bool IsBlockedAnchorBreakingAction(Action* action) const;
    };
}

#pragma once

#include "playerbot/strategy/Multiplier.h"

#include <string>

class Action;

namespace ai
{
    class SuppressionRoomPassiveMultiplier : public Multiplier
    {
    public:
        SuppressionRoomPassiveMultiplier(PlayerbotAI* ai) : Multiplier(ai, "suppression room passive") {}
        float GetValue(Action* action) override;
    };

    class RazorgoreCrowdControlMultiplier : public Multiplier
    {
    public:
        RazorgoreCrowdControlMultiplier(PlayerbotAI* ai) : Multiplier(ai, "razorgore suppress crowd control") {}
        float GetValue(Action* action) override;
    };

    class RazorgoreEggPhaseMovementMultiplier : public Multiplier
    {
    public:
        RazorgoreEggPhaseMovementMultiplier(PlayerbotAI* ai) : Multiplier(ai, "razorgore egg phase movement") {}
        float GetValue(Action* action) override;
    };
}

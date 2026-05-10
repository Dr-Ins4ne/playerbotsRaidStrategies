#pragma once

#include "AhnQirajTempleDungeonUtils.h"
#include "../actions/GenericActions.h"
#include "../actions/GenericSpellActions.h"
#include "playerbot/strategy/Multiplier.h"

#include <set>
#include <string>

class Action;

namespace ai
{
    class SkeramDamageControlMultiplier : public Multiplier
    {
    public:
        SkeramDamageControlMultiplier(PlayerbotAI* ai)
            : Multiplier(ai, "skeram damage control") {}

        float GetValue(Action* action) override;

    private:
        bool IsBlockedRealSkeramAction(Action* action) const;
    };
}

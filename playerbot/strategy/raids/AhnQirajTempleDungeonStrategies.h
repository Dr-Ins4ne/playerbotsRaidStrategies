#pragma once

#include "playerbot/strategy/Strategy.h"

namespace ai
{
    class AhnQirajTempleDungeonStrategy : public Strategy
    {
    public:
        AhnQirajTempleDungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "ahnqiraj temple"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class SkeramFightStrategy : public Strategy
    {
    public:
        SkeramFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "skeram"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitMultipliers(std::list<Multiplier*>& multipliers) override;
    };
}

#pragma once

#include "../generic/DungeonStrategy.h"
#include "../raids/DungeonBossLifecycle.h"

namespace ai
{
    class BlackwingLairDungeonStrategy : public Strategy
    {
    public:
        BlackwingLairDungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "blackwing lair"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RazorgoreFightStrategy : public DungeonBossStrategy
    {
    public:
        RazorgoreFightStrategy(PlayerbotAI* ai)
            : DungeonBossStrategy(ai, "razorgore") {}

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;

    };

    class VaelastraszFightStrategy : public DungeonBossStrategy
    {
    public:
        VaelastraszFightStrategy(PlayerbotAI* ai)
            : DungeonBossStrategy(ai, "vaelastrasz") {}

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class BroodlordFightStrategy : public DungeonBossStrategy
    {
    public:
        BroodlordFightStrategy(PlayerbotAI* ai)
            : DungeonBossStrategy(ai, "broodlord") {}

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };

    class SuppressionRoomStrategy : public Strategy
    {
    public:
        SuppressionRoomStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "suppression room"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
        void InitNonCombatMultipliers(std::list<Multiplier*>& multipliers) override;
        void OnStrategyAdded(BotState state) override;
    };
}
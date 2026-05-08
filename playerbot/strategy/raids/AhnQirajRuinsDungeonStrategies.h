#pragma once
#include "playerbot/strategy/Strategy.h"
#include "playerbot/strategy/Value.h"
#include "playerbot/strategy/Multiplier.h"

namespace ai
{
    // General Ahn'Qiraj (AQ20) Dungeon Strategy
    class AhnQirajRuinsDungeonStrategy : public Strategy
    {
    public:
        AhnQirajRuinsDungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "ahnqiraj ruins"; }
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // Kurinnaxx Fight Specific Strategy
    class KurinnaxxFightStrategy : public Strategy
    {
    public:
        KurinnaxxFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "kurinnaxx"; }
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class BuruFightStrategy : public Strategy
    {
    public:
        BuruFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "buru"; }

        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class AyamissFightStrategy : public Strategy
    {
    public:
        AyamissFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "ayamiss"; }

        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class OssirianFightStrategy : public Strategy
    {
    public:
        OssirianFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "ossirian"; }

        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
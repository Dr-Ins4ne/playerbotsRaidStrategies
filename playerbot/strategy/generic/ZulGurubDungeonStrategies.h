#pragma once
#include "playerbot/strategy/Strategy.h"

namespace ai
{
    class ZulGurubDungeonStrategy : public Strategy
    {
    public:
        ZulGurubDungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "zul gurub"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };


    class HighPriestessJeklikFightStrategy : public Strategy
    {
    public:
        HighPriestessJeklikFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "high priestess jeklik"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class HighPriestVenoxisFightStrategy : public Strategy
    {
    public:
        HighPriestVenoxisFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "high priest venoxis"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class HighPriestessMarliFightStrategy : public Strategy
    {
    public:
        HighPriestessMarliFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "high priestess mar'li"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class BloodlordMandokirFightStrategy : public Strategy
    {
    public:
        BloodlordMandokirFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "bloodlord mandokir"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class GahzrankaFightStrategy : public Strategy
    {
    public:
        GahzrankaFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "gahz'ranka"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class HighPriestThekalFightStrategy : public Strategy
    {
    public:
        HighPriestThekalFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "thekal"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class HighPriestessArlokkFightStrategy : public Strategy
    {
    public:
        HighPriestessArlokkFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "high priestess arlokk"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class JindoTheHexxerFightStrategy : public Strategy
    {
    public:
        JindoTheHexxerFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "jin'do the hexxer"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class HakkarFightStrategy : public Strategy
    {
    public:
        HakkarFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "hakkar"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    // Optional Edge of Madness bosses

    class GrilekFightStrategy : public Strategy
    {
    public:
        GrilekFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "gri'lek"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class HazzarahFightStrategy : public Strategy
    {
    public:
        HazzarahFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "hazza'rah"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class RenatakiFightStrategy : public Strategy
    {
    public:
        RenatakiFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "renataki"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };


    class WushoolayFightStrategy : public Strategy
    {
    public:
        WushoolayFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        std::string getName() override { return "wushoolay"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers) override;
    };
}
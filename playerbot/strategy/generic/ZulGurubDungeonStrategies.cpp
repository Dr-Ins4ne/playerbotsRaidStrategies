#include "playerbot/playerbot.h"
#include "ZulGurubDungeonStrategies.h"
#include "DungeonMultipliers.h"

using namespace ai;

void ZulGurubDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start jeklik fight",
        NextAction::array(0, new NextAction("enable jeklik fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start high priest venoxis fight",
        NextAction::array(0, new NextAction("enable high priest venoxis fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start high priestess mar'li fight",
        NextAction::array(0, new NextAction("enable high priestess mar'li fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start bloodlord mandokir fight",
        NextAction::array(0, new NextAction("enable bloodlord mandokir fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start gahz'ranka fight",
        NextAction::array(0, new NextAction("enable gahz'ranka fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start thekal fight",
        NextAction::array(0,
            new NextAction("enable thekal fight strategy", 100.0f),
            new NextAction("mark thekal targets", 90.0f),
            NULL)));

    triggers.push_back(new TriggerNode(
        "start high priestess arlokk fight",
        NextAction::array(0, new NextAction("enable high priestess arlokk fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start jin'do the hexxer fight",
        NextAction::array(0, new NextAction("enable jin'do the hexxer fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start hakkar fight",
        NextAction::array(0, new NextAction("enable hakkar fight strategy", 100.0f), NULL)));

    // Optional Edge of Madness bosses

    triggers.push_back(new TriggerNode(
        "start gri'lek fight",
        NextAction::array(0, new NextAction("enable gri'lek fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start hazza'rah fight",
        NextAction::array(0, new NextAction("enable hazza'rah fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start renataki fight",
        NextAction::array(0, new NextAction("enable renataki fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start wushoolay fight",
        NextAction::array(0, new NextAction("enable wushoolay fight strategy", 100.0f), NULL)));
}

void ZulGurubDungeonStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No generic Zul'Gurub non-combat triggers yet.
}


// High Priestess Jeklik Fight Strategy

void HighPriestessJeklikFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "jeklik casting",
        NextAction::array(0, new NextAction("interrupt jeklik", 300.0f), NULL)));
}


void HighPriestessJeklikFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end jeklik fight",
        NextAction::array(0, new NextAction("disable jeklik fight strategy", 100.0f), NULL)));
}

void HighPriestessJeklikFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end jeklik fight",
        NextAction::array(0, new NextAction("disable jeklik fight strategy", 100.0f), NULL)));
}

void HighPriestessJeklikFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "jeklik casting",
        NextAction::array(0, new NextAction("interrupt jeklik", 300.0f), NULL)));
}

void HighPriestessJeklikFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// High Priest Venoxis Fight Strategy

void HighPriestVenoxisFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void HighPriestVenoxisFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end high priest venoxis fight",
        NextAction::array(0, new NextAction("disable high priest venoxis fight strategy", 100.0f), NULL)));
}

void HighPriestVenoxisFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end high priest venoxis fight",
        NextAction::array(0, new NextAction("disable high priest venoxis fight strategy", 100.0f), NULL)));
}

void HighPriestVenoxisFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "venoxis poison cloud",
        NextAction::array(0, new NextAction("move away from hazard", 100.0f), NULL)));
}

void HighPriestVenoxisFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// High Priestess Mar'li Fight Strategy

void HighPriestessMarliFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void HighPriestessMarliFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end high priestess mar'li fight",
        NextAction::array(0, new NextAction("disable high priestess mar'li fight strategy", 100.0f), NULL)));
}

void HighPriestessMarliFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end high priestess mar'li fight",
        NextAction::array(0, new NextAction("disable high priestess mar'li fight strategy", 100.0f), NULL)));
}

void HighPriestessMarliFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void HighPriestessMarliFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Bloodlord Mandokir Fight Strategy

void BloodlordMandokirFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void BloodlordMandokirFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end bloodlord mandokir fight",
        NextAction::array(0, new NextAction("disable bloodlord mandokir fight strategy", 100.0f), NULL)));
}

void BloodlordMandokirFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end bloodlord mandokir fight",
        NextAction::array(0, new NextAction("disable bloodlord mandokir fight strategy", 100.0f), NULL)));
}

void BloodlordMandokirFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void BloodlordMandokirFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Gahz'ranka Fight Strategy

void GahzrankaFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();
    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        triggers.push_back(new TriggerNode(
            "gahz'ranka too close",
            NextAction::array(0, new NextAction("move away from gahz'ranka", 100.0f), NULL)));
    }
}

void GahzrankaFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end gahz'ranka fight",
        NextAction::array(0, new NextAction("disable gahz'ranka fight strategy", 100.0f), NULL)));
}

void GahzrankaFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end gahz'ranka fight",
        NextAction::array(0, new NextAction("disable gahz'ranka fight strategy", 100.0f), NULL)));
}

void GahzrankaFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void GahzrankaFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    Player* bot = ai->GetBot();
    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        multipliers.push_back(new PreventMoveAwayFromCreatureOnReachToCastMultiplier(ai));
    }
}


// High Priest Thekal Fight Strategy
void HighPriestThekalFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // Keep raid icons updated.
    triggers.push_back(new TriggerNode(
        "thekal targets need marking",
        NextAction::array(0, new NextAction("mark thekal targets", 300.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "lorkhan casting",
        NextAction::array(0, new NextAction("interrupt lorkhan", 290.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "lorkhan needs curse of tongues",
        NextAction::array(0, new NextAction("curse of tongues lorkhan", 260.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "thekal tiger alive",
        NextAction::array(0, new NextAction("select thekal tiger rti", 220.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "thekal trio broken",
        NextAction::array(0, new NextAction("select finish thekal rti", 215.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "thekal trio ready to finish",
        NextAction::array(0, new NextAction("select finish thekal rti", 210.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "thekal trio needs balance",
        NextAction::array(0, new NextAction("select balanced thekal rti", 120.0f), NULL)));
}

void HighPriestThekalFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end thekal fight",
        NextAction::array(0, new NextAction("disable thekal fight strategy", 100.0f), NULL)));
}

void HighPriestThekalFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end thekal fight",
        NextAction::array(0, new NextAction("disable thekal fight strategy", 100.0f), NULL)));
}

void HighPriestThekalFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "lorkhan casting",
        NextAction::array(0, new NextAction("interrupt lorkhan", 300.0f), NULL)));
}

void HighPriestThekalFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
}

// High Priestess Arlokk Fight Strategy

void HighPriestessArlokkFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void HighPriestessArlokkFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end high priestess arlokk fight",
        NextAction::array(0, new NextAction("disable high priestess arlokk fight strategy", 100.0f), NULL)));
}

void HighPriestessArlokkFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end high priestess arlokk fight",
        NextAction::array(0, new NextAction("disable high priestess arlokk fight strategy", 100.0f), NULL)));
}

void HighPriestessArlokkFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void HighPriestessArlokkFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Jin'do the Hexxer Fight Strategy

void JindoTheHexxerFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void JindoTheHexxerFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end jin'do the hexxer fight",
        NextAction::array(0, new NextAction("disable jin'do the hexxer fight strategy", 100.0f), NULL)));
}

void JindoTheHexxerFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end jin'do the hexxer fight",
        NextAction::array(0, new NextAction("disable jin'do the hexxer fight strategy", 100.0f), NULL)));
}

void JindoTheHexxerFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void JindoTheHexxerFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Hakkar Fight Strategy

void HakkarFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void HakkarFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end hakkar fight",
        NextAction::array(0, new NextAction("disable hakkar fight strategy", 100.0f), NULL)));
}

void HakkarFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end hakkar fight",
        NextAction::array(0, new NextAction("disable hakkar fight strategy", 100.0f), NULL)));
}

void HakkarFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void HakkarFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Gri'lek Fight Strategy

void GrilekFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void GrilekFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end gri'lek fight",
        NextAction::array(0, new NextAction("disable gri'lek fight strategy", 100.0f), NULL)));
}

void GrilekFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end gri'lek fight",
        NextAction::array(0, new NextAction("disable gri'lek fight strategy", 100.0f), NULL)));
}

void GrilekFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void GrilekFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Hazza'rah Fight Strategy

void HazzarahFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void HazzarahFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end hazza'rah fight",
        NextAction::array(0, new NextAction("disable hazza'rah fight strategy", 100.0f), NULL)));
}

void HazzarahFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end hazza'rah fight",
        NextAction::array(0, new NextAction("disable hazza'rah fight strategy", 100.0f), NULL)));
}

void HazzarahFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void HazzarahFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Renataki Fight Strategy

void RenatakiFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void RenatakiFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end renataki fight",
        NextAction::array(0, new NextAction("disable renataki fight strategy", 100.0f), NULL)));
}

void RenatakiFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end renataki fight",
        NextAction::array(0, new NextAction("disable renataki fight strategy", 100.0f), NULL)));
}

void RenatakiFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void RenatakiFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


// Wushoolay Fight Strategy

void WushoolayFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void WushoolayFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end wushoolay fight",
        NextAction::array(0, new NextAction("disable wushoolay fight strategy", 100.0f), NULL)));
}

void WushoolayFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end wushoolay fight",
        NextAction::array(0, new NextAction("disable wushoolay fight strategy", 100.0f), NULL)));
}

void WushoolayFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void WushoolayFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}
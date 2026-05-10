#include "playerbot/playerbot.h"
#include "AhnQirajRuinsDungeonStrategies.h"

#include "../triggers/DungeonTriggers.h"   // For StartBossFightTrigger, EndBossFightTrigger
#include "../actions/ChangeStrategyAction.h" // For ChangeAllStrategyAction

// Note: In "old style", you'd typically manually add entries.
// For this example, I'll still use the concepts of the custom triggers/actions.

using namespace ai;

// --- AhnQirajDungeonStrategy Implementation ---

void AhnQirajRuinsDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start kurinnaxx fight",
        NextAction::array(0, new NextAction("enable kurinnaxx strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start buru fight",
        NextAction::array(0, new NextAction("enable buru strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start ayamiss fight",
        NextAction::array(0, new NextAction("enable ayamiss strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "start ossirian fight",
        NextAction::array(0, new NextAction("enable ossirian strategy", 100.0f), NULL)));
}


void AhnQirajRuinsDungeonStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific general AQ20 non-combat triggers for the overall dungeon strategy currently
    // (e.g., if there were specific raid-wide mechanics outside of boss fights)
}

void AhnQirajRuinsDungeonStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific general AQ20 dead triggers for the overall dungeon strategy currently
}



// ------------------------------------------------------------
// Kurinnaxx
// ------------------------------------------------------------

void KurinnaxxFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "kurinnaxx mortal wound high",
        NextAction::array(0,
            new NextAction("taunt kurinnaxx", 95.0f),
            new NextAction("kurinnaxx tank retreat", 90.0f),
            NULL)));
}

void KurinnaxxFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "kurinnaxx sand trap close",
        NextAction::array(0, new NextAction("move away from kurinnaxx sand trap", 100.0f), NULL)));
}

void KurinnaxxFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end kurinnaxx fight",
        NextAction::array(0, new NextAction("disable kurinnaxx strategy", 100.0f), NULL)));
}

void KurinnaxxFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end kurinnaxx fight",
        NextAction::array(0, new NextAction("disable kurinnaxx strategy", 100.0f), NULL)));
}

// ------------------------------------------------------------
// Buru
// ------------------------------------------------------------

void BuruFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // Kited player must always keep moving to the next egg.
    triggers.push_back(new TriggerNode(
        "buru focused me",
        NextAction::array(0,
            new NextAction("move to buru egg", 100.0f),
            new NextAction("mark nearest buru egg", 99.0f),
            NULL)));

    // Adds first.
    triggers.push_back(new TriggerNode(
        "buru add alive",
        NextAction::array(0,
            new NextAction("select buru add", 60.0f),
            new NextAction("attack rti target", 59.0f),
            NULL)));

    // Phase 2: burn boss.
    triggers.push_back(new TriggerNode(
        "buru shell broken",
        NextAction::array(0,
            new NextAction("select buru boss", 40.0f),
            new NextAction("attack rti target", 39.0f),
            NULL)));

    // Only focus eggs when no hatchling is alive.
    triggers.push_back(new TriggerNode(
        "buru egg available",
        NextAction::array(0,
            new NextAction("select buru egg", 35.0f),
            new NextAction("attack rti target", 34.0f),
            NULL)));
}

void BuruFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end buru fight",
        NextAction::array(0, new NextAction("disable buru strategy", 100.0f), NULL)));
}

void BuruFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end buru fight",
        NextAction::array(0, new NextAction("disable buru strategy", 100.0f), NULL)));
}

// ------------------------------------------------------------
// Ayamiss
// ------------------------------------------------------------

void AyamissFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // Sacrifice/altar teleport recovery: the bot itself is on the altar and must run out.
    triggers.push_back(new TriggerNode(
        "bot on ayamiss altar",
        NextAction::array(0,
            new NextAction("move out of ayamiss altar", 120.0f),
            NULL)));

    triggers.push_back(new TriggerNode(
        "ayamiss warlock tank needed",
        NextAction::array(0,
            new NextAction("warlock tank ayamiss", 105.0f),
            NULL)));

    triggers.push_back(new TriggerNode(
        "ayamiss melee larva target needed",
        NextAction::array(0,
            new NextAction("select ayamiss larva", 100.0f),
            new NextAction("attack rti target", 99.0f),
            NULL)));

    triggers.push_back(new TriggerNode(
        "ayamiss boss available",
        NextAction::array(0,
            new NextAction("select ayamiss boss", 80.0f),
            new NextAction("attack rti target", 79.0f),
            NULL)));

    triggers.push_back(new TriggerNode(
        "ayamiss poison stinger high",
        NextAction::array(0,
            new NextAction("ayamiss stinger retreat", 90.0f),
            NULL)));
}

void AyamissFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end ayamiss fight",
        NextAction::array(0, new NextAction("disable ayamiss strategy", 100.0f), NULL)));
}

void AyamissFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end ayamiss fight",
        NextAction::array(0, new NextAction("disable ayamiss strategy", 100.0f), NULL)));
}

// ------------------------------------------------------------
// Ossirian
// ------------------------------------------------------------

void OssirianFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // If a crystal is close enough, click it immediately.
    triggers.push_back(new TriggerNode(
        "ossirian crystal close",
        NextAction::array(0,
            new NextAction("use ossirian crystal", 100.0f),
            NULL)));

    // Tank drags Ossirian to the next crystal whenever the Strength buff is active.
    triggers.push_back(new TriggerNode(
        "ossirian needs crystal",
        NextAction::array(0,
            new NextAction("move to ossirian crystal", 95.0f),
            new NextAction("select ossirian boss", 80.0f),
            NULL)));
}

void OssirianFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end ossirian fight",
        NextAction::array(0, new NextAction("disable ossirian strategy", 100.0f), NULL)));
}

void OssirianFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end ossirian fight",
        NextAction::array(0, new NextAction("disable ossirian strategy", 100.0f), NULL)));
}
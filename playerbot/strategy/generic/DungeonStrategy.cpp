
#include "playerbot/playerbot.h"
#include "DungeonStrategy.h"

using namespace ai;

void DungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // Add this combat triggers in case the bot gets summoned into the dungeon and goes straight into combat
    triggers.push_back(new TriggerNode(
        "enter onyxia lair",
        NextAction::array(0, new NextAction("enable onyxia lair strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter molten core",
        NextAction::array(0, new NextAction("enable molten core strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter blackwing lair",
        NextAction::array(0, new NextAction("enable blackwing lair strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter karazhan",
        NextAction::array(0, new NextAction("enable karazhan strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter zul gurub",
        NextAction::array(0, new NextAction("enable zul gurub strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter aq20",
        NextAction::array(0, new NextAction("enable ahnqiraj ruins strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter aq40",
        NextAction::array(0, new NextAction("enable ahnqiraj temple strategy", 100.0f), NULL)));
}

void DungeonStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "enter onyxia lair",
        NextAction::array(0, new NextAction("enable onyxia lair strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "leave onyxia lair",
        NextAction::array(0, new NextAction("disable onyxia lair strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter molten core",
        NextAction::array(0, new NextAction("enable molten core strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "leave molten core",
        NextAction::array(0, new NextAction("disable molten core strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter blackwing lair",
        NextAction::array(0, new NextAction("enable blackwing lair strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "leave blackwing lair",
        NextAction::array(0, new NextAction("disable blackwing lair strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter karazhan",
        NextAction::array(0, new NextAction("enable karazhan strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "leave karazhan",
        NextAction::array(0, new NextAction("disable karazhan strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "enter zul gurub",
        NextAction::array(0, new NextAction("enable zul gurub strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "leave zul gurub",
        NextAction::array(0, new NextAction("disable zul gurub strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter aq20",
        NextAction::array(0, new NextAction("enable ahnqiraj ruins strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "leave aq20",
        NextAction::array(0, new NextAction("disable ahnqiraj ruins strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enter aq40",
        NextAction::array(0, new NextAction("enable ahnqiraj temple strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "leave aq40",
        NextAction::array(0, new NextAction("disable ahnqiraj temple strategy", 100.0f), NULL)));
}
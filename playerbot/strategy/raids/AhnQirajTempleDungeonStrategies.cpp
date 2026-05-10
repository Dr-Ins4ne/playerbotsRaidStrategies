#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonStrategies.h"
#include "AhnQirajTempleDungeonMultipliers.h"

using namespace ai;

void AhnQirajTempleDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start skeram fight",
        NextAction::array(0, new NextAction("enable skeram fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "end skeram fight",
        NextAction::array(0, new NextAction("disable skeram fight strategy", 100.0f), NULL)));
}

void AhnQirajTempleDungeonStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end skeram fight",
        NextAction::array(0, new NextAction("disable skeram fight strategy", 100.0f), NULL)));
}

void SkeramFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // Split phase: immediately stop long casts, rewrite kill icons, and force DPS onto images.
    triggers.push_back(new TriggerNode(
        "skeram images active",
        NextAction::array(0,
            new NextAction("interrupt current spell", 100.0f),
            new NextAction("skeram mark images", 99.0f),
            new NextAction("skeram select image target", 98.0f),
            new NextAction("attack rti target", 97.0f),
            NULL)));

    // Normal phase: keep the real boss marked and only let the multiplier allow wand/white melee damage.
    triggers.push_back(new TriggerNode(
        "skeram no images active",
        NextAction::array(0,
            new NextAction("skeram mark real", 80.0f),
            new NextAction("skeram select real target", 79.0f),
            new NextAction("attack rti target", 78.0f),
            new NextAction("shoot", 50.0f),
            new NextAction("melee", 50.0f),
            NULL)));
}

void SkeramFightStrategy::InitMultipliers(std::list<Multiplier*>& multipliers)
{
    multipliers.push_back(new SkeramDamageControlMultiplier(ai));
}

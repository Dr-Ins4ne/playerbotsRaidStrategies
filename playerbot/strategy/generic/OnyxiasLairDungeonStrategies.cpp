#include "playerbot/playerbot.h"
#include "OnyxiasLairDungeonStrategies.h"
#include "DungeonMultipliers.h"

using namespace ai;

void OnyxiasLairDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start onyxia fight",
        NextAction::array(0, new NextAction("enable onyxia fight strategy", 100.0f), NULL)));
}




// Onyxia Fight Strategy

void OnyxiaFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();

    // Whelps are the highest target priority.
    triggers.push_back(new TriggerNode(
        "onyxia whelp alive",
        NextAction::array(0, new NextAction("target onyxia whelp", 250.0f), NULL)));

    // Ranged and healers should not stand too close to Onyxia.
    // This is not Shadow Flame avoidance; it is only basic boss-spacing.
    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        triggers.push_back(new TriggerNode(
            "onyxia too close",
            NextAction::array(0, new NextAction("move away from onyxia", 180.0f), NULL)));
    }

    // During flying phase, ranged DPS can keep Onyxia targeted.
    // Healers should not be forced to target her.
    if (ai->IsRanged(bot) && !ai->IsHeal(bot))
    {
        triggers.push_back(new TriggerNode(
            "onyxia flying",
            NextAction::array(0, new NextAction("target onyxia", 120.0f), NULL)));
    }

    // Default fallback target.
    triggers.push_back(new TriggerNode(
        "onyxia alive",
        NextAction::array(0, new NextAction("target onyxia", 50.0f), NULL)));
}

void OnyxiaFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end onyxia fight",
        NextAction::array(0, new NextAction("disable onyxia fight strategy", 100.0f), NULL)));
}

void OnyxiaFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end onyxia fight",
        NextAction::array(0, new NextAction("disable onyxia fight strategy", 100.0f), NULL)));
}

void OnyxiaFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // Intentionally empty.
    // No Shadow Flame fear/avoidance logic.
    // No Deep Breath movement logic.
}

void OnyxiaFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    Player* bot = ai->GetBot();

    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        multipliers.push_back(new PreventMoveAwayFromCreatureOnReachToCastMultiplier(ai));
    }
}


#include "playerbot/playerbot.h"
#include "OnyxiasLairDungeonStrategies.h"
#include "../generic/DungeonMultipliers.h"

using namespace ai;

void OnyxiasLairDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // Enable the Onyxia boss strategy once the boss fight starts.
    // The StartBossFightTrigger itself prevents repeated enabling while "+onyxia" is already active.
    triggers.push_back(new TriggerNode(
        "start onyxia fight",
        NextAction::array(0, new NextAction("enable onyxia fight strategy", 100.0f), NULL)));
}


// -----------------------------------------------------------------------------
// Onyxia Fight Strategy
// -----------------------------------------------------------------------------

void OnyxiaFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();
    if (!bot)
        return;
    triggers.push_back(new TriggerNode(
        "onyxia egg pit too close",
        NextAction::array(0, new NextAction("move out of onyxia egg pit", 110.0f), NULL)));
    // Phase 2:
    // Melee DPS and tanks cannot reliably attack flying Onyxia,
    // so they should switch to Onyxian Whelps.
    //
    // This trigger only fires if:
    // - Onyxia is flying
    // - bot is not ranged
    // - bot is not healer
    // - a whelp exists
    // - current target is not already a whelp
    if (!ai->IsRanged(bot) && !ai->IsHeal(bot))
    {
        triggers.push_back(new TriggerNode(
            "onyxia flying melee needs whelp target",
            NextAction::array(0, new NextAction("target onyxia whelp", 110.0f), NULL)));
    }

    // Phase 2:
    // Ranged DPS should keep attacking Onyxia while she is flying.
    //
    // This trigger only fires if:
    // - Onyxia is flying
    // - bot is ranged DPS
    // - current target is not already Onyxia
    if (ai->IsRanged(bot) && !ai->IsHeal(bot))
    {
        triggers.push_back(new TriggerNode(
            "onyxia flying ranged needs onyxia target",
            NextAction::array(0, new NextAction("target onyxia", 90.0f), NULL)));
    }

    // Basic boss-spacing for ranged DPS and healers.
    // This is not Shadow Flame avoidance and not Deep Breath logic.
    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        triggers.push_back(new TriggerNode(
            "onyxia too close",
            NextAction::array(0, new NextAction("move away from onyxia", 180.0f), NULL)));
    }

    // Important:
    // Do NOT add an unconditional fallback like:
    //
    // "onyxia alive" -> "target onyxia"
    //
    // That causes repeated retargeting and can starve normal class combat actions.
    //
    // Also do NOT add an unconditional:
    //
    // "onyxia whelp alive" -> "target onyxia whelp"
    //
    // because that would force ranged DPS off Onyxia during phase 2.
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
    if (!bot)
        return;

    // Prevent "move away from onyxia" from immediately undoing a reach-to-cast movement.
    // This keeps ranged/healers from oscillating between "reach spell" and "move away".
    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        multipliers.push_back(new PreventMoveAwayFromCreatureOnReachToCastMultiplier(ai));
    }
}


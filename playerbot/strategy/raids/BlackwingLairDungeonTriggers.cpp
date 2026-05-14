#include "playerbot/playerbot.h"
#include "playerbot/strategy/AiObjectContext.h"
#include "BlackwingLairDungeonTriggers.h"

using namespace ai;

namespace
{
    std::list<GuidPosition> GetGuidPositionsValue(PlayerbotAI* ai, const std::string& valueName)
    {
        if (!ai)
            return std::list<GuidPosition>();

        AiObjectContext* context = ai->GetAiObjectContext();
        if (!context)
            return std::list<GuidPosition>();

        Value<std::list<GuidPosition>>* value = context->GetValue<std::list<GuidPosition>>(valueName);
        if (!value)
            return std::list<GuidPosition>();

        return value->Get();
    }

    std::list<GuidPosition> GetUsableSuppressionDevicesInSight(PlayerbotAI* ai)
    {
        return GetGuidPositionsValue(ai,
            "go usable filter::go trapped filter::entry filter::{gos in sight,suppression devices}");
    }

    std::list<GuidPosition> GetUsableSuppressionDevicesClose(PlayerbotAI* ai)
    {
        return GetGuidPositionsValue(ai,
            "go usable filter::go trapped filter::entry filter::{gos close,suppression devices}");
    }
}



bool RazorgoreControllerNeedsTargetTrigger::IsActive()
{
    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    Unit* controller = BlackwingLair::FindGrethokController(ai);
    if (!controller || !controller->IsAlive())
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");

    // This trigger must be a "needs target" trigger, not an "is alive" trigger.
    // Stop firing once the bot is actually targeting Grethok. Do not require the
    // raid icon to be successfully set here, because bots without icon authority
    // would otherwise enqueue this action forever.
    return !currentTarget || !currentTarget->IsAlive() ||
           currentTarget->GetObjectGuid() != controller->GetObjectGuid();
}



bool RazorgoreEggPhaseTrigger::IsActive()
{
    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    return BlackwingLair::IsRazorgoreEggPhase(ai);
}

bool RazorgoreFarFromBossTrigger::IsActive()
{
    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    if (!BlackwingLair::IsRazorgoreEggPhase(ai))
        return false;

    if (AI_VALUE2(bool, "moving", "self target"))
        return false;

    Unit* razorgore = BlackwingLair::FindRazorgore(ai);
    if (!razorgore)
        return false;

    return bot->GetDistance(razorgore) > BlackwingLair::RAZORGORE_EGG_PHASE_NEAR_DISTANCE;
}

bool VaelastraszTankNeedsPullPositionTrigger::IsActive()
{
    if (!ai->HasStrategy("vaelastrasz", BotState::BOT_STATE_COMBAT))
        return false;

    if (AI_VALUE2(bool, "moving", "self target"))
        return false;

    if (!BlackwingLair::FindVaelastrasz(ai))
        return false;

    if (!BlackwingLair::IsVaelastraszTargetingBot(ai))
        return false;

    return BlackwingLair::DistanceToVaelastraszPullPosition(bot) >
           BlackwingLair::VAELASTRASZ_POSITION_REACHED_DISTANCE;
}

bool VaelastraszRangedNearPullPositionTrigger::IsActive()
{
    if (!ai->HasStrategy("vaelastrasz", BotState::BOT_STATE_COMBAT))
        return false;

    if (AI_VALUE2(bool, "moving", "self target"))
        return false;

    if (!ai->IsRanged(bot))
        return false;

    if (!BlackwingLair::FindVaelastrasz(ai))
        return false;

    if (BlackwingLair::IsVaelastraszTargetingBot(ai))
        return false;

    if (BlackwingLair::DistanceToVaelastraszPullPosition(bot) >
        BlackwingLair::VAELASTRASZ_RANGED_PULL_POSITION_DISTANCE)
    {
        return false;
    }

    return BlackwingLair::DistanceToVaelastraszRangedPosition(bot) >
           BlackwingLair::VAELASTRASZ_POSITION_REACHED_DISTANCE;
}

bool BroodlordOutOfPositionTrigger::IsActive()
{
    if (!ai->HasStrategy("broodlord", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!ai->CanMove())
        return false;

    if (!BlackwingLair::FindBroodlord(ai))
        return false;

    const Action* lastAction = ai->GetLastExecutedAction(BotState::BOT_STATE_COMBAT);
    if (lastAction && lastAction->getName() == "move to broodlord stack position")
        return false;

    return !BlackwingLair::IsBotNearBroodlordStackPosition(ai);
}

bool SuppressionDeviceNeedStealthTrigger::IsActive()
{
    if (bot->getClass() != CLASS_ROGUE)
        return false;

    if (ai->HasAura("stealth", bot))
        return false;

    return !GetUsableSuppressionDevicesInSight(ai).empty();
}

bool SuppressionDeviceInSightTrigger::IsActive()
{
    if (bot->getClass() != CLASS_ROGUE)
        return false;

    if (AI_VALUE2(bool, "moving", "self target"))
        return false;

    std::list<GuidPosition> gosInSight = GetUsableSuppressionDevicesInSight(ai);
    std::list<GuidPosition> gosClose = GetUsableSuppressionDevicesClose(ai);

    return !gosInSight.empty() && gosClose.empty();
}

bool SuppressionDeviceCloseTrigger::IsActive()
{
    if (bot->getClass() != CLASS_ROGUE)
        return false;

    if (!bot->HasSpell(SPELL_DISARM_TRAP))
        return false;

    if (AI_VALUE2(bool, "moving", "self target"))
        return false;

    return !GetUsableSuppressionDevicesClose(ai).empty();
}

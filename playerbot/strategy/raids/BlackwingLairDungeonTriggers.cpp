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

bool RazorgoreStartFightTrigger::IsActive()
{
    if (ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    if (!ai->HasStrategy("blackwing lair", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot->IsInWorld() || bot->IsBeingTeleported() || bot->GetMapId() != BlackwingLair::MAP_ID)
        return false;

    if (!ai->IsStateActive(BotState::BOT_STATE_COMBAT))
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (currentTarget && currentTarget->IsAlive() &&
        (currentTarget->GetEntry() == BlackwingLair::NPC_GRETHOK_CONTROLLER ||
         currentTarget->GetEntry() == BlackwingLair::NPC_RAZORGORE))
    {
        return true;
    }

    std::list<ObjectGuid> attackers = AI_VALUE(std::list<ObjectGuid>, "attackers");
    for (const ObjectGuid& attackerGuid : attackers)
    {
        Unit* attacker = ai->GetUnit(attackerGuid);
        if (!attacker || !attacker->IsAlive())
            continue;

        if (attacker->GetEntry() == BlackwingLair::NPC_GRETHOK_CONTROLLER ||
            attacker->GetEntry() == BlackwingLair::NPC_RAZORGORE)
        {
            return true;
        }
    }

    return false;
}

bool RazorgoreEndFightTrigger::IsActive()
{
    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot->IsInWorld() || bot->IsBeingTeleported() || bot->GetMapId() != BlackwingLair::MAP_ID)
        return true;

    return !ai->IsStateActive(BotState::BOT_STATE_COMBAT);
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

bool VaelastraszStartFightTrigger::IsActive()
{
    if (ai->HasStrategy("vaelastrasz", BotState::BOT_STATE_COMBAT))
        return false;

    if (!ai->HasStrategy("blackwing lair", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot->IsInWorld() || bot->IsBeingTeleported() || bot->GetMapId() != BlackwingLair::MAP_ID)
        return false;

    if (!ai->IsStateActive(BotState::BOT_STATE_COMBAT))
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (currentTarget && currentTarget->IsAlive() &&
        currentTarget->GetEntry() == BlackwingLair::NPC_VAELASTRASZ)
    {
        return true;
    }

    std::list<ObjectGuid> attackers = AI_VALUE(std::list<ObjectGuid>, "attackers");
    for (const ObjectGuid& attackerGuid : attackers)
    {
        Unit* attacker = ai->GetUnit(attackerGuid);
        if (!attacker || !attacker->IsAlive())
            continue;

        if (attacker->GetEntry() == BlackwingLair::NPC_VAELASTRASZ)
            return true;
    }

    return false;
}

bool VaelastraszEndFightTrigger::IsActive()
{
    if (!ai->HasStrategy("vaelastrasz", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot->IsInWorld() || bot->IsBeingTeleported() || bot->GetMapId() != BlackwingLair::MAP_ID)
        return true;

    return !ai->IsStateActive(BotState::BOT_STATE_COMBAT);
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

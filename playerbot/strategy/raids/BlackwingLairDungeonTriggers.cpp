#include "playerbot/playerbot.h"
#include "BlackwingLairDungeonTriggers.h"
#include "BlackwingLairDungeonData.h"

using namespace ai;

bool RazorgoreStartFightTrigger::IsActive()
{
    if (ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    if (!ai->HasStrategy("blackwing lair", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot->IsInWorld() || bot->IsBeingTeleported() || bot->GetMapId() != BlackwingLair::MAP_ID)
        return false;

    // Grethok is the actual opening target; Razorgore may not be attacking yet.
    return BlackwingLair::IsRazorgoreFightVisible(ai);
}

bool RazorgoreEndFightTrigger::IsActive()
{
    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    if (!ai->IsStateActive(BotState::BOT_STATE_COMBAT))
        return true;

    return !BlackwingLair::IsRazorgoreFightVisible(ai);
}

bool RazorgoreControllerAliveTrigger::IsActive()
{
    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    return BlackwingLair::FindGrethokController(ai) != nullptr;
}

bool RazorgoreEggPhaseTrigger::IsActive()
{
    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return false;

    return BlackwingLair::IsRazorgoreEggPhase(ai);
}

bool RazorgoreFarFromBossTrigger::IsActive()
{
    if (!RazorgoreEggPhaseTrigger::IsActive())
        return false;

    Unit* razorgore = BlackwingLair::FindRazorgore(ai);
    if (!razorgore)
        return false;

    return bot->GetDistance(razorgore) > BlackwingLair::RAZORGORE_EGG_PHASE_NEAR_DISTANCE;
}

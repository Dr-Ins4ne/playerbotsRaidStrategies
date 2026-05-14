#include "playerbot/playerbot.h"
#include "BlackwingLairDungeonMultipliers.h"
#include "BlackwingLairDungeonData.h"

using namespace ai;

float SuppressionRoomPassiveMultiplier::GetValue(Action* action)
{
    return 1.0f;
}

float RazorgoreCrowdControlMultiplier::GetValue(Action* action)
{
    return 1.0f;
}

float RazorgoreEggPhaseMovementMultiplier::GetValue(Action* action)
{
    return 1.0f;
}

float BroodlordSuppressFleeMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!ai->HasStrategy("broodlord", BotState::BOT_STATE_COMBAT))
        return 1.0f;

    if (!BlackwingLair::FindBroodlord(ai))
        return 1.0f;

    const std::string name = action->getName();

    if (name == "flee" ||
        name == "flee with pet" ||
        name == "flee to master" ||
        name == "runaway")
    {
        return 0.0f;
    }

    return 1.0f;
}

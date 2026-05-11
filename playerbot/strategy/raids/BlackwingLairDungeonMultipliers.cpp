#include "playerbot/playerbot.h"
#include "BlackwingLairDungeonMultipliers.h"

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

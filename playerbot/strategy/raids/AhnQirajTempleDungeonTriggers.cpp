#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonTriggers.h"

using namespace ai;

bool SkeramImagesActiveTrigger::IsActive()
{
    return AhnQirajTemple::HasSkeramImages(ai);
}

bool SkeramNoImagesActiveTrigger::IsActive()
{
    return AhnQirajTemple::GetRealSkeram(ai) && !AhnQirajTemple::HasSkeramImages(ai);
}

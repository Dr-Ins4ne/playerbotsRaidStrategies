#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonTriggers.h"
#include "DungeonTargetHelper.h"

using namespace ai;

bool SkeramImagesActiveTrigger::IsActive()
{
    std::vector<Unit*> images = AhnQirajTemple::GetSkeramImages(ai);
    if (images.empty())
        return false;

    Unit* real = AhnQirajTemple::GetRealSkeram(ai);

    // Fire if visual marking is wrong.
    if (real && !DungeonTargetHelper::HasCorrectTargetIcon(ai, "star", real))
        return true;

    if (images.size() >= 1 && !DungeonTargetHelper::HasCorrectTargetIcon(ai, "skull", images[0]))
        return true;

    if (images.size() >= 2 && !DungeonTargetHelper::HasCorrectTargetIcon(ai, "cross", images[1]))
        return true;

    // Fire if DPS RTI/current target is wrong.
    Unit* skull = DungeonTargetHelper::GetTargetIconUnit(ai, "skull");
    if (skull && skull->IsAlive())
        return DungeonTargetHelper::NeedsDpsRtiSelection(ai, "skull", skull);

    Unit* cross = DungeonTargetHelper::GetTargetIconUnit(ai, "cross");
    if (cross && cross->IsAlive())
        return DungeonTargetHelper::NeedsDpsRtiSelection(ai, "cross", cross);

    return false;
}

bool SkeramNoImagesActiveTrigger::IsActive()
{
    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (!real)
        return false;

    if (AhnQirajTemple::HasSkeramImages(ai))
        return false;

    // Fire if real Skeram is not skull.
    if (!DungeonTargetHelper::HasCorrectTargetIcon(ai, "skull", real))
        return true;

    // Fire if DPS RTI/current target is wrong.
    return DungeonTargetHelper::NeedsDpsRtiSelection(ai, "skull", real);
}
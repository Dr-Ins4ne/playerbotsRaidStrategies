#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonActions.h"

using namespace ai;

bool SkeramMarkImagesAction::isUseful()
{
    return AhnQirajTemple::HasSkeramImages(ai);
}

bool SkeramMarkImagesAction::Execute(Event& event)
{
    std::vector<Unit*> images = AhnQirajTemple::GetSkeramImages(ai);
    if (images.empty())
        return false;

    bool changed = false;

    // Keep a stable marker on the real boss so it is easy to see what is being ignored.
    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (real)
        changed |= SetTargetIcon("star", real);

    // Images must take kill icons immediately.
    changed |= SetTargetIconsForCreatures({ "skull", "cross" }, images);

    // DPS bots focus images only. Tanks/healers are intentionally skipped by SelectFirstAliveDpsRti().
    changed |= SelectFirstAliveDpsRti({ "skull", "cross" });

    return changed;
}

bool SkeramMarkRealAction::isUseful()
{
    return AhnQirajTemple::GetRealSkeram(ai) && !AhnQirajTemple::HasSkeramImages(ai);
}

bool SkeramMarkRealAction::Execute(Event& event)
{
    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (!real)
        return false;

    bool changed = false;
    changed |= SetTargetIcon("skull", real);
    changed |= SelectDpsRti("skull", real);
    return changed;
}

bool SkeramSelectImageTargetAction::isUseful()
{
    return AhnQirajTemple::HasSkeramImages(ai);
}

bool SkeramSelectImageTargetAction::Execute(Event& event)
{
    return SelectFirstAliveDpsRti({ "skull", "cross" });
}

bool SkeramSelectRealTargetAction::isUseful()
{
    return AhnQirajTemple::GetRealSkeram(ai) && !AhnQirajTemple::HasSkeramImages(ai);
}

bool SkeramSelectRealTargetAction::Execute(Event& event)
{
    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (!real)
        return false;

    return SelectDpsRti("skull", real);
}

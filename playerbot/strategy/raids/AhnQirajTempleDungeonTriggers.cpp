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

bool SkeramTankOutOfPositionTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    if (!ai->CanMove())
        return false;

    return !AhnQirajTemple::IsBotNearSkeramPullPosition(ai);
}

bool SkeramControlledTargetNeedsCcTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot)
        return false;

    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (!real)
        return false;

    return AhnQirajTemple::CanCcSkeramControlledTarget(ai, bot);
}

bool SkeramControlledPlayerTooCloseTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!ai->CanMove())
        return false;

    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (!real)
        return false;

    // The aggro holder must keep real Skeram anchored.
    if (AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    // Avoid movement spam.
    time_t now = time(0);
    if (lastMoveTime && now < lastMoveTime + 2)
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(
        ai,
        bot,
        AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE
    );

    if (!controlled)
        return false;

    lastMoveTime = now;
    return true;
}
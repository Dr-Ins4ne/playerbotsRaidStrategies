#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonTriggers.h"

using namespace ai;

bool SkeramImagesActiveTrigger::IsActive()
{
    std::vector<Unit*> images = AhnQirajTemple::GetSkeramImages(ai);
    if (images.empty())
        return false;

    Unit* real = AhnQirajTemple::GetRealSkeram(ai);

    if (real && !DungeonTargetHelper::HasCorrectTargetIcon(ai, "star", real))
        return true;

    Unit* skull = DungeonTargetHelper::GetTargetIconUnit(ai, "skull");
    Unit* cross = DungeonTargetHelper::GetTargetIconUnit(ai, "cross");

    bool skullIsImage = skull && skull->IsAlive() && AhnQirajTemple::IsSkeramImage(ai, skull);
    bool crossIsImage = cross && cross->IsAlive() && AhnQirajTemple::IsSkeramImage(ai, cross);

    // Do not enforce images[0] -> skull and images[1] -> cross. That causes icon flipping as health/order changes.
    // Only require a live skull image, and a distinct live cross image while two images are alive.
    if (!skullIsImage)
        return true;

    if (images.size() >= 2)
    {
        if (!crossIsImage)
            return true;

        if (cross->GetObjectGuid() == skull->GetObjectGuid())
            return true;
    }

    if (DungeonTargetHelper::NeedsDpsRtiSelection(ai, "skull", skull))
        return true;

    return false;
}

bool SkeramNoImagesActiveTrigger::IsActive()
{
    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (!real)
        return false;

    if (AhnQirajTemple::HasSkeramImages(ai))
        return false;

    if (!DungeonTargetHelper::HasCorrectTargetIcon(ai, "skull", real))
        return true;

    return DungeonTargetHelper::NeedsDpsRtiSelection(ai, "skull", real);
}

bool SkeramTankOutOfPositionTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!ai->CanMove())
        return false;

    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    return !AhnQirajTemple::IsBotNearSkeramPullPosition(ai);
}

bool SkeramControlledTargetNeedsCcTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot)
        return false;

    if (!AhnQirajTemple::GetRealSkeram(ai))
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

    if (!AhnQirajTemple::GetRealSkeram(ai))
        return false;

    if (AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(ai, bot, AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE);
    return controlled != nullptr;
}

bool SkeramTankTargetControlledPlayerTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot)
        return false;

    if (!ai->IsTank(bot) && !AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget)
        return false;

    return AhnQirajTemple::IsValidSkeramControlledTarget(ai, bot, currentTarget, false);
}

bool SkeramRtiTargetNotVisibleTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!ai->CanMove())
        return false;

    if (!AhnQirajTemple::GetRealSkeram(ai))
        return false;

    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(ai, bot, AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE);
    if (controlled)
        return false;

    Unit* rtiTarget = DungeonTargetHelper::GetRtiTarget(ai);
    if (!DungeonTargetHelper::IsValidUnitForSelection(ai, rtiTarget, true))
        return false;

    if (sServerFacade.IsWithinLOSInMap(bot, rtiTarget))
        return false;

    return !AhnQirajTemple::IsBotNearSkeramPullPosition(ai);
}


bool SkeramRangedPositionNotAssignedTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!AhnQirajTemple::GetRealSkeram(ai))
        return false;

    if (!AhnQirajTemple::IsSkeramRangedPositionBot(ai))
        return false;

    return !AhnQirajTemple::HasSkeramAssignedRangedPosition(bot);
}

bool SkeramRangedOutOfPositionTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!ai->CanMove())
        return false;

    if (!AhnQirajTemple::GetRealSkeram(ai))
        return false;

    if (!AhnQirajTemple::IsSkeramRangedPositionBot(ai))
        return false;

    if (AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(ai, bot, AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE);
    if (controlled)
        return false;

    return !AhnQirajTemple::IsBotNearSkeramAssignedRangedPosition(ai);
}

bool SkeramGreaterNatureProtectionPotionReadyTrigger::IsActive()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot)
        return false;

    if (!AhnQirajTemple::GetRealSkeram(ai))
        return false;

    return AhnQirajTemple::IsGreaterNatureProtectionPotionReady(ai, bot);
}

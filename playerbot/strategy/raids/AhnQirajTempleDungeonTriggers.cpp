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

    if (images.size() >= 1 && !DungeonTargetHelper::HasCorrectTargetIcon(ai, "skull", images[0]))
        return true;

    if (images.size() >= 2 && !DungeonTargetHelper::HasCorrectTargetIcon(ai, "cross", images[1]))
        return true;

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

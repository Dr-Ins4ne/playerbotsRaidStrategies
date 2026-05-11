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

    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (real)
        changed |= SetTargetIcon("star", real);

    auto isAliveImage = [this](Unit* unit) -> bool
    {
        return unit && unit->IsAlive() && AhnQirajTemple::IsSkeramImage(ai, unit);
    };

    Unit* skullTarget = GetTargetIconUnit("skull");
    Unit* crossTarget = GetTargetIconUnit("cross");

    if (!isAliveImage(skullTarget))
        skullTarget = nullptr;

    if (!isAliveImage(crossTarget))
        crossTarget = nullptr;

    if (skullTarget && crossTarget && skullTarget->GetObjectGuid() == crossTarget->GetObjectGuid())
        crossTarget = nullptr;

    // Sticky focus rule:
    // Keep the existing skull image while it is alive. Only promote/assign a new skull when skull died/disappeared.
    // This prevents DPS from bouncing between two active images just because health/order changed.
    if (!skullTarget)
    {
        if (crossTarget)
        {
            skullTarget = crossTarget;
            crossTarget = nullptr;
        }
        else if (!images.empty())
        {
            skullTarget = images[0];
        }
    }

    if (!crossTarget)
    {
        for (Unit* image : images)
        {
            if (!isAliveImage(image))
                continue;

            if (skullTarget && image->GetObjectGuid() == skullTarget->GetObjectGuid())
                continue;

            crossTarget = image;
            break;
        }
    }

    if (skullTarget)
        changed |= SetTargetIcon("skull", skullTarget);

    if (crossTarget)
        changed |= SetTargetIcon("cross", crossTarget);

    // All DPS focus skull until it dies, then cross is promoted by the next marking pass.
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

bool SkeramPullToTankPositionAction::isUseful()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    return !AhnQirajTemple::IsBotNearSkeramPullPosition(ai);
}

bool SkeramPullToTankPositionAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool SkeramPullToTankPositionAction::Execute(Event& event)
{
    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (real)
        DungeonTargetHelper::SetCurrentTarget(ai, real);

    if (AhnQirajTemple::IsBotNearSkeramPullPosition(ai))
        return false;

    const float distance = bot->GetDistance(AhnQirajTemple::SKERAM_PULL_X, AhnQirajTemple::SKERAM_PULL_Y, AhnQirajTemple::SKERAM_PULL_Z);

    if (MoveTo(bot->GetMapId(), AhnQirajTemple::SKERAM_PULL_X, AhnQirajTemple::SKERAM_PULL_Y, AhnQirajTemple::SKERAM_PULL_Z, false, IsReaction(), false, true))
    {
        if (IsReaction())
            WaitForReach(distance);

        return true;
    }

    return false;
}

bool SkeramMoveToPullPositionAction::isUseful()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    return !AhnQirajTemple::IsBotNearSkeramPullPosition(ai);
}

bool SkeramMoveToPullPositionAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool SkeramMoveToPullPositionAction::Execute(Event& event)
{
    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    if (AhnQirajTemple::IsBotNearSkeramPullPosition(ai))
        return false;

    const float distance = bot->GetDistance(AhnQirajTemple::SKERAM_PULL_X, AhnQirajTemple::SKERAM_PULL_Y, AhnQirajTemple::SKERAM_PULL_Z);

    if (MoveTo(bot->GetMapId(), AhnQirajTemple::SKERAM_PULL_X, AhnQirajTemple::SKERAM_PULL_Y, AhnQirajTemple::SKERAM_PULL_Z, false, IsReaction(), false, true))
    {
        if (IsReaction())
            WaitForReach(distance);

        return true;
    }

    return false;
}


bool SkeramAssignRangedPositionAction::isUseful()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!AhnQirajTemple::IsSkeramRangedPositionBot(ai))
        return false;

    return !AhnQirajTemple::HasSkeramAssignedRangedPosition(bot);
}

bool SkeramAssignRangedPositionAction::Execute(Event& event)
{
    if (!bot)
        return false;

    if (!AhnQirajTemple::IsSkeramRangedPositionBot(ai))
        return false;

    if (AhnQirajTemple::HasSkeramAssignedRangedPosition(bot))
        return false;

    return AhnQirajTemple::AssignSkeramRangedPosition(bot);
}


bool SkeramClearRangedPositionAction::isUseful()
{
    if (!bot)
        return false;

    return AhnQirajTemple::HasSkeramAssignedRangedPosition(bot);
}

bool SkeramClearRangedPositionAction::Execute(Event& event)
{
    if (!bot)
        return false;

    return AhnQirajTemple::ClearSkeramAssignedRangedPosition(bot);
}

bool SkeramMoveToRangedPositionAction::isUseful()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
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

bool SkeramMoveToRangedPositionAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool SkeramMoveToRangedPositionAction::Execute(Event& event)
{
    if (!isUseful())
        return false;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    if (!AhnQirajTemple::GetSkeramAssignedRangedPosition(bot, x, y, z))
        return false;

    const float distance = bot->GetDistance(x, y, z);

    if (MoveTo(bot->GetMapId(), x, y, z, false, IsReaction(), false, true))
    {
        if (IsReaction())
            WaitForReach(distance);

        return true;
    }

    return false;
}

bool CrowdControlSkeramControlledTargetAction::isPossible()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    return AhnQirajTemple::CanCcSkeramControlledTarget(ai, bot);
}

bool CrowdControlSkeramControlledTargetAction::Execute(Event& event)
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    return AhnQirajTemple::CastCcOnSkeramControlledTarget(ai, bot);
}

bool SkeramSelectControlledPlayerForFleeAction::isUseful()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(ai, bot, AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE);
    return controlled != nullptr;
}

bool SkeramSelectControlledPlayerForFleeAction::Execute(Event& event)
{
    if (!isUseful())
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(ai, bot, AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE);
    if (!controlled)
        return false;

    return DungeonTargetHelper::SetCurrentTarget(ai, controlled);
}

bool SkeramTankTargetRealAction::isUseful()
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

bool SkeramTankTargetRealAction::Execute(Event& event)
{
    if (!bot)
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");

    if (currentTarget && AhnQirajTemple::IsValidSkeramControlledTarget(ai, bot, currentTarget, false))
    {
        bot->AttackStop();

        Value<Unit*>* currentTargetValue = context->GetValue<Unit*>("current target");
        if (currentTargetValue)
            currentTargetValue->Set(nullptr);

        bot->SetSelectionGuid(ObjectGuid());
    }

    Unit* real = AhnQirajTemple::GetRealSkeram(ai);
    if (!real)
        return true;

    if (!sServerFacade.IsWithinLOSInMap(bot, real))
        return true;

    return DungeonTargetHelper::SetCurrentTarget(ai, real);
}

UseGreaterNatureProtectionPotionAction::UseGreaterNatureProtectionPotionAction(PlayerbotAI* ai) : UseItemIdAction(ai, "use greater nature protection potion")
{
    qualifier = "{13458}";
}

bool UseGreaterNatureProtectionPotionAction::isUseful()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot)
        return false;

    if (AhnQirajTemple::HasGreaterNatureProtection(ai, bot))
        return false;

    return UseItemIdAction::isUseful();
}

bool UseGreaterNatureProtectionPotionAction::isPossible()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!AhnQirajTemple::IsGreaterNatureProtectionPotionReady(ai, bot))
        return false;

    return UseItemIdAction::isPossible();
}

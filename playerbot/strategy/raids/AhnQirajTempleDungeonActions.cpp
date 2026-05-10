#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonActions.h"

using namespace ai;

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

bool MoveAwayFromSkeramControlledPlayerAction::isUseful()
{
    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return false;

    if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
        return false;

    if (!ai->CanMove())
        return false;

    // If this bot is currently holding real Skeram aggro, do not abandon the Skeram anchor.
    if (AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(
        ai,
        bot,
        AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE
    );

    return controlled != nullptr;
}

bool MoveAwayFromSkeramControlledPlayerAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool MoveAwayFromSkeramControlledPlayerAction::Execute(Event& event)
{
    if (!isUseful())
        return false;

    Unit* controlled = AhnQirajTemple::FindSkeramControlledTargetNearBot(
        ai,
        bot,
        AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE
    );

    if (!controlled)
        return false;

    WorldPosition botPosition(bot);
    WorldPosition controlledPosition(controlled);

    const float desiredDistance =
        AhnQirajTemple::SKERAM_CONTROLLED_PLAYER_AVOID_DISTANCE +
        bot->GetCombatReach() +
        controlled->GetCombatReach();

    const float baseAngle = controlledPosition.getAngleTo(botPosition);

    static const float angleOffsets[] =
    {
        0.0f,
        0.5f,
        -0.5f,
        1.0f,
        -1.0f,
        1.5f,
        -1.5f,
        3.1415926f
    };

    for (float offset : angleOffsets)
    {
        float angle = baseAngle + offset;

        WorldPosition point =
            controlledPosition +
            WorldPosition(
                0,
                desiredDistance * std::cos(angle),
                desiredDistance * std::sin(angle),
                1.0f
            );

        point.setZ(point.getHeight());

        if (!botPosition.canPathTo(point, bot))
            continue;

        if (!bot->IsWithinLOS(
                point.getX(),
                point.getY(),
                point.getZ() + bot->GetCollisionHeight()))
        {
            continue;
        }

        if (MoveTo(
                bot->GetMapId(),
                point.getX(),
                point.getY(),
                point.getZ(),
                false,
                IsReaction(),
                false,
                true))
        {
            if (IsReaction())
                WaitForReach(botPosition.distance(point));

            return true;
        }
    }

    return false;
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
    if (!real)
        return false;

    // Keep the aggro bot focused on real Skeram while dragging him.
    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget || currentTarget->GetObjectGuid() != real->GetObjectGuid())
    {
        context->GetValue<Unit*>("current target")->Set(real);
        bot->SetSelectionGuid(real->GetObjectGuid());
    }

    if (AhnQirajTemple::IsBotNearSkeramPullPosition(ai))
        return false;

    const float distance = bot->GetDistance(
        AhnQirajTemple::SKERAM_PULL_X,
        AhnQirajTemple::SKERAM_PULL_Y,
        AhnQirajTemple::SKERAM_PULL_Z
    );

    if (MoveTo(
        bot->GetMapId(),
        AhnQirajTemple::SKERAM_PULL_X,
        AhnQirajTemple::SKERAM_PULL_Y,
        AhnQirajTemple::SKERAM_PULL_Z,
        false,
        IsReaction(),
        false,
        true))
    {
        if (IsReaction())
            WaitForReach(distance);

        return true;
    }

    return false;
}

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

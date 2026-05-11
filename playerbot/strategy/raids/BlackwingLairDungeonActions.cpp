#include "playerbot/playerbot.h"
#include "BlackwingLairDungeonActions.h"

using namespace ai;

bool FocusRazorgoreControllerAction::Execute(Event& event)
{
    Unit* controller = BlackwingLair::FindGrethokController(ai);
    if (!controller)
        return false;

    bool changed = DungeonTargetHelper::SetCurrentTarget(ai, controller);

    bot->SetSelectionGuid(controller->GetObjectGuid());

    if (bot->CanReachWithMeleeAttack(controller))
        bot->Attack(controller, true);

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
        ai->TellPlayerNoFacing(GetMaster(), "Razorgore: focusing Grethok the Controller");

    return changed || DungeonTargetHelper::IsCurrentTarget(ai, controller);
}

bool FocusRazorgoreControllerAction::isUseful()
{
    Unit* controller = BlackwingLair::FindGrethokController(ai);
    if (!controller)
        return false;

    return !DungeonTargetHelper::IsCurrentTarget(ai, controller);
}

bool MoveNearRazorgoreAction::Execute(Event& event)
{
    Unit* razorgore = BlackwingLair::FindRazorgore(ai);
    if (!razorgore)
        return false;

    WorldPosition razorgorePos(razorgore);
    WorldPosition botPos(bot);

    // Deterministic per-bot ring placement. This keeps the group close to
    // Razorgore during the egg phase without making every bot stack on him.
    float normalized = float(bot->GetGUIDLow() % 6283) / 6283.0f;
    float angle = normalized * 2.0f * M_PI_F;
    float radiusSpread = float(bot->GetGUIDLow() % 1000) / 1000.0f;
    float radius = BlackwingLair::RAZORGORE_EGG_PHASE_RING_MIN +
                   radiusSpread * (BlackwingLair::RAZORGORE_EGG_PHASE_RING_MAX - BlackwingLair::RAZORGORE_EGG_PHASE_RING_MIN);

    WorldPosition point = razorgorePos + WorldPosition(0, radius * cos(angle), radius * sin(angle), 1.0f);
    point.setZ(point.getHeight());

    if (!bot->IsWithinLOS(point.getX(), point.getY(), point.getZ() + bot->GetCollisionHeight()) ||
        !razorgorePos.canPathTo(point, bot))
    {
        return MoveNear(razorgore, BlackwingLair::RAZORGORE_EGG_PHASE_NEAR_DISTANCE - 2.0f);
    }

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
        ai->TellPlayerNoFacing(GetMaster(), "Razorgore: moving near boss for egg phase");

    return MoveTo(bot->GetMapId(), point.getX(), point.getY(), point.getZ(), false, IsReaction(), false, true);
}

bool MoveNearRazorgoreAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool MoveNearRazorgoreAction::isUseful()
{
    if (!BlackwingLair::IsRazorgoreEggPhase(ai))
        return false;

    Unit* razorgore = BlackwingLair::FindRazorgore(ai);
    if (!razorgore)
        return false;

    return bot->GetDistance(razorgore) > BlackwingLair::RAZORGORE_EGG_PHASE_NEAR_DISTANCE;
}

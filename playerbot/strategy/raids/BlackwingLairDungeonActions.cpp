#include "playerbot/playerbot.h"
#include "playerbot/strategy/AiObjectContext.h"
#include "BlackwingLairDungeonActions.h"

#include <map>

using namespace ai;

namespace
{
    std::list<GuidPosition> GetGuidPositionsValue(PlayerbotAI* ai, const std::string& valueName)
    {
        if (!ai)
            return std::list<GuidPosition>();

        AiObjectContext* context = ai->GetAiObjectContext();
        if (!context)
            return std::list<GuidPosition>();

        Value<std::list<GuidPosition>>* value = context->GetValue<std::list<GuidPosition>>(valueName);
        if (!value)
            return std::list<GuidPosition>();

        return value->Get();
    }

    struct RazorgoreStrategySnapshot
    {
        bool hadWaitForAttack = false;
        bool hadThreat = false;
        bool hadPullBack = false;
    };

    std::map<uint32, RazorgoreStrategySnapshot> sRazorgoreStrategySnapshots;

    RazorgoreStrategySnapshot BuildRazorgoreStrategySnapshot(PlayerbotAI* ai)
    {
        RazorgoreStrategySnapshot snapshot;
        snapshot.hadWaitForAttack = ai->HasStrategy("wait for attack", BotState::BOT_STATE_COMBAT);
        snapshot.hadThreat = ai->HasStrategy("threat", BotState::BOT_STATE_COMBAT);
        snapshot.hadPullBack = ai->HasStrategy("pull back", BotState::BOT_STATE_COMBAT);
        return snapshot;
    }

    void RemoveRazorgoreDelayedEngagementStrategies(PlayerbotAI* ai, const RazorgoreStrategySnapshot& snapshot)
    {
        if (snapshot.hadWaitForAttack)
            ai->ChangeStrategy("-wait for attack", BotState::BOT_STATE_COMBAT);

        if (snapshot.hadThreat)
            ai->ChangeStrategy("-threat", BotState::BOT_STATE_COMBAT);

        if (snapshot.hadPullBack)
            ai->ChangeStrategy("-pull back", BotState::BOT_STATE_COMBAT);
    }

    void RestoreRazorgoreDelayedEngagementStrategies(PlayerbotAI* ai, const RazorgoreStrategySnapshot& snapshot)
    {
        if (snapshot.hadWaitForAttack && !ai->HasStrategy("wait for attack", BotState::BOT_STATE_COMBAT))
            ai->ChangeStrategy("+wait for attack", BotState::BOT_STATE_COMBAT);

        if (snapshot.hadThreat && !ai->HasStrategy("threat", BotState::BOT_STATE_COMBAT))
            ai->ChangeStrategy("+threat", BotState::BOT_STATE_COMBAT);

        if (snapshot.hadPullBack && !ai->HasStrategy("pull back", BotState::BOT_STATE_COMBAT))
            ai->ChangeStrategy("+pull back", BotState::BOT_STATE_COMBAT);
    }

    std::list<GuidPosition> GetUsableSuppressionDevicesInSight(PlayerbotAI* ai)
    {
        return GetGuidPositionsValue(ai,
            "go usable filter::go trapped filter::entry filter::{gos in sight,suppression devices}");
    }

    std::list<GuidPosition> GetUsableSuppressionDevicesClose(PlayerbotAI* ai)
    {
        return GetGuidPositionsValue(ai,
            "go usable filter::go trapped filter::entry filter::{gos close,suppression devices}");
    }
}


bool MarkRazorgoreControllerAction::Execute(Event& event)
{
    Unit* controller = BlackwingLair::FindGrethokController(ai);
    if (!controller || !controller->IsAlive())
        return false;

    bool changed = false;
    changed |= SetTargetIcon("skull", controller);
    changed |= SetRti("skull");
    changed |= SetCurrentTarget(controller);

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
        ai->TellPlayerNoFacing(GetMaster(), "Razorgore: marked and selected Grethok the Controller");

    return changed || IsDpsRtiSelected("skull", controller);
}

bool MarkRazorgoreControllerAction::isUseful()
{
    Unit* controller = BlackwingLair::FindGrethokController(ai);
    if (!controller || !controller->IsAlive())
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    return !currentTarget || !currentTarget->IsAlive() ||
           currentTarget->GetObjectGuid() != controller->GetObjectGuid();
}

bool MoveNearRazorgoreAction::Execute(Event& event)
{
    Unit* razorgore = BlackwingLair::FindRazorgore(ai);
    if (!razorgore)
        return false;

    WorldPosition razorgorePos(razorgore);

    // Deterministic per-bot ring placement. This keeps the group close to
    // Razorgore during the egg phase without stacking every bot on one point.
    float normalized = float(bot->GetGUIDLow() % 6283) / 6283.0f;
    float angle = normalized * 2.0f * M_PI_F;
    float radiusSpread = float(bot->GetGUIDLow() % 1000) / 1000.0f;
    float radius = BlackwingLair::RAZORGORE_EGG_PHASE_RING_MIN +
                   radiusSpread * (BlackwingLair::RAZORGORE_EGG_PHASE_RING_MAX -
                                   BlackwingLair::RAZORGORE_EGG_PHASE_RING_MIN);

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

bool MoveToVaelastraszTankPositionAction::Execute(Event& event)
{
    if (!BlackwingLair::FindVaelastrasz(ai))
        return false;

    if (!BlackwingLair::IsVaelastraszTargetingBot(ai))
        return false;

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
        ai->TellPlayerNoFacing(GetMaster(), "Vaelastrasz: pulling boss to tank position");

    return MoveToDungeonPosition(BlackwingLair::MAP_ID,
                                 BlackwingLair::VAELASTRASZ_PULL_X,
                                 BlackwingLair::VAELASTRASZ_PULL_Y,
                                 BlackwingLair::VAELASTRASZ_PULL_Z);
}

bool MoveToVaelastraszTankPositionAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool MoveToVaelastraszTankPositionAction::isUseful()
{
    if (!ai->HasStrategy("vaelastrasz", BotState::BOT_STATE_COMBAT))
        return false;

    if (!BlackwingLair::FindVaelastrasz(ai))
        return false;

    if (!BlackwingLair::IsVaelastraszTargetingBot(ai))
        return false;

    return BlackwingLair::DistanceToVaelastraszPullPosition(bot) >
           BlackwingLair::VAELASTRASZ_POSITION_REACHED_DISTANCE;
}

bool MoveToVaelastraszRangedPositionAction::Execute(Event& event)
{
    if (!BlackwingLair::FindVaelastrasz(ai))
        return false;

    if (BlackwingLair::IsVaelastraszTargetingBot(ai))
        return false;

    if (!ai->IsRanged(bot))
        return false;

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
        ai->TellPlayerNoFacing(GetMaster(), "Vaelastrasz: ranged bot returning to ranged position");

    return MoveToDungeonPosition(BlackwingLair::MAP_ID,
                                 BlackwingLair::VAELASTRASZ_RANGED_X,
                                 BlackwingLair::VAELASTRASZ_RANGED_Y,
                                 BlackwingLair::VAELASTRASZ_RANGED_Z);
}

bool MoveToVaelastraszRangedPositionAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool MoveToVaelastraszRangedPositionAction::isUseful()
{
    if (!ai->HasStrategy("vaelastrasz", BotState::BOT_STATE_COMBAT))
        return false;

    if (!BlackwingLair::FindVaelastrasz(ai))
        return false;

    if (BlackwingLair::IsVaelastraszTargetingBot(ai))
        return false;

    if (!ai->IsRanged(bot))
        return false;

    if (BlackwingLair::DistanceToVaelastraszPullPosition(bot) >
        BlackwingLair::VAELASTRASZ_RANGED_PULL_POSITION_DISTANCE)
    {
        return false;
    }

    return BlackwingLair::DistanceToVaelastraszRangedPosition(bot) >
           BlackwingLair::VAELASTRASZ_POSITION_REACHED_DISTANCE;
}

bool MoveToBroodlordStackPositionAction::isUseful()
{
    if (!ai->HasStrategy("broodlord", BotState::BOT_STATE_COMBAT))
        return false;

    if (!BlackwingLair::FindBroodlord(ai))
        return false;

    if (!IsBotReadyForDungeonMovement(BlackwingLair::MAP_ID))
        return false;

    return !BlackwingLair::IsBotNearBroodlordStackPosition(ai);
}

bool MoveToBroodlordStackPositionAction::Execute(Event& event)
{
    if (!isUseful())
        return false;

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
    {
        ai->TellPlayerNoFacing(GetMaster(), "Broodlord: moving to stack position");
    }

    float x, y, z;
    BlackwingLair::GetBroodlordStackPosition(ai, x, y, z);

    return MoveToDungeonPosition(
        BlackwingLair::MAP_ID,
        x,
        y,
        z);
}

bool MoveToSuppressionDeviceAction::Execute(Event& event)
{
    std::list<GuidPosition> gos = GetUsableSuppressionDevicesInSight(ai);
    if (gos.empty())
        return false;

    WorldPosition botPos(bot);
    GuidPosition closest;
    float closestDist = FLT_MAX;

    for (const GuidPosition& gp : gos)
    {
        float dist = botPos.distance(gp);
        if (dist < closestDist)
        {
            closestDist = dist;
            closest = gp;
        }
    }

    if (!closest)
        return false;

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_NON_COMBAT))
        ai->TellPlayerNoFacing(GetMaster(), "Moving to Suppression Device at " + std::to_string((int)closestDist) + " yards");

    return MoveTo(closest.getMapId(), closest.getX(), closest.getY(), closest.getZ());
}

bool MoveToSuppressionDeviceAction::isPossible()
{
    return MovementAction::isPossible() && ai->CanMove();
}

bool MoveToSuppressionDeviceAction::isUseful()
{
    return !GetUsableSuppressionDevicesInSight(ai).empty();
}

bool StealthForSuppressionDeviceAction::Execute(Event& event)
{
    if (bot->getClass() != CLASS_ROGUE)
        return false;

    if (ai->HasAura("stealth", bot))
        return false;

    if (ai->CastSpell("stealth", bot))
    {
        ai->ChangeStrategy("+stealthed", BotState::BOT_STATE_COMBAT);
        ai->ChangeStrategy("+stealthed", BotState::BOT_STATE_NON_COMBAT);
        bot->InterruptSpell(CURRENT_MELEE_SPELL);
        return true;
    }

    return false;
}

bool StealthForSuppressionDeviceAction::isPossible()
{
    return bot->getClass() == CLASS_ROGUE && !ai->HasAura("stealth", bot);
}

bool StealthForSuppressionDeviceAction::isUseful()
{
    if (ai->HasAura("stealth", bot))
        return false;

    // Core rogue stealth logic has WSG/EYE flag checks; keep those here too.
    return !ai->HasAura(23333, bot) && !ai->HasAura(23335, bot) && !ai->HasAura(34976, bot);
}

bool DeactivateSuppressionDeviceAction::Execute(Event& event)
{
    std::list<GuidPosition> gos = GetUsableSuppressionDevicesClose(ai);
    if (gos.empty())
        return false;

    for (const GuidPosition& guidPos : gos)
    {
        GameObject* go = ai->GetGameObject(guidPos);
        if (!go)
            continue;

        if (go->GetLootState() != GO_READY)
            continue;

        if (!bot->GetGameObjectIfCanInteractWith(go->GetObjectGuid(), GAMEOBJECT_TYPE_TRAP))
            continue;

        std::unique_ptr<WorldPacket> packet(new WorldPacket(CMSG_GAMEOBJ_USE));
        *packet << go->GetObjectGuid();
        bot->GetSession()->QueuePacket(std::move(packet));

        if (ai->HasStrategy("debug move", BotState::BOT_STATE_NON_COMBAT))
            ai->TellPlayerNoFacing(GetMaster(), "Deactivating Suppression Device");

        return true;
    }

    return false;
}

bool DeactivateSuppressionDeviceAction::isPossible()
{
    return ai->CanMove();
}

bool DeactivateSuppressionDeviceAction::isUseful()
{
    return !GetUsableSuppressionDevicesClose(ai).empty();
}

bool DisarmSuppressionDeviceAction::Execute(Event& event)
{
    if (bot->getClass() != CLASS_ROGUE)
        return false;

    if (!bot->HasSpell(SPELL_DISARM_TRAP))
        return false;

    std::list<GuidPosition> gos = GetUsableSuppressionDevicesClose(ai);
    if (gos.empty())
        return false;

    WorldPosition botPos(bot);
    GameObject* closestGo = nullptr;
    float closestDist = FLT_MAX;

    for (const GuidPosition& guidPos : gos)
    {
        GameObject* go = ai->GetGameObject(guidPos);
        if (!go)
            continue;

        if (go->GetLootState() != GO_READY)
            continue;

        float dist = botPos.distance(WorldPosition(go));
        if (dist < closestDist)
        {
            closestDist = dist;
            closestGo = go;
        }
    }

    if (!closestGo)
        return false;

    if (ai->HasStrategy("debug move", BotState::BOT_STATE_NON_COMBAT))
        ai->TellPlayerNoFacing(GetMaster(), "Casting Disarm Trap on Suppression Device");

    return ai->CastSpell(SPELL_DISARM_TRAP, closestGo);
}

bool DisarmSuppressionDeviceAction::isPossible()
{
    return bot->getClass() == CLASS_ROGUE &&
           bot->HasSpell(SPELL_DISARM_TRAP) &&
           ai->CanMove();
}

bool DisarmSuppressionDeviceAction::isUseful()
{
    return !GetUsableSuppressionDevicesClose(ai).empty();
}

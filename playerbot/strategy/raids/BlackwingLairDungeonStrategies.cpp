#include "playerbot/playerbot.h"
#include "BlackwingLairDungeonStrategies.h"
#include "BlackwingLairDungeonMultipliers.h"

using namespace ai;

void BlackwingLairDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start razorgore fight",
        NextAction::array(0, new NextAction("enable razorgore fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "end razorgore fight",
        NextAction::array(0, new NextAction("disable razorgore fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device close",
        NextAction::array(0, new NextAction("disarm suppression device", 80.0f), NULL)));
}

void BlackwingLairDungeonStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end razorgore fight",
        NextAction::array(0, new NextAction("disable razorgore fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device need stealth",
        NextAction::array(0, new NextAction("stealth for suppression device", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device in sight",
        NextAction::array(0, new NextAction("move to suppression device", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device close",
        NextAction::array(0, new NextAction("disarm suppression device", ACTION_HIGH + 4), NULL)));
}

void RazorgoreFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "razorgore controller alive",
        NextAction::array(0,
            new NextAction("focus razorgore controller", ACTION_EMERGENCY + 10),
            new NextAction("attack", ACTION_EMERGENCY + 9),
            NULL)));

    triggers.push_back(new TriggerNode(
        "razorgore far from boss",
        NextAction::array(0, new NextAction("move near razorgore", ACTION_EMERGENCY + 6), NULL)));
}

void RazorgoreFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "razorgore controller alive",
        NextAction::array(0, new NextAction("focus razorgore controller", ACTION_HIGH + 10), NULL)));
}

void RazorgoreFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    multipliers.push_back(new RazorgoreCrowdControlMultiplier(ai));
    multipliers.push_back(new RazorgoreEggPhaseMovementMultiplier(ai));
}

void RazorgoreFightStrategy::InitNonCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    multipliers.push_back(new RazorgoreCrowdControlMultiplier(ai));
}

void RazorgoreFightStrategy::OnStrategyAdded(BotState state)
{
    // Razorgore phase 1 is about killing Grethok immediately and then staying near
    // Razorgore while the orb controller destroys eggs. Avoid-movement strategies
    // tend to pull bots away from the boss/add stack, so disable them for this fight.
    ai->ChangeStrategy("-avoid aoe", BotState::BOT_STATE_COMBAT);
    ai->ChangeStrategy("-avoid aoe", BotState::BOT_STATE_NON_COMBAT);
    ai->ChangeStrategy("-avoid aoe", BotState::BOT_STATE_REACTION);
    ai->ChangeStrategy("-avoid mobs", BotState::BOT_STATE_COMBAT);
    ai->ChangeStrategy("-avoid mobs", BotState::BOT_STATE_NON_COMBAT);
    ai->ChangeStrategy("-avoid mobs", BotState::BOT_STATE_REACTION);
}

void SuppressionRoomStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "suppression device need stealth",
        NextAction::array(0, new NextAction("vanish", ACTION_EMERGENCY + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device in sight",
        NextAction::array(0, new NextAction("move to suppression device", ACTION_HIGH + 8), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device close",
        NextAction::array(0, new NextAction("disarm suppression device", 90.0f), NULL)));
}

void SuppressionRoomStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "suppression device need stealth",
        NextAction::array(0, new NextAction("stealth for suppression device", ACTION_MOVE), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device in sight",
        NextAction::array(0, new NextAction("move to suppression device", ACTION_HIGH + 8), NULL)));

    triggers.push_back(new TriggerNode(
        "suppression device close",
        NextAction::array(0, new NextAction("disarm suppression device", ACTION_MOVE + 2), NULL)));
}

void SuppressionRoomStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    multipliers.push_back(new SuppressionRoomPassiveMultiplier(ai));
}

void SuppressionRoomStrategy::InitNonCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    multipliers.push_back(new SuppressionRoomPassiveMultiplier(ai));
}

void SuppressionRoomStrategy::OnStrategyAdded(BotState state)
{
    if (ai->GetBot()->getClass() == CLASS_ROGUE)
    {
        ai->ChangeStrategy("-avoid aoe", BotState::BOT_STATE_COMBAT);
        ai->ChangeStrategy("-avoid aoe", BotState::BOT_STATE_NON_COMBAT);
        ai->ChangeStrategy("-avoid aoe", BotState::BOT_STATE_REACTION);
        ai->ChangeStrategy("-avoid mobs", BotState::BOT_STATE_COMBAT);
        ai->ChangeStrategy("-avoid mobs", BotState::BOT_STATE_NON_COMBAT);
        ai->ChangeStrategy("-avoid mobs", BotState::BOT_STATE_REACTION);
    }
}

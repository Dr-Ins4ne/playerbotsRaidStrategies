#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonStrategies.h"
#include "AhnQirajTempleDungeonMultipliers.h"

using namespace ai;

void AhnQirajTempleDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("start skeram fight", NextAction::array(0, new NextAction("enable skeram fight strategy", 100.0f), NULL)));
    triggers.push_back(new TriggerNode("end skeram fight", NextAction::array(0, new NextAction("skeram clear ranged position", 101.0f), new NextAction("disable skeram fight strategy", 100.0f), NULL)));
}

void AhnQirajTempleDungeonStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode("end skeram fight", NextAction::array(0, new NextAction("skeram clear ranged position", 101.0f), new NextAction("disable skeram fight strategy", 100.0f), NULL)));
}

void SkeramFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();
    if (!bot)
        return;

    triggers.push_back(new TriggerNode("skeram greater nature protection potion ready", NextAction::array(0, new NextAction("use greater nature protection potion", 112.0f), NULL)));
    triggers.push_back(new TriggerNode("skeram tank target controlled player", NextAction::array(0, new NextAction("interrupt current spell", 111.0f), new NextAction("skeram tank target real", 110.0f), NULL)));
    triggers.push_back(new TriggerNode("skeram controlled target needs cc", NextAction::array(0, new NextAction("interrupt current spell", 109.0f), new NextAction("cc skeram controlled target", 108.0f), NULL)));
    //triggers.push_back(new TriggerNode("skeram controlled player too close", NextAction::array(0, new NextAction("skeram select controlled player for flee", 107.0f), new NextAction("flee", 106.0f), NULL)));
    //triggers.push_back(new TriggerNode("skeram rti target not visible", NextAction::array(0, new NextAction("skeram move to pull position", 103.0f), NULL)));
    triggers.push_back(new TriggerNode("skeram tank out of position", NextAction::array(0, new NextAction("skeram pull to tank position", 102.0f), NULL)));

    if ((ai->IsRanged(bot) || ai->IsHeal(bot)) && !ai->IsTank(bot))
    {
        triggers.push_back(new TriggerNode("skeram ranged position not assigned", NextAction::array(0, new NextAction("skeram assign ranged position", 101.5f), NULL)));
        triggers.push_back(new TriggerNode("skeram ranged out of position", NextAction::array(0, new NextAction("skeram move to ranged position", 101.0f), NULL)));
    }

    triggers.push_back(new TriggerNode("skeram images active", NextAction::array(0, new NextAction("interrupt current spell", 100.0f), new NextAction("skeram mark images", 99.0f), new NextAction("skeram select image target", 98.0f), new NextAction("attack rti target", 97.0f), NULL)));
    triggers.push_back(new TriggerNode("skeram no images active", NextAction::array(0, new NextAction("skeram mark real", 80.0f), new NextAction("skeram select real target", 79.0f), new NextAction("attack rti target", 78.0f), NULL)));
}

void SkeramFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    multipliers.push_back(new SkeramDamageControlMultiplier(ai));
    multipliers.push_back(new SkeramControlledPlayerDamageMultiplier(ai));
    multipliers.push_back(new SkeramTankAnchorMultiplier(ai));
}

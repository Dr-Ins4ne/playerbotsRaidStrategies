#include "playerbot/playerbot.h"
#include "UsePotionsStrategy.h"

using namespace ai;

class UsePotionsStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    UsePotionsStrategyActionNodeFactory()
    {
        creators["healthstone"] = &healthstone;
        creators["healing potion"] = &healing_potion;
        creators["rejuvenation potion"] = &rejuvenation_potion;
        creators["dark rune"] = &dark_rune;
        creators["mana potion"] = &mana_potion;
    }

private:
    // Health chain:
    // healthstone -> healing potion -> rejuvenation potion -> bandage
    ACTION_NODE_A(healthstone, "healthstone", "healing potion");

    ACTION_NODE_A(healing_potion, "healing potion", "rejuvenation potion");

    ACTION_NODE_A(rejuvenation_potion, "rejuvenation potion", "use bandage");

    // Mana chain:
    // dark rune -> mana potion -> rejuvenation potion
    ACTION_NODE_A(dark_rune, "dark rune", "mana potion");

    ACTION_NODE_A(mana_potion, "mana potion", "rejuvenation potion");
};

UsePotionsStrategy::UsePotionsStrategy(PlayerbotAI* ai) : Strategy(ai)
{
    actionNodeFactories.Add(std::make_unique<UsePotionsStrategyActionNodeFactory>());
}

void UsePotionsStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("healthstone", ACTION_CRITICAL_HEAL + 1), NULL)));

#ifdef MANGOSBOT_ZERO
    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("whipper root tuber", ACTION_CRITICAL_HEAL), NULL)));
#endif

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("healing potion", ACTION_MEDIUM_HEAL), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("dark rune", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "has poison debuff",
        NextAction::array(0, new NextAction("anti-venom", ACTION_DISPEL), NULL)));
}

void UsePotionsStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}
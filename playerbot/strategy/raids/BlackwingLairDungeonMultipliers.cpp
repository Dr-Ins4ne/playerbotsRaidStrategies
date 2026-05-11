#include "playerbot/playerbot.h"
#include "BlackwingLairDungeonMultipliers.h"
#include "BlackwingLairDungeonData.h"

using namespace ai;

namespace
{
    bool StartsWith(const std::string& text, const std::string& prefix)
    {
        return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
    }

    bool IsExactOrActionVariant(const std::string& name, const std::string& base)
    {
        return name == base || StartsWith(name, base + " ") || StartsWith(name, base + " on ") || StartsWith(name, base + "::");
    }

    bool IsCrowdControlActionName(const std::string& name)
    {
        static const char* crowdControlActions[] =
        {
            "polymorph",
            "sap",
            "seduction",
            "fear",
            "psychic scream",
            "intimidating shout",
            "hibernate",
            "entangling roots",
            "banish",
            "shackle undead",
            "freezing trap",
            "frost nova",
            "blind",
            "gouge",
            "cheap shot",
            "kidney shot",
            "scatter shot",
            "wyvern sting",
            "war stomp",
            "repentance",
            "hammer of justice"
        };

        for (const char* actionName : crowdControlActions)
        {
            if (IsExactOrActionVariant(name, actionName))
                return true;
        }

        return false;
    }

    bool IsAllowedRazorgoreMovementAction(const std::string& name)
    {
        return name == "move near razorgore" ||
               name == "focus razorgore controller" ||
               name == "reach spell" || StartsWith(name, "reach spell::") ||
               name == "reach melee" || StartsWith(name, "reach melee::") ||
               name == "attack" ||
               name == "melee" ||
               name == "dps assist" ||
               name == "tank assist" ||
               name == "heal" ||
               name.find(" on party") != std::string::npos;
    }

    bool IsSuppressedEggPhaseMovementAction(const std::string& name)
    {
        static const char* movementActions[] =
        {
            "flee",
            "flee with pet",
            "runaway",
            "move out of enemy contact",
            "move random",
            "move to loot",
            "open loot",
            "loot",
            "follow",
            "return",
            "return to stay position",
            "return to pull position",
            "move to circle formation",
            "move to rpg target",
            "move to travel target"
        };

        for (const char* actionName : movementActions)
        {
            if (name == actionName)
                return true;
        }

        return false;
    }
}

float SuppressionRoomPassiveMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (ai->GetBot()->getClass() != CLASS_ROGUE)
        return 1.0f;

    const std::string& name = action->getName();

    // Enable only the following actions for suppression room to avoid regular combat breaking logic.
    if (name == "stealth for suppression device" ||
        name == "move to suppression device" ||
        name == "disarm suppression device" ||
        name == "deactivate suppression device")
    {
        return 1.0f;
    }

    if (name == "stealth" ||
        name == "unstealth" ||
        name == "check stealth" ||
        name == "sprint" ||
        name == "vanish")
    {
        return 1.0f;
    }

    if (name == "co" ||
        name == "nc" ||
        name == "load ai" ||
        name == "save ai" ||
        name == "list ai" ||
        name == "reset ai" ||
        name == "reset strats" ||
        name == "reset values" ||
        name == "check mount state" ||
        name == "accept invitation" ||
        name == "set combat state" ||
        name == "set non combat state" ||
        name == "set dead state" ||
        name == "update pvp strats" ||
        name == "update pve strats" ||
        name == "update raid strats" ||
        name == "loot roll" ||
        name == "auto loot roll" ||
        name == "follow" ||
        name == "stay" ||
        name == "food" ||
        name == "drink")
    {
        return 1.0f;
    }

    return 0.0f;
}

float RazorgoreCrowdControlMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!ai->HasStrategy("razorgore", BotState::BOT_STATE_COMBAT))
        return 1.0f;

    const std::string& name = action->getName();
    if (IsCrowdControlActionName(name))
        return 0.0f;

    return 1.0f;
}

float RazorgoreEggPhaseMovementMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!BlackwingLair::IsRazorgoreEggPhase(ai))
        return 1.0f;

    const std::string& name = action->getName();
    if (IsAllowedRazorgoreMovementAction(name))
        return 1.0f;

    if (IsSuppressedEggPhaseMovementAction(name))
        return 0.0f;

    return 1.0f;
}

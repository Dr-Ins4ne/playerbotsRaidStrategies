#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonMultipliers.h"
#include "AhnQirajTempleDungeonUtils.h"

#include <set>
#include <string>

using namespace ai;

bool SkeramDamageControlMultiplier::IsBlockedRealSkeramAction(Action* action) const
{
    if (!action)
        return false;

    std::string name = action->getName();
    strToLower(name);

    if (name == "melee" || name == "attack" || name == "shoot" || name == "attack rti target" ||
        name == "skeram mark images" || name == "skeram mark real" ||
        name == "skeram select image target" || name == "skeram select real target" ||
        name == "skeram pull to tank position" || name == "skeram move to pull position" ||
        name == "skeram tank target real" || name == "cc skeram controlled target" ||
        name == "skeram select controlled player for flee" || name == "flee" || name == "flee with pet" ||
        name == "use greater nature protection potion")
    {
        return false;
    }

    static const std::set<std::string> blocked =
    {
        "fireball", "frostbolt", "scorch", "pyroblast", "arcane missiles",
        "arcane explosion", "flamestrike", "blizzard", "cone of cold", "fire blast",
        "shadow bolt", "immolate", "corruption", "curse of agony", "curse of doom",
        "siphon life", "drain life", "drain soul", "searing pain", "rain of fire", "hellfire",
        "smite", "holy fire", "mind blast", "mind flay", "shadow word: pain", "devouring plague",
        "lightning bolt", "chain lightning", "earth shock", "flame shock", "frost shock", "fire nova totem",
        "wrath", "starfire", "moonfire", "insect swarm", "hurricane",
        "arcane shot", "aimed shot", "multi-shot", "serpent sting", "scorpid sting", "viper sting",
        "volley", "explosive trap", "immolation trap",
        "sinister strike", "backstab", "eviscerate", "rupture", "slice and dice", "gouge",
        "heroic strike", "cleave", "mortal strike", "bloodthirst", "whirlwind", "execute",
        "judgement", "consecration", "exorcism", "hammer of wrath",
        "dps aoe", "goblin sapper", "oil of immolation", "throw grenade", "dragonbreath chili"
    };

    return blocked.find(name) != blocked.end();
}

float SkeramDamageControlMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return 1.0f;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget)
        return 1.0f;

    if (AhnQirajTemple::IsSkeramImage(ai, currentTarget))
        return 1.0f;

    if (AhnQirajTemple::IsRealSkeram(ai, currentTarget) && IsBlockedRealSkeramAction(action))
        return 0.0f;

    return 1.0f;
}

bool SkeramControlledPlayerDamageMultiplier::IsAllowedControlledPlayerAction(Action* action) const
{
    if (!action)
        return false;

    std::string name = action->getName();
    strToLower(name);

    return name == "cc skeram controlled target" ||
           name == "skeram select controlled player for flee" ||
           name == "flee" ||
           name == "flee with pet" ||
           name == "interrupt current spell" ||
           name == "skeram pull to tank position" ||
           name == "skeram move to pull position" ||
           name == "skeram tank target real" ||
           name == "use greater nature protection potion";
}

float SkeramControlledPlayerDamageMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return 1.0f;

    Player* bot = ai->GetBot();
    if (!bot)
        return 1.0f;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget)
        return 1.0f;

    if (!AhnQirajTemple::IsValidSkeramControlledTarget(ai, bot, currentTarget, false))
        return 1.0f;

    if (IsAllowedControlledPlayerAction(action))
        return 1.0f;

    std::string name = action->getName();
    strToLower(name);

    static const std::set<std::string> blocked =
    {
        "melee", "attack", "shoot", "attack rti target",
        "fireball", "frostbolt", "scorch", "pyroblast", "arcane missiles",
        "arcane explosion", "flamestrike", "blizzard", "cone of cold", "fire blast",
        "shadow bolt", "immolate", "corruption", "curse of agony", "curse of doom",
        "siphon life", "drain life", "drain soul", "searing pain", "rain of fire", "hellfire",
        "smite", "holy fire", "mind blast", "mind flay", "shadow word: pain", "devouring plague",
        "lightning bolt", "chain lightning", "earth shock", "flame shock", "frost shock", "fire nova totem",
        "wrath", "starfire", "moonfire", "insect swarm", "hurricane",
        "arcane shot", "aimed shot", "multi-shot", "serpent sting", "scorpid sting", "viper sting",
        "volley", "explosive trap", "immolation trap",
        "sinister strike", "backstab", "eviscerate", "rupture", "slice and dice", "gouge",
        "heroic strike", "cleave", "mortal strike", "bloodthirst", "whirlwind", "execute",
        "judgement", "consecration", "exorcism", "hammer of wrath",
        "dps aoe", "goblin sapper", "oil of immolation", "throw grenade", "dragonbreath chili"
    };

    return blocked.find(name) != blocked.end() ? 0.0f : 1.0f;
}

bool SkeramTankAnchorMultiplier::IsBlockedAnchorBreakingAction(Action* action) const
{
    if (!action)
        return false;

    std::string name = action->getName();
    strToLower(name);

    return name == "reach melee" ||
           name == "reach spell" ||
           name == "reach pull" ||
           name == "set behind" ||
           name == "move out of enemy contact";
}

float SkeramTankAnchorMultiplier::GetValue(Action* action)
{
    if (!action)
        return 1.0f;

    if (!ai->HasStrategy("skeram", BotState::BOT_STATE_COMBAT))
        return 1.0f;

    if (!AhnQirajTemple::IsRealSkeramTargetingBot(ai))
        return 1.0f;

    std::string name = action->getName();
    strToLower(name);

    if (name == "skeram pull to tank position" || name == "skeram tank target real" || name == "use greater nature protection potion")
        return 1.0f;

    if (IsBlockedAnchorBreakingAction(action))
        return 0.0f;

    return 1.0f;
}

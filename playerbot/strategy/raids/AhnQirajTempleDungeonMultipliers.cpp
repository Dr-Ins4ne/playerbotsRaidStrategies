#include "playerbot/playerbot.h"
#include "AhnQirajTempleDungeonMultipliers.h"

using namespace ai;

bool SkeramDamageControlMultiplier::IsBlockedRealSkeramAction(Action* action) const
{
    if (!action)
        return false;

    std::string name = action->getName();
    strToLower(name);

    // Allow white damage and target management. Everything below is explicit spell/aoe suppression.
    if (name == "melee" || name == "attack" || name == "shoot" || name == "attack rti target" ||
        name == "skeram mark images" || name == "skeram mark real" ||
        name == "skeram select image target" || name == "skeram select real target")
    {
        return false;
    }

    static const std::set<std::string> blocked =
    {
        // Mage
        "fireball", "frostbolt", "scorch", "pyroblast", "arcane missiles",
        "arcane explosion", "flamestrike", "blizzard", "cone of cold", "fire blast",

        // Warlock
        "shadow bolt", "immolate", "corruption", "curse of agony", "curse of doom",
        "siphon life", "drain life", "drain soul", "searing pain", "rain of fire", "hellfire",

        // Priest offensive spells
        "smite", "holy fire", "mind blast", "mind flay", "shadow word: pain", "devouring plague",

        // Shaman offensive spells
        "lightning bolt", "chain lightning", "earth shock", "flame shock", "frost shock", "fire nova totem",

        // Druid offensive spells
        "wrath", "starfire", "moonfire", "insect swarm", "hurricane",

        // Hunter special shots / aoe. The generic shoot action is still allowed.
        "arcane shot", "aimed shot", "multi-shot", "serpent sting", "scorpid sting", "viper sting",
        "volley", "explosive trap", "immolation trap",

        // Rogue/warrior/paladin special damage; white melee remains allowed.
        "sinister strike", "backstab", "eviscerate", "rupture", "slice and dice", "gouge",
        "heroic strike", "cleave", "mortal strike", "bloodthirst", "whirlwind", "execute",
        "judgement", "consecration", "exorcism", "hammer of wrath",

        // Generic aoe / consumable burst that can push real boss too hard.
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

    // When the current target is an image, full normal DPS is allowed.
    if (AhnQirajTemple::IsSkeramImage(ai, currentTarget))
        return 1.0f;

    // When the current target is the real boss, force controlled damage only.
    if (AhnQirajTemple::IsRealSkeram(ai, currentTarget) && IsBlockedRealSkeramAction(action))
        return 0.0f;

    return 1.0f;
}

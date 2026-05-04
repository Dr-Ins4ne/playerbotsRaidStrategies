
#include "playerbot/playerbot.h"
#include "MoltenCoreDungeonStrategies.h"
#include "../generic/DungeonMultipliers.h"

using namespace ai;

void MoltenCoreDungeonStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start lucifron fight",
        NextAction::array(0, new NextAction("enable lucifron fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start magmadar fight",
        NextAction::array(0, new NextAction("enable magmadar fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start gehennas fight",
        NextAction::array(0, new NextAction("enable gehennas fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start garr fight",
        NextAction::array(0, new NextAction("enable garr fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start baron geddon fight",
        NextAction::array(0, new NextAction("enable baron geddon fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start shazzrah fight",
        NextAction::array(0, new NextAction("enable shazzrah fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start sulfuron harbinger fight",
        NextAction::array(0, new NextAction("enable sulfuron harbinger fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start golemagg fight",
        NextAction::array(0, new NextAction("enable golemagg fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start majordomo fight",
        NextAction::array(0, new NextAction("enable majordomo fight strategy", 100.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        "start ragnaros fight",
        NextAction::array(0, new NextAction("enable ragnaros fight strategy", 100.0f), NULL)));
}

void MoltenCoreDungeonStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    /*
    triggers.push_back(new TriggerNode(
        "val::and::{"
        "action possible::use id::17333,"
        "has object::go usable filter::go trapped filter::entry filter::{gos in sight,mc runes},"
        "not::has object::entry filter::{gos close,mc runes}"
        "}",
        NextAction::array(0, new NextAction("move to::entry filter::{gos in sight,mc runes}", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "val::has object::go usable filter::entry filter::{gos close,mc runes}",
        NextAction::array(0, new NextAction("use id::{17333,entry filter::{gos close,mc runes}}", 1.0f), NULL)));
        */

    triggers.push_back(new TriggerNode(
        "mc rune in sight",
        NextAction::array(0, new NextAction("move to mc rune", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "mc rune close",
        NextAction::array(0,
            new NextAction("douse mc rune eternal", 2.0f),
            new NextAction("douse mc rune aqual", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fire protection potion ready",
        NextAction::array(0, new NextAction("fire protection potion", 100.0f), NULL)));
}

// Lucifron Fight Strategy
void LucifronFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void LucifronFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end lucifron fight",
        NextAction::array(0, new NextAction("disable lucifron fight strategy", 100.0f), NULL)));
}

void LucifronFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end lucifron fight",
        NextAction::array(0, new NextAction("disable lucifron fight strategy", 100.0f), NULL)));
}

void LucifronFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void LucifronFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}


void MagmadarFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();
    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        triggers.push_back(new TriggerNode(
            "magmadar too close",
            NextAction::array(0, new NextAction("move away from magmadar", 100.0f), NULL)));
    }

    triggers.push_back(new TriggerNode(
        "fire protection potion ready",
        NextAction::array(0, new NextAction("fire protection potion", 100.0f), NULL)));
}

void MagmadarFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end magmadar fight",
        NextAction::array(0, new NextAction("disable magmadar fight strategy", 100.0f), NULL)));
}

void MagmadarFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end magmadar fight",
        NextAction::array(0, new NextAction("disable magmadar fight strategy", 100.0f), NULL)));
}

void MagmadarFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "magmadar lava bomb",
        NextAction::array(0, new NextAction("move away from hazard", 100.0f), NULL)));
}

void MagmadarFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    Player* bot = ai->GetBot();
    if (ai->IsRanged(bot) || ai->IsHeal(bot))
    {
        multipliers.push_back(new PreventMoveAwayFromCreatureOnReachToCastMultiplier(ai));
    }
}
    

// Gehennas Fight Strategy
void GehennasFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void GehennasFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end gehennas fight",
        NextAction::array(0, new NextAction("disable gehennas fight strategy", 100.0f), NULL)));
}

void GehennasFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end gehennas fight",
        NextAction::array(0, new NextAction("disable gehennas fight strategy", 100.0f), NULL)));
}

void GehennasFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void GehennasFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}

// Garr Fight Strategy
void GarrFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void GarrFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end garr fight",
        NextAction::array(0, new NextAction("disable garr fight strategy", 100.0f), NULL)));
}

void GarrFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end garr fight",
        NextAction::array(0, new NextAction("disable garr fight strategy", 100.0f), NULL)));
}

void GarrFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void GarrFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}

// Baron Geddon Fight Strategy
void BaronGeddonFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void BaronGeddonFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end baron geddon fight",
        NextAction::array(0, new NextAction("disable baron geddon fight strategy", 100.0f), NULL)));
}

void BaronGeddonFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end baron geddon fight",
        NextAction::array(0, new NextAction("disable baron geddon fight strategy", 100.0f), NULL)));
}

void BaronGeddonFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void BaronGeddonFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}

// Shazzrah Fight Strategy
void ShazzrahFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void ShazzrahFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end shazzrah fight",
        NextAction::array(0, new NextAction("disable shazzrah fight strategy", 100.0f), NULL)));
}

void ShazzrahFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end shazzrah fight",
        NextAction::array(0, new NextAction("disable shazzrah fight strategy", 100.0f), NULL)));
}

void ShazzrahFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void ShazzrahFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}

// Sulfuron Harbinger Fight Strategy
void SulfuronHarbingerFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void SulfuronHarbingerFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end sulfuron fight",
        NextAction::array(0, new NextAction("disable sulfuron fight strategy", 100.0f), NULL)));
}

void SulfuronHarbingerFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end sulfuron fight",
        NextAction::array(0, new NextAction("disable sulfuron fight strategy", 100.0f), NULL)));
}

void SulfuronHarbingerFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void SulfuronHarbingerFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}

// Golemagg Fight Strategy
void GolemaggFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();
    if (!bot)
        return;

    // Raid icons:
    // skull  = Golemagg
    // cross  = Core Rager 1
    // square = Core Rager 2
    triggers.push_back(new TriggerNode(
        "golemagg targets need marking",
        NextAction::array(0, new NextAction("mark golemagg targets", 150.0f), NULL)));

    // The bot currently tanking Golemagg should drag/hold him at the fixed tank position.
    // This does not move Core Rager tanks.
    if (ai->IsTank(bot))
    {
        triggers.push_back(new TriggerNode(
            "golemagg aggro holder out of position",
            NextAction::array(0, new NextAction("move golemagg to tank position", 140.0f), NULL)));
    }

    // DPS should stay on skull / Golemagg.
    // Healers and tanks are skipped by the action itself too, but keeping the role check here
    // avoids scheduling the action for roles that should not use it.
    if (!ai->IsHeal(bot) && !ai->IsTank(bot))
    {
        triggers.push_back(new TriggerNode(
            "golemagg alive",
            NextAction::array(0, new NextAction("select golemagg dps rti", 80.0f), NULL)));
    }

    // Core Rager tanks should keep their ragers away from Golemagg.
    // This only fires for tanks currently tanking a Core Rager.
    if (ai->IsTank(bot))
    {
        triggers.push_back(new TriggerNode(
            "golemagg rager tank too close",
            NextAction::array(0, new NextAction("move away from golemagg", 120.0f), NULL)));
    }
}

void GolemaggFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end golemagg fight",
        NextAction::array(0, new NextAction("disable golemagg fight strategy", 100.0f), NULL)));
}

void GolemaggFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end golemagg fight",
        NextAction::array(0, new NextAction("disable golemagg fight strategy", 100.0f), NULL)));
}

void GolemaggFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void GolemaggFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}

// -----------------------------------------------------------------------------
// Majordomo Executus Fight Strategy
// -----------------------------------------------------------------------------

void MajordomoExecutusFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();
    if (!bot)
        return;

    // Mark all adds:
    // elites  -> skull/cross/star/circle
    // healers -> moon/square/diamond/triangle
    triggers.push_back(new TriggerNode(
        "majordomo targets need marking",
        NextAction::array(0, new NextAction("mark majordomo targets", 180.0f), NULL)));

    // Emergency: if teleported into pit/lava, leave immediately.
    triggers.push_back(new TriggerNode(
        "majordomo teleported to pit",
        NextAction::array(0, new NextAction("move out of majordomo pit", 250.0f), NULL)));

    // Keep CC active on healer icons.
    // This is allowed for healers too if they have CC, but not tanks.
    if (!ai->IsTank(bot))
    {
        triggers.push_back(new TriggerNode(
            "majordomo needs crowd control",
            NextAction::array(0, new NextAction("cc majordomo add", 140.0f), NULL)));
    }

    // DPS should follow kill order via RTI.
    // Tanks/healers are skipped both here and in SelectDpsRti.
    if (!ai->IsHeal(bot) && !ai->IsTank(bot))
    {
        triggers.push_back(new TriggerNode(
            "majordomo adds alive",
            NextAction::array(0, new NextAction("select majordomo dps rti", 90.0f), NULL)));
    }
}

void MajordomoExecutusFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end majordomo fight",
        NextAction::array(0, new NextAction("disable majordomo fight strategy", 100.0f), NULL)));
}

void MajordomoExecutusFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end majordomo fight",
        NextAction::array(0, new NextAction("disable majordomo fight strategy", 100.0f), NULL)));
}

void MajordomoExecutusFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // Put pit escape here too so it can interrupt low-priority behavior immediately.
    triggers.push_back(new TriggerNode(
        "majordomo teleported to pit",
        NextAction::array(0, new NextAction("move out of majordomo pit", 300.0f), NULL)));
}

void MajordomoExecutusFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // Intentionally empty for now.
}

// Ragnaros Fight Strategy
void RagnarosFightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific combat triggers
}

void RagnarosFightStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end ragnaros fight",
        NextAction::array(0, new NextAction("disable ragnaros fight strategy", 100.0f), NULL)));
}

void RagnarosFightStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "end ragnaros fight",
        NextAction::array(0, new NextAction("disable ragnaros fight strategy", 100.0f), NULL)));
}

void RagnarosFightStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    // No specific reaction triggers
}

void RagnarosFightStrategy::InitCombatMultipliers(std::list<Multiplier*>& multipliers)
{
    // No specific combat multipliers
}
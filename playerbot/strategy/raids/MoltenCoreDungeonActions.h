#pragma once

#include "../actions/DungeonActions.h"
#include "../actions/ChangeStrategyAction.h"
#include "../actions/UseItemAction.h"
#include "RaidIconActionBase.h"

#include <string>
#include <vector>

namespace ai
{
    // -------------------------------------------------------------------------
    // Molten Core runes
    // -------------------------------------------------------------------------

    class MoveToMCRuneAction : public MoveToAction
    {
    public:
        MoveToMCRuneAction(PlayerbotAI* ai)
            : MoveToAction(ai, "move to mc rune")
        {
            qualifier = "entry filter::{gos in sight,mc runes}";
        }
    };


    class DouseMCRuneActionAqual : public UseItemIdAction
    {
    public:
        DouseMCRuneActionAqual(PlayerbotAI* ai)
            : UseItemIdAction(ai, "douse mc rune aqual")
        {
            qualifier = "{17333,entry filter::{gos close,mc runes}}";
        }
    };


    class DouseMCRuneActionEternal : public UseItemIdAction
    {
    public:
        DouseMCRuneActionEternal(PlayerbotAI* ai)
            : UseItemIdAction(ai, "douse mc rune eternal")
        {
            qualifier = "{22754,entry filter::{gos close,mc runes}}";
        }
    };


    // -------------------------------------------------------------------------
    // Golemagg the Incinerator
    // -------------------------------------------------------------------------

    class MarkGolemaggTargetsAction : public RaidIconActionBase
    {
    public:
        MarkGolemaggTargetsAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "mark golemagg targets") {}

        bool Execute(Event& event) override
        {
            bool changed = false;

            // skull = Golemagg
            changed |= SetTargetIcon("skull", NPC_GOLEMAGG);

            // cross/square = Core Ragers, only for tank awareness.
            changed |= SetTargetIconsForEntry({"cross", "square"}, NPC_CORE_RAGER);

            return changed;
        }

    private:
        static const uint32 NPC_GOLEMAGG   = 11988;
        static const uint32 NPC_CORE_RAGER = 11672;
    };


    class SelectGolemaggDpsRtiAction : public RaidIconActionBase
    {
    public:
        SelectGolemaggDpsRtiAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "select golemagg dps rti") {}

        bool Execute(Event& event) override
        {
            // DPS only: skull / Golemagg.
            // Tanks and healers are skipped inside SelectDpsRti().
            return SelectDpsRti("skull", NPC_GOLEMAGG);
        }

    private:
        static const uint32 NPC_GOLEMAGG = 11988;
    };


    class MoveGolemaggToTankPositionAction : public MovementAction
    {
    public:
        MoveGolemaggToTankPositionAction(PlayerbotAI* ai)
            : MovementAction(ai, "move golemagg to tank position") {}

        bool Execute(Event& event) override
        {
            if (IsAtGolemaggTankPosition())
                return false;

            return MoveTo(
                bot->GetMapId(),
                GolemaggTankX(),
                GolemaggTankY(),
                GolemaggTankZ(),
                false,
                IsReaction(),
                false,
                true
            );
        }

        bool isPossible() override
        {
            return MovementAction::isPossible() && ai->CanMove();
        }

    private:
        bool IsAtGolemaggTankPosition()
        {
            if (!bot)
                return false;

            return bot->GetDistance(
                GolemaggTankX(),
                GolemaggTankY(),
                GolemaggTankZ()
            ) <= GolemaggTankPositionRadius();
        }

        // Using functions instead of static constexpr float members avoids
        // older C++ linkage issues with non-integral static constexpr fields.
        static float GolemaggTankX() { return 780.6f; }
        static float GolemaggTankY() { return -1001.8f; }
        static float GolemaggTankZ() { return -206.0f; }

        // Stored for reference/future facing logic.
        // Current MoveTo(...) usage does not pass orientation directly.
        static float GolemaggTankO() { return 1.0f; }

        static float GolemaggTankPositionRadius() { return 5.0f; }
    };


    // -------------------------------------------------------------------------
    // Majordomo Executus
    // -------------------------------------------------------------------------

    class MarkMajordomoTargetsAction : public RaidIconActionBase
    {
    public:
        MarkMajordomoTargetsAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "mark majordomo targets") {}

        bool Execute(Event& event) override
        {
            bool changed = false;

            std::vector<Unit*> elites  = FindAliveCreatures(NPC_FLAMEWAKER_ELITE);
            std::vector<Unit*> healers = FindAliveCreatures(NPC_FLAMEWAKER_HEALER);

            // Kill order.
            changed |= SetTargetIconsForCreatures(
                {"skull", "cross", "star", "circle"},
                elites
            );

            // Crowd-control pool.
            changed |= SetTargetIconsForCreatures(
                {"moon", "square", "diamond", "triangle"},
                healers
            );

            return changed;
        }

    private:
        static const uint32 NPC_FLAMEWAKER_HEALER = 11663;
        static const uint32 NPC_FLAMEWAKER_ELITE  = 11664;
    };


    class SelectMajordomoDpsRtiAction : public RaidIconActionBase
    {
    public:
        SelectMajordomoDpsRtiAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "select majordomo dps rti") {}

        bool Execute(Event& event) override
        {
            std::vector<std::string> eliteIcons = {
                "skull",
                "cross",
                "star",
                "circle"
            };

            std::vector<std::string> healerIcons = {
                "moon",
                "square",
                "diamond",
                "triangle"
            };

            // Important:
            // Do not simply do:
            //
            // if (SelectFirstAliveDpsRti(eliteIcons)) return true;
            // return SelectFirstAliveDpsRti(healerIcons);
            //
            // because SelectFirstAliveDpsRti(...) can return false when the RTI
            // is already correct. In that case we still must NOT fall through
            // to healer icons while an elite is alive.

            if (FindFirstAliveIconTarget(eliteIcons))
                return SelectFirstAliveDpsRti(eliteIcons);

            return SelectFirstAliveDpsRti(healerIcons);
        }
    };


    class CrowdControlMajordomoAddAction : public RaidIconActionBase
    {
    public:
        CrowdControlMajordomoAddAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "cc majordomo add") {}

        bool Execute(Event& event) override
        {
            Unit* target = FindCcTarget();
            if (!target)
                return false;

            if (IsCommonCrowdControlled(target))
                return false;

            return TryCommonCrowdControl(target);
        }

    private:
        Unit* FindCcTarget()
        {
            // Main CC targets: Flamewaker Healers.
            Unit* target = FindFirstAliveIconTargetWithoutAuras(
                {"moon", "square", "diamond", "triangle"},
                GetCommonCrowdControlAuras()
            );

            if (target)
                return target;

            // Fallback: if healer icons are gone, try controlling remaining
            // non-primary kill targets.
            return FindFirstAliveIconTargetWithoutAuras(
                {"circle", "star", "cross"},
                GetCommonCrowdControlAuras()
            );
        }
    };


    class MoveOutOfMajordomoPitAction : public MovementAction
    {
    public:
        MoveOutOfMajordomoPitAction(PlayerbotAI* ai)
            : MovementAction(ai, "move out of majordomo pit") {}

        bool Execute(Event& event) override
        {
            if (IsAtMajordomoPitExit())
                return false;

            return MoveTo(
                bot->GetMapId(),
                MajordomoPitExitX(),
                MajordomoPitExitY(),
                MajordomoPitExitZ(),
                false,
                IsReaction(),
                false,
                true
            );
        }

        bool isPossible() override
        {
            return MovementAction::isPossible() && ai->CanMove();
        }

    private:
        bool IsAtMajordomoPitExit()
        {
            if (!bot)
                return false;

            return bot->GetDistance(
                MajordomoPitExitX(),
                MajordomoPitExitY(),
                MajordomoPitExitZ()
            ) <= MajordomoPitExitRadius();
        }

    private:
        static float MajordomoPitExitX() { return 711.7f; }
        static float MajordomoPitExitY() { return -1180.1f; }
        static float MajordomoPitExitZ() { return -119.3f; }

        static float MajordomoPitExitRadius() { return 3.0f; }
    };


    // -------------------------------------------------------------------------
    // Molten Core action context
    // -------------------------------------------------------------------------
    class MoltenCoreActionContext : public NamedObjectContext<Action>
    {
    public:
        MoltenCoreActionContext()
        {
            creators["enable molten core strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable molten core strategy", "+molten core"); };
            creators["disable molten core strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable molten core strategy", "-molten core"); };

            creators["enable lucifron fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable lucifron fight strategy", "+lucifron"); };
            creators["disable lucifron fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable lucifron fight strategy", "-lucifron"); };

            creators["enable magmadar fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable magmadar fight strategy", "+magmadar"); };
            creators["disable magmadar fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable magmadar fight strategy", "-magmadar"); };
            creators["move away from magmadar"] = [](PlayerbotAI* ai){ return new MoveAwayFromCreature(ai, "move away from magmadar", 11982, 31.0f); };

            creators["enable gehennas fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable gehennas fight strategy", "+gehennas"); };
            creators["disable gehennas fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable gehennas fight strategy", "-gehennas"); };

            creators["enable garr fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable garr fight strategy", "+garr"); };
            creators["disable garr fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable garr fight strategy", "-garr"); };

            creators["enable baron geddon fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable baron geddon fight strategy", "+baron geddon"); };
            creators["disable baron geddon fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable baron geddon fight strategy", "-baron geddon"); };

            creators["enable shazzrah fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable shazzrah fight strategy", "+shazzrah"); };
            creators["disable shazzrah fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable shazzrah fight strategy", "-shazzrah"); };

            creators["enable sulfuron fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable sulfuron fight strategy", "+sulfuron harbinger"); };
            creators["disable sulfuron fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable sulfuron fight strategy", "-sulfuron harbinger"); };

            creators["enable golemagg fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable golemagg fight strategy", "+golemagg"); };
            creators["disable golemagg fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable golemagg fight strategy", "-golemagg"); };
            creators["mark golemagg targets"] = [](PlayerbotAI* ai){ return new MarkGolemaggTargetsAction(ai); };
            creators["select golemagg dps rti"] = [](PlayerbotAI* ai){ return new SelectGolemaggDpsRtiAction(ai); };
            creators["move away from golemagg"] = [](PlayerbotAI* ai){ return new MoveAwayFromCreature(ai, "move away from golemagg", 11988, 30.0f); };
            creators["move golemagg to tank position"] = [](PlayerbotAI* ai){ return new MoveGolemaggToTankPositionAction(ai); };

            creators["enable majordomo fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable majordomo fight strategy", "+majordomo"); };
            creators["disable majordomo fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable majordomo fight strategy", "-majordomo"); };
            creators["mark majordomo targets"] = [](PlayerbotAI* ai){ return new MarkMajordomoTargetsAction(ai); };
            creators["select majordomo dps rti"] = [](PlayerbotAI* ai){ return new SelectMajordomoDpsRtiAction(ai); };
            creators["cc majordomo add"] = [](PlayerbotAI* ai){ return new CrowdControlMajordomoAddAction(ai); };
            creators["move out of majordomo pit"] = [](PlayerbotAI* ai){ return new MoveOutOfMajordomoPitAction(ai); };

            creators["enable ragnaros fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "enable ragnaros fight strategy", "+ragnaros"); };
            creators["disable ragnaros fight strategy"] = [](PlayerbotAI* ai){ return new ChangeAllStrategyAction(ai, "disable ragnaros fight strategy", "-ragnaros"); };

            creators["move away from hazard"] = [](PlayerbotAI* ai){ return new MoveAwayFromHazard(ai, "move away from hazard"); };
            creators["move to mc rune"] = &MoltenCoreActionContext::move_to_mc_rune;
            creators["douse mc rune aqual"] = &MoltenCoreActionContext::douse_mc_rune_aqual;
            creators["douse mc rune eternal"] = &MoltenCoreActionContext::douse_mc_rune_eternal;
        }

    private:
        static Action* move_to_mc_rune(PlayerbotAI* ai){ return new MoveToMCRuneAction(ai); }
        static Action* douse_mc_rune_aqual(PlayerbotAI* ai){ return new DouseMCRuneActionAqual(ai); }
        static Action* douse_mc_rune_eternal(PlayerbotAI* ai){ return new DouseMCRuneActionEternal(ai); }
    };
 
}
#pragma once

#include "../actions/DungeonActions.h"          
#include "../actions/GenericSpellActions.h"     
#include "../actions/MovementActions.h"

namespace ai
{
    // 1. Move away from Kurinnaxx's Sand Trap
    // In your branch, MoveAwayFromHazard is generic. 
    // It will automatically move away from 180630 if it's in the HazardsValue list.
    class MoveAwayFromKurinaxxSandTrapAction : public MoveAwayFromHazard
    {
    public:
        MoveAwayFromKurinaxxSandTrapAction(PlayerbotAI* ai) : MoveAwayFromHazard(ai, "move away from kurinnaxx sand trap") {}
    };



    // 3. Taunt Kurinnaxx
    class TauntKurinaxxAction : public CastSpellAction
    {
    public:
        TauntKurinaxxAction(PlayerbotAI* ai) : CastSpellAction(ai, "taunt") {}
        
        virtual std::string GetTargetName() override { return "creature id"; }
        virtual std::string GetTargetQualifier() override { return "15339"; }
    };

    // 4. Tank Retreat
    // CHANGED: Using MoveAwayFromCreature because FleeAction doesn't support IDs in your branch.
    // Syntax: ai, name, creatureID, range
    class KurinaxxTankRetreatAction : public MoveAwayFromCreature
    {
    public:
        KurinaxxTankRetreatAction(PlayerbotAI* ai) : MoveAwayFromCreature(ai, "kurinnaxx tank retreat", 15339, 30.0f) {}
    };

    // The Context Class
    class AhnQirajRuinsActionContext : public NamedObjectContext<Action>
    {
    public:
        AhnQirajRuinsActionContext()
        {
            creators["move away from kurinnaxx sand trap"] = &AhnQirajRuinsActionContext::move_away_from_kurinnaxx_sand_trap;
            creators["taunt kurinnaxx"] = &AhnQirajRuinsActionContext::taunt_kurinnaxx;
            creators["kurinnaxx tank retreat"] = &AhnQirajRuinsActionContext::kurinnaxx_tank_retreat;
        }

    private:
        static Action* move_away_from_kurinnaxx_sand_trap(PlayerbotAI* ai) { return new MoveAwayFromKurinaxxSandTrapAction(ai); }
        static Action* taunt_kurinnaxx(PlayerbotAI* ai) { return new TauntKurinaxxAction(ai); }
        static Action* kurinnaxx_tank_retreat(PlayerbotAI* ai) { return new KurinaxxTankRetreatAction(ai); }
    };
}
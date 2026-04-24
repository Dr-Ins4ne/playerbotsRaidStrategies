#pragma once

#include "DungeonTriggers.h" 
#include "GenericTriggers.h" 

namespace ai
{
    // 1. Trigger for Kurinnaxx's Sand Trap
    class KurinaxxSandTrapTrigger : public CloseToGameObjectHazardTrigger
    {
    public:
        KurinaxxSandTrapTrigger(PlayerbotAI* ai) : CloseToGameObjectHazardTrigger(ai, "kurinnaxx sand trap close", 180630, 8.0f, 60) {}
    };

    // 2. Trigger for Kurinnaxx's Toxic Volley
    class KurinaxxToxicVolleyTrigger : public HasAuraTrigger
    {
    public:
        // Using the simplified constructor from your GenericTriggers.h
        KurinaxxToxicVolleyTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "toxic volley poison aura") {}
        
        // We set the spell name/ID via qualifier logic or manual override
        virtual std::string getName() override { return "26056"; }
        virtual std::string GetTargetName() override { return "party member to dispel"; }
    };

    // 3. Trigger for Kurinnaxx's Mortal Wound (Tank Swap)
    class KurinaxxMortalWoundTrigger : public HasAuraTrigger
    {
    public:
        // We use the spell ID 26038 for "Mortal Wound"
        KurinaxxMortalWoundTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "mortal wound", 1) {}

        virtual std::string GetTargetName() override { return "tank assist"; }

        virtual bool IsActive() override
        {
            Unit* target = AI_VALUE(Unit*, GetTargetName());
            if (!target)
                return false;

            // Use the AI's built-in GetAura method (Spell ID 26038)
            // We check for >= 5 stacks as that is the usual threshold for Kurinnaxx swaps
            Aura* aura = ai->GetAura(26038, target);
            return aura && aura->GetStackAmount() >= 5;
        }
    };

    class AhnQirajRuinsTriggerContext : public NamedObjectContext<Trigger>
    {
    public:
        AhnQirajRuinsTriggerContext()
        {
            creators["kurinnaxx sand trap close"] = &AhnQirajRuinsTriggerContext::kurinnaxx_sand_trap_close;
            creators["toxic volley poison aura"] = &AhnQirajRuinsTriggerContext::toxic_volley_poison_aura;
            creators["kurinnaxx mortal wound high"] = &AhnQirajRuinsTriggerContext::kurinnaxx_mortal_wound_high;
        }

    private:
        static Trigger* kurinnaxx_sand_trap_close(PlayerbotAI* ai) { return new KurinaxxSandTrapTrigger(ai); }
        static Trigger* toxic_volley_poison_aura(PlayerbotAI* ai) { return new KurinaxxToxicVolleyTrigger(ai); }
        static Trigger* kurinnaxx_mortal_wound_high(PlayerbotAI* ai) { return new KurinaxxMortalWoundTrigger(ai); }
    };
}
#pragma once
#include "DungeonActions.h"
#include "ChangeStrategyAction.h"
#include "UseItemAction.h"


namespace ai
{
    // Movement action for the Edge of Madness brazier or Gahz'ranka's fishing node
    class MoveToZGInteractableAction : public MoveToAction
    {
    public:
        MoveToZGInteractableAction(PlayerbotAI* ai) : MoveToAction(ai, "move to zg interactable") { qualifier = "entry filter::{gos in sight,zg interactables}"; }
    };

    // Edge of Madness Brazier - Uses Gurubashi Mojo Madness (Item ID 19931)
    class UseGurubashiMojoAction : public UseItemIdAction
    {
    public:
        UseGurubashiMojoAction(PlayerbotAI* ai) : UseItemIdAction(ai, "use gurubashi mojo madness") { qualifier = "{19931,entry filter::{gos close,zg braziers}}"; }
    };

    // Gahz'ranka Summon - Uses Mudskunk Lure (Item ID 19974)
    class UseMudskunkLureAction : public UseItemIdAction
    {
    public:
        UseMudskunkLureAction(PlayerbotAI* ai) : UseItemIdAction(ai, "use mudskunk lure") { qualifier = "{19974,entry filter::{gos close,zg water nodes}}"; }
    };


   class ThekalActionBase : public Action
    {
    public:
        ThekalActionBase(PlayerbotAI* ai, std::string name) : Action(ai, name) {}

    protected:
        static const uint32 NPC_THEKAL  = 14509;
        static const uint32 NPC_LORKHAN = 11347;
        static const uint32 NPC_ZATH    = 11348;
        static const uint32 NPC_TIGER   = 11361;

        static constexpr float HOLD_HP_PCT = 12.0f;

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            std::list<ObjectGuid> targets = AI_VALUE(std::list<ObjectGuid>, "possible attack targets");

            for (ObjectGuid const& guid : targets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!unit)
                    continue;

                if (!unit->IsAlive())
                    continue;

                if (unit->GetEntry() == entry)
                    return unit;
            }

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget && currentTarget->IsAlive() && currentTarget->GetEntry() == entry)
                return currentTarget;

            return nullptr;
        }

        float GetHealthPct(Unit* unit)
        {
            if (!unit || !unit->GetMaxHealth())
                return 0.0f;

            return 100.0f * float(unit->GetHealth()) / float(unit->GetMaxHealth());
        }

        bool SetCurrentTarget(Unit* target)
        {
            if (!target)
                return false;

            context->GetValue<Unit*>("current target")->Set(target);
            ai->GetBot()->SetSelectionGuid(target->GetObjectGuid());

            return true;
        }

        Unit* GetHighestHpTarget(std::vector<Unit*> const& targets)
        {
            Unit* best = nullptr;
            float bestHp = -1.0f;

            for (Unit* target : targets)
            {
                if (!target || !target->IsAlive())
                    continue;

                float hp = GetHealthPct(target);
                if (hp > bestHp)
                {
                    bestHp = hp;
                    best = target;
                }
            }

            return best;
        }

        Unit* GetHighestHpTargetAboveHold(std::vector<Unit*> const& targets)
        {
            Unit* best = nullptr;
            float bestHp = -1.0f;

            for (Unit* target : targets)
            {
                if (!target || !target->IsAlive())
                    continue;

                float hp = GetHealthPct(target);

                if (hp <= HOLD_HP_PCT)
                    continue;

                if (hp > bestHp)
                {
                    bestHp = hp;
                    best = target;
                }
            }

            return best;
        }
    };

    class TargetThekalTigerAction : public ThekalActionBase
    {
    public:
        TargetThekalTigerAction(PlayerbotAI* ai)
            : ThekalActionBase(ai, "target thekal tiger") {}

        bool Execute(Event event) override
        {
            return SetCurrentTarget(FindAliveCreature(NPC_TIGER));
        }
    };


    class InterruptLorkhanAction : public ThekalActionBase
    {
    public:
        InterruptLorkhanAction(PlayerbotAI* ai)
            : ThekalActionBase(ai, "interrupt lorkhan") {}

        bool Execute(Event event) override
        {
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            if (!lorkhan)
                return false;

            SetCurrentTarget(lorkhan);

            if (ai->CastSpell("kick", lorkhan))
                return true;

            if (ai->CastSpell("counterspell", lorkhan))
                return true;

            if (ai->CastSpell("pummel", lorkhan))
                return true;

            if (ai->CastSpell("shield bash", lorkhan))
                return true;

            if (ai->CastSpell("earth shock", lorkhan))
                return true;

            if (ai->CastSpell("spell lock", lorkhan))
                return true;

            if (ai->CastSpell("hammer of justice", lorkhan))
                return true;

            return false;
        }
    };


    class CurseOfTonguesLorkhanAction : public ThekalActionBase
    {
    public:
        CurseOfTonguesLorkhanAction(PlayerbotAI* ai)
            : ThekalActionBase(ai, "curse of tongues lorkhan") {}

        bool Execute(Event event) override
        {
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            if (!lorkhan)
                return false;

            SetCurrentTarget(lorkhan);
            return ai->CastSpell("curse of tongues", lorkhan);
        }
    };


    class BalanceThekalTrioAction : public ThekalActionBase
    {
    public:
        BalanceThekalTrioAction(PlayerbotAI* ai)
            : ThekalActionBase(ai, "balance thekal trio") {}

        bool Execute(Event event) override
        {
            Unit* thekal  = FindAliveCreature(NPC_THEKAL);
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            Unit* zath    = FindAliveCreature(NPC_ZATH);

            std::vector<Unit*> trio;

            if (thekal)
                trio.push_back(thekal);

            if (lorkhan)
                trio.push_back(lorkhan);

            if (zath)
                trio.push_back(zath);

            if (trio.empty())
                return false;

            Player* bot = ai->GetBot();

            // Rogue behavior is different:
            // rogue pressures Lor'Khan, but only while Lor'Khan is above hold HP.
            if (bot->getClass() == CLASS_ROGUE)
            {
                if (lorkhan && GetHealthPct(lorkhan) > HOLD_HP_PCT)
                    return SetCurrentTarget(lorkhan);

                Unit* target = GetHighestHpTargetAboveHold(trio);
                if (target)
                    return SetCurrentTarget(target);

                return SetCurrentTarget(GetHighestHpTarget(trio));
            }

            // Generic behavior:
            // balance the trio by always attacking the highest-health target above hold HP.
            Unit* target = GetHighestHpTargetAboveHold(trio);
            if (target)
                return SetCurrentTarget(target);

            return SetCurrentTarget(GetHighestHpTarget(trio));
        }
    };


    class FinishThekalTrioAction : public ThekalActionBase
    {
    public:
        FinishThekalTrioAction(PlayerbotAI* ai)
            : ThekalActionBase(ai, "finish thekal trio") {}

        bool Execute(Event event) override
        {
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            Unit* zath    = FindAliveCreature(NPC_ZATH);
            Unit* thekal  = FindAliveCreature(NPC_THEKAL);

            // Burn healer first.
            if (lorkhan)
                return SetCurrentTarget(lorkhan);

            if (zath)
                return SetCurrentTarget(zath);

            if (thekal)
                return SetCurrentTarget(thekal);

            return false;
        }
    };

    class ZulGurubActionContext : public NamedObjectContext<Action>
    {
    public:
        ZulGurubActionContext()
        {
            creators["enable zul gurub strategy"] = [](PlayerbotAI* ai){return new ChangeAllStrategyAction(ai, "enable zul gurub strategy", "+zul gurub");};
            creators["disable zul gurub strategy"] = [](PlayerbotAI* ai){return new ChangeAllStrategyAction(ai, "disable zul gurub strategy", "-zul gurub");};

            // High Priestess Jeklik (Bat)
            creators["enable jeklik fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable jeklik fight strategy", "+jeklik");};
            creators["disable jeklik fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable jeklik fight strategy", "-jeklik");};
            
            // High Priest Venoxis (Snake)
            creators["enable venoxis fight strategy"] = [](PlayerbotAI* ai){return new ChangeAllStrategyAction(ai, "enable venoxis fight strategy", "+venoxis");};
            creators["disable venoxis fight strategy"] = [](PlayerbotAI* ai){return new ChangeAllStrategyAction(ai, "disable venoxis fight strategy", "-venoxis");};
            
            // High Priestess Mar'li (Spider)
            creators["enable marli fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable marli fight strategy", "+marli");};
            creators["disable marli fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable marli fight strategy", "-marli");};
            
            // Bloodlord Mandokir
            creators["enable mandokir fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable mandokir fight strategy", "+mandokir");};
            creators["disable mandokir fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable mandokir fight strategy", "-mandokir");};
            
            // Gahz'ranka
            creators["enable gahzranka fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable gahzranka fight strategy", "+gahzranka");};
            creators["disable gahzranka fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable gahzranka fight strategy", "-gahzranka");};
            
            // High Priest Thekal (Tiger)
            creators["enable thekal fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable thekal fight strategy", "+thekal");};
            creators["disable thekal fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable thekal fight strategy", "-thekal");};
            creators["attack thekal tiger"] = [](PlayerbotAI* ai){return new AttackCreatureAction(ai, "attack thekal tiger", NPC_ZULIAN_TIGER);};
            creators["interrupt lorkhan"] = [](PlayerbotAI* ai){return new InterruptCreatureAction(ai, "interrupt lorkhan", NPC_ZEALOT_LORKHAN);};
            creators["curse of tongues lorkhan"] = [](PlayerbotAI* ai){return new CastSpellOnCreatureAction(ai, "curse of tongues lorkhan", "curse of tongues", NPC_ZEALOT_LORKHAN);};
            creators["balance thekal trio"] = [](PlayerbotAI* ai){return new BalanceThekalTrioAction(ai);};
            creators["finish thekal trio"] = [](PlayerbotAI* ai){return new FinishThekalTrioAction(ai);};

            // High Priestess Arlokk (Panther)
            creators["enable arlokk fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable arlokk fight strategy", "+arlokk");};
            creators["disable arlokk fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable arlokk fight strategy", "-arlokk");};
            
            // Jin'do the Hexxer
            creators["enable jindo fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable jindo fight strategy", "+jindo");};
            creators["disable jindo fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable jindo fight strategy", "-jindo");};
            
            // Hakkar
            creators["enable hakkar fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable hakkar fight strategy", "+hakkar");};
            creators["disable hakkar fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable hakkar fight strategy", "-hakkar");};

            // Avoidance and Hazards (NPC 14507 = Venoxis)
            creators["move away from venoxis"] = [](PlayerbotAI* ai){ return new MoveAwayFromCreature(ai, "move away from venoxis", 14507, 31.0f);};
            creators["move away from hazard"] = [](PlayerbotAI* ai){return new MoveAwayFromHazard(ai, "move away from venoxis poison cloud");};
            
            // Interactable bindings
            creators["move to zg interactable"] = &ZulGurubActionContext::move_to_zg_interactable;
            creators["use gurubashi mojo madness"] = &ZulGurubActionContext::use_gurubashi_mojo_madness;
            creators["use mudskunk lure"] = &ZulGurubActionContext::use_mudskunk_lure;
        }
    
      
        private:
        static Action* move_to_zg_interactable(PlayerbotAI* ai) { return new MoveToZGInteractableAction(ai); }
        static Action* use_gurubashi_mojo_madness(PlayerbotAI* ai) { return new UseGurubashiMojoAction(ai); }
        static Action* use_mudskunk_lure(PlayerbotAI* ai) { return new UseMudskunkLureAction(ai); }

    };
}
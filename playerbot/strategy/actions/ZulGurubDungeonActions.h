#pragma once
#include "DungeonActions.h"
#include "ChangeStrategyAction.h"
#include "UseItemAction.h"
#include "playerbot/strategy/values/RtiTargetValue.h"
#include "Groups/Group.h"

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


    class ThekalRtiActionBase : public Action
    {
    public:
        ThekalRtiActionBase(PlayerbotAI* ai, std::string name) : Action(ai, name) {}

    protected:
        static const uint32 NPC_THEKAL  = 14509;
        static const uint32 NPC_LORKHAN = 11347;
        static const uint32 NPC_ZATH    = 11348;
        static const uint32 NPC_TIGER   = 11361;

        static constexpr float HOLD_HP_PCT   = 12.0f;
        static constexpr float FINISH_HP_PCT = 12.0f;

    protected:
        Unit* FindAliveCreature(uint32 entry)
        {
            std::list<ObjectGuid> attackTargets = AI_VALUE(std::list<ObjectGuid>, "possible attack targets");

            for (ObjectGuid const& guid : attackTargets)
            {
                Unit* unit = ai->GetUnit(guid);
                if (!unit)
                    continue;

                if (!unit->IsAlive())
                    continue;

                if (unit->GetEntry() == entry)
                    return unit;
            }

            std::list<ObjectGuid> possibleTargets = AI_VALUE(std::list<ObjectGuid>, "possible targets");

            for (ObjectGuid const& guid : possibleTargets)
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
        bool ShouldSkipDpsRtiSelection()
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return true;

            if (ai->IsHeal(bot))
                return true;

            // Use this only if your PlayerbotAI has IsTank().
            // If it does not compile, replace it with your framework's tank-role check.
            if (ai->IsTank(bot))
                return true;

            return false;
        }

        float GetHealthPct(Unit* unit)
        {
            if (!unit || !unit->GetMaxHealth())
                return 0.0f;

            return 100.0f * float(unit->GetHealth()) / float(unit->GetMaxHealth());
        }

        bool SetRti(std::string rti)
        {
            context->GetValue<std::string>("rti")->Set(rti);
            return true;
        }

        std::string GetIconForUnit(Unit* unit)
        {
            if (!unit)
                return "none";

            switch (unit->GetEntry())
            {
                case NPC_LORKHAN:
                    return "skull";
                case NPC_ZATH:
                    return "cross";
                case NPC_THEKAL:
                    return "square";
                case NPC_TIGER:
                    return "triangle";
                default:
                    return "none";
            }
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

        Unit* GetHighestHpTargetAbove(std::vector<Unit*> const& targets, float threshold)
        {
            Unit* best = nullptr;
            float bestHp = -1.0f;

            for (Unit* target : targets)
            {
                if (!target || !target->IsAlive())
                    continue;

                float hp = GetHealthPct(target);

                if (hp <= threshold)
                    continue;

                if (hp > bestHp)
                {
                    bestHp = hp;
                    best = target;
                }
            }

            return best;
        }

        bool SetTargetIcon(std::string icon, Unit* target)
        {
            if (!target)
                return false;

            Group* group = bot->GetGroup();
            if (!group)
                return false;

            if (bot->InBattleGround())
                return false;

            int index = RtiTargetValue::GetRtiIndex(icon);
            if (index < 0)
                return false;

            ObjectGuid currentGuid = group->GetTargetIcon(index);
            if (currentGuid == target->GetObjectGuid())
                return true;

    #ifndef MANGOSBOT_TWO
            group->SetTargetIcon(index, target->GetObjectGuid());
    #else
            group->SetTargetIcon(index, bot->GetObjectGuid(), target->GetObjectGuid());
    #endif

            return true;
        }

        bool IsTrioReadyToFinish()
        {
            Unit* thekal  = FindAliveCreature(NPC_THEKAL);
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            Unit* zath    = FindAliveCreature(NPC_ZATH);

            if (!thekal || !lorkhan || !zath)
                return false;

            return GetHealthPct(thekal)  <= FINISH_HP_PCT &&
                GetHealthPct(lorkhan) <= FINISH_HP_PCT &&
                GetHealthPct(zath)    <= FINISH_HP_PCT;
        }
    };

    class MarkThekalTargetsAction : public ThekalRtiActionBase
    {
    public:
        MarkThekalTargetsAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "mark thekal targets") {}

        bool Execute(Event& event) override
        {
            bool marked = false;

            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            Unit* zath    = FindAliveCreature(NPC_ZATH);
            Unit* thekal  = FindAliveCreature(NPC_THEKAL);
            Unit* tiger   = FindAliveCreature(NPC_TIGER);

            if (lorkhan)
                marked |= SetTargetIcon("skull", lorkhan);

            if (zath)
                marked |= SetTargetIcon("cross", zath);

            if (thekal)
                marked |= SetTargetIcon("square", thekal);

            if (tiger)
                marked |= SetTargetIcon("triangle", tiger);

            return marked;
        }
    };

    class SelectThekalTigerRtiAction : public ThekalRtiActionBase
    {
    public:
        SelectThekalTigerRtiAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "select thekal tiger rti") {}

        bool Execute(Event& event) override
        {
            if (ShouldSkipDpsRtiSelection())
                return false;
            Unit* tiger = FindAliveCreature(NPC_TIGER);
            if (!tiger)
                return false;

            SetTargetIcon("triangle", tiger);
            return SetRti("triangle");
        }
    };


    class InterruptLorkhanAction : public ThekalRtiActionBase
    {
    public:
        InterruptLorkhanAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "interrupt lorkhan") {}

        bool Execute(Event& event) override
        {
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            if (!lorkhan)
                return false;

            if (!lorkhan->IsNonMeleeSpellCasted(true))
                return false;

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


    class CurseOfTonguesLorkhanAction : public ThekalRtiActionBase
    {
    public:
        CurseOfTonguesLorkhanAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "curse of tongues lorkhan") {}

        bool Execute(Event& event) override
        {
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            if (!lorkhan)
                return false;

            if (ai->HasAura("curse of tongues", lorkhan))
                return false;

            return ai->CastSpell("curse of tongues", lorkhan);
        }
    };

    class SelectBalancedThekalRtiAction : public ThekalRtiActionBase
    {
    public:
        SelectBalancedThekalRtiAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "select balanced thekal rti") {}

        bool Execute(Event& event) override
        {
            if (ShouldSkipDpsRtiSelection())
                return false;
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

            Unit* target = nullptr;

            // Rogue-specific:
            // Rogue pressures Lor'Khan, but stops before killing her too early.
            if (bot->getClass() == CLASS_ROGUE)
            {
                if (lorkhan && GetHealthPct(lorkhan) > HOLD_HP_PCT)
                    target = lorkhan;
                else
                    target = GetHighestHpTargetAbove(trio, HOLD_HP_PCT);
            }
            else
            {
                // Generic:
                // Attack highest-health trio member above hold threshold.
                target = GetHighestHpTargetAbove(trio, HOLD_HP_PCT);
            }

            if (!target)
                target = GetHighestHpTarget(trio);

            std::string icon = GetIconForUnit(target);
            if (icon == "none")
                return false;

            SetTargetIcon(icon, target);
            return SetRti(icon);
        }
    };

    class SelectFinishThekalRtiAction : public ThekalRtiActionBase
    {
    public:
        SelectFinishThekalRtiAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "select finish thekal rti") {}

        bool Execute(Event& event) override
        {
            if (ShouldSkipDpsRtiSelection())
                return false;
            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            Unit* zath    = FindAliveCreature(NPC_ZATH);
            Unit* thekal  = FindAliveCreature(NPC_THEKAL);

            Unit* target = nullptr;

            // During final burn, kill the healer first.
            if (lorkhan)
                target = lorkhan;
            else if (zath)
                target = zath;
            else if (thekal)
                target = thekal;

            if (!target)
                return false;

            std::string icon = GetIconForUnit(target);
            if (icon == "none")
                return false;

            SetTargetIcon(icon, target);
            return SetRti(icon);
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
            creators["interrupt lorkhan"] = [](PlayerbotAI* ai){ return new InterruptLorkhanAction(ai);};
            creators["curse of tongues lorkhan"] = [](PlayerbotAI* ai){return new CurseOfTonguesLorkhanAction(ai);};
            creators["mark thekal targets"] = [](PlayerbotAI* ai){return new MarkThekalTargetsAction(ai);};
            creators["select thekal tiger rti"] = [](PlayerbotAI* ai){return new SelectThekalTigerRtiAction(ai);};
            creators["select balanced thekal rti"] = [](PlayerbotAI* ai){return new SelectBalancedThekalRtiAction(ai);};
            creators["select finish thekal rti"] = [](PlayerbotAI* ai){return new SelectFinishThekalRtiAction(ai);};


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
            creators["move away from hazard"] = [](PlayerbotAI* ai){return new MoveAwayFromHazard(ai, "move away from hazard");};
            
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
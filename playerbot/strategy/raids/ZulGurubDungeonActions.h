#pragma once
#include "../actions/DungeonActions.h"
#include "../actions/ChangeStrategyAction.h"
#include "../actions/UseItemAction.h"
#include "playerbot/strategy/values/RtiTargetValue.h"
#include "Groups/Group.h"
#include "RaidIconActionBase.h"
#include "ZulGurubDungeonUtils.h"

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



    class MoveHakkarToTankPositionAction : public MovementAction
    {
    public:
        MoveHakkarToTankPositionAction(PlayerbotAI* ai)
            : MovementAction(ai, "move hakkar to tank position") {}

        bool Execute(Event& event) override
        {
            if (ZulGurubDungeonUtils::IsAtHakkarTankPosition(bot))
                return false;

            return MoveTo(
                bot->GetMapId(),
                ZulGurubDungeonUtils::HAKKAR_TANK_X,
                ZulGurubDungeonUtils::HAKKAR_TANK_Y,
                ZulGurubDungeonUtils::HAKKAR_TANK_Z,
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
    };


    class CrowdControlHakkarMindControlAction : public Action
    {
    public:
        CrowdControlHakkarMindControlAction(PlayerbotAI* ai)
            : Action(ai, "cc hakkar mind control target") {}

        bool Execute(Event& event) override
        {
            return ZulGurubDungeonUtils::CastCcOnMindControlledTarget(ai, bot);
        }

        bool isPossible() override
        {
            return ZulGurubDungeonUtils::CanCcMindControlledTarget(ai, bot);
        }
    };

    

    class CurseOfTonguesJeklikAction : public RaidIconActionBase
    {
    public:
        CurseOfTonguesJeklikAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "curse of tongues jeklik") {}

        bool Execute(Event& event) override
        {
            Player* bot = ai->GetBot();
            if (!bot || bot->getClass() != CLASS_WARLOCK)
                return false;

            Unit* jeklik = FindAliveCreature(14517);
            if (!jeklik)
                return false;

            if (ai->HasAura("curse of tongues", jeklik))
                return false;

            return ai->CastSpell("curse of tongues", jeklik);
        }
    };

    class InterruptJeklikAction : public RaidIconActionBase
    {
    public:
        InterruptJeklikAction(PlayerbotAI* ai)
            : RaidIconActionBase(ai, "interrupt jeklik") {}

        bool Execute(Event& event) override
        {
            Unit* jeklik = FindAliveCreature(14517);
            if (!jeklik)
                return false;

            if (!jeklik->IsNonMeleeSpellCasted(true))
                return false;

            if (ai->CastSpell("silence", jeklik))
                return true;

            if (ai->CastSpell("kick", jeklik))
                return true;

            if (ai->CastSpell("counterspell", jeklik))
                return true;

            if (ai->CastSpell("pummel", jeklik))
                return true;

            if (ai->CastSpell("shield bash", jeklik))
                return true;

            if (ai->CastSpell("earth shock", jeklik))
                return true;

            if (ai->CastSpell("spell lock", jeklik))
                return true;

            if (ai->CastSpell("hammer of justice", jeklik))
                return true;

            return false;
        }
    };

    class ThekalRtiActionBase : public RaidIconActionBase
    {
    public:
        ThekalRtiActionBase(PlayerbotAI* ai, std::string name) : RaidIconActionBase(ai, name) {}

    protected:
        static const uint32 NPC_THEKAL  = 14509;
        static const uint32 NPC_LORKHAN = 11347;
        static const uint32 NPC_ZATH    = 11348;
        static const uint32 NPC_TIGER   = 11361;

        static constexpr float HOLD_HP_PCT   = 15.0f;
        static constexpr float FINISH_HP_PCT = 7.0f;

    protected:
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

        bool IsThekalTankTarget(Unit* unit)
        {
            if (!unit || !unit->IsAlive())
                return false;

            switch (unit->GetEntry())
            {
                case NPC_TIGER:
                case NPC_THEKAL:
                case NPC_ZATH:
                    return true;

                case NPC_LORKHAN:
                default:
                    return false;
            }
        }

        Unit* GetPreferredThekalTankTarget()
        {
            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (IsThekalTankTarget(currentTarget))
                return currentTarget;

            Unit* tiger = FindAliveCreature(NPC_TIGER);
            if (tiger)
                return tiger;

            Unit* thekal = FindAliveCreature(NPC_THEKAL);
            if (thekal)
                return thekal;

            Unit* zath = FindAliveCreature(NPC_ZATH);
            if (zath)
                return zath;

            return nullptr;
        }

        bool SelectOwnRti(std::string const& icon, Unit* target)
        {
            if (!target)
                return false;

            if (icon == "none")
                return false;

            bool changed = false;
            changed |= SetTargetIcon(icon, target);
            changed |= SetRti(icon);

            return changed;
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

    class SelectThekalTankRtiAction : public ThekalRtiActionBase
    {
    public:
        SelectThekalTankRtiAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "select thekal tank rti") {}

        bool Execute(Event& event) override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            if (!ai->IsTank(bot))
                return false;

            Unit* target = GetPreferredThekalTankTarget();
            if (!target)
                return false;

            // Hard safety: tanks should never intentionally select Lor'Khan.
            if (target->GetEntry() == NPC_LORKHAN)
                return false;

            std::string icon = GetIconForUnit(target);
            return SelectOwnRti(icon, target);
        }
    };

    class MarkThekalTargetsAction : public ThekalRtiActionBase
    {
    public:
        MarkThekalTargetsAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "mark thekal targets") {}

        bool Execute(Event& event) override
        {
            return SetTargetIconsByEntry({
                {"skull",    NPC_LORKHAN},
                {"cross",    NPC_ZATH},
                {"square",   NPC_THEKAL},
                {"triangle", NPC_TIGER}
            });
        }
    };

    class SelectThekalTigerRtiAction : public ThekalRtiActionBase
    {
    public:
        SelectThekalTigerRtiAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "select thekal tiger rti") {}

        bool Execute(Event& event) override
        {
            return SelectDpsRti("triangle", NPC_TIGER);
        }
    };


    class InterruptLorkhanAction : public ThekalRtiActionBase
    {
    public:
        InterruptLorkhanAction(PlayerbotAI* ai)
            : ThekalRtiActionBase(ai, "interrupt lorkhan") {}

        bool Execute(Event& event) override
        {
            Player* bot = ai->GetBot();
            if (!bot)
                return false;

            Unit* lorkhan = FindAliveCreature(NPC_LORKHAN);
            if (!lorkhan)
                return false;

            if (!lorkhan->IsNonMeleeSpellCasted(true))
                return false;

            switch (bot->getClass())
            {
                case CLASS_MAGE:
                    return TryCastOnUnit("counterspell", lorkhan);

                case CLASS_ROGUE:
                    return TryRogueInterruptLorkhan(lorkhan);

                case CLASS_WARRIOR:
                    if (TryCastOnUnit("pummel", lorkhan))
                        return true;

                    return TryCastOnUnit("shield bash", lorkhan);

                case CLASS_SHAMAN:
                    return TryCastOnUnit("earth shock", lorkhan);

                case CLASS_WARLOCK:
                    return TryCastOnUnit("fear", lorkhan);

                case CLASS_PALADIN:
                    return TryCastOnUnit("hammer of justice", lorkhan);

                // Important:
                // Druids should not root Lor'Khan.
                // Do not cast entangling roots here.
                case CLASS_DRUID:
                    return false;

                default:
                    return false;
            }
        }
        
    private:
        bool TryRogueInterruptLorkhan(Unit* lorkhan)
        {
            if (!lorkhan)
                return false;

            // Primary rogue interrupt.
            if (TryCastOnUnit("kick", lorkhan))
                return true;

            // Finisher stun fallback.
            // Only try Kidney Shot if Lor'Khan is the current combo-point target.
            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget &&
                currentTarget->GetObjectGuid() == lorkhan->GetObjectGuid())
            {
                uint8 comboPoints = AI_VALUE2(uint8, "combo", "current target");
                if (comboPoints > 0)
                {
                    if (TryCastOnUnit("kidney shot", lorkhan))
                        return true;
                }
            }

            // Last-resort interrupt/control.
            // Spelled "gouge", not "gauge".
            if (TryCastOnUnit("gouge", lorkhan))
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

            bool changed = false;
            changed |= SetTargetIcon(icon, target);
            changed |= SetRti(icon);

            return changed;
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

            // During final burn, kill the thekal first.
            if (thekal)
                target = thekal;
            else if (lorkhan)
                target = lorkhan;
            else if (zath)
                target = zath;
   

            if (!target)
                return false;

            std::string icon = GetIconForUnit(target);
            if (icon == "none")
                return false;

            bool changed = false;
            changed |= SetTargetIcon(icon, target);
            changed |= SetRti(icon);

            return changed;
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
            creators["interrupt jeklik"] = [](PlayerbotAI* ai){return new InterruptJeklikAction(ai);};
            creators["curse of tongues jeklik"] = [](PlayerbotAI* ai){return new CurseOfTonguesJeklikAction(ai);};
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
            creators["select thekal tank rti"] = [](PlayerbotAI* ai){return new SelectThekalTankRtiAction(ai);};

            // High Priestess Arlokk (Panther)
            creators["enable arlokk fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable arlokk fight strategy", "+arlokk");};
            creators["disable arlokk fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable arlokk fight strategy", "-arlokk");};
            
            // Jin'do the Hexxer
            creators["enable jindo fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable jindo fight strategy", "+jindo");};
            creators["disable jindo fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable jindo fight strategy", "-jindo");};
            
            // Hakkar
            creators["enable hakkar fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "enable hakkar fight strategy", "+hakkar");};
            creators["disable hakkar fight strategy"] = [](PlayerbotAI* ai) { return new ChangeAllStrategyAction(ai, "disable hakkar fight strategy", "-hakkar");};
            creators["move hakkar to tank position"] = [](PlayerbotAI* ai){return new MoveHakkarToTankPositionAction(ai);};
            creators["cc hakkar mind control target"] = [](PlayerbotAI* ai){return new CrowdControlHakkarMindControlAction(ai);};
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
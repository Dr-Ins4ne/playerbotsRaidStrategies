#pragma once

#include "playerbot/playerbot.h"
#include "playerbot/strategy/Strategy.h"
#include "playerbot/strategy/Trigger.h"
#include "../actions/ChangeStrategyAction.h"
#include "../triggers/DungeonTriggers.h"
#include "DungeonTargetHelper.h"

#include <cstddef>
#include <list>
#include <string>

namespace ai
{
    struct DungeonBossDefinition
    {
        const char* name;
        uint32 entry;
    };

    class DungeonBossVisibleStartTrigger : public Trigger
    {
    public:
        DungeonBossVisibleStartTrigger(
            PlayerbotAI* ai,
            const std::string& triggerName,
            const std::string& bossStrategy,
            uint32 bossEntry)
            : Trigger(ai, triggerName, 1),
              bossStrategy(bossStrategy),
              bossEntry(bossEntry)
        {
        }

        bool IsActive() override
        {
            if (ai->HasStrategy(bossStrategy, BotState::BOT_STATE_COMBAT))
                return false;

            if (!bot || !bot->IsInWorld() || bot->IsBeingTeleported())
                return false;

            return DungeonTargetHelper::FindAliveCreature(ai, bossEntry) != nullptr;
        }

    private:
        std::string bossStrategy;
        uint32 bossEntry;
    };

    class DungeonBossLifecycle
    {
    public:
        static std::string StartTriggerName(const std::string& bossName)
        {
            return "start " + bossName + " fight";
        }

        static std::string EndTriggerName(const std::string& bossName)
        {
            return "end " + bossName + " fight";
        }

        static std::string EnableActionName(const std::string& bossName)
        {
            return "enable " + bossName + " fight strategy";
        }

        static std::string DisableActionName(const std::string& bossName)
        {
            return "disable " + bossName + " fight strategy";
        }

        static std::string EnableStrategyCommand(const std::string& bossName)
        {
            return "+" + bossName;
        }

        static std::string DisableStrategyCommand(const std::string& bossName)
        {
            return "-" + bossName;
        }

        static void AddStartTriggers(
            std::list<TriggerNode*>& triggers,
            const DungeonBossDefinition* bosses,
            size_t count,
            float priority = 100.0f)
        {
            for (size_t i = 0; i < count; ++i)
            {
                const std::string bossName = bosses[i].name;
                const std::string startTriggerName = StartTriggerName(bossName);
                const std::string enableActionName = EnableActionName(bossName);

                triggers.push_back(new TriggerNode(
                    startTriggerName,
                    NextAction::array(0,
                        new NextAction(enableActionName, priority),
                        NULL)));
            }
        }

        template <typename CreatorMap>
        static void RegisterStrategyActions(
            CreatorMap& creators,
            const DungeonBossDefinition* bosses,
            size_t count)
        {
            for (size_t i = 0; i < count; ++i)
            {
                const std::string bossName = bosses[i].name;

                const std::string enableActionName = EnableActionName(bossName);
                const std::string disableActionName = DisableActionName(bossName);
                const std::string enableCommand = EnableStrategyCommand(bossName);
                const std::string disableCommand = DisableStrategyCommand(bossName);

                creators[enableActionName] =
                    [enableActionName, enableCommand](PlayerbotAI* ai)
                    {
                        return new ChangeAllStrategyAction(
                            ai,
                            enableActionName,
                            enableCommand);
                    };

                creators[disableActionName] =
                    [disableActionName, disableCommand](PlayerbotAI* ai)
                    {
                        return new ChangeAllStrategyAction(
                            ai,
                            disableActionName,
                            disableCommand);
                    };
            }
        }

        template <typename CreatorMap>
        static void RegisterBossFightTriggers(
            CreatorMap& creators,
            const DungeonBossDefinition* bosses,
            size_t count)
        {
            for (size_t i = 0; i < count; ++i)
            {
                const std::string bossName = bosses[i].name;
                const uint32 bossEntry = bosses[i].entry;

                const std::string startTriggerName = StartTriggerName(bossName);
                const std::string endTriggerName = EndTriggerName(bossName);

                creators[startTriggerName] =
                    [startTriggerName, bossName, bossEntry](PlayerbotAI* ai)
                    {
                        return new DungeonBossVisibleStartTrigger(
                            ai,
                            startTriggerName,
                            bossName,
                            bossEntry);
                    };

                creators[endTriggerName] =
                    [endTriggerName, bossName, bossEntry](PlayerbotAI* ai)
                    {
                        return new EndBossFightTrigger(
                            ai,
                            endTriggerName,
                            bossName,
                            bossEntry);
                    };
            }
        }
    };

    class DungeonBossStrategy : public Strategy
    {
    public:
        DungeonBossStrategy(PlayerbotAI* ai, const std::string& bossName)
            : Strategy(ai),
              bossName(bossName)
        {
        }

        std::string getName() override
        {
            return bossName;
        }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override final
        {
            AddEndFightTrigger(triggers);
            InitBossNonCombatTriggers(triggers);
        }

        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override final
        {
            AddEndFightTrigger(triggers);
            InitBossDeadTriggers(triggers);
        }

    protected:
        virtual void InitBossNonCombatTriggers(std::list<TriggerNode*>& triggers) {}
        virtual void InitBossDeadTriggers(std::list<TriggerNode*>& triggers) {}

    private:
        void AddEndFightTrigger(std::list<TriggerNode*>& triggers)
        {
            const std::string endTriggerName = DungeonBossLifecycle::EndTriggerName(bossName);
            const std::string disableActionName = DungeonBossLifecycle::DisableActionName(bossName);

            triggers.push_back(new TriggerNode(
                endTriggerName,
                NextAction::array(0,
                    new NextAction(disableActionName, 100.0f),
                    NULL)));
        }

    private:
        std::string bossName;
    };
}
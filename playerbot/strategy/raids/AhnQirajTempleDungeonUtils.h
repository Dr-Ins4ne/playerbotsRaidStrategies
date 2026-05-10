#pragma once

#include "DungeonTargetHelper.h"
#include "playerbot/playerbot.h"

#include <algorithm>
#include <vector>

namespace ai
{
    namespace AhnQirajTemple
    {
        static constexpr uint32 MAP_AHNQIRAJ_TEMPLE = 531;
        static constexpr uint32 NPC_PROPHET_SKERAM = 15263;

        inline bool IsSkeram(Unit* unit)
        {
            return unit && unit->IsAlive() && unit->GetEntry() == NPC_PROPHET_SKERAM;
        }

        inline std::vector<Unit*> GetSkeramUnits(PlayerbotAI* ai)
        {
            return DungeonTargetHelper::FindAliveCreatures(ai, NPC_PROPHET_SKERAM);
        }

        inline Unit* GetRealSkeram(PlayerbotAI* ai)
        {
            std::vector<Unit*> units = GetSkeramUnits(ai);

            Unit* best = nullptr;
            uint32 bestMaxHealth = 0;
            uint32 bestHealth = 0;

            for (Unit* unit : units)
            {
                if (!IsSkeram(unit))
                    continue;

                uint32 maxHealth = unit->GetMaxHealth();
                uint32 health = unit->GetHealth();

                // Real Skeram should have the highest absolute/max health.
                // Do not use health percentage here: images often spawn at 100%.
                if (!best ||
                    maxHealth > bestMaxHealth ||
                    (maxHealth == bestMaxHealth && health > bestHealth))
                {
                    best = unit;
                    bestMaxHealth = maxHealth;
                    bestHealth = health;
                }
            }

            return best;
        }

        inline bool IsRealSkeram(PlayerbotAI* ai, Unit* unit)
        {
            if (!IsSkeram(unit))
                return false;

            Unit* real = GetRealSkeram(ai);
            return real && real->GetObjectGuid() == unit->GetObjectGuid();
        }

        inline bool IsSkeramImage(PlayerbotAI* ai, Unit* unit)
        {
            if (!IsSkeram(unit))
                return false;

            Unit* real = GetRealSkeram(ai);
            if (!real)
                return false;

            return real->GetObjectGuid() != unit->GetObjectGuid();
        }

        inline std::vector<Unit*> GetSkeramImages(PlayerbotAI* ai)
        {
            std::vector<Unit*> units = GetSkeramUnits(ai);
            Unit* real = GetRealSkeram(ai);

            std::vector<Unit*> images;
            for (Unit* unit : units)
            {
                if (!IsSkeram(unit))
                    continue;

                if (real && unit->GetObjectGuid() == real->GetObjectGuid())
                    continue;

                images.push_back(unit);
            }

            std::stable_sort(images.begin(), images.end(), [](Unit* left, Unit* right)
            {
                if (!left)
                    return false;
                if (!right)
                    return true;

                return left->GetHealth() > right->GetHealth();
            });

            return images;
        }

        inline bool HasSkeramImages(PlayerbotAI* ai)
        {
            return !GetSkeramImages(ai).empty();
        }
    }
}

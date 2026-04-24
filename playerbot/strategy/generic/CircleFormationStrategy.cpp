
#include "playerbot/playerbot.h"
#include "CircleFormationStrategy.h"
#include "playerbot/ServerFacade.h"
#include "playerbot/strategy/AiObjectContext.h"
#include "playerbot/strategy/values/PositionValue.h"

using namespace ai;

void CircleFormationStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    Player* bot = ai->GetBot();
    std::string role = GetBotRole(bot); 
    float radius = GetRadiusForRole(role);

    float angleDegrees = 0; 
    Group* group = bot->GetGroup();
    int totalBotsInGroup = 1; 
    int botIndexInGroup = 0;    

    if (group)
    {
        botIndexInGroup = GetBotGroupIndex(bot, group, totalBotsInGroup);
    }
    
    if (totalBotsInGroup > 0 && botIndexInGroup != -1)
    {
        // 1. Calculate the basic slice of the circle
        float angleIncrement = 360.0f / totalBotsInGroup;
        angleDegrees = botIndexInGroup * angleIncrement;

        // 2. LOCK TO MASTER/LEADER INSTEAD OF TARGET
        // By using the Master's orientation at the moment the strategy is initialized,
        // the "North" of your circle is always relative to where the leader was looking.
        Player* master = ai->GetMaster();
        if (master)
        {
            float masterOrient = master->GetOrientation() * (180.0f / M_PI_F);
            angleDegrees = fmod(masterOrient + angleDegrees, 360.0f);
        }
    }
    else 
    {
        angleDegrees = (bot->GetObjectGuid().GetCounter() % 24) * 5.0f;
    }

    // Wrap the degrees to ensure 0-360
    if (angleDegrees < 0) angleDegrees += 360.0f;

    std::ostringstream qualifierStream;
    qualifierStream.precision(1);
    qualifierStream << std::fixed << radius << "," << angleDegrees;
    std::string actionQualifier = qualifierStream.str();

    // Trigger registration remains the same, 
    // but actionQualifier is now "baked" with a static angle.
    triggers.push_back(new TriggerNode(
        "combat start", 
        NextAction::array(0, new NextAction("move to circle formation::" + actionQualifier, ACTION_EMERGENCY+10), NULL))); 
    
    triggers.push_back(new TriggerNode(
        "often", 
        NextAction::array(0, 
            new NextAction("move to circle formation::" + actionQualifier, ACTION_EMERGENCY+10), 
            NULL))); 
}


void CircleFormationStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    /*
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("move to circle formation::30,10", 100.0f), new NextAction("say::combat start", 99.0f), NULL)));


    triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0,new NextAction("move to circle formation::30,10", 100.0f),  NULL)));
        */
 
}

std::string CircleFormationStrategy::GetBotRole(Player* pBot)
{
    PlayerbotAI* botAi = pBot->GetPlayerbotAI();
    if (botAi->HasStrategy("ranged", BotState::BOT_STATE_COMBAT))
    {
        return "ranged";
    }
    else
    {
        return "melee";
    }
  
}

float CircleFormationStrategy::GetRadiusForRole(const std::string& roleName)
{
    if (roleName == "melee")
        return 2;
    else // ranged (and default)
        return 30;
}

int CircleFormationStrategy::GetBotGroupIndex(Player* pBot, Group* group, int& outTotalBotsInFormation)
{
    outTotalBotsInFormation = 0;
    if (!pBot || !group) return -1;

    std::vector<ObjectGuid> groupBotGuids;
    for (GroupReference* ref = group->GetFirstMember(); ref != nullptr; ref = ref->next())
    {
        Player* member = ref->getSource();
        if (member && !member->IsPlayer() && member->IsAlive() && member->GetMapId() == pBot->GetMapId())
        {
            groupBotGuids.push_back(member->GetObjectGuid());
            // We count all playerbots in the group for the total slots,
            // assuming they will all attempt some form of positioning.
            // If only bots with THIS strategy should be counted, the check needs to be more specific.
        }
    }
    
    outTotalBotsInFormation = groupBotGuids.size(); // Total is simply the number of valid playerbots found
    if (outTotalBotsInFormation == 0) return -1;

    std::sort(groupBotGuids.begin(), groupBotGuids.end());

    for (int i = 0; i < groupBotGuids.size(); ++i)
    {
        if (groupBotGuids[i] == pBot->GetObjectGuid())
        {
            return i; 
        }
    }
    return -1; 
}
#pragma once

#include "../world/SimpleWorld.hpp"
#include <string>

namespace nlm {
namespace config {

class AgentBrainConfig {
public:
    static void initializeConfig(AgentBrain& agentBrain, const SimpleWorld& world);
    static void enableRewardModulation(AgentBrain& agentBrain, bool enable);
    static void enableStructuralPlasticity(AgentBrain& agentBrain, bool enable);
    static void enableDevelopment(AgentBrain& agentBrain, bool enable);
    static void enableCuriosity(AgentBrain& agentBrain, bool enable);
    
    static bool isRewardModulationEnabled(const AgentBrain& agentBrain);
    static bool isStructuralPlasticityEnabled(const AgentBrain& agentBrain);
    static bool isDevelopmentEnabled(const AgentBrain& agentBrain);
    static bool isCuriosityEnabled(const AgentBrain& agentBrain);
};

} // namespace config
} // namespace nlm

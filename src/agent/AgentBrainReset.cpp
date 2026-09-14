#include "AgentBrainReset.hpp"
#include "AgentBrain.hpp"
#include <algorithm>

namespace nlm {
namespace reset {

void AgentBrainReset::resetState(AgentBrain& agentBrain) {
    agentBrain.dopamineLevel_ = 0.0f;
    agentBrain.noveltyLevel_ = 0.0f;
    agentBrain.curiosityLevel_ = 0.0f;
    agentBrain.predictionError_ = 0.0f;
    agentBrain.expectedReward_ = 0.0f;
    agentBrain.developmentalAge_ = 0.0;
    agentBrain.plasticityModifier_ = 1.0f;
    
    // Clear previous vision
    std::fill(agentBrain.previousVision_.begin(), agentBrain.previousVision_.end(), 0.0f);
}

} // namespace reset
} // namespace nlm

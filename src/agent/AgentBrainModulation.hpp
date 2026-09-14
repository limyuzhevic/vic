#pragma once

namespace nlm {
namespace modulation {

class AgentBrainModulation {
public:
    static void applyRewardModulation(AgentBrain& agentBrain, float reward, float predictedReward);
};

} // namespace modulation
} // namespace nlm

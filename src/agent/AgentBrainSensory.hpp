#pragma once

#include "SensoryPercept.hpp"

namespace nlm {
namespace sensory {

class AgentBrainSensory {
public:
    static void processSensoryInput(AgentBrain& agentBrain, const SensoryPercept& percept);
};

} // namespace sensory
} // namespace nlm

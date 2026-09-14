#pragma once

namespace nlm {
namespace state {

class AgentBrainState {
public:
    static size_t getSensoryInputSize();
    static size_t getMotorOutputSize();
    static DevelopmentalStage getDevelopmentalStage(const AgentBrain& agentBrain);
    static float getNeuromodulationLevel(const AgentBrain& agentBrain);
    static float getCuriosityLevel(const AgentBrain& agentBrain);
    static float getNoveltyLevel(const AgentBrain& agentBrain);
    static float getPredictionError(const AgentBrain& agentBrain);
};

} // namespace state
} // namespace nlm

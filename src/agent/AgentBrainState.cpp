#include "AgentBrainState.hpp"
#include "AgentBrain.hpp"

namespace nlm {
namespace state {

size_t AgentBrainState::getSensoryInputSize() {
    // Vision (16x16) + touch (8) + internal (4) + proprioception (6)
    return 256 + 8 + 4 + 6;
}

size_t AgentBrainState::getMotorOutputSize() {
    // One motor neuron per action
    return 6;
}

DevelopmentalStage AgentBrainState::getDevelopmentalStage(const AgentBrain& agentBrain) {
    if (!agentBrain.brain_) return DevelopmentalStage::Initial;
    return agentBrain.brain_->getDevelopmentalStage();
}

float AgentBrainState::getNeuromodulationLevel(const AgentBrain& agentBrain) {
    return agentBrain.dopamineLevel_;
}

float AgentBrainState::getCuriosityLevel(const AgentBrain& agentBrain) {
    return agentBrain.curiosityLevel_;
}

float AgentBrainState::getNoveltyLevel(const AgentBrain& agentBrain) {
    return agentBrain.noveltyLevel_;
}

float AgentBrainState::getPredictionError(const AgentBrain& agentBrain) {
    return agentBrain.predictionError_;
}

} // namespace state
} // namespace nlm

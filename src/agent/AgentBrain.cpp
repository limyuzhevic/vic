// Additional advanced features and commands implementation

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace nlm {

// Advanced Configuration Methods
void AgentBrain::enableAttention(bool enable) {
    attentionEnabled_ = enable;
    if (debugMode_) {
        NLM_LOG_INFO("AgentBrain: Attention system " + std::string(enable ? "enabled" : "disabled"));
    }
}

void AgentBrain::enableConceptFormation(bool enable) {
    conceptFormationEnabled_ = enable;
    if (debugMode_) {
        NLM_LOG_INFO("AgentBrain: Concept formation system " + std::string(enable ? "enabled" : "disabled"));
    }
}

void AgentBrain::setSimulationSpeed(float speed) {
    simulationSpeed_ = std::max(0.1f, std::min(10.0f, speed));  // Clamp to reasonable range
    if (debugMode_) {
        NLM_LOG_INFO("AgentBrain: Simulation speed set to " + std::to_string(simulationSpeed_));
    }
}

void AgentBrain::pauseSimulation() {
    simulationPaused_ = true;
    if (debugMode_) {
        NLM_LOG_INFO("AgentBrain: Simulation paused");
    }
}

void AgentBrain::resumeSimulation() {
    simulationPaused_ = false;
    if (debugMode_) {
        NLM_LOG_INFO("AgentBrain: Simulation resumed");
    }
}

void AgentBrain::enableDebugMode(bool enable) {
    debugMode_ = enable;
    if (debugMode_) {
        NLM_LOG_INFO("AgentBrain: Debug mode enabled");
    }
}

void AgentBrain::setPlanningDepth(size_t depth) {
    if (brain_ && brain_->getPlanner()) {
        brain_->getPlanner()->setPlanningDepth(depth);
        if (debugMode_) {
            NLM_LOG_INFO("AgentBrain: Planning depth set to " + std::to_string(depth));
        }
    }
}

void AgentBrain::setMemoryCapacity(size_t capacity) {
    if (brain_ && brain_->getWorkingMemory()) {
        brain_->getWorkingMemory()->setCapacity(capacity);
        if (debugMode_) {
            NLM_LOG_INFO("AgentBrain: Working memory capacity set to " + std::to_string(capacity));
        }
    }
}

void AgentBrain::setDevelopmentStage(DevelopmentalStage stage) {
    if (brain_) {
        brain_->setDevelopmentalStage(stage);
        if (debugMode_) {
            NLM_LOG_INFO("AgentBrain: Development stage set to " + std::to_string(static_cast<int>(stage)));
        }
    }
}

void AgentBrain::injectTestStimulus(float current, NeuronType type) {
    if (brain_) {
        brain_->injectCurrentToNeurons(type, current);
        if (debugMode_) {
            NLM_LOG_INFO("AgentBrain: Injected test stimulus " + std::to_string(current) + 
                        " to neurons of type " + std::to_string(static_cast<int>(type)));
        }
    }
}

bool AgentBrain::saveBrainState(const std::string& filepath) const {
    if (!brain_) {
        if (debugMode_) NLM_LOG_ERROR("AgentBrain: Cannot save - brain is null");
        return false;
    }
    
    bool success = brain_->save(filepath);
    if (success) {
        if (debugMode_) {
            NLM_LOG_INFO("AgentBrain: Brain state saved to " + filepath);
        }
    } else {
        if (debugMode_) NLM_LOG_ERROR("AgentBrain: Failed to save brain state");
    }
    return success;
}

bool AgentBrain::loadBrainState(const std::string& filepath) {
    if (!brain_) {
        if (debugMode_) NLM_LOG_ERROR("AgentBrain: Cannot load - brain is null");
        return false;
    }
    
    bool success = brain_->load(filepath);
    if (success) {
        if (debugMode_) {
            NLM_LOG_INFO("AgentBrain: Brain state loaded from " + filepath);
        }
    } else {
        if (debugMode_) NLM_LOG_ERROR("AgentBrain: Failed to load brain state");
    }
    return success;
}

std::vector<float> AgentBrain::getMemoryActivity() const {
    if (!brain_ || !brain_->getWorkingMemory()) {
        return std::vector<float>();
    }
    return brain_->getWorkingMemory()->retrieve();
}

std::vector<NeuronId> AgentBrain::getMemoryNeurons() const {
    if (!brain_ || !brain_->getWorkingMemory()) {
        return std::vector<NeuronId>();
    }
    return brain_->getWorkingMemory()->getMemoryNeurons();
}

void AgentBrain::resetAllSystems() {
    reset();
    
    // Reset developmental age and plasticity
    developmentalAge_ = 0.0;
    plasticityModifier_ = 1.0f;
    
    // Reset neuromodulation levels
    dopamineLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
    
    if (debugMode_) {
        NLM_LOG_INFO("AgentBrain: All systems reset");
    }
}

// Additional helper methods for advanced analysis
float AgentBrain::getLearningProgress() const {
    if (!brain_) return 0.0f;
    return (brain_->getTotalSpikeCount() / static_cast<float>(std::max(1, brain_->getTotalNeuronCount())));
}

float AgentBrain::getMemoryRetention() const {
    if (!brain_ || !brain_->getEpisodicMemory()) {
        return 0.0f;
    }
    // Normalize to 0-1 range, assuming reasonable number of episodes
    return std::min(1.0f, static_cast<float>(brain_->getEpisodicMemory()->getEpisodeCount()) / 10.0f);
}

bool AgentBrain::isMemorySystemHealthy() const {
    if (!brain_ || !brain_->getWorkingMemory()) {
        return false;
    }
    return brain_->getWorkingMemory()->getMemoryActivity() > 0.1f;
}

bool AgentBrain::isCognitiveSystemHealthy() const {
    if (!brain_) {
        return false;
    }
    return brain_->getPlanner() && brain_->getConceptFormation();
}

bool AgentBrain::isPredictionSystemHealthy() const {
    if (!brain_) {
        return false;
    }
    return brain_->getPredictionSystem();
}

// Update development to use new attention/concept formation flags
void AgentBrain::updateDevelopment(double timestep) {
    // Call original implementation
    AgentBrain::updateDevelopment(timestep);
    
    // Additional development effects based on enabled systems
    if (debugMode_ && developmentalAge_ > 100.0) {  // Log at specific age
        NLM_LOG_INFO("AgentBrain: Development age " + std::to_string(developmentalAge_) +
                    ", Attention: " + std::string(attentionEnabled_ ? "enabled" : "disabled") +
                    ", Concept formation: " + std::string(conceptFormationEnabled_ ? "enabled" : "disabled"));
    }
}

} // namespace nlm

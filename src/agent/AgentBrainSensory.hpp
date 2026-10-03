#ifndef NLM_AGENT_BRAIN_SENORY_HPP
#define NLM_AGENT_BRAIN_SENORY_HPP

#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/SensoryPercept.hpp"
#include "AgentBrainConfig.hpp"
#include "AgentBrainError.hpp"
#include <memory>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace nlm {

class AgentBrainSensory {
public:
    AgentBrainSensory(std::shared_ptr<AgentBrainConfig> config = nullptr);
    ~AgentBrainSensory() = default;
    
    // Distribute sensory neurons from brain - returns error if brain is null
    AgentBrainResult<void> distributeSensoryNeurons(const Brain& brain);
    
    // Initialize with world - returns error if world or config is null
    AgentBrainResult<void> initialize(const SimpleWorld& world);
    
    // Process sensory input - returns error if percept is null or invalid
    AgentBrainResult<void> processInput(const SensoryPercept& percept);
    
    // Update novelty based on vision input - returns error if inputs invalid
    AgentBrainResult<void> updateNovelty(const std::vector<float>& currentVision, std::vector<float>& previousVision);
    
    // Update curiosity level - returns error if inputs invalid
    AgentBrainResult<void> updateCuriosity(float novelty, float predictionError, float& curiosityLevel);
    
    // Get sensory input size - returns error if config is null
    AgentBrainResult<size_t> getSensoryInputSize() const;
    
    // Reset sensory state - returns error if previousVision pointer is null
    AgentBrainResult<void> reset(std::vector<float>& previousVision);
    
private:
    // Configuration
    std::shared_ptr<AgentBrainConfig> config_;
    
    // Sensory neuron groups (vision, touch, internal, proprioception)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    float noveltyDecay_;
    float noveltyLevel_;
};

} // namespace nlm

#endif // NLM_AGENT_BRAIN_SENORY_HPP
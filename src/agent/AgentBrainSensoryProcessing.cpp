// NLM Agent Brain Sensory Processing Module
// Phase 1: Sensory transduction and world perception

#include "AgentBrain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// AgentBrainSensoryProcessing: Handles sensory input and brain injection
class AgentBrainSensoryProcessing {
public:
    AgentBrainSensoryProcessing(AgentBrain* agentBrain);
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Get last vision data
    const std::vector<float>& getLastVision() const { return lastVision_; }
    
    // Get forward touch value
    float getForwardTouch() const { return forwardTouch_; }
    
    // Get max touch value
    float getMaxTouch() const { return maxTouch_; }
    
    // Get forward object vision
    float getForwardObjectVision() const { return forwardObjectVision_; }
    
private:
    AgentBrain* agentBrain_;
    std::vector<float> lastVision_;
    float forwardTouch_;
    float maxTouch_;
    float forwardObjectVision_;
    std::vector<size_t> lastText_;
};

// Vision processing functions
void processVisionInput(const std::vector<float>& vision, std::vector<Neuron*>& sensoryVision_);

// Touch processing functions
void processTouchInput(const std::vector<float>& touch, float& maxTouch, float& forwardTouch);

// Internal signal processing functions
void processInternalSignals(const std::vector<float>& internal, std::vector<Neuron*>& sensoryInternal_);

// Proprioception processing functions
void processProprioceptionInput(const std::vector<float>& proprioception, std::vector<Neuron*>& sensoryProprioception_);
}
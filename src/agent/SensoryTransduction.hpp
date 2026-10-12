#pragma once

#include "../core/Types/Types.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../agent/AgentBody.hpp"
#include "../core/Config/Config.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {

// Forward declarations
class Brain;

// SensoryTransduction: Handles all sensory input processing
// Converts raw sensory signals from the environment into neural current injections
class SensoryTransduction {
public:
    SensoryTransduction(std::shared_ptr<Config> config);
    ~SensoryTransduction();
    
    // Configure with agent body dimensions
    void initialize(std::shared_ptr<Brain> brain, const AgentBody& body);
    
    // Process sensory percept and inject current into neurons
    void processSensoryInput(const SensoryPercept& percept);
    
    // Get expected sensory input size based on configuration
    size_t getSensoryInputSize() const;
    
    // Get vision dimensions
    size_t getVisionWidth() const { return visionWidth_; }
    size_t getVisionHeight() const { return visionHeight_; }
    
    // Get touch input count
    size_t getTouchCount() const { return touchCount_; }
    
    // Get internal signal count
    size_t getInternalSignalCount() const { return internalSignalCount_; }
    
    // Get proprioception count
    size_t getProprioceptionCount() const { return proprioceptionCount_; }
    
    // Get previous vision for novelty detection
    const std::vector<float>& getPreviousVision() const { return previousVision_; }
    
    // Update previous vision state
    void setPreviousVision(const std::vector<float>& vision) { previousVision_ = vision; }
    
    // Get novelty level
    float getNoveltyLevel() const { return noveltyLevel_; }
    
private:
    // Configuration
    std::shared_ptr<Config> config_;
    
    // Brain reference
    std::shared_ptr<Brain> brain_;
    
    // Sensory neuron groups (owned by brain, just references)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Sensory dimensions from config
    size_t visionWidth_;
    size_t visionHeight_;
    size_t touchCount_;
    size_t internalSignalCount_;
    size_t proprioceptionCount_;
    
    // Novelty detection
    float noveltyLevel_;
    float sensoryNoveltyDecay_;
    std::vector<float> previousVision_;
    
    // Inject current into a vector of neurons
    void injectCurrentToNeurons(const std::vector<Neuron*>& neurons, const std::vector<float>& signals, float scaleFactor);
};

} // namespace nlm

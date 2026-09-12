#pragma once

#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../performance/NoveltyOptimizer.hpp"
#include "../motor/Action.hpp"
#include "CurrentInjector.hpp"
#include <memory>
#include <vector>

namespace nlm {

class SensoryProcessor {
public:
    SensoryProcessor(std::shared_ptr<Brain> brain);
    ~SensoryProcessor() = default;
    
    // Process sensory percept and inject into brain
    void process(const SensoryPercept& percept);
    
    // Update novelty calculation (O(n) optimization)
    void updateNovelty(const std::vector<float>& vision);
    
    // Get novelty level
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Update curiosity based on novelty
    void updateCuriosity(float novelty, float predictionError, bool enabled);
    
    // Get curiosity level
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    // Reset processor state
    void reset();
    
    // Configuration
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Get previous vision for backward compatibility
    const std::vector<float>& getPreviousVision() const { return previousVision_; }
    
    // Set up neurons (for initialization)
    void setMotorNeurons(const std::vector<Neuron*>& forward,
                        const std::vector<Neuron*>& backward,
                        const std::vector<Neuron*>& left,
                        const std::vector<Neuron*>& right,
                        const std::vector<Neuron*>& interact,
                        const std::vector<Neuron*>& wait);
    
    void setSensoryNeurons(const std::vector<Neuron*>& vision,
                          const std::vector<Neuron*>& touch,
                          const std::vector<Neuron*>& internal,
                          const std::vector<Neuron*>& proprioception);
    
    // Get sensory input size
    size_t getSensoryInputSize() const;
    
    // Get motor output size
    size_t getMotorOutputSize() const;
    
    // Update curiosity level (for backward compatibility)
    void updateCuriosityLevel(float level) { curiosityLevel_ = level; }
    
    // Helper function to inject current into neurons
    void injectCurrentToNeurons(const std::vector<Neuron*>& neurons, 
                               const std::vector<float>& values,
                               float scaleFactor);
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups (for backward compatibility)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // State
    float noveltyLevel_;
    float curiosityLevel_;
    bool curiosityEnabled_;
    
    // Previous sensory state
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
#pragma once

#include "../brain/Neuron.hpp"
#include <vector>
#include <cmath>

namespace nlm {

class SensoryProcessor {
public:
    SensoryProcessor();
    ~SensoryProcessor();
    
    // Process sensory input for different modalities
    void processVisionInput(
        const std::vector<Neuron*>& sensoryVision,
        const std::vector<float>& vision,
        float scaleFactor = 5.0f
    );
    
    void processTouchInput(
        const std::vector<Neuron*>& sensoryTouch,
        const std::vector<float>& touch,
        float scaleFactor = 8.0f
    );
    
    void processInternalInput(
        const std::vector<Neuron*>& sensoryInternal,
        const std::vector<float>& internal,
        float scaleFactor = 5.0f
    );
    
    void processProprioceptionInput(
        const std::vector<Neuron*>& sensoryProprioception,
        const std::vector<float>& proprio,
        float scaleFactor = 3.0f
    );
    
    // Compute novelty (difference from previous vision)
    float computeNovelty(const std::vector<float>& vision,
                       const std::vector<float>& previousVision);
    
    // Update curiosity level based on novelty and prediction error
    float updateCuriosity(float novelty, float predictionError,
                         float decayFactor = 2.0f, float errorWeight = 0.5f);
    
    // Get current novelty level
    float getNoveltyLevel() const { return noveltyLevel_; }
    
    // Reset novelty state
    void reset() { noveltyLevel_ = 0.0f; }

private:
    float noveltyLevel_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
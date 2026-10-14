#pragma once

#include "AgentBrainConfig.hpp"

namespace nlm {

class CuriositySystem {
public:
    CuriositySystem(float noveltyLevel,
                   float predictionError,
                   float& curiosityLevel,
                   bool enabled)
        : noveltyLevel_(noveltyLevel)
        , predictionError_(predictionError)
        , curiosityLevel_(curiosityLevel)
        , enabled_(enabled) {
        validateSetup();
    }
    
    // Update curiosity level based on novelty and prediction error
    void updateCuriosity(float novelty, float predictionError, bool enabled) {
        if (!enabled) {
            curiosityLevel_ = 0.0f;
            return;
        }
        
        curiosityLevel_ = novelty * AgentBrainConfig::CURIOSITY_NOVELTY_WEIGHT +
                         std::abs(predictionError) * AgentBrainConfig::CURIOSITY_PREDICTION_ERROR_WEIGHT;
        
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, AgentBrainConfig::CURIOSITY_MAX);
    }
    
    // Get current curiosity level
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    // Enable or disable curiosity-driven behavior
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    bool validateSetup() {
        return true;
    }
    
    // Reset method for testing and state clearing
    void reset() {
        curiosityLevel_ = 0.0f;
    }
    
private:
    float& noveltyLevel_;
    float& predictionError_;
    float& curiosityLevel_;
    bool enabled_;
};

} // namespace nlm

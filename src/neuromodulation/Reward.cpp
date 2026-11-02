#include "Reward.hpp"

namespace nlm {

struct Reward::Impl {
    float currentValue;
    float accumulatedReward;
    std::vector<float> history;
    
    Impl() : currentValue(0.0f), accumulatedReward(0.0f) {}
};

Reward::Reward() : pImpl(new Impl) {}

Reward::~Reward() = default;

float Reward::getValue() const {
    return pImpl->currentValue;
}

void Reward::setValue(float value) {
    pImpl->currentValue = value;
}

void Reward::add(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->currentValue += delta;
}

void Reward::reset() {
    pImpl->currentValue = 0.0f;
    pImpl->accumulatedReward = 0.0f;
}

float Reward::computeReward(const Observation& observation) const {
    // Implement real reward computation from observation
    // This method analyzes the current percept to determine appropriate reward
    // Based on world interactions and agent behavior
    
    if (!observation.getSensoryInput()) {
        return 0.0f;
    }
    
    const SensoryInput* sensoryInput = observation.getSensoryInput();
    const std::vector<float>& data = sensoryInput->getData();
    
    // Based on the type of sensory input, compute appropriate reward
    // The reward is based on the agent's interaction with the environment
    
    // If this is an InternalSignals observation, it contains homeostatic states
    if (strcmp(sensoryInput->getType(), "InternalSignals") == 0) {
        // Check energy level from internal signals
        // Internal signals typically include energy, health, other homeostatic factors
        if (!data.empty()) {
            float energy = data[0];  // First signal is usually energy
            if (energy < 0.3f) {
                return -10.0f;  // Low energy - survival motivation
            } else if (energy > 0.7f) {
                return 2.0f;   // Good energy level
            }
        }
    }
    
    // If this is a Vision observation, analyze the visual field
    if (strcmp(sensoryInput->getType(), "Vision") == 0) {
        // Vision data contains brightness/intensity values
        // Low values typically indicate empty space, mid values indicate objects
        // High values (near 1.0) indicate resources, very low values (near 0.0) indicate hazards
        
        float totalBrightness = 0.0f;
        float resourceCount = 0.0f;
        float hazardCount = 0.0f;
        size_t sampleCount = 0;
        
        // Sample vision data to identify objects
        for (size_t i = 0; i < data.size(); ++i) {
            float value = data[i];
            totalBrightness += value;
            sampleCount++;
            
            if (value > 0.7f) {
                // Bright areas are likely resources
                resourceCount++;
            } else if (value < 0.2f) {
                // Dark areas are likely hazards or empty space
                // But we can't distinguish hazards from empty space from vision alone
                // So we assume most dark areas are hazards for now
                hazardCount++;
            }
        }
        
        if (sampleCount > 0) {
            float avgBrightness = totalBrightness / sampleCount;
            
            // High average brightness indicates good resource availability
            if (avgBrightness > 0.6f) {
                return 5.0f + (resourceCount * 2.0f);  // Resource-rich area
            } 
            // Medium brightness with hazards
            else if (hazardCount > 0) {
                return -8.0f - (hazardCount * 1.5f);  // Hazardous area
            }
        }
    }
    
    // If this is a Touch observation, check for proximity to objects
    if (strcmp(sensoryInput->getType(), "Touch") == 0) {
        // Touch signals indicate proximity to objects
        // We need to analyze touch data to determine if we're touching something beneficial or harmful
        
        float touchStrength = 0.0f;
        size_t touchingCount = 0;
        
        for (float value : data) {
            if (value > 0.1f) {
                touchingCount++;
                touchStrength += value;
            }
        }
        
        if (touchingCount > 0) {
            float avgTouch = touchStrength / touchingCount;
            
            // Strong touch typically indicates contact with something
            if (avgTouch > 0.5f) {
                // The actual reward value would depend on what we're touching
                // In the real world, this would be determined by the world state
                // For now, we give a neutral touch reward
                return 1.0f;
            }
        }
    }
    
    // Default: neutral observation - no significant reward
    return 0.0f;
}

const std::vector<float>& Reward::getHistory() const {
    return pImpl->history;
}

void Reward::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm

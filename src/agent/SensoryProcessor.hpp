#pragma once

#include "SensoryPercept.hpp"
#include "SensoryNeuronGroup.hpp"
#include "AgentBrainConfig.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

class SensoryProcessor {
public:
    SensoryProcessor(SensoryNeuronGroup& visionGroup,
                   SensoryNeuronGroup& touchGroup,
                   SensoryNeuronGroup& internalGroup,
                   SensoryNeuronGroup& proprioceptiveGroup)
        : visionGroup_(visionGroup)
        , touchGroup_(touchGroup)
        , internalGroup_(internalGroup)
        , proprioceptiveGroup_(proprioceptiveGroup) {
        validateSetup();
    }
    
    // Process vision input
    void processVision(const std::vector<float>& visionSignal) {
        validateInput(visionSignal, "Vision");
        
        if (!visionGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Vision neurons");
        }
        
        // Process each vision neuron based on intensity
        for (size_t i = 0; i < visionGroup_.size() && i < visionSignal.size(); ++i) {
            float current = visionSignal[i] * AgentBrainConfig::VISION_SCALE;
            visionGroup_[i]->injectCurrent(current);
        }
    }
    
    // Process touch input
    void processTouch(const std::vector<float>& touchSignal) {
        validateInput(touchSignal, "Touch");
        
        if (!touchGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Touch neurons");
        }
        
        for (size_t i = 0; i < touchGroup_.size() && i < touchSignal.size(); ++i) {
            float current = touchSignal[i] * AgentBrainConfig::TOUCH_SCALE;
            touchGroup_[i]->injectCurrent(current);
        }
    }
    
    // Process internal state signals
    void processInternal(const std::vector<float>& internalSignal) {
        validateInput(internalSignal, "Internal");
        
        if (!internalGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Internal neurons");
        }
        
        for (size_t i = 0; i < internalGroup_.size() && i < internalSignal.size(); ++i) {
            float current = (internalSignal[i] * 2.0f - 1.0f) * AgentBrainConfig::INTERNAL_SCALE;
            internalGroup_[i]->injectCurrent(current);
        }
    }
    
    // Process proprioception signals
    void processProprioception(const std::vector<float>& proprioceptionSignal) {
        validateInput(proprioceptionSignal, "Proprioception");
        
        if (!proprioceptiveGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Proprioceptive neurons");
        }
        
        for (size_t i = 0; i < proprioceptiveGroup_.size() && i < proprioceptionSignal.size(); ++i) {
            float current = (proprioceptionSignal[i] * 2.0f - 1.0f) * AgentBrainConfig::PROPRIOCEPTIVE_SCALE;
            proprioceptiveGroup_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty level based on vision signal changes
    float computeNovelty(const std::vector<float>& currentVision,
                       std::vector<float>& previousVision,
                       float& noveltyLevel) {
        validateInput(currentVision, "Vision for novelty computation");
        
        if (currentVision.empty()) {
            noveltyLevel = 0.0f;
            return noveltyLevel;
        }
        
        // Calculate total difference from previous vision
        float totalDiff = 0.0f;
        for (size_t i = 0; i < currentVision.size() && i < previousVision.size(); ++i) {
            float diff = std::abs(currentVision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        // Normalize by vision size
        noveltyLevel = totalDiff / static_cast<float>(std::max<size_t>(currentVision.size(), 1));
        
        // Apply exponential decay
        noveltyLevel *= AgentBrainConfig::SENSORY_NOVELTY_DECAY;
        
        // Update previous vision for next comparison
        previousVision = currentVision;
        
        return noveltyLevel;
    }
    
    // Get processed sensory activity levels
    float getVisionActivity() const { return visionGroup_.calculateActivity(); }
    float getTouchActivity() const { return touchGroup_.calculateActivity(); }
    float getInternalActivity() const { return internalGroup_.calculateActivity(); }
    float getProprioceptiveActivity() const { return proprioceptiveGroup_.calculateActivity(); }
    
    // Update curiosity based on novelty and prediction error
    void updateCuriosity(float novelty, float predictionError, float& curiosityLevel, bool enabled) {
        if (!enabled) {
            curiosityLevel = 0.0f;
            return;
        }
        
        curiosityLevel = novelty * AgentBrainConfig::CURIOSITY_NOVELTY_WEIGHT +
                        std::abs(predictionError) * AgentBrainConfig::CURIOSITY_PREDICTION_ERROR_WEIGHT;
        
        curiosityLevel = std::clamp(curiosityLevel, 0.0f, AgentBrainConfig::CURIOSITY_MAX);
    }
    
    // Validate that all sensory groups are properly configured
    bool validateSetup() {
        bool isValid = true;
        
        if (!visionGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Vision neuron group");
        }
        isValid = false;
        
        if (!touchGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Touch neuron group");
        }
        isValid = false;
        
        if (!internalGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Internal neuron group");
        }
        isValid = false;
        
        if (!proprioceptiveGroup_.isValid()) {
            throw AgentBrainException::nullPointerError("Proprioceptive neuron group");
        }
        isValid = false;
        
        return isValid;
    }
    
    // Reset method for testing
    void reset() {
        // Clear any accumulated state
    }
    
private:
    void validateInput(const std::vector<float>& input, const std::string& inputName) {
        if (input.empty()) {
            throw AgentBrainException::invalidValueError(
                "Input validation for " + inputName, 0.0f, AgentBrainConfig::VALIDATION_EPSILON, 1.0f);
        }
    }
    
    SensoryNeuronGroup& visionGroup_;
    SensoryNeuronGroup& touchGroup_;
    SensoryNeuronGroup& internalGroup_;
    SensoryNeuronGroup& proprioceptiveGroup_;
    
    // Reset method for testing and state clearing
    void reset() {
        // Clear any accumulated state
    }
};

} // namespace nlm


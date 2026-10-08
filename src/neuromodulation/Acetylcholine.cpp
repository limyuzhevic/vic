#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float currentLevel;
    float attentionStrength;
    float memoryConsolidationRate;
    float learningReadiness;
    float corticalActivation;
    float plasticityFactor;
    
    // Attention dynamics
    std::vector<float> attentionHistory;
    std::vector<float> memoryConsolidationHistory;
    
    // Learning state
    float learningEfficiency;
    float neuralConnectivity;
    
    // Time constants
    float attentionDecayRate;
    float consolidationRate;
    float activationRecovery;
    
    // Temporary states
    float currentFocus;
    float consolidationTarget;
    
    // Temporal dynamics
    TimestepDuration timeSinceLastAttentionChange;
    TimestepDuration timeSinceLastLearningEvent;
    
    Impl() 
        : currentLevel(0.6f)
        , attentionStrength(0.7f)
        , memoryConsolidationRate(0.8f)
        , learningReadiness(0.5f)
        , corticalActivation(0.6f)
        , plasticityFactor(1.0f)
        , learningEfficiency(0.8f)
        , neuralConnectivity(0.9f)
        , attentionDecayRate(0.05f)
        , consolidationRate(0.1f)
        , activationRecovery(0.02f)
        , currentFocus(0.5f)
        , consolidationTarget(0.7f)
        , timeSinceLastAttentionChange(0.0f)
        , timeSinceLastLearningEvent(0.0f)
    {
    }
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {
}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine strongly enhances plasticity
    // Higher attention and memory consolidation increase plasticity
    
    // Attention-dependent modulation
    float attentionModulation = 1.0f + 0.5f * pImpl->attentionStrength;
    
    // Memory consolidation enhances plasticity
    float consolidationModulation = 1.0f + 0.8f * pImpl->memoryConsolidationRate;
    
    // Learning readiness improves efficiency
    float learningModulation = 1.0f + 0.6f * pImpl->learningReadiness;
    
    // Cortical activation increases neural processing
    float activationModulation = 1.0f + 0.4f * pImpl->corticalActivation;
    
    // Combined effect
    float factor = 1.5f * attentionModulation * consolidationModulation * learningModulation * activationModulation;
    
    // Clamp to reasonable range
    return std::clamp(factor, 0.5f, 4.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->timeSinceLastAttentionChange += dt;
    pImpl->timeSinceLastLearningEvent += dt;
    
    // Attention naturally decays over time
    pImpl->attentionStrength = std::max(0.1f, pImpl->attentionStrength - pImpl->attentionDecayRate * dt);
    
    // Memory consolidation recovers
    pImpl->memoryConsolidationRate = std::min(1.0f, pImpl->memoryConsolidationRate + pImpl->consolidationRate * dt);
    
    // Learning readiness and cortical activation recover
    pImpl->learningReadiness = std::min(1.0f, pImpl->learningReadiness + 0.03f * dt);
    pImpl->corticalActivation = std::min(1.0f, pImpl->corticalActivation + pImpl->activationRecovery * dt);
    
    // Add to history
    pImpl->attentionHistory.push_back(pImpl->attentionStrength);
    pImpl->memoryConsolidationHistory.push_back(pImpl->memoryConsolidationRate);
    
    if (pImpl->attentionHistory.size() > 1000) {
        pImpl->attentionHistory.erase(pImpl->attentionHistory.begin());
        pImpl->memoryConsolidationHistory.erase(pImpl->memoryConsolidationHistory.begin());
    }
}

void Acetylcholine::signalAttentionFocus(float focusStrength) {
    // Apply attention focus modulation
    pImpl->attentionStrength = std::clamp(pImpl->attentionStrength + focusStrength * 0.5f, 0.1f, 1.0f);
    
    // Increases learning readiness
    pImpl->learningReadiness = std::min(1.0f, pImpl->learningReadiness + focusStrength * 0.3f);
    
    // Boosts cortical activation
    pImpl->corticalActivation = std::min(1.0f, pImpl->corticalActivation + focusStrength * 0.4f);
    
    // Improves efficiency
    pImpl->learningEfficiency = std::min(1.0f, pImpl->learningEfficiency + focusStrength * 0.2f);
    
    // Enhances neural connectivity
    pImpl->neuralConnectivity = std::min(1.0f, pImpl->neuralConnectivity + focusStrength * 0.15f);
    
    pImpl->currentFocus = focusStrength;
    pImpl->timeSinceLastAttentionChange = 0.0f;
    
    NLM_LOG_INFO(std::string("Acetylcholine: Attention focus = ") + std::to_string(focusStrength));
}

void Acetylcholine::signalMemoryConsolidation(float memoryPriority) {
    // Memory consolidation priority affects consolidation rate
    if (memoryPriority > 0.0f) {
        pImpl->memoryConsolidationRate = std::min(1.0f, pImpl->memoryConsolidationRate + memoryPriority * 0.4f);
        
        // Enhances plasticity for memory encoding
        pImpl->plasticityFactor = std::min(4.0f, pImpl->plasticityFactor + memoryPriority * 0.3f);
        
        // Boosts learning readiness
        pImpl->learningReadiness = std::min(1.0f, pImpl->learningReadiness + memoryPriority * 0.2f);
        
        pImpl->consolidationTarget = memoryPriority;
        pImpl->timeSinceLastLearningEvent = 0.0f;
        
        NLM_LOG_INFO(std::string("Acetylcholine: Memory consolidation priority = ") + std::to_string(memoryPriority));
    }
}

void Acetylcholine::signalLearningReadiness(float learningReadiness) {
    // Set learning readiness level
    pImpl->learningReadiness = std::clamp(learningReadiness, 0.0f, 1.0f);
    
    // Learning readiness affects attention strength
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + learningReadiness * 0.2f);
    
    // Influences cortical activation
    pImpl->corticalActivation = std::min(1.0f, pImpl->corticalActivation + learningReadiness * 0.3f);
    
    // Improves efficiency based on readiness
    pImpl->learningEfficiency = std::min(1.0f, pImpl->learningEfficiency + learningReadiness * 0.25f);
    
    NLM_LOG_INFO(std::string("Acetylcholine: Learning readiness = ") + std::to_string(learningReadiness));
}

void Acetylcholine::signalCorticalActivation(float activationLevel) {
    // Set cortical activation level
    pImpl->corticalActivation = std::clamp(activationLevel, 0.0f, 1.0f);
    
    // Cortical activation affects attention
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + activationLevel * 0.2f);
    
    // Influences learning readiness
    pImpl->learningReadiness = std::min(1.0f, pImpl->learningReadiness + activationLevel * 0.15f);
    
    // Enhances plasticity
    pImpl->plasticityFactor = std::min(4.0f, pImpl->plasticityFactor + activationLevel * 0.2f);
    
    NLM_LOG_INFO(std::string("Acetylcholine: Cortical activation = ") + std::to_string(activationLevel));
}

} // namespace nlm

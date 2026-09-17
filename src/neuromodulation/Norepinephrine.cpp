#include "Norepinephrine.hpp"
#include "../../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    // NE dynamics parameters
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal and vigilance parameters
    float arousalLevel;
    float vigilanceScore;
    float stressLevel;
    
    // Alertness parameters
    float alertness;
    float reactiveness;
    float inhibition;
    
    // State management
    float phasicArousal;
    float tonicVigilance;
    std::vector<float> alertHistory;
    std::vector<float> stressHistory;
    
    // Learning-related parameters
    float explorationBias;
    float riskAssessment;
    float noveltySensitivity;
    
    Impl() : level(0.02f), baseline(0.02f), peak(0.3f), decayRate(0.03f), 
             releaseRate(1.0f), arousalLevel(0.0f), vigilanceScore(0.0f),
             stressLevel(0.0f), alertness(0.0f), reactiveness(0.0f), inhibition(0.0f),
             phasicArousal(0.0f), tonicVigilance(0.0f), explorationBias(0.5f),
             riskAssessment(0.5f), noveltySensitivity(0.7f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity in complex ways depending on level
    float factor = 1.0f;
    
    if (pImpl->level < 0.1f) {
        // Low NE: reduced alertness, slower learning
        factor = 0.5f + pImpl->level * 2.0f;
    } else if (pImpl->level < 0.3f) {
        // Moderate NE: optimal alertness and learning
        factor = 0.8f + pImpl->level * 2.5f;
    } else if (pImpl->level < 0.6f) {
        // High NE: increased arousal, enhanced attention but potential interference
        factor = 1.0f + pImpl->level * 1.5f;
    } else {
        // Very high NE: stress overload, reduced learning efficiency
        factor = 0.7f + pImpl->level * 0.5f;
    }
    
    // Apply vigilance modulation
    factor *= (1.0f + pImpl->vigilanceScore * 0.4f);
    
    // Apply arousal modulation
    factor *= (1.0f + pImpl->arousalLevel * 0.3f);
    
    return factor;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update arousal and vigilance
    pImpl->arousalLevel = pImpl->arousalLevel * 0.9f + pImpl->level * 0.2f;
    pImpl->tonicVigilance = pImpl->tonicVigilance * 0.95f + pImpl->level * 0.05f;
    
    // Update alertness based on NE level
    pImpl->alertness = pImpl->alertness * 0.9f + pImpl->level * 0.15f;
    pImpl->reactiveness = pImpl->reactiveness * 0.9f + pImpl->level * 0.1f;
    
    // Inhibition decreases with NE level
    pImpl->inhibition = 0.9f + pImpl->level * 0.2f;
    
    // Update stress level
    if (pImpl->level > 0.4f) {
        // Stress accumulation at high NE levels
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + pImpl->level * 0.01f);
    } else {
        // Stress recovery at moderate NE levels
        pImpl->stressLevel *= 0.98f;
    }
    
    // Store history
    pImpl->alertHistory.push_back(pImpl->alertness);
    if (pImpl->alertHistory.size() > 100) {
        pImpl->alertHistory.erase(pImpl->alertHistory.begin());
    }
    
    pImpl->stressHistory.push_back(pImpl->stressLevel);
    if (pImpl->stressHistory.size() > 100) {
        pImpl->stressHistory.erase(pImpl->stressHistory.begin());
    }
}

void Norepinephrine::signalArousal(float intensity, float novelty) {
    // Phasic NE response to arousing stimuli
    if (intensity > 0.0f) {
        // Arousal response
        float arousalResponse = intensity * 2.0f;
        pImpl->level = std::max(pImpl->level, pImpl->baseline + arousalResponse);
        
        // Update phasic arousal
        pImpl->phasicArousal = pImpl->phasicArousal * 0.8f + intensity * 0.2f;
        
        // Novelty enhances NE response
        if (novelty > 0.0f) {
            float noveltyBoost = novelty * pImpl->noveltySensitivity;
            pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyBoost);
            
            // Novelty increases exploration bias
            pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + novelty * 0.1f);
        }
        
        // Update risk assessment
        pImpl->riskAssessment = std::min(1.0f, pImpl->riskAssessment + intensity * 0.05f);
    }
}

void Norepinephrine::signalStress(float stressor, float duration) {
    // Stress response from environmental challenges
    if (stressor > 0.0f) {
        // Acute stress response
        float stressResponse = stressor * duration * 0.5f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse);
        
        // Stress increases arousal and vigilance
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressResponse);
        pImpl->vigilanceScore = std::min(1.0f, pImpl->vigilanceScore + stressor * 0.3f);
        
        // High stress impairs learning
        pImpl->inhibition = std::min(2.0f, pImpl->inhibition + stressResponse);
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceScore() const {
    return pImpl->vigilanceScore;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getAlertness() const {
    return pImpl->alertness;
}

float Norepinephrine::getExplorationBias() const {
    return pImpl->explorationBias;
}

} // namespace nlm
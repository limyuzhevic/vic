#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;                    // Current 5-HT concentration (0-1)
    float baseline;                  // Baseline level (0.15)
    float peak;                      // Peak level (1.0)
    float decayRate;                 // Natural decay rate (0.06)
    float socialSensitivity;          // Sensitivity to social feedback (0.7)
    float patienceLevel;             // Decision patience factor (1.2)
    float aversionWeight;            // Avoidance behavior weight (0.8)
    float approachWeight;            // Approach behavior weight (1.0)
    
    Impl() 
        : level(0.0f)
        , baseline(0.15f)
        , peak(1.0f)
        , decayRate(0.06f)
        , socialSensitivity(0.7f)
        , patienceLevel(1.2f)
        , aversionWeight(0.8f)
        , approachWeight(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates decision-making and behavioral flexibility
    // Higher levels promote patience and reduce impulsivity
    float baseFactor = 1.0f;  // Normal plasticity
    
    // Calculate approach/avoidance balance
    float approach = pImpl->approachWeight * (1.0f - pImpl->level);
    float avoid = pImpl->aversionWeight * pImpl->level;
    float balance = approach + avoid;
    
    return baseFactor * balance;
}

void Serotonin::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::respondToSocialFeedback(float socialValue, float predictionError) {
    if (socialValue == 0.0f) return;
    
    // Positive social feedback increases serotonin
    // Negative social feedback decreases serotonin
    float response = socialValue * (1.0f + predictionError);
    
    if (response > 0.0f) {
        // Increase serotonin for positive feedback
        float increase = std::min(0.3f, response * pImpl->socialSensitivity);
        pImpl->level = std::min(pImpl->peak, pImpl->level + increase);
        
        NLM_LOG_INFO("5-HT social response: feedback=" + std::to_string(socialValue) + 
                    " prediction_error=" + std::to_string(predictionError) + 
                    " level=" + std::to_string(pImpl->level));
    } else {
        // Decrease serotonin for negative feedback
        float decrease = std::min(0.2f, -response * pImpl->socialSensitivity);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease);
    }
}

float Serotonin::modulateDecisionThreshold(float baselineThreshold, float urgency) const {
    // 5-HT increases decision thresholds (promotes patience)
    // Reduces impulsivity in favor of careful deliberation
    float patienceFactor = pImpl->patienceLevel * (1.0f + pImpl->level * 0.5f);
    return baselineThreshold * patienceFactor;
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm

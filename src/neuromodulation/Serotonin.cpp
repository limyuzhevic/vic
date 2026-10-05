#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float plasticityModifier;
    float neuromodulatoryIndex;
    
    // Receptor states
    float fiveHThreeReceptorActivation;
    float fiveHFourReceptorActivation;
    
    // Time integration
    float timeSinceLastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , adaptationRate(0.05f), plasticityModifier(1.0f), neuromodulatoryIndex(0.0f)
        , fiveHThreeReceptorActivation(0.0f), fiveHFourReceptorActivation(0.0f)
        , timeSinceLastUpdate(0.0f) {}
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
    // Serotonin modulates plasticity differently than dopamine:
    // 5-HT1A activation enhances LTP, 5-HT2A enhances LTD
    // This helps regulate mood and learning
    
    float h1aEffect = pImpl->fiveHThreeReceptorActivation * 0.6f;  // 5-HT1A promotes LTP
    float h2aEffect = pImpl->fiveHFourReceptorActivation * 0.4f;  // 5-HT2A promotes LTD
    
    // Net effect: 5-HT1A promotes learning, 5-HT2A regulates impulsivity
    float netEffect = h1aEffect - h2aEffect;
    float plasticityFactor = 0.5f + netEffect * 0.3f;  // Range: 0.2 to 0.8
    
    return std::clamp(plasticityFactor, 0.2f, 0.8f);
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->timeSinceLastUpdate = dt;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Receptor dynamics
    float decayRate = pImpl->decayRate * 0.5f;
    pImpl->fiveHThreeReceptorActivation = std::max(0.0f, pImpl->fiveHThreeReceptorActivation - decayRate * static_cast<float>(dt));
    pImpl->fiveHFourReceptorActivation = std::max(0.0f, pImpl->fiveHFourReceptorActivation - decayRate * static_cast<float>(dt));
    
    // Neuromodulatory index tracks recent serotonin activity
    pImpl->neurotransmitterIndex *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSatiety(float satietyLevel) {
    // Serotonin responds to satiety and well-being
    // Higher satiety increases serotonin level
    float signal = satietyLevel * pImpl->releaseRate;
    
    // Increase level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + signal);
    
    // Activate 5-HT1A receptors (well-being signal)
    pImpl->fiveHThreeReceptorActivation = std::min(1.0f, pImpl->fiveHThreeReceptorActivation + signal * 0.6f);
    
    // Can also affect 5-HT2A receptors depending on context
    if (satietyLevel > 0.5f) {
        pImpl->fiveHFourReceptorActivation = std::min(1.0f, pImpl->fiveHFourReceptorActivation + (satietyLevel - 0.5f) * 0.3f);
    }
}

void Serotonin::signalSocialStatus(float statusLevel) {
    // Serotonin responds to social status
    // Higher social status increases serotonin
    float signal = statusLevel * pImpl->releaseRate * 0.8f;  // Stronger effect for social signals
    
    // Increase level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + signal);
    
    // Activate 5-HT2A receptors (social status signal)
    pImpl->fiveHFourReceptorActivation = std::min(1.0f, pImpl->fiveHFourReceptorActivation + signal * 0.5f);
}

float Serotonin::get5HT1AEffect() const {
    return pImpl->fiveHThreeReceptorActivation;
}

float Serotonin::get5HT2AEffect() const {
    return pImpl->fiveHFourReceptorActivation;
}

std::string Serotonin::getStateDescription() const {
    std::stringstream ss;
    ss << "Serotonin level: " << pImpl->level << std::endl;
    ss << "5-HT1A receptor activation: " << pImpl->fiveHThreeReceptorActivation << std::endl;
    ss << "5-HT2A receptor activation: " << pImpl->fiveHFourReceptorActivation << std::endl;
    ss << "Plasticity factor: " << getPlasticityFactor() << std::endl;
    return ss.str();
}

} // namespace nlm
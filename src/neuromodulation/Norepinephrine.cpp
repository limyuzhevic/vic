#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float plasticityModifier;
    float neuromodulatoryIndex;
    
    // Receptor states
    float alphaReceptorActivation;
    float betaReceptorActivation;
    
    // Time integration
    float timeSinceLastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , adaptationRate(0.05f), plasticityModifier(1.0f), neuromodulatoryIndex(0.0f)
        , alphaReceptorActivation(0.0f), betaReceptorActivation(0.0f)
        , timeSinceLastUpdate(0.0f) {}
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
    // Norepinephrine modulates plasticity:
    // Alpha receptor enhances LTP, beta receptor enhances LTD
    // This regulates arousal and vigilance effects on learning
    
    float alphaEffect = pImpl->alphaReceptorActivation * 0.6f;  // Alpha enhances LTP
    float betaEffect = pImpl->betaReceptorActivation * 0.4f;   // Beta enhances LTD
    
    // Net effect on plasticity
    float netEffect = alphaEffect - betaEffect;
    float plasticityFactor = 0.5f + netEffect * 0.4f;  // Range: 0.1 to 0.9
    
    return std::clamp(plasticityFactor, 0.1f, 0.9f);
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->timeSinceLastUpdate = dt;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Receptor dynamics
    float decayRate = pImpl->decayRate * 0.5f;
    pImpl->alphaReceptorActivation = std::max(0.0f, pImpl->alphaReceptorActivation - decayRate * static_cast<float>(dt));
    pImpl->betaReceptorActivation = std::max(0.0f, pImpl->betaReceptorActivation - decayRate * static_cast<float>(dt));
    
    // Neuromodulatory index tracks recent norepinephrine activity
    pImpl->neurotransmitterIndex *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // Norepinephrine responds to arousal and stress
    // Higher arousal increases NE level
    float signal = arousalLevel * pImpl->releaseRate * 1.2f;  // Stronger effect for arousal
    
    // Increase level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + signal);
    
    // Activate alpha receptors (arousal enhancement)
    pImpl->alphaReceptorActivation = std::min(1.0f, pImpl->alphaReceptorActivation + signal * 0.7f);
}

void Norepinephrine::increaseVigilance(float intensity) {
    // Norepinephrine increases vigilance and attention
    // Higher intensity increases NE level and receptor activation
    float signal = intensity * pImpl->releaseRate * 1.5f;
    
    // Increase level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + signal);
    
    // Activate beta receptors (vigilance enhancement)
    pImpl->betaReceptorActivation = std::min(1.0f, pImpl->betaReceptorActivation + signal * 0.5f);
}

std::string Norepinephrine::getStateDescription() const {
    std::stringstream ss;
    ss << "Norepinephrine level: " << pImpl->level << std::endl;
    ss << "Alpha receptor activation: " << pImpl->alphaReceptorActivation << std::endl;
    ss << "Beta receptor activation: " << pImpl->betaReceptorActivation << std::endl;
    ss << "Plasticity factor: " << getPlasticityFactor() << std::endl;
    return ss.str();
}

} // namespace nlm
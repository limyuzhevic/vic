#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float plasticityModifier;
    float neuromodulatoryIndex;
    
    // Receptor states
    float mamReceptorActivation;
    float nicotinicReceptorActivation;
    
    // Time integration
    float timeSinceLastUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , adaptationRate(0.05f), plasticityModifier(1.0f), neuromodulatoryIndex(0.0f)
        , mamReceptorActivation(0.0f), nicotinicReceptorActivation(0.0f)
        , timeSinceLastUpdate(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates plasticity differently:
    // M1 receptor enhances LTP, nicotinic receptors enhance overall excitability
    float m1Effect = pImpl->mamReceptorActivation * 0.6f;  // M1 enhances LTP
    float nicotinicEffect = pImpl->nicotinicReceptorActivation * 0.8f;  // Nicotinic increases excitability
    
    // Net effect on plasticity
    float netEffect = m1Effect + nicotinicEffect * 0.5f;
    float plasticityFactor = 0.5f + netEffect * 0.5f;  // Range: 0.0 to 1.0
    
    return std::clamp(plasticityFactor, 0.2f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->timeSinceLastUpdate = dt;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Receptor dynamics
    float decayRate = pImpl->decayRate * 0.5f;
    pImpl->mamReceptorActivation = std::max(0.0f, pImpl->mamReceptorActivation - decayRate * static_cast<float>(dt));
    pImpl->nicotinicReceptorActivation = std::max(0.0f, pImpl->nicotinicReceptorActivation - decayRate * static_cast<float>(dt));
    
    // Neuromodulatory index tracks recent acetylcholine activity
    pImpl->neurotransmitterIndex *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionLevel) {
    // Acetylcholine responds to attention demands
    // Higher attention increases ACh level
    float signal = attentionLevel * pImpl->releaseRate;
    
    // Increase level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + signal);
    
    // Activate M1 receptors (attention enhancement)
    pImpl->mamReceptorActivation = std::min(1.0f, pImpl->mamReceptorActivation + signal * 0.7f);
    
    // Activate nicotinic receptors (excitability enhancement)
    pImpl->nicotinicReceptorActivation = std::min(1.0f, pImpl->nicotinicReceptorActivation + signal * 0.5f);
}

void Acetylcholine::enhanceMemoryConsolidation(float strength) {
    // Acetylcholine enhances memory consolidation
    // Higher strength increases receptor activation
    float signal = strength * pImpl->releaseRate * 1.5f;
    
    // Increase level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + signal);
    
    // Activate M1 receptors (memory consolidation)
    pImpl->mamReceptorActivation = std::min(1.0f, pImpl->mamReceptorActivation + signal * 0.8f);
    
    // Store in neuromodulatory index for later use
    pImpl->neurotransmitterIndex = std::min(1.0f, pImpl->neurotransmitterIndex + signal * 0.3f);
}

std::string Acetylcholine::getStateDescription() const {
    std::stringstream ss;
    ss << "Acetylcholine level: " << pImpl->level << std::endl;
    ss << "M1 receptor activation: " << pImpl->mamReceptorActivation << std::endl;
    ss << "Nicotinic receptor activation: " << pImpl->nicotinicReceptorActivation << std::endl;
    ss << "Plasticity factor: " << getPlasticityFactor() << std::endl;
    return ss.str();
}

} // namespace nlm
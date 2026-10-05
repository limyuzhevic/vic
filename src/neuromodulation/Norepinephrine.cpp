#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceGain;
    float stressLevel;
    float locusCoeruleusActivity;
    float firingPattern;  // 0.0 = tonic, 1.0 = phasic bursts
    
    // Vigilance-related parameters
    float alertnessThreshold;
    float responseBias;
    float habituationRate;
    
    // Stress response
    float cortisolLevel;
    float stressResponse;
    
    Impl() : level(0.0f), baseline(0.01f), peak(0.6f), decayRate(0.03f), releaseRate(2.0f),
             arousalLevel(0.0f), vigilanceGain(1.0f), stressLevel(0.0f),
             locusCoeruleusActivity(0.0f), firingPattern(0.0f),
             alertnessThreshold(0.2f), responseBias(0.0f), habituationRate(0.01f),
             cortisolLevel(0.0f), stressResponse(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    // Add realistic noise and state-dependent fluctuations
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.02f;
    float stateModulation = pImpl->firingPattern * 0.1f;  // Phasic firing adds variability
    return pImpl->level + noise + stateModulation;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update arousal state based on level
    updateArousalState();
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity bidirectionally: low levels enhance LTP, high levels enhance LTD
    // Based on the Yerkes-Dodson law of arousal and learning
    
    // Validate state before calculating
    if (std::isnan(pImpl->arousalLevel) || std::isinf(pImpl->arousalLevel)) {
        NLM_LOG_WARNING("Invalid NE arousal level: " + std::to_string(pImpl->arousalLevel));
        return 1.0f;  // Safe default
    }
    
    float arousal = pImpl->arousalLevel;
    
    if (arousal < 0.3f) {
        // Low arousal: enhances LTP, promotes learning of new information
        return 1.0f + arousal * 0.8f;  // 1.0 to 1.8
    } else if (arousal < 0.7f) {
        // Moderate arousal: balanced plasticity
        return 1.0f + 0.8f - std::abs(arousal - 0.5f) * 1.6f;  // 1.0 to 1.8, peak at 0.5
    } else {
        // High arousal: enhances LTD, promotes memory updating
        return 1.0f - (arousal - 0.7f) * 0.9f;  // 1.0 to 0.1
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics based on locus coerulean firing patterns and stress response
    
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Stress response integration
    if (pImpl->stressResponse > 0.0f) {
        pImpl->level += pImpl->stressResponse * pImpl->releaseRate * 0.3f;
        pImpl->stressResponse *= 0.9f;  // Decay stress response
    }
    
    // Vigilance maintenance
    if (pImpl->arousalLevel > pImpl->alertnessThreshold) {
        float vigilanceMaintenance = pImpl->vigilanceGain * 0.05f * static_cast<float>(dt);
        pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceMaintenance);
    }
    
    // Habituation to sustained stimulation
    pImpl->level *= (1.0f - pImpl->habituationRate * static_cast<float>(dt));
    
    // Cortisol interaction: stress hormone modulates NE response
    if (pImpl->cortisolLevel > 0.1f) {
        pImpl->level *= (1.0f + pImpl->cortisolLevel * 0.3f);
    }
}

void Norepinephrine::signalAlertness(float intensity, bool novel, bool challenging) {
    // Real alertness signaling based on stimulus properties
    // LC-NE system responds to surprising, challenging, or threatening stimuli
    
    float response = 0.0f;
    
    // Novelty response: LC neurons show burst firing to novel stimuli
    if (novel) {
        response += intensity * 2.0f;  // Strong phasic response to novelty
        pImpl->firingPattern = 1.0f;  // Phasic firing mode
    }
    
    // Challenge response: difficulty increases NE release
    if (challenging) {
        response += intensity * 1.5f;
        pImpl->stressLevel += intensity * 0.3f;
    }
    
    // Update arousal level based on overall response
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + response * 0.2f);
    
    // Update NE level
    if (response > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate * 0.15f);
    }
}

void Norepinephrine::signalStress(float stressorIntensity, float duration) {
    // Stress response via sympathetic nervous system activation
    // Sustained stress leads to cortisol release and NE system changes
    
    pImpl->stressResponse += stressorIntensity * duration * 0.5f;
    
    // Chronic stress reduces baseline NE levels
    if (duration > 100.0f) {  // Prolonged stress
        pImpl->baseline *= 0.95f;
    }
    
    // Acute stress triggers NE surge
    if (stressorIntensity > 0.7f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + stressorIntensity * 2.0f);
    }
}

void Norepinephrine::updateArousalState() {
    // Update arousal state variables based on current NE level
    
    if (pImpl->level > pImpl->alertnessThreshold * 1.5f) {
        // High NE: high arousal state, ready for action
        pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + 0.1f);
        pImpl->responseBias = 1.0f;  // Approach bias
    } else if (pImpl->level > pImpl->alertnessThreshold) {
        // Moderate NE: optimal arousal
        pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + 0.05f);
        pImpl->responseBias = 0.5f;  // Balanced response
    } else {
        // Low NE: low arousal, fatigue
        pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - 0.05f);
        pImpl->responseBias = 0.0f;  // Withdrawal bias
    }
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getResponseBias() const {
    return pImpl->responseBias;
}

bool Norepinephrine::isStressed() const {
    return pImpl->stressLevel > 0.5f;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 0.0f;
    pImpl->stressLevel = 0.0f;
    pImpl->cortisolLevel = 0.0f;
    pImpl->stressResponse = 0.0f;
    pImpl->firingPattern = 0.0f;
    pImpl->responseBias = 0.0f;
    
    // Validate reset state
    if (std::isnan(pImpl->level) || std::isinf(pImpl->level) ||
        std::isnan(pImpl->arousalLevel) || std::isinf(pImpl->arousalLevel) ||
        std::isnan(pImpl->stressLevel) || std::isinf(pImpl->stressLevel)) {
        NLM_LOG_ERROR("Invalid state after NE reset");
        pImpl->level = pImpl->baseline;  // Force recovery
        pImpl->arousalLevel = 0.0f;
        pImpl->stressLevel = 0.0f;
    }
}

} // namespace nlm
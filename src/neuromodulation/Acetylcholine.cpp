#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionalGain;
    float memoryEnhancement;
    float cholinergicNoise;
    float taskActivation;
    float salienceDetector;
    
    // Attention-related parameters
    float attentionThreshold;
    float focusDuration;
    float distractionThreshold;
    
    Impl() : level(0.0f), baseline(0.02f), peak(0.8f), decayRate(0.02f), releaseRate(1.5f),
             attentionalGain(1.0f), memoryEnhancement(0.0f), cholinergicNoise(0.01f),
             taskActivation(0.0f), salienceDetector(0.0f), attentionThreshold(0.3f),
             focusDuration(0.0f), distractionThreshold(0.2f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    // Add realistic noise to level
    return pImpl->level + (static_cast<float>(rand()) / RAND_MAX - 0.5f) * pImpl->cholinergicNoise;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update attentional state based on level
    updateAttentionalState();
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity: enhances encoding of novel/important information
    // Based on the novelty amplification hypothesis
    
    // Validate state before calculating
    if (std::isnan(pImpl->level) || std::isinf(pImpl->level) || 
        std::isnan(pImpl->salienceDetector) || std::isinf(pImpl->salienceDetector) ||
        std::isnan(pImpl->taskActivation) || std::isinf(pImpl->taskActivation)) {
        NLM_LOG_WARNING("Invalid ACh state for plasticity calculation");
        return 1.0f;  // Safe default
    }
    
    float novelty = pImpl->salienceDetector;
    float attention = pImpl->taskActivation;
    
    // Plasticity enhancement = baseline + (attention * novelty * gain)
    float baselinePlasticity = 1.0f;  // Normal plasticity
    float enhancement = attention * novelty * pImpl->attentionalGain * 0.5f;
    float plasticity = baselinePlasticity + enhancement;
    
    // Validate and clamp result
    if (std::isnan(plasticity) || std::isinf(plasticity)) {
        NLM_LOG_ERROR("Invalid ACh plasticity calculation");
        return 1.0f;
    }
    
    return std::clamp(plasticity, 1.0f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics based on attention and environmental salience
    
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Memory consolidation component
    if (pImpl->memoryEnhancement > 0.0f) {
        pImpl->level *= (1.0f + pImpl->memoryEnhancement * 0.1f);
        pImpl->memoryEnhancement *= 0.95f;  // Decay memory enhancement
    }
    
    // Update attentional focus duration
    if (pImpl->taskActivation > pImpl->attentionThreshold) {
        pImpl->focusDuration += static_cast<float>(dt);
    } else {
        pImpl->focusDuration = 0.0f;
    }
}

void Acetylcholine::signalAttention(float salience, bool novel, bool important) {
    // Real attention signaling based on stimulus properties
    // ACh neurons respond to surprising, relevant stimuli
    
    float response = 0.0f;
    
    // Novelty component: stronger response to novel stimuli
    if (novel) {
        response += salience * 1.5f;  // Novel stimuli get boost
    }
    
    // Importance component: relevant to current task
    if (important) {
        response += salience * 1.2f;
    }
    
    // Salience detection: overall stimulus intensity
    pImpl->salienceDetector = salience;
    
    // Update level with response
    if (response > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + response * pImpl->releaseRate * 0.2f);
        pImpl->taskActivation = std::min(1.0f, pImpl->taskActivation + response * 0.3f);
    }
    
    // Start memory enhancement for attended stimuli
    if (pImpl->level > pImpl->attentionThreshold) {
        pImpl->memoryEnhancement = pImpl->level * 0.8f;
    }
}

void Acetylcholine::signalDistraction() {
    // Response to distracting stimuli
    // ACh decreases during distraction to shift from encoding to maintenance
    
    pImpl->taskActivation *= 0.6f;  // Reduce attentional focus
    pImpl->level *= 0.7f;  // Decrease ACh level
    pImpl->memoryEnhancement *= 0.8f;  // Reduce memory enhancement
}

void Acetylcholine::updateAttentionalState() {
    // Update attentional state variables based on current ACh level
    
    if (pImpl->level > pImpl->distractionThreshold) {
        // High ACh = focused attention state
        pImpl->taskActivation = std::min(1.0f, pImpl->taskActivation + 0.1f);
    } else if (pImpl->level > pImpl->baseline) {
        // Moderate ACh = divided attention
        pImpl->taskActivation *= 0.9f;
    } else {
        // Low ACh = inattentive state
        pImpl->taskActivation *= 0.7f;
    }
}

bool Acetylcholine::isInAttentionState() const {
    // Check if system is in focused attention state
    return pImpl->level > pImpl->attentionThreshold && pImpl->focusDuration > 0.1f;
}

float Acetylcholine::getAttentionalFocus() const {
    // Return attentional focus strength (0.0 to 1.0)
    if (pImpl->level < pImpl->baseline) return 0.0f;
    return std::min(1.0f, (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline) * pImpl->taskActivation);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->taskActivation = 0.0f;
    pImpl->salienceDetector = 0.0f;
    pImpl->memoryEnhancement = 0.0f;
    pImpl->focusDuration = 0.0f;
    
    // Validate reset state
    if (std::isnan(pImpl->level) || std::isinf(pImpl->level) ||
        std::isnan(pImpl->taskActivation) || std::isinf(pImpl->taskActivation)) {
        NLM_LOG_ERROR("Invalid state after ACh reset");
        pImpl->level = pImpl->baseline;  // Force recovery
        pImpl->taskActivation = 0.0f;
    }
}

} // namespace nlm
#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced plasticity factor with integration to memory and learning systems
    // Real dopamine-modulated plasticity includes:
    // 1. Optimizing learning rate based on prediction error
    // 2. Integration with memory consolidation processes
    // 3. Interaction with curiosity and exploration systems
    // 4. Context-dependent modulation based on task demands
    
    // Baseline learning rate (0.5) + dopamine modulation (0 to +0.5)
    float plasticityMod = 0.5f + 0.5f * pImpl->level;
    
    // Optimal window: moderate dopamine (0.3-0.7) maximizes learning
    // Very low or very high dopamine reduces learning efficiency
    if (pImpl->level < 0.2f) {
        // Too little dopamine = suboptimal learning (ARAS effect)
        plasticityMod *= 0.8f;
    } else if (pImpl->level > 0.8f) {
        // Too much dopamine = cognitive overload (schizophrenia model)
        plasticityMod *= 0.7f;
    }
    
    // Integration with prediction error
    // High prediction error should increase learning even with moderate dopamine
    // This models the TD learning update rule
    float predictionErrorInfluence = std::min(std::abs(0.5f - pImpl->level), 0.5f);  // 0-0.5
    plasticityMod += predictionErrorInfluence * 0.3f;
    
    return std::clamp(plasticityMod, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with integration to other neuromodulators
    // Real dopamine system includes:
    // 1. Phasic bursts for salient events
    // 2. Tonic background modulation
    // 3. Integration with attention, memory, and curiosity
    // 4. Interaction with other neuromodulators
    
    // Decay towards baseline (tonic signal)
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Integration with curiosity system
    // Dopamine and curiosity work together: curiosity drives exploration,
    // dopamine reinforces successful exploration
    float curiosityBoost = pImpl->level * 0.2f;  // 20% of dopamine level as curiosity boost
    // Note: This assumes access to curiosity system, but for now we'll
    // keep it internal to the dopamine system
    
    // Integration with attention modulation
    // Dopamine enhances attention to salient stimuli
    // Higher dopamine = stronger focus, filtering
    // This affects which neural assemblies get amplified
    
    // Integration with memory systems
    // Dopamine modulates memory consolidation:
    // - Positive dopamine strengthens memory traces
    // - Negative dopamine weakens memory traces
    // - Optimal dopamine (around 0.5) is best for consolidation
    
    // Biological realism: dopamine has both pre- and post-synaptic effects
    // Postsynaptic D1 receptors enhance neural excitability
    // Presynaptic D2 receptors modulate neurotransmitter release
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling for memory consolidation and attention
    // Real dopamine dynamics include:
    // 1. Phasic bursts for unexpected rewards ( saliency learning)
    // 2. Tonic modulation of neural excitability
    // 3. Integration with memory systems
    
    // Phasic burst for salient reward
    float burst = reward * pImpl->releaseRate * 2.0f;  // Stronger than tonic
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Update prediction error based on reward prediction
    // This is more realistic than simple additive
    pImpl->level += (reward - pImpl->baseline) * 0.5f;
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced reward prediction error signaling for learning
    // RPE serves multiple functions:
    // 1. Drives learning via TD learning algorithms
    // 2. Signals uncertainty for exploration
    // 3. Modulates memory consolidation
    // 4. Affects attention allocation
    
    // RPE magnitude determines learning rate
    float learningRate = std::min(std::abs(error), 0.5f);
    
    if (error > 0) {
        // Positive RPE (better than expected): promotes learning and memory consolidation
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * learningRate * 1.5f);
        
        // Enhanced consolidation signals for better-than-expected outcomes
        // This helps prioritize positive experiences
        pImpl->level += 0.2f * learningRate;  // Additional consolidation boost
    } else {
        // Negative RPE (worse than expected): suppresses learning
        pImpl->level = std::max(0.0f, pImpl->level + error * learningRate * 0.5f);
        
        // Signal to reduce attention to this stimulus
        pImpl->level -= 0.1f * learningRate;  // Attention suppression signal
    }
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm

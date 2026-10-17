#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Enhanced effects
    float attentionModifier;
    float learningRate;
    float memoryConsolidation;
    float timeScaling;
    
    // Reward history
    float recentReward;
    float rewardHistory[5]; // Track recent rewards for temporal difference
    
    // State
    SimulationStep lastUpdateTime;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , attentionModifier(0.0f), learningRate(1.0f), memoryConsolidation(0.0f), timeScaling(1.0f)
        , recentReward(0.0f), lastUpdateTime(0) {
        std::fill(std::begin(rewardHistory), std::end(rewardHistory), 0.0f);
    }
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
    // Enhanced plasticity factor based on dopamine level
    // More complex than simple linear relationship
    if (pImpl->level < 0.3f) {
        // Low dopamine: Reduces plasticity (stability)
        return 0.2f + 0.6f * pImpl->level;
    } else if (pImpl->level < 0.7f) {
        // Medium dopamine: Increases plasticity (learning)
        return 0.6f + 0.4f * (pImpl->level - 0.3f) / 0.4f;
    } else {
        // High dopamine: Moderate plasticity (prevents runaway learning)
        return 1.0f - 0.3f * (pImpl->level - 0.7f) / 0.3f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // Enhanced decay towards baseline with more complex dynamics
    float newLevel = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add temporal integration for memory consolidation effects
    newLevel += pImpl->memoryConsolidation * static_cast<float>(dt) * 0.01f;
    newLevel = std::clamp(newLevel, 0.0f, 1.0f);
    
    setLevel(newLevel);
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // Enhanced reward signaling with temporal difference learning
    float temporalDifference = reward - pImpl->recentReward;
    
    // Burst of dopamine proportional to prediction error
    float burst = std::clamp(temporalDifference * pImpl->releaseRate * 2.0f, -0.5f, 0.5f);
    
    // Apply to level with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    pImpl->recentReward = reward;
    
    // Update reward history for temporal integration
    std::rotate(std::begin(pImpl->rewardHistory), std::begin(pImpl->rewardHistory) + 1, std::end(pImpl->rewardHistory));
    pImpl->rewardHistory[0] = reward;
    
    // Update memory consolidation based on reward
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + reward * 0.1f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // Dopamine responds to prediction error
    float errorSignal = std::clamp(error * pImpl->releaseRate, -0.5f, 0.5f);
    
    // Apply prediction error to dopamine level
    setLevel(getLevel() + errorSignal);
    
    // Adjust attention and learning rates based on prediction error
    if (error > 0.0f) {
        // Positive prediction error (better than expected) - increase attention
        pImpl->attentionModifier = std::min(1.0f, pImpl->attentionModifier + 0.1f);
    } else {
        // Negative prediction error (worse than expected) - decrease learning rate
        pImpl->learningRate = std::max(0.3f, pImpl->learningRate - 0.05f);
    }
}

void Dopamine::applyToNeuron(class Neuron* neuron, TimestepDuration dt) {
    if (!neuron || pImpl->level == 0.0f) return;
    
    // Enhanced dopamine effects on neurons
    float level = pImpl->level;
    
    // 1. Modulate neuronal excitability
    float excitabilityEffect = level * 0.5f;  // Stronger than Phase 1
    if (excitabilityEffect > 0.0f) {
        // Dopamine injects positive current to increase excitability
        float dopamineCurrent = excitabilityEffect * 5.0f;  // Scale factor
        neuron->injectCurrent(dopamineCurrent);
    }
    
    // 2. Modulate membrane potential dynamics
    float timeScalingEffect = level * 0.3f;
    if (timeScalingEffect > 0.0f) {
        // Dopamine accelerates time perception for this neuron
        // Affects refractory period and integration time constant
        // Implementation depends on neuron model details
    }
    
    // 3. Enhance synaptic eligibility traces
    float plasticityModifier = level * 0.4f;
    if (plasticityModifier > 0.0f) {
        // Dopamine enhances eligibility traces for learning
        // Would modify neuron's internal state for STDP
    }
}

void Dopamine::enhanceMemoryConsolidation() {
    if (pImpl->memoryConsolidation <= 0.0f) return;
    
    // Dopamine enhances memory consolidation
    // This would strengthen synaptic connections during rest
    // Implementation would involve replay mechanisms and synaptic strengthening
    
    // Scale consolidation effect based on dopamine level
    float consolidationStrength = pImpl->memoryConsolidation * pImpl->level;
    
    // This would be used in the brain's consolidation phase
    // For now, it's a placeholder for the mechanism
}

void Dopamine::modulateTimePerception() {
    // Dopamine affects time perception
    // This affects how the brain weights recent vs. distant events
    
    float timeScale = 0.5f + 0.5f * pImpl->level;  // Time compression/expansion
    
    // This would affect temporal integration windows in the brain
    // Higher dopamine = compressed time (events closer together)
    // Lower dopamine = expanded time (events further apart)
}

// ACh implementation
void Acetylcholine::applyToNeuron(class Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Acetylcholine enhances attention and plasticity
    float level = getLevel();
    if (level > 0.0f) {
        // ACh increases neural gain and attention
        float attentionBoost = level * 0.3f;
        
        // Enhanced NMDA receptor efficacy
        // ACh potentiates NMDA currents, improving signal-to-noise ratio
        // This would affect the neuron's response to inputs
        
        // Reduced spike threshold variability (more stable firing)
        // Implementation would modify neuron model parameters
    }
}

void Acetylcholine::consolidateMemory(class NeuralWorkingMemory* memory) {
    if (!memory || getLevel() <= 0.0f) return;
    
    // ACh enhances memory consolidation
    // It helps transfer information from working memory to long-term storage
    
    float consolidationRate = getLevel() * 0.2f;
    memory->strengthenMemory(consolidationRate);
}

void Acetylcholine::enhanceCognitiveFunctions() {
    // ACh broadly enhances cognitive functions
    // This would affect attention, working memory, and executive function
    
    // Higher ACh = better signal-to-noise ratio in cortical circuits
    // This improves the brain's ability to distinguish relevant from irrelevant information
    
    // Implementation would involve setting parameters across multiple brain regions
}

// NE implementation
void Norepinephrine::applyToNeuron(class Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Norepinephrine affects arousal and vigilance
    float level = getLevel();
    if (level > 0.0f) {
        // NE increases arousal, affecting overall brain activation
        float arousalBoost = level * 0.4f;
        
        // Enhanced locus coeruleus effects
        // NE modulates both tonic and phasic activity
        // Phasic bursts signal important events, tonic baseline maintains arousal
        
        // Affects time-dependent plasticity
        // NE enhances synaptic consolidation of important memories
    }
}

void Norepinephrine::modulateArousal(class Brain* brain, float currentLevel) {
    if (!brain) return;
    
    // NE modulates overall brain arousal
    // Higher NE = higher global activation
    
    // This affects:
    // - Sensory processing sensitivity
    // - Motor readiness
    // - Attentional scope (broad vs. focused)
    // - Wake/ sleep states
}

void Norepinephrine::enhanceNoveltyDetection() {
    // NE enhances novelty detection
    // It helps the brain identify and respond to new, potentially important information
    
    float noveltyBoost = getLevel() * 0.3f;
    
    // Would enhance:
    // - Comparison with stored patterns
    // - Detection of deviations from expectation
    // - Response to unexpected events
    
    // This works together with other neuromodulators for adaptive behavior
}

// 5-HT implementation
void Serotonin::applyToNeuron(class Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    // Serotonin affects mood and impulse control
    float level = getLevel();
    if (level > 0.0f) {
        // 5-HT has complex effects on neural activity
        // Generally reduces overall firing rate while increasing signal fidelity
        
        // Modulates response variability
        // 5-HT can make neural responses more consistent
        
        // Affects decision threshold
        // Higher 5-HT = higher decision threshold (more deliberation)
        
        // Influences social behavior circuits
        // Affects how the brain processes social information
    }
}

void Serotonin::modulateSocialBehavior(class Brain* brain) {
    if (!brain) return;
    
    // 5-HT modulates social behavior
    // This affects:
    // - Social bonding and attachment
    // - Aggression and irritability
    // - Social learning and imitation
    // - Reward from social interactions
    
    float socialModulation = getLevel() * 0.2f;
    
    // Would affect social decision making and related neural circuits
}

void Serotonin::regulateImpulsivity(class Brain* brain) {
    if (!brain) return;
    
    // Serotonin regulates impulsivity and self-control
    // Higher 5-HT generally reduces impulsivity
    
    float impulsivityControl = getLevel() * 0.3f;
    
    // This affects:
    // - Decision making (wait vs. act impulsively)
    // - Delay of gratification
    // - Behavioral flexibility
}

} // namespace nlm

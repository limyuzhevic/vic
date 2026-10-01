#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>
#include <chrono>

namespace nlm {

// =============================================================================
// ENHANCED DOPAMINE NEUROMODULATOR
// =============================================================================

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityMax;
    float excitationMax;
    float workingMemoryConsolidation;
    float rewardPredictionError;
    float predictionErrorLearningRate;
    
    // Neural excitability components
    float currentExcitabilityModulation;
    float synapticGainModulation;
    
    // Plasticity components
    float stdpLTPModulation;
    float stdpLTDModulation;
    float eligibilityTraceModulation;
    
    // Working memory components
    float memoryReplayStrength;
    float memoryConsolidationThreshold;
    float workingMemoryEnhancement;
    
    // Integration with other systems
    float attentionModulation;
    float arousalModulation;
    
    // History and tracking
    float history[max_history_size];
    SimulationStep lastReplayStep;
    
    // Random generator for learning dynamics
    std::mt19937 rng;
    
    // Reference to brain for integration
    class Brain* brain_;
    
    static const size_t max_history_size = 100;
    
    Impl() : level(0.0f), baseline(0.1f), peak(2.0f), decayRate(0.05f), releaseRate(1.0f),
             plasticityMax(2.0f), excitationMax(1.5f), workingMemoryConsolidation(1.2f),
             rewardPredictionError(0.0f), predictionErrorLearningRate(0.3f),
             currentExcitabilityModulation(1.0f), synapticGainModulation(1.0f),
             stdpLTPModulation(1.0f), stdpLTDModulation(1.0f), eligibilityTraceModulation(1.0f),
             memoryReplayStrength(0.5f), memoryConsolidationThreshold(0.3f),
             workingMemoryEnhancement(1.1f), attentionModulation(1.0f),
             arousalModulation(1.0f), lastReplayStep(0), brain_(nullptr) {
        rng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
        for (size_t i = 0; i < max_history_size; ++i) {
            history[i] = baseline;
        }
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    
    // Update history
    std::memmove(pImpl->history, pImpl->history + 1, sizeof(float) * (pImpl->max_history_size - 1));
    pImpl->history[pImpl->max_history_size - 1] = pImpl->level;
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced plasticity factor based on multiple dopamine components
    float rewardPLEffect = std::tanh(pImpl->level * 2.0f);
    float rpeEffect = std::clamp(pImpl->rewardPredictionError, 0.0f, 1.0f);
    float combinedEffect = rewardPLEffect * 0.6f + rpeEffect * 0.4f;
    return 1.0f + combinedEffect * (pImpl->plasticityMax - 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update neuromodulatory effects
    updateNeuralExcitability(pImpl->level, dt);
    updatePlasticity(pImpl->level, dt);
    updateWorkingMemoryConsolidation(pImpl->level, dt);
}

void Dopamine::updateNeuralExcitability(float level, TimestepDuration dt) {
    // Neural excitability modulation
    pImpl->currentExcitabilityModulation = 1.0f + level * pImpl->excitationMax;
    
    // Synaptic gain modulation
    pImpl->synapticGainModulation = 1.0f + level * 0.3f;
}

void Dopamine::updatePlasticity(float level, TimestepDuration dt) {
    // STDP modulation
    pImpl->stdpLTPModulation = 1.0f + level * 0.5f;
    pImpl->stdpLTDModulation = 1.0f - level * 0.3f;
    
    // Eligibility trace modulation
    pImpl->eligibilityTraceModulation = 1.0f + level * 0.4f;
}

void Dopamine::updateWorkingMemoryConsolidation(float level, TimestepDuration dt) {
    // Working memory enhancement
    pImpl->workingMemoryEnhancement = 1.0f + level * (pImpl->workingMemoryConsolidation - 1.0f);
    
    // Memory consolidation threshold modulation
    if (level > pImpl->memoryConsolidationThreshold) {
        // Consolidate memories when dopamine is high
        if (pImpl->brain_) {
            // Trigger memory consolidation via episodic memory
            auto* episodicMem = pImpl->brain_->getEpisodicMemory();
            if (episodicMem) {
                episodicMem->consolidate(0.5f + level * 0.5f);
            }
        }
    }
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    float burst = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    setLevel(burst);
    
    // Update reward prediction error
    pImpl->rewardPredictionError = std::clamp(pImpl->rewardPredictionError + reward * pImpl->predictionErrorLearningRate, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    float newLevel = pImpl->level + error * pImpl->releaseRate;
    setLevel(newLevel);
    
    // Update RPE component
    pImpl->rewardPredictionError = std::clamp(error, 0.0f, 1.0f);
}

void Dopamine::applyEffects(class Brain* brain) {
    // Store brain reference for memory operations
    pImpl->brain_ = brain;
}

// =============================================================================
// ENHANCED ACETYLCHOLINE NEUROMODULATOR
// =============================================================================

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    float memoryEncoding;
    float corticalExcitability;
    float workingMemoryCapacity;
    float memoryRehearsal;
    
    // Attention components
    float bottomUpSalience;
    float topDownBias;
    float selectionStrength;
    
    // Integration with working memory
    float workingMemorySNR;
    float encodingLocusCoherence;
    
    // History tracking
    float attentionHistory[max_history_size];
    
    // Reference to brain for integration
    class Brain* brain_;
    
    static const size_t max_history_size = 100;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.5f), decayRate(0.03f), releaseRate(0.8f),
             attentionModulation(1.0f), memoryEncoding(1.3f), corticalExcitability(1.2f),
             workingMemoryCapacity(1.0f), memoryRehearsal(1.1f),
             bottomUpSalience(0.0f), topDownBias(0.0f), selectionStrength(1.0f),
             workingMemorySNR(1.0f), encodingLocusCoherence(1.0f), brain_(nullptr) {
        for (size_t i = 0; i < max_history_size; ++i) {
            attentionHistory[i] = baseline;
        }
    }
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
    
    // Update history
    std::memmove(pImpl->attentionHistory, pImpl->attentionHistory + 1, sizeof(float) * (pImpl->max_history_size - 1));
    pImpl->attentionHistory[pImpl->max_history_size - 1] = pImpl->level;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances memory encoding and attention
    return 1.0f + pImpl->level * (pImpl->memoryEncoding - 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention system effects
    updateAttentionEffects(pImpl->level, dt);
    
    // Update memory encoding effects
    updateMemoryEncoding(pImpl->level, dt);
}

void Acetylcholine::updateAttentionEffects(float level, TimestepDuration dt) {
    // Attention modulation
    pImpl->attentionModulation = 1.0f + level * 0.8f;
    
    // Working memory signal-to-noise enhancement
    pImpl->workingMemorySNR = 1.0f + level * 0.5f;
    
    // Cortical excitability
    pImpl->corticalExcitability = 1.0f + level * 0.3f;
}

void Acetylcholine::updateMemoryEncoding(float level, TimestepDuration dt) {
    // Memory encoding enhancement
    pImpl->memoryEncoding = 1.0f + level * 0.5f;
    
    // Working memory capacity increase
    pImpl->workingMemoryCapacity = 1.0f + level * 0.3f;
    
    // Memory rehearsal strength
    pImpl->memoryRehearsal = 1.0f + level * 0.4f;
    
    // Integrate with attention system
    if (pImpl->brain_) {
        auto* attention = pImpl->brain_->getAttention();
        if (attention) {
            // Enhance attention with acetylcholine
            float enhancedInhibition = attention->getInhibitionStrength() * pImpl->attentionModulation;
            float enhancedExcitation = attention->getExcitationStrength() * pImpl->attentionModulation;
            
            attention->setInhibitionStrength(enhancedInhibition);
            attention->setExcitationStrength(enhancedExcitation);
        }
        
        // Enhance working memory
        auto* workingMem = pImpl->brain_->getWorkingMemory();
        if (workingMem) {
            // Increase working memory capacity
            workingMem->setCapacity(static_cast<size_t>(workingMem->getCapacity() * pImpl->workingMemoryCapacity));
            
            // Enhance memory rehearsal
            if (level > 0.3f) {
                workingMem->strengthenMemory(pImpl->memoryRehearsal);
            }
        }
    }
}

void Acetylcholine::signalAttention(float salience, float bias) {
    // Bottom-up and top-down attention signals
    pImpl->bottomUpSalience = salience;
    pImpl->topDownBias = bias;
    
    // Convert to acetylcholine level
    float level = (salience + bias) * 0.5f;
    setLevel(level);
}

void Acetylcholine::enhanceWorkingMemory(NeuronId neuron, float activation) {
    // Enhance working memory encoding through acetylcholine
    if (!pImpl->brain_) return;
    
    auto* workingMem = pImpl->brain_->getWorkingMemory();
    if (workingMem) {
        // Store neuron with enhanced SNR
        float enhancedActivation = activation * pImpl->workingMemorySNR;
        workingMem->storeToNeuron(neuron, enhancedActivation);
        
        // Apply attention to this neuron
        auto* attention = pImpl->brain_->getAttention();
        if (attention) {
            attention->applyBottomUpSalience(neuron, enhancedActivation * 0.5f);
        }
    }
}

void Acetylcholine::applyEffects(class Brain* brain) {
    // Store brain reference for integration
    pImpl->brain_ = brain;
}

// =============================================================================
// STUB IMPLEMENTATIONS FOR OTHER NEUROMODULATORS
// =============================================================================

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.1f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    return 1.0f;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    return 1.0f;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

} // namespace nlm
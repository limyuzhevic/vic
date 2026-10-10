#include "../../brain/Synapse.hpp"
#include "../../brain/Brain.hpp"
#include "../../core/Random/Random.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Enhanced plasticity modulation parameters
    float stdpModulation;
    float hebbianModulation;
    float structuralModulation;
    float predictionErrorWeight;
    float rewardPredictionWeight;
    
    // Forbid factors for different plasticity types
    float forbidLTP;
    float forbidLTD;
    float inhibitStructural;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             stdpModulation(1.0f), hebbianModulation(1.0f), structuralModulation(1.0f),
             predictionErrorWeight(0.5f), rewardPredictionWeight(0.5f),
             forbidLTP(0.0f), forbidLTD(0.0f), inhibitStructural(0.0f) {}
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
    // Enhanced dopamine-modulated plasticity factor
    // Dopamine has different effects on different plasticity types
    // STDP: Potentiation and depression modulation
    // Hebbian: Correlation-based learning enhancement
    // Structural: Synaptogenesis and pruning inhibition
    
    // Calculate enhanced plasticity factor
    float baseFactor = 0.5f + 0.5f * pImpl->level;
    
    // Different modulation for different plasticity types
    // STDP is more sensitive to moderate dopamine levels
    float stdpFactor = baseFactor * (1.0f + pImpl->stdpModulation);
    // Hebbian learning is enhanced by high dopamine
    float hebbianFactor = baseFactor * (1.0f + pImpl->hebbianModulation);
    // Structural plasticity is inhibited by dopamine
    float structuralFactor = baseFactor * (1.0f - pImpl->inhibitStructural);
    
    // Return weighted average based on current plastic state
    return (stdpFactor + hebbianFactor + structuralFactor) / 3.0f;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update modulation parameters based on current level
    // This creates sophisticated neuromodulation rules
    pImpl->stdpModulation = pImpl->level * 0.8f;
    pImpl->hebbianModulation = std::min(1.0f, pImpl->level * 2.0f);
    pImpl->inhibitStructural = pImpl->level * 0.5f;
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Update modulation parameters based on reward
    // Reward strengthens certain plasticity pathways
    pImpl->stdpModulation = std::min(1.0f, pImpl->stdpModulation + 0.2f);
    pImpl->hebbianModulation = std::min(1.0f, pImpl->hebbianModulation + 0.3f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement real reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Prediction error enhances learning precision
    pImpl->predictionErrorWeight = std::min(1.0f, pImpl->predictionErrorWeight + 0.1f);
}

// Enhanced neuromodulation methods
void Dopamine::enhanceSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) return;
    
    // Apply dopamine-modulated STDP
    // Higher dopamine enhances STDP timing sensitivity
    float timingFactor = 1.0f + pImpl->level * 0.5f;
    
    // Modify synaptic weights based on dopamine level
    // This creates sophisticated neuromodulation rules
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Potentiation modulated by dopamine
                float potentiation = synapse->getWeight() * 0.01f * timingFactor;
                synapse->addToWeight(potentiation);
            } else if (dt < 0) {
                // Depression modulated by dopamine
                float depression = -synapse->getWeight() * 0.012f * timingFactor;
                synapse->addToWeight(depression);
            }
        }
    }
}

void Dopamine::enhanceHebbian(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) return;
    
    // Apply dopamine-modulated Hebbian learning
    // High dopamine enhances correlation-based learning
    float correlationFactor = 1.0f + pImpl->level * 0.8f;
    
    // Count correlated spikes
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 100.0f) {  // 100ms window
                ++correlationCount;
            }
        }
    }
    
    // Apply Hebbian weight change with dopamine modulation
    float delta = 0.01f * static_cast<float>(correlationCount) * correlationFactor;
    synapse->addToWeight(delta);
}

void Dopamine::enhanceStructuralPlasticity(Brain* brain, RandomGenerator& rng, float activityThreshold) {
    if (!brain) return;
    
    // Apply dopamine-modulated structural plasticity
    // Dopamine inhibits structural changes to stabilize learning
    float structuralInhibition = 1.0f - pImpl->inhibitStructural;
    
    // Modify synaptogenesis probability
    float synaptogenesisRate = brain->getStructuralPlasticity()->getSynaptogenesisRate();
    synaptogenesisRate *= structuralInhibition;
    
    // Modify pruning probability
    float pruningRate = brain->getStructuralPlasticity()->getPruningRate();
    pruningRate *= (1.0f + pImpl->level * 0.5f);  // High dopamine increases pruning
    
    // Apply enhanced structural plasticity
    brain->getStructuralPlasticity()->setSynaptogenesisRate(synaptogenesisRate);
    brain->getStructuralPlasticity()->setPruningRate(pruningRate);
}

void Dopamine::updatePredictionIntegration(float predictionError, float reward) {
    // Update integration with prediction system
    // Reward prediction error is a key signal for learning
    if (predictionError > 0.0f) {
        // Positive prediction error (better than expected)
        pImpl->predictionErrorWeight += predictionError * 0.1f;
        pImpl->rewardPredictionWeight *= 0.95f;  // Reduce reward prediction weight
    } else if (reward > 0.0f) {
        // Reward received
        pImpl->rewardPredictionWeight += reward * 0.2f;
        pImpl->predictionErrorWeight *= 0.9f;  // Reduce prediction error weight
    }
}

// Get enhanced neuromodulation factors
float Dopamine::getEnhancedSTDPFactor() const {
    return 1.0f + pImpl->stdpModulation;
}

float Dopamine::getEnhancedHebbianFactor() const {
    return 1.0f + pImpl->hebbianModulation;
}

float Dopamine::getEnhancedStructuralFactor() const {
    return 1.0f - pImpl->inhibitStructural;
}

float Dopamine::getPredictionErrorWeight() const {
    return pImpl->predictionErrorWeight;
}

float Dopamine::getRewardPredictionWeight() const {
    return pImpl->rewardPredictionWeight;
}

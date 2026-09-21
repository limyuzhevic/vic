// Neuromodulator system enhancements with biological accuracy
// PHASE 8: ADVANCED NEUROMODULATION WITH BIOLOGICAL PLACEMENT

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <limits>

namespace nlm {

struct Dopamine::Impl {
    // Biological parameters for dopamine dynamics
    float level;                    // Current dopamine level (0-1, normalized)
    float baseline;                 // Baseline dopamine level
    float peak;                     // Maximum dopamine level
    float decayRate;                // Decay rate (per ms)
    float releaseRate;              // Release rate (per unit reward)
    float adaptationTimeConstant;   // Time constant for adaptation (ms)
    float presynapticR;             // Presynaptic release probability
    float postsynapticR;            // Postsynaptic receptor density
    
    // Activity-dependent state variables
    float eligibilityTrace;         // Synaptic eligibility trace for STDP
    float predictionError;          // Reward prediction error
    float temporalDerivative;       // dDA/dt - rate of change
    
    // Cellular mechanisms
    float synthesisRate;            // Dopamine synthesis rate
    float reuptakeRate;             // Dopamine reuptake rate
    float diffusionRate;            // Intracellular diffusion rate
    
    // Behavioral state
    float rewardPrediction;          // Predicted reward
    float surprise;                 // Arousal/surprise signal
    float vigor;                    // Behavioral vigor/effort
    
    // Learning history
    float lastReward;               // Last reward received
    SimulationStep lastRewardStep;  // Step of last reward
    
    Impl() : level(0.05f), baseline(0.05f), peak(1.0f), decayRate(0.01f), releaseRate(0.5f),
             adaptationTimeConstant(100.0f), presynapticR(0.8f), postsynapticR(1.0f),
             eligibilityTrace(0.0f), predictionError(0.0f), temporalDerivative(0.0f),
             synthesisRate(0.02f), reuptakeRate(0.01f), diffusionRate(0.1f),
             rewardPrediction(0.0f), surprise(0.0f), vigor(0.5f),
             lastReward(0.0f), lastRewardStep(0) {
        // Initialize with biologically plausible parameters
        // Based on rodent striatal dopamine measurements
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

// Enhanced dopamine-modulated plasticity factor with synaptic dynamics
float Dopamine::getPlasticityFactor() const {
    // Biological basis: Dopamine modulates STDP and Hebbian plasticity
    // Higher dopamine increases LTP and decreases LTD
    
    // Base plasticity factor with dopamine modulation
    float plasticity = 0.5f + 0.5f * pImpl->level;
    
    // Apply asymmetric modulation: DA boosts LTP more than it suppresses LTD
    float ltpBoost = 1.0f + 1.5f * pImpl->level;  // Up to 2.5x boost
    float ltdSuppression = 1.0f - 0.3f * pImpl->level;  // Up to 30% suppression
    
    // Time-dependent modulation based on reward prediction error
    float rpeEffect = 1.0f + pImpl->predictionError * 0.5f;
    rpeEffect = std::clamp(rpeEffect, 0.5f, 2.0f);
    
    // Combine effects
    float totalFactor = plasticity * ltpBoost * ltdSuppression * rpeEffect;
    
    // Bounds and saturation
    return std::clamp(totalFactor, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Biologically realistic dopamine dynamics
    float timeStep = static_cast<float>(dt);
    
    // Update temporal derivative
    pImpl->temporalDerivative = (pImpl->level - pImpl->temporalDerivative) / pImpl->adaptationTimeConstant;
    
    // Apply homeostatic adaptation
    float homeostaticDrive = pImpl->baseline - pImpl->level;
    pImpl->level += homeostaticDrive * 0.01f * timeStep;
    
    // Apply decay with non-linear kinetics
    float decay = pImpl->decayRate * timeStep * (1.0f + pImpl->level);
    pImpl->level = std::max(0.0f, pImpl->level - decay);
    
    // Apply reuptake
    float reuptake = pImpl->reuptakeRate * timeStep * pImpl->level;
    pImpl->level -= reuptake;
    
    // Ensure bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with phasic response dynamics
    // Biologically inspired: burst firing with specific patterns
    
    // Calculate prediction error
    float expected = pImpl->rewardPrediction;
    float error = reward - expected;
    pImpl->predictionError = error;
    
    // Update reward prediction (trace learning)
    pImpl->rewardPrediction = 0.9f * pImpl->rewardPrediction + 0.1f * reward;
    
    // Phasic response: stronger for unexpected rewards
    float phasicStrength = 1.0f + std::abs(error) * 2.0f;
    
    // Temporal dynamics: burst followed by ramp-down
    float burstComponent = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * phasicStrength);
    
    // Apply burst
    pImpl->level = burstComponent;
    
    // Record reward timing
    pImpl->lastReward = reward;
    pImpl->surprise += std::abs(error);
    
    // Update vigor based on reward prediction error
    pImpl->vigor = 0.5f + 0.5f * std::min(1.0f, error * 2.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dedicated RPE signaling for reinforcement learning
    // Complementary to direct reward signaling
    
    // Update prediction error
    pImpl->predictionError = error;
    
    // Modulate level based on RPE sign and magnitude
    float rpeEffect = error * pImpl->releaseRate;
    
    if (error > 0.0f) {
        // Positive RPE: increase dopamine level
        pImpl->level = std::min(pImpl->peak, pImpl->level + rpeEffect * 2.0f);
    } else {
        // Negative RPE: decrease dopamine level but not below baseline
        pImpl->level = std::max(pImpl->baseline, pImpl->level + rpeEffect);
    }
    
    // Update surprise and learning rate
    pImpl->surprise = std::abs(error);
    
    // RPE modulates plasticity eligibility
    pImpl->eligibilityTrace += error * 0.1f;
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
}

// Action potential generation for dopaminergic neurons
void Dopamine::generateActionPotential() {
    // Simulate burst firing pattern characteristic of dopamine neurons
    // Based on in vivo recordings
    
    // Burst of 3-5 spikes at high frequency (20-50 Hz)
    float burstFrequency = 30.0f;  // Hz
    float burstDuration = 50.0f;  // ms
    
    // Generate spike train
    float timeStep = 1.0f / burstFrequency;
    int spikeCount = static_cast<int>(burstDuration * burstFrequency / 1000.0f);
    
    for (int i = 0; i < spikeCount; ++i) {
        // Each spike increases dopamine release
        float spikeEffect = 0.2f * (1.0f - static_cast<float>(i) / spikeCount);
        pImpl->level += spikeEffect;
        
        // Allow some decay between spikes
        pImpl->level *= 0.9f;
    }
    
    // Clamp to peak
    pImpl->level = std::min(pImpl->level, pImpl->peak);
}

// Get reward prediction error
float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

// Get behavioral vigor
float Dopamine::getVigor() const {
    return pImpl->vigor;
}

// Get surprise signal
float Dopamine::getSurprise() const {
    return pImpl->surprise;
}

// Update eligibility trace for synaptic plasticity
void Dopamine::updateEligibilityTrace(float synapseActivity, TimestepDuration dt) {
    // Update synaptic eligibility trace for reward-modulated learning
    
    float timeStep = static_cast<float>(dt);
    
    // Update trace based on synapse activity
    pImpl->eligibilityTrace += synapseActivity * timeStep * 0.1f;
    
    // Apply decay
    pImpl->eligibilityTrace *= (1.0f - 0.01f * timeStep);
    
    // Bound eligibility trace
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
}

// Get current eligibility trace
float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

// Reset dopamine system
void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->rewardPrediction = 0.0f;
    pImpl->surprise = 0.0f;
    pImpl->vigor = 0.5f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->temporalDerivative = 0.0f;
}

// Curiosity system enhancements with biological accuracy
// PHASE 8: ADVANCED EXPLORATION DRIVES WITH PREDICTION ERROR

struct Curiosity::Impl {
    // Neural correlates of curiosity
    class Brain* brain;
    
    // Exploration drive components
    float level;                    // Overall curiosity level (0-1)
    float noveltySensitivity;       // Sensitivity to novel stimuli
    float predictionErrorSensitivity; // Sensitivity to prediction errors
    float arousalBaseline;          // Baseline arousal level
    float explorationCost;          // Cost of exploration behavior
    
    // Prediction error history
    float predictionErrorHistory;    // Moving average of prediction errors
    float predictionErrorVariance;   // Variance of prediction errors
    
    // Novelty detection state
    float noveltyAccumulator;       // Accumulated novelty experience
    float noveltyDecay;              // Novelty decay rate
    
    // Arousal system
    float arousalLevel;             // Overall arousal (0-1)
    float attentionGain;             // Attention modulation by arousal
    float memoryAccessRate;          // Rate of memory sampling
    
    // Reward prediction
    float expectedNovelty;           // Predicted novelty value
    float actualNovelty;             // Actual experienced novelty
    
    // Learning components
    float learningRate;              // Curiosity-driven learning rate
    float metaLearningRate;          // Meta-learning rate
    float strategyUpdateFrequency;   // How often strategies are updated
    
    Impl() : brain(nullptr), level(0.3f), noveltySensitivity(0.7f), 
             predictionErrorSensitivity(0.6f), arousalBaseline(0.2f),
             explorationCost(0.1f), predictionErrorHistory(0.0f),
             predictionErrorVariance(0.0f), noveltyAccumulator(0.0f),
             noveltyDecay(0.05f), arousalLevel(0.2f), attentionGain(1.0f),
             memoryAccessRate(0.3f), expectedNovelty(0.0f), actualNovelty(0.0f),
             learningRate(0.01f), metaLearningRate(0.005f),
             strategyUpdateFrequency(100.0f) {
        // Initialize with biologically plausible parameters
        // Based on rodent exploration studies
    }
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Enhanced Curiosity system initialized with prediction error sensitivity");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Biologically realistic curiosity dynamics
    float timeStep = static_cast<float>(dt);
    
    // Update prediction error statistics
    pImpl->predictionErrorHistory = 0.9f * pImpl->predictionErrorHistory + 0.1f * predictionError;
    pImpl->predictionErrorVariance = 0.95f * pImpl->predictionErrorVariance + 
                                    0.05f * (predictionError - pImpl->predictionErrorHistory) * 
                                           (predictionError - pImpl->predictionErrorHistory);
    
    // Curiosity increases with prediction error (information gain)
    float predictionErrorComponent = pImpl->predictionErrorSensitivity * predictionError;
    
    // Novelty detection with accumulation
    pImpl->noveltyAccumulator = 0.9f * pImpl->noveltyAccumulator + 0.1f * novelty;
    pImpl->actualNovelty = novelty;
    
    // Expected vs actual novelty comparison
    float noveltyError = std::abs(novelty - pImpl->expectedNovelty);
    float noveltyComponent = pImpl->noveltySensitivity * noveltyError;
    
    // Arousal system activation
    pImpl->arousalLevel = pImpl->arousalBaseline + 
                         predictionErrorComponent * 0.5f + 
                         noveltyComponent * 0.5f;
    pImpl->arousalLevel = std::clamp(pImpl->arousalLevel, 0.0f, 1.0f);
    
    // Update attention gain
    pImpl->attentionGain = 1.0f + pImpl->arousalLevel * 2.0f;
    
    // Memory access rate increases with arousal
    pImpl->memoryAccessRate = 0.3f + pImpl->arousalLevel * 0.7f;
    
    // Exploration decision
    float explorationDrive = predictionErrorComponent + noveltyComponent;
    
    // Apply exploration cost
    float netExploration = explorationDrive - pImpl->explorationCost * pImpl->level;
    
    // Smooth update with arousal modulation
    pImpl->level += netExploration * pImpl->arousalLevel * timeStep * 0.1f;
    
    // Natural decay of curiosity
    pImpl->level *= (1.0f - pImpl->noveltyDecay * timeStep);
    
    // Bound level
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Update expected novelty for next time
    pImpl->expectedNovelty = 0.8f * pImpl->expectedNovelty + 0.2f * novelty;
}

float Curiosity::getExplorationDrive() const {
    // Curiosity-driven exploration
    return pImpl->level * pImpl->arousalLevel;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltySensitivity = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorSensitivity = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->noveltyAccumulator = 0.0f;
    pImpl->predictionErrorHistory = 0.0f;
    pImpl->predictionErrorVariance = 0.0f;
    pImpl->arousalLevel = pImpl->arousalBaseline;
    pImpl->attentionGain = 1.0f;
    pImpl->memoryAccessRate = 0.3f;
}

// Get prediction error variance for uncertainty sampling
float Curiosity::getPredictionErrorVariance() const {
    return pImpl->predictionErrorVariance;
}

// Get novelty accumulation for memory consolidation
float Curiosity::getNoveltyAccumulator() const {
    return pImpl->noveltyAccumulator;
}

// Get attention gain level
float Curiosity::getAttentionGain() const {
    return pImpl->attentionGain;
}

// Get memory access rate
float Curiosity::getMemoryAccessRate() const {
    return pImpl->memoryAccessRate;
}

// Get arousal level
float Curiosity::getArousalLevel() const {
    return pImpl->arousalLevel;
}

// Strategic exploration based on curiosity
ActionType Curiosity::selectExplorationAction(const std::vector<NeuronId>& motorNeurons,
                                             float currentActivity,
                                             const SimpleWorld& world) {
    // Curiosity-driven action selection
    float explorationThreshold = 0.3f + pImpl->level * 0.7f;  // Higher curiosity = more exploration
    
    if (pImpl->level > explorationThreshold) {
        // High curiosity: random exploration
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> dist(0, 7);
        
        int choice = dist(rng);
        switch (choice) {
            case 0: return ActionType::MoveForward;
            case 1: return ActionType::MoveBackward;
            case 2: return ActionType::TurnLeft;
            case 3: return ActionType::TurnRight;
            case 4: return ActionType::LookLeft;
            case 5: return ActionType::LookRight;
            case 6: return ActionType::Interact;
            default: return ActionType::Wait;
        }
    } else {
        // Lower curiosity: goal-directed action
        // Would use value estimation here
        return ActionType::Wait;  // Placeholder
    }
}

// Novelty detection and encoding
void Curiosity::encodeNovelty(const std::vector<float>& sensoryState,
                            const std::vector<float>& previousState,
                            SimulationStep step) {
    // Novelty detection based on sensory change
    float novelty = calculateSensoryNovelty(sensoryState, previousState);
    
    // Update novelty accumulator for memory consolidation
    pImpl->noveltyAccumulator = 0.9f * pImpl->noveltyAccumulator + 0.1f * novelty;
    
    // Trigger memory encoding if novelty is high
    if (novelty > 0.5f) {
        // Would trigger hippocampal encoding here
        // For now, just record high novelty events
        NLM_LOG_DEBUG("High novelty event detected at step " + std::to_string(step));
    }
}

// Calculate sensory novelty using information theory
float Curiosity::calculateSensoryNovelty(const std::vector<float>& current,
                                       const std::vector<float>& previous) const {
    if (current.size() != previous.size() || current.empty()) return 0.0f;
    
    // Calculate information gain using Kullback-Leibler divergence
    // Simplified: sum of absolute differences normalized by range
    float totalDiff = 0.0f;
    float range = 2.0f;  // Assuming normalized inputs [-1, 1]
    
    for (size_t i = 0; i < current.size(); ++i) {
        totalDiff += std::abs(current[i] - previous[i]);
    }
    
    return totalDiff / (current.size() * range);
}

} // namespace nlm
#include "AdvancedPlasticity.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Random/Random.hpp"
#include "../../plasticity/STDP.hpp"
#include "../../plasticity/Hebbian.hpp"
#include "../../plasticity/StructuralPlasticity.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

// Homeostatic Plasticity Implementation
struct HomeostaticPlasticity::Impl {
    // Core homeostatic parameters
    float targetFiringRate;      // Target firing rate in Hz (default: 10.0)
    float homeostaticGain;       // Learning rate for homeostatic adjustments (default: 0.01)
    float timeConstant;           // Time constant for homeostatic dynamics (default: 100.0 ms)
    float synapticScalingThreshold; // Activity threshold for synaptic scaling (default: 0.5 Hz error)
    float intrinsicPlasticityThreshold; // Activity threshold for intrinsic plasticity (default: 0.2 Hz error)
    
    // State variables
    float currentFiringRate;     // Current network firing rate
    float homeostaticError;      // Difference from target firing rate
    float synapticScalingFactor;  // Global scaling factor for synaptic weights
    float intrinsicPlasticitySignal; // Signal for intrinsic neuron changes
    
    // Memory for homeostatic dynamics
    float smoothedFiringRate;    // Exponential moving average of firing rate
    float stabilityLevel;        // Network stability measure (0-1)
    float adaptationHistory;     // History of homeostatic changes
    
    // Parameters for different homeostatic mechanisms
    float scalingMinFactor;      // Minimum scaling factor (default: 0.5)
    float scalingMaxFactor;      // Maximum scaling factor (default: 2.0)
    float intrinsicMinGain;      // Minimum intrinsic plasticity gain (default: 0.1)
    float intrinsicMaxGain;      // Maximum intrinsic plasticity gain (default: 2.0)
    
    // Timescale parameters
    float scalingTimeConstant;   // Time constant for synaptic scaling (default: 500.0 ms)
    float intrinsicTimeConstant; // Time constant for intrinsic plasticity (default: 1000.0 ms)
    
    Impl() : targetFiringRate(10.0f),
             homeostaticGain(0.01f),
             timeConstant(100.0f),
             synapticScalingThreshold(0.5f),
             intrinsicPlasticityThreshold(0.2f),
             currentFiringRate(0.0f),
             homeostaticError(0.0f),
             synapticScalingFactor(1.0f),
             intrinsicPlasticitySignal(0.0f),
             smoothedFiringRate(0.0f),
             stabilityLevel(1.0f),
             adaptationHistory(0.0f),
             scalingMinFactor(0.5f),
             scalingMaxFactor(2.0f),
             intrinsicMinGain(0.1f),
             intrinsicMaxGain(2.0f),
             scalingTimeConstant(500.0f),
             intrinsicTimeConstant(1000.0f) {}
};

HomeostaticPlasticity::HomeostaticPlasticity() : pImpl(std::make_unique<Impl>()) {}

HomeostaticPlasticity::~HomeostaticPlasticity() = default;

HomeostaticPlasticity::HomeostaticPlasticity(HomeostaticPlasticity&&) noexcept = default;

HomeostaticPlasticity& HomeostaticPlasticity::operator=(HomeostaticPlasticity&&) noexcept = default;

void HomeostaticPlasticity::setTargetFiringRate(float targetHz) {
    pImpl->targetFiringRate = std::max(1.0f, std::min(targetHz, 100.0f));
}

void HomeostaticPlasticity::setHomeostaticGain(float gain) {
    pImpl->homeostaticGain = std::max(0.001f, std::min(gain, 0.1f));
}

void HomeostaticPlasticity::setTimeConstant(float tauMs) {
    pImpl->timeConstant = std::max(10.0f, std::min(tauMs, 1000.0f));
}

void HomeostaticPlasticity::setSynapticScalingThreshold(float threshold) {
    pImpl->synapticScalingThreshold = std::max(0.1f, std::min(threshold, 5.0f));
}

void HomeostaticPlasticity::setIntrinsicPlasticityThreshold(float threshold) {
    pImpl->intrinsicPlasticityThreshold = std::max(0.05f, std::min(threshold, 2.0f));
}

float HomeostaticPlasticity::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

float HomeostaticPlasticity::getCurrentFiringRate() const {
    return pImpl->currentFiringRate;
}

float HomeostaticPlasticity::getHomeostaticError() const {
    return pImpl->homeostaticError;
}

float HomeostaticPlasticity::getSynapticScalingFactor() const {
    return pImpl->synapticScalingFactor;
}

void HomeostaticPlasticity::update(float currentFiringRate, TimestepDuration dt) {
    pImpl->currentFiringRate = currentFiringRate;
    
    // Exponential moving average of firing rate
    float alpha = static_cast<float>(dt) / (pImpl->timeConstant / 1000.0f + dt);
    pImpl->smoothedFiringRate = alpha * currentFiringRate + (1.0f - alpha) * pImpl->smoothedFiringRate;
    
    // Compute homeostatic error
    pImpl->homeostaticError = pImpl->targetFiringRate - pImpl->smoothedFiringRate;
    
    // Update synaptic scaling factor (slower timescale)
    float scalingRate = static_cast<float>(dt) / (pImpl->scalingTimeConstant + dt);
    float targetScalingFactor = 1.0f + pImpl->homeostaticGain * pImpl->homeostaticError;
    targetScalingFactor = std::clamp(targetScalingFactor, pImpl->scalingMinFactor, pImpl->scalingMaxFactor);
    
    pImpl->synapticScalingFactor = scalingRate * targetScalingFactor + (1.0f - scalingRate) * pImpl->synapticScalingFactor;
    
    // Update intrinsic plasticity signal (faster timescale)
    float intrinsicRate = static_cast<float>(dt) / (pImpl->intrinsicTimeConstant + dt);
    float targetIntrinsicSignal = pImpl->homeostaticGain * std::abs(pImpl->homeostaticError);
    targetIntrinsicSignal = std::clamp(targetIntrinsicSignal, pImpl->intrinsicMinGain, pImpl->intrinsicMaxGain);
    
    pImpl->intrinsicPlasticitySignal = intrinsicRate * targetIntrinsicSignal + (1.0f - intrinsicRate) * pImpl->intrinsicPlasticitySignal;
    
    // Update stability level based on homeostatic changes
    pImpl->stabilityLevel = std::exp(-std::abs(pImpl->homeostaticError) * 0.1f);
    
    // Update adaptation history
    pImpl->adaptationHistory = (1.0f - scalingRate) * pImpl->adaptationHistory + scalingRate * std::abs(pImpl->homeostaticError);
}

void HomeostaticPlasticity::applyToSynapse(Synapse* synapse, SynapticWeight currentWeight,
                                         float recentActivity, TimestepDuration dt) {
    if (!synapse) return;
    
    // Apply synaptic scaling
    if (std::abs(pImpl->homeostaticError) > pImpl->synapticScalingThreshold) {
        float scaledWeight = currentWeight * pImpl->synapticScalingFactor;
        scaledWeight = std::clamp(scaledWeight, -1.0f, 1.0f);
        
        // Only apply significant changes
        if (std::abs(scaledWeight - currentWeight) > 1e-4f) {
            synapse->setWeight(scaledWeight);
        }
    }
    
    // Apply intrinsic plasticity based on recent activity
    if (recentActivity > pImpl->intrinsicPlasticityThreshold) {
        // Strong activity increases intrinsic excitability
        float activityFactor = recentActivity / pImpl->targetFiringRate;
        float adjustment = pImpl->intrinsicPlasticitySignal * (activityFactor - 1.0f);
        
        float adjustedWeight = currentWeight + adjustment;
        adjustedWeight = std::clamp(adjustedWeight, -1.0f, 1.0f);
        
        if (std::abs(adjustedWeight - currentWeight) > 1e-4f) {
            synapse->setWeight(adjustedWeight);
        }
    }
}

void HomeostaticPlasticity::applySynapticScaling(const std::vector<Synapse*>& synapses,
                                                 float globalScalingFactor) {
    for (Synapse* synapse : synapses) {
        if (synapse) {
            float newWeight = synapse->getWeight() * globalScalingFactor;
            newWeight = std::clamp(newWeight, -1.0f, 1.0f);
            synapse->setWeight(newWeight);
        }
    }
}

void HomeostaticPlasticity::applyIntrinsicPlasticity(Neuron* neuron, float neuronActivity,
                                                   TimestepDuration dt) {
    if (!neuron) return;
    
    // Adjust neuron intrinsic properties based on activity
    if (neuronActivity > pImpl->intrinsicPlasticityThreshold) {
        // Modify threshold based on activity
        float currentThreshold = neuron->getThreshold();
        float thresholdAdjustment = pImpl->intrinsicPlasticitySignal * 
                                   (1.0f - neuronActivity / pImpl->targetFiringRate) * 5.0f;
        
        float newThreshold = currentThreshold + thresholdAdjustment;
        neuron->setThreshold(newThreshold);
        
        // Adjust other intrinsic properties
        float currentRestingPotential = neuron->getRestingPotential();
        float restingAdjustment = pImpl->intrinsicPlasticitySignal * 
                                 (neuronActivity - pImpl->targetFiringRate) * 0.5f;
        
        neuron->setRestingPotential(currentRestingPotential + restingAdjustment);
    }
}

void HomeostaticPlasticity::reset() {
    pImpl->currentFiringRate = 0.0f;
    pImpl->homeostaticError = 0.0f;
    pImpl->synapticScalingFactor = 1.0f;
    pImpl->intrinsicPlasticitySignal = 0.0f;
    pImpl->smoothedFiringRate = 0.0f;
    pImpl->stabilityLevel = 1.0f;
    pImpl->adaptationHistory = 0.0f;
}

void HomeostaticPlasticity::initializeRandom(RandomGenerator& rng) {
    pImpl->targetFiringRate = rng.uniformReal(5.0f, 20.0f);
    pImpl->homeostaticGain = rng.uniformReal(0.005f, 0.03f);
    pImpl->timeConstant = rng.uniformReal(50.0f, 200.0f);
    pImpl->synapticScalingThreshold = rng.uniformReal(0.3f, 1.0f);
    pImpl->intrinsicPlasticityThreshold = rng.uniformReal(0.1f, 0.5f);
}

const char* HomeostaticPlasticity::getName() const {
    return "HomeostaticPlasticity";
}

// Reward-Modulated Plasticity Implementation
struct RewardModulatedPlasticity::Impl {
    // Core reward-modulated learning parameters
    float learningRate;          // Base learning rate (default: 0.01)
    float predictionErrorGain;   // Gain for prediction error signals (default: 1.0)
    float traceDecayRate;        // Decay rate for eligibility traces (default: 0.995)
    float rewardDelay;            // Delay for reward processing (default: 100.0 ms)
    float alternativeLearningRate; // Learning rate for alternative paths (default: 0.005)
    float temporalDifferenceLambda; // TD(lambda) parameter (default: 0.9)
    
    // Reward prediction and error state
    float expectedReward;        // Expected future reward
    float predictionError;       // Reward prediction error (reward - expected)
    float temporalDifference;    // TD error for sequential learning
    
    // Eligibility traces
    std::vector<float> eligibilityTraces;  // One per synapse (would be managed externally)
    float maxTraceValue;          // Maximum trace value (default: 1.0)
    
    // Neuromodulator state
    float dopamineLevel;         // Dopamine level (default: 0.0)
    float curiosityLevel;        // Curiosity level (default: 0.0)
    float noveltyLevel;           // Novelty detection level (default: 0.0)
    
    // Reward history
    std::vector<float> rewardHistory;     // Recent rewards
    float rewardDecayRate;        // Decay rate for reward history (default: 0.99)
    
    // Alternative learning parameters
    float explorationRate;       // Rate of exploration learning (default: 0.1)
    float consolidationRate;     // Rate of memory consolidation (default: 0.05)
    
    Impl() : learningRate(0.01f),
             predictionErrorGain(1.0f),
             traceDecayRate(0.995f),
             rewardDelay(100.0f),
             alternativeLearningRate(0.005f),
             temporalDifferenceLambda(0.9f),
             expectedReward(0.0f),
             predictionError(0.0f),
             temporalDifference(0.0f),
             maxTraceValue(1.0f),
             dopamineLevel(0.0f),
             curiosityLevel(0.0f),
             noveltyLevel(0.0f),
             rewardDecayRate(0.99f),
             explorationRate(0.1f),
             consolidationRate(0.05f) {}
};

RewardModulatedPlasticity::RewardModulatedPlasticity() : pImpl(std::make_unique<Impl>()) {}

RewardModulatedPlasticity::~RewardModulatedPlasticity() = default;

RewardModulatedPlasticity::RewardModulatedPlasticity(RewardModulatedPlasticity&&) noexcept = default;

RewardModulatedPlasticity& RewardModulatedPlasticity::operator=(RewardModulatedPlasticity&&) noexcept = default;

void RewardModulatedPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.001f, std::min(rate, 0.1f));
}

void RewardModulatedPlasticity::setPredictionErrorGain(float gain) {
    pImpl->predictionErrorGain = std::max(0.1f, std::min(gain, 10.0f));
}

void RewardModulatedPlasticity::setTraceDecayRate(float decayRate) {
    pImpl->traceDecayRate = std::max(0.9f, std::min(decayRate, 0.999f));
}

void RewardModulatedPlasticity::setRewardDelay(float delayMs) {
    pImpl->rewardDelay = std::max(10.0f, std::min(delayMs, 1000.0f));
}

void RewardModulatedPlasticity::setAlternativeLearningRate(float rate) {
    pImpl->alternativeLearningRate = std::max(0.001f, std::min(rate, 0.05f));
}

void RewardModulatedPlasticity::setTemporalDifferenceLambda(float lambda) {
    pImpl->temporalDifferenceLambda = std::max(0.5f, std::min(lambda, 1.0f));
}

float RewardModulatedPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

float RewardModulatedPlasticity::getPredictionErrorGain() const {
    return pImpl->predictionErrorGain;
}

float RewardModulatedPlasticity::getTraceDecayRate() const {
    return pImpl->traceDecayRate;
}

float RewardModulatedPlasticity::getExpectedReward() const {
    return pImpl->expectedReward;
}

float RewardModulatedPlasticity::getPredictionError() const {
    return pImpl->predictionError;
}

void RewardModulatedPlasticity::update(float reward, float predictedReward, TimestepDuration dt) {
    // Store reward in history
    pImpl->rewardHistory.push_back(reward);
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
    }
    
    // Decay reward history
    for (float& r : pImpl->rewardHistory) {
        r *= pImpl->rewardDecayRate;
    }
    
    // Compute expected reward (exponential moving average)
    float alpha = static_cast<float>(dt) / (pImpl->rewardDelay + dt);
    pImpl->expectedReward = alpha * reward + (1.0f - alpha) * pImpl->expectedReward;
    
    // Compute prediction error
    pImpl->predictionError = reward - pImpl->expectedReward;
    
    // Compute temporal difference error (simplified TD learning)
    if (!pImpl->rewardHistory.empty()) {
        float avgReward = std::accumulate(pImpl->rewardHistory.begin(), 
                                         pImpl->rewardHistory.end(), 0.0f) / 
                         pImpl->rewardHistory.size();
        pImpl->temporalDifference = pImpl->predictionError + pImpl->temporalDifferenceLambda * 
                                   (avgReward - pImpl->expectedReward);
    }
    
    // Update neuromodulator levels
    pImpl->dopamineLevel = pImpl->predictionError;
    pImpl->curiosityLevel = std::abs(pImpl->predictionError) * pImpl->predictionErrorGain;
    pImpl->noveltyLevel = pImpl->noveltyLevel * pImpl->traceDecayRate; // Simplified novelty update
}

void RewardModulatedPlasticity::computeEligibilityTrace(Synapse* synapse, float synapticActivity,
                                                        Timestamp spikeTime, TimestepDuration dt) {
    if (!synapse) return;
    
    // This would integrate with synapse's existing eligibility trace system
    // For now, we'll update the synapse's eligibility trace based on activity
    float currentTrace = synapse->getEligibilityTrace();
    float traceIncrement = synapticActivity * pImpl->learningRate;
    
    // Decay and update trace
    currentTrace *= pImpl->traceDecayRate;
    currentTrace = std::min(currentTrace + traceIncrement, pImpl->maxTraceValue);
    
    synapse->setEligibilityTrace(currentTrace);
}

void RewardModulatedPlasticity::applyToSynapse(Synapse* synapse, float neuromodulatorLevel,
                                              TimestepDuration dt) {
    if (!synapse) return;
    
    float currentTrace = synapse->getEligibilityTrace();
    if (std::abs(currentTrace) > 1e-6f) {
        // Apply reward-modulated weight change
        float delta = currentTrace * neuromodulatorLevel * pImpl->learningRate;
        synapse->addToWeight(delta);
        
        // Decay eligibility trace
        synapse->decayEligibilityTrace(0.1f);
    }
}

void RewardModulatedPlasticity::addPredictionError(Synapse* synapse, float predictionError,
                                                  TimestepDuration dt) {
    if (!synapse) return;
    
    // Add prediction error to synapse's weight via eligibility trace
    float currentTrace = synapse->getEligibilityTrace();
    float delta = predictionError * pImpl->predictionErrorGain * pImpl->learningRate;
    
    synapse->setEligibilityTrace(currentTrace + delta);
}

float RewardModulatedPlasticity::computeAlternativeLearningRate(float novelty, float curiosity,
                                                                TimestepDuration dt) {
    // Alternative learning rate based on exploration signals
    float alternativeRate = pImpl->alternativeLearningRate;
    
    if (curiosity > 0.5f) {
        // High curiosity increases exploration learning rate
        alternativeRate *= (1.0f + curiosity * 2.0f);
    }
    
    if (novelty > 0.7f) {
        // High novelty increases learning rate
        alternativeRate *= (1.0f + novelty);
    }
    
    return std::clamp(alternativeRate, 0.001f, 0.1f);
}

void RewardModulatedPlasticity::reset() {
    pImpl->expectedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->temporalDifference = 0.0f;
    pImpl->dopamineLevel = 0.0f;
    pImpl->curiosityLevel = 0.0f;
    pImpl->noveltyLevel = 0.0f;
    pImpl->rewardHistory.clear();
}

void RewardModulatedPlasticity::initializeRandom(RandomGenerator& rng) {
    pImpl->learningRate = rng.uniformReal(0.005f, 0.02f);
    pImpl->predictionErrorGain = rng.uniformReal(0.5f, 2.0f);
    pImpl->traceDecayRate = rng.uniformReal(0.98f, 0.999f);
    pImpl->rewardDelay = rng.uniformReal(50.0f, 200.0f);
    pImpl->alternativeLearningRate = rng.uniformReal(0.001f, 0.01f);
    pImpl->temporalDifferenceLambda = rng.uniformReal(0.7f, 1.0f);
}

const char* RewardModulatedPlasticity::getName() const {
    return "RewardModulatedPlasticity";
}

// Meta-Plasticity Implementation
struct MetaPlasticity::Impl {
    // Core metaplastic parameters
    float plasticityLearningRate;   // Learning rate for plasticity itself (default: 0.001)
    float stabilityFactor;           // Factor promoting stability (default: 0.8)
    float complexityTarget;          // Target network complexity (default: 0.5)
    float memoryAllocationRate;      // Rate of memory allocation (default: 0.01)
    float consolidationStrength;     // Strength of memory consolidation (default: 0.7)
    
    // Metaplastic state
    float plasticityLearningProgress; // Progress in adapting plasticity rules
    float networkComplexity;         // Current network complexity measure
    float memoryUtilization;         // Memory usage (0-1)
    float stabilityScore;             // Current network stability
    
    // Metaplastic history
    float plasticityAdaptation;      // History of plasticity adaptations
    float complexityHistory;         // History of complexity changes
    float memoryHistory;             // History of memory allocation
    
    // Parameters for different metaplastic mechanisms
    float consolidationThreshold;    // Threshold for memory consolidation (default: 0.6)
    float noveltyThreshold;          // Threshold for new memory allocation (default: 0.8)
    float interferenceFactor;        // Factor for managing interference between memories (default: 0.3)
    
    Impl() : plasticityLearningRate(0.001f),
             stabilityFactor(0.8f),
             complexityTarget(0.5f),
             memoryAllocationRate(0.01f),
             consolidationStrength(0.7f),
             plasticityLearningProgress(0.0f),
             networkComplexity(0.0f),
             memoryUtilization(0.0f),
             stabilityScore(1.0f),
             plasticityAdaptation(0.0f),
             complexityHistory(0.0f),
             memoryHistory(0.0f),
             consolidationThreshold(0.6f),
             noveltyThreshold(0.8f),
             interferenceFactor(0.3f) {}
};

MetaPlasticity::MetaPlasticity() : pImpl(std::make_unique<Impl>()) {}

MetaPlasticity::~MetaPlasticity() = default;

MetaPlasticity::MetaPlasticity(MetaPlasticity&&) noexcept = default;

MetaPlasticity& MetaPlasticity::operator=(MetaPlasticity&&) noexcept = default;

void MetaPlasticity::setPlasticityLearningRate(float rate) {
    pImpl->plasticityLearningRate = std::max(0.0001f, std::min(rate, 0.01f));
}

void MetaPlasticity::setStabilityFactor(float factor) {
    pImpl->stabilityFactor = std::max(0.5f, std::min(factor, 0.95f));
}

void MetaPlasticity::setComplexityTarget(float target) {
    pImpl->complexityTarget = std::max(0.1f, std::min(target, 1.0f));
}

void MetaPlasticity::setMemoryAllocationRate(float rate) {
    pImpl->memoryAllocationRate = std::max(0.001f, std::min(rate, 0.1f));
}

void MetaPlasticity::setConsolidationStrength(float strength) {
    pImpl->consolidationStrength = std::max(0.3f, std::min(strength, 1.0f));
}

float MetaPlasticity::getPlasticityLearningRate() const {
    return pImpl->plasticityLearningRate;
}

float MetaPlasticity::getStabilityFactor() const {
    return pImpl->stabilityFactor;
}\nfloat MetaPlasticity::getComplexityScore() const {
    return pImpl->networkComplexity;
}

float MetaPlasticity::getMemoryUtilization() const {
    return pImpl->memoryUtilization;
}

void MetaPlasticity::update(float learningProgress, float networkComplexity,
                            TimestepDuration dt) {
    pImpl->networkComplexity = std::clamp(networkComplexity, 0.0f, 1.0f);
    
    // Update plasticity learning progress
    float progressRate = static_cast<float>(dt) / (100.0f + dt); // 100ms timescale
    pImpl->plasticityLearningProgress = progressRate * learningProgress + 
                                        (1.0f - progressRate) * pImpl->plasticityLearningProgress;
    
    // Update stability score based on complexity and progress
    float complexityError = std::abs(pImpl->networkComplexity - pImpl->complexityTarget);
    pImpl->stabilityScore = std::exp(-complexityError * 5.0f) * 
                           (pImpl->stabilityFactor + pImpl->plasticityLearningProgress * 0.3f);
    
    // Update memory utilization based on consolidation
    float consolidationRate = pImpl->consolidationStrength * pImpl->plasticityLearningProgress;
    pImpl->memoryUtilization = std::min(1.0f, pImpl->memoryUtilization + consolidationRate * dt);
    
    // Update plasticity adaptation based on stability requirements
    pImpl->plasticityAdaptation = pImpl->stabilityScore * pImpl->plasticityLearningRate * dt;
    
    // Update complexity history
    pImpl->complexityHistory = pImpl->networkComplexity;
    pImpl->memoryHistory = pImpl->memoryUtilization;
}

void MetaPlasticity::adaptPlasticityRules(STDP* stdp, Hebbian* hebbian,
                                         Synapse* synapse, TimestepDuration dt) {
    if (!stdp || !hebbian || !synapse) return;
    
    // Adjust plasticity parameters based on metaplastic state
    float stabilityAdjustment = 1.0f - pImpl->stabilityScore;
    
    // Adjust STDP parameters for stability
    float currentLTP = stdp->getLTPWeight();
    float currentLTD = stdp->getLTDWeight();
    
    float ltpAdjustment = stabilityAdjustment * pImpl->plasticityAdaptation * 0.1f;
    float ltdAdjustment = stabilityAdjustment * pImpl->plasticityAdaptation * 0.1f;
    
    stdp->setLTPWeight(currentLTP + ltpAdjustment);
    stdp->setLTDWeight(currentLTD + ltdAdjustment);
    
    // Adjust Hebbian learning rate
    float currentHebbianRate = hebbian->getLearningRate();
    float hebbianAdjustment = pImpl->plasticityLearningProgress * 0.05f;
    
    hebbian->setLearningRate(currentHebbianRate + hebbianAdjustment);
}

void MetaPlasticity::consolidateMemory(Synapse* synapse, float consolidationSignal,
                                      TimestepDuration dt) {
    if (!synapse) return;
    
    // Apply soft consolidation to synaptic weights
    float currentWeight = synapse->getWeight();
    float consolidationAmount = consolidationSignal * pImpl->consolidationStrength * dt;
    
    // Apply consolidation that makes weights more stable
    float stabilizedWeight = currentWeight * (1.0f - pImpl->stabilityFactor) +
                             (currentWeight + consolidationAmount) * pImpl->stabilityFactor;
    
    stabilizedWeight = std::clamp(stabilizedWeight, -1.0f, 1.0f);
    
    if (std::abs(stabilizedWeight - currentWeight) > 1e-4f) {
        synapse->setWeight(stabilizedWeight);
    }
}

void MetaPlasticity::allocateNewMemory(Synapse* synapse, float novelty,
                                       TimestepDuration dt) {
    if (!synapse) return;
    
    if (novelty > pImpl->noveltyThreshold) {
        // Allocate new memory based on novelty
        float allocationAmount = novelty * pImpl->memoryAllocationRate * dt;
        
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + allocationAmount;
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        
        if (std::abs(newWeight - currentWeight) > 1e-4f) {
            synapse->setWeight(newWeight);
        }
    }
}

float MetaPlasticity::computeSynapticComplexity(const std::vector<Synapse*>& synapses) {
    float totalComplexity = 0.0f;
    
    for (Synapse* synapse : synapses) {
        if (synapse) {
            float weightComplexity = std::abs(synapse->getWeight());
            float historyComplexity = synapse->getPreSpikeHistory().size() * 0.1f +
                                     synapse->getPostSpikeHistory().size() * 0.1f;
            float plasticityComplexity = std::abs(synapse->getEligibilityTrace()) * 0.5f;
            
            totalComplexity += weightComplexity + historyComplexity + plasticityComplexity;
        }
    }
    
    // Normalize by number of synapses
    return synapses.empty() ? 0.0f : totalComplexity / synapses.size();
}

void MetaPlasticity::reset() {
    pImpl->plasticityLearningProgress = 0.0f;
    pImpl->networkComplexity = 0.0f;
    pImpl->memoryUtilization = 0.0f;
    pImpl->stabilityScore = 1.0f;
    pImpl->plasticityAdaptation = 0.0f;
    pImpl->complexityHistory = 0.0f;
    pImpl->memoryHistory = 0.0f;
}

void MetaPlasticity::initializeRandom(RandomGenerator& rng) {
    pImpl->plasticityLearningRate = rng.uniformReal(0.0001f, 0.005f);
    pImpl->stabilityFactor = rng.uniformReal(0.6f, 0.9f);
    pImpl->complexityTarget = rng.uniformReal(0.3f, 0.7f);
    pImpl->memoryAllocationRate = rng.uniformReal(0.005f, 0.02f);
    pImpl->consolidationStrength = rng.uniformReal(0.5f, 0.9f);
}

const char* MetaPlasticity::getName() const {
    return "MetaPlasticity";
}

// Synaptic Scaling Implementation
struct SynapticScaling::Impl {
    // Core synaptic scaling parameters
    float scalingGain;                // Gain for synaptic scaling (default: 1.0)
    float targetExcitationInhibitionRatio; // Target E/I ratio (default: 1.0)
    bool structuralPlasticityEnabled;  // Enable structural plasticity (default: true)
    float optimizationInterval;      // Time between optimizations (default: 1000.0 ms)
    float networkStabilityThreshold;  // Threshold for network instability (default: 0.7)
    
    // Scaling state
    float currentEIRatio;           // Current E/I ratio
    float networkStability;         // Network stability measure (0-1)
    float optimizationProgress;     // Progress of network optimization
    float scalingAccumulator;       // Accumulator for scaling adjustments
    
    // Structural plasticity state
    float synaptogenesisRate;        // Rate of new synapse formation (default: 0.001)
    float pruningRate;               // Rate of synapse elimination (default: 0.0001)
    
    // Optimization state
    float targetComplexity;          // Target network complexity (default: 0.6)
    float currentComplexity;         // Current network complexity
    float convergenceScore;          // How close to target complexity
    
    Impl() : scalingGain(1.0f),
             targetExcitationInhibitionRatio(1.0f),
             structuralPlasticityEnabled(true),
             optimizationInterval(1000.0f),
             networkStabilityThreshold(0.7f),
             currentEIRatio(1.0f),
             networkStability(1.0f),
             optimizationProgress(0.0f),
             scalingAccumulator(0.0f),
             synaptogenesisRate(0.001f),
             pruningRate(0.0001f),
             targetComplexity(0.6f),
             currentComplexity(0.0f),
             convergenceScore(0.0f) {}
};

SynapticScaling::SynapticScaling() : pImpl(std::make_unique<Impl>()) {}

SynapticScaling::~SynapticScaling() = default;

SynapticScaling::SynapticScaling(SynapticScaling&&) noexcept = default;

SynapticScaling& SynapticScaling::operator=(SynapticScaling&&) noexcept = default;

void SynapticScaling::setScalingGain(float gain) {
    pImpl->scalingGain = std::max(0.5f, std::min(gain, 2.0f));
}

void SynapticScaling::setTargetExcitationInhibitionRatio(float ratio) {
    pImpl->targetExcitationInhibitionRatio = std::max(0.1f, std::min(ratio, 10.0f));
}

void SynapticScaling::setStructuralPlasticityEnabled(bool enabled) {
    pImpl->structuralPlasticityEnabled = enabled;
}

void SynapticScaling::setOptimizationInterval(float intervalMs) {
    pImpl->optimizationInterval = std::max(100.0f, std::min(intervalMs, 10000.0f));
}

void SynapticScaling::setNetworkStabilityThreshold(float threshold) {
    pImpl->networkStabilityThreshold = std::max(0.5f, std::min(threshold, 1.0f));
}

float SynapticScaling::getScalingGain() const {
    return pImpl->scalingGain;
}

float SynapticScaling::getCurrentEIRatio() const {
    return pImpl->currentEIRatio;
}

float SynapticScaling::getNetworkStability() const {
    return pImpl->networkStability;
}

float SynapticScaling::getOptimizationProgress() const {
    return pImpl->optimizationProgress;
}

void SynapticScaling::update(float currentEIRatio, const std::vector<Synapse*>& synapses,
                            TimestepDuration dt) {
    pImpl->currentEIRatio = currentEIRatio;
    
    // Compute network stability based on E/I ratio
    float eiError = std::abs(pImpl->currentEIRatio - pImpl->targetExcitationInhibitionRatio);
    pImpl->networkStability = std::exp(-eiError * 5.0f);
    
    // Check if scaling is needed
    if (pImpl->networkStability < pImpl->networkStabilityThreshold) {
        // Apply scaling to restore stability
        float scalingNeeded = (pImpl->targetExcitationInhibitionRatio - pImpl->currentEIRatio) /
                             pImpl->currentEIRatio;
        scalingNeeded = std::clamp(scalingNeeded * pImpl->scalingGain, -0.1f, 0.1f);
        
        pImpl->scalingAccumulator += scalingNeeded * dt;
        
        // Update optimization progress
        pImpl->optimizationProgress = std::min(1.0f, pImpl->optimizationProgress + 
                                              std::abs(scalingNeeded) * 10.0f * dt);
    } else {
        pImpl->scalingAccumulator = 0.0f;
    }
    
    // Update network complexity
    pImpl->currentComplexity = computeNetworkComplexity(synapses);
    
    // Check convergence to target complexity
    pImpl->convergenceScore = 1.0f - std::abs(pImpl->currentComplexity - pImpl->targetComplexity);
}

void SynapticScaling::applyGlobalScaling(const std::vector<Synapse*>& synapses,
                                        float globalScalingFactor, TimestepDuration dt) {
    for (Synapse* synapse : synapses) {
        if (synapse) {
            float currentWeight = synapse->getWeight();
            float scaledWeight = currentWeight * globalScalingFactor;
            
            // Apply gradual scaling to prevent instability
            float maxChange = 0.1f * dt; // Maximum change per timestep
            float weightChange = scaledWeight - currentWeight;
            weightChange = std::clamp(weightChange, -maxChange, maxChange);
            
            scaledWeight = currentWeight + weightChange;
            scaledWeight = std::clamp(scaledWeight, -1.0f, 1.0f);
            
            synapse->setWeight(scaledWeight);
        }
    }
}

void SynapticScaling::applyStructuralPlasticity(StructuralPlasticity* sp,
                                               float networkActivity, TimestepDuration dt) {
    if (!sp) return;
    
    // Adjust structural plasticity rates based on network stability
    if (pImpl->networkStability < pImpl->networkStabilityThreshold) {
        // Increase synaptogenesis when network is unstable
        float synaptogenesisMultiplier = 1.0f + (1.0f - pImpl->networkStability) * 5.0f;
        sp->setSynaptogenesisRate(pImpl->synaptogenesisRate * synaptogenesisMultiplier);
        
        // Decrease pruning when network is unstable
        float pruningMultiplier = std::max(0.1f, 1.0f - (1.0f - pImpl->networkStability) * 5.0f);
        sp->setPruningRate(pImpl->pruningRate * pruningMultiplier);
    } else {
        // Restore normal rates
        sp->setSynaptogenesisRate(pImpl->synaptogenesisRate);
        sp->setPruningRate(pImpl->pruningRate);
    }
}

void SynapticScaling::reorganizeConnections(const std::vector<Synapse*>& synapses,
                                           const Brain* brain, TimestepDuration dt) {
    // Reorganize synaptic connections based on activity patterns
    // This is a simplified version - would need more sophisticated algorithms
    
    float stabilityThreshold = pImpl->networkStabilityThreshold;
    
    if (pImpl->networkStability < stabilityThreshold) {
        // High activity correlates -> strengthen
        // Low activity correlates -> weaken
        for (Synapse* synapse : synapses) {
            if (synapse) {
                // This would require access to spike history and correlation analysis
                // For now, just apply gentle scaling based on current weight
                float currentWeight = synapse->getWeight();
                float reorganizationFactor = 1.0f + (currentWeight * 0.1f);
                reorganizationFactor = std::clamp(reorganizationFactor, 0.9f, 1.1f);
                
                float newWeight = currentWeight * reorganizationFactor;
                newWeight = std::clamp(newWeight, -1.0f, 1.0f);
                
                if (std::abs(newWeight - currentWeight) > 1e-4f) {
                    synapse->setWeight(newWeight);
                }
            }
        }
    }
}

void SynapticScaling::optimizeNetworkTopology(const std::vector<Synapse*>& synapses,
                                              TimestepDuration dt) {
    // Optimize network topology based on convergence score
    if (pImpl->convergenceScore > 0.8f) {
        // Network is well-converged, apply subtle optimizations
        for (Synapse* synapse : synapses) {
            if (synapse) {
                float currentWeight = synapse->getWeight();
                
                // Small perturbations based on convergence
                float perturbation = (1.0f - pImpl->convergenceScore) * 0.01f * dt;
                float sign = (rng_.uniformReal(0.0f, 1.0f) < 0.5f) ? 1.0f : -1.0f;
                
                float newWeight = currentWeight + sign * perturbation;
                newWeight = std::clamp(newWeight, -1.0f, 1.0f);
                
                if (std::abs(newWeight - currentWeight) > 1e-4f) {
                    synapse->setWeight(newWeight);
                }
            }
        }
    }
}

float SynapticScaling::computeNetworkComplexity(const std::vector<Synapse*>& synapses) {
    float totalComplexity = 0.0f;
    
    for (Synapse* synapse : synapses) {
        if (synapse) {
            // Synaptic complexity based on weight, history, and plasticity
            float weightComplexity = std::abs(synapse->getWeight());
            float historyComplexity = synapse->getPreSpikeHistory().size() * 0.1f +
                                     synapse->getPostSpikeHistory().size() * 0.1f;
            float plasticityComplexity = std::abs(synapse->getEligibilityTrace()) * 0.5f;
            
            totalComplexity += weightComplexity + historyComplexity + plasticityComplexity;
        }
    }
    
    // Normalize by number of synapses and scale appropriately
    return synapses.empty() ? 0.0f : std::min(1.0f, totalComplexity / synapses.size() * 2.0f);
}

void SynapticScaling::reset() {
    pImpl->currentEIRatio = 1.0f;
    pImpl->networkStability = 1.0f;
    pImpl->optimizationProgress = 0.0f;
    pImpl->scalingAccumulator = 0.0f;
    pImpl->currentComplexity = 0.0f;
    pImpl->convergenceScore = 0.0f;
}

void SynapticScaling::initializeRandom(RandomGenerator& rng) {
    pImpl->scalingGain = rng.uniformReal(0.8f, 1.5f);
    pImpl->targetExcitationInhibitionRatio = rng.uniformReal(0.8f, 2.0f);
    pImpl->structuralPlasticityEnabled = true;
    pImpl->optimizationInterval = rng.uniformReal(500.0f, 2000.0f);
    pImpl->networkStabilityThreshold = rng.uniformReal(0.6f, 0.8f);
    pImpl->synaptogenesisRate = rng.uniformReal(0.0005f, 0.002f);
    pImpl->pruningRate = rng.uniformReal(0.00005f, 0.0002f);
}

const char* SynapticScaling::getName() const {
    return "SynapticScaling";
}

// Advanced Plasticity Manager Implementation
struct AdvancedPlasticity::Impl {
    // Core advanced plasticity parameters
    bool homeostaticPlasticityEnabled;     // Enable homeostatic plasticity (default: true)
    bool rewardModulatedPlasticityEnabled; // Enable reward-modulated plasticity (default: true)
    bool metaPlasticityEnabled;             // Enable meta-plasticity (default: true)
    bool synapticScalingEnabled;            // Enable synaptic scaling (default: true)
    
    // Advanced plasticity components
    HomeostaticPlasticity homeostatic;
    RewardModulatedPlasticity rewardModulated;
    MetaPlasticity metaPlasticity;
    SynapticScaling synapticScaling;
    
    // Network state tracking
    float networkStability;                 // Current network stability
    float learningEfficiency;               // Measure of learning progress
    float adaptabilityScore;                // Ability to adapt to changes
    
    // Network statistics
    float avgFiringRate;                    // Average firing rate across neurons
    float totalSynapseCount;                 // Total number of synapses
    float plasticityActivity;               // Overall plasticity activity
    
    // Integration state
    bool systemsIntegrated;                 // Whether all systems are properly integrated
    float integrationQuality;               // Quality of system integration (0-1)
    
    Impl() : homeostaticPlasticityEnabled(true),
             rewardModulatedPlasticityEnabled(true),
             metaPlasticityEnabled(true),
             synapticScalingEnabled(true),
             networkStability(1.0f),
             learningEfficiency(0.0f),
             adaptabilityScore(0.0f),
             avgFiringRate(0.0f),
             totalSynapseCount(0.0f),
             plasticityActivity(0.0f),
             systemsIntegrated(true),
             integrationQuality(1.0f) {}
};

AdvancedPlasticity::AdvancedPlasticity() : pImpl(std::make_unique<Impl>()) {}

AdvancedPlasticity::~AdvancedPlasticity() = default;

AdvancedPlasticity::AdvancedPlasticity(AdvancedPlasticity&&) noexcept = default;

AdvancedPlasticity& AdvancedPlasticity::operator=(AdvancedPlasticity&&) noexcept = default;

void AdvancedPlasticity::enableHomeostaticPlasticity(bool enabled) {
    pImpl->homeostaticPlasticityEnabled = enabled;
}

void AdvancedPlasticity::enableRewardModulatedPlasticity(bool enabled) {
    pImpl->rewardModulatedPlasticityEnabled = enabled;
}

void AdvancedPlasticity::enableMetaPlasticity(bool enabled) {
    pImpl->metaPlasticityEnabled = enabled;
}

void AdvancedPlasticity::enableSynapticScaling(bool enabled) {
    pImpl->synapticScalingEnabled = enabled;
}

void AdvancedPlasticity::setHomeostaticTarget(float targetHz) {
    pImpl->homeostatic.enableHomeostaticPlasticity(true);
    pImpl->homeostatic.setTargetFiringRate(targetHz);
}

void AdvancedPlasticity::setRewardLearningRate(float rate) {
    pImpl->rewardModulated.enableRewardModulatedPlasticity(true);
    pImpl->rewardModulated.setLearningRate(rate);
}

void AdvancedPlasticity::setMetaPlasticityLearningRate(float rate) {
    pImpl->metaPlasticity.enableMetaPlasticity(true);
    pImpl->metaPlasticity.setPlasticityLearningRate(rate);
}

void AdvancedPlasticity::setSynapticScalingGain(float gain) {
    pImpl->synapticScaling.enableSynapticScaling(true);
    pImpl->synapticScaling.setScalingGain(gain);
}

void AdvancedPlasticity::update(float currentFiringRate, float reward, float predictionError,
                               float novelty, float curiosity, TimestepDuration dt) {
    // Update each plasticity system
    if (pImpl->homeostaticPlasticityEnabled) {
        pImpl->homeostatic.update(currentFiringRate, dt);
    }
    
    if (pImpl->rewardModulatedPlasticityEnabled) {
        pImpl->rewardModulated.update(reward, currentFiringRate, dt);
    }
    
    if (pImpl->metaPlasticityEnabled) {
        pImpl->metaPlasticity.update(pImpl->homeostatic.getPlasticityLearningProgress(),
                                   pImpl->synapticScaling.getCurrentComplexity(), dt);
    }
    
    if (pImpl->synapticScalingEnabled) {
        pImpl->synapticScaling.update(pImpl->homeostatic.getCurrentFiringRate(),
                                    std::vector<Synapse*>(), dt); // Would need synapse list
    }
    
    // Update network statistics and quality measures
    pImpl->avgFiringRate = currentFiringRate;
    pImpl->learningEfficiency = calculateLearningEfficiency();
    pImpl->adaptabilityScore = calculateAdaptabilityScore();
    
    // Check system integration
    pImpl->systemsIntegrated = checkSystemIntegration();
    pImpl->integrationQuality = calculateIntegrationQuality();
}

void AdvancedPlasticity::applyToSynapse(Synapse* synapse, float neuralActivity,
                                       float neuromodulatorLevel, float rewardError,
                                       TimestepDuration dt) {
    if (!synapse) return;
    
    // Apply homeostatic plasticity
    if (pImpl->homeostaticPlasticityEnabled) {
        pImpl->homeostatic.applyToSynapse(synapse, synapse->getWeight(),
                                         neuralActivity, dt);
    }
    
    // Apply reward-modulated plasticity
    if (pImpl->rewardModulatedPlasticityEnabled) {
        pImpl->rewardModulated.applyToSynapse(synapse, neuromodulatorLevel, dt);
        pImpl->rewardModulated.addPredictionError(synapse, rewardError, dt);
    }
    
    // Apply meta-plasticity
    if (pImpl->metaPlasticityEnabled) {
        // Would need access to STDP and Hebbian systems
        // pImpl->metaPlasticity.adaptPlasticityRules(stdp, hebbian, synapse, dt);
    }
    
    // Apply synaptic scaling
    if (pImpl->synapticScalingEnabled) {
        // This would be applied globally, not per synapse
    }
}

void AdvancedPlasticity::applyToBrain(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Apply to all synapses in the brain
    for (const auto& region : brain->getRegions()) {
        for (const auto& synapse : region->getSynapses()) {
            applyToSynapse(synapse.get(), 0.0f, 0.0f, 0.0f, dt);
        }
    }
}

bool AdvancedPlasticity::isHomeostaticPlasticityEnabled() const {
    return pImpl->homeostaticPlasticityEnabled;
}

bool AdvancedPlasticity::isRewardModulatedPlasticityEnabled() const {
    return pImpl->rewardModulatedPlasticityEnabled;
}

bool AdvancedPlasticity::isMetaPlasticityEnabled() const {
    return pImpl->metaPlasticityEnabled;
}

bool AdvancedPlasticity::isSynapticScalingEnabled() const {
    return pImpl->synapticScalingEnabled;
}

float AdvancedPlasticity::getNetworkStability() const {
    return pImpl->networkStability;
}

float AdvancedPlasticity::getLearningEfficiency() const {
    return pImpl->learningEfficiency;
}

float AdvancedPlasticity::getAdaptabilityScore() const {
    return pImpl->adaptabilityScore;
}

void AdvancedPlasticity::reset() {
    if (pImpl->homeostaticPlasticityEnabled) pImpl->homeostatic.reset();
    if (pImpl->rewardModulatedPlasticityEnabled) pImpl->rewardModulated.reset();
    if (pImpl->metaPlasticityEnabled) pImpl->metaPlasticity.reset();
    if (pImpl->synapticScalingEnabled) pImpl->synapticScaling.reset();
    
    pImpl->networkStability = 1.0f;
    pImpl->learningEfficiency = 0.0f;
    pImpl->adaptabilityScore = 0.0f;
    pImpl->systemsIntegrated = true;
    pImpl->integrationQuality = 1.0f;
}

void AdvancedPlasticity::initializeRandom(RandomGenerator& rng) {
    if (pImpl->homeostaticPlasticityEnabled) pImpl->homeostatic.initializeRandom(rng);
    if (pImpl->rewardModulatedPlasticityEnabled) pImpl->rewardModulated.initializeRandom(rng);
    if (pImpl->metaPlasticityEnabled) pImpl->metaPlasticity.initializeRandom(rng);
    if (pImpl->synapticScalingEnabled) pImpl->synapticScaling.initializeRandom(rng);
}

const char* AdvancedPlasticity::getName() const {
    return "AdvancedPlasticity";
}

// Helper methods (would need access to rng and other systems)
float AdvancedPlasticity::calculateLearningEfficiency() const {
    // Calculate based on plasticity activity and convergence
    return pImpl->plasticityActivity * pImpl->integrationQuality;
}

float AdvancedPlasticity::calculateAdaptabilityScore() const {
    // Calculate based on response to changes and plasticity dynamics
    return pImpl->networkStability * pImpl->learningEfficiency;
}

bool AdvancedPlasticity::checkSystemIntegration() const {
    // Check if all systems are properly integrated
    return pImpl->homeostaticPlasticityEnabled &&
           pImpl->rewardModulatedPlasticityEnabled &&
           pImpl->metaPlasticityEnabled &&
           pImpl->synapticScalingEnabled;
}

float AdvancedPlasticity::calculateIntegrationQuality() const {
    // Calculate overall integration quality
    return (pImpl->homeostaticPlasticityEnabled ? 0.25f : 0.0f) +
           (pImpl->rewardModulatedPlasticityEnabled ? 0.25f : 0.0f) +
           (pImpl->metaPlasticityEnabled ? 0.25f : 0.0f) +
           (pImpl->synapticScalingEnabled ? 0.25f : 0.0f);
}

} // namespace nlm

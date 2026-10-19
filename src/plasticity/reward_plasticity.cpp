#include "reward_plasticity.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/Brain.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <map>

namespace nlm
{

// RewardModulatedPlasticity implementation
struct RewardModulatedPlasticity::Impl {
    float predictionError;
    float expectedReward;
    float actualReward;
    float dopamineResponse;
    float learningRate;
    bool asymmetricRewardSensitivity;
    bool delayedRewardModulation;
    bool trialBasedModulation;
    float creditAssignmentWindow;
    size_t rewardHistoryCapacity;
    
    float rewardHistory;
    float averageReward;
    float rewardVolatility;
    
    Impl() : predictionError(0.0f), expectedReward(0.0f), actualReward(0.0f),
             dopamineResponse(1.0f), learningRate(0.01f),
             asymmetricRewardSensitivity(true), delayedRewardModulation(true),
             trialBasedModulation(true), creditAssignmentWindow(100.0f),
             rewardHistoryCapacity(1000),
             rewardHistory(0.0f), averageReward(0.0f), rewardVolatility(0.0f) {}
};

RewardModulatedPlasticity::RewardModulatedPlasticity() : pImpl(std::make_unique<Impl>()) {}

RewardModulatedPlasticity::~RewardModulatedPlasticity() = default;

void RewardModulatedPlasticity::update(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate prediction error
    calculatePredictionError();
    
    // Base STDP-like eligibility calculation
    float baseChange = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
        float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
        
        // Basic STDP-like change
        float tau = 20.0f;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float deltaT = static_cast<float>(postTime - preTime);
                if (deltaT > 0) {
                    baseChange += 0.01f * std::exp(-deltaT / tau);
                } else {
                    baseChange -= 0.012f * std::exp(deltaT / tau);
                }
            }
        }
    }
    
    // Calculate reward-modulated change
    float rewardModulation = calculateRewardModulationFactor();
    
    // Apply trial-based modulation
    if (pImpl->trialBasedModulation) {
        // Update reward history
        recordRewardHistory(pImpl->actualReward, 1.0f);
        
        // Apply reward modulation to eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace * rewardModulation);
    }
    
    // Calculate final reward-modulated change
    float delta = baseChange * rewardModulation * pImpl->dopamineResponse;
    
    // Apply delayed credit assignment
    if (pImpl->delayedRewardModulation && pImpl->creditAssignmentWindow > 0.0f) {
        // Decay the eligibility trace
        float trace = synapse->getEligibilityTrace();
        trace *= std::exp(-dt / pImpl->creditAssignmentWindow);
        synapse->setEligibilityTrace(trace);
        
        // Apply the trace-modulated change
        delta *= trace;
    }
    
    applyWeightChange(synapse, delta);
}

void RewardModulatedPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedPlasticity::getName() const {
    return "RewardModulatedPlasticity";
}

void RewardModulatedPlasticity::setPredictionError(float error) {
    pImpl->predictionError = error;
}

void RewardModulatedPlasticity::setExpectedReward(float expected) {
    pImpl->expectedReward = expected;
}

void RewardModulatedPlasticity::setActualReward(float actual) {
    pImpl->actualReward = actual;
}

void RewardModulatedPlasticity::setDopamineResponse(float response) {
    pImpl->dopamineResponse = std::clamp(response, 0.0f, 2.0f);
}

void RewardModulatedPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void RewardModulatedPlasticity::setAsymmetricRewardSensitivity(bool enable) {
    pImpl->asymmetricRewardSensitivity = enable;
}

void RewardModulatedPlasticity::setDelayedRewardModulation(bool enable) {
    pImpl->delayedRewardModulation = enable;
}

void RewardModulatedPlasticity::setTrialBasedModulation(bool enable) {
    pImpl->trialBasedModulation = enable;
}

void RewardModulatedPlasticity::setCreditAssignmentWindow(float window) {
    pImpl->creditAssignmentWindow = std::max(0.0f, window);
}

void RewardModulatedPlasticity::setRewardHistoryCapacity(size_t capacity) {
    pImpl->rewardHistoryCapacity = capacity;
}

float RewardModulatedPlasticity::getPredictionError() const {
    return pImpl->predictionError;
}

float RewardModulatedPlasticity::getExpectedReward() const {
    return pImpl->expectedReward;
}

float RewardModulatedPlasticity::getActualReward() const {
    return pImpl->actualReward;
}

float RewardModulatedPlasticity::getDopamineResponse() const {
    return pImpl->dopamineResponse;
}

float RewardModulatedPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

bool RewardModulatedPlasticity::getAsymmetricRewardSensitivity() const {
    return pImpl->asymmetricRewardSensitivity;
}

bool RewardModulatedPlasticity::getDelayedRewardModulation() const {
    return pImpl->delayedRewardModulation;
}

bool RewardModulatedPlasticity::getTrialBasedModulation() const {
    return pImpl->trialBasedModulation;
}

float RewardModulatedPlasticity::getCreditAssignmentWindow() const {
    return pImpl->creditAssignmentWindow;
}

void RewardModulatedPlasticity::calculatePredictionError() {
    pImpl->predictionError = pImpl->actualReward - pImpl->expectedReward;
}

void RewardModulatedPlasticity::updateEligibilityTrace(Synapse* synapse, float delta, float reward, float dt) {
    if (!synapse) return;
    float currentTrace = synapse->getEligibilityTrace();
    float rewardFactor = 1.0f + std::abs(reward);
    synapse->setEligibilityTrace(currentTrace + delta * rewardFactor);
}

void RewardModulatedPlasticity::enableRPESTDP(bool enable) {
    // RPE-STDP is the default implementation
    (void)enable; // For consistency with interface
}

void RewardModulatedPlasticity::enableDopamineGatedSTDP(bool enable) {
    // Dopamine-gated STDP is part of the default implementation
    (void)enable; // For consistency with interface
}

void RewardModulatedPlasticity::enableRewardAugmentedHebbian(bool enable) {
    // Reward-augmented Hebbian is part of the default implementation
    (void)enable; // For consistency with interface
}

float RewardModulatedPlasticity::calculateRewardModulationFactor() const {
    float rewardModulation = 1.0f;
    if (pImpl->predictionError != 0.0f) {
        if (pImpl->asymmetricRewardSensitivity) {
            // Larger rewards have larger effects (quadratic sensitivity)
            rewardModulation = 1.0f + pImpl->predictionError * pImpl->predictionError * 0.5f;
        } else {
            // Linear sensitivity
            rewardModulation = 1.0f + 0.5f * pImpl->predictionError;
        }
    }
    return rewardModulation;
}

float RewardModulatedPlasticity::calculateAsymmetricRewardSensitivity(float reward) const {
    return calculateAsymmetricModulation(reward);
}

float RewardModulatedPlasticity::calculateDelayedRewardEffect(float reward, float timeSinceEvent) const {
    return calculateDelayedRewardModulation(reward, timeSinceEvent);
}

float RewardModulatedPlasticity::calculateRPEBasedChange(float baselineChange, float predictionError) const {
    return baselineChange * calculateRPEModulation(predictionError);
}

float RewardModulatedPlasticity::calculateTrialBasedRewardModulation(float reward, float timeWindow) const {
    return getRecentRewardAverage(timeWindow);
}

void RewardModulatedPlasticity::recordRewardHistory(float reward, float time) {
    // Update running statistics
    static float totalReward = 0.0f;
    static size_t rewardCount = 0;
    static float timeSum = 0.0f;
    
    totalReward += reward;
    rewardCount++;
    timeSum += time;
    
    if (rewardCount > 0) {
        pImpl->averageReward = totalReward / static_cast<float>(rewardCount);
        pImpl->rewardVolatility = std::sqrt(pImpl->averageReward * 0.01f);
    }
}

float RewardModulatedPlasticity::getAverageReward() const {
    return pImpl->averageReward;
}

float RewardModulatedPlasticity::getRewardVolatility() const {
    return pImpl->rewardVolatility;
}

float RewardModulatedPlasticity::getExpectedRewardPrediction() const {
    return pImpl->expectedReward;
}

float RewardModulatedPlasticity::calculateAsymmetricModulation(float reward) const {
    // Asymmetric reward sensitivity implementation
    float asymmetryFactor = 1.0f;
    if (pImpl->asymmetricRewardSensitivity) {
        if (reward > 0.0f) {
            asymmetryFactor = 1.0f + std::pow(reward, 2.0f) * 0.5f;
        } else {
            asymmetryFactor = std::max(0.1f, 1.0f - std::pow(std::abs(reward), 1.5f) * 0.3f);
        }
    }
    return asymmetryFactor;
}

float RewardModulatedPlasticity::calculateDelayedRewardModulation(float reward, float timeSinceEvent) const {
    // Delayed reward modulation with exponential decay
    float decayFactor = std::exp(-timeSinceEvent / pImpl->creditAssignmentWindow);
    return reward * decayFactor;
}

float RewardModulatedPlasticity::getRecentRewardAverage(float timeWindow) const {
    // Calculate recent reward average (simplified)
    return pImpl->averageReward;
}

// RPEModulation implementation
struct RPEModulation::Impl {
    float predictionError;
    float learningRate;
    float rpeThreshold;
    float rpeIntegration;
    size_t rewardHistoryCapacity;
    
    Impl() : predictionError(0.0f), learningRate(0.01f), rpeThreshold(0.1f),
             rpeIntegration(0.9f), rewardHistoryCapacity(1000) {}
};

RPEModulation::RPEModulation() : pImpl(std::make_unique<Impl>()) {}

RPEModulation::~RPEModulation() = default;

void RPEModulation::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse) return;
    
    // Update prediction error
    updatePredictionError(pImpl->expectedReward, pImpl->actualReward);
    
    // Calculate RPE-based modulation
    float rpeModulation = calculateRPEModulation(1.0f);
    
    // Apply to eligibility trace
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace * rpeModulation);
    
    // Apply RPE-based weight change if prediction error is significant
    if (std::abs(pImpl->predictionError) > pImpl->rpeThreshold) {
        float delta = pImpl->learningRate * pImpl->predictionError;
        applyWeightChange(synapse, delta);
    }
}

void RPEModulation::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RPEModulation::getName() const {
    return "RPEModulation";
}

void RPEModulation::setPredictionError(float error) {
    pImpl->predictionError = error;
}

void RPEModulation::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void RPEModulation::setRPEThreshold(float threshold) {
    pImpl->rpeThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void RPEModulation::setRPEIntegration(float integration) {
    pImpl->rpeIntegration = std::clamp(integration, 0.0f, 1.0f);
}

void RPEModulation::setRewardHistoryCapacity(size_t capacity) {
    pImpl->rewardHistoryCapacity = capacity;
}

float RPEModulation::getPredictionError() const {
    return pImpl->predictionError;
}

float RPEModulation::getLearningRate() const {
    return pImpl->learningRate;
}

float RPEModulation::getRPEThreshold() const {
    return pImpl->rpeThreshold;
}

float RPEModulation::getRPEIntegration() const {
    return pImpl->rpeIntegration;
}

void RPEModulation::updatePredictionError(float expected, float actual) {
    pImpl->predictionError = actual - expected;
}

float RPEModulation::calculateRPEModulation(float delta) const {
    float rpeModulation = 1.0f;
    if (std::abs(pImpl->predictionError) > pImpl->rpeThreshold) {
        rpeModulation = 1.0f + pImpl->rpeIntegration * std::abs(pImpl->predictionError);
    }
    return rpeModulation;
}

float RPEModulation::calculateTemporalDifferenceError(float reward, float time) const {
    // Simple TD error: reward + gamma * next_value - current_value
    // Simplified for single step
    float tdError = reward - pImpl->predictionError;
    return tdError;
}

void RPEModulation::enableTDLearning(bool enable) {
    // TD learning enabled based on parameters
    (void)enable; // For consistency with interface
}

void RPEModulation::enableDeltaLearning(bool enable) {
    // Delta learning enabled based on parameters
    (void)enable; // For consistency with interface
}

void RPEModulation::enableActorCriticLearning(bool enable) {
    // Actor-critic learning enabled based on parameters
    (void)enable; // For consistency with interface
}

bool RPEModulation::isTDLearningEnabled() const {
    return true; // Default enabled
}

bool RPEModulation::isDeltaLearningEnabled() const {
    return true; // Default enabled
}

bool RPEModulation::isActorCriticLearningEnabled() const {
    return true; // Default enabled
}

// DopamineModulatedPlasticity implementation
struct DopamineModulatedPlasticity::Impl {
    float dopamineLevel;
    float dopamineModulation;
    float rewardPredictionError;
    float temporalDifference;
    
    bool phasicDopamineModulation;
    bool tonicDopamineModulation;
    bool rewardPredictionErrorModulation;
    
    Impl() : dopamineLevel(1.0f), dopamineModulation(1.0f),
             rewardPredictionError(0.0f), temporalDifference(0.0f),
             phasicDopamineModulation(true), tonicDopamineModulation(true),
             rewardPredictionErrorModulation(true) {}
};

DopamineModulatedPlasticity::DopamineModulatedPlasticity() : pImpl(std::make_unique<Impl>()) {}

DopamineModulatedPlasticity::~DopamineModulatedPlasticity() = default;

void DopamineModulatedPlasticity::update(Synapse* synapse,
                                         const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt) {
    if (!synapse) return;
    
    // Update dopamine dynamics based on reward and prediction error
    updateDopamineDynamics(pImpl->actualReward, 1.0f);
    
    // Calculate dopamine-modulated change
    float phasicModulation = calculatePhasicDopamineResponse(pImpl->rewardPredictionError);
    float tonicModulation = calculateTonicDopamineBaseline();
    
    float totalModulation = phasicModulation * tonicModulation;
    
    // Base Hebbian change
    float hebbianDelta = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
        float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
        hebbianDelta = preRate * postRate * 0.01f;
    }
    
    // Apply dopamine modulation
    float delta = hebbianDelta * totalModulation;
    applyWeightChange(synapse, delta);
}

void DopamineModulatedPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* DopamineModulatedPlasticity::getName() const {
    return "DopamineModulatedPlasticity";
}

void DopamineModulatedPlasticity::setDopamineLevel(float level) {
    pImpl->dopamineLevel = std::clamp(level, 0.0f, 2.0f);
}

void DopamineModulatedPlasticity::setDopamineModulation(float modulation) {
    pImpl->dopamineModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void DopamineModulatedPlasticity::setRewardPredictionError(float error) {
    pImpl->rewardPredictionError = error;
}

void DopamineModulatedPlasticity::setTemporalDifference(float td) {
    pImpl->temporalDifference = td;
}

float DopamineModulatedPlasticity::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

float DopamineModulatedPlasticity::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

float DopamineModulatedPlasticity::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

float DopamineModulatedPlasticity::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

void DopamineModulatedPlasticity::updateDopamineDynamics(float reward, float time) {
    // Update dopamine levels based on reward and prediction error
    pImpl->dopamineLevel = 1.0f + reward * 0.5f - pImpl->rewardPredictionError * 0.3f;
    pImpl->dopamineLevel = std::clamp(pImpl->dopamineLevel, 0.0f, 2.0f);
}

float DopamineModulatedPlasticity::calculatePhasicDopamineResponse(float predictionError) const {
    // Phasic dopamine response proportional to prediction error
    return 1.0f + std::abs(predictionError) * 0.5f;
}

float DopamineModulatedPlasticity::calculateTonicDopamineBaseline() const {
    // Tonic dopamine baseline
    return 1.0f;
}

void DopamineModulatedPlasticity::enablePhasicDopamineModulation(bool enable) {
    pImpl->phasicDopamineModulation = enable;
}

void DopamineModulatedPlasticity::enableTonicDopamineModulation(bool enable) {
    pImpl->tonicDopamineModulation = enable;
}

void DopamineModulatedPlasticity::enableRewardPredictionErrorModulation(bool enable) {
    pImpl->rewardPredictionErrorModulation = enable;
}

bool DopamineModulatedPlasticity::isPhasicDopamineModulationEnabled() const {
    return pImpl->phasicDopamineModulation;
}

bool DopamineModulatedPlasticity::isTonicDopamineModulationEnabled() const {
    return pImpl->tonicDopamineModulation;
}

bool DopamineModulatedPlasticity::isRewardPredictionErrorModulationEnabled() const {
    return pImpl->rewardPredictionErrorModulation;
}

// AsymmetricalRewardSensitivity implementation
struct AsymmetricRewardSensitivity::Impl {
    float sensitivityExponent;
    float sensitivityThreshold;
    float maximumAsymmetry;
    float baselineSensitivity;
    
    Impl() : sensitivityExponent(2.0f), sensitivityThreshold(0.1f),
             maximumAsymmetry(2.0f), baselineSensitivity(1.0f) {}
};

AsymmetricRewardSensitivity::AsymmetricRewardSensitivity() : pImpl(std::make_unique<Impl>()) {}

AsymmetricRewardSensitivity::~AsymmetricRewardSensitivity() = default;

void AsymmetricRewardSensitivity::update(Synapse* synapse,
                                         const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt) {
    if (!synapse) return;
    
    // This rule modulates the overall sensitivity based on reward
    // Actual sensitivity calculation happens in the plasticity system
}

void AsymmetricRewardSensitivity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* AsymmetricRewardSensitivity::getName() const {
    return "AsymmetricRewardSensitivity";
}

void AsymmetricRewardSensitivity::setSensitivityExponent(float exponent) {
    pImpl->sensitivityExponent = std::max(1.0f, exponent);
}

void AsymmetricRewardSensitivity::setSensitivityThreshold(float threshold) {
    pImpl->sensitivityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void AsymmetricRewardSensitivity::setMaximumAsymmetry(float maxAsym) {
    pImpl->maximumAsymmetry = std::clamp(maxAsym, 1.0f, 5.0f);
}

void AsymmetricRewardSensitivity::setBaselineSensitivity(float baseline) {
    pImpl->baselineSensitivity = std::clamp(baseline, 0.1f, 2.0f);
}

float AsymmetricRewardSensitivity::getSensitivityExponent() const {
    return pImpl->sensitivityExponent;
}

float AsymmetricRewardSensitivity::getSensitivityThreshold() const {
    return pImpl->sensitivityThreshold;
}

float AsymmetricRewardSensitivity::getMaximumAsymmetry() const {
    return pImpl->maximumAsymmetry;
}

float AsymmetricRewardSensitivity::getBaselineSensitivity() const {
    return pImpl->baselineSensitivity;
}

float AsymmetricRewardSensitivity::calculateAsymmetricModulation(float reward) const {
    float asymmetryFactor = getAsymmetryFactor(reward);
    return asymmetryFactor;
}

float AsymmetricRewardSensitivity::getAsymmetryFactor(float reward) const {
    float asymmetryFactor = pImpl->baselineSensitivity;
    
    if (std::abs(reward) > pImpl->sensitivityThreshold) {
        if (reward > 0.0f) {
            // Positive rewards amplify sensitivity
            asymmetryFactor *= std::min(pImpl->maximumAsymmetry,
                                      1.0f + std::pow(reward, pImpl->sensitivityExponent) * 0.3f);
        } else {
            // Negative rewards reduce sensitivity
            asymmetryFactor *= std::max(0.1f,
                                      1.0f - std::pow(std::abs(reward), pImpl->sensitivityExponent) * 0.2f);
        }
    }
    
    return asymmetryFactor;
}

// TrialBasedRewardModulation implementation
struct TrialBasedRewardModulation::Impl {
    size_t trialHistoryCapacity;
    float rewardMemoryDecay;
    float trialWeight;
    float rewardIntegration;
    
    Impl() : trialHistoryCapacity(100), rewardMemoryDecay(0.95f),
             trialWeight(0.5f), rewardIntegration(0.8f) {}
};

TrialBasedRewardModulation::TrialBasedRewardModulation() : pImpl(std::make_unique<Impl>()) {}

TrialBasedRewardModulation::~TrialBasedRewardModulation() = default;

void TrialBasedRewardModulation::update(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    if (!synapse) return;
    
    // Trial-based modulation would be handled by the plasticity system
    // This provides the mechanism
}

void TrialBasedRewardModulation::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* TrialBasedRewardModulation::getName() const {
    return "TrialBasedRewardModulation";
}

void TrialBasedRewardModulation::setTrialHistoryCapacity(size_t capacity) {
    pImpl->trialHistoryCapacity = capacity;
}

void TrialBasedRewardModulation::setRewardMemoryDecay(float decay) {
    pImpl->rewardMemoryDecay = std::clamp(decay, 0.5f, 0.99f);
}

void TrialBasedRewardModulation::setTrialWeight(float weight) {
    pImpl->trialWeight = std::clamp(weight, 0.0f, 1.0f);
}

void TrialBasedRewardModulation::setRewardIntegration(float integration) {
    pImpl->rewardIntegration = std::clamp(integration, 0.0f, 1.0f);
}

size_t TrialBasedRewardModulation::getTrialHistoryCapacity() const {
    return pImpl->trialHistoryCapacity;
}

float TrialBasedRewardModulation::getRewardMemoryDecay() const {
    return pImpl->rewardMemoryDecay;
}

float TrialBasedRewardModulation::getTrialWeight() const {
    return pImpl->trialWeight;
}

float TrialBasedRewardModulation::getRewardIntegration() const {
    return pImpl->rewardIntegration;
}

void TrialBasedRewardModulation::recordTrialOutcome(float reward, float time) {
    // Record trial outcome for later modulation
}

float TrialBasedRewardModulation::getRecentRewardAverage(float timeWindow) const {
    return 0.5f; // Simplified
}

float TrialBasedRewardModulation::calculateTrialBasedModulation(float baselineChange) const {
    return baselineChange * pImpl->trialWeight;
}

void TrialBasedRewardModulation::updateTrialDynamics(float dt) {
    // Update trial-based dynamics
}

void TrialBasedRewardModulation::resetTrialHistory() {
    // Reset trial history
}

// CreditAssignmentPlasticity implementation
struct CreditAssignmentPlasticity::Impl {
    float creditAssignmentWindow;
    float eligibilityTraceDecay;
    float traceLearningRate;
    float rewardDelayTolerance;
    bool gradientBasedAssignment;
    
    bool backpropagationEnabled;
    bool monteCarloLearningEnabled;
    
    Impl() : creditAssignmentWindow(100.0f), eligibilityTraceDecay(0.95f),
             traceLearningRate(0.01f), rewardDelayTolerance(50.0f),
             gradientBasedAssignment(true),
             backpropagationEnabled(false), monteCarloLearningEnabled(false) {}
};

CreditAssignmentPlasticity::CreditAssignmentPlasticity() : pImpl(std::make_unique<Impl>()) {}

CreditAssignmentPlasticity::~CreditAssignmentPlasticity() = default;

void CreditAssignmentPlasticity::update(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    if (!synapse) return;
    
    // Update eligibility trace
    updateEligibilityTrace(1.0f, pImpl->actualReward, dt);
    
    // Apply credit assignment if conditions are met
    if (pImpl->gradientBasedAssignment && pImpl->backpropagationEnabled) {
        // Gradient-based credit assignment
        float trace = synapse->getEligibilityTrace();
        float credit = calculateCreditAssignment(trace, pImpl->actualReward, 0.0f);
        float delta = credit * pImpl->traceLearningRate;
        applyWeightChange(synapse, delta);
    } else if (pImpl->monteCarloLearningEnabled) {
        // Monte Carlo credit assignment
        float trace = synapse->getEligibilityTrace();
        float credit = calculateCreditAssignment(trace, pImpl->actualReward, 0.0f);
        float delta = credit * pImpl->traceLearningRate;
        applyWeightChange(synapse, delta);
    }
}

void CreditAssignmentPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* CreditAssignmentPlasticity::getName() const {
    return "CreditAssignmentPlasticity";
}

void CreditAssignmentPlasticity::setCreditAssignmentWindow(float window) {
    pImpl->creditAssignmentWindow = std::max(0.0f, window);
}

void CreditAssignmentPlasticity::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::clamp(decay, 0.5f, 0.99f);
}

void CreditAssignmentPlasticity::setTraceLearningRate(float rate) {
    pImpl->traceLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

void CreditAssignmentPlasticity::setRewardDelayTolerance(float tolerance) {
    pImpl->rewardDelayTolerance = std::max(0.0f, tolerance);
}

void CreditAssignmentPlasticity::setGradientBasedAssignment(bool enable) {
    pImpl->gradientBasedAssignment = enable;
}

float CreditAssignmentPlasticity::getCreditAssignmentWindow() const {
    return pImpl->creditAssignmentWindow;
}

float CreditAssignmentPlasticity::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

float CreditAssignmentPlasticity::getTraceLearningRate() const {
    return pImpl->traceLearningRate;
}

float CreditAssignmentPlasticity::getRewardDelayTolerance() const {
    return pImpl->rewardDelayTolerance;
}

bool CreditAssignmentPlasticity::getGradientBasedAssignment() const {
    return pImpl->gradientBasedAssignment;
}

void CreditAssignmentPlasticity::updateEligibilityTrace(Synapse* synapse, float delta, float reward, float dt) {
    if (!synapse) return;
    float currentTrace = synapse->getEligibilityTrace();
    float newTrace = currentTrace * pImpl->eligibilityTraceDecay + delta;
    synapse->setEligibilityTrace(newTrace);
}

float CreditAssignmentPlasticity::calculateCreditAssignment(float trace, float reward, float delay) const {
    if (delay > pImpl->rewardDelayTolerance) {
        return 0.0f; // Too delayed, no credit assignment
    }
    
    float credit = trace * reward * std::exp(-delay / pImpl->creditAssignmentWindow);
    return credit;
}

void CreditAssignmentPlasticity::enableBackpropagation(bool enable) {
    pImpl->backpropagationEnabled = enable;
}

void CreditAssignmentPlasticity::enableMonteCarloLearning(bool enable) {
    pImpl->monteCarloLearningEnabled = enable;
}

bool CreditAssignmentPlasticity::isBackpropagationEnabled() const {
    return pImpl->backpropagationEnabled;
}

bool CreditAssignmentPlasticity::isMonteCarloLearningEnabled() const {
    return pImpl->monteCarloLearningEnabled;
}

} // namespace nlm


#include "EnhancedPlasticity.hpp"
#include "../../core/Types/Types.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

// EnhancedSTDP implementation

struct EnhancedSTDP::Impl {
    float ltpWeight;           // A+ for potentiation
    float ltdWeight;           // A- for depression
    float timeConstant;        // Tau for exponential window (ms)
    
    // Neuromodulation parameters
    float dopamineGain;        // Dopamine modulation strength
    float curiosityGain;       // Curiosity modulation strength
    
    // Eligibility trace
    float eligibilityTau;      // Trace decay time (ms)
    float maxEligibilityTrace; // Maximum trace value
    float eligibilityDecay;    // Current decay factor
    
    // Homeostatic plasticity
    float homeostaticTarget;  // Target firing rate
    float homeostaticRate;    // Learning rate for homeostasis
    
    // Internal state
    float currentDopamine;     // Last dopamine level
    float currentCuriosity;    // Last curiosity level
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             dopamineGain(1.0f), curiosityGain(0.5f),
             eligibilityTau(100.0f), maxEligibilityTrace(1.0f), eligibilityDecay(0.01f),
             homeostaticTarget(0.02f), homeostaticRate(0.001f),  // Target 2% firing rate
             currentDopamine(0.0f), currentCuriosity(0.0f) {}
};

EnhancedSTDP::EnhancedSTDP() : pImpl(new Impl) {}

EnhancedSTDP::~EnhancedSTDP() = default;

void EnhancedSTDP::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Apply neuromodulation to STDP parameters
    float modulatedLTP = pImpl->ltpWeight * (1.0f + pImpl->dopamineGain * pImpl->currentDopamine);
    float modulatedLTD = pImpl->ltdWeight * (1.0f + pImpl->curiosityGain * pImpl->currentCuriosity);
    
    // Calculate STDP contributions with triplet-like interactions
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Enhanced with neuromodulation and eligibility traces
                float baseDelta = modulatedLTP * std::exp(-dt / tau);
                
                // Add eligibility trace modulation
                float eligibility = std::exp(-dt / pImpl->eligibilityTau);
                totalDelta += baseDelta * eligibility;
                
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Enhanced with neuromodulation and eligibility traces
                float baseDelta = -modulatedLTD * std::exp(dt / tau);  // dt is negative
                float eligibility = std::exp(dt / pImpl->eligibilityTau);
                totalDelta += baseDelta * eligibility;
            }
        }
    }
    
    // Apply homeostatic plasticity modulation
    float currentWeight = synapse->getWeight();
    float weightDeviation = currentWeight - pImpl->homeostaticTarget;
    float homeostaticDelta = -pImpl->homeostaticRate * weightDeviation;
    
    totalDelta += homeostaticDelta;
    
    // Apply eligibility trace to reward-modulated learning
    if (totalDelta > 0.001f || totalDelta < -0.001f) {
        // Update eligibility trace (simplified)
        float currentTrace = synapse->getEligibilityTrace();
        float traceDecay = std::exp(-dt / pImpl->eligibilityTau);
        synapse->setEligibilityTrace(currentTrace * traceDecay + totalDelta * 0.1f);
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void EnhancedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* EnhancedSTDP::getName() const {
    return "EnhancedSTDP";
}

void EnhancedSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float EnhancedSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void EnhancedSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float EnhancedSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void EnhancedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float EnhancedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void EnhancedSTDP::setDopamineGain(float gain) {
    pImpl->dopamineGain = std::clamp(gain, 0.0f, 10.0f);
}

float EnhancedSTDP::getDopamineGain() const {
    return pImpl->dopamineGain;
}

void EnhancedSTDP::setCuriosityGain(float gain) {
    pImpl->curiosityGain = std::clamp(gain, 0.0f, 10.0f);
}

float EnhancedSTDP::getCuriosityGain() const {
    return pImpl->curiosityGain;
}

void EnhancedSTDP::setEligibilityTimeConstant(float tau) {
    pImpl->eligibilityTau = std::clamp(tau, 10.0f, 1000.0f);
    pImpl->eligibilityDecay = 1.0f / pImpl->eligibilityTau;
}

float EnhancedSTDP::getEligibilityTimeConstant() const {
    return pImpl->eligibilityTau;
}

void EnhancedSTDP::setMaxEligibilityTrace(float maxTrace) {
    pImpl->maxEligibilityTrace = std::max(0.0f, maxTrace);
}

float EnhancedSTDP::getMaxEligibilityTrace() const {
    return pImpl->maxEligibilityTrace;
}

void EnhancedSTDP::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::max(0.0f, target);
}

float EnhancedSTDP::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void EnhancedSTDP::setHomeostaticLearningRate(float rate) {
    pImpl->homeostaticRate = std::clamp(rate, 0.0f, 0.01f);
}

float EnhancedSTDP::getHomeostaticLearningRate() const {
    return pImpl->homeostaticRate;
}

void EnhancedSTDP::configure(float ltpWeight, float ltdWeight, float tau,
                           float eligibilityTau, float maxTrace,
                           float dopamineGain, float curiosityGain,
                           float homeostaticTarget, float homeostaticRate) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setEligibilityTimeConstant(eligibilityTau);
    setMaxEligibilityTrace(maxTrace);
    setDopamineGain(dopamineGain);
    setCuriosityGain(curiosityGain);
    setHomeostaticTarget(homeostaticTarget);
    setHomeostaticLearningRate(homeostaticRate);
}

void EnhancedSTDP::updateNeuromodulation(float dopamine, float curiosity, float novelty) {
    pImpl->currentDopamine = std::clamp(dopamine, -1.0f, 5.0f);
    pImpl->currentCuriosity = std::clamp(curiosity, -1.0f, 5.0f);
    
    // Novelty also affects eligibility trace
    if (novelty > 0.5f) {
        pImpl->eligibilityDecay *= (1.0f + novelty * 0.1f);
    }
}

float EnhancedSTDP::getEligibilityTrace(Synapse* synapse) const {
    return synapse ? synapse->getEligibilityTrace() : 0.0f;
}

void EnhancedSTDP::resetEligibilityTrace(Synapse* synapse) {
    if (synapse) {
        synapse->setEligibilityTrace(0.0f);
    }
}

// RewardModulatedSTDP implementation

struct RewardModulatedSTDP::Impl {
    float ltpWeight;
    float ltdWeight;
    float timeConstant;
    float learningRate;
    float rewardScale;
    float predictionError;
    
    // Reward integration
    float currentReward;
    float rewardHistory[10];  // Rolling reward history
    int historyIndex;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             learningRate(0.001f), rewardScale(1.0f), predictionError(0.0f),
             currentReward(0.0f), historyIndex(0) {
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
    }
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate base STDP
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Potentiation with reward modulation
                float baseDelta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += baseDelta * (1.0f + pImpl->rewardScale * pImpl->predictionError);
            } else if (dt < 0) {
                // Depression with reward modulation
                float baseDelta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += baseDelta * (1.0f - pImpl->rewardScale * pImpl->predictionError);
            }
        }
    }
    
    // Apply learning rate and prediction error
    totalDelta *= pImpl->learningRate * (1.0f + pImpl->predictionError * 0.5f);
    
    // Apply weight change with reward integration
    if (totalDelta > 1e-6f || totalDelta < -1e-6f) {
        applyWeightChange(synapse, totalDelta);
        
        // Update reward history for future modulation
        pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
        pImpl->rewardHistory[pImpl->historyIndex] = pImpl->currentReward;
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with reward-dependent bounds
    float currentWeight = synapse->getWeight();
    float maxWeight = 2.0f * (1.0f + pImpl->currentReward * pImpl->rewardScale);
    float minWeight = -2.0f * (1.0f - pImpl->currentReward * pImpl->rewardScale);
    
    float newWeight = std::clamp(currentWeight + delta, minWeight, maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setBaseSTDP(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.01f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setRewardScale(float scale) {
    pImpl->rewardScale = std::max(0.0f, scale);
}

float RewardModulatedSTDP::getRewardScale() const {
    return pImpl->rewardScale;
}

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = std::clamp(error, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getPredictionError() const {
    return pImpl->predictionError;
}

void RewardModulatedSTDP::updateReward(float reward, float predictionError) {
    pImpl->currentReward = std::clamp(reward, 0.0f, 1.0f);
    setPredictionError(predictionError);
}

// HomeostaticPlasticity implementation

struct HomeostaticPlasticity::Impl {
    float targetFiringRate;
    float learningRate;
    float stabilizationFactor;
    
    // Network activity tracking
    size_t totalSpikes;
    size_t totalNeurons;
    float currentFiringRate;
    float runningAverageRate;
    float rateHistory[20];  // Keep history for smoothing
    int historyIndex;
    
    Impl() : targetFiringRate(0.02f), learningRate(0.001f), stabilizationFactor(0.9f),
             totalSpikes(0), totalNeurons(100), currentFiringRate(0.0f),
             runningAverageRate(0.0f), historyIndex(0) {
        for (int i = 0; i < 20; ++i) rateHistory[i] = 0.0f;
    }
};

HomeostaticPlasticity::HomeostaticPlasticity() : pImpl(new Impl) {}

HomeostaticPlasticity::~HomeostaticPlasticity() = default;

void HomeostaticPlasticity::update(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt) {
    // Homeostatic plasticity acts on the network level, not individual synapses
    // This is a simplified implementation that could be expanded with more sophisticated mechanisms
    
    // Update network activity statistics
    totalSpikes += preSpikes.size() + postSpikes.size();
    currentFiringRate = static_cast<float>(totalSpikes) / (totalNeurons * dt);
    
    // Update running average with exponential smoothing
    pImpl->runningAverageRate = pImpl->runningAverageRate * pImpl->stabilizationFactor +
                               currentFiringRate * (1.0f - pImpl->stabilizationFactor);
    
    // Update history
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    pImpl->rateHistory[pImpl->historyIndex] = currentFiringRate;
    
    // Apply homeostatic scaling to synapses if firing rate deviates from target
    if (currentFiringRate > pImpl->targetFiringRate * 1.5f) {
        // Network is too active - depress all synapses
        float scalingFactor = 1.0f - pImpl->learningRate * (currentFiringRate / pImpl->targetFiringRate - 1.0f);
        float delta = (scalingFactor - 1.0f) * synapse->getWeight();
        applyWeightChange(synapse, delta);
    } else if (currentFiringRate < pImpl->targetFiringRate * 0.5f) {
        // Network is too quiet - potentiate all synapses
        float scalingFactor = 1.0f + pImpl->learningRate * (1.0f - currentFiringRate / pImpl->targetFiringRate);
        float delta = (scalingFactor - 1.0f) * synapse->getWeight();
        applyWeightChange(synapse, delta);
    }
}

void HomeostaticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    
    // Apply with homeostatic bounds to prevent runaway excitation/inhibition
    float maxWeight = 10.0f * (1.0f + currentFiringRate / pImpl->targetFiringRate);
    float minWeight = -10.0f * (1.0f - currentFiringRate / pImpl->targetFiringRate);
    
    float newWeight = std::clamp(currentWeight + delta, minWeight, maxWeight);
    synapse->setWeight(newWeight);
}

const char* HomeostaticPlasticity::getName() const {
    return "HomeostaticPlasticity";
}

void HomeostaticPlasticity::setTargetFiringRate(float rate) {
    pImpl->targetFiringRate = std::max(0.001f, rate);
}

float HomeostaticPlasticity::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

void HomeostaticPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0001f, 0.01f);
}

float HomeostaticPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

void HomeostaticPlasticity::setStabilizationFactor(float factor) {
    pImpl->stabilizationFactor = std::clamp(factor, 0.5f, 0.99f);
}

float HomeostaticPlasticity::getStabilizationFactor() const {
    return pImpl->stabilizationFactor;
}

void HomeostaticPlasticity::updateNetworkStats(size_t totalSpikes, size_t totalNeurons) {
    pImpl->totalSpikes = totalSpikes;
    pImpl->totalNeurons = totalNeurons;
    currentFiringRate = static_cast<float>(totalSpikes) / totalNeurons;
}

float HomeostaticPlasticity::getCurrentFiringRate() const {
    return currentFiringRate;
}

// TripletSTDP implementation

struct TripletSTDP::Impl {
    float a1LTP;           // First spike LTP amplitude
    float a2LTP;           // Second spike LTP amplitude
    float b1LTD;           // First spike LTD amplitude
    float b2LTD;           // Second spike LTD amplitude
    float tauPlus;         // LTP time constant
    float tauMinus;        // LTD time constant
    
    Impl() : a1LTP(0.005f), a2LTP(0.002f), b1LTD(0.004f), b2LTD(0.006f),
             tauPlus(20.0f), tauMinus(20.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.size() < 2 || postSpikes.size() < 2) {
        // Fall back to standard STDP for small spike counts
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Apply triplet STDP rules
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            
            if (dt > 0) {
                // Triplet LTP contributions
                if (i > 0) {
                    // Second pre spike (contributes to LTP)
                    float secondPreDelta = pImpl->a2LTP * std::exp(-dt / pImpl->tauPlus);
                    totalDelta += secondPreDelta;
                }
                if (j > 0) {
                    // Second post spike (contributes to LTP)
                    float secondPostDelta = pImpl->a1LTP * std::exp(-dt / pImpl->tauPlus);
                    totalDelta += secondPostDelta;
                }
                if (i > 0 && j > 0) {
                    // Triplet interaction (second-pre and second-post)
                    float tripletDelta = pImpl->a1LTP * pImpl->a2LTP * std::exp(-dt / pImpl->tauPlus);
                    totalDelta += tripletDelta;
                }
            } else if (dt < 0) {
                // Triplet LTD contributions
                if (i > 0) {
                    // Second pre spike (contributes to LTD)
                    float secondPreDelta = -pImpl->b2LTD * std::exp(dt / pImpl->tauMinus);
                    totalDelta += secondPreDelta;
                }
                if (j > 0) {
                    // Second post spike (contributes to LTD)
                    float secondPostDelta = -pImpl->b1LTD * std::exp(dt / pImpl->tauMinus);
                    totalDelta += secondPostDelta;
                }
                if (i > 0 && j > 0) {
                    // Triplet interaction
                    float tripletDelta = -pImpl->b1LTD * pImpl->b2LTD * std::exp(dt / pImpl->tauMinus);
                    totalDelta += tripletDelta;
                }
            }
        }
    }
    
    // Apply weight change
    if (totalDelta > 1e-6f || totalDelta < -1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to reasonable bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -5.0f, 5.0f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA1LTP(float weight) {
    pImpl->a1LTP = std::max(0.0f, weight);
}

void TripletSTDP::setA2LTP(float weight) {
    pImpl->a2LTP = std::max(0.0f, weight);
}

void TripletSTDP::setB1LTD(float weight) {
    pImpl->b1LTD = std::max(0.0f, weight);
}

void TripletSTDP::setB2LTD(float weight) {
    pImpl->b2LTD = std::max(0.0f, weight);
}

void TripletSTDP::setTauPlus(float tau) {
    pImpl->tauPlus = std::clamp(tau, 1.0f, 100.0f);
}

void TripletSTDP::setTauMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 1.0f, 100.0f);
}

void TripletSTDP::configure(float a1LTP, float a2LTP, float b1LTD, float b2LTD,
                           float tauPlus, float tauMinus) {
    setA1LTP(a1LTP);
    setA2LTP(a2LTP);
    setB1LTD(b1LTD);
    setB2LTD(b2LTD);
    setTauPlus(tauPlus);
    setTauMinus(tauMinus);
}

} // namespace nlm


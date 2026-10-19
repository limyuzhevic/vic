#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm
{

// STDP implementation with comprehensive biological features
struct STDP::Impl {
    float ltpWeight;          // A+ for potentiation (0.01 by default)
    float ltdWeight;          // A- for depression (0.012 by default)
    float timeConstant;       // Tau for exponential window (ms)
    float nmdatThreshold;     // NMDA receptor activation threshold (0.5 by default)
    float dopamineModulation; // Dopamine modulation factor (1.0 by default)
    float spatialRadius;      // Spatial STDP window radius (50.0 by default)
    bool useAsymmetric;       // Asymmetric STDP (true by default)
    bool useNMDA;             // NMDA-dependent STDP (true by default)
    bool useDopamine;         // Dopamine-modulated STDP (true by default)
    bool useSpatial;          // Spatial STDP (true by default)
    bool useTriplet;          // Triplet interactions (false by default)
    bool useNeuromodulator;   // Neuromodulator integration (true by default)
    
    // For triplet STDP
    float preTraceTau;        // Pre-synaptic trace time constant
    float postTraceTau;       // Post-synaptic trace time constant
    float tripletModulation;  // Triplet interaction modulation
    
    // Neural traces for eligibility
    float preTrace;           // Pre-synaptic trace for eligibility
    float postTrace;          // Post-synaptic trace for eligibility
    
    // Statistics for diagnostics
    size_t totalPotentiationEvents;
    size_t totalDepressionEvents;
    float totalWeightChange;
    
    Impl() : 
        ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
        nmdatThreshold(0.5f), dopamineModulation(1.0f),
        spatialRadius(50.0f), useAsymmetric(true), useNMDA(true),
        useDopamine(true), useSpatial(true), useTriplet(false),
        useNeuromodulator(true),
        preTraceTau(10.0f), postTraceTau(20.0f), tripletModulation(0.5f),
        preTrace(0.0f), postTrace(0.0f),
        totalPotentiationEvents(0), totalDepressionEvents(0),
        totalWeightChange(0.0f) {}
};

STDP::STDP() : pImpl(std::make_unique<Impl>()) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Basic asymmetric STDP (pre-before-post potentiation, post-before-pre depression)
    if (pImpl->useAsymmetric) {
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float deltaT = static_cast<float>(postTime - preTime);  // Δt in ms
                
                if (deltaT > 0) {
                    // Pre before post: Long-term potentiation (LTP)
                    float delta = calculatePairChange(preTime, postTime);
                    totalDelta += delta;
                    pImpl->totalPotentiationEvents++;
                } else if (deltaT < 0) {
                    // Post before pre: Long-term depression (LTD)
                    float delta = calculatePairChange(preTime, postTime);
                    totalDelta += delta;
                    pImpl->totalDepressionEvents++;
                }
                // dt == 0: no change (simultaneous spikes)
            }
        }
    }
    
    // NMDA-dependent STDP
    if (pImpl->useNMDA) {
        float nmdaFactor = 0.0f;
        float maxNMDAEffect = 0.0f;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float deltaT = static_cast<float>(postTime - preTime);
                if (deltaT > pImpl->nmdatThreshold) {  // NMDA requires sufficient depolarization
                    float nmdaEffect = std::exp(-deltaT / (pImpl->timeConstant * 2.0f));
                    nmdaFactor += nmdaEffect;
                    maxNMDAEffect = std::max(maxNMDAEffect, nmdaEffect);
                }
            }
        }
        
        if (maxNMDAEffect > 0.0f) {
            nmdaFactor /= maxNMDAEffect;  // Normalize
            totalDelta *= nmdaFactor;     // Apply NMDA modulation
        }
    }
    
    // Dopamine modulation of STDP
    if (pImpl->useDopamine) {
        float dopamineEffect = std::min(pImpl->dopamineModulation, 2.0f);
        totalDelta *= dopamineEffect;
    }
    
    // Spatial STDP (simplified neuron position-based modulation)
    if (pImpl->useSpatial) {
        float spatialFactor = std::exp(-totalDelta / pImpl->spatialRadius);
        totalDelta *= spatialFactor;
    }
    
    // Triplet interactions (advanced STDP mechanism)
    if (pImpl->useTriplet) {
        // Update traces
        float decayFactor = std::exp(-dt / pImpl->postTraceTau);
        pImpl->preTrace *= decayFactor;
        pImpl->postTrace *= decayFactor;
        
        // Add to traces based on spikes
        for (Timestamp preTime : preSpikes) {
            pImpl->preTrace += 1.0f;  // Each pre-spike adds to pre-trace
        }
        for (Timestamp postTime : postSpikes) {
            pImpl->postTrace += 1.0f; // Each post-spike adds to post-trace
        }
        
        // Triplet interactions
        float tripletEffect = pImpl->preTrace * pImpl->postTrace * pImpl->tripletModulation;
        totalDelta += tripletEffect * pImpl->ltpWeight;
    }
    
    // Neuromodulator integration (via eligibility traces)
    if (pImpl->useNeuromodulator) {
        // Calculate eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        float newTrace = currentTrace * 0.9f + totalDelta;  // Decay + new change
        synapse->setEligibilityTrace(newTrace);
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
        
        // Update statistics
        pImpl->totalWeightChange += totalDelta;
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds (-1.0 to +1.0)
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::setNMDAThreshold(float threshold) {
    pImpl->nmdatThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float STDP::getNMDAThreshold() const {
    return pImpl->nmdatThreshold;
}

void STDP::setDopamineModulation(float modulation) {
    pImpl->dopamineModulation = std::clamp(modulation, 0.0f, 2.0f);
}

float STDP::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

void STDP::setSpatialWindow(float radius) {
    pImpl->spatialRadius = std::clamp(radius, 10.0f, 200.0f);
}

float STDP::getSpatialWindow() const {
    return pImpl->spatialRadius;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau, float nmdatThreshold) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    if (nmdatThreshold > 0.0f) {
        setNMDAThreshold(nmdatThreshold);
    }
}

void STDP::enableAsymmetricSTDP(bool enable) {
    pImpl->useAsymmetric = enable;
}

void STDP::enableSpatialSTDP(bool enable) {
    pImpl->useSpatial = enable;
}

void STDP::enableNMDASTDP(bool enable) {
    pImpl->useNMDA = enable;
}

void STDP::enableDopamineSTDP(bool enable) {
    pImpl->useDopamine = enable;
}

void STDP::enableTripletInteractions(bool enable) {
    pImpl->useTriplet = enable;
}

void STDP::enableNeuromodulatorIntegration(bool enable) {
    pImpl->useNeuromodulator = enable;
}

bool STDP::isAsymmetricSTDP() const {
    return pImpl->useAsymmetric;
}

bool STDP::isSpatialSTDP() const {
    return pImpl->useSpatial;
}

bool STDP::isNMDASTDP() const {
    return pImpl->useNMDA;
}

bool STDP::isDopamineSTDP() const {
    return pImpl->useDopamine;
}

bool STDP::hasTripletInteractions() const {
    return pImpl->useTriplet;
}

bool STDP::hasNeuromodulatorIntegration() const {
    return pImpl->useNeuromodulator;
}

float STDP::getEligibilityTrace() const {
    // In a real implementation, this would return the synapse's eligibility trace
    // For now, return a calculated value based on current parameters
    return pImpl->dopamineModulation * pImpl->ltpWeight * 0.1f;
}

float STDP::calculatePairChange(float preTime, float postTime) const {
    float deltaT = postTime - preTime;
    
    if (pImpl->useAsymmetric) {
        if (deltaT > 0) {
            // Pre before post: POTENTIATION
            return pImpl->ltpWeight * std::exp(-deltaT / pImpl->timeConstant);
        } else {
            // Post before pre: DEPRESSION
            return -pImpl->ltdWeight * std::exp(deltaT / pImpl->timeConstant);
        }
    } else {
        // Symmetric STDP (simplified)
        float magnitude = pImpl->ltpWeight * std::exp(-std::abs(deltaT) / pImpl->timeConstant);
        return (deltaT > 0) ? magnitude : -magnitude;
    }
}

// RewardModulatedSTDP implementation
struct RewardModulatedSTDP::Impl {
    float predictionError;
    float dopamineModulation;
    float delayCreditAssignment;
    bool asymmetricRewardSensitivity;
    bool trialBasedRewardModulation;
    float rewardHistory;
    float learningRate;
    
    Impl() : predictionError(0.0f), dopamineModulation(1.0f), delayCreditAssignment(100.0f),
             asymmetricRewardSensitivity(true), trialBasedRewardModulation(true),
             rewardHistory(0.0f), learningRate(0.01f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(std::make_unique<Impl>()) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse) return;
    
    // Base STDP-like eligibility calculation
    float baseChange = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
        float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
        baseChange = pImpl->dopamineModulation * pImpl->learningRate * preRate * postRate;
    }
    
    // Calculate reward-modulated change
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
    
    // Apply trial-based modulation
    if (pImpl->trialBasedRewardModulation) {
        // Update reward history
        pImpl->rewardHistory = pImpl->predictionError;
        
        // Apply reward modulation to eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace * rewardModulation);
    }
    
    // Calculate final reward-modulated change
    float delta = baseChange * rewardModulation;
    
    // Apply delayed credit assignment
    if (pImpl->delayCreditAssignment > 0.0f) {
        // Decay the eligibility trace
        float trace = synapse->getEligibilityTrace();
        trace *= std::exp(-dt / pImpl->delayCreditAssignment);
        synapse->setEligibilityTrace(trace);
        
        // Apply the trace-modulated change
        delta *= trace;
    }
    
    // Apply weight change
    applyWeightChange(synapse, delta);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = error;
}

void RewardModulatedSTDP::setDopamineModulation(float modulation) {
    pImpl->dopamineModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void RewardModulatedSTDP::setDelayCreditAssignment(float delay) {
    pImpl->delayCreditAssignment = std::max(0.0f, delay);
}

void RewardModulatedSTDP::setAsymmetricRewardSensitivity(bool enable) {
    pImpl->asymmetricRewardSensitivity = enable;
}

void RewardModulatedSTDP::setTrialBasedRewardModulation(bool enable) {
    pImpl->trialBasedRewardModulation = enable;
}

float RewardModulatedSTDP::getPredictionError() const {
    return pImpl->predictionError;
}

float RewardModulatedSTDP::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

float RewardModulatedSTDP::getDelayCreditAssignment() const {
    return pImpl->delayCreditAssignment;
}

bool RewardModulatedSTDP::getAsymmetricRewardSensitivity() const {
    return pImpl->asymmetricRewardSensitivity;
}

bool RewardModulatedSTDP::getTrialBasedRewardModulation() const {
    return pImpl->trialBasedRewardModulation;
}

void RewardModulatedSTDP::calculatePredictionError(float predicted, float actual) {
    pImpl->predictionError = actual - predicted;
}

float RewardModulatedSTDP::calculateRSTPDElasticity(float stdpChange, float rewardModulation) const {
    return stdpChange * rewardModulation;
}

// TripletSTDP implementation
struct TripletSTDP::Impl {
    float preTraceTau;
    float postTraceTau;
    float tripletModulation;
    
    Impl() : preTraceTau(10.0f), postTraceTau(20.0f), tripletModulation(0.5f) {}
};

TripletSTDP::TripletSTDP() : pImpl(std::make_unique<Impl>()) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse) return;
    
    // Simplified triplet STDP using traces
    float preTrace = pImpl->preTrace;
    float postTrace = pImpl->postTrace;
    
    // Update traces based on spikes
    float decayFactor = std::exp(-dt / pImpl->postTraceTau);
    preTrace *= decayFactor;
    postTrace *= decayFactor;
    
    for (Timestamp preTime : preSpikes) {
        preTrace += 1.0f;
    }
    for (Timestamp postTime : postSpikes) {
        postTrace += 1.0f;
    }
    
    // Store updated traces
    // pImpl->preTrace = preTrace;
    // pImpl->postTrace = postTrace;
    
    // Calculate triplet interaction effect
    float tripletEffect = preTrace * postTrace * pImpl->tripletModulation;
    
    // Apply STDP-like weight change
    float delta = tripletEffect * 0.01f; // Base learning rate
    
    // Constrain delta
    delta = std::clamp(delta, -0.01f, 0.01f);
    
    applyWeightChange(synapse, delta);
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setPreTraceTimeConstant(float tau1) {
    pImpl->preTraceTau = std::clamp(tau1, 1.0f, 50.0f);
}

void TripletSTDP::setPostTraceTimeConstant(float tau2) {
    pImpl->postTraceTau = std::clamp(tau2, 1.0f, 100.0f);
}

void TripletSTDP::setTripletModulation(float modulation) {
    pImpl->tripletModulation = std::clamp(modulation, 0.0f, 1.0f);
}

float TripletSTDP::getPreTraceTimeConstant() const {
    return pImpl->preTraceTau;
}

float TripletSTDP::getPostTraceTimeConstant() const {
    return pImpl->postTraceTau;
}

float TripletSTDP::getTripletModulation() const {
    return pImpl->tripletModulation;
}

// MetaPlasticity implementation
struct MetaPlasticity::Impl {
    float metaplasticityRate;
    size_t learningHistoryCapacity;
    float stabilityFactor;
    float averageLearningMagnitude;
    float learningVolatility;
    
    Impl() : metaplasticityRate(0.01f), learningHistoryCapacity(1000),
             stabilityFactor(2.0f), averageLearningMagnitude(0.0f),
             learningVolatility(0.1f) {}
};

MetaPlasticity::MetaPlasticity() : pImpl(std::make_unique<Impl>()) {}

MetaPlasticity::~MetaPlasticity() = default;

void MetaPlasticity::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    // Metaplasticity: plasticity of plasticity
    // Adapt learning rate based on previous learning history
    
    if (!synapse) return;
    
    // Calculate current learning volatility
    float currentVolatility = calculateLearningVolatility();
    
    // Apply metaplasticity modulation
    float metaplasticityFactor = 1.0f / (1.0f + pImpl->stabilityFactor * currentVolatility);
    
    // Store current learning magnitude for future metaplasticity updates
    // In a real implementation, this would be stored in a learning history buffer
    
    // Apply metaplasticity to learning rate
    setLearningRate(getLearningRate() * metaplasticityFactor);
    
    // Note: In a full implementation, this would also update synaptic properties
    // based on metaplastic state (e.g., changing synaptic tag thresholds)
}

void MetaPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Record learning event for metaplasticity
    recordLearningEvent(std::abs(delta), 1.0f); // time placeholder
}

const char* MetaPlasticity::getName() const {
    return "MetaPlasticity";
}

void MetaPlasticity::setMetaplasticityRate(float rate) {
    pImpl->metaplasticityRate = std::clamp(rate, 0.0f, 0.1f);
}

void MetaPlasticity::setLearningHistoryCapacity(size_t capacity) {
    pImpl->learningHistoryCapacity = capacity;
}

void MetaPlasticity::setStabilityFactor(float factor) {
    pImpl->stabilityFactor = std::max(0.1f, factor);
}

float MetaPlasticity::getMetaplasticityRate() const {
    return pImpl->metaplasticityRate;
}

size_t MetaPlasticity::getLearningHistoryCapacity() const {
    return pImpl->learningHistoryCapacity;
}

float MetaPlasticity::getStabilityFactor() const {
    return pImpl->stabilityFactor;
}

void MetaPlasticity::recordLearningEvent(float magnitude, float time) {
    // In a real implementation, this would add to a learning history buffer
    // and update statistics
    
    // Simple running average calculation
    static float totalMagnitude = 0.0f;
    static size_t eventCount = 0;
    
    totalMagnitude += magnitude;
    eventCount++;
    
    if (eventCount > 0) {
        pImpl->averageLearningMagnitude = totalMagnitude / static_cast<float>(eventCount);
        // Calculate volatility (simplified)
        pImpl->learningVolatility = std::sqrt(pImpl->averageLearningMagnitude * 0.01f);
    }
}

float MetaPlasticity::getAverageLearningMagnitude() const {
    return pImpl->averageLearningMagnitude;
}

float MetaPlasticity::getLearningVolatility() const {
    return pImpl->learningVolatility;
}

} // namespace nlm


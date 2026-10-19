#include "PlasticityRule.hpp"

namespace nlm
{

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Forward declaration of Brain class
class Brain;

// Initialize default implementations
struct Hebbian::Impl {
    float learningRate;
    float baselineThreshold;
    float homeostaticTarget;
    bool useCovarianceRule;
    bool useBCMTheory;
    bool useAntiHebbian;
    float preSpikeHistory;
    float postSpikeHistory;
    
    Impl() : 
        learningRate(0.01f),
        baselineThreshold(0.5f),
        homeostaticTarget(0.1f),
        useCovarianceRule(true),
        useBCMTheory(false),
        useAntiHebbian(false),
        preSpikeHistory(0.0f),
        postSpikeHistory(0.0f) {}
};

Hebbian::Hebbian() : pImpl(std::make_unique<Impl>()) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate firing rates
    float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
    float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Update spike history for baseline calculation
    pImpl->preSpikeHistory = preRate;
    pImpl->postSpikeHistory = postRate;
    
    float delta = 0.0f;
    
    if (pImpl->useCovarianceRule) {
        // Covariance-based Hebbian learning
        float baselinePre = std::max(pImpl->preSpikeHistory, pImpl->postSpikeHistory) * 0.1f;
        float baselinePost = std::max(pImpl->postSpikeHistory, pImpl->preSpikeHistory) * 0.1f;
        float timeConstant = 20.0f;
        
        delta = calculateCovarianceChange(preRate, postRate, baselinePre, baselinePost,
                                         pImpl->learningRate, timeConstant);
    } else if (pImpl->useBCMTheory) {
        // BCM theory implementation (delegated to separate BCM class)
        // This would integrate with the BCM rule for threshold-based learning
        delta = pImpl->learningRate * (preRate * postRate - pImpl->baselineThreshold * postRate);
    } else {
        // Classic Hebbian: "fire together, wire together"
        delta = pImpl->learningRate * preRate * postRate;
    }
    
    // Apply anti-Hebbian if enabled
    if (pImpl->useAntiHebbian) {
        delta *= -1.0f; // Invert the sign for anti-Hebbian
    }
    
    // Apply homeostatic scaling if needed
    if (!homeostaticallyScaled_) {
        applyHomeostaticScaling(postRate, pImpl->homeostaticTarget);
    }
    
    // Apply metaplasticity based on previous learning
    updateMetaplasticity(lastLearningMagnitude_, dt);
    
    // Update eligibility trace for reward-modulated learning
    eligibilityTrace_ = calculateEligibilityTrace(delta, 0.9f);
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        lastLearningMagnitude_ = std::abs(delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, -1.0f, 1.0f); // Biological bounds
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setBaselineThreshold(float threshold) {
    pImpl->baselineThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Hebbian::getBaselineThreshold() const {
    return pImpl->baselineThreshold;
}

void Hebbian::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, 0.0f, 1.0f);
}

float Hebbian::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void Hebbian::enableCovarianceRule(bool enable) {
    pImpl->useCovarianceRule = enable;
}

void Hebbian::enableBCMTheory(bool enable) {
    pImpl->useBCMTheory = enable;
}

void Hebbian::enableAntiHebbian(bool enable) {
    pImpl->useAntiHebbian = enable;
}

// AntiHebbian implementation
struct AntiHebbian::Impl {
    float depressionRate;
    float baselineFiring;
    
    Impl() : depressionRate(0.01f), baselineFiring(0.01f) {}
};

AntiHebbian::AntiHebbian() : pImpl(std::make_unique<Impl>()) {}

AntiHebbian::~AntiHebbian() = default;

void AntiHebbian::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Anti-Hebbian: decrease weight when neurons fire together
    float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
    float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Strong depression when both fire
    float delta = -pImpl->depressionRate * preRate * postRate;
    
    // Clamp delta magnitude to prevent runaway depression
    if (std::abs(delta) > pImpl->depressionRate * 0.5f) {
        delta = std::copysign(pImpl->depressionRate * 0.5f, delta);
    }
    
    applyWeightChange(synapse, delta);
}

void AntiHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* AntiHebbian::getName() const {
    return "AntiHebbian";
}

void AntiHebbian::setDepressionRate(float rate) {
    pImpl->depressionRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbian::getDepressionRate() const {
    return pImpl->depressionRate;
}

// BCM implementation
struct BCM::Impl {
    float theta;          // Sliding threshold
    float sigma;          // Threshold sensitivity
    float learningRate;
    float preSpikeHistory;
    float postSpikeHistory;
    
    Impl() : theta(0.5f), sigma(0.1f), learningRate(0.01f),
             preSpikeHistory(0.0f), postSpikeHistory(0.0f) {}
};

BCM::BCM() : pImpl(std::make_unique<Impl>()) {}

BCM::~BCM() = default;

void BCM::update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Update spike history
    float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    pImpl->postSpikeHistory = postRate;
    float preRate = postRate > 0 ? pImpl->preSpikeHistory : 0.0f; // Simplified
    
    // BCM learning rule: Δw = η × (post × (post - θ))
    float postMinusTheta = postRate - pImpl->theta;
    float delta = pImpl->learningRate * postRate * postMinusTheta;
    
    // Limit the change magnitude
    delta = std::clamp(delta, -pImpl->learningRate, pImpl->learningRate);
    
    applyWeightChange(synapse, delta);
}

void BCM::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* BCM::getName() const {
    return "BCM";
}

void BCM::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
}

void BCM::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.0f, 1.0f);
}

void BCM::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCM::getTheta() const {
    return pImpl->theta;
}

float BCM::getSigma() const {
    return pImpl->sigma;
}

float BCM::getLearningRate() const {
    return pImpl->learningRate;
}

// HomeostaticPlasticity implementation
struct HomeostaticPlasticity::Impl {
    float targetFiringRate;
    float plasticityExponent;
    float learningRate;
    float currentFiringRate;
    
    Impl() : targetFiringRate(0.1f), plasticityExponent(2.0f), learningRate(0.01f),
             currentFiringRate(0.0f) {}
};

HomeostaticPlasticity::HomeostaticPlasticity() : pImpl(std::make_unique<Impl>()) {}

HomeostaticPlasticity::~HomeostaticPlasticity() = default;

void HomeostaticPlasticity::update(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Calculate current firing rate from post-synaptic activity
    float postRate = postSpikes.empty() ? 0.0f : 
                     static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    pImpl->currentFiringRate = postRate;
    
    // Homeostatic rule: scale weight based on deviation from target
    float rateError = pImpl->currentFiringRate - pImpl->targetFiringRate;
    float scalingFactor = std::pow(1.0f + rateError, pImpl->plasticityExponent);
    
    float delta = pImpl->learningRate * (scalingFactor - 1.0f);
    
    // Ensure weight doesn't change too drastically
    delta = std::clamp(delta, -pImpl->learningRate, pImpl->learningRate);
    
    applyWeightChange(synapse, delta);
}

void HomeostaticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* HomeostaticPlasticity::getName() const {
    return "HomeostaticPlasticity";
}

void HomeostaticPlasticity::setTargetFiringRate(float target) {
    pImpl->targetFiringRate = std::max(0.0f, target);
}

void HomeostaticPlasticity::setPlasticityExponent(float exponent) {
    pImpl->plasticityExponent = std::max(0.1f, exponent);
}

void HomeostaticPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HomeostaticPlasticity::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

float HomeostaticPlasticity::getPlasticityExponent() const {
    return pImpl->plasticityExponent;
}

float HomeostaticPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

// STDP implementation
struct STDP::Impl {
    float ltpWeight;          // A+ for potentiation
    float ltdWeight;          // A- for depression
    float timeConstant;       // Tau for exponential window (ms)
    float nmdatThreshold;     // NMDA receptor activation threshold
    float dopamineModulation; // Dopamine modulation factor
    float spatialRadius;      // Spatial STDP window radius
    bool useAsymmetric;       // Asymmetric STDP
    bool useNMDA;             // NMDA-dependent STDP
    bool useDopamine;         // Dopamine-modulated STDP
    bool useSpatial;          // Spatial STDP
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             nmdatThreshold(0.5f), dopamineModulation(1.0f),
             spatialRadius(50.0f), useAsymmetric(true), useNMDA(true),
             useDopamine(true), useSpatial(true) {}
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
    float tau = pImpl->timeConstant;
    
    // Apply asymmetric STDP
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                
                // Apply NMDA-dependent modulation
                if (pImpl->useNMDA) {
                    delta *= (dt / (dt + pImpl->nmdatThreshold));
                }
                
                // Apply dopamine modulation
                if (pImpl->useDopamine) {
                    delta *= pImpl->dopamineModulation;
                }
                
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                
                // Apply NMDA-dependent modulation
                if (pImpl->useNMDA) {
                    delta *= (std::abs(dt) / (std::abs(dt) + pImpl->nmdatThreshold));
                }
                
                // Apply dopamine modulation
                if (pImpl->useDopamine) {
                    delta *= pImpl->dopamineModulation;
                }
                
                totalDelta += delta;
            }
        }
    }
    
    // Apply spatial window if enabled
    if (pImpl->useSpatial) {
        // Simplified spatial modulation - in real implementation would consider
        // neuron positions and synaptic type
        float spatialFactor = std::exp(-totalDelta / pImpl->spatialRadius);
        totalDelta *= spatialFactor;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds
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

void STDP::configure(float ltpWeight, float ltdWeight, float tau, float nmdqaThreshold) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    if (nmdqaThreshold > 0.0f) {
        setNMDAThreshold(nmdqaThreshold);
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

// RewardModulatedPlasticity implementation
struct RewardModulatedPlasticity::Impl {
    float predictionError;
    float dopamineModulation;
    float delayCreditAssignment;
    bool asymmetricSensitivity;
    bool trialBasedModulation;
    float rewardHistory;
    float learningRate;
    
    Impl() : predictionError(0.0f), dopamineModulation(1.0f), delayCreditAssignment(100.0f),
             asymmetricSensitivity(true), trialBasedModulation(true),
             rewardHistory(0.0f), learningRate(0.01f) {}
};

RewardModulatedPlasticity::RewardModulatedPlasticity() : pImpl(std::make_unique<Impl>()) {}

RewardModulatedPlasticity::~RewardModulatedPlasticity() = default;

void RewardModulatedPlasticity::update(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate STDP-based eligibility trace
    float totalSTDP = 0.0f;
    float tau = 20.0f; // Standard STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (deltaT > 0) {
                // Potentiation
                float delta = pImpl->dopamineModulation * pImpl->learningRate * 
                             std::exp(-deltaT / tau);
                totalSTDP += delta;
            } else {
                // Depression
                float delta = -pImpl->dopamineModulation * pImpl->learningRate * 
                             std::exp(deltaT / tau);
                totalSTDP += delta;
            }
        }
    }
    
    // Apply reward prediction error modulation
    float rewardModulation = 1.0f;
    if (pImpl->predictionError != 0.0f) {
        if (pImpl->asymmetricSensitivity) {
            // Larger rewards have larger effects
            rewardModulation = 1.0f + std::abs(pImpl->predictionError);
        } else {
            // Linear sensitivity
            rewardModulation = 1.0f + 0.5f * pImpl->predictionError;
        }
    }
    
    // Apply trial-based modulation if enabled
    if (pImpl->trialBasedModulation) {
        // Reset eligibility trace based on reward
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace * rewardModulation);
    }
    
    // Calculate reward-modulated weight change
    float delta = totalSTDP * rewardModulation * pImpl->dopamineModulation;
    
    // Apply delayed credit assignment
    if (pImpl->delayCreditAssignment > 0.0f) {
        float currentTrace = synapse->getEligibilityTrace();
        delta *= std::exp(-dt / pImpl->delayCreditAssignment);
    }
    
    applyWeightChange(synapse, delta);
}

void RewardModulatedPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedPlasticity::getName() const {
    return "RewardModulatedPlasticity";
}

void RewardModulatedPlasticity::setPredictionError(float error) {
    pImpl->predictionError = error;
}

void RewardModulatedPlasticity::setDopamineModulation(float modulation) {
    pImpl->dopamineModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void RewardModulatedPlasticity::setDelayCreditAssignment(float delay) {
    pImpl->delayCreditAssignment = std::max(0.0f, delay);
}

void RewardModulatedPlasticity::setAsymmetricSensitivity(bool enable) {
    pImpl->asymmetricSensitivity = enable;
}

void RewardModulatedPlasticity::setTrialBasedModulation(bool enable) {
    pImpl->trialBasedModulation = enable;
}

float RewardModulatedPlasticity::getPredictionError() const {
    return pImpl->predictionError;
}

float RewardModulatedPlasticity::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

float RewardModulatedPlasticity::getDelayCreditAssignment() const {
    return pImpl->delayCreditAssignment;
}

bool RewardModulatedPlasticity::getAsymmetricSensitivity() const {
    return pImpl->asymmetricSensitivity;
}

bool RewardModulatedPlasticity::getTrialBasedModulation() const {
    return pImpl->trialBasedModulation;
}

void RewardModulatedPlasticity::calculatePredictionError(float predictedReward, float actualReward) {
    pImpl->predictionError = actualReward - predictedReward;
}

// NeuromodulatorPlasticity implementation
struct NeuromodulatorPlasticity::Impl {
    float dopamineLevel;
    float acetylcholineLevel;
    float norepinephrineLevel;
    float serotoninLevel;
    
    float dopamineModulation;
    float acetylcholineModulation;
    float norepinephrineModulation;
    float serotoninModulation;
    
    Impl() : 
        dopamineLevel(1.0f), acetylcholineLevel(1.0f), norepinephrineLevel(1.0f),
        serotoninLevel(1.0f),
        dopamineModulation(1.0f), acetylcholineModulation(1.0f),
        norepinephrineModulation(1.0f), serotoninModulation(1.0f) {}
};

NeuromodulatorPlasticity::NeuromodulatorPlasticity() : pImpl(std::make_unique<Impl>()) {}

NeuromodulatorPlasticity::~NeuromodulatorPlasticity() = default;

void NeuromodulatorPlasticity::update(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Base Hebbian contribution
    float hebbianDelta = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
        float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
        hebbianDelta = pImpl->dopamineModulation * preRate * postRate;
    }
    
    // Apply neuromodulator-specific modulations
    float totalModulation = 1.0f;
    
    // Dopamine modulation: enhances learning for rewarded predictions
    if (pImpl->dopamineModulation != 1.0f) {
        totalModulation *= pImpl->dopamineModulation;
    }
    
    // Acetylcholine modulation: enhances attention and memory consolidation
    if (pImpl->acetylcholineModulation != 1.0f) {
        totalModulation *= (1.0f + (pImpl->acetylcholineModulation - 1.0f) * 0.5f);
    }
    
    // Norepinephrine modulation: enhances arousal and vigilance
    if (pImpl->norepinephrineModulation != 1.0f) {
        totalModulation *= (1.0f + (pImpl->norepinephrineModulation - 1.0f) * 0.3f);
    }
    
    // Serotonin modulation: modulates mood and social behavior
    if (pImpl->serotoninModulation != 1.0f) {
        totalModulation *= (1.0f - 0.2f * (pImpl->serotoninModulation - 1.0f));
    }
    
    // Calculate final weight change
    float delta = hebbianDelta * totalModulation;
    
    applyWeightChange(synapse, delta);
}

void NeuromodulatorPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* NeuromodulatorPlasticity::getName() const {
    return "NeuromodulatorPlasticity";
}

void NeuromodulatorPlasticity::setDopamineLevel(float level) {
    pImpl->dopamineLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorPlasticity::setAcetylcholineLevel(float level) {
    pImpl->acetylcholineLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorPlasticity::setNorepinephrineLevel(float level) {
    pImpl->norepinephrineLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorPlasticity::setSerotoninLevel(float level) {
    pImpl->serotoninLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorPlasticity::setDopamineModulation(float modulation) {
    pImpl->dopamineModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void NeuromodulatorPlasticity::setAcetylcholineModulation(float modulation) {
    pImpl->acetylcholineModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void NeuromodulatorPlasticity::setNorepinephrineModulation(float modulation) {
    pImpl->norepinephrineModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void NeuromodulatorPlasticity::setSerotoninModulation(float modulation) {
    pImpl->serotoninModulation = std::clamp(modulation, 0.0f, 2.0f);
}

float NeuromodulatorPlasticity::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

float NeuromodulatorPlasticity::getAcetylcholineLevel() const {
    return pImpl->acetylcholineLevel;
}

float NeuromodulatorPlasticity::getNorepinephrineLevel() const {
    return pImpl->norepinephrineLevel;
}

float NeuromodulatorPlasticity::getSerotoninLevel() const {
    return pImpl->serotoninLevel;
}

float NeuromodulatorPlasticity::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

float NeuromodulatorPlasticity::getAcetylcholineModulation() const {
    return pImpl->acetylcholineModulation;
}

float NeuromodulatorPlasticity::getNorepinephrineModulation() const {
    return pImpl->norepinephrineModulation;
}

float NeuromodulatorPlasticity::getSerotoninModulation() const {
    return pImpl->serotoninModulation;
}

void NeuromodulatorPlasticity::integrateDopamineModulation(float baselineLearning, float rewardPrediction) {
    pImpl->dopamineModulation = 1.0f + rewardPrediction * 0.5f;
}

void NeuromodulatorPlasticity::integrateAcetylcholineModulation(float attention, float novelty) {
    pImpl->acetylcholineModulation = 1.0f + attention * 0.3f + novelty * 0.2f;
}

void NeuromodulatorPlasticity::integrateNorepinephrineModulation(float arousal, float stress) {
    pImpl->norepinephrineModulation = 1.0f + arousal * 0.4f + stress * 0.3f;
}

void NeuromodulatorPlasticity::integrateSerotoninModulation(float mood, float social) {
    pImpl->serotoninModulation = 1.0f + mood * 0.2f - social * 0.3f;
}

} // namespace nlm


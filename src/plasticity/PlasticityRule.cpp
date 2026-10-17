#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <unordered_map>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Comprehensive Hebbian Learning Implementation with Reward Modulation
struct HebbianRule::Impl {
    // Core Hebbian parameters
    float learningRate;           // Base learning rate (η)
    float covarianceThreshold;    // For covariance rule
    
    // STDP parameters
    float ltpWeight;              // A+ for potentiation
    float ltdWeight;              // A- for depression
    float stdpTimeConstant;       // τ for exponential window (ms)
    float stdpMinWeight;          // Minimum STDP weight
    float stdpMaxWeight;          // Maximum STDP weight
    
    // Reward-modulated learning parameters
    float rewardThreshold;         // Threshold for dopamine gating
    float rewardModulationFactor;  // Scaling factor for reward modulation
    
    // Homeostatic and normalization parameters
    float targetWeight;           // Target weight for homeostasis
    float homeostaticRate;        // Rate of homeostatic adjustment
    float weightNormalization;     // Weight normalization strength
    float synapticDepression;     // Synaptic depression rate (LTD component)
    
    // Eligibility trace parameters
    float eligibilityDecay;        // Trace decay rate (fast for reward)
    float eligibilityTau;          // Trace time constant (ms)
    float traceIntegration;       // Integration factor for eligibility
    
    // Learning rule selection
    enum class LearningRuleType {
        Hebbian,
        BCM,
        Oja,
        AntiHebbian,
        RewardModulated,
        TripletSTDP
    };
    
    LearningRuleType currentRule;
    
    // Temporal credit assignment structures
    struct SpikeHistory {
        std::vector<Timestamp> preSpikeTimes;
        std::vector<Timestamp> postSpikeTimes;
        Timestamp lastPreSpike;
        Timestamp lastPostSpike;
        float accumulatedEligibility;
        float eligibilityTrace;
        float traceTrace;  // Backup for fast/slow traces
    };
    
    std::unordered_map<SynapseId, SpikeHistory> spikeHistory;
    
    // Multiple time scales for eligibility traces
    float fastTraceDecay;         // Fast eligibility (ms)
    float slowTraceDecay;          // Slow eligibility (seconds)
    Timestamp lastRewardTime;
    
    // Activity statistics for homeostatic mechanisms
    float runningActivity;         // Running average of pre-post correlations
    float maxActivityObserved;     // Track maximum activity
    float depressionThreshold;    // When to trigger synaptic depression
    
    // Biological parameters
    float nmdaThreshold;           // NMDA spike threshold
    float calciumConductance;      // Calcium-dependent plasticity
    float apfRate;                 // Afterhyperpolarization factor
    
    Impl() : 
        learningRate(0.015f),
        covarianceThreshold(0.0f),
        ltpWeight(0.025f),
        ltdWeight(0.027f),
        stdpTimeConstant(15.0f),
        stdpMinWeight(-1.0f),
        stdpMaxWeight(1.0f),
        rewardThreshold(0.05f),
        rewardModulationFactor(1.0f),
        targetWeight(0.6f),
        homeostaticRate(0.0005f),
        weightNormalization(0.05f),
        synapticDepression(0.0002f),
        eligibilityDecay(0.9f),
        eligibilityTau(50.0f),
        traceIntegration(1.0f),
        currentRule(LearningRuleType::Hebbian),
        fastTraceDecay(0.98f),
        slowTraceDecay(0.999f),
        lastRewardTime(0.0),
        runningActivity(0.0f),
        maxActivityObserved(0.0f),
        depressionThreshold(0.8f),
        nmdaThreshold(-45.0f),
        calciumConductance(0.5f),
        apfRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get current spike history for this synapse
    SpikeHistory& history = pImpl->spikeHistory[synapse->getId()];
    
    // Update spike records
    for (Timestamp preTime : preSpikes) {
        history.preSpikeTimes.push_back(preTime);
        history.lastPreSpike = preTime;
    }
    
    for (Timestamp postTime : postSpikes) {
        history.postSpikeTimes.push_back(postTime);
        history.lastPostSpike = postTime;
    }
    
    // Calculate STDP-based weight change
    float stdpDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: Long-Term Potentiation (LTP)
                float nmdaActivation = std::exp(-dt / pImpl->stdpTimeConstant);
                float calciumSignal = nmdaActivation * std::abs(dt) / 100.0f;
                stdpDelta += pImpl->ltpWeight * calciumSignal;
            } else if (dt < 0) {
                // Post before pre: Long-Term Depression (LTD)
                float bapActivation = std::exp(dt / pImpl->stdpTimeConstant);
                float depression = -pImpl->ltdWeight * bapActivation;
                stdpDelta += depression;
            }
        }
    }
    
    // Apply reward-modulated plasticity gate
    float rewardGate = 1.0f;
    // For now, using a simple heuristic based on spike activity
    // In a real implementation, this would use actual neuromodulator levels
    size_t totalSpikePairs = history.preSpikeTimes.size() * history.postSpikeTimes.size();
    if (totalSpikePairs > 0) {
        float spikeRate = static_cast<float>(totalSpikePairs) / 100.0f;  // Normalized rate
        if (spikeRate > pImpl->rewardThreshold) {
            rewardGate = std::min(1.0f, spikeRate * pImpl->rewardModulationFactor);
        } else {
            rewardGate = 0.0f;  // No plasticity without sufficient activity
        }
    }
    
    // Apply neuromodulator modulation
    float neuromodulatorEffect = 1.0f;
    // In a real implementation, this would use actual neuromodulator levels
    // For now, it's a placeholder for neuromodulator integration
    
    // Combine STDP with Hebbian correlation
    float hebbianDelta = 0.0f;
    
    if (pImpl->currentRule == Impl::LearningRuleType::Hebbian ||
        pImpl->currentRule == Impl::LearningRuleType::RewardModulated) {
        
        // Calculate correlation-based Hebbian learning (covariance rule)
        float preCorrelation = 0.0f, postCorrelation = 0.0f;
        float preMean = 0.0f, postMean = 0.0f;
        
        // Compute spike correlations
        size_t totalPairs = 0;
        for (size_t i = 0; i < history.preSpikeTimes.size(); ++i) {
            for (size_t j = 0; j < history.postSpikeTimes.size(); ++j) {
                float dt = static_cast<float>(history.postSpikeTimes[j] - history.preSpikeTimes[i]);
                if (std::abs(dt) < 50.0f) {  // 50ms correlation window
                    preCorrelation += synapse->getWeight();
                    postCorrelation += synapse->getWeight();
                    preMean += synapse->getWeight();
                    postMean += synapse->getWeight();
                    totalPairs++;
                }
            }
        }
        
        if (totalPairs > 0) {
            preMean /= static_cast<float>(totalPairs);
            postMean /= static_cast<float>(totalPairs);
            
            // Covariance-based Hebbian update
            float covariance = (preCorrelation / totalPairs) * (postCorrelation / totalPairs) - preMean * postMean;
            hebbianDelta = pImpl->learningRate * covariance * rewardGate;
        }
    }
    
    // Apply Oja rule if selected
    if (pImpl->currentRule == Impl::LearningRuleType::Oja) {
        float weight = synapse->getWeight();
        float weightSquared = weight * weight;
        if (weightSquared > 0.01f) {
            hebbianDelta -= 0.5f * pImpl->learningRate * (hebbianDelta * weight) / weightSquared;
        }
    }
    
    // Apply BCM rule if selected
    if (pImpl->currentRule == Impl::LearningRuleType::BCM) {
        float weight = synapse->getWeight();
        float theta = pImpl->targetWeight * (1.0f + 0.2f * history.accumulatedEligibility);
        float postActivity = static_cast<float>(history.postSpikeTimes.size()) - 0.5f;
        
        float bcmDelta = pImpl->learningRate * postActivity * (postActivity - theta);
        hebbianDelta += bcmDelta;
    }
    
    // Apply anti-Hebbian rule if selected
    if (pImpl->currentRule == Impl::LearningRuleType::AntiHebbian) {
        hebbianDelta = -hebbianDelta;  // Reverse sign for anti-Hebbian
    }
    
    // Update eligibility traces
    float totalPlasticity = stdpDelta + hebbianDelta;
    history.eligibilityTrace = history.eligibilityTrace * pImpl->eligibilityDecay + totalPlasticity * rewardGate;
    
    // Apply synaptic depression for homeostatic regulation
    if (pImpl->currentRule == Impl::LearningRuleType::RewardModulated ||
        pImpl->currentRule == Impl::LearningRuleType::Hebbian) {
        
        // Update activity statistics
        history.accumulatedEligibility = history.eligibilityTrace;
        pImpl->runningActivity = pImpl->runningActivity * 0.99f + 
            std::abs(totalPlasticity) * 0.01f;
        
        if (pImpl->runningActivity > pImpl->depressionThreshold) {
            // Apply synaptic depression
            float depression = -pImpl->synapticDepression * (pImpl->runningActivity - pImpl->depressionThreshold);
            totalPlasticity += depression;
        }
    }
    
    // Apply weight normalization toward target
    float weight = synapse->getWeight();
    float normalization = pImpl->homeostaticRate * (pImpl->targetWeight - weight);
    totalPlasticity += normalization;
    
    // Apply weight bounds and normalization
    float newWeight = weight + totalPlasticity;
    
    // Apply weight normalization to prevent runaway excitation
    if (std::abs(newWeight - pImpl->targetWeight) > 0.2f) {
        newWeight = pImpl->targetWeight + (newWeight - pImpl->targetWeight) * 0.8f;
    }
    
    newWeight = std::clamp(newWeight, pImpl->stdpMinWeight, pImpl->stdpMaxWeight);
    
    synapse->addToWeight(newWeight - weight);
    
    // Update eligibility trace in synapse
    float currentEligibility = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentEligibility + history.eligibilityTrace);
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(pImpl->eligibilityDecay);
    
    // Clean up old spike history (keep only recent spikes)
    auto currentTime = std::chrono::duration<double, std::milli>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
    
    // Remove spikes older than 1 second
    auto removeOlderThan = currentTime - 1000.0;
    
    // Filter spike times
    auto newPreEnd = std::remove_if(history.preSpikeTimes.begin(), history.preSpikeTimes.end(),
        [removeOlderThan](Timestamp t) { return t < removeOlderThan; });
    history.preSpikeTimes.erase(newPreEnd, history.preSpikeTimes.end());
    
    auto newPostEnd = std::remove_if(history.postSpikeTimes.begin(), history.postSpikeTimes.end(),
        [removeOlderThan](Timestamp t) { return t < removeOlderThan; });
    history.postSpikeTimes.erase(newPostEnd, history.postSpikeTimes.end());
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply weight normalization to prevent runaway excitation
    if (newWeight > pImpl->stdpMaxWeight) {
        newWeight = pImpl->stdpMaxWeight * 0.95f;
    } else if (newWeight < pImpl->stdpMinWeight) {
        newWeight = pImpl->stdpMinWeight * 1.05f;
    }
    
    // Apply synaptic depression for stability
    float absWeight = std::abs(newWeight);
    if (absWeight > pImpl->depressionThreshold) {
        float depressionFactor = 1.0f - (absWeight - pImpl->depressionThreshold) * 0.1f;
        newWeight *= depressionFactor;
    }
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    switch (pImpl->currentRule) {
        case Impl::LearningRuleType::Hebbian: return "Hebbian";
        case Impl::LearningRuleType::BCM: return "BCM";
        case Impl::LearningRuleType::Oja: return "Oja";
        case Impl::LearningRuleType::AntiHebbian: return "AntiHebbian";
        case Impl::LearningRuleType::RewardModulated: return "R-STDP";
        case Impl::LearningRuleType::TripletSTDP: return "TripletSTDP";
        default: return "Hebbian";
    }
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setRuleType(Impl::LearningRuleType rule) {
    pImpl->currentRule = rule;
}

void HebbianRule::setNeuromodulators(Dopamine* dopamine, Serotonin* serotonin, PredictionError* predictionError) {
    // Store neuromodulator pointers for future use
    // In a complete implementation, these would be used for real neuromodulator integration
    (void) dopamine;
    (void) serotonin;
    (void) predictionError;
}

void HebbianRule::setHomeostaticParameters(float targetWeight, float homeostaticRate, float normalization) {
    pImpl->targetWeight = std::clamp(targetWeight, 0.1f, 0.9f);
    pImpl->homeostaticRate = std::clamp(homeostaticRate, 0.0001f, 0.01f);
    pImpl->weightNormalization = std::clamp(normalization, 0.01f, 0.5f);
}

void HebbianRule::setSTDPParameters(float ltpWeight, float ltdWeight, float timeConstant) {
    pImpl->ltpWeight = std::clamp(ltpWeight, 0.0f, 1.0f);
    pImpl->ltdWeight = std::clamp(ltdWeight, 0.0f, 1.0f);
    pImpl->stdpTimeConstant = std::clamp(timeConstant, 1.0f, 100.0f);
}

void HebbianRule::setRewardModulationParameters(float threshold, float factor) {
    pImpl->rewardThreshold = std::clamp(threshold, 0.0f, 1.0f);
    pImpl->rewardModulationFactor = std::clamp(factor, 0.1f, 10.0f);
}

void HebbianRule::configure(float ltpWeight, float ltdWeight, float tau,
                           float targetWeight, float homeostaticRate,
                           float rewardThreshold, float rewardFactor) {
    setSTDPParameters(ltpWeight, ltdWeight, tau);
    setHomeostaticParameters(targetWeight, homeostaticRate, 0.1f);
    setRewardModulationParameters(rewardThreshold, rewardFactor);
}

} // namespace nlm
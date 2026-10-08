#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    // Real neuroscience-based Hebbian learning parameters
    float learningRate;            // Base learning rate (η)
    float maxWeight;              // Maximum synaptic weight
    float minWeight;              // Minimum synaptic weight
    float covarianceThreshold;     // Threshold for covariance-based learning
    
    // Activity-dependent plasticity mechanisms
    float rateCorrelationWindow;   // Time window for rate correlation (ms)
    float spikeCountHistorySize;   // Number of spikes to store for rate calculation
    
    // Synaptic tagging and capture (STC) for persistence
    float tagDecayRate;            // Rate at which synaptic tags decay
    float captureThreshold;        // Threshold for tag capture
    float maxTagStrength;          // Maximum tag strength
    
    // Cooperative and competitive mechanisms
    float cooperativeFactor;       // Factor for cooperative learning between synapses
    float competitiveFactor;       // Factor for competitive learning for resources
    
    // Synaptic scaling parameters
    float scalingTargetMean;       // Target mean synaptic weight (0.5)
    float scalingStrength;         // Strength of synaptic scaling
    float globalRate;              // Global firing rate for normalization
    
    // Homeostatic stabilization
    float homeostaticRate;         // Homeostatic learning rate
    float homeostaticTarget;       // Target firing rate for homeostasis
    
    // Spike-timing dependent Hebbian (STDHebbian)
    float hebbianTimeWindow;       // Time window for spike timing-dependent Hebbian
    float hebbianLtpFactor;        // Enhancement factor for LTP
    float hebbianLtdFactor;        // Enhancement factor for LTD
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), rateCorrelationWindow(100.0f),
             spikeCountHistorySize(10),
             tagDecayRate(0.1f), captureThreshold(0.5f), maxTagStrength(1.0f),
             cooperativeFactor(0.8f), competitiveFactor(0.3f),
             scalingTargetMean(0.5f), scalingStrength(0.05f), globalRate(10.0f),
             homeostaticRate(0.01f), homeostaticTarget(5.0f),
             hebbianTimeWindow(50.0f), hebbianLtpFactor(1.5f), hebbianLtdFactor(1.2f) {
        
        NLM_LOG_INFO("Hebbian system initialized with biologically accurate mechanisms");
    }
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0) {
        return;
    }
    
    // Update global rate estimate based on recent activity
    pImpl->globalRate = (pImpl->globalRate * 0.9f) + (static_cast<float>(preSpikes.size()) * 10.0f) * 0.1f;
    
    float totalDelta = 0.0f;
    
    // Calculate spike statistics for covariance-based Hebbian learning
    size_t preCount = preSpikes.size();
    size_t postCount = postSpikes.size();
    
    // 1. Classical Correlated Activity (Hebb's rule)
    // "Neurons that fire together, wire together"
    // Enhanced for precise spike timing (coincidence detection)
    size_t preciseCoincidences = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timingDiff = std::abs(static_cast<float>(postTime - preTime));
            if (timingDiff < 5.0f) {  // Precise timing window for spike pairing
                preciseCoincidences++;
            }
        }
    }
    
    // Apply classical Hebbian potentiation
    if (preciseCoincidences > 0) {
        float hebbianDelta = pImpl->learningRate * static_cast<float>(preciseCoincidences);
        
        // Activity-dependent modulation based on firing rates
        float preRate = static_cast<float>(preCount) / std::max(1.0f, pImpl->rateCorrelationWindow);
        float postRate = static_cast<float>(postCount) / std::max(1.0f, pImpl->rateCorrelationWindow);
        
        // Rate correlation enhances learning when both neurons fire at similar rates
        float rateCorrelation = 1.0f - std::abs(preRate - postRate) / std::max(1.0f, pImpl->globalRate);
        hebbianDelta *= (1.0f + rateCorrelation * 0.5f);
        
        // Synaptic tagging and capture (STC)
        // Tag strength accumulates with correlated activity
        float currentTag = synapse->getEligibilityTrace(); // Reusing eligibility trace for tag
        float tagIncrement = hebbianDelta * pImpl->learningRate;
        currentTag += tagIncrement;
        currentTag = std::min(currentTag, pImpl->maxTagStrength);
        synapse->setEligibilityTrace(currentTag);
        
        // Apply tag capture if threshold reached
        if (currentTag > pImpl->captureThreshold) {
            // Persistent synaptic modification (tagged)
            hebbianDelta *= pImpl->hebbianLtpFactor;
            NLM_LOG_DEBUG("Hebbian tag capture: tag=" + std::to_string(currentTag) + ", enhancing LTP");
        }
        
        // Decay tag over time
        currentTag *= (1.0f - pImpl->tagDecayRate);
        synapse->setEligibilityTrace(currentTag);
        
        totalDelta += hebbianDelta;
        
        NLM_LOG_DEBUG("Hebbian classical: coincidences=" + std::to_string(preciseCoincidences) + 
                     ", preRate=" + std::to_string(preRate) + 
                     ", postRate=" + std::to_string(postRate) +
                     ", correlation=" + std::to_string(rateCorrelation) +
                     ", tag=" + std::to_string(currentTag) +
                     ", Δw=" + std::to_string(hebbianDelta));
    }
    
    // 2. Covariance-based Hebbian learning
    // Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // More sophisticated than simple correlation, accounts for baseline activity
    float preMeanRate = static_cast<float>(preCount) / static_cast<float>(preSpikes.size() > 0 ? preSpikes.size() : 1);
    float postMeanRate = static_cast<float>(postCount) / static_cast<float>(postSpikes.size() > 0 ? postSpikes.size() : 1);
    
    float covariance = calculateCovariance(preSpikes, postSpikes, dt);
    if (std::abs(covariance) > pImpl->covarianceThreshold) {
        float covarianceDelta = pImpl->learningRate * covariance;
        
        // Apply with tag system
        float tag = synapse->getEligibilityTrace();
        covarianceDelta *= (1.0f + tag * 0.5f); // Tags enhance covariance learning
        
        totalDelta += covarianceDelta;
        
        NLM_LOG_DEBUG("Hebbian covariance: cov=" + std::to_string(covariance) + ", Δw=" + std::to_string(covarianceDelta));
    }
    
    // 3. Cooperative and competitive learning
    // Cooperative: neighboring synapses strengthen together
    // Competitive: synapses compete for limited resources
    float cooperativeDelta = 0.0f;
    float competitiveDelta = 0.0f;
    
    // Cooperative component: correlated activity with recent learning history
    if (synapse->getEligibilityTrace() > 0.3f) {
        cooperativeDelta = pImpl->learningRate * pImpl->cooperativeFactor * totalDelta;
        totalDelta += cooperativeDelta;
        NLM_LOG_DEBUG("Hebbian cooperative learning: enhanced by tags");
    }
    
    // Competitive component: prevents runaway excitation
    float competitivePressure = calculateCompetitivePressure(synapse);
    if (competitivePressure > 0.0f) {
        competitiveDelta = -pImpl->learningRate * pImpl->competitiveFactor * competitivePressure;
        totalDelta += competitiveDelta;
        NLM_LOG_DEBUG("Hebbian competitive: pressure=" + std::to_string(competitivePressure) + ", Δw=" + std::to_string(competitiveDelta));
    }
    
    // 4. Homeostatic synaptic scaling
    // Scale all synapses to maintain target mean weight
    float currentWeight = synapse->getWeight();
    float weightDeviation = currentWeight - pImpl->scalingTargetMean;
    
    if (std::abs(weightDeviation) > 0.05f) { // Scaling threshold
        float scalingDelta = -pImpl->scalingStrength * weightDeviation;
        totalDelta += scalingDelta;
        
        NLM_LOG_DEBUG("Hebbian scaling: target=" + std::to_string(pImpl->scalingTargetMean) + 
                     ", weight=" + std::to_string(currentWeight) +
                     ", deviation=" + std::to_string(weightDeviation) +
                     ", Δw=" + std::to_string(scalingDelta));
    }
    
    // 5. Homeostatic firing rate regulation
    // Adjust synaptic weights to maintain target firing rate
    float activityDeviation = calculateActivityDeviation(preSpikes, postSpikes);
    if (std::abs(activityDeviation) > 1.0f) {
        float homeostaticDelta = pImpl->homeostaticRate * activityDeviation;
        totalDelta += homeostaticDelta;
        
        NLM_LOG_DEBUG("Hebbian homeostasis: targetRate=" + std::to_string(pImpl->homeostaticTarget) + 
                     ", activityDev=" + std::to_string(activityDeviation) +
                     ", Δw=" + std::to_string(homeostaticDelta));
    }
    
    // Apply all Hebbian components
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
        
        // Also update eligibility trace for reward integration
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.5f); // Partial trace update
        
        // Decay eligibility trace
        synapse->decayEligibilityTrace(pImpl->tagDecayRate);
        
        NLM_LOG_INFO("Hebbian learning applied: Δw=" + std::to_string(totalDelta) + 
                    ", weight=" + std::to_string(synapse->getWeight()) +
                    ", tag=" + std::to_string(synapse->getEligibilityTrace()));
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
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

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm

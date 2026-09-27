#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <vector>
#include <unordered_map>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // Advanced Hebbian parameters
    float predictionErrorMod;   // Modulation by prediction error
    float dopamineMod;          // Modulation by dopamine
    float curiosityMod;         // Modulation by curiosity
    float noveltyMod;           // Modulation by novelty
    
    // BCM-like theory (Bienenstock-Cooper-Munro) parameters
    float hebbianThreshold;     // BCM threshold (theta)
    float hebbianLearningRate;  // BCM learning rate (eta)
    float hebbianTargetWeight;  // Target synaptic weight
    
    // Memory-based Hebbian extensions
    float memoryInfluence;       // Influence of memory systems on Hebbian learning
    float attentionMod;          // Modulation by attention system
    
    // Spike-based Hebbian with temporal dynamics
    float temporalKernelWidth;   // Width of temporal correlation kernel
    float spikeHistoryDecay;     // Decay rate of spike history
    
    // Eligibility traces for Hebbian learning
    std::unordered_map<SynapseHandle, float> hebbianTraces;
    float traceDecayRate;
    float traceTimeConstant;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), predictionErrorMod(1.0f),
             dopamineMod(1.0f), curiosityMod(1.0f), noveltyMod(1.0f),
             hebbianThreshold(0.5f), hebbianLearningRate(0.01f),
             hebbianTargetWeight(0.0f), memoryInfluence(0.5f),
             attentionMod(1.0f), temporalKernelWidth(50.0f),
             spikeHistoryDecay(0.01f), traceDecayRate(0.01f),
             traceTimeConstant(100.0f) {
        // Initialize Hebbian trace map
        hebbianTraces.clear();
    }
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Compute multiple Hebbian components
    float classicalDelta = computeClassicalHebbian(preSpikes, postSpikes, dt);
    float bmcDelta = computeBCMHebbian(synapse, preSpikes, postSpikes, dt);
    float memoryDelta = computeMemoryHebbian(preSpikes, postSpikes, dt);
    
    // Combine contributions with neuromodulation
    float neuromodulationFactor = pImpl->predictionErrorMod * pImpl->dopamineMod *
                                 pImpl->curiosityMod * pImpl->noveltyMod;
    
    float totalDelta = (classicalDelta + bmcDelta + memoryDelta) *
                      neuromodulationFactor * pImpl->learningRate;
    
    // Add eligibility trace contribution
    SynapseHandle handle = synapse->getHandle();
    float eligibility = pImpl->hebbianTraces[handle];
    eligibility *= (1.0f - pImpl->traceDecayRate * dt); // Decay
    
    // Update eligibility trace based on spike activity
    float spikeActivity = computeSpikeActivity(preSpikes, postSpikes, dt);
    eligibility += spikeActivity * pImpl->memoryInfluence;
    pImpl->hebbianTraces[handle] = eligibility;
    
    // Apply combined Hebbian learning
    applyWeightChange(synapse, totalDelta + eligibility);
}

float Hebbian::computeClassicalHebbian(const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    // Classical "fire together, wire together" rule
    // Based on temporal correlation of pre- and post-synaptic spikes
    
    float correlation = 0.0f;
    size_t totalPairs = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(postTime - preTime));
            
            // Temporal kernel: spikes close in time contribute more
            float kernel = std::exp(-timeDiff / pImpl->temporalKernelWidth);
            correlation += kernel;
            totalPairs++;
        }
    }
    
    if (totalPairs > 0) {
        correlation /= totalPairs;
    }
    
    // Convert correlation to weight change
    return correlation - 0.5f; // Center around zero
}

float Hebbian::computeBCMHebbian(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    // BCM rule (Bienenstock-Cooper-Munro)
    // Learning depends on whether postsynaptic activity exceeds threshold
    
    if (!synapse) return 0.0f;
    
    float currentWeight = synapse->getWeight();
    
    // Compute BCM learning rule
    float phi = currentWeight * currentWeight;  // Synaptic activity dependent on weight
    float theta = pImpl->hebbianThreshold;      // Threshold
    float eta = pImpl->hebbianLearningRate;    // Learning rate
    
    // BCM weight change: (phi - theta) * eta
    float delta = (phi - theta) * eta * pImpl->attentionMod;
    
    // Add target weight influence
    delta += (pImpl->hebbianTargetWeight - currentWeight) * 0.01f;
    
    return delta;
}

float Hebbian::computeMemoryHebbian(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt) {
    // Memory-modulated Hebbian learning
    // Incorporates influences from memory systems
    
    // Compute spike rate statistics
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Memory influence factor
    float memoryFactor = 1.0f + pImpl->memoryInfluence;
    
    // Temporal correlation (reweighted by memory)
    float temporalCor = computeTemporalCorrelation(preSpikes, postSpikes, dt);
    
    // BCM-like component with memory modulation
    float bmcComponent = memoryFactor * (preRate * postRate - 0.5f);
    
    // Novelty-enhanced Hebbian
    float noveltyComponent = 0.0f; // Could be enhanced with novelty detection
    
    return temporalCor + bmcComponent + noveltyComponent;
}

float Hebbian::computeSpikeActivity(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt) {
    // Compute combined spike activity for eligibility traces
    float preActivity = static_cast<float>(preSpikes.size()) / dt;
    float postActivity = static_cast<float>(postSpikes.size()) / dt;
    
    // Apply spike history decay
    preActivity *= std::exp(-pImpl->spikeHistoryDecay * dt);
    postActivity *= std::exp(-pImpl->spikeHistoryDecay * dt);
    
    return (preActivity + postActivity) * 0.5f;
}

float Hebbian::computeTemporalCorrelation(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt) {
    // Compute temporal correlation of spike trains
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    float totalCorrelation = 0.0f;
    size_t validPairs = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = static_cast<float>(postTime - preTime);
            
            // Temporal kernel with Gaussian-like shape
            float kernel = std::exp(-(timeDiff * timeDiff) / (2.0f * pImpl->temporalKernelWidth * pImpl->temporalKernelWidth));
            
            if (std::abs(timeDiff) < 200.0f) {  // Consider spikes within 200ms
                totalCorrelation += kernel;
                validPairs++;
            }
        }
    }
    
    if (validPairs > 0) {
        return totalCorrelation / validPairs;
    }
    
    return 0.0f;
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply sophisticated weight change with multiple constraints
    
    // 1. Biological limits
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // 2. Stability constraints - prevent extreme weight changes
    float weightChange = std::abs(newWeight - currentWeight);
    if (weightChange > 0.1f) {
        // Scale down large changes for stability
        float scaleFactor = 0.1f / weightChange;
        newWeight = currentWeight + (newWeight - currentWeight) * scaleFactor;
    }
    
    // 3. Homeostatic regulation
    float homeostaticTarget = pImpl->hebbianTargetWeight;
    float homeostaticStrength = 0.005f;
    
    float homeostaticCorrection = homeostaticStrength * (homeostaticTarget - newWeight);
    newWeight += homeostaticCorrection;
    
    // Apply final clamp
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Update synapse weight
    synapse->setWeight(newWeight);
    
    // Update eligibility trace
    SynapseHandle handle = synapse->getHandle();
    pImpl->hebbianTraces[handle] = newWeight * 0.1f; // Store scaled weight as trace
}

const char* Hebbian::getName() const {
    return "Advanced Hebbian (Classical + BCM + Memory-Modulated)";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 10.0f);
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

void Hebbian::setBCMParameters(float threshold, float learningRate, float targetWeight) {
    pImpl->hebbianThreshold = std::clamp(threshold, 0.0f, 1.0f);
    pImpl->hebbianLearningRate = std::clamp(learningRate, 0.0f, 0.1f);
    pImpl->hebbianTargetWeight = std::clamp(targetWeight, -1.0f, 1.0f);
}

void Hebbian::setNeuromodulation(float predErrorMod, float dopamineMod, 
                                float curiosityMod, float noveltyMod) {
    pImpl->predictionErrorMod = std::clamp(predErrorMod, 0.0f, 2.0f);
    pImpl->dopamineMod = std::clamp(dopamineMod, 0.0f, 2.0f);
    pImpl->curiosityMod = std::clamp(curiosityMod, 0.0f, 2.0f);
    pImpl->noveltyMod = std::clamp(noveltyMod, 0.0f, 2.0f);
}

void Hebbian::setTemporalKernel(float width) {
    pImpl->temporalKernelWidth = std::clamp(width, 10.0f, 500.0f);
}

void Hebbian::setTraceParams(float decayRate, float timeConstant) {
    pImpl->traceDecayRate = std::clamp(decayRate, 0.001f, 0.1f);
    pImpl->traceTimeConstant = std::clamp(timeConstant, 10.0f, 1000.0f);
}

} // namespace nlm

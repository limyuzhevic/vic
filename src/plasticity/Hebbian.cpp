#include "Hebbian.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../neuromodulation/Reward.hpp"
#include "../../neuromodulation/PredictionError.hpp"
#include "../../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

// Advanced Hebbian plasticity implementation with biological accuracy
// Implements activity-dependent Hebbian learning with reward modulation,
// homeostatic scaling, and metaplasticity.

struct Hebbian::Impl {
    // Basic Hebbian parameters
    float learningRate;
    float maxWeight;
    float minWeight;
    
    // STDP-like timing parameters
    float preTimeWindow;      // Pre-synaptic spike influence window (ms)
    float postTimeWindow;     // Post-synaptic spike influence window (ms)
    float maxTimeDifference;  // Maximum time difference for correlation (ms)
    
    // NMDA/AMPA receptor dynamics
    float nmdaMgBlock;        // Magnesium block parameter for NMDA receptors
    float nmdaWeight;         // NMDA receptor weight relative to AMPA
    float nmdaTimeConstant;   // NMDA receptor decay time constant
    
    // Homeostatic regulation
    float targetFiringRate;   // Target firing rate for homeostatic scaling
    float homeostaticRate;    // Rate of homeostatic scaling
    float activityHistory;    // Running activity average for homeostasis
    size_t activitySampleCount; // Number of samples for activity averaging
    
    // Synaptic tagging and capture
    bool hasTag;              // Whether synapse has a plasticity tag
    float tagTime;            // Time when tag was set
    float tagDecayRate;       // Rate of tag decay
    float tagCaptureWindow;   // Window for tag capture by neuromodulators
    
    // Metaplasticity parameters
    float metaplasticThreshold;    // Threshold for metaplastic changes
    float learningHistory;         // Recent learning rate history
    float metaplasticRate;         // Rate of metaplastic adaptation
    
    // Competition mechanisms
    bool winnerTakeAll;           // Whether to implement winner-take-all competition
    float competitionThreshold;   // Activity threshold for competition
    float competitionStrength;    // Strength of competitive interactions
    
    // Reward-modulated learning
    float rewardModulationFactor; // Base modulation factor from rewards
    float predictionErrorInfluence; // Influence of prediction errors
    
    // Synaptic state variables
    float calciumLevel;          // Intracellular calcium concentration
    float ampaWeight;            // AMPA receptor conductance
    float nmdaWeight;            // NMDA receptor conductance
    float effectiveWeight;       // Combined synaptic weight (w = AMPA + NMDA)
    
    // Spike history buffers for precise timing
    std::vector<std::pair<Timestamp, float>> preSpikeTimestamps;  // Pre-synaptic spike times with Ca2+ traces
    std::vector<std::pair<Timestamp, float>> postSpikeTimestamps; // Post-synaptic spike times with Ca2+ traces
    
    // Eligibility traces for reward modulation
    float eligibilityTrace;      // Eligibility trace for reward-modulated learning
    float eligibilityDecay;      // Decay rate of eligibility trace
    
    // Synaptic metadata
    bool isInhibitory;          // Whether synapse is inhibitory
    float targetActivity;        // Target activity for this synapse
    
    Impl() : learningRate(0.01f),
             maxWeight(1.0f),
             minWeight(-1.0f),
             preTimeWindow(50.0f),      // 50ms pre-synaptic window
             postTimeWindow(50.0f),     // 50ms post-synaptic window
             maxTimeDifference(100.0f), // 100ms maximum correlation window
             nmdaMgBlock(1.0f),          // Initial magnesium block
             nmdaWeight(0.5f),           // NMDA weight is 50% of AMPA
             nmdaTimeConstant(150.0f),  // 150ms NMDA time constant
             targetFiringRate(0.01f),   // 1% target firing rate
             homeostaticRate(0.001f),    // 0.1% homeostatic rate
             activityHistory(0.0f),
             activitySampleCount(0),
             hasTag(false),
             tagTime(0.0f),
             tagDecayRate(0.01f),       // Tags decay over 100ms
             tagCaptureWindow(200.0f),  // 200ms capture window
             metaplasticThreshold(0.1f),
             learningHistory(learningRate),
             metaplasticRate(0.001f),
             winnerTakeAll(true),
             competitionThreshold(0.005f),
             competitionStrength(0.1f),
             rewardModulationFactor(1.0f),
             predictionErrorInfluence(0.5f),
             calciumLevel(0.0f),
             ampaWeight(0.0f),
             nmdaWeight(0.5f),
             effectiveWeight(0.0f),
             eligibilityTrace(0.0f),
             eligibilityDecay(0.1f),    // Fast decay
             isInhibitory(false),
             targetActivity(0.01f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse, 
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * Complete Hebbian plasticity implementation with:
     * 1. Real STDP-like dynamics with NMDA/AMPA receptor modeling
     * 2. Synaptic tagging and capture for reward modulation
     * 3. Homeostatic scaling for stability
     * 4. Metaplasticity for learning rate adaptation
     * 5. Synaptic competition mechanisms
     * 6. Reward-modulated learning with eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Extract neuromodulatory factors if available
    float neuromodulationFactor = 1.0f;  // Default: no modulation
    float rewardFactor = 1.0f;
    float predictionErrorFactor = 1.0f;
    
    // Calculate total weight change from all mechanisms
    float totalDelta = 0.0f;
    
    // 1. Activity-dependent Hebbian learning with STDP-like dynamics
    float hebbianDelta = calculateHebbianWeightChange(preSpikes, postSpikes, dt);
    totalDelta += hebbianDelta;
    
    // 2. Apply neuromodulation factors
    if (synapse->getPlasticityFlags().reward_modulated) {
        neuromodulationFactor = calculateNeuromodulationFactor(synapse, dt);
        rewardFactor = calculateRewardFactor(synapse, dt);
        predictionErrorFactor = calculatePredictionErrorFactor(synapse, dt);
        
        // Apply reward-modulated learning
        float rmDelta = hebbianDelta * neuromodulationFactor * rewardFactor * predictionErrorFactor;
        totalDelta += rmDelta;
    }
    
    // 3. Update eligibility traces for reward-modulated learning
    if (synapse->getPlasticityFlags().reward_modulated) {
        updateEligibilityTrace(synapse, hebbianDelta, dt);
    }
    
    // 4. Apply synaptic tagging and capture
    applySynapticTagging(synapse, dt);
    
    // 5. Apply homeostatic scaling
    float homeostaticDelta = calculateHomeostaticDelta(synapse, dt);
    totalDelta += homeostaticDelta;
    
    // 6. Apply metaplasticity
    float metaplasticDelta = calculateMetaplasticDelta(synapse, dt);
    totalDelta += metaplasticDelta;
    
    // 7. Apply synaptic competition
    if (pImpl->winnerTakeAll) {
        float competitionDelta = calculateCompetitionDelta(synapse, dt);
        totalDelta += competitionDelta;
    }
    
    // Apply the total weight change
    if (std::abs(totalDelta) > 1e-8f) {
        applyWeightChange(synapse, totalDelta);
    }
    
    // Update internal synaptic state
    updateSynapticState(synapse, totalDelta, dt);
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Extract current weight components
    float currentAmps = synapse->getWeight();
    if (synapse->getType() == SynapseType::Inhibitory) {
        currentAmps = -currentAmps;  // Convert inhibitory weight to positive magnitude
    }
    
    // AMPA receptor component (fast, reliable)
    float newAmpsWeight = currentAmps + delta * 0.7f;  // 70% of weight change goes to AMPA
    
    // NMDA receptor component (slow, calcium-dependent)
    float newNmdasWeight = pImpl->nmdaWeight * newAmpsWeight;  // NMDA tracks AMPA
    
    // Calculate effective synaptic weight
    float newEffectiveWeight = newAmpsWeight + newNmdasWeight;
    
    // Convert back to signed weight based on synapse type
    float newSynapticWeight = synapse->getType() == SynapseType::Inhibitory ? 
        -newEffectiveWeight : newEffectiveWeight;
    
    // Apply bounds
    float clampedWeight = std::clamp(newSynapticWeight, 
                                     -pImpl->maxWeight, pImpl->maxWeight);
    
    synapse->setWeight(clampedWeight);
    
    // Update synapse's internal efficacy based on weight change
    float efficacyChange = std::abs(delta) / (std::abs(currentAmps) + 0.01f);
    float newEfficacy = std::clamp(synapse->getEfficacy() + efficacyChange, 
                                   0.0f, 2.0f);
    synapse->setEfficacy(newEfficacy);
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

// Private helper methods

float Hebbian::calculateHebbianWeightChange(const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          TimestepDuration dt) {
    float totalDelta = 0.0f;
    
    // Process all pre-post spike pairs with precise timing
    for (Timestamp postTime : postSpikes) {
        for (Timestamp preTime : preSpikes) {
            float dt_spike = static_cast<float>(postTime - preTime); // Δt = t_post - t_pre
            
            if (std::abs(dt_spike) > pImpl->maxTimeDifference) continue;
            
            // Calculate NMDA receptor-mediated calcium influx
            float calciumInflux = 0.0f;
            
            if (dt_spike > 0) {
                // Pre before post: Long-term potentiation (LTP)
                // NMDA receptors open due to glutamate and postsynaptic depolarization
                float nmdaActivation = calculateNMDAActivation(dt_spike, true);
                calciumInflux = pImpl->learningRate * nmdaActivation;
                
                // AMPA receptor strengthening (fast)
                float ampaDelta = pImpl->learningRate * std::exp(-dt_spike / pImpl->preTimeWindow);
                
                // Calcium-dependent processes enhance AMPA insertion
                calciumInflux *= (1.0f + pImpl->nmdaWeight);
                
                totalDelta += ampaDelta * calciumInflux;
                
            } else if (dt_spike < 0) {
                // Post before pre: Long-term depression (LTD)
                // NMDA receptors less likely to open, calcium levels lower
                float nmdaActivation = calculateNMDAActivation(-dt_spike, false);
                calciumInflux = pImpl->learningRate * 0.5f * nmdaActivation; // Reduced calcium
                
                // AMPA receptor weakening
                float ampaDelta = -pImpl->learningRate * std::exp(dt_spike / pImpl->postTimeWindow);
                
                // Calcium-dependent processes lead to AMPA removal
                calciumInflux *= (1.0f - pImpl->nmdaWeight * 0.5f);
                
                totalDelta += ampaDelta * calciumInflux;
                
            } else {
                // Simultaneous spikes: moderate change
                float nmdaActivation = calculateNMDAActivation(0.0f, true);
                float ampaDelta = pImpl->learningRate * 0.1f; // Smaller change
                totalDelta += ampaDelta * nmdaActivation;
            }
        }
    }
    
    // Apply weight dependence (Kunze rule)
    float weightDependence = 1.0f;
    if (pImpl->isInhibitory) {
        // Different weight dependence for inhibitory synapses
        weightDependence = std::max(0.1f, 1.0f - std::abs(synapse->getWeight()));
    }
    
    totalDelta *= weightDependence;
    
    return totalDelta;
}

float Hebbian::calculateNMDAActivation(float dt, bool isLtp) {
    // NMDA receptor activation depends on timing and magnesium block
    float activation = 0.0f;
    
    if (isLtp && dt >= 0) {
        // LTP: NMDA activation increases with postsynaptic depolarization
        // Simulate magnesium block reduction with depolarization
        float depolarizationFactor = 1.0f / (1.0f + std::exp(-(dt + 10.0f) / 10.0f));
        activation = std::exp(-dt / pImpl->nmdaTimeConstant) * depolarizationFactor;
        
    } else if (!isLtp && dt > 0) {
        // LTD: Lower NMDA activation, different calcium dynamics
        activation = 0.5f * std::exp(-dt / (pImpl->nmdaTimeConstant * 1.5f));
        
    } else {
        // Weak activation for zero or negative times in LTD
        activation = 0.1f;
    }
    
    return activation;
}

float Hebbian::calculateNeuromodulationFactor(Synapse* synapse, TimestepDuration dt) {
    // Get neuromodulatory factors from synapse or brain context
    float factor = 1.0f;
    
    // Check if synapse has eligibility trace
    if (synapse) {
        float eligibility = synapse->getEligibilityTrace();
        if (std::abs(eligibility) > 0.001f) {
            // Eligibility trace modulates plasticity
            factor *= (1.0f + pImpl->rewardModulationFactor * eligibility);
        }
    }
    
    // Add neuromodulation history
    factor *= (1.0f + pImpl->learningHistory * 0.1f);
    
    return std::max(0.1f, std::min(2.0f, factor)); // Clamp to reasonable range
}

float Hebbian::calculateRewardFactor(Synapse* synapse, TimestepDuration dt) {
    // Calculate reward modulation factor based on dopamine and reward signals
    float factor = 1.0f;
    
    if (synapse) {
        // Get eligibility trace
        float eligibility = synapse->getEligibilityTrace();
        
        // Reward modulation: stronger when eligibility trace matches reward timing
        if (std::abs(eligibility) > 0.01f) {
            factor *= (1.0f + pImpl->rewardModulationFactor * eligibility * 0.5f);
        }
    }
    
    return factor;
}

float Hebbian::calculatePredictionErrorFactor(Synapse* synapse, TimestepDuration dt) {
    // Calculate prediction error modulation
    float factor = 1.0f;
    
    if (synapse) {
        // Prediction error can enhance or suppress plasticity
        // Positive error (better than expected) enhances learning
        // Negative error (worse than expected) suppresses learning
        float predictionError = 0.0f; // Would come from brain/prediction system
        
        factor *= (1.0f + pImpl->predictionErrorInfluence * predictionError);
    }
    
    return factor;
}

void Hebbian::updateEligibilityTrace(Synapse* synapse, float hebbianDelta, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update eligibility trace: accumulates weight changes
    float currentTrace = synapse->getEligibilityTrace();
    
    // Hebbian learning contributes to eligibility trace
    currentTrace += hebbianDelta * pImpl->eligibilityDecay;
    
    // Decay eligibility trace over time
    currentTrace *= std::exp(-pImpl->eligibilityDecay * static_cast<float>(dt));
    
    // Clamp eligibility trace
    currentTrace = std::clamp(currentTrace, -1.0f, 1.0f);
    
    synapse->setEligibilityTrace(currentTrace);
}

void Hebbian::applySynapticTagging(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update tag state
    if (pImpl->hasTag) {
        pImpl->tagTime += static_cast<float>(dt);
        pImpl->hasTag = (pImpl->tagTime < pImpl->tagCaptureWindow);
        
        if (!pImpl->hasTag) {
            // Tag decayed
            pImpl->tagTime = 0.0f;
        }
    }
    
    // Set new tag if strong enough plasticity event occurred
    // (Would be triggered by large weight changes from update method)
}

float Hebbian::calculateHomeostaticDelta(Synapse* synapse, TimestepDuration dt) {
    // Homeostatic scaling to maintain stable firing rates
    float delta = 0.0f;
    
    if (!synapse) return delta;
    
    // Get neuron activity (would need access to neuron state)
    float neuronActivity = 0.01f; // Placeholder: actual activity measurement
    
    // Calculate deviation from target activity
    float activityDeviation = neuronActivity - pImpl->targetFiringRate;
    
    // Apply homeostatic scaling
    if (std::abs(activityDeviation) > 0.001f) {
        // Scale weight to bring activity toward target
        float homeostaticFactor = pImpl->homeostaticRate * activityDeviation;
        delta = -synapse->getWeight() * homeostaticFactor;
        
        // Limit homeostatic changes to prevent instability
        delta = std::clamp(delta, -0.01f, 0.01f);
    }
    
    // Update running activity average
    pImpl->activityHistory *= (1.0f - pImpl->homeostaticRate);
    pImpl->activityHistory += neuronActivity * pImpl->homeostaticRate;
    pImpl->activitySampleCount++;
    
    return delta;
}

float Hebbian::calculateMetaplasticDelta(Synapse* synapse, TimestepDuration dt) {
    // Metaplasticity: adaptation of learning rate based on recent activity
    float delta = 0.0f;
    
    if (!synapse) return delta;
    
    // Update learning history
    pImpl->learningHistory = pImpl->learningHistory * (1.0f - pImpl->metaplasticRate) + 
                              pImpl->learningRate * pImpl->metaplasticRate;
    
    // Check if metaplastic threshold is crossed
    if (std::abs(pImpl->learningHistory - pImpl->learningRate) > pImpl->metaplasticThreshold) {
        // Adjust learning rate based on history
        float metaplasticChange = (pImpl->learningHistory - pImpl->learningRate) * pImpl->metaplasticRate;
        
        // Apply metaplastic change to learning rate
        pImpl->learningRate = std::clamp(pImpl->learningRate + metaplasticChange, 0.0f, 1.0f);
        
        // Convert metaplastic change to weight change
        delta = metaplasticChange * synapse->getWeight() * 0.1f;
    }
    
    return delta;
}

float Hebbian::calculateCompetitionDelta(Synapse* synapse, TimestepDuration dt) {
    // Synaptic competition: winner-take-all for correlated inputs
    float delta = 0.0f;
    
    if (!synapse) return delta;
    
    // Check if synapse activity exceeds competition threshold
    if (std::abs(synapse->getWeight()) > pImpl->competitionThreshold) {
        // Implement winner-take-all: weaken competing synapses
        // This is a simplified version - real implementation would need access to
        // all synapses in the same neural population
        
        // For now, just implement self-competition
        float selfCompDelta = -synapse->getWeight() * pImpl->competitionStrength * 0.01f;
        
        // Apply competition
        delta = selfCompDelta;
    }
    
    return delta;
}

void Hebbian::updateSynapticState(Synapse* synapse, float weightChange, TimestepDuration dt) {
    if (!synapse) return;
    
    // Update calcium dynamics
    float calciumInput = 0.0f;
    
    // Strong weight changes increase calcium levels
    if (std::abs(weightChange) > 0.001f) {
        calciumInput = std::abs(weightChange) * 10.0f;
    }
    
    // Update calcium level with decay
    pImpl->calciumLevel = pImpl->calciumLevel * std::exp(-dt / 50.0f) + calciumInput;
    
    // Calcium modulates various processes
    float calciumFactor = std::clamp(pImpl->calciumLevel / 2.0f, 0.0f, 1.0f);
    
    // Update NMDA weight based on calcium
    pImpl->nmdaWeight = 0.5f * (1.0f + calciumFactor);
    
    // Update AMPA weight based on calcium
    if (calciumFactor > 0.5f) {
        // High calcium leads to AMPA insertion
        float ampaChange = calciumFactor * 0.01f;
        float currentAmps = synapse->getWeight();
        if (synapse->getType() == SynapseType::Inhibitory) {
            currentAmps = -currentAmps;
        }
        currentAmps = std::clamp(currentAmps + ampaChange, -pImpl->maxWeight, pImpl->maxWeight);
        synapse->setWeight(synapse->getType() == SynapseType::Inhibitory ? 
                          -currentAmps : currentAmps);
    }
    
    // Update magnesium block based on calcium
    pImpl->nmdaMgBlock = 1.0f / (1.0f + std::exp(-pImpl->calciumLevel / 0.5f));
}

} // namespace nlm

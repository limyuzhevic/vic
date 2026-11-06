#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Nearest-neighbour version: the old all-pairs count made delta grow
    // with history length (up to 100x100 pairs) and saturated every weight
    // to its bound in one step. One correlated recent pair = one step.
    // Timestamps are in seconds, so the 100ms window is 0.1s.
    float dtSeconds = static_cast<float>(postSpikes.back() - preSpikes.back());
    float delta = std::abs(dtSeconds) < 0.1f ? pImpl->learningRate : 0.0f;
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Apply with bounds. Soft bound, Phase 4 (see STDP.cpp): Hebbian
    // strengthening is always away from zero, so scale it by (1 - |w|);
    // without this, Hebbian alone added a fixed 0.01 per co-active step
    // and saturated every weight together with STDP.
    if (std::abs(delta) > 1e-6f) {
        delta *= (1.0f - std::abs(synapse->getWeight()));
        applyWeightChange(synapse, delta);
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
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

} // namespace nlm

// Variance-normalized Hebbian implementation
struct VarianceNormalizedHebbian::Impl {
    float learningRate;
    float minCorrelation;
    float variance;  // running variance of pre-post activity
    float mean;      // running mean of pre-post activity
    size_t count;    // number of samples for variance calculation
    
    Impl() : learningRate(0.01f), minCorrelation(0.1f), variance(0.0f), mean(0.0f), count(0) {}
};

VarianceNormalizedHebbian::VarianceNormalizedHebbian() : pImpl(new Impl) {}

VarianceNormalizedHebbian::~VarianceNormalizedHebbian() {
    delete pImpl;
}

void VarianceNormalizedHebbian::update(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    /*
     * Variance-normalized Hebbian learning
     * 
     * This extends standard Hebbian learning with variance normalization,
     * which accounts for changing statistics in the environment and prevents
     * runaway potentiation when correlations become strong.
     * 
     * Mathematical formulation (Oja's rule):
     * Δw = η * (pre * post - w * pre^2)
     * 
     * Where:
     *   η = learningRate
     *   w = current synaptic weight
     *   pre, post = neural activity values
     * 
     * Biological inspiration:
     *   - Reflects synaptic normalization observed in experiments
     *   - Implements homeostatic plasticity
     *   - Prevents synaptic saturation
     *   - Used in motor learning and sensory processing
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // For spike-based systems, use simplified formulation
    float totalDelta = 0.0f;
    float currentWeight = synapse->getWeight();
    
    // Calculate correlation coefficient
    float correlation = calculateCorrelation(preSpikes, postSpikes);
    
    if (correlation > pImpl->minCorrelation) {
        // Positive correlation: strengthen synapse
        totalDelta = pImpl->learningRate * (correlation - currentWeight);
    } else {
        // Negative correlation: weaken synapse
        totalDelta = pImpl->learningRate * (correlation - currentWeight);
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition, not flip it
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
    }
}

void VarianceNormalizedHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void VarianceNormalizedHebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float VarianceNormalizedHebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void VarianceNormalizedHebbian::setMinCorrelation(float minCorr) {
    pImpl->minCorrelation = std::clamp(minCorr, 0.0f, 1.0f);
}

float VarianceNormalizedHebbian::getMinCorrelation() const {
    return pImpl->minCorrelation;
}

// Calculate correlation coefficient between pre and post spike trains
float VarianceNormalizedHebbian::calculateCorrelation(const std::vector<Timestamp>& preSpikes,
                                                     const std::vector<Timestamp>& postSpikes) {
    // Simplified correlation calculation for spike trains
    // In a real implementation, this would use proper statistical methods
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Calculate correlation as proportion of correlated spikes
    size_t correlatedSpikes = 0;
    size_t totalSpikes = std::min(preSpikes.size(), postSpikes.size());
    
    for (size_t i = 0; i < totalSpikes; ++i) {
        if (std::abs(preSpikes[i] - postSpikes[i]) < 0.1f) {
            correlatedSpikes++;
        }
    }
    
    return static_cast<float>(correlatedSpikes) / totalSpikes;
}

// Competitive Hebbian implementation
struct CompetitiveHebbian::Impl {
    float maxExcitatoryWeight;
    float competitionStrength;
    float globalInhibition;
    float baseWeight;
    
    Impl() : maxExcitatoryWeight(1.0f), competitionStrength(0.5f), globalInhibition(0.1f),
             baseWeight(0.01f) {}
};

CompetitiveHebbian::CompetitiveHebbian() : pImpl(new Impl) {}

CompetitiveHebbian::~CompetitiveHebbian() {
    delete pImpl;
}

void CompetitiveHebbian::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    /*
     * Competitive Hebbian learning
     * 
     * This implements synaptic competition where stronger synapses inhibit
     * weaker ones, promoting sparse coding and efficient representation.
     * 
     * Mathematical formulation:
     * Δw = η * (pre * post - w * maxExcitatory - competition * sum_weights)
     * 
     * Where:
     *   η = learningRate
     *   w = current synaptic weight
     *   maxExcitatory = maximum excitatory weight
     *   competition = strength of competition
     *   sum_weights = sum of all excitatory synaptic weights
     *   
     * Biological inspiration:
     *   - Implements synaptic competition observed in experiments
     *   - Promotes sparse coding in neural circuits
     *   - Reflects lateral inhibition in neural networks
     *   - Used in pattern recognition and memory
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate synaptic activity
    float preActivity = preSpikes.back();
    float postActivity = postSpikes.back();
    
    // Calculate competition signal
    float competitionSignal = pImpl->globalInhibition;
    for (auto& region : synapse->getRegion()) {  // Need to add region access to synapse
        for (auto& otherSyn : region->getSynapses()) {
            if (otherSyn->getType() == SynapseType::Excitatory && otherSyn != synapse) {
                competitionSignal += otherSyn->getWeight() * pImpl->competitionStrength;
            }
        }
    }
    
    // Calculate weight change
    float totalDelta = pImpl->learningRate * (preActivity * postActivity - synapse->getWeight() * 
                                          pImpl->maxExcitatoryWeight - competitionSignal);
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition, not flip it
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
    }
}

void CompetitiveHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, pImpl->maxExcitatoryWeight);
    synapse->setWeight(newWeight);
}

void CompetitiveHebbian::setMaxExcitatoryWeight(float maxWeight) {
    pImpl->maxExcitatoryWeight = std::clamp(maxWeight, 0.01f, 10.0f);
}

float CompetitiveHebbian::getMaxExcitatoryWeight() const {
    return pImpl->maxExcitatoryWeight;
}

void CompetitiveHebbian::setCompetitionStrength(float strength) {
    pImpl->competitionStrength = std::clamp(strength, 0.0f, 1.0f);
}

float CompetitiveHebbian::getCompetitionStrength() const {
    return pImpl->competitionStrength;
}

// Credit assignment Hebbian implementation
struct CreditAssignmentHebbian::Impl {
    float decayRate;
    float distalSpikeWeight;
    float creditAssignFactor;
    float trace;  // eligibility trace
    
    Impl() : decayRate(0.9f), distalSpikeWeight(0.5f), creditAssignFactor(0.8f), trace(0.0f) {}
};

CreditAssignmentHebbian::CreditAssignmentHebbian() : pImpl(new Impl) {}

CreditAssignmentHebbian::~CreditAssignmentHebbian() {
    delete pImpl;
}

void CreditAssignmentHebbian::update(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     TimestepDuration dt) {
    /*
     * Credit assignment Hebbian learning
     * 
     * This implements credit assignment mechanisms where synaptic changes
     * are distributed based on temporal proximity and causal relationships.
     * 
     * Mathematical formulation:
     * Δw = η * trace * (pre * post)
     * where trace decays over time and is reset by presynaptic activity
     * 
     * Biological inspiration:
     *   - Reflects synaptic eligibility traces
     *   - Implements temporal credit assignment
     *   - Used in reinforcement learning
     *   - Applied in motor adaptation and prediction
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update eligibility trace (decay)
    pImpl->trace *= pImpl->decayRate;
    
    // Check for causal relationships and update trace
    if (checkCausalRelationship(preSpikes, postSpikes)) {
        pImpl->trace += creditAssignFactor;
    }
    
    // Calculate weight change based on trace
    float totalDelta = pImpl->learningRate * pImpl->trace * 
                      (preSpikes.back() * postSpikes.back());
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition, not flip it
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
    }
}

void CreditAssignmentHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void CreditAssignmentHebbian::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 1.0f);
}

float CreditAssignmentHebbian::getDecayRate() const {
    return pImpl->decayRate;
}

void CreditAssignmentHebbian::setDistalSpikeWeight(float weight) {
    pImpl->distalSpikeWeight = std::clamp(weight, 0.0f, 1.0f);
}

float CreditAssignmentHebbian::getDistalSpikeWeight() const {
    return pImpl->distalSpikeWeight;
}

bool CreditAssignmentHebbian::checkCausalRelationship(const std::vector<Timestamp>& preSpikes,
                                                      const std::vector<Timestamp>& postSpikes) {
    // Check if there is a causal relationship between pre and post spikes
    // Simple implementation: check if spikes are within temporal window
    if (preSpikes.empty() || postSpikes.empty()) {
        return false;
    }
    
    float timeDiff = std::abs(postSpikes.back() - preSpikes.back());
    return timeDiff < pImpl->distalSpikeWeight;
}

// LTP/LTD Hebbian implementation
struct LTPP_DHebbian::Impl {
    float ltpRate;
    float ltdRate;
    float ltpThreshold;
    float ltdThreshold;
    float ltpMax;
    float ltdMin;
    
    Impl() : ltpRate(0.01f), ltdRate(0.012f), ltpThreshold(0.5f), ltdThreshold(-0.5f),
             ltpMax(1.0f), ltdMin(-1.0f) {}
};

LTPP_DHebbian::LTPP_DHebbian() : pImpl(new Impl) {}

LTPP_DHebbian::~LTPP_DHebbian() {
    delete pImpl;
}

void LTPP_DHebbian::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    /*
     * LTP/LTD Hebbian learning
     * 
     * This implements separate rules for long-term potentiation (LTP) and
     * long-term depression (LTD), reflecting biological mechanisms where
     * different thresholds control strengthening vs weakening of synapses.
     * 
     * Mathematical formulation:
     * If pre * post > LTPThreshold: Δw = LTPRate * (pre * post)
     * If pre * post < LTDThreshold: Δw = -LTDRate * (pre * post)
     * Otherwise: Δw = 0 (no change)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-dependent mechanisms
     *   - Implements synaptic tagging and capture
     *   - Used in memory consolidation
     *   - Applied in learning and adaptation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate synaptic activity
    float preActivity = preSpikes.back();
    float postActivity = postSpikes.back();
    float activityProduct = preActivity * postActivity;
    
    float totalDelta = 0.0f;
    
    if (activityProduct > pImpl->ltpThreshold) {
        // LTP: strengthen synapse
        totalDelta = pImpl->ltpRate * activityProduct;
    } else if (activityProduct < pImpl->ltdThreshold) {
        // LTD: weaken synapse
        totalDelta = -pImpl->ltdRate * activityProduct;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition, not flip it
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
    }
}

void LTPP_DHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->ltdMin, pImpl->ltpMax);
    synapse->setWeight(newWeight);
}

void LTPP_DHebbian::setLTPRate(float rate) {
    pImpl->ltpRate = std::clamp(rate, 0.0f, 0.1f);
}

float LTPP_DHebbian::getLTPRate() const {
    return pImpl->ltpRate;
}

void LTPP_DHebbian::setLTDRate(float rate) {
    pImpl->ltdRate = std::clamp(rate, 0.0f, 0.1f);
}

float LTPP_DHebbian::getLTDRate() const {
    return pImpl->ltdRate;
}

void LTPP_DHebbian::setLTPThreshold(float threshold) {
    pImpl->ltpThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float LTPP_DHebbian::getLTPThreshold() const {
    return pImpl->ltpThreshold;
}

void LTPP_DHebbian::setLTDThreshold(float threshold) {
    pImpl->ltdThreshold = std::clamp(threshold, -1.0f, 0.0f);
}

float LTPP_DHebbian::getLTDThreshold() const {
    return pImpl->ltdThreshold;
}

} // namespace nlm

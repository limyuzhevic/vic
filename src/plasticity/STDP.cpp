#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() {
    delete pImpl;
}

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Limitations:
     *   - Simplified pairwise rule (doesn't capture triplet interactions)
     *   - Assumes single exponential window (more complex in biology)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;

    // Nearest-neighbour STDP: only the most recent pre/post pair is used.
    // The old all-pairs loop re-applied every historic pair on every step,
    // so weights saturated to their bounds in a few steps and the cost was
    // O(history^2) per synapse per step. Timestamps here are in seconds
    // (Brain steps at 1ms), so convert to ms for the tau window.
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    if (dtMs > 0) {
        totalDelta += pImpl->ltpWeight * std::exp(-dtMs / tau);
    } else if (dtMs < 0) {
        totalDelta += -pImpl->ltdWeight * std::exp(dtMs / tau);
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;

        // Inhibitory plasticity must strengthen inhibition, not flip it:
        // pre-before-post on an inhibitory synapse should make its weight
        // more negative, not positive. Without this, every inhibitory
        // synapse drifted through zero and became excitatory.
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }

        // Soft (weight-dependent) bounds, Phase 4: the old rule added a
        // fixed ~0.01 to every co-active synapse on every step, so with
        // STDP and Hebbian both firing, weights ran from 0.2 to the 1.0
        // clamp in ~40 steps and every synapse saturated together — no
        // pattern could stay selective. Potentiation (away from zero)
        // now scales by (1 - |w|) and depression (toward zero) by |w|,
        // so weights approach their bounds asymptotically, increments
        // shrink as a synapse saturates, and relative differences between
        // synapses are preserved instead of all being clamped equal.
        {
            float w = synapse->getWeight();
            bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
            totalDelta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
        }

        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01.
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
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

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Reward-modulated STDP implementation
struct RewardModulatedSTDP::Impl {
    float predictionError;
    float baseLtpWeight;
    float baseLtdWeight;
    float timeConstant;
    
    Impl() : predictionError(0.0f), baseLtpWeight(0.01f), baseLtdWeight(0.012f), timeConstant(20.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() {
    delete pImpl;
}

void RewardModulatedSTDP::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    /*
     * Reward-modulated STDP (R-STDP)
     * 
     * R-STDP combines spike timing with reward prediction error
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * Δw = A+ * δ * exp(-|Δt| / τ) for Δt > 0 (pre before post)
     * Δw = A- * δ * exp(-|Δt| / τ) for Δt < 0 (post before pre)
     * 
     * Where:
     *   A+ = baseLtpWeight * (1 + δ) for potentiation
     *   A- = baseLtdWeight * (1 + δ) for depression
     *   δ = predictionError (reward prediction error)
     *   τ = timeConstant
     * 
     * Biological inspiration:
     *   - Combines STDP with reinforcement learning
     *   - Reflects dopamine-modulated plasticity
     *   - Implements reward prediction error signal
     *   - Used in motor learning and habit formation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float predictionError = pImpl->predictionError;
    
    // Nearest-neighbour STDP: only the most recent pre/post pair is used
    float dtSeconds = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    if (dtSeconds > 0) {
        // Potentiation: compute weight change scaled by prediction error
        float ltpWeight = pImpl->baseLtpWeight * (1.0f + predictionError);
        totalDelta += ltpWeight * std::exp(-dtSeconds / pImpl->timeConstant);
    } else if (dtSeconds < 0) {
        // Depression: compute weight change scaled by prediction error
        float ltdWeight = pImpl->baseLtdWeight * (1.0f + predictionError);
        totalDelta += -ltdWeight * std::exp(dtSeconds / pImpl->timeConstant);
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

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = std::clamp(error, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getPredictionError() const {
    return pImpl->predictionError;
}

// Triplet STDP implementation
struct TripletSTDP::Impl {
    float t1;      // First triplet time window (ms)
    float t2;      // Second triplet time window (ms)
    float t3;      // Third triplet time window (ms)
    float ltpWeight;
    float ltdWeight;
    float timeConstant;
    
    Impl() : t1(-20.0f), t2(-10.0f), t3(-5.0f), ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() {
    delete pImpl;
}

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Triplet STDP (Schmidt et al., 2014)
     * 
     * Triplet STDP extends standard STDP with three spike interactions:
     * - Pair1: standard pre-before-post potentiation
     * - Pair2: standard post-before-pre depression
     * - Pair3: triplet interactions involving three spikes
     * 
     * Mathematical formulation (simplified):
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): 
     *   Δw = A+ * exp(-Δt / τ+) + triplet contribution
     *   
     * If Δt < 0 (post before pre):
     *   Δw = A- * exp(Δt / τ-) + triplet contribution
     *   
     * Triplet contribution depends on auxiliary traces of recent activity
     * 
     * Biological inspiration:
     *   - Captures triplet interactions observed in experiments
     *   - Reflects calcium dynamics in dendritic compartments
     *   - Includes NMDA receptor and voltage-gated channel effects
     *   - Used in hippocampal and cortical plasticity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float dtSeconds = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    if (dtSeconds > 0) {
        // Potentiation: standard STDP component
        totalDelta += pImpl->ltpWeight * std::exp(-dtSeconds / pImpl->timeConstant);
        
        // Triplet contribution (simplified)
        // Check for preceding pre-spike within t1 window
        if (preSpikes.size() >= 2) {
            float dt2 = static_cast<float>((postSpikes.back() - preSpikes[preSpikes.size() - 2]) * 1000.0);
            if (dt2 > pImpl->t1 && dt2 < 0) {
                totalDelta += 0.005f * std::exp(-dt2 / pImpl->timeConstant);
            }
        }
    } else if (dtSeconds < 0) {
        // Depression: standard STDP component
        totalDelta += -pImpl->ltdWeight * std::exp(dtSeconds / pImpl->timeConstant);
        
        // Triplet contribution (simplified)
        // Check for following post-spike within t1 window
        if (postSpikes.size() >= 2) {
            float dt1 = static_cast<float>((postSpikes[postSpikes.size() - 2] - preSpikes.back()) * 1000.0);
            if (dt1 > 0 && dt1 < pImpl->t1) {
                totalDelta -= 0.005f * std::exp(dt1 / pImpl->timeConstant);
            }
        }
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

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void TripletSTDP::setTripletParameters(float t1, float t2, float t3) {
    pImpl->t1 = t1;
    pImpl->t2 = t2;
    pImpl->t3 = t3;
}

void TripletSTDP::getTripletParameters(float& t1, float& t2, float& t3) const {
    t1 = pImpl->t1;
    t2 = pImpl->t2;
    t3 = pImpl->t3;
}

// Resource-constrained STDP implementation
struct ResourceConstrainedSTDP::Impl {
    float resourceCapacity;
    float competitionFactor;
    float currentResources;
    float baseLtpWeight;
    float baseLtdWeight;
    float timeConstant;
    
    Impl() : resourceCapacity(1.0f), competitionFactor(0.5f), currentResources(0.0f),
             baseLtpWeight(0.01f), baseLtdWeight(0.012f), timeConstant(20.0f) {}
};

ResourceConstrainedSTDP::ResourceConstrainedSTDP() : pImpl(new Impl) {}

ResourceConstrainedSTDP::~ResourceConstrainedSTDP() {
    delete pImpl;
}

void ResourceConstrainedSTDP::update(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    /*
     * Resource-constrained STDP
     * 
     * This STDP rule incorporates synaptic resource competition, where synapses
     * compete for limited resources (e.g., AMPA receptors, synaptic proteins).
     * 
     * Mathematical formulation:
     * Δw = A+ * exp(-|Δt| / τ) * (1 - R) for Δt > 0
     * Δw = -A- * exp(-|Δt| / τ) * (1 - R) for Δt < 0
     * 
     * Where:
     *   R = currentResources / resourceCapacity
     *   (1 - R) = remaining resource availability
     *   Higher resource usage reduces future plasticity
     *   
     * Biological inspiration:
     *   - Reflects synaptic resource limitations
     *   - Implements homeostatic plasticity
     *   - Explains synaptic strengthening saturation
     *   - Used in activity-dependent synaptic tagging
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float dtSeconds = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    // Calculate resource availability
    float resourceAvailability = 1.0f - (pImpl->currentResources / pImpl->resourceCapacity);
    resourceAvailability = std::clamp(resourceAvailability, 0.0f, 1.0f);
    
    if (dtSeconds > 0) {
        // Potentiation: scaled by resource availability
        totalDelta += pImpl->baseLtpWeight * std::exp(-dtSeconds / pImpl->timeConstant) * resourceAvailability;
    } else if (dtSeconds < 0) {
        // Depression: scaled by resource availability
        totalDelta += -pImpl->baseLtdWeight * std::exp(dtSeconds / pImpl->timeConstant) * resourceAvailability;
    }
    
    // Apply resource competition
    totalDelta *= (1.0f - pImpl->competitionFactor * pImpl->currentResources);
    
    // Update resource usage
    pImpl->currentResources = std::min(pImpl->currentResources + std::abs(totalDelta), pImpl->resourceCapacity);
    
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

void ResourceConstrainedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void ResourceConstrainedSTDP::setResourceCapacity(float capacity) {
    pImpl->resourceCapacity = std::clamp(capacity, 0.1f, 10.0f);
    // Recalculate current resources to fit within new capacity
    pImpl->currentResources = std::min(pImpl->currentResources, pImpl->resourceCapacity);
}

float ResourceConstrainedSTDP::getResourceCapacity() const {
    return pImpl->resourceCapacity;
}

void ResourceConstrainedSTDP::setCompetitionFactor(float factor) {
    pImpl->competitionFactor = std::clamp(factor, 0.0f, 1.0f);
}

float ResourceConstrainedSTDP::getCompetitionFactor() const {
    return pImpl->competitionFactor;
}

// Neuromodulated STDP implementation
struct NeuromodulatedSTDP::Impl {
    float dopamineLevel;
    float curiosityLevel;
    float predictionErrorLevel;
    float baseLtpWeight;
    float baseLtdWeight;
    float timeConstant;
    
    Impl() : dopamineLevel(0.5f), curiosityLevel(0.5f), predictionErrorLevel(0.0f),
             baseLtpWeight(0.01f), baseLtdWeight(0.012f), timeConstant(20.0f) {}
};

NeuromodulatedSTDP::NeuromodulatedSTDP() : pImpl(new Impl) {}

NeuromodulatedSTDP::~NeuromodulatedSTDP() {
    delete pImpl;
}

void NeuromodulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    /*
     * Neuromodulated STDP
     * 
     * This STDP rule incorporates multiple neuromodulators to create
     * a more sophisticated and biologically realistic learning system.
     * 
     * Mathematical formulation:
     * Δw = A+ * (1 + δ_dopamine + δ_curiosity + δ_prediction_error) * exp(-|Δt| / τ) for Δt > 0
     * Δw = -A- * (1 + δ_dopamine + δ_curiosity + δ_prediction_error) * exp(-|Δt| / τ) for Δt < 0
     * 
     * Where:
     *   δ_dopamine = dopamineLevel * scaling
     *   δ_curiosity = curiosityLevel * scaling
     *   δ_prediction_error = predictionErrorLevel * scaling
     *   Scaling factors determine contribution of each neuromodulator
     *   
     * Biological inspiration:
     *   - Integrates multiple neuromodulatory systems
     *   - Combines dopaminergic and octopaminergic signals
     *   - Includes curiosity-driven learning
     *   - Reflects prediction error in plasticity
     *   - Used in adaptive behavior and decision making
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float dtSeconds = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    // Calculate neuromodulation factor
    float neuromodulationFactor = 1.0f + pImpl->dopamineLevel + pImpl->curiosityLevel + pImpl->predictionErrorLevel;
    neuromodulationFactor = std::clamp(neuromodulationFactor, 0.0f, 10.0f);
    
    if (dtSeconds > 0) {
        // Potentiation: scaled by neuromodulation factor
        totalDelta += pImpl->baseLtpWeight * neuromodulationFactor * std::exp(-dtSeconds / pImpl->timeConstant);
    } else if (dtSeconds < 0) {
        // Depression: scaled by neuromodulation factor
        totalDelta += -pImpl->baseLtdWeight * neuromodulationFactor * std::exp(dtSeconds / pImpl->timeConstant);
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

void NeuromodulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void NeuromodulatedSTDP::setDopamineLevel(float level) {
    pImpl->dopamineLevel = std::clamp(level, 0.0f, 2.0f);
}

float NeuromodulatedSTDP::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

void NeuromodulatedSTDP::setCuriosityLevel(float level) {
    pImpl->curiosityLevel = std::clamp(level, 0.0f, 2.0f);
}

float NeuromodulatedSTDP::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

void NeuromodulatedSTDP::setPredictionErrorLevel(float level) {
    pImpl->predictionErrorLevel = std::clamp(level, -1.0f, 1.0f);
}

float NeuromodulatedSTDP::getPredictionErrorLevel() const {
    return pImpl->predictionErrorLevel;
}

} // namespace nlm

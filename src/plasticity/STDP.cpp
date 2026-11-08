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

STDP::~STDP() = default;

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

// ==================== RewardModulatedSTDP Implementation ====================

struct RewardModulatedSTDP::Impl {
    float learningRate;
    float predictionErrorGain;
    float eligibilityDecay;
    
    Impl() : learningRate(0.05f), predictionErrorGain(0.5f), eligibilityDecay(0.1f) {}
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
    
    // Get current prediction error from synapse (placeholder for real integration)
    float predictionError = synapse->getPredictionError();
    
    // Calculate STDP weight change
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    float stdpDelta = 0.0f;
    if (dtMs > 0) {
        stdpDelta = pImpl->learningRate * std::exp(-dtMs / 20.0f);
    } else if (dtMs < 0) {
        stdpDelta = -pImpl->learningRate * std::exp(dtMs / 20.0f);
    }
    
    // Modulate STDP by prediction error
    float modulatedDelta = stdpDelta * (1.0f + pImpl->predictionErrorGain * predictionError);
    
    // Update eligibility trace
    float currentEligibility = synapse->getEligibilityTrace();
    float newEligibility = currentEligibility + modulatedDelta;
    synapse->setEligibilityTrace(newEligibility);
    
    // Apply eligibility trace with decay
    float eligibilityAfterDecay = newEligibility * (1.0f - pImpl->eligibilityDecay * static_cast<float>(dt));
    
    // Store for actual weight application
    if (std::abs(eligibilityAfterDecay) > 1e-6f) {
        synapse->addToEligibilityTrace(eligibilityAfterDecay);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Use eligibility trace to modulate weight change
    float eligibility = synapse->getEligibilityTrace();
    float weightChange = delta * eligibility;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(pImpl->eligibilityDecay);
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setPredictionErrorGain(float gain) {
    pImpl->predictionErrorGain = std::clamp(gain, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getPredictionErrorGain() const {
    return pImpl->predictionErrorGain;
}

// ==================== TripletSTDP Implementation ====================

struct TripletSTDP::Impl {
    float tripletWeight;    // A1 presynaptic trace contribution
    float stdpWeight;       // A2 standard STDP weight
    float preTraceDecay;   // Presynaptic trace decay rate
    float postTraceDecay;  // Postsynaptic trace decay rate
    
    Impl() : tripletWeight(0.005f), stdpWeight(0.01f), 
             preTraceDecay(0.05f), postTraceDecay(0.05f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike timings
    float t_pre = preSpikes.back();
    float t_post = postSpikes.back();
    float dtMs = static_cast<float>((t_post - t_pre) * 1000.0);
    
    // Calculate contributions from different spike combinations
    float stdpContribution = 0.0f;
    float tripletContribution = 0.0f;
    
    if (dtMs > 0) {
        // Pre before post: standard STDP + presynaptic triplet
        stdpContribution = pImpl->stdpWeight * std::exp(-dtMs / 20.0f);
        
        // Presynaptic triplet (pre-pre-post): stronger LTP
        float dtPre = std::max(0.0f, dtMs - 10.0f);  // 10ms presynaptic window
        if (dtPre >= 0) {
            tripletContribution = pImpl->tripletWeight * std::exp(-dtPre / 15.0f);
        }
    } else {
        // Post before pre: standard STDP + postsynaptic triplet  
        stdpContribution = -pImpl->stdpWeight * std::exp(dtMs / 20.0f);
        
        // Postsynaptic triplet (pre-post-post): stronger LTD
        float dtPost = std::max(0.0f, -dtMs - 10.0f);  // 10ms postsynaptic window
        if (dtPost >= 0) {
            tripletContribution = -pImpl->tripletWeight * std::exp(-dtPost / 15.0f);
        }
    }
    
    // Total triplet STDP
    float totalDelta = stdpContribution + tripletContribution;
    
    // Apply with soft bounds for selective learning
    if (std::abs(totalDelta) > 1e-6f) {
        float w = synapse->getWeight();
        bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
        totalDelta *= awayFromZero ? (1.maintenancef - std::abs(w)) : std::abs(w);
        
        // Update eligibility trace
        float currentEligibility = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentEligibility + totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Use eligibility trace for weight application
    float eligibility = synapse->getEligibilityTrace();
    float weightChange = delta * eligibility;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, -0.1f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Maintain sign constraints
    if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.1f) {
        synapse->setWeight(-0.1f);
    } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.1f) {
        synapse->setWeight(0.1f);
    }
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(0.05f);
}

void TripletSTDP::setTripletWeight(float weight) {
    pImpl->tripletWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getTripletWeight() const {
    return pImpl->tripletWeight;
}

void TripletSTDP::setSTDPWeight(float weight) {
    pImpl->stdpWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getSTDPWeight() const {
    return pImpl->stdpWeight;
}

// ==================== CompetitiveHebbianRule Implementation ====================

struct NeuronsSimilarity {
    float visionSimilarity;
    float internalSimilarity;
    float motorSimilarity;
};

struct CompetitiveHebbianRule::Impl {
    float competitionStrength;
    float selectivityTarget;
    float minWeightDifference;
    
    Impl() : competitionStrength(0.1f), selectivityTarget(0.3f), minWeightDifference(0.1f) {}
};

CompetitiveHebbianRule::CompetitiveHebbianRule() : pImpl(new Impl) {}

CompetitiveHebbianRule::~CompetitiveHebbianRule() = default;

void CompetitiveHebbianRule::update(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate Hebbian contribution
    float hebbianDelta = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        hebbianDelta = 0.02f * std::exp(-0.01f * 
            (std::max(preSpikes.back(), postSpikes.back()) - std::min(preSpikes.back(), postSpikes.back())));
    }
    
    // Get source and target neurons
    Neuron* sourceNeuron = synapse->getSourceNeuron();
    Neuron* targetNeuron = synapse->getDestinationNeuron();
    
    if (sourceNeuron && targetNeuron) {
        // Calculate neural similarity
        NeuronsSimilarity similarity = calculateNeuralSimilarity(sourceNeuron, targetNeuron);
        
        // Apply competitive penalty based on similarity
        float competitivePenalty = calculateCompetitivePenalty(sourceNeuron, targetNeuron, {});
        
        // Modulate Hebbian learning by competition
        float selectiveDelta = hebbianDelta * (1.0f - competitionStrength * similarityTotal() * competitivePenalty);
        
        // Ensure selectivity: similar neurons should have different weights
        if (similarity.visionSimilarity > 0.5f || similarity.internalSimilarity > 0.5f) {
            float currentWeight = synapse->getWeight();
            float targetWeight = std::min(0.5f, currentWeight + 0.02f);
            
            if (currentWeight < targetWeight - minWeightDifference) {
                selectiveDelta *= 0.5f;  // Reduce learning for similar neurons
            }
        }
        
        // Apply soft bounds for selective learning
        bool awayFromZero = std::abs(targetWeight + selectiveDelta) > std::abs(targetWeight);
        selectiveDelta *= awayFromZero ? (1.0f - std::abs(targetWeight)) : std::abs(targetWeight);
        
        // Update eligibility trace
        float currentEligibility = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentEligibility + selectiveDelta);
    }
}

void CompetitiveHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Use eligibility trace for weight application
    float eligibility = synapse->getEligibilityTrace();
    float weightChange = delta * eligibility;
    
    // Apply selective learning constraints
    float currentWeight = synapse->getWeight();
    float targetWeight = currentWeight + weightChange;
    
    // Ensure minimum weight difference between similar synapses
    if (std::abs(targetWeight - currentWeight) > minWeightDifference) {
        // Clamp to maintain selectivity
        targetWeight = std::clamp(targetWeight, -0.1f, 0.5f);
        synapse->setWeight(targetWeight);
        
        // Update eligibility trace based on actual change
        float actualChange = targetWeight - currentWeight;
        synapse->setEligibilityTrace(eligibility - actualChange);
    }
}

void CompetitiveHebbianRule::setCompetitionStrength(float strength) {
    pImpl->competitionStrength = std::clamp(strength, 0.0f, 1.0f);
}

float CompetitiveHebbianRule::getCompetitionStrength() const {
    return pImpl->competitionStrength;
}

void CompetitiveHebbianRule::setSelectivityTarget(float target) {
    pImpl->selectivityTarget = std::clamp(target, 0.0f, 1.0f);
}

float CompetitiveHebbianRule::getSelectivityTarget() const {
    return pImpl->selectivityTarget;
}

float CompetitiveHebbianRule::calculateCompetitivePenalty(Neuron* source, Neuron* target,
                                                     const std::vector<Neuron*>& allNeurons) const {
    if (!source || !target) return 0.0f;
    
    // Calculate similarity based on neural characteristics
    NeuronsSimilarity similarity = calculateNeuralSimilarity(source, target);
    
    // Higher similarity means higher penalty
    float similarityTotal = similarity.visionSimilarity + similarity.internalSimilarity + similarity.motorSimilarity;
    
    return 1.0f - similarityTotal;  // 0 when identical, 1 when completely different
}

} // namespace nlm


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
     * Enhanced STDP implementation with competitive learning and selective plasticity
     * 
     * Mathematical formulation (Enhanced pairwise rule):
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * activityFactor * competitionFactor
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-) * activityFactor * competitionFactor
     * 
     * Enhanced with competitive mechanisms:
     *   - Activity-dependent modulation based on network context
     *   - Competitive normalization to prevent runaway potentiation
     *   - Heterosynaptic interactions to promote selective learning
     *   
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     *   activityFactor = network activity modulation (0.5-1.5x)
     *   competitionFactor = 1 - relative synaptic strength
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   - Includes competitive synaptic pruning (like development)
     *   - Activity-dependent plasticity (like homeostatic scaling)
     *   
     * Benefits over classic STDP:
     *   - Promotes selective learning (only correlated pairs strengthen)
     *   - Prevents runaway potentiation through competitive normalization
     *   - Maintains synaptic diversity and network stability
     *   - Better pattern separation/selection
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;

    // Nearest-neighbour STDP: only the most recent pre/post pair is used.
    // Enhanced with competitive mechanisms:
    // Timestamps are in seconds (Brain steps at 1ms), so convert to ms for the tau window.
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    if (dtMs > 0) {
        // Pre-before-post: potentiation
        // Enhanced with competitive factors
        float basePotentiation = pImpl->ltpWeight * std::exp(-dtMs / tau);
        
        // Activity-dependent modulation: stronger learning when network is active
        float networkActivity = std::min(1.0f, synapse->getEfficacy() * 2.0f);
        float activityModulation = 1.0f + 0.5f * networkActivity;
        
        // Competitive normalization: prevent runaway potentiation
        float weightMagnitude = std::abs(synapse->getWeight());
        float competitionFactor = 1.0f - weightMagnitude / pImpl->maxWeight;
        
        // Synaptic eligibility trace for reward modulation
        float eligibility = synapse->getEligibilityTrace();
        float eligibilityModulation = std::max(0.5f, 1.0f + eligibility);
        
        totalDelta += basePotentiation * activityModulation * competitionFactor * eligibilityModulation;
        
    } else if (dtMs < 0) {
        // Post-before-pre: depression
        // Enhanced with competitive mechanisms
        float baseDepression = -pImpl->ltdWeight * std::exp(dtMs / tau);
        
        // Activity-dependent modulation
        float networkActivity = std::min(1.0f, synapse->getEfficacy() * 2.0f);
        float activityModulation = 1.0f + 0.3f * networkActivity;
        
        // Competitive normalization: stronger depression for stronger synapses
        float weightMagnitude = std::abs(synapse->getWeight());
        float competitionFactor = weightMagnitude / pImpl->maxWeight;
        
        // Synaptic eligibility trace
        float eligibility = synapse->getEligibilityTrace();
        float eligibilityModulation = std::max(0.5f, 1.0f + eligibility);
        
        totalDelta += baseDepression * activityModulation * competitionFactor * eligibilityModulation;
    }
    
    // Apply weight change with enhanced bounds and competitive mechanisms
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;

        // Inhibitory plasticity must strengthen inhibition, not flip it:
        // pre-before-post on an inhibitory synapse should make its weight
        // more negative, not positive.
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }

        // Enhanced competitive soft bounds:
        // - Potentiation (away from zero) scales by (1 - |w|)
        // - Depression (toward zero) scales by |w|
        // - This creates competitive dynamics where strong synapses can only
        //   grow slowly, while weak synapses grow faster
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
        // Only update if this is a genuine Hebbian/STDP event (not random noise)
        if (std::abs(dtMs) < 200.0f) {  // Only recent spike pairs
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + totalDelta * 0.5f);
        }
        
        // Competitive synaptic pruning: if synapse is strong but not very active,
        // weaken it to make room for other synapses
        if (std::abs(synapse->getWeight()) > pImpl->maxWeight * 0.9f && 
            efficacy < 0.5f && eligibility < 0.01f) {
            float pruningFactor = 0.01f;
            float prunedWeight = synapse->getWeight() * (1.0f - pruningFactor);
            if (synapse->getType() == SynapseType::Excitatory && 
                prunedWeight > 0.01f) {
                synapse->setWeight(prunedWeight);
            } else if (synapse->getType() == SynapseType::Inhibitory && 
                      prunedWeight < -0.01f) {
                synapse->setWeight(prunedWeight);
            }
        }
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

} // namespace nlm

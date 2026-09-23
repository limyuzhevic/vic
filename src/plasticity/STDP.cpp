#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Validation/Validation.hpp"
#include "../../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <format>

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
    
    // Comprehensive null and parameter validation
    VALIDATE_NULL_RETURN(synapse, "STDP::update: synapse pointer is null", nullptr);
    VALIDATE_PARAMETER_RETURN(!Validation::IsEmpty(preSpikes), 
                           "STDP::update: preSpikes vector is empty", 
                           ErrorCode::InvalidParameter, nullptr);
    VALIDATE_PARAMETER_RETURN(!Validation::IsEmpty(postSpikes),
                           "STDP::update: postSpikes vector is empty",
                           ErrorCode::InvalidParameter, nullptr);
    VALIDATE_PARAMETER_RETURN(Validation::IsValidTimestep(dt),
                           "STDP::update: timestep is invalid",
                           ErrorCode::InvalidParameter, nullptr);
    
    // Validate internal STDP parameters
    VALIDATE_PARAMETER_RETURN(Validation::IsValidWeight(pImpl->ltpWeight),
                           "STDP::update: LTP weight is invalid",
                           ErrorCode::ConfigurationError, nullptr);
    VALIDATE_PARAMETER_RETURN(Validation::IsValidWeight(pImpl->ltdWeight),
                           "STDP::update: LTD weight is invalid",
                           ErrorCode::ConfigurationError, nullptr);
    VALIDATE_PARAMETER_RETURN(Validation::IsValidTimeConstant(pImpl->timeConstant),
                           "STDP::update: time constant is invalid",
                           ErrorCode::ConfigurationError, nullptr);
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // O(n log n) sliding window approach using binary search
    // Since both spike lists are sorted by timestamp (chronological order),
    // we can efficiently find all spike pairs within the time window τ
    // For each pre spike, find all post spikes within [preTime-τ, preTime+τ]
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        Timestamp preTime = preSpikes[i];
        VALIDATE_PARAMETER_RETURN(Validation::IsValidTimestamp(preTime),
                               "STDP::update: preSpike timestamp is invalid",
                               ErrorCode::InvalidParameter, nullptr);
        
        // Find the range of post spikes within the time window using binary search
        // Lower bound: postTime >= preTime - tau
        auto lowerIt = std::lower_bound(
            postSpikes.begin(), postSpikes.end(), 
            preTime - tau
        );
        
        // Upper bound: postTime <= preTime + tau  
        // Need exclusive upper bound, so find first > preTime + tau
        auto upperIt = std::upper_bound(
            postSpikes.begin(), postSpikes.end(),
            preTime + tau
        );
        
        // Process all post spikes in the range [lowerIt, upperIt)
        for (auto postIt = lowerIt; postIt != upperIt; ++postIt) {
            Timestamp postTime = *postIt;
            VALIDATE_PARAMETER_RETURN(Validation::IsValidTimestamp(postTime),
                                   "STDP::update: postSpike timestamp is invalid",
                                   ErrorCode::InvalidParameter, nullptr);
            
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            VALIDATE_PARAMETER_RETURN(Validation::IsFinite(dt),
                                   "STDP::update: time difference is not finite",
                                   ErrorCode::InvalidParameter, nullptr);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                if (Validation::IsFinite(delta)) {
                    totalDelta += delta;
                } else {
                    LOG_WARNING("STDP::update: non-finite delta in potentiation case, skipping");
                }
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                // For negative dt, exp(dt/tau) still yields valid depression
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                if (Validation::IsFinite(delta)) {
                    totalDelta += delta;
                } else {
                    LOG_WARNING("STDP::update: non-finite delta in depression case, skipping");
                }
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        VALIDATE_PARAMETER_RETURN(Validation::IsFinite(efficacy),
                               "STDP::update: synapse efficacy is not finite",
                               ErrorCode::InvalidParameter, nullptr);
        
        totalDelta *= efficacy;
        
        // Apply weight change with validation
        if (Validation::ValidateWeightChange(totalDelta, pImpl->minWeight, pImpl->maxWeight)) {
            synapse->addToWeight(totalDelta);
            
            // Update eligibility trace for reward-modulated learning
            float currentTrace = synapse->getEligibilityTrace();
            VALIDATE_PARAMETER_RETURN(Validation::IsFinite(currentTrace),
                                   "STDP::update: eligibility trace is not finite",
                                   ErrorCode::InvalidParameter, nullptr);
            synapse->setEligibilityTrace(currentTrace + totalDelta);
            
            LOG_INFO(std::format("STDP::update: Applied weight change {:.6f} to synapse {}, new weight: {:.6f}",
                             totalDelta, synapse->getId(), synapse->getWeight()));
        } else {
            LOG_ERROR(std::format("STDP::update: Failed to apply weight change {:.6f} to synapse {} due to validation failure",
                               totalDelta, synapse->getId()));
        }
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Comprehensive null pointer validation
    VALIDATE_NULL_RETURN(synapse, "STDP::applyWeightChange: synapse pointer is null",);
    
    // Validate delta parameter
    VALIDATE_PARAMETER_RETURN(Validation::IsFinite(delta),
                           "STDP::applyWeightChange: weight change is not finite",
                           ErrorCode::InvalidParameter,);
    
    // Get current weight and validate it
    float currentWeight = synapse->getWeight();
    VALIDATE_PARAMETER_RETURN(Validation::IsFinite(currentWeight),
                           "STDP::applyWeightChange: current synapse weight is not finite",
                           ErrorCode::InvalidParameter,);
    
    // Calculate new weight
    float newWeight = currentWeight + delta;
    
    // Clamp weight to bounds with validation
    float clampedWeight = Validation::Clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Validate bounds
    VALIDATE_PARAMETER_RETURN(Validation::IsValidWeight(clampedWeight),
                           "STDP::applyWeightChange: clamped weight is invalid",
                           ErrorCode::OutOfBounds,);
    
    // Apply weight change
    synapse->setWeight(clampedWeight);
    
    // Log the change if significant
    if (std::abs(delta) > 1e-6f) {
        LOG_INFO(std::format("STDP::applyWeightChange: Changed synapse {} weight from {:.6f} to {:.6f} (delta: {:.6f})",
                         synapse->getId(), currentWeight, clampedWeight, delta));
    }
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

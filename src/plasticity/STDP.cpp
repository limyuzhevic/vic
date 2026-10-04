#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
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
             minWeight(-1.0f), maxWeight(1.0f) {
        try {
            // Validate initial values
            if (timeConstant <= 0.0f) {
                throw std::invalid_argument("Time constant must be positive");
            }
            if (minWeight >= maxWeight) {
                throw std::invalid_argument("Min weight must be less than max weight");
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to initialize STDP parameters: " + std::string(e.what()));
            throw;
        }
    }
};

STDP::STDP() : pImpl(new Impl) {
    try {
        if (!pImpl) {
            throw std::runtime_error("Failed to allocate STDP implementation");
        }
        NLM_LOG_INFO("STDP initialized successfully");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize STDP: " + std::string(e.what()));
        throw;
    }
}

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
    
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            NLM_LOG_WARNING("Invalid STDP parameters: null synapse or empty spike history");
            return;
        }
        
        if (dt <= 0.0f) {
            NLM_LOG_WARNING("Invalid timestep in STDP update: " + std::to_string(dt));
            return;
        }
        
        float totalDelta = 0.0f;
        float tau = pImpl->timeConstant;
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt_val = static_cast<float>(postTime - preTime);  // Δt in ms
                
                if (dt_val > 0) {
                    // Pre before post: POTENTIATION
                    // "Cells that fire together, wire together" - but only if pre fires before post
                    float delta = pImpl->ltpWeight * std::exp(-dt_val / tau);
                    totalDelta += delta;
                } else if (dt_val < 0) {
                    // Post before pre: DEPRESSION
                    // "Anti-Hebbian" - connection weakens if post fires without pre
                    float delta = -pImpl->ltdWeight * std::exp(dt_val / tau);  // dt_val is negative, so this subtracts
                    totalDelta += delta;
                }
                // dt_val == 0: no change (simultaneous spikes - rare in practice)
            }
        }
        
        // Apply weight change with bounds
        if (std::abs(totalDelta) > 1e-6f) {
            // Scale by synaptic efficacy if available
            float efficacy = synapse->getEfficacy();
            totalDelta *= efficacy;
            
            // Apply weight change
            synapse->addToWeight(totalDelta);
            
            // Update eligibility trace for reward-modulated learning
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + totalDelta);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in STDP update: ") + e.what());
        throw;
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        
        if (!synapse) {
            NLM_LOG_WARNING("Cannot apply STDP weight change to null synapse");
            return;
        }
        
        // Clamp weight to bounds
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in STDP applyWeightChange: ") + e.what());
        throw;
    }
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        
        if (weight < 0.0f) {
            NLM_LOG_WARNING("Negative LTP weight value: " + std::to_string(weight));
        }
        
        pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error setting STDP LTP weight: ") + e.what());
        throw;
    }
}

float STDP::getLTPWeight() const {
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        return pImpl->ltpWeight;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting STDP LTP weight: ") + e.what());
        throw;
    }
}

void STDP::setLTDWeight(float weight) {
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        
        if (weight < 0.0f) {
            NLM_LOG_WARNING("Negative LTD weight value: " + std::to_string(weight));
        }
        
        pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error setting STDP LTD weight: ") + e.what());
        throw;
    }
}

float STDP::getLTDWeight() const {
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        return pImpl->ltdWeight;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting STDP LTD weight: ") + e.what());
        throw;
    }
}

void STDP::setTimeConstant(float tau) {
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        
        if (tau <= 0.0f) {
            throw std::invalid_argument("Time constant must be positive: " + std::to_string(tau));
        }
        
        pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error setting STDP time constant: ") + e.what());
        throw;
    }
}

float STDP::getTimeConstant() const {
    try {
        if (!pImpl) {
            throw std::runtime_error("STDP not properly initialized");
        }
        return pImpl->timeConstant;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error getting STDP time constant: ") + e.what());
        throw;
    }
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    try {
        setLTPWeight(ltpWeight);
        setLTDWeight(ltdWeight);
        setTimeConstant(tau);
        
        NLM_LOG_INFO("STDP configured with LTP weight: " + std::to_string(ltpWeight) +
                    ", LTD weight: " + std::to_string(ltdWeight) +
                    ", time constant: " + std::to_string(tau));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error configuring STDP: ") + e.what());
        throw;
    }
}

} // namespace nlm

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
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    try {
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
        
        // Validate synapse pointer
        if (!synapse) {
            Logger::getGlobal().warning("STDP::update: Synapse pointer is null");
            return;
        }
        
        // Validate spike vectors
        if (preSpikes.empty() || postSpikes.empty()) {
            Logger::getGlobal().warning("STDP::update: One or both spike history vectors are empty");
            return;
        }
        
        // Validate timestep
        if (dt <= 0.0) {
            Logger::getGlobal().error("STDP::update: Invalid timestep: " + std::to_string(dt));
            return;
        }
        
        // Validate STDP parameters
        if (pImpl->ltpWeight < 0.0f || pImpl->ltpWeight > 1.0f) {
            Logger::getGlobal().warning("STDP::update: Invalid LTP weight: " + std::to_string(pImpl->ltpWeight) + ", clamping to [0,1]");
            pImpl->ltpWeight = std::clamp(pImpl->ltpWeight, 0.0f, 1.0f);
        }
        
        if (pImpl->ltdWeight < 0.0f || pImpl->ltdWeight > 1.0f) {
            Logger::getGlobal().warning("STDP::update: Invalid LTD weight: " + std::to_string(pImpl->ltdWeight) + ", clamping to [0,1]");
            pImpl->ltdWeight = std::clamp(pImpl->ltdWeight, 0.0f, 1.0f);
        }
        
        if (pImpl->timeConstant < 1.0f || pImpl->timeConstant > 100.0f) {
            Logger::getGlobal().warning("STDP::update: Invalid time constant: " + std::to_string(pImpl->timeConstant) + ", clamping to [1,100]");
            pImpl->timeConstant = std::clamp(pImpl->timeConstant, 1.0f, 100.0f);
        }
        
        float totalDelta = 0.0f;
        float tau = pImpl->timeConstant;
        
        // Check for potential overflow in spike count multiplication
        if (preSpikes.size() > 1000 || postSpikes.size() > 1000) {
            Logger::getGlobal().warning("STDP::update: Large spike history sizes may impact performance");
        }
        
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);  // Δt in ms
                
                // Validate timing difference
                if (std::isnan(dt) || std::isinf(dt)) {
                    Logger::getGlobal().warning("STDP::update: Invalid timing difference: " + std::to_string(dt));
                    continue;
                }
                
                if (dt > 0) {
                    // Pre before post: POTENTIATION
                    // "Cells that fire together, wire together" - but only if pre fires before post
                    float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                    
                    // Check for overflow
                    if (std::isnan(delta) || std::isinf(delta)) {
                        Logger::getGlobal().warning("STDP::update: NaN or infinite delta for potentiation");
                        delta = 0.0f;
                    }
                    
                    totalDelta += delta;
                    
                    // Check for overflow in accumulated delta
                    if (std::isnan(totalDelta) || std::isinf(totalDelta)) {
                        Logger::getGlobal().error("STDP::update: Overflow in accumulated delta");
                        totalDelta = 0.0f;
                        break;
                    }
                } else if (dt < 0) {
                    // Post before pre: DEPRESSION
                    // "Anti-Hebbian" - connection weakens if post fires without pre
                    float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                    
                    // Check for overflow
                    if (std::isnan(delta) || std::isinf(delta)) {
                        Logger::getGlobal().warning("STDP::update: NaN or infinite delta for depression");
                        delta = 0.0f;
                    }
                    
                    totalDelta += delta;
                    
                    // Check for overflow in accumulated delta
                    if (std::isnan(totalDelta) || std::isinf(totalDelta)) {
                        Logger::getGlobal().error("STDP::update: Overflow in accumulated delta");
                        totalDelta = 0.0f;
                        break;
                    }
                }
                // dt == 0: no change (simultaneous spikes - rare in practice)
            }
            
            // Check if we've accumulated too large a delta
            if (std::abs(totalDelta) > 1.0f) {
                Logger::getGlobal().warning("STDP::update: Large accumulated delta: " + std::to_string(totalDelta) + ", may need scaling");
            }
        }
        
        // Apply weight change with bounds
        if (std::abs(totalDelta) > 1e-6f) {
            try {
                // Scale by synaptic efficacy if available
                float efficacy = synapse->getEfficacy();
                totalDelta *= efficacy;
                
                // Apply weight change
                synapse->addToWeight(totalDelta);
                
                // Update eligibility trace for reward-modulated learning
                float currentTrace = synapse->getEligibilityTrace();
                synapse->setEligibilityTrace(currentTrace + totalDelta);
                
            } catch (const std::exception& e) {
                Logger::getGlobal().error("STDP::update: Exception while applying weight change: " + std::string(e.what()));
            }
        }
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("STDP::update: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("STDP::update: Unexpected exception: " + std::string(e.what()));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    try {
        // Validate synapse pointer
        if (!synapse) {
            Logger::getGlobal().warning("STDP::applyWeightChange: Synapse pointer is null");
            return;
        }
        
        // Validate delta
        if (std::isnan(delta) || std::isinf(delta)) {
            Logger::getGlobal().error("STDP::applyWeightChange: Invalid delta value: " + std::to_string(delta));
            return;
        }
        
        // Clamp weight to bounds
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        
        // Check for NaN or infinite weight
        if (std::isnan(newWeight) || std::isinf(newWeight)) {
            Logger::getGlobal().error("STDP::applyWeightChange: Invalid computed weight: " + std::to_string(newWeight));
            return;
        }
        
        synapse->setWeight(newWeight);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("STDP::applyWeightChange: Exception while applying weight change: " + std::string(e.what()));
    }
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    try {
        // Validate weight
        if (std::isnan(weight) || std::isinf(weight)) {
            Logger::getGlobal().error("STDP::setLTPWeight: Invalid weight: " + std::to_string(weight));
            return;
        }
        
        pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("STDP::setLTPWeight: Exception while setting LTP weight: " + std::string(e.what()));
    }
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

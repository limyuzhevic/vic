#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct TripletSTDP::Impl {
    float aPlus;          // A+ for LTP (pre-post pairing)
    float aMinus;         // A- for LTD (post-pre pairing)
    float aU;             // A_U for unpaired spikes
    float tauPlus;        // τ+ for pre-post influence
    float tauMinus;       // τ- for post-pre influence
    float tauU;           // τ_U for unpaired influence
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float wMax;           // Maximum synaptic weight (alternative parameter)
    float wMin;           // Minimum synaptic weight (alternative parameter)
    
    Impl() : aPlus(0.025f), aMinus(0.0275f), aU(0.005f),
             tauPlus(20.0f), tauMinus(20.0f), tauU(200.0f),
             minWeight(-1.0f), maxWeight(1.0f), wMax(1.0f), wMin(-1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    // Input validation
    if (synapse == nullptr) {
        NLM_LOG_WARN("TripletSTDP::update: synapse is null");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        // No spikes to process - this is normal, not an error
        return;
    }
    
    if (dt <= 0.0f) {
        NLM_LOG_WARN("TripletSTDP::update: invalid timestep duration: " + std::to_string(dt));
        return;
    }
    
    try {
        // Validate parameters
        if (pImpl->tauPlus <= 0.0f || pImpl->tauMinus <= 0.0f || pImpl->tauU <= 0.0f) {
            NLM_LOG_WARN("TripletSTDP::update: invalid time constants: tauPlus=" + std::to_string(pImpl->tauPlus) +
                        ", tauMinus=" + std::to_string(pImpl->tauMinus) +
                        ", tauU=" + std::to_string(pImpl->tauU));
            return;
        }
        
        if (pImpl->minWeight >= pImpl->maxWeight) {
            NLM_LOG_WARN("TripletSTDP::update: invalid weight bounds: min=" + std::to_string(pImpl->minWeight) + 
                        ", max=" + std::to_string(pImpl->maxWeight));
            return;
        }
        
        float totalDelta = 0.0f;
        
        // Track recent spike history for triplet interactions
        // For demonstration, consider all spikes (in practice, use appropriate time windows)
        std::vector<Timestamp> recentPreSpikes = preSpikes;
        std::vector<Timestamp> recentPostSpikes = postSpikes;
        
        // Apply triplet interactions
        for (size_t i = 0; i < recentPreSpikes.size(); ++i) {
            Timestamp preTime = recentPreSpikes[i];
            
            for (size_t j = 0; j < recentPostSpikes.size(); ++j) {
                Timestamp postTime = recentPostSpikes[j];
                float dt = static_cast<float>(postTime - preTime);  // Δt in ms
                
                if (dt > 0) {
                    // Pre before post: LTP components
                    // Main paired component
                    float ltp = pImpl->aPlus * std::exp(-dt / pImpl->tauPlus);
                    totalDelta += ltp;
                    
                    // Unpaired component from other post spike
                    // (simplified - would consider all spike combinations in full implementation)
                } else if (dt < 0) {
                    // Post before pre: LTD components
                    // Main paired component
                    float ltd = -pImpl->aMinus * std::exp(dt / pImpl->tauMinus);
                    totalDelta += ltd;
                    
                    // Unpaired component from other pre spike
                    // (simplified - would consider all spike combinations in full implementation)
                } else {
                    // dt == 0: Simultaneous spikes - unpaired component
                    // This contributes aU weight changes for both pre and post spikes
                    totalDelta += pImpl->aU * 2.0f;  // Both pre and post spike effects
                }
            }
        }
        
        // Apply weight change with bounds and homeostatic regulation
        if (std::abs(totalDelta) > 1e-6f) {
            try {
                // Scale by synaptic efficacy if available
                float efficacy = synapse->getEfficacy();
                if (efficacy < 0.0f || efficacy > 2.0f) {
                    NLM_LOG_WARN("TripletSTDP::update: invalid efficacy value: " + std::to_string(efficacy));
                    efficacy = std::clamp(efficacy, 0.0f, 2.0f);
                }
                
                totalDelta *= efficacy;
                
                // Apply homeostatic scaling based on current weight
                float currentWeight = synapse->getWeight();
                float weightRange = pImpl->maxWeight - pImpl->minWeight;
                if (weightRange > 0.0f) {
                    float homeostaticFactor = 1.0f - std::abs(currentWeight) / weightRange;
                    totalDelta *= (1.0f + 0.1f * homeostaticFactor); // Prevent runaway potentiation
                }
                
                // Apply weight change
                synapse->addToWeight(totalDelta);
                
                // Update synaptic tags for protein synthesis-dependent plasticity
                float currentTagStrength = synapse->getPlasticityTag();
                synapse->setPlasticityTag(currentTagStrength + std::abs(totalDelta));
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("TripletSTDP::update: error applying weight change: " + std::string(e.what()));
            }
        }
        
        // Log activity for debugging
        if (std::abs(totalDelta) > 1e-6f) {
            NLM_LOG_DEBUG("TripletSTDP::update: aPlus=" + std::to_string(pImpl->aPlus) +
                         ", aMinus=" + std::to_string(pImpl->aMinus) +
                         ", aU=" + std::to_string(pImpl->aU) +
                         ", total_delta=" + std::to_string(totalDelta) +
                         ", final_weight=" + std::to_string(synapse->getWeight()));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("TripletSTDP::update: error processing spike pairs: " + std::string(e.what()));
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse == nullptr) {
        NLM_LOG_WARN("TripletSTDP::applyWeightChange: synapse is null");
        return;
    }
    
    if (std::abs(delta) < 1e-6f) {
        // Very small weight change - skip to avoid numerical issues
        return;
    }
    
    try {
        // Clamp weight to bounds using both parameter sets
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        newWeight = std::clamp(newWeight, pImpl->wMin, pImpl->wMax);
        
        // Check for weight change
        if (std::abs(newWeight - synapse->getWeight()) > 1e-6f) {
            synapse->setWeight(newWeight);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("TripletSTDP::applyWeightChange: error applying weight change: " + std::string(e.what()));
    }
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setAPlus(float aPlus) {
    if (aPlus < 0.0f) {
        NLM_LOG_WARN("TripletSTDP::setAPlus: negative A+ value will be clamped: " + std::to_string(aPlus));
    }
    pImpl->aPlus = std::clamp(aPlus, 0.0f, 1.0f);
}

float TripletSTDP::getAPlus() const {
    return pImpl->aPlus;
}

void TripletSTDP::setAMinus(float aMinus) {
    if (aMinus < 0.0f) {
        NLM_LOG_WARN("TripletSTDP::setAMinus: negative A- value will be clamped: " + std::to_string(aMinus));
    }
    pImpl->aMinus = std::clamp(aMinus, 0.0f, 1.0f);
}

float TripletSTDP::getAMinus() const {
    return pImpl->aMinus;
}

void TripletSTDP::setAU(float aU) {
    if (aU < 0.0f) {
        NLM_LOG_WARN("TripletSTDP::setAU: negative A_U value will be clamped: " + std::to_string(aU));
    }
    pImpl->aU = std::clamp(aU, 0.0f, 1.0f);
}

float TripletSTDP::getAU() const {
    return pImpl->aU;
}

void TripletSTDP::setTauPlus(float tauPlus) {
    if (tauPlus <= 0.0f) {
        NLM_LOG_WARN("TripletSTDP::setTauPlus: invalid time constant: " + std::to_string(tauPlus));
        return;
    }
    pImpl->tauPlus = std::clamp(tauPlus, 1.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tauMinus) {
    if (tauMinus <= 0.0f) {
        NLM_LOG_WARN("TripletSTDP::setTauMinus: invalid time constant: " + std::to_string(tauMinus));
        return;
    }
    pImpl->tauMinus = std::clamp(tauMinus, 1.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::setTauU(float tauU) {
    if (tauU <= 0.0f) {
        NLM_LOG_WARN("TripletSTDP::setTauU: invalid time constant: " + std::to_string(tauU));
        return;
    }
    pImpl->tauU = std::clamp(tauU, 10.0f, 1000.0f);
}

float TripletSTDP::getTauU() const {
    return pImpl->tauU;
}

void TripletSTDP::configure(float aPlus, float aMinus, float aU, 
                           float tauPlus, float tauMinus, float tauU) {
    if (std::abs(aPlus - pImpl->aPlus) < 1e-6f && 
        std::abs(aMinus - pImpl->aMinus) < 1e-6f &&
        std::abs(aU - pImpl->aU) < 1e-6f &&
        std::abs(tauPlus - pImpl->tauPlus) < 1e-6f &&
        std::abs(tauMinus - pImpl->tauMinus) < 1e-6f &&
        std::abs(tauU - pImpl->tauU) < 1e-6f) {
        return; // No change needed
    }
    
    setAPlus(aPlus);
    setAMinus(aMinus);
    setAU(aU);
    setTauPlus(tauPlus);
    setTauMinus(tauMinus);
    setTauU(tauU);
    
    NLM_LOG_INFO("TripletSTDP configured: aPlus=" + std::to_string(pImpl->aPlus) +
                ", aMinus=" + std::to_string(pImpl->aMinus) +
                ", aU=" + std::to_string(pImpl->aU) +
                ", tauPlus=" + std::to_string(pImpl->tauPlus) +
                ", tauMinus=" + std::to_string(pImpl->tauMinus) +
                ", tauU=" + std::to_string(pImpl->tauU));
}

} // namespace nlm
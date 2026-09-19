#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct STDP::Impl {
    // STDP core parameters
    float ltpWeight;              // A+ for potentiation
    float ltdWeight;              // A- for depression
    float timeConstant;           // Tau for exponential window (ms)
    float minWeight;              // Minimum synaptic weight
    float maxWeight;              // Maximum synaptic weight
    
    // Calcium dynamics parameters
    float caConductance;          // Calcium conductance factor
    float caDecayRate;            // Calcium decay rate (1/ms)
    float caThresholdLTP;         // Calcium threshold for LTP
    float caThresholdLTD;         // Calcium threshold for LTD
    float caBaseline;             // Baseline calcium level
    
    // Spike history tracking
    std::unordered_map<SynapseId, std::vector<Timestamp>> preSpikeHistory;
    std::unordered_map<SynapseId, std::vector<Timestamp>> postSpikeHistory;
    float historyDecayRate;       // Spike history decay (1/ms)
    
    // Neuromodulatory control
    float neuromodulatorLevel;    // Neuromodulator concentration (e.g., dopamine)
    float neuromodulatorInfluence; // Strength of neuromodulatory effect
    bool neuromodulatorEnabled;   // Whether neuromodulation is active
    
    // Weight dependence parameters
    float weightDependenceLTP;    // Weight dependence for LTP
    float weightDependenceLTD;    // Weight dependence for LTD
    float maxWeightTrace;        // Maximum weight trace value
    
    // Triplet factor interactions (for more biologically accurate STDP)
    float tripletTrace;           // Triplet trace variable
    float tripletDecayRate;       // Triplet decay rate
    
    // Statistics and debugging
    size_t totalSTDPUpdates;
    float cumulativeWeightChange;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             caConductance(0.005f), caDecayRate(0.02f), caThresholdLTP(0.15f),
             caThresholdLTD(0.05f), caBaseline(0.01f), historyDecayRate(0.005f),
             neuromodulatorLevel(1.0f), neuromodulatorInfluence(1.0f),
             neuromodulatorEnabled(false), weightDependenceLTP(0.5f),
             weightDependenceLTD(0.5f), maxWeightTrace(1.0f),
             tripletTrace(0.0f), tripletDecayRate(0.01f),
             totalSTDPUpdates(0), cumulativeWeightChange(0.0f) {}
};

// Helper function to calculate calcium dynamics based on spike timing
inline float calculateCalciumChange(const Timestamp& preTime, const Timestamp& postTime,
                                   const Timestamp& currentTime, float caDecayRate,
                                   float caBaseline, bool preSpiked, bool postSpiked) {
    float dt = static_cast<float>(currentTime - preTime);
    float calcium = caBaseline;
    
    // Add calcium due to postsynaptic spike (backpropagating action potential)
    if (postSpiked) {
        float timeDiff = static_cast<float>(currentTime - postTime);
        if (timeDiff >= 0.0f) {
            // Calcium increase from post spike decays with time
            calcium += 1.0f * std::exp(-timeDiff / 10.0f);
        }
    }
    
    // Add calcium due to presynaptic spike (NMDA receptor activation)
    if (preSpiked) {
        float timeDiff = static_cast<float>(currentTime - preTime);
        if (timeDiff >= 0.0f) {
            // Calcium increase from pre spike depends on timing
            calcium += 0.5f * std::exp(-timeDiff / 15.0f);
        }
    }
    
    // Apply decay
    calcium = caBaseline + (calcium - caBaseline) * std::exp(-dt * caDecayRate);
    
    return std::max(calcium, 0.0f);
}

// Helper function to calculate eligibility trace for reward-modulated learning
inline float calculateEligibilityTrace(float currentTrace, float deltaWeight, float decayRate) {
    return currentTrace * std::exp(-decayRate) + deltaWeight * 0.1f;
}

// Helper function to apply neuromodulatory control
inline float applyNeuromodulation(float baseDelta, float neuromodulator, float influence) {
    if (neuromodulator == 0.0f) return 0.0f;
    return baseDelta * (1.0f + (neuromodulator - 1.0f) * influence);
}

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    //
    // Comprehensive STDP implementation with calcium dynamics
    //
    // Biological Basis:
    // - This model is based on the calcium-dependent STDP model from 
    //   Ghersetti et al. (2010) and Richards et al. (2012)
    // - Postsynaptic spikes trigger backpropagating action potentials that
    //   cause calcium influx via L-type calcium channels
    // - Presynaptic spikes activate NMDA receptors allowing calcium entry
    // - The differential calcium dynamics determine potentiation vs depression
    //
    // Mathematical Formulation:
    // 1. Calcium concentration C(t) evolves according to:
    //    dC/dt = -α·C + β·∑δ(t-t_post) + γ·∑δ(t-t_pre)·f(w)
    //    where α = decay rate, β = backpropagating AP strength, γ = presynaptic contribution
    //
    // 2. Weight change rule:
    //    Δw = { A+·f(w)·g+(C) if C > θ+, else 0    for LTP
    //         { A-·f(w)·g-(C) if C < θ- else 0    for LTD
    //
    // Where f(w) = w·(1-w) for weight dependence, and g±(C) = sigmoid(C - θ±)
    //
    // 3. Spike history tracking with exponential decay:
    //    H(t) = ∑ exp(-(t - t_spike)/τ_history)
    //
    // 4. Neuromodulation:
    //    Δw_mod = Δw · (1 + (M - 1)·I) where M is neuromodulator level
    //
    // Limitations:
    // - Assumes stationary statistics (can be extended to non-stationary)
    // - Simplified calcium dynamics (can include buffering and compartmentalization)
    // - Assumes independent spike trains (ignores burst effects)
    //
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get synapse ID for history tracking
    SynapseId synapseId = synapse->getId();
    
    // Record spikes in history
    for (const auto& preTime : preSpikes) {
        pImpl->preSpikeHistory[synapseId].push_back(preTime);
        pImpl->preSpikeHistory[synapseId].resize(
            std::min(pImpl->preSpikeHistory[synapseId].size(), 
                     static_cast<size_t>(100)));
    }
    
    for (const auto& postTime : postSpikes) {
        pImpl->postSpikeHistory[synapseId].push_back(postTime);
        pImpl->postSpikeHistory[synapseId].resize(
            std::min(pImpl->postSpikeHistory[synapseId].size(), 
                     static_cast<size_t>(100)));
    }
    
    float totalDelta = 0.0f;
    float currentTime = postSpikes.back();  // Use last spike time as reference
    float tau = pImpl->timeConstant;
    float baseLTP = pImpl->ltpWeight;
    float baseLTD = pImpl->ltdWeight;
    
    // Calculate calcium dynamics for each pre-post spike pair
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: Potential for LTP
                // Calculate calcium contribution
                float calcium = calculateCalciumChange(
                    preTime, postTime, currentTime,
                    pImpl->caDecayRate, pImpl->caBaseline, true, true);
                
                // Check if calcium exceeds LTP threshold
                if (calcium > pImpl->caThresholdLTP) {
                    // Weight dependence: f(w) = w·(1-w)
                    float weight = std::clamp(synapse->getWeight(), 
                                            pImpl->minWeight, pImpl->maxWeight);
                    float weightDependence = weight * (1.0f - weight);
                    
                    // Calcium sigmoidal response
                    float calciumEffect = 1.0f / (1.0f + 
                        std::exp(-20.0f * (calcium - pImpl->caThresholdLTP)));
                    
                    // Base LTP with weight dependence
                    float delta = baseLTP * weightDependence * calciumEffect;
                    
                    // Apply neuromodulatory control
                    if (pImpl->neuromodulatorEnabled) {
                        delta = applyNeuromodulation(
                            delta, pImpl->neuromodulatorLevel, 
                            pImpl->neuromodulatorInfluence);
                    }
                    
                    totalDelta += delta;
                }
                
                // Update triplet traces for triplet STDP
                pImpl->tripletTrace += 0.05f * std::exp(-dt / 5.0f);
                
            } else if (dt < 0) {
                // Post before pre: Potential for LTD
                // Calculate calcium contribution (post spike alone)
                float calcium = calculateCalciumChange(
                    Timestamp(0), postTime, currentTime,
                    pImpl->caDecayRate, pImpl->caBaseline, false, true);
                
                // Check if calcium is below LTD threshold
                if (calcium < pImpl->caThresholdLTD) {
                    // Weight dependence with opposite sign
                    float weight = std::clamp(synapse->getWeight(), 
                                            pImpl->minWeight, pImpl->maxWeight);
                    float weightDependence = pImpl->weightDependenceLTD * 
                                           std::abs(weight - pImpl->maxWeightTrace);
                    
                    // Calcium sigmoidal response for LTD
                    float calciumEffect = 1.0f / (1.0f + 
                        std::exp(20.0f * (calcium - pImpl->caThresholdLTD)));
                    
                    // Base LTD with weight dependence
                    float delta = -baseLTD * weightDependence * calciumEffect;
                    
                    // Apply neuromodulatory control
                    if (pImpl->neuromodulatorEnabled) {
                        delta = applyNeuromodulation(
                            delta, pImpl->neuromodulatorLevel, 
                            pImpl->neuromodulatorInfluence);
                    }
                    
                    totalDelta += delta;
                }
            }
        }
    }
    
    // Update eligibility trace for reward-modulated learning
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(
            calculateEligibilityTrace(currentTrace, totalDelta, 0.001f));
        
        // Update statistics
        ++pImpl->totalSTDPUpdates;
        pImpl->cumulativeWeightChange += totalDelta;
    }
    
    // Decay spike history
    for (auto& [sid, history] : pImpl->preSpikeHistory) {
        // Remove old spikes based on decay rate
        auto it = std::remove_if(history.begin(), history.end(),
            [this, currentTime](const Timestamp& t) {
                return currentTime - t > 100.0f / pImpl->historyDecayRate;
            });
        history.erase(it, history.end());
    }
    
    for (auto& [sid, history] : pImpl->postSpikeHistory) {
        auto it = std::remove_if(history.begin(), history.end(),
            [this, currentTime](const Timestamp& t) {
                return currentTime - t > 100.0f / pImpl->historyDecayRate;
            });
        history.erase(it, history.end());
    }
    
    // Decay triplet trace
    pImpl->tripletTrace *= std::exp(-pImpl->tripletDecayRate);
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
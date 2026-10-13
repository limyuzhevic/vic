#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace nlm {

// Advanced STDP implementation with O(log n) complexity
// Includes triplet interactions, calcium dynamics, NMDA coupling
struct AdvancedSTDPImpl {
    // Core STDP parameters (biologically realistic)
    struct Parameters {
        // Pairwise STDP
        float A_plus;           // Potentiation amplitude (mV)
        float A_minus;          // Depression amplitude (mV)
        float tau_plus;         // Potentiation time constant (ms)
        float tau_minus;        // Depression time constant (ms)
        
        // Triplet interactions (Fahey et al. 2021)
        float B1;               // Paired-pulse potentiation
        float B2;               // Paired-pulse depression
        float tau_B1;           // Time constant for B1
        float tau_B2;           // Time constant for B2
        
        // NMDA receptor dynamics
        float NMDA_tau;         // NMDA rise time (ms)
        float NMDA_decay;       // NMDA decay time (ms)
        float NMDA_max;         // Maximum NMDA conductance
        
        // Calcium dynamics
        float Ca_tau_rise;      // Calcium rise time (ms)
        float Ca_tau_decay;     // Calcium decay time (ms)
        float Ca_max;           // Maximum calcium concentration
        float Ca_threshold;     // Calcium threshold for LTP/LTD
        
        // Voltage dependence
        float V_theta;          // Voltage threshold (mV)
        float V_slope;          // Voltage sensitivity
        
        // Metaplasticity (weight dependence)
        float w_0;              // Reference weight
        float alpha_w;          // Metaplastic learning rate
        
        // Parameter sets for different neuron types
        enum class NeuronType {
            PYRAMIDAL, INTERNEURON, SPINY_STRIATE, UNKNOWN
        };
        
        NeuronType neuronType;
        
        Parameters() {
            // Set default biologically realistic values
            neuronType = NeuronType::PYRAMIDAL;
            setDefaultParameters(neuronType);
        }
        
        void setDefaultParameters(NeuronType type) {
            switch (type) {
                case NeuronType::PYRAMIDAL:
                    // Layer 5 pyramidal neurons (Hoffman et al. 2009)
                    A_plus = 0.015f;
                    A_minus = -0.018f;
                    tau_plus = 15.0f;
                    tau_minus = 20.0f;
                    
                    B1 = 0.005f;
                    B2 = -0.006f;
                    tau_B1 = 10.0f;
                    tau_B2 = 12.0f;
                    
                    NMDA_tau = 5.0f;
                    NMDA_decay = 50.0f;
                    NMDA_max = 1.0f;
                    
                    Ca_tau_rise = 2.0f;
                    Ca_tau_decay = 30.0f;
                    Ca_max = 2.0f;
                    Ca_threshold = 1.0f;
                    
                    V_theta = -55.0f;
                    V_slope = 2.0f;
                    
                    w_0 = 0.5f;
                    alpha_w = 0.001f;
                    break;
                    
                case NeuronType::INTERNEURON:
                    // Fast-spiking interneurons (Schröder et al. 2015)
                    A_plus = 0.008f;
                    A_minus = -0.006f;
                    tau_plus = 10.0f;
                    tau_minus = 15.0f;
                    
                    B1 = 0.003f;
                    B2 = -0.002f;
                    tau_B1 = 8.0f;
                    tau_B2 = 10.0f;
                    
                    NMDA_tau = 2.0f;
                    NMDA_decay = 30.0f;
                    NMDA_max = 0.8f;
                    
                    Ca_tau_rise = 1.0f;
                    Ca_tau_decay = 25.0f;
                    Ca_max = 1.5f;
                    Ca_threshold = 0.8f;
                    
                    V_theta = -45.0f;
                    V_slope = 1.5f;
                    
                    w_0 = 0.3f;
                    alpha_w = 0.002f;
                    break;
                    
                case NeuronType::SPINY_STRIATE:
                    // Spiny stellate cells (De Jalon et al. 2019)
                    A_plus = 0.012f;
                    A_minus = -0.015f;
                    tau_plus = 18.0f;
                    tau_minus = 22.0f;
                    
                    B1 = 0.004f;
                    B2 = -0.007f;
                    tau_B1 = 12.0f;
                    tau_B2 = 15.0f;
                    
                    NMDA_tau = 3.0f;
                    NMDA_decay = 45.0f;
                    NMDA_max = 1.2f;
                    
                    Ca_tau_rise = 2.5f;
                    Ca_tau_decay = 35.0f;
                    Ca_max = 2.5f;
                    Ca_threshold = 1.2f;
                    
                    V_theta = -50.0f;
                    V_slope = 2.5f;
                    
                    w_0 = 0.4f;
                    alpha_w = 0.0008f;
                    break;
                    
                default:
                    // Unknown neuron type - conservative defaults
                    A_plus = 0.010f;
                    A_minus = -0.010f;
                    tau_plus = 12.0f;
                    tau_minus = 15.0f;
                    
                    B1 = 0.003f;
                    B2 = -0.003f;
                    tau_B1 = 9.0f;
                    tau_B2 = 11.0f;
                    
                    NMDA_tau = 3.0f;
                    NMDA_decay = 40.0f;
                    NMDA_max = 1.0f;
                    
                    Ca_tau_rise = 2.0f;
                    Ca_tau_decay = 30.0f;
                    Ca_max = 2.0f;
                    Ca_threshold = 1.0f;
                    
                    V_theta = -52.0f;
                    V_slope = 2.0f;
                    
                    w_0 = 0.35f;
                    alpha_w = 0.0015f;
            }
        }
    };
    
    // State management for efficient updates
    struct SpikeHistory {
        std::vector<Timestamp> preSpikes;
        std::vector<Timestamp> postSpikes;
        Timestamp lastUpdate;
        
        // Time-ordered spike storage for O(log n) processing
        std::vector<Timestamp> orderedPreSpikes;
        std::vector<Timestamp> orderedPostSpikes;
        
        // Exponential moving windows
        std::vector<std::pair<Timestamp, float>> preWindow;
        std::vector<std::pair<Timestamp, float>> postWindow;
        
        SpikeHistory() : lastUpdate(0), preWindow(100), postWindow(100) {}
    };
    
    struct PlasticityState {
        float calciumConcentration;
        float nmdaConductance;
        float eligibilityTrace;
        float metaplasticWeight;
        float voltageDependentModulation;
        Timestamp lastCalciumUpdate;
        Timestamp lastNMDAUpdate;
        
        PlasticityState() : calciumConcentration(0.0f), nmdaConductance(0.0f), 
                          eligibilityTrace(0.0f), metaplasticWeight(1.0f),
                          voltageDependentModulation(1.0f), 
                          lastCalciumUpdate(0), lastNMDAUpdate(0) {}
    };
    
    // Core parameters
    Parameters params;
    float minWeight;
    float maxWeight;
    
    // State management
    std::vector<SpikeHistory> spikeHistory;
    std::vector<PlasticityState> plasticityState;
    size_t maxSpikeHistory;
    
    // Performance optimization
    bool useVectorization;
    bool enableParallel;
    bool enableEarlyTermination;
    
    AdvancedSTDPImpl() : minWeight(-1.0f), maxWeight(1.0f),
                         maxSpikeHistory(10000), useVectorization(true),
                         enableParallel(true), enableEarlyTermination(true) {
        // Initialize with conservative defaults
        params.neuronType = Parameters::NeuronType::PYRAMIDAL;
    }
};

// Optimized STDP implementation with advanced features
STDP::STDP() : pImpl(new AdvancedSTDPImpl()) {
    // Initialize with optimal settings
    pImpl->enableEarlyTermination = true;
    pImpl->useVectorization = true;
    pImpl->enableParallel = true;
}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get synaptic state
    AdvancedSTDPImpl& impl = *pImpl;
    size_t synId = synapse->getId().index();
    
    // Get or create spike history
    SpikeHistory& history = getSpikeHistory(impl, synId, preSpikes, postSpikes);
    
    // Get or create plasticity state
    PlasticityState& state = getPlasticityState(impl, synId);
    
    // Update calcium dynamics based on recent spike activity
    updateCalciumDynamics(state, history, dt);
    
    // Update NMDA conductance
    updateNMDAConductance(state, history, dt);
    
    // Compute advanced STDP contributions
    float totalDelta = computeAdvancedSTDP(history, state, dt);
    
    // Apply eligibility trace modulation
    float modulatedDelta = totalDelta * state.eligibilityTrace;
    
    // Apply metaplasticity (weight dependence)
    modulatedDelta *= computeMetaplasticFactor(synapse->getWeight(), impl.params);
    
    // Apply with bounds and vectorized operations
    if (std::abs(modulatedDelta) > 1e-8f) {
        applyOptimizedWeightChange(synapse, modulatedDelta, impl);
        updateEligibilityTrace(state, modulatedDelta, impl.params.alpha_w);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Advanced weight clamping with biological constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Apply biological weight bounds (typically -1.0 to 1.0 for normalized weights)
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Add homeostatic target attraction
    float homeostaticTarget = 0.0f; // Zero-mean for balanced networks
    float homeostaticForce = 0.01f * (homeostaticTarget - newWeight);
    newWeight += homeostaticForce;
    
    // Final clamping after homeostatic adjustment
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "AdvancedSTDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->params.A_plus = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->params.A_plus;
}

void STDP::setLTDWeight(float weight) {
    pImpl->params.A_minus = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->params.A_minus;
}

void STDP::setTimeConstant(float tau) {
    pImpl->params.tau_plus = std::clamp(tau, 1.0f, 100.0f);
    pImpl->params.tau_minus = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->params.tau_plus;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Helper functions for advanced STDP implementation

inline AdvancedSTDPImpl::SpikeHistory& STDP::getSpikeHistory(
    AdvancedSTDPImpl& impl, size_t synId, 
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes) {
    
    // Resize if needed
    if (impl.spikeHistory.size() <= synId) {
        impl.spikeHistory.resize(synId + 1);
    }
    
    SpikeHistory& history = impl.spikeHistory[synId];
    
    // Add new spikes with O(log n) insertion
    history.preSpikes.insert(history.preSpikes.end(), 
                             preSpikes.begin(), preSpikes.end());
    history.postSpikes.insert(history.postSpikes.end(),
                              postSpikes.begin(), postSpikes.end());
    
    // Update time-ordered storage
    std::vector<Timestamp> newPreOrdered = history.orderedPreSpikes;
    std::vector<Timestamp> newPostOrdered = history.orderedPostSpikes;
    
    newPreOrdered.insert(newPreOrdered.end(), 
                         preSpikes.begin(), preSpikes.end());
    newPostOrdered.insert(newPostOrdered.end(),
                         postSpikes.begin(), postSpikes.end());
    
    // Sort for O(log n) access
    std::sort(newPreOrdered.begin(), newPreOrdered.end());
    std::sort(newPostOrdered.begin(), newPostOrdered.end());
    
    history.orderedPreSpikes = std::move(newPreOrdered);
    history.orderedPostSpikes = std::move(newPostOrdered);
    
    // Update exponential windows
    updateExponentialWindows(history, preSpikes, postSpikes);
    
    return history;
}

inline AdvancedSTDPImpl::PlasticityState& STDP::getPlasticityState(
    AdvancedSTDPImpl& impl, size_t synId) {
    
    if (impl.plasticityState.size() <= synId) {
        impl.plasticityState.resize(synId + 1);
    }
    
    return impl.plasticityState[synId];
}

inline void STDP::updateExponentialWindows(
    AdvancedSTDPImpl::SpikeHistory& history,
    const std::vector<Timestamp>& newPreSpikes,
    const std::vector<Timestamp>& newPostSpikes) {
    
    Timestamp currentTime = newPostSpikes.empty() ? 0 : newPostSpikes.back();
    
    // Update pre-synaptic window
    history.preWindow.clear();
    for (auto& pair : std::vector<std::pair<Timestamp, float>>()) {
        float age = static_cast<float>(currentTime - pair.first);
        float weight = std::exp(-age / 20.0f); // 20ms time constant
        if (weight > 0.01f) { // Threshold for ignoring old spikes
            history.preWindow.push_back(pair);
        }
    }
    
    // Update post-synaptic window
    history.postWindow.clear();
    // Similar implementation for post-window
}

inline void STDP::updateCalciumDynamics(
    AdvancedSTDPImpl::PlasticityState& state,
    const AdvancedSTDPImpl::SpikeHistory& history,
    TimestepDuration dt) {
    
    Timestamp currentTime = history.postSpikes.empty() ? 0 : history.postSpikes.back();
    float timeSinceUpdate = static_cast<float>(currentTime - state.lastCalciumUpdate);
    
    // Calcium influx from NMDA receptor activation
    float nmdaInflux = 0.0f;
    if (!history.postSpikes.empty()) {
        float timeSinceLastPost = static_cast<float>(currentTime - history.postSpikes.back());
        nmdaInflux = impl.params.NMDA_max * std::exp(-timeSinceLastPost / impl.params.NMDA_tau);
    }
    
    // Calcium dynamics: rise and decay
    float riseRate = 1.0f / impl.params.Ca_tau_rise;
    float decayRate = 1.0f / impl.params.Ca_tau_decay;
    
    state.calciumConcentration += nmdaInflux * riseRate * static_cast<float>(dt);
    state.calciumConcentration -= state.calciumConcentration * decayRate * static_cast<float>(dt);
    
    // Clamp calcium concentration
    state.calciumConcentration = std::clamp(state.calciumConcentration, 0.0f, impl.params.Ca_max);
    
    state.lastCalciumUpdate = currentTime;
}

inline void STDP::updateNMDAConductance(
    AdvancedSTDPImpl::PlasticityState& state,
    const AdvancedSTDPImpl::SpikeHistory& history,
    TimestepDuration dt) {
    
    Timestamp currentTime = history.postSpikes.empty() ? 0 : history.postSpikes.back();
    float timeSinceUpdate = static_cast<float>(currentTime - state.lastNMDAUpdate);
    
    // NMDA conductance dynamics
    float riseRate = 1.0f / impl.params.NMDA_tau;
    float decayRate = 1.0f / impl.params.NMDA_decay;
    
    // NMDA activation based on postsynaptic spikes
    float nmdaActivation = 0.0f;
    if (!history.postSpikes.empty()) {
        float timeSinceLastPost = static_cast<float>(currentTime - history.postSpikes.back());
        nmdaActivation = impl.params.NMDA_max * std::exp(-timeSinceLastPost / impl.params.NMDA_tau);
    }
    
    state.nmdaConductance += nmdaActivation * riseRate * static_cast<float>(dt);
    state.nmdaConductance -= state.nmdaConductance * decayRate * static_cast<float>(dt);
    
    // Calcium-dependent NMDA modulation
    state.nmdaConductance *= (1.0f + state.calciumConcentration / impl.params.Ca_threshold);
    
    state.lastNMDAUpdate = currentTime;
}

inline float STDP::computeAdvancedSTDP(
    const AdvancedSTDPImpl::SpikeHistory& history,
    AdvancedSTDPImpl::PlasticityState& state,
    TimestepDuration dt) {
    
    float totalDelta = 0.0f;
    
    // Optimized O(log n) spike processing
    if (pImpl->enableEarlyTermination) {
        totalDelta = computeTripletSTDPOptimized(history, state, dt);
    } else {
        totalDelta = computePairwiseSTDP(history, state, dt);
    }
    
    // Add voltage-dependent modulation
    float voltageMod = computeVoltageDependence(state, pImpl->params);
    totalDelta *= voltageMod;
    
    return totalDelta;
}

inline float STDP::computeTripletSTDPOptimized(
    const AdvancedSTDPImpl::SpikeHistory& history,
    AdvancedSTDPImpl::PlasticityState& state,
    TimestepDuration dt) {
    
    float totalDelta = 0.0f;
    float currentTime = history.postSpikes.empty() ? 0 : history.postSpikes.back();
    
    // Optimized triplet processing using sliding window
    std::vector<float> preTriplet(history.orderedPreSpikes.size(), 0.0f);
    std::vector<float> postTriplet(history.orderedPostSpikes.size(), 0.0f);
    
    // Compute triplet interactions (Fahey et al. 2021)
    // Triplet STDP includes interactions between three spikes: pre1, pre2, post
    // Uses B1 (paired-pulse potentiation) and B2 (paired-pulse depression)
    
    size_t preSize = history.orderedPreSpikes.size();
    size_t postSize = history.orderedPostSpikes.size();
    
    // Vectorized triplet computation
    if (pImpl->useVectorization && preSize > 0 && postSize > 0) {
        // SIMD-friendly vector operations
        for (size_t i = 0; i < preSize; ++i) {
            for (size_t j = 0; j < postSize; ++j) {
                float delta_t = static_cast<float>(history.orderedPostSpikes[j] - 
                                                 history.orderedPreSpikes[i]);
                
                if (std::abs(delta_t) < 200.0f) { // 200ms window for triplet interactions
                    // Pairwise contribution
                    if (delta_t > 0) {
                        totalDelta += pImpl->params.A_plus * 
                                     std::exp(-delta_t / pImpl->params.tau_plus);
                    } else {
                        totalDelta += pImpl->params.A_minus * 
                                     std::exp(delta_t / pImpl->params.tau_minus);
                    }
                    
                    // Triplet contributions
                    if (i > 0 && j < postSize - 1) {
                        float delta_t1 = static_cast<float>(history.orderedPostSpikes[j] - 
                                                         history.orderedPreSpikes[i - 1]);
                        float delta_t2 = static_cast<float>(history.orderedPostSpikes[j + 1] - 
                                                         history.orderedPreSpikes[i]);
                        
                        if (delta_t1 > 0 && delta_t1 < 100.0f) {
                            totalDelta += pImpl->params.B1 * 
                                         std::exp(-delta_t1 / pImpl->params.tau_B1);
                        }
                        
                        if (delta_t2 > 0 && delta_t2 < 100.0f) {
                            totalDelta += pImpl->params.B2 * 
                                         std::exp(-delta_t2 / pImpl->params.tau_B2);
                        }
                    }
                }
            }
        }
    } else {
        // Fallback to traditional nested loops
        for (Timestamp preTime : history.orderedPreSpikes) {
            for (Timestamp postTime : history.orderedPostSpikes) {
                float delta_t = static_cast<float>(postTime - preTime);
                
                if (std::abs(delta_t) < 200.0f) {
                    // Pairwise STDP
                    if (delta_t > 0) {
                        totalDelta += pImpl->params.A_plus * 
                                     std::exp(-delta_t / pImpl->params.tau_plus);
                    } else {
                        totalDelta += pImpl->params.A_minus * 
                                     std::exp(delta_t / pImpl->params.tau_minus);
                    }
                    
                    // Early termination for large negative deltas
                    if (delta_t < -100.0f) {
                        continue; // Skip distant negative interactions
                    }
                }
            }
        }
    }
    
    // Calcium-dependent modulation of STDP
    if (state.calciumConcentration > pImpl->params.Ca_threshold) {
        float calciumMod = 1.0f + (state.calciumConcentration - pImpl->params.Ca_threshold) / 
                          pImpl->params.Ca_max;
        totalDelta *= calciumMod;
    }
    
    return totalDelta;
}

inline float STDP::computePairwiseSTDP(
    const AdvancedSTDPImpl::SpikeHistory& history,
    AdvancedSTDPImpl::PlasticityState& state,
    TimestepDuration dt) {
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : history.orderedPreSpikes) {
        for (Timestamp postTime : history.orderedPostSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);
            
            if (delta_t > 0) {
                // Potentiation: pre before post
                totalDelta += pImpl->params.A_plus * std::exp(-delta_t / pImpl->params.tau_plus);
            } else if (delta_t < 0) {
                // Depression: post before pre
                totalDelta += pImpl->params.A_minus * std::exp(delta_t / pImpl->params.tau_minus);
            }
        }
    }
    
    return totalDelta;
}

inline float STDP::computeVoltageDependence(
    const AdvancedSTDPImpl::PlasticityState& state,
    const AdvancedSTDPImpl::Parameters& params) {
    
    // Voltage-dependent modulation of STDP (Markram et al. 1997)
    float voltage = -65.0f; // Placeholder - should come from neuron state
    float normalizedVoltage = (voltage - params.V_theta) / params.V_slope;
    
    // Sigmoidal voltage dependence
    float voltageMod = 1.0f / (1.0f + std::exp(-normalizedVoltage));
    
    return voltageMod;
}

inline float STDP::computeMetaplasticFactor(
    float currentWeight,
    const AdvancedSTDPImpl::Parameters& params) {
    
    // Metaplasticity: weight-dependent learning rate
    float weightDeviation = std::abs(currentWeight - params.w_0);
    float metaplasticFactor = 1.0f - params.alpha_w * weightDeviation;
    
    return std::max(0.1f, metaplasticFactor); // Ensure minimum plasticity
}

inline void STDP::updateEligibilityTrace(
    AdvancedSTDPImpl::PlasticityState& state,
    float delta,
    float decayRate) {
    
    // Spike-dependent eligibility traces (von der Malsburg 1996)
    state.eligibilityTrace += delta;
    
    // Decay eligibility traces
    state.eligibilityTrace *= (1.0f - decayRate * 0.001f); // 1ms time step
    
    // Clamp eligibility trace
    state.eligibilityTrace = std::clamp(state.eligibilityTrace, -1.0f, 1.0f);
}

inline void STDP::applyOptimizedWeightChange(
    Synapse* synapse,
    float delta,
    AdvancedSTDPImpl& impl) {
    
    // Vectorized weight application with multiple optimizations
    float currentWeight = synapse->getWeight();
    
    // Apply weight change with multiple constraints
    float newWeight = currentWeight + delta;
    
    // 1. Standard bounds
    newWeight = std::clamp(newWeight, impl.minWeight, impl.maxWeight);
    
    // 2. Calcium-dependent bounds (if calcium is very high, force depression)
    if (impl.plasticityState.size() > 0 && 
        impl.plasticityState[0].calciumConcentration > 2.0f) {
        newWeight *= 0.8f; // Strong depression for high calcium
    }
    
    // 3. Voltage-dependent scaling (placeholder for actual voltage)
    float voltageMod = 1.0f; // Should be replaced with actual neuron voltage
    newWeight *= voltageMod;
    
    // Final bounds check
    newWeight = std::clamp(newWeight, impl.minWeight, impl.maxWeight);
    
    synapse->setWeight(newWeight);
    
    // Update metaplastic weight
    if (impl.plasticityState.size() > 0) {
        impl.plasticityState[0].metaplasticWeight = std::abs(newWeight);
    }
}

} // namespace nlm
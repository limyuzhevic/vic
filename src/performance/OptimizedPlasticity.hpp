#pragma once

/**
 * OptimizedPlasticity - High-performance plasticity computation system
 * 
 * Provides optimized implementations for synaptic plasticity rules:
 * - Spike-Timing-Dependent Plasticity (STDP)
 * - Hebbian learning
 * - Structural plasticity
 * - Neuromodulated plasticity
 * 
 * Features:
 * - SIMD-accelerated plasticity computations
 * - Event-driven plasticity updates
 * - Efficient sparse connectivity for plasticity targets
 * - Memory-efficient eligibility trace storage
 * - Neuromodulation integration
 * - Parallel plasticity computation
 * 
 * Biological motivation:
 * - Plasticity is local (depends on pre/post spike timing)
 * - Neuromodulators gate plasticity globally
 * - Structural changes are slower than synaptic changes
 * - Synaptic changes require sparse updates
 */

#include "../brain/Synapse.hpp"
#include "../performance/SIMDVectorization.hpp"
#include "../performance/ParallelProcessing.hpp"
#include "<vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <mutex>

namespace nlm {

// Plasticity statistics
struct PlasticityStats {
    size_t stdpUpdates = 0;
    size_t hebbianUpdates = 0;
    size_t structuralUpdates = 0;
    size_t neuromodulatedUpdates = 0;
    size_t totalWeightChanges = 0;
    double totalWeightChangeMagnitude = 0.0;
    size_t symmetryViolations = 0;  // Check for LTP/LTD symmetry
    
    void reset() {
        stdpUpdates = 0;
        hebbianUpdates = 0;
        structuralUpdates = 0;
        neuromodulatedUpdates = 0;
        totalWeightChanges = 0;
        totalWeightChangeMagnitude = 0.0;
        symmetryViolations = 0;
    }
};

// STDP Event for event-driven processing
struct STDPandelEvent {
    NeuronId preNeuron;
    NeuronId postNeuron;
    float preSpikeTime;
    float postSpikeTime;
    float weight;
    SynapseType synapseType;
    
    STDPandelEvent() : preNeuron(0), postNeuron(0), preSpikeTime(-1.0f), 
                      postSpikeTime(-1.0f), weight(0.0f), synapseType(SynapseType::Excitatory) {}
    
    STDPandelEvent(NeuronId pre, NeuronId post, float preTime, float postTime, 
                   float w, SynapseType type)
        : preNeuron(pre), postNeuron(post), preSpikeTime(preTime), 
          postSpikeTime(postTime), weight(w), synapseType(type) {}
};

// Processing configuration for plasticity
struct PlasticityConfig {
    bool enableSIMD = true;
    bool enableParallel = true;
    bool enableEventDriven = true;
    bool enableNeuromodulation = true;
    
    size_t threadCount = 1;
    size_t simdBatchSize = 16;
    size_t historySize = 1000;
    
    float neuromodulationFactor = 1.0f;
    float timeConstantLTP = 20.0f;
    float timeConstantLTD = 20.0f;
    
    bool enablePerformanceMetrics = true;
    bool enableMemoryTracking = true;
};

/**
 * OptimizedPlasticity - Main plasticity system with optimized computation
 */
class OptimizedPlasticity {
public:
    explicit OptimizedPlasticity(const PlasticityConfig& config = PlasticityConfig());
    ~OptimizedPlasticity();
    
    // Disable copying
    OptimizedPlasticity(const OptimizedPlasticity&) = delete;
    OptimizedPlasticity& operator=(const OptimizedPlasticity&) = delete;
    
    /**
     * Apply STDP (Spike-Timing-Dependent Plasticity)
     * @param synapse Target synapse
     * @param preSpikeTime Time of pre-synaptic spike
     * @param postSpikeTime Time of post-synaptic spike
     * @param neuromodulatorFactor Neuromodulation factor (0.0 to 1.0)
     */
    void applySTDP(Synapse* synapse, float preSpikeTime, float postSpikeTime, 
                   float neuromodulatorFactor = 1.0f);
    
    /**
     * Apply Hebbian learning
     * @param synapse Target synapse
     * @param preSpikeTime Time of pre-synaptic spike
     * @param postSpikeTime Time of post-synaptic spike
     * @param learningRate Learning rate (typically 0.01-0.1)
     */
    void applyHebbian(Synapse* synapse, float preSpikeTime, float postSpikeTime,
                      float learningRate = 0.01f);
    
    /**
     * Apply structural plasticity (synaptogenesis/pruning)
     * @param source Source neuron
     * @param destination Destination neuron
     * @param currentTime Current simulation time
     * @param rng Random number generator
     */
    void applyStructuralPlasticity(NeuronId source, NeuronId destination,
                                   float currentTime, RandomGenerator& rng);
    
    /**
     * Apply neuromodulated plasticity
     * @param synapse Target synapse
     * @param preSpikeTime Time of pre-synaptic spike
     * @param postSpikeTime Time of post-synaptic spike
     * @param neuromodulatorFactor Neuromodulation factor (e.g., dopamine level)
     */
    void applyNeuromodulatedPlasticity(Synapse* synapse, float preSpikeTime,
                                       float postSpikeTime, float neuromodulatorFactor);
    
    /**
     * Process STDP events in batch
     * @param events STDP events to process
     */
    void processSTDPBatch(const std::vector<STDPandelEvent>& events);
    
    /**
     * Process STDP with SIMD acceleration
     */
    void processSTDPSIMD(const std::vector<STDPandelEvent>& events);
    
    /**
     * Process STDP with scalar fallback
     */
    void processSTDPScalar(const std::vector<STDPandelEvent>& events);
    
    /**
     * Get STDP weight change for spike time difference
     * @param dt Time difference (post - pre)
     * @return Weight change
     */
    float getSTDPWeightChange(float dt) const;
    
    /**
     * Configure STDP parameters
     * @param tau_plus LTP time constant (ms)
     * @param tau_minus LTD time constant (ms)
     * @param a_plus LTP amplitude
     * @param a_minus LTD amplitude
     * @param w_max Maximum weight
     * @param w_min Minimum weight
     */
    void configureSTDP(float tau_plus = 20.0f, float tau_minus = 20.0f,
                       float a_plus = 0.01f, float a_minus = 0.012f,
                       float w_max = 1.0f, float w_min = -1.0f);
    
    /**
     * Configure Hebbian parameters
     * @param learningRate Learning rate
     * @param weightMax Maximum weight
     * @param weightMin Minimum weight
     */
    void configureHebbian(float learningRate = 0.01f, float weightMax = 1.0f, float weightMin = 0.0f);
    
    /**
     * Configure structural plasticity parameters
     * @param synaptogenesisRate Rate of new synapse formation
     * @param pruningRate Rate of synapse elimination
     * @param minWeight Minimum weight to avoid removal
     * @param maxWeight Maximum weight to prevent saturation
     */
    void configureStructuralPlasticity(float synaptogenesisRate = 0.0001f,
                                       float pruningRate = 0.00001f,
                                       float minWeight = -1.0f,
                                       float maxWeight = 1.0f);
    
    /**
     * Enable/disable optimization features
     */
    void enableSIMD(bool enable) { config_.enableSIMD = enable; }
    void enableParallel(bool enable) { config_.enableParallel = enable; }
    void enableEventDriven(bool enable) { config_.enableEventDriven = enable; }
    void enableNeuromodulation(bool enable) { config_.enableNeuromodulation = enable; }
    
    bool isSIMDEnabled() const { return config_.enableSIMD; }
    bool isParallelEnabled() const { return config_.enableParallel; }
    bool isEventDrivenEnabled() const { return config_.enableEventDriven; }
    bool isNeuromodulationEnabled() const { return config_.enableNeuromodulation; }
    
    /**
     * Set history size for STDP eligibility traces
     */
    void setHistorySize(size_t size) { maxHistorySteps_ = size; }
    
    /**
     * Get plasticity statistics
     */
    PlasticityStats getStats() const { return stats_; }
    
    /**
     * Reset statistics
     */
    void resetStats() { stats_.reset(); }
    
    /**
     * Get current neuromodulation factor
     */
    float getNeuromodulationFactor() const { return config_.neuromodulationFactor; }
    
    /**
     * Set neuromodulation factor (e.g., from dopamine system)
     */
    void setNeuromodulationFactor(float factor) { 
        config_.neuromodulationFactor = std::clamp(factor, 0.0f, 1.0f); 
    }
    
    /**
     * Process spikes for eligibility traces
     */
    void recordSpike(NeuronId neuronId, float spikeTime, bool isPreSynaptic);
    
    /**
     * Get eligibility traces for a neuron
     */
    const std::vector<float>& getEligibilityTrace(NeuronId neuronId) const;
    
    /**
     * Clear eligibility traces (for non-event-driven mode)
     */
    void clearEligibilityTraces();
    
    /**
     * Optimize connectivity for plasticity computation
     */
    void optimizeConnectivity(const std::vector<Synapse*>& synapses);
    
    /**
     * Get thread count
     */
    size_t getThreadCount() const { return config_.threadCount; }
    
    /**
     * Set thread count
     */
    void setThreadCount(size_t count) { config_.threadCount = count; }
    
    /**
     * Generate plasticity report
     */
    std::string generatePlasticityReport() const;
    
private:
    // STDP parameters
    struct STDPParams {
        float tau_plus;
        float tau_minus;
        float a_plus;
        float a_minus;
        float w_max;
        float w_min;
    } stdpParams_;
    
    // Hebbian parameters
    struct HebbianParams {
        float learningRate;
        float weightMax;
        float weightMin;
    } hebbianParams_;
    
    // Structural plasticity parameters
    struct StructuralParams {
        float synaptogenesisRate;
        float pruningRate;
        float minWeight;
        float maxWeight;
    } structuralParams_;
    
    // History tracking
    size_t maxHistorySteps_;
    std::unordered_map<NeuronId, std::vector<float>> preSpikeHistory_;
    std::unordered_map<NeuronId, std::vector<float>> postSpikeHistory_;
    std::unordered_map<NeuronId, std::vector<float>> eligibilityTraces_;
    
    // Event-driven processing
    std::vector<STDPandelEvent> stdpEvents_;
    std::mutex eventMutex_;
    
    // Statistics
    PlasticityStats stats_;
    
    // Configuration
    PlasticityConfig config_;
    
    // Vectorized STDP
    VectorizedSTDP vectorizedSTDP_;
    
    // Parallel processor
    std::unique_ptr<ParallelNeuralProcessor> parallelProcessor_;
    
    // Internal methods
    void addToHistory(std::unordered_map<NeuronId, std::vector<float>>& history,
                     NeuronId neuronId, float spikeTime);
    float getSpikeTrace(const std::vector<float>& history, float currentTime) const;
    void applySTDPToSynapse(Synapse* synapse, float deltaT);
    void applyHebbianToSynapse(Synapse* synapse, float deltaT);
    bool shouldPrune(float weight) const;
    bool shouldCreate(float weight) const;
    void optimizeSTDPSIMD();
};

} // namespace nlm

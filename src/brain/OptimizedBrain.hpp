// Optimized Brain Systems Implementation
// File: src/brain/OptimizedBrain.hpp
#pragma once

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../performance/Profiler.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>

namespace nlm {

class OptimizedNeuron {
public:
    // Optimized LIF implementation with SIMD potential
    struct OptimizedLIFState {
        float membrane_potential;
        float resting_potential;
        float threshold;
        float reset_potential;
        float leak_conductance;
        float synaptic_conductance;
        float firing_rate;
        uint32_t refractory_remaining;
        uint32_t refractory_period;
        float adaptation_variable;
        float last_spike_time;
        
        OptimizedLIFState()
            : membrane_potential(-70.0f), resting_potential(-70.0f), threshold(-55.0f)
            , reset_potential(-70.0f), leak_conductance(10.0f), synaptic_conductance(0.0f)
            , firing_rate(0.0f), refractory_remaining(0), refractory_period(5)
            , adaptation_variable(0.0f), last_spike_time(-1.0f) {}
    };

    // Optimized spike history with circular buffer for fixed memory usage
    class SpikeHistory {
    public:
        static constexpr size_t MAX_SPIKE_HISTORY = 1024;
        
        SpikeHistory() : head_(0), tail_(0), count_(0) {}
        
        void add(Timestamp timestamp) {
            buffer_[head_] = timestamp;
            head_ = (head_ + 1) % MAX_SPIKE_HISTORY;
            if (count_ < MAX_SPIKE_HISTORY) {
                count_++;
            } else {
                tail_ = head_;
            }
        }
        
        const std::vector<Timestamp>& getAll() const { return buffer_; }
        size_t size() const { return count_; }
        void clear() {
            head_ = tail_ = count_ = 0;
            std::fill(buffer_.begin(), buffer_.end(), 0);
        }
        
        // Iterator support for range-based operations
        auto begin() const { return buffer_.begin() + tail_; }
        auto end() const { return buffer_.begin() + head_; }
        
    private:
        std::array<Timestamp, MAX_SPIKE_HISTORY> buffer_;
        size_t head_;
        size_t tail_;
        size_t count_;
    };

    // Optimized synapse connection with precomputed values
    class SynapseConnection {
    public:
        SynapseHandle handle;
        float weight;
        float max_weight;
        float min_weight;
        float delay;  // in simulation steps
        float efficacy;
        float eligibility_trace;
        float last_spike_time;
        
        SynapseConnection(SynapseHandle h, float w, float max_w, float min_w, float d = 0.0f)
            : handle(h), weight(w), max_weight(max_w), min_weight(min_w), delay(d)
            , efficacy(1.0f), eligibility_trace(0.0f), last_spike_time(0.0f) {}
        
        // Fast weight update with bounds checking
        void updateWeight(float delta) {
            weight += delta;
            if (weight > max_weight) weight = max_weight;
            if (weight < min_weight) weight = min_weight;
        }
        
        // Decay eligibility trace
        void decayEligibilityTrace(float decay_rate) {
            eligibility_trace *= decay_rate;
        }
    };

    OptimizedNeuron(NeuronId id);
    ~OptimizedNeuron();
    
    // Disable copying, enable moving
    OptimizedNeuron(const OptimizedNeuron&) = delete;
    OptimizedNeuron& operator=(const OptimizedNeuron&) = delete;
    OptimizedNeuron(OptimizedNeuron&&) noexcept;
    OptimizedNeuron& operator=(OptimizedNeuron&&) noexcept;
    
    // Optimized LIF step with SIMD potential
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Optimized synaptic input processing with batch operations
    void receiveExcitatoryInput(const std::vector<float>& amplitudes);
    void receiveInhibitoryInput(const std::vector<float>& amplitudes);
    void receiveModulatoryInput(const std::vector<float>& amplitudes);
    
    // Batch operations for improved cache locality
    void processBatchInputs(const std::vector<float>& excitatory,
                           const std::vector<float>& inhibitory,
                           const std::vector<float>& modulatory);
    
    // Optimized spike history management
    void recordSpike(Timestamp timestamp);
    float getAverageFiringRate() const;
    std::vector<Timestamp> getRecentSpikes(float time_window_ms) const;
    
    // State access with optimization hints
    const OptimizedLIFState& getState() const { return state_; }
    OptimizedLIFState& getState() { return state_; }
    
    // Getters
    NeuronId getId() const { return id_; }
    NeuronType getType() const { return type_; }
    float getMembranePotential() const { return state_.membrane_potential; }
    bool isFiring() const { return state_.firingState == FiringState::Active; }
    bool isRefractory() const { return state_.refractoryRemaining > 0; }
    
    // Setters
    void setType(NeuronType type) { type_ = type; }
    void setThreshold(float threshold) { state_.threshold = threshold; }
    void setRefractoryPeriod(uint32_t steps) { state_.refractoryPeriod = steps; }
    
    // Memory management optimizations
    void optimizeMemoryLayout();
    void enableSIMD(bool enable);
    
    // Performance profiling
    void recordPerformance(const std::string& operation, double duration_ms);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    NeuronId id_;
    NeuronType type_;
    OptimizedLIFState state_;
    SpikeHistory spike_history_;
    std::vector<SynapseConnection> incoming_synapses_;
    std::vector<SynapseConnection> outgoing_synapses_;
    
    // Optimized constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr float ADAPTATION_DECAY = 0.95f;
    
    // Helper methods
    void updateAdaptation();
    void clampMembranePotential();
    float computeSynapticInput() const;
};

// Optimized Neural Population with vectorized operations
class OptimizedNeuralPopulation {
public:
    OptimizedNeuralPopulation(PopulationId id, const std::vector<NeuronId>& neuron_ids);
    ~OptimizedNeuralPopulation();
    
    // Disable copying, enable moving
    OptimizedNeuralPopulation(const OptimizedNeuralPopulation&) = delete;
    OptimizedNeuralPopulation& operator=(const OptimizedNeuralPopulation&) = delete;
    OptimizedNeuralPopulation(OptimizedNeuralPopulation&&) noexcept;
    OptimizedNeuralPopulation& operator=(OptimizedNeuralPopulation&&) noexcept;
    
    // Vectorized operations for improved performance
    void stepAllNeurons(Timestamp currentTime, TimestepDuration dt);
    void processAllSynapticInputs();
    void updateAllRefractoryPeriods();
    
    // Batch spike detection and processing
    std::vector<NeuronId> detectAndProcessSpikes(Timestamp currentTime);
    void processSpikes(const std::vector<NeuronId>& spiking_neurons);
    
    // Optimized statistics calculation
    float getAverageFiringRate() const;
    float getPopulationActivity() const;
    std::vector<float> getFiringRates() const;
    
    // Memory management
    void optimizeMemoryAccess();
    void enableVectorization(bool enable);
    
    // Getters
    PopulationId getId() const { return id_; }
    size_t getNeuronCount() const { return neurons_.size(); }
    const std::vector<NeuronId>& getNeuronIds() const { return neuron_ids_; }
    
    // Constants
    static constexpr float SYNAPTIC_INPUT_THRESHOLD = 0.1f;
    static constexpr float REFACTORY_DECAY = 1.0f / 5.0f;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PopulationId id_;
    std::vector<NeuronId> neuron_ids_;
    std::vector<std::unique_ptr<OptimizedNeuron>> neurons_;
    
    // Optimized synaptic input arrays for batch processing
    std::vector<float> excitatory_inputs_;
    std::vector<float> inhibitory_inputs_;
    std::vector<float> modulatory_inputs_;
    
    // Spike processing buffers
    std::vector<NeuronId> spiking_neurons_;
    std::vector<float> spike_times_;
    
    // Performance profiling
    struct PerformanceMetrics {
        double step_time_ms;
        double synaptic_time_ms;
        double spike_processing_time_ms;
        size_t neurons_processed;
        size_t spikes_detected;
        float average_activity;
        
        PerformanceMetrics() : step_time_ms(0.0), synaptic_time_ms(0.0), spike_processing_time_ms(0.0),
                               neurons_processed(0), spikes_detected(0), average_activity(0.0) {}
    };
    
    PerformanceMetrics performance_metrics_;
    
    // Helper methods
    void updateSynapticInputs();
    void resetInputBuffers();
    void updatePerformanceMetrics();
};

// Optimized Neural Region with spatial indexing
class OptimizedNeuralRegion {
public:
    OptimizedNeuralRegion(RegionId id);
    ~OptimizedNeuralRegion();
    
    // Disable copying, enable moving
    OptimizedNeuralRegion(const OptimizedNeuralRegion&) = delete;
    OptimizedNeuralRegion& operator=(const OptimizedNeuralRegion&) = delete;
    OptimizedNeuralRegion(OptimizedNeuralRegion&&) noexcept;
    OptimizedNeuralRegion& operator=(OptimizedNeuralRegion&&) noexcept;
    
    // Neuron management with optimized allocation
    void addNeuron(std::unique_ptr<OptimizedNeuron> neuron);
    void removeNeuron(NeuronId id);
    OptimizedNeuron* getNeuron(NeuronId id);
    const OptimizedNeuron* getNeuron(NeuronId id) const;
    
    // Population management
    void addPopulation(std::unique_ptr<OptimizedNeuralPopulation> population);
    OptimizedNeuralPopulation* getPopulation(PopulationId id);
    const OptimizedNeuralPopulation* getPopulation(PopulationId id) const;
    
    // Optimized region operations
    void stepRegion(Timestamp currentTime, TimestepDuration dt);
    void processConnections();
    void updateRegionStatistics();
    
    // Spatial indexing for fast neuron access
    void buildSpatialIndex();
    std::vector<NeuronId> getNeuronsInRegion(float x_min, float x_max, 
                                             float y_min, float y_max) const;
    std::vector<NeuronId> getNeuronsByType(NeuronType type) const;
    
    // Memory management optimizations
    void optimizeMemoryUsage();
    void enableSpatialIndexing(bool enable);
    
    // Statistics and profiling
    float getRegionActivity() const;
    float getAverageFiringRate() const;
    size_t getNeuronCount() const;
    size_t getPopulationCount() const;
    
    // Performance profiling
    void recordRegionPerformance(const std::string& operation, double duration_ms);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    RegionId id_;
    std::vector<std::unique_ptr<OptimizedNeuron>> neurons_;
    std::vector<std::unique_ptr<OptimizedNeuralPopulation>> populations_;
    
    // Spatial indexing structures
    struct SpatialIndex {
        std::vector<std::pair<float, float>> neuron_positions;
        std::vector<NeuronId> neuron_ids;
        float x_min, x_max, y_min, y_max;
        
        SpatialIndex() : x_min(0.0f), x_max(1.0f), y_min(0.0f), y_max(1.0f) {}
    } spatial_index_;
    
    // Performance metrics
    struct RegionPerformanceMetrics {
        double step_time_ms;
        double connection_time_ms;
        double statistics_time_ms;
        size_t neurons_stepped;
        size_t populations_stepped;
        float average_activity;
        
        RegionPerformanceMetrics() : step_time_ms(0.0), connection_time_ms(0.0), 
                                     statistics_time_ms(0.0), neurons_stepped(0), 
                                     populations_stepped(0), average_activity(0.0) {}
    };
    
    RegionPerformanceMetrics performance_metrics_;
    
    // Helper methods
    void updateSpatialIndex();
    void optimizeConnectionProcessing();
    void updateRegionPerformanceMetrics();
};

// Optimized Brain with advanced optimizations
class OptimizedBrain {
public:
    OptimizedBrain();
    ~OptimizedBrain();
    
    // Disable copying, enable moving
    OptimizedBrain(const OptimizedBrain&) = delete;
    OptimizedBrain& operator=(const OptimizedBrain&) = delete;
    OptimizedBrain(OptimizedBrain&&) noexcept;
    OptimizedBrain& operator=(OptimizedBrain&&) noexcept;
    
    // Neuron management
    void addNeuron(std::unique_ptr<OptimizedNeuron> neuron);
    void addRegion(std::unique_ptr<OptimizedNeuralRegion> region);
    void addPopulation(std::unique_ptr<OptimizedNeuralPopulation> population);
    
    // Optimized brain operations
    void stepBrain(Timestamp currentTime, TimestepDuration dt);
    void processAllConnections();
    void updateAllStatistics();
    
    // Performance monitoring
    void enableRealTimeProfiling(bool enable);
    void enableSIMD(bool enable);
    void enableVectorization(bool enable);
    
    // Statistics and reporting
    float getOverallActivity() const;
    float getAverageFiringRate() const;
    size_t getNeuronCount() const;
    size_t getRegionCount() const;
    size_t getPopulationCount() const;
    
    // Performance optimization
    void optimizeMemoryLayout();
    void enableParallelProcessing(bool enable);
    void setThreadCount(size_t threads);
    
    // Performance profiling
    void recordBrainPerformance(const std::string& operation, double duration_ms);
    std::string generatePerformanceReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Brain components
    std::vector<std::unique_ptr<OptimizedNeuralRegion>> regions_;
    std::vector<std::unique_ptr<OptimizedNeuralPopulation>> populations_;
    std::vector<std::unique_ptr<OptimizedNeuron>> neurons_;
    
    // Performance settings
    bool real_time_profiling_enabled_;
    bool simd_enabled_;
    bool vectorization_enabled_;
    bool parallel_processing_enabled_;
    size_t thread_count_;
    
    // Global performance metrics
    struct BrainPerformanceMetrics {
        double total_step_time_ms;
        double total_connection_time_ms;
        double total_statistics_time_ms;
        size_t total_neurons_stepped;
        size_t total_spikes_generated;
        float overall_activity;
        double average_step_time_ms;
        double average_connection_time_ms;
        
        BrainPerformanceMetrics() : total_step_time_ms(0.0), total_connection_time_ms(0.0),
                                   total_statistics_time_ms(0.0), total_neurons_stepped(0),
                                   total_spikes_generated(0), overall_activity(0.0),
                                   average_step_time_ms(0.0), average_connection_time_ms(0.0) {}
    };
    
    BrainPerformanceMetrics global_metrics_;
    
    // Helper methods
    void updateGlobalPerformanceMetrics();
    void optimizeConnectionGraph();
    void enableMemoryPools();
};

// Global optimized brain instance
OptimizedBrain& getGlobalOptimizedBrain();

// Factory functions
std::unique_ptr<OptimizedBrain> createOptimizedBrain(const std::string& config_file = "");

} // namespace nlm

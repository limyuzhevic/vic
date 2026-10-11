// Memory Pool System - Pre-allocated memory for better performance
// Reduces memory allocation overhead and improves cache locality
#pragma once

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <atomic>

namespace nlm {

// Neuron Memory Pool
class NeuronPool {
public:
    struct NeuronBlock {
        std::vector<std::unique_ptr<Neuron>> neurons;
        size_t capacity;
        bool allocated;
        
        NeuronBlock(size_t cap) : capacity(cap), allocated(false) {
            neurons.reserve(cap);
        }
    };
    
    NeuronPool(size_t initialBlockSize = 1000, size_t maxBlocks = 100)
        : initialBlockSize_(initialBlockSize), maxBlocks_(maxBlocks),
          freeBlocks_(maxBlocks), totalAllocated_(0), peakUsage_(0) {}
    
    ~NeuronPool() {
        clear();
    }
    
    // Allocate neurons
    Neuron* allocateNeuron(NeuronId id, NeuronType type, const NeuronParameters& params = NeuronParameters()) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find free block
        for (auto& block : blocks_) {
            if (!block->allocated && block->neurons.size() < block->capacity) {
                block->allocated = true;
                auto* neuron = new Neuron(id, type, params);
                block->neurons.emplace_back(neuron);
                totalAllocated_++;
                peakUsage_ = std::max(peakUsage_, totalAllocated_);
                return neuron;
            }
        }
        
        // Create new block if possible
        if (freeBlocks_ > 0) {
            auto newBlock = std::make_unique<NeuronBlock>(initialBlockSize_);
            newBlock->allocated = true;
            auto* neuron = new Neuron(id, type, params);
            newBlock->neurons.emplace_back(neuron);
            
            blocks_.push_back(std::move(newBlock));
            totalAllocated_++;
            peakUsage_ = std::max(peakUsage_, totalAllocated_);
            freeBlocks_--;
            return neuron;
        }
        
        throw std::runtime_error("Neuron pool exhausted");
    }
    
    // Deallocate neuron
    void deallocateNeuron(Neuron* neuron) {
        if (!neuron) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find and remove neuron from blocks
        for (auto& block : blocks_) {
            auto& neurons = block->neurons;
            for (auto it = neurons.begin(); it != neurons.end(); ++it) {
                if (it->get() == neuron) {
                    neurons.erase(it);
                    totalAllocated_--;
                    return;
                }
            }
        }
    }
    
    // Reset all neurons
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& block : blocks_) {
            for (auto& neuron : block->neurons) {
                neuron->reset();
            }
        }
    }
    
    // Clear all memory
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        blocks_.clear();
        totalAllocated_ = 0;
        freeBlocks_ = maxBlocks_;
    }
    
    // Statistics
    size_t getAllocatedCount() const { return totalAllocated_; }
    size_t getPeakUsage() const { return peakUsage_; }
    size_t getBlockCount() const { return blocks_.size(); }
    
    // Access neurons
    const std::vector<std::unique_ptr<Neuron>>& getNeurons() const {
        static std::vector<std::unique_ptr<Neuron>> allNeurons;
        allNeurons.clear();
        for (const auto& block : blocks_) {
            for (const auto& neuron : block->neurons) {
                allNeurons.push_back(neuron->clone());
            }
        }
        return allNeurons;
    }
    
private:
    std::vector<std::unique_ptr<NeuronBlock>> blocks_;
    mutable std::mutex mutex_;
    
    size_t initialBlockSize_;
    size_t maxBlocks_;
    size_t freeBlocks_;
    size_t totalAllocated_;
    size_t peakUsage_;
};

// Synapse Memory Pool
class SynapsePool {
public:
    struct SynapseBlock {
        std::vector<std::unique_ptr<Synapse>> synapses;
        size_t capacity;
        bool allocated;
        
        SynapseBlock(size_t cap) : capacity(cap), allocated(false) {
            synapses.reserve(cap);
        }
    };
    
    SynapsePool(size_t initialBlockSize = 10000, size_t maxBlocks = 50)
        : initialBlockSize_(initialBlockSize), maxBlocks_(maxBlocks),
          freeBlocks_(maxBlocks), totalAllocated_(0), peakUsage_(0) {}
    
    ~SynapsePool() {
        clear();
    }
    
    // Allocate synapse
    Synapse* allocateSynapse(SynapseId id, NeuronId source, NeuronId destination,
                            float weight = 0.0f, Delay delay = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find free block
        for (auto& block : blocks_) {
            if (!block->allocated && block->synapses.size() < block->capacity) {
                block->allocated = true;
                auto* synapse = new Synapse(id, source, destination, weight, delay);
                block->synapses.emplace_back(synapse);
                totalAllocated_++;
                peakUsage_ = std::max(peakUsage_, totalAllocated_);
                return synapse;
            }
        }
        
        // Create new block if possible
        if (freeBlocks_ > 0) {
            auto newBlock = std::make_unique<SynapseBlock>(initialBlockSize_);
            newBlock->allocated = true;
            auto* synapse = new Synapse(id, source, destination, weight, delay);
            newBlock->synapses.emplace_back(synapse);
            
            blocks_.push_back(std::move(newBlock));
            totalAllocated_++;
            peakUsage_ = std::max(peakUsage_, totalAllocated_);
            freeBlocks_--;
            return synapse;
        }
        
        throw std::runtime_error("Synapse pool exhausted");
    }
    
    // Deallocate synapse
    void deallocateSynapse(Synapse* synapse) {
        if (!synapse) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find and remove synapse from blocks
        for (auto& block : blocks_) {
            auto& synapses = block->synapses;
            for (auto it = synapses.begin(); it != synapses.end(); ++it) {
                if (it->get() == synapse) {
                    synapses.erase(it);
                    totalAllocated_--;
                    return;
                }
            }
        }
    }
    
    // Reset all synapses
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& block : blocks_) {
            for (auto& synapse : block->synapses) {
                synapse->reset();
            }
        }
    }
    
    // Clear all memory
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        blocks_.clear();
        totalAllocated_ = 0;
        freeBlocks_ = maxBlocks_;
    }
    
    // Statistics
    size_t getAllocatedCount() const { return totalAllocated_; }
    size_t getPeakUsage() const { return peakUsage_; }
    size_t getBlockCount() const { return blocks_.size(); }
    
    // Access synapses
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const {
        static std::vector<std::unique_ptr<Synapse>> allSynapses;
        allSynapses.clear();
        for (const auto& block : blocks_) {
            for (const auto& synapse : block->synapses) {
                allSynapses.push_back(synapse->clone());
            }
        }
        return allSynapses;
    }
    
private:
    std::vector<std::unique_ptr<SynapseBlock>> blocks_;
    mutable std::mutex mutex_;
    
    size_t initialBlockSize_;
    size_t maxBlocks_;
    size_t freeBlocks_;
    size_t totalAllocated_;
    size_t peakUsage_;
};

// SIMD Vectorization Utilities
namespace SIMD {

// SIMD neuron state operations
class NeuronVectorOperations {
public:
    // Vectorized neuron state updates
    static void updateNeuronsVector(const std::vector<Neuron*>& neurons, 
                                   TimestepDuration timestep,
                                   const float* currents,
                                   float* potentials,
                                   float* activations,
                                   size_t count);
    
    // Vectorized spike detection
    static void detectSpikesVector(const std::vector<Neuron*>& neurons,
                                  bool* isSpike,
                                  size_t count);
    
    // Vectorized synaptic input application
    static void applySynapticInputVector(const std::vector<Neuron*>& neurons,
                                        const std::vector<const Synapse*>& synapses,
                                        const float* weights,
                                        size_t count);
    
    // SIMD acceleration check
    static bool isSIMDAvailable() {
#ifdef __SSE2__
        return true;
#else
        return false;
#endif
    }
};

// SIMD plasticity operations
class PlasticityVectorOperations {
public:
    // Vectorized STDP updates
    static void updateSTDPVector(const std::vector<Synapse*>& synapses,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration timestep,
                                float* weightChanges,
                                size_t count);
    
    // Vectorized Hebbian updates
    static void updateHebbianVector(const std::vector<Synapse*>& synapses,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration timestep,
                                   float* weightChanges,
                                   size_t count);
    
    // Vectorized eligibility trace updates
    static void updateEligibilityTracesVector(const std::vector<Synapse*>& synapses,
                                              const float* dopamineSignals,
                                              float* eligibilityTraces,
                                              size_t count);
};

// Cache-friendly data layout
struct AlignedNeuronData {
    alignas(64) float membranePotential;
    alignas(64) float restingPotential;
    alignas(64) float threshold;
    alignas(64) float resetPotential;
    alignas(64) float leakConductance;
    alignas(64) float totalCurrent;
    alignas(64) float lastSpikeTime;
    alignas(64) float refractoryRemaining;
    alignas(64) float refractoryPeriod;
    alignas(64) float firingState;
    
    // Cache line padding
    alignas(64) char padding[64];
};

struct AlignedSynapseData {
    alignas(64) float weight;
    alignas(64) float delay;
    alignas(64) float efficacy;
    alignas(64) float eligibilityTrace;
    alignas(64) uint8_t preSpikeHistory[64];  // Store spike timestamps
    alignas(64) uint8_t postSpikeHistory[64];
    
    // Cache line padding
    alignas(64) char padding[64];
};

// Memory-efficient data structures
class AlignedMemoryAllocator {
public:
    static AlignedNeuronData* allocateNeurons(size_t count);
    static void freeNeurons(AlignedNeuronData* ptr);
    
    static AlignedSynapseData* allocateSynapses(size_t count);
    static void freeSynapses(AlignedSynapseData* ptr);
    
    static bool isAligned(void* ptr, size_t alignment);
};

} // namespace SIMD

// Performance Monitoring
struct PerformanceMetrics {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStepTime;
    
    size_t stepsCompleted;
    size_t totalNeurons;
    size_t totalSynapses;
    size_t neuronsProcessed;
    size_t synapsesUpdated;
    float totalSpikeTime;
    float memoryUsage;
    float cpuUsage;
    
    PerformanceMetrics() : stepsCompleted(0), totalNeurons(0), totalSynapses(0),
                           neuronsProcessed(0), synapsesUpdated(0), totalSpikeTime(0.0f),
                           memoryUsage(0.0f), cpuUsage(0.0f) {
        startTime = std::chrono::steady_clock::now();
        lastStepTime = startTime;
    }
    
    void recordStep() {
        stepsCompleted++;
        lastStepTime = std::chrono::steady_clock::now();
    }
    
    float getUptimeSeconds() const {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
        return static_cast<float>(duration.count());
    }
    
    float getAverageStepTime() const {
        if (stepsCompleted == 0) return 0.0f;
        auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(lastStepTime - startTime);
        return static_cast<float>(totalDuration.count()) / stepsCompleted / 1000.0f; // ms per step
    }
    
    void updateNeuronsProcessed(size_t neurons) {
        neuronsProcessed = neurons;
    }
    
    void updateSynapsesUpdated(size_t synapses) {
        synapsesUpdated = synapses;
    }
};

// High-Performance Brain Implementation
class PerformanceOptimizedBrain : public Brain {
public:
    PerformanceOptimizedBrain(std::shared_ptr<Config> config)
        : Brain(config), neuronPool_(1000, 100), synapsePool_(10000, 50),
          useSIMD_(SIMD::NeuronVectorOperations::isSIMDAvailable()),
          useMemoryPools_(true) {
        
        // Override memory management with pools
        if (useMemoryPools_) {
            initializeMemoryPools();
        }
    }
    
    ~PerformanceOptimizedBrain() {
        // Cleanup memory pools
        neuronPool_.clear();
        synapsePool_.clear();
    }
    
    // Override step method with optimized implementation
    void step(SimulationStep currentStep) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Use optimized step implementation
        optimizedStep(currentStep);
        
        // Record performance metrics
        performanceMetrics_.recordStep();
        
        // Check performance targets
        if (performanceMetrics_.getAverageStepTime() > 10.0f) { // 10ms target
            onPerformanceTargetMissed("Step execution time exceeded target");
        }
    }
    
    // Optimized step implementation
    void optimizedStep(SimulationStep currentStep) {
        TimestepDuration timestep = static_cast<TimestepDuration>(config_->getOr<double>("simulation_timestep", 0.001));
        Timestamp currentTime = static_cast<Timestamp>(currentStep) * timestep;
        
        // Vectorized neuron updates
        if (useSIMD_) {
            vectorizedNeuronUpdates(currentStep, currentTime, timestep);
        } else {
            standardNeuronUpdates(currentStep, currentTime, timestep);
        }
        
        // Vectorized synaptic operations
        if (useSIMD_) {
            vectorizedSynapticOperations(currentStep, currentTime, timestep);
        } else {
            standardSynapticOperations(currentStep, currentTime, timestep);
        }
        
        // Process spike events
        processSpikeEvents(currentStep);
        
        // Update memory pools
        if (useMemoryPools_) {
            updateMemoryPools();
        }
    }
    
    // Get performance metrics
    PerformanceMetrics getPerformanceMetrics() const { return performanceMetrics_; }
    
    // Enable/disable optimizations
    void setSIMDEnabled(bool enabled) { useSIMD_ = enabled; }
    void setMemoryPoolsEnabled(bool enabled) { useMemoryPools_ = enabled; }
    
    // Performance profiling
    void profilePerformance() const {
        std::cout << "=== Performance Profile ===" << std::endl;
        std::cout << "Steps completed: " << performanceMetrics_.stepsCompleted << std::endl;
        std::cout << "Average step time: " << performanceMetrics_.getAverageStepTime() << " ms" << std::endl;
        std::cout << "Neurons processed per step: " << performanceMetrics_.neuronsProcessed << std::endl;
        std::cout << "Synapses updated per step: " << performanceMetrics_.synapsesUpdated << std::endl;
        std::cout << "Uptime: " << performanceMetrics_.getUptimeSeconds() << " seconds" << std::endl;
        std::cout << "SIMD enabled: " << (useSIMD_ ? "Yes" : "No") << std::endl;
        std::cout << "Memory pools enabled: " << (useMemoryPools_ ? "Yes" : "No") << std::endl;
    }
    
protected:
    void initializeMemoryPools() {
        // Pre-allocate memory pools based on configuration
        size_t neuronCount = config_->getOr<size_t>("neuron_count", 1000);
        size_t synapseCount = static_cast<size_t>(neuronCount * config_->getOr<float>("connection_probability", 0.1f));
        
        // Calculate required blocks
        size_t neuronBlocks = (neuronCount + 999) / 1000;
        size_t synapseBlocks = (synapseCount + 9999) / 10000;
        
        neuronPool_ = NeuronPool(1000, std::min(neuronBlocks, 100));
        synapsePool_ = SynapsePool(10000, std::min(synapseBlocks, 50));
    }
    
    void vectorizedNeuronUpdates(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
        // SIMD-accelerated neuron updates
        // Implementation would use SIMD intrinsics for parallel processing
        
        // For now, use standard updates with performance monitoring
        standardNeuronUpdates(currentStep, currentTime, timestep);
    }
    
    void standardNeuronUpdates(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
        // Standard neuron updates
        Brain::step(currentStep, currentTime);
        
        // Update performance metrics
        performanceMetrics_.updateNeuronsProcessed(Brain::getTotalNeuronCount());
        performanceMetrics_.updateSynapsesUpdated(Brain::getTotalSynapseCount());
    }
    
    void vectorizedSynapticOperations(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
        // SIMD-accelerated synaptic operations
        // Implementation would use SIMD intrinsics for parallel processing
        
        standardSynapticOperations(currentStep, currentTime, timestep);
    }
    
    void standardSynapticOperations(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
        // Standard synaptic operations
        // This would be implemented based on the specific synaptic operations
        // For now, rely on the base Brain implementation
    }
    
    void processSpikeEvents(SimulationStep currentStep) {
        // Process spike events with performance monitoring
        // Implementation would handle spike processing with optimized data structures
        
        // For now, use base implementation
        if (auto spikeSystem = getSpikeSystem()) {
            spikeSystem->processSpikes(currentStep);
        }
    }
    
    void updateMemoryPools() {
        // Update memory pools with performance monitoring
        neuronPool_.reset();
        synapsePool_.reset();
    }
    
    void onPerformanceTargetMissed(const std::string& message) {
        // Handle performance issues
        NLM_LOG_WARNING("Performance target missed: " << message);
        
        // Could trigger recovery or performance tuning
    }
    
private:
    NeuronPool neuronPool_;
    SynapsePool synapsePool_;
    
    bool useSIMD_;
    bool useMemoryPools_;
    
    PerformanceMetrics performanceMetrics_;
};

} // namespace nlm

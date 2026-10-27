// Performance improvements for NLM Brain
// Adding efficient data structures and optimized algorithms

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>
#include <mutex>

namespace nlm {

// Forward declarations
class PerformanceProfiler;

// Performance metrics structure
struct PerformanceMetrics {
    size_t neuronsProcessed;
    size_t spikesGenerated;
    size_t synapsesUpdated;
    double totalSimulationTime;
    double averageStepTime;
    size_t memoryUsage;
    std::chrono::system_clock::time_point timestamp;
    
    PerformanceMetrics() 
        : neuronsProcessed(0), spikesGenerated(0), synapsesUpdated(0),
          totalSimulationTime(0.0), averageStepTime(0.0), memoryUsage(0),
          timestamp(std::chrono::system_clock::now()) {}
};

// Optimized neural region implementation with efficient data structures
class OptimizedNeuralRegion {
public:
    OptimizedNeuralRegion(RegionId id, const std::string& name = "")
        : id(id), name(name), populationCapacity(0), nextNeuronId(0),
          nextSynapseId(0), dirtyFlag(true) {
        InitializeEfficientStructures();
    }
    
    ~OptimizedNeuralRegion() {
        Cleanup();
    }
    
    // Disable copying, enable moving
    OptimizedNeuralRegion(const OptimizedNeuralRegion&) = delete;
    OptimizedNeuralRegion& operator=(const OptimizedNeuralRegion&) = delete;
    OptimizedNeuralRegion(OptimizedNeuralRegion&& other) noexcept;
    OptimizedNeuralRegion& operator=(OptimizedNeuralRegion&& other) noexcept;
    
    // Region identity
    RegionId getId() const { return id; }
    const std::string& getName() const { return name; }
    
    // Population management with fast lookup
    PopulationId addPopulation(size_t neuronCount, NeuronType type);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const { return populations.size(); }
    size_t getTotalNeuronCount() const { return totalNeurons; }
    
    // Fast neuron lookup by ID
    Neuron* getNeuron(NeuronId neuronId);
    const Neuron* getNeuron(NeuronId neuronId) const;
    Neuron* getNeuron(size_t neuronIndex);
    const Neuron* getNeuron(size_t neuronIndex) const;
    
    // Fast neuron access by type
    std::vector<Neuron*> getNeuronsByType(NeuronType type);
    std::vector<const Neuron*> getNeuronsByType(NeuronType type) const;
    
    // Synapse management with efficient spatial hashing
    SynapseId addSynapse(NeuronId source, NeuronId destination, SynapseType type = SynapseType::Excitatory);
    void removeSynapse(SynapseId synapseId);
    Synapse* getSynapse(SynapseId synapseId);
    const Synapse* getSynapse(SynapseId synapseId) const;
    
    // Fast synapse lookup by neuron IDs
    std::vector<Synapse*> getSynapsesFrom(NeuronId sourceNeuron);
    std::vector<const Synapse*> getSynapsesFrom(NeuronId sourceNeuron) const;
    std::vector<Synapse*> getSynapsesTo(NeuronId destinationNeuron);
    std::vector<const Synapse*> getSynapsesTo(NeuronId destinationNeuron) const;
    
    // Efficient connectivity query
    size_t getOutgoingConnectionCount(NeuronId sourceNeuron) const;
    size_t getIncomingConnectionCount(NeuronId destinationNeuron) const;
    float getAverageSynapticWeight() const;
    
    // Bulk operations for performance
    void injectCurrentToAll(NeuronType type, MembranePotential current);
    void updateAllNeurons(Timestamp currentTime, TimestepDuration dt);
    size_t processSpikes();
    
    // Memory management
    void reset();
    void cleanup();
    
    // Performance optimization flags
    void setOptimizationLevel(size_t level) { optimizationLevel = level; }
    size_t getOptimizationLevel() const { return optimizationLevel; }
    
    // Performance statistics
    struct RegionStats {
        size_t neuronCount;
        size_t synapseCount;
        size_t populationCount;
        float averageFiringRate;
        size_t activeNeurons;
        double lastStepTime;
        size_t optimizationLevel;
        
        RegionStats() 
            : neuronCount(0), synapseCount(0), populationCount(0),
              averageFiringRate(0.0f), activeNeurons(0), lastStepTime(0.0),
              optimizationLevel(1) {}
    };
    
    RegionStats getStats() const;
    
    // Dirty flag management for batch operations
    void markDirty() { dirtyFlag = true; }
    bool isDirty() const { return dirtyFlag; }
    void clearDirty() { dirtyFlag = false; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void InitializeEfficientStructures();
};

// Optimized brain implementation with performance monitoring
class OptimizedBrain {
public:
    OptimizedBrain(std::shared_ptr<Config> config);
    ~OptimizedBrain();
    
    // Disable copying, enable moving
    OptimizedBrain(const OptimizedBrain&) = delete;
    OptimizedBrain& operator=(const OptimizedBrain&) = delete;
    OptimizedBrain(OptimizedBrain&& other) noexcept;
    OptimizedBrain& operator=(OptimizedBrain&& other) noexcept;
    
    // Core simulation with performance monitoring
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // I/O operations
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Memory management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Performance monitoring
    PerformanceMetrics getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Fast access to commonly used components
    std::vector<Neuron*> getAllNeurons();
    std::vector<Synapse*> getAllSynapses();
    std::vector<NeuralRegion*> getAllRegions();
    
    // Advanced performance features
    void enableConcurrentProcessing(bool enable) { concurrentProcessing = enable; }
    void setThreadCount(size_t threads) { threadCount = threads; }
    void enableCacheOptimization(bool enable) { cacheOptimization = enable; }
    
    // Performance profiling
    void startProfiling();
    void stopProfiling();
    double getAverageStepTime() const { return averageStepTime; }
    double getTotalSimulationTime() const { return totalSimulationTime; }
    
    // Configuration
    void setConfig(std::shared_ptr<Config> config) { config = config; }
    std::shared_ptr<const Config> getConfig() const { return config; }
    
    // Error recovery
    bool isHealthy() const;
    std::vector<std::string> getErrorMessages() const { return errorMessages; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Performance monitoring
    mutable PerformanceMetrics metrics;
    std::chrono::high_resolution_clock::time_point lastStepTime;
    double averageStepTime;
    double totalSimulationTime;
    size_t stepCount;
    
    // Performance flags
    bool concurrentProcessing;
    bool cacheOptimization;
    size_t threadCount;
    
    // Error handling
    mutable std::vector<std::string> errorMessages;
    mutable std::mutex errorMutex;
    
    // Performance profiling
    std::chrono::high_resolution_clock::time_point profilingStartTime;
    bool isProfiling;
    std::vector<double> stepTimes;
    
    // Internal helpers
    void UpdatePerformanceMetrics(Timestamp currentTime, size_t neuronsProcessed, 
                                  size_t spikesGenerated, size_t synapsesUpdated);
    void AddError(const std::string& error);
    void LogPerformanceStats() const;
};

// Performance profiler for detailed analysis
class PerformanceProfiler {
public:
    PerformanceProfiler() : totalTime(0.0), callCount(0) {}
    
    void start() { startTime = std::chrono::high_resolution_clock::now(); }
    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        totalTime += duration.count() * 1e-9; // Convert to seconds
        callCount++;
    }
    
    double getAverageTime() const { return callCount > 0 ? totalTime / callCount : 0.0; }
    double getTotalTime() const { return totalTime; }
    size_t getCallCount() const { return callCount; }
    
    void reset() { totalTime = 0.0; callCount = 0; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    double totalTime;
    size_t callCount;
};

// Thread-safe concurrent processor
class ConcurrentProcessor {
public:
    ConcurrentProcessor(size_t threadCount = std::thread::hardware_concurrency())
        : threadCount(threadCount), activeThreads(0) {}
    
    template<typename Function>
    void processAsync(Function func) {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (threadCount == 1 || activeThreads >= threadCount) {
            // Run synchronously if single thread or max threads reached
            func();
        } else {
            // Add to work queue
            workQueue.push_back(func);
        }
    }
    
    void processWorkQueue() {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (workQueue.empty()) return;
        
        // Process all work items
        for (auto& func : workQueue) {
            func();
        }
        workQueue.clear();
    }
    
    size_t getQueueSize() const {
        std::lock_guard<std::mutex> lock(mutex);
        return workQueue.size();
    }
    
private:
    std::vector<std::function<void()>> workQueue;
    std::mutex mutex;
    size_t threadCount;
    size_t activeThreads;
};

// Memory pool for efficient allocation/deallocation
class MemoryPool {
public:
    MemoryPool(size_t initialSize = 1024, size_t maxSize = 65536)
        : initialSize(initialSize), maxSize(maxSize), currentSize(0) {}
    
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (currentSize >= maxSize) {
            return nullptr; // Pool exhausted
        }
        
        T* ptr = new T(std::forward<Args>(args)...);
        memoryBlocks.push_back(ptr);
        currentSize++;
        return ptr;
    }
    
    template<typename T>
    void deallocate(T* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = std::find(memoryBlocks.begin(), memoryBlocks.end(), ptr);
        if (it != memoryBlocks.end()) {
            delete* it;
            memoryBlocks.erase(it);
            currentSize--;
        }
    }
    
    void cleanup() {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto* ptr : memoryBlocks) {
            delete ptr;
        }
        memoryBlocks.clear();
        currentSize = 0;
    }
    
    size_t getAllocatedCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return currentSize;
    }
    
private:
    std::vector<void*> memoryBlocks;
    std::mutex mutex;
    size_t initialSize;
    size_t maxSize;
    size_t currentSize;
};

} // namespace nlm
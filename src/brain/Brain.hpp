#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../performance/SpikeBatching.hpp"
#include "../performance/SIMDNeuronUpdate.hpp"
#include "../performance/MemoryPool.hpp"
#include "../performance/BatchOperations.hpp"
#include "../performance/SIMDVectorization.hpp"
#include <memory>
#include <string>
#include <vector>
#include <thread>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Performance optimization flags
struct PerformanceOptimizations {
    bool spikeBatchingEnabled = true;
    bool simdOptimizationEnabled = true;
    bool memoryPoolingEnabled = true;
    bool batchProcessingEnabled = true;
    bool multithreadingEnabled = false;
    size_t simdWidth = 16;
    size_t threadCount = std::thread::hardware_concurrency();
};

// Performance statistics structure
struct PerformanceStats {
    double neuronsPerSecond;
    double spikesPerSecond;
    double synapticEventsPerSecond;
    double realTimeFactor;
    size_t activeNeurons;
    size_t firingNeurons;
    size_t pendingSpikes;
    size_t pendingDelayedSpikes;
    float avgFiringRate;
    float memoryUsageMB;
    double stepTimeMs;
    size_t spikeProcessingTimeNs;
    size_t neuronUpdateTimeNs;
    size_t synapticUpdateTimeNs;
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step - optimized with spike batching and SIMD
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== PERFORMANCE OPTIMIZATION CONTROLS ==========
    
    // Enable/disable performance optimizations
    void enableSpikeBatching(bool enable) { performanceOpts_.spikeBatchingEnabled = enable; }
    void enableSIMDOptimization(bool enable) { performanceOpts_.simdOptimizationEnabled = enable; }
    void enableMemoryPooling(bool enable) { performanceOpts_.memoryPoolingEnabled = enable; }
    void enableBatchProcessing(bool enable) { performanceOpts_.batchProcessingEnabled = enable; }
    void enableMultithreading(bool enable) { performanceOpts_.multithreadingEnabled = enable; }
    
    bool isSpikeBatchingEnabled() const { return performanceOpts_.spikeBatchingEnabled; }
    bool isSIMDOptimizationEnabled() const { return performanceOpts_.simdOptimizationEnabled; }
    bool isMemoryPoolingEnabled() const { return performanceOpts_.memoryPoolingEnabled; }
    bool isBatchProcessingEnabled() const { return performanceOpts_.batchProcessingEnabled; }
    bool isMultithreadingEnabled() const { return performanceOpts_.multithreadingEnabled; }
    
    // Thread count configuration
    void setThreadCount(size_t count) { performanceOpts_.threadCount = count; }
    size_t getThreadCount() const { return performanceOpts_.threadCount; }
    
    // Performance monitoring
    PerformanceStats getPerformanceStats() const;
    
    // Get performance module access
    SpikeBatching* getSpikeBatching() { return spikeBatching_.get(); }
    SIMDNeuronUpdate* getSIMDNeuronUpdate() { return simdNeuronUpdate_.get(); }
    BatchOperations* getBatchOperations() { return batchOperations_.get(); }
    SIMDVectorization* getSIMDVectorization() { return simdVectorization_.get(); }
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    void logPerformanceStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Performance optimization modules
    std::unique_ptr<SpikeBatching> spikeBatching_;
    std::unique_ptr<SIMDNeuronUpdate> simdNeuronUpdate_;
    std::unique_ptr<BatchOperations> batchOperations_;
    std::unique_ptr<SIMDVectorization> simdVectorization_;
    
    // Performance configuration
    PerformanceOptimizations performanceOpts_;
    PerformanceStats perfStats_;
    
    // Performance timing
    std::chrono::high_resolution_clock::time_point lastStepTime_;
    size_t totalSteps_;
    
    // Internal optimization methods
    void initializePerformanceModules();
    void updatePerformanceStats() const;
    void optimizedStep(SimulationStep currentStep, Timestamp currentTime);
    void processSpikeBatch(SimulationStep currentStep, Timestamp currentTime);
    void updateNeuronStates(SimulationStep currentStep, Timestamp currentTime);
    void processSynapticEvents(SimulationStep currentStep, Timestamp currentTime);
    void applyPlasticity(SimulationStep currentStep, Timestamp currentTime);
    void processMemorySystems(SimulationStep currentStep, Timestamp currentTime);
    void processPredictionSystems(SimulationStep currentStep, Timestamp currentTime);
    void processCognitionSystems(SimulationStep currentStep, Timestamp currentTime);
    void processDevelopmentSystems(SimulationStep currentStep, Timestamp currentTime);
    void processNeuromodulation(SimulationStep currentStep, Timestamp currentTime);
    void processActionSystems(SimulationStep currentStep, Timestamp currentTime);
    
    // Performance monitoring helpers
    void measureStepTime() const;
    void updatePerformanceMetrics() const;
    void logOptimizationStatus() const;
};

} // namespace nlm

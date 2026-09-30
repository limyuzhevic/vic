#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/Performance/PerformanceMonitoring.hpp"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

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

// Batch operation result structure
struct BatchOperationResult {
    size_t operationsCompleted;
    double totalTime;
    std::vector<double> operationTimes;
    bool success;
    std::string errorMessage;
};

// Performance monitoring for brain operations
struct BrainPerformanceStats {
    PerformanceCounter neuronInjectionCounter;
    PerformanceCounter sensoryProcessingCounter;
    PerformanceCounter actionProductionCounter;
    PerformanceCounter spikeDeliveryCounter;
    PerformanceCounter plasticityApplicationCounter;
    Timer avgNeuronInjectionTimer;
    Timer avgSensoryProcessingTimer;
    Timer avgActionProductionTimer;
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
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== BATCH OPERATIONS ==========
    
    // Batch operation for injecting currents into multiple neurons
    BatchOperationResult injectCurrents(const std::vector<std::pair<NeuronId, MembranePotential>>& neuronCurrents,
        bool parallel = true, bool vectorized = true);
    
    // Batch operation for processing multiple sensory inputs
    BatchOperationResult processSensoryInputs(const std::vector<const class SensoryInput*>& inputs,
        bool parallel = true);
    
    // Batch operation for producing multiple actions
    std::vector<std::unique_ptr<class Action>> produceActions(size_t count = 1,
        bool parallel = true, bool vectorized = true);
    
    // Batch operation for world updates in SimpleWorld
    BatchOperationResult batchUpdateWorld(const std::vector<std::pair<double, Timestamp>>& timeSteps,
        bool parallel = true);
    
    // Batch operation for object operations in SimpleWorld
    BatchOperationResult batchObjectOperations(const std::vector<std::function<bool(WorldObject*)>>& operations,
        bool parallel = true);
    
    // Batch operation for AgentBrain sensory processing
    BatchOperationResult batchProcessSensory(const std::vector<const SensoryPercept*>& percepts,
        bool parallel = true);
    
    // Batch operation for AgentBrain command decoding
    std::vector<MotorCommand> batchDecodeCommands(size_t count = 1,
        bool parallel = true, bool vectorized = true);
    
    // Batch operation for Config loading/saving
    BatchOperationResult batchLoadConfigs(const std::vector<std::string>& filepaths,
        bool parallel = true);
    
    BatchOperationResult batchSaveConfigs(const std::vector<std::string>& filepaths,
        bool parallel = true);
    
    // Performance monitoring methods
    void recordPerformanceStat(const std::string& name, double time);
    void recordPerformanceCounter(const std::string& name, size_t count, double timePerOp = 0.0);
    void printPerformanceReport() const;
    void resetPerformanceStats();
    
    // Memory optimization utilities
    void preAllocateMemory();
    void optimizeMemoryAccess();
    size_t getCurrentMemoryUsage() const;
    
    // Progress tracking
    void startLongOperation(const std::string& description, size_t totalSteps);
    void advanceProgress(size_t steps = 1);
    std::string getProgressStatus() const;
    
    // Benchmarking
    struct BenchmarkResult {
        std::string operationName;
        size_t iterations;
        double totalTime;
        double avgTime;
        double minTime;
        double maxTime;
        size_t operationsPerSecond;
    };
    
    BenchmarkResult benchmarkOperation(const std::string& name,
        std::function<void()> operation, size_t iterations = 100);
    
    // SIMD/parallel processing
    static void processNeuronsSIMD(const std::vector<Neuron*>& neurons,
        const std::function<void(Neuron*)>& operation);
    
    static void processSynapsesSIMD(const std::vector<Synapse*>& synapses,
        const std::function<void(Synapse*)>& operation);
    
    // Thread pool for parallel operations
    static ThreadPool& getThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    
    // Cache management for frequently accessed data
    void cacheNeuronRegions();
    void clearNeuronCache();
    const NeuralRegion* getCachedRegion(RegionId id) const;
    
    // Compressed data operations
    bool compressState(const std::string& filepath, float compressionLevel = 0.5f) const;
    bool decompressState(std::string& filepath, float compressionLevel = 0.5f);
    
    // Enhanced initialization with batch setup
    bool initializeWithBatchSetup(const std::vector<std::string>& batchConfigs);
    
protected:
    // Configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
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
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    
    // Cognition system access
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Region management
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
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
    
    // Logging
    void logStatus() const;
    
    // ========== BACKWARD COMPATIBILITY WRAPPERS ==========
    
    // Original methods (now use performance-enhanced versions)
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<class Action> produceAction();
    void applyNeuromodulation(const class Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    RegionId addRegion(const std::string& name = "");
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Performance monitoring
    mutable BrainPerformanceStats performanceStats_;
    
    // Progress tracking
    mutable ProgressTracker progressTracker_;
    
    // Memory cache
    mutable std::unordered_map<RegionId, const NeuralRegion*> regionCache_;
    mutable std::mutex cacheMutex_;
    
    // Thread pool for batch operations
    static thread_local ThreadPool threadPool_;
    
    // SIMD optimization flags
    static constexpr bool USE_SIMD = true;
    static constexpr bool USE_VECTORIZATION = true;
    static constexpr bool USE_PARALLEL = true;
};

} // namespace nlm


#pragma once

/**
 * Brain.hpp - Updated with performance optimizations and memory pools
 * 
 * This version includes:
 * - Memory pool management for neurons, synapses, and working memory
 * - Performance optimization hooks and monitoring
 * - Cache management integration
 * - Batch processing support
 * - Memory usage tracking
 */

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include "AdvancedBrainInterface.hpp"

// Memory pool includes
#include "../core/memory/MemoryPool.hpp"
#include "../core/memory/NeuronMemoryPool.hpp"
#include "../core/memory/SynapseMemoryPool.hpp"
#include "../core/memory/WorkingMemoryPool.hpp"
#include "../core/memory/PlasticityPool.hpp"

// Performance optimization includes
#include "../core/performance/PerformanceOptimizer.hpp"
#include "../core/performance/CacheManager.hpp"
#include "../core/performance/BatchProcessor.hpp"

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
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class PredictionError;
class Novelty;

// Performance-related forward declarations
class PerformanceOptimizer;
class MemoryManager;
class CacheManager;
class BatchProcessor;

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

/**
 * Performance configuration for brain initialization
 */
struct BrainPerformanceConfig {
    // Memory pool sizes
    size_t neuronPoolSize;
    size_t synapsePoolSize;
    size_t workingMemoryPoolSize;
    size_t plasticityPoolSize;
    
    // Performance optimization settings
    bool enableMemoryPools;
    bool enableCacheOptimization;
    bool enableBatchProcessing;
    bool enableSIMD;
    bool enablePrefetching;
    
    // Performance monitoring
    bool enablePerformanceMonitoring;
    uint32_t performanceStatsInterval;
    
    // Cache configuration
    size_t cacheSize;
    size_t cacheLineSize;
    
    // Batch processing configuration
    size_t batchSize;
    bool enableBatching;
    
    BrainPerformanceConfig()
        : neuronPoolSize(1024), synapsePoolSize(4096), workingMemoryPoolSize(512), plasticityPoolSize(2048)
        , enableMemoryPools(true), enableCacheOptimization(true), enableBatchProcessing(true)
        , enableSIMD(true), enablePrefetching(true)
        , enablePerformanceMonitoring(true), performanceStatsInterval(100)
        , cacheSize(64 * 1024), cacheLineSize(64)
        , batchSize(100), enableBatching(true) {}
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
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - connections between representations  
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Note: SemanticMemory, ProceduralMemory, and AssociativeMemory
    // are defined in Memory.hpp but not yet implemented (TODO PHASE 2)
    // NeuralSemanticMemory class doesn't exist - should be SemanticMemory
    // NeuralProceduralMemory class doesn't exist - should be ProceduralMemory
    // NeuralAssociativeMemory already implemented in NeuralEpisodicMemory.hpp
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement learning
    Dopamine* getDopamine();
    
    // Acetylcholine - attention and memory consolidation
    Acetylcholine* getAcetylcholine();
    
    // Norepinephrine - arousal and vigilance
    Norepinephrine* getNorepinephrine();
    
    // Serotonin - mood and social behavior
    Serotonin* getSerotonin();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Performance optimization access
    PerformanceOptimizer* getPerformanceOptimizer();
    const PerformanceOptimizer* getPerformanceOptimizer() const;
    
    MemoryManager* getMemoryManager();
    const MemoryManager* getMemoryManager() const;
    
    CacheManager* getCacheManager();
    const CacheManager* getCacheManager() const;
    
    BatchProcessor* getBatchProcessor();
    const BatchProcessor* getBatchProcessor() const;
    
    // Advanced Brain Interface
    AdvancedBrainInterface* getAdvancedInterface() { return &advancedInterface_; }
    const AdvancedBrainInterface* getAdvancedInterface() const { return &advancedInterface_; }
    
    // Advanced functionality
    bool initializeAdvancedInterface(const AdvancedBrainInterface::Config& config);
    void shutdownAdvancedInterface();
    bool executeAdvancedCommand(const std::string& name,
                               const std::vector<std::string>& args,
                               const std::unordered_map<std::string, std::any>& parameters = {});
    bool createComponent(const std::string& type,
                        const std::unordered_map<std::string, std::string>& params = {});
    bool registerExtensionPointHandler(const std::string& pointName,
                                       ExtensionPointHandler handler,
                                       const std::string& handlerName = "",
                                       int priority = 0);
    bool emitEvent(const std::string& eventName,
                  const std::any& data = {},
                  const std::string& sourceExtension = "");
    bool loadExtensionsFromPath(const std::string& path);
    std::string getAdvancedInterfaceStatus() const;
    
    // Performance configuration
    bool configurePerformance(const BrainPerformanceConfig& config);
    BrainPerformanceConfig getCurrentPerformanceConfig() const;
    
    // Performance monitoring
    std::string getPerformanceReport() const;
    bool isPerformanceOptimized() const;
    
    // Logging
    void logStatus() const;
    
    // ========== ATTENTION AND MEMORY ENHANCEMENT ==========
    
    // Boost attention-related processes using acetylcholine signals
    void boostAttentionProcesses(float achLevel);
    
    // Enhance working memory capacity based on acetylcholine levels
    void enhanceWorkingMemoryCapacity(float achLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

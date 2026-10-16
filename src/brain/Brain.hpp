#pragma once

#include "../performance/PerformanceProfiler.hpp"

// Forward declarations for performance profiling
class PerformanceProfiler;

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

// Forward declarations for performance profiling
class PerformanceProfiler;

// Forward declarations for registry
class SystemRegistry;

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
    
    // ========= SYSTEM REGISTRY ACCESS =========
    
    // Get a system by name (type-safe access)
    BrainSystem* getSystem(const std::string& name);
    const BrainSystem* getSystem(const std::string& name) const;
    
    // Get specific system type (type-safe, throws if type mismatch)
    template<typename T>
    T* getSystemAs() {
        static_assert(std::is_base_of<BrainSystem, T>::value, 
                     "T must derive from BrainSystem");
        // Registry access - can be overridden by implementations
        return nullptr;
    }
    
    template<typename T>
    const T* getSystemAs() const {
        static_assert(std::is_base_of<BrainSystem, T>::value, 
                     "T must derive from BrainSystem");
        // Registry access - can be overridden by implementations
        return nullptr;
    }
    
    // Check if system is available
    bool hasSystem(const std::string& name) const;
    
    // Get all registered system names
    std::vector<std::string> getRegisteredSystemNames() const;
    
    // Get underlying concrete systems (for backward compatibility)
    NeuralWorkingMemory* getWorkingMemory();
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    PredictionSystem* getPredictionSystem();
    const PredictionSystem* getPredictionSystem() const;
    
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // (delegated to registry)
    
    // Episodic memory - experience storage
    // (delegated to registry)
    
    // Associative memory - pattern associations
    // (delegated to registry)
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // (delegated to registry)
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    const NeuralPlanner* getPlanner() const;
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    const ConceptFormation* getConceptFormation() const;
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    const AttentionalSelection* getAttention() const;
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    const PredictionError* getPredictionErrorSignal() const;
    
        // Get performance profiler
    PerformanceProfiler& getPerformanceProfiler() { return *pImpl->performanceProfiler; }
    const PerformanceProfiler& getPerformanceProfiler() const { return *pImpl->performanceProfiler; }
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    bool isPerformanceMonitoring() const;
    void recordPerformanceSnapshot();
    
    // Memory tracking
    void enableMemoryTracking(bool enable) { pImpl->performanceProfiler->setMemoryTrackingEnabled(enable); }
    void enableOperationProfiling(bool enable) { pImpl->performanceProfiler->setOperationProfilingEnabled(enable); }
    void enableMetricsCollection(bool enable) { pImpl->performanceProfiler->setMetricsCollectionEnabled(enable); }
    
    // Get performance statistics
    std::string generatePerformanceReport() const;
    std::string generateOptimizationReport() const;
    
    // Get current simulation metrics
    size_t getCurrentStep() const { return pImpl->currentStep; }
    double getCurrentSimulationTime() const { return pImpl->currentTime; }
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    size_t getPendingSpikeCount() const;
    size_t getTotalSpikeCount() const;
    
    // Performance helper functions
    static std::string getOperationTypeName(OperationProfiler::OperationType type);
    static std::string getAllocationTypeName(MemoryTracker::AllocationType type);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

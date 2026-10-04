#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

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
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
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
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
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
    
    // Configuration validation and management
    bool validateConfiguration();
    
    // Advanced logging and statistics
    void logAdvancedConfiguration();
    void logAdvancedStats();
    void logAdvancedMetrics() const;
    
    // Development stage management
    void setDevelopmentalStageFromString(const std::string& stageStr);
    bool isValidDevelopmentalStageString(const std::string& stageStr);
    
    // Advanced metrics update
    void updateAdvancedMetrics();
    
    // Status and information
    std::string getSystemInfo() const;
    std::string getConfigurationSummary() const;
    std::string getPerformanceStats() const;
    
    // Debug and diagnostic tools
    void debugSystemState() const;
    void diagnoseIntegrationIssues() const;
    void validateAllSystems() const;
    
    // Utility functions
    static std::string getVersionInfo();
    static std::string getBuildInfo();
    
    // Export and import utilities
    bool exportSystemState(const std::string& filepath) const;
    bool importSystemState(const std::string& filepath);
    
    // Performance monitoring
    float getSystemPerformanceScore() const;
    size_t getMemoryUsage() const;
    size_t getProcessingTime() const;
    
    // Configuration helpers
    void printConfiguration() const;
    void printSystemStatus() const;
    void printIntegrationStatus() const;

    // ========== ADVANCED FEATURE ACCESS ==========

    // Memory system diagnostics
    size_t getWorkingMemoryCapacity() const;
    size_t getEpisodicMemoryCapacity() const;
    size_t getAssociativeMemoryCapacity() const;
    
    // Neuromodulation diagnostics
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    
    // Prediction system diagnostics
    float getPredictionAccuracy() const;
    size_t getPredictionErrorCount() const;
    
    // Cognition system diagnostics
    size_t getPlannerDepth() const;
    size_t getConceptCount() const;
    float getAttentionStrength() const;
    
    // Plasticity system diagnostics
    bool isSTDPActive() const;
    bool isHebbianActive() const;
    bool isStructuralPlasticityActive() const;
    
    // Development system diagnostics
    DevelopmentalStage getCurrentDevelopmentalStage() const;
    float getPlasticityModulation() const;
    
    // Environment and interaction
    void enableAllSubsystems();
    void disableAllSubsystems();
    void optimizeForPerformance();
    void optimizeForLearning();
    
    // Simulation control
    void pauseSimulation();
    void resumeSimulation();
    bool isSimulationPaused() const;
    
    // Learning control
    void enableLearning();
    void disableLearning();
    bool isLearningEnabled() const;
    
    // Memory management
    void clearWorkingMemory();
    void clearEpisodicMemory();
    void clearAllCaches();
    
    // Emergency controls
    void emergencyShutdown();
    void emergencyReset();
    bool isEmergencyMode() const;

    // ========== INTERNAL SYSTEM ACCESS ==========

    // Direct access to internal systems (advanced usage)
    NeuralWorkingMemory* getWorkingMemoryInternal();
    NeuralEpisodicMemory* getEpisodicMemoryInternal();
    NeuralAssociativeMemory* getAssociativeMemoryInternal();
    PredictionSystem* getPredictionSystemInternal();
    NeuralPlanner* getPlannerInternal();
    ConceptFormation* getConceptFormationInternal();
    AttentionalSelection* getAttentionInternal();
    DevelopmentSystem* getDevelopmentSystemInternal();
    Dopamine* getDopamineInternal();
    Curiosity* getCuriosityInternal();
    Novelty* getNoveltyInternal();
    PredictionError* getPredictionErrorSignalInternal();
    STDP* getSTDPInternal();
    Hebbian* getHebbianInternal();
    StructuralPlasticity* getStructuralPlasticityInternal();
    
    // Implementation details (for debugging)
    const Impl* getImplementation() const;
    Impl* getImplementation();

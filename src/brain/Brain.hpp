#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

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

// Memory system statistics
struct MemoryStatistics {
    // Working memory statistics
    size_t workingMemoryCapacity = 0;
    size_t workingMemoryActiveItems = 0;
    double workingMemoryUtil = 0.0;
    
    // Episodic memory statistics
    size_t episodicMemoryTotalEpisodes = 0;
    size_t episodicMemoryTotalMemories = 0;
    double episodicMemoryUtil = 0.0;
    
    // Associative memory statistics
    size_t associativeMemoryTotalAssociations = 0;
    size_t associativeMemoryActiveAssociations = 0;
    double associativeMemoryUtil = 0.0;
    
    // Prediction system statistics
    size_t predictionTotalPredictions = 0;
    size_t predictionSuccessfulPredictions = 0;
    double predictionAccuracy = 0.0;
    
    // Cognitive system statistics
    size_t conceptTotalConcepts = 0;
    size_t conceptActiveConcepts = 0;
    size_t planningTotalPlans = 0;
    size_t attentionTotalFocusAreas = 0;
    
    // Development statistics
    double developmentalProgress = 0.0;
    size_t developmentPlasticityEvents = 0;
    
    // Neuromodulation statistics
    float averageDopamine = 0.0f;
    float averageCuriosity = 0.0f;
    float averageNovelty = 0.0f;
    float averagePredictionError = 0.0f;
    
    // Network statistics
    size_t totalNeurons = 0;
    size_t totalSynapses = 0;
    size_t firingNeuronCount = 0;
    
    // Performance statistics
    double averageFiringRate = 0.0;
    double learningRate = 0.0;
    
    void reset();
    void update(const Brain& brain);
    std::string toString() const;
    double getOverallUtilization() const;
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
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // Memory management operations
    bool consolidateMemory();
    bool replayMemory();
    bool pruneMemory(float threshold = 0.1f);
    bool compressMemory();
    
    // Memory search and retrieval
    std::vector<MemoryId> searchMemory(const MemoryQuery& query) const;
    std::vector<MemoryId> findSimilarMemories(const MemoryPattern& pattern) const;
    bool retrieveMemory(MemoryId id, std::shared_ptr<Memory>& memory) const;
    
    // Memory statistics
    MemoryStatistics getMemoryStatistics() const;
    std::string getMemoryStatisticsString() const;
    
    // Memory checkpointing
    bool saveMemoryState(const std::string& filepath) const;
    bool loadMemoryState(const std::string& filepath);
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    const PredictionSystem* getPredictionSystem() const;
    
    // Prediction management
    void resetPredictionSystem();
    bool updatePrediction(const SensoryInput& input);
    float getPredictionError() const;
    
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
    
    // Cognitive management
    void resetCognition();
    void updateCognition();
    bool planActions(const std::vector<MotorCommand>& options);
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Development management
    void resetDevelopment();
    void advanceDevelopment(double timestep);
    bool isInCriticalPeriod() const;
    double getDevelopmentalProgress() const;
    
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
    
    // Neuromodulation management
    void resetNeuromodulation();
    void updateNeuromodulation();
    float getOverallModulationLevel() const;
    
    // ========== CORE BRAIN SYSTEM ==========
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    const RandomGenerator* getRandomGenerator() const;
    
    // Simulation control
    void startSimulation();
    void pauseSimulation();
    void stopSimulation();
    bool isRunning() const;
    
    // Time management
    void setTimestep(TimestepDuration timestep);
    TimestepDuration getTimestep() const;
    void advanceTime(double time);
    Timestamp getCurrentTime() const;
    SimulationStep getCurrentStep() const;
    
    // Learning and adaptation
    void enableLearning(bool enable);
    void setLearningRate(float rate);
    float getLearningRate() const;
    bool isLearningEnabled() const;
    
    // Integration and coordination
    bool integrateSensoryInput(const SensoryInput& input);
    bool integrateMotorOutput(const Action& action);
    bool integrateMemoryUpdates();
    bool integrateCognitiveUpdates();
    
    // Statistics and monitoring
    MemoryStatistics getSystemStatistics() const;
    std::string getSystemStatisticsString() const;
    double getSystemUtilization() const;
    
    // Logging
    void logStatus() const;
    void logMemoryStatus() const;
    void logPredictionStatus() const;
    void logCognitiveStatus() const;
    void logDevelopmentStatus() const;
    void logNeuromodulationStatus() const;
    
    // Event system
    using MemoryEventCallback = std::function<void(const Memory&)>;
    using PredictionEventCallback = std::function<void(float)>
    using CognitiveEventCallback = std::function<void(const Action&)>
    using DevelopmentEventCallback = std::function<void(DevelopmentalStage)>
    using NeuromodulationEventCallback = std::function<void(float)>
    
    void setMemoryEventCallback(MemoryEventCallback callback);
    void setPredictionEventCallback(PredictionEventCallback callback);
    void setCognitiveEventCallback(CognitiveEventCallback callback);
    void setDevelopmentEventCallback(DevelopmentEventCallback callback);
    void setNeuromodulationEventCallback(NeuromodulationEventCallback callback);
    
    // Performance optimization
    bool optimizeMemoryUsage();
    bool optimizeProcessing();
    bool optimizeConnectivity();
    
    // Error handling and recovery
    bool handleMemoryError(const std::string& error);
    bool handlePredictionError(const std::string& error);
    bool handleCognitiveError(const std::string& error);
    bool handleDevelopmentError(const std::string& error);
    bool handleNeuromodulationError(const std::string& error);
    
    // Validation and integrity checks
    bool validateSystemIntegrity() const;
    bool validateMemoryIntegrity() const;
    bool validatePredictionIntegrity() const;
    bool validateCognitiveIntegrity() const;
    bool validateDevelopmentIntegrity() const;
    bool validateNeuromodulationIntegrity() const;
    
    // System health monitoring
    bool checkSystemHealth() const;
    bool checkMemoryHealth() const;
    bool checkPredictionHealth() const;
    bool checkCognitiveHealth() const;
    bool checkDevelopmentHealth() const;
    bool checkNeuromodulationHealth() const;
    
    // Backup and recovery
    bool createBackup(const std::string& backupPath) const;
    bool restoreFromBackup(const std::string& backupPath);
    bool listBackups(std::vector<std::string>& backupPaths) const;
    
    // Configuration management
    bool updateConfiguration(const Config& newConfig);
    bool validateConfiguration() const;
    std::string getConfigurationErrors() const;
    
    // Export and import
    bool exportState(const std::string& filepath) const;
    bool importState(const std::string& filepath);
    bool exportMemory(const std::string& filepath) const;
    bool importMemory(const std::string& filepath);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

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
    
// Configuration validation and error handling
    bool validateConfiguration() const;
    
    // Region creation and management
    bool createRegions(size_t regionCount);
    bool createNeurons(size_t neuronCount, size_t regionCount);
    bool initializeConnectivity(float connectionProbability);
    
    // System initialization
    bool initializeMemorySystems();
    bool initializeCognitionSystems();
    bool initializeNeuromodulationSystems();
    void configureCheckpointManager();
    void registerSpikeHandlers();

    // Spike event processing
    void processSpikeEvents(SimulationStep currentStep, Timestamp currentTime);
    
    // System update methods (Phase 6: Integrated Brain Loop)
    void updateAllNeurons(Timestamp currentTime);
    void updateWorkingMemory();
    void applyNeuromodulationEffects();
    void applyPlasticityRules();
    void updateEpisodicMemory();
    void updatePredictionSystem();
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayImportantMemories(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep);
    void consolidateMemories(SimulationStep currentStep);
    void manageCheckpoints(SimulationStep currentStep, Timestamp currentTime);
    void logStepStatistics();

    // Logging and status reporting
    void logStatus() const;

    // ========== USER-FRIENDLY CONVENIENCE METHODS ==========

    // Create a simple brain with default configuration
    static std::unique_ptr<Brain> createSimpleBrain(size_t neuronCount = 1000, 
                                                      size_t regionCount = 1,
                                                      float connectionProbability = 0.1f);

    // Create a sensory-motor brain for simple tasks
    static std::unique_ptr<Brain> createSensoryMotorBrain(size_t sensoryNeurons = 20,
                                                           size_t motorNeurons = 10,
                                                           size_t internalNeurons = 200);

    // Create a learning-enabled brain (with neuromodulation)
    static std::unique_ptr<Brain> createLearningBrain(size_t neuronCount = 500,
                                                       bool enableCuriosity = true,
                                                       bool enableNovelty = true,
                                                       bool enableReward = true);

    // Quick start methods for rapid prototyping
    bool initializeDefault(size_t neuronCount = 100);
    bool initializeLearning(size_t neuronCount = 200);
    bool initializeSimple(size_t neuronCount = 50);

    // High-level control methods
    void enableLearning(bool enable = true);
    void setLearningRate(float rate);
    void enableNeuromodulation(bool enable = true);
    void enableDevelopment(bool enable = true);

    // Convenience actions
    void moveForward();
    void turnLeft();
    void turnRight();
    void wait();

    // Get current action as string
    std::string getActionString() const;

    // Check if brain is learning
    bool isLearning() const;

    // Get brain statistics as formatted string
    std::string getStatisticsString() const;

    // ========== EXPERIMENTATION HELPERS ==========

    // Run a simple experiment (placeholder for future)
    void runExperiment(const std::string& experimentName);

    // Get current experimental state
    std::string getExperimentalState() const;

    // Reset experimental state
    void resetExperimentalState();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

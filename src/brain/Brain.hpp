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
    
    // Logging
    void logStatus() const;
    
    // ========== CONVENIENCE METHODS ==========
    
    // Quick brain initialization with common configurations
    bool initializeForSimulation(const std::string& preset = "standard");
    
    // Simple simulation loop with profiling
    void runSimulation(SimulationStep steps, bool withProfiling = false);
    
    // Generate random sensory input patterns
    void injectRandomSensoryInput(float intensity = 1.0f, bool correlated = false);
    
    // Count active neurons above threshold
    size_t getActiveCountAboveThreshold(MembranePotential threshold) const;
    
    // Check if brain has reached stable state
    bool isStable(float stabilityThreshold = 0.01f, size_t stabilitySteps = 100) const;
    
    // Reset and reinitialize brain safely
    bool resetAndInitialize();
    
    // Safe step with parameter validation
    bool safeStep(SimulationStep currentStep, Timestamp currentTime = 0.0);
    
    // Get comprehensive brain health status
    struct BrainHealth {
        bool isInitialized;
        size_t totalNeuronCount;
        size_t activeNeuronCount;
        size_t firingNeuronCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikeCount;
        size_t pendingEvents;
        bool isStable;
        float stabilityScore;
        std::string statusMessage;
        
        BrainHealth()
            : isInitialized(false), totalNeuronCount(0), activeNeuronCount(0), firingNeuronCount(0),
              averageFiringRate(0.0f), excitationInhibitionRatio(0.0f), totalSpikeCount(0), pendingEvents(0),
              isStable(false), stabilityScore(0.0f), statusMessage("Initialized") {}
    };
    
    BrainHealth getBrainHealth() const;
    
    // Get neuron IDs in specific region
    std::vector<NeuronId> getNeuronIdsInRegion(RegionId regionId) const;
    
    // Get detailed neuron statistics
    struct NeuronStats {
        size_t totalNeurons;
        size_t activeNeurons;
        float averageFiringRate;
        float minFiringRate;
        float maxFiringRate;
        float averageMembranePotential;
        float averageRestingPotential;
        size_t neuronsAboveThreshold;
        size_t neuronsBelowThreshold;
        
        NeuronStats()
            : totalNeurons(0), activeNeurons(0), averageFiringRate(0.0f), minFiringRate(0.0f),
              maxFiringRate(0.0f), averageMembranePotential(0.0f), averageRestingPotential(0.0f),
              neuronsAboveThreshold(0), neuronsBelowThreshold(0) {}
    };
    
    NeuronStats getNeuronStats(RegionId regionId = RegionId(-1)) const;
    
    // Check if brain has enough neurons for a task
    bool hasEnoughNeurons(size_t requiredNeurons) const;
    
    // Safe configuration setting with validation
    bool setConfigSafely(const std::string& key, const std::string& value);
    bool setConfigSafely(const std::string& key, int value);
    bool setConfigSafely(const std::string& key, float value);
    bool setConfigSafely(const std::string& key, bool value);
    
    private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

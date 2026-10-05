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
class SensoryInput;
class Action;
class Neuromodulator;
class Environment;
class CheckpointManager;

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
    void receiveSensoryInput(const SensoryInput& input);
    
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
    std::unique_ptr<Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);
    
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

    // ========== CONVENIENCE METHODS ==========
    
    // Quick brain setup with common configurations
    bool initializeForSimulation(size_t neuronCount = 1000, size_t regionCount = 1, 
                                 float connectionProbability = 0.1);
    
    // Simple simulation loop - runs for specified steps
    void runSimulation(size_t steps, bool verbose = false);
    
    // Inject random sensory input pattern
    void injectRandomSensoryInput(float intensity = 1.0f);
    
    // Get active neuron statistics
    size_t getActiveCountAboveThreshold(float threshold = 5.0f) const;
    
    // Check if brain has reached a stable state
    bool isStable(float stabilityThreshold = 0.95f, size_t stabilitySteps = 100) const;
    
    // Reset brain to initial state and reinitialize
    bool resetAndInitialize()
    {
        reset();
        return initialize();
    }
    
    // Safe step with parameter validation
    bool safeStep(SimulationStep currentStep, Timestamp currentTime = 0.0f)
    {
        if (currentStep < 0) {
            NLM_LOG_WARN("Negative step number: " + std::to_string(currentStep));
            return false;
        }
        step(currentStep, currentTime);
        return true;
    }
    
    // Get brain health status
    struct BrainHealth {
        float memoryUtilization = 0.0f;      // 0.0 to 1.0
        float stability = 0.0f;              // 0.0 to 1.0
        float learningProgress = 0.0f;      // 0.0 to 1.0
        bool isHealthy = false;
        std::string statusMessage;
    };
    
    BrainHealth getBrainHealth() const;
    
    // ========== COMMON PATTERNS ==========
    
    // Pattern 1: Simple agent loop
    std::unique_ptr<Action> processInputAndProduceAction(const SensoryInput& input)
    {
        receiveSensoryInput(input);
        return produceAction();
    }
    
    // Pattern 2: Run episode with learning
    struct EpisodeResult {
        size_t steps;
        float totalReward;
        size_t finalSpikeCount;
        float finalFiringRate;
        bool success;
    };
    
    EpisodeResult runLearningEpisode(size_t maxSteps = 1000,
                                    float targetFiringRate = 10.0f);
    
    // Pattern 3: Quick validation test
    bool runQuickValidationTest()
    {
        // Basic connectivity test
        auto* region = getRegion(RegionId(1));
        if (!region) return false;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) return false;
        
        // Test with a few neurons
        size_t initialSpikes = getTotalSpikeCount();
        for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 10; ++step) {
            step(step);
        }
        
        size_t spikes = getTotalSpikeCount() - initialSpikes;
        return spikes > 0;
    }

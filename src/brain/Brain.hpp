#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <mutex>
#include <atomic>

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
    
    // ========== MEMORY LEAK DETECTION ==========
    
    // Get current number of active Brain instances
    static size_t getActiveInstanceCount();
    
    // Validate all integrated systems
    bool validateAndCheck() const;
    
};

namespace nlm {

// Helper function for parallel processing of neurons
inline void parallelProcessNeurons(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);

// Helper function for optimized spike detection
inline void detectAndQueueSpikes(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);

// Helper function for optimized neuromodulation
inline void applyNeuromodulationOptimized(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);

// Helper function for optimized plasticity
inline void applyPlasticityOptimized(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);

// Helper function for optimized episodic memory
inline void updateEpisodicMemoryOptimized(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);

// Helper function for remaining updates
inline void applyRemainingUpdatesOptimized(Brain::Impl& impl, SimulationStep currentStep, Timestamp currentTime);

} // namespace nlm

private:
    struct Impl {
        std::shared_ptr<Config> config;
        std::unique_ptr<RandomGenerator> rng;
        std::vector<std::unique_ptr<NeuralRegion>> regions;
        std::vector<InterRegionConnection> interRegionConnections;
        
        // ========== INTEGRATED MEMORY SYSTEMS ==========
        std::unique_ptr<NeuralWorkingMemory> workingMemory;
        std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
        std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
        
        // ========== INTEGRATED PREDICTION SYSTEM ==========
        std::unique_ptr<PredictionSystem> predictionSystem;
        
        // ========== INTEGRATED COGNITION SYSTEMS ==========
        std::unique_ptr<NeuralPlanner> planner;
        std::unique_ptr<ConceptFormation> conceptFormation;
        std::unique_ptr<AttentionalSelection> attention;
        
        // ========== DEVELOPMENT SYSTEM ==========
        std::unique_ptr<DevelopmentSystem> developmentSystem;
        
        // ========== NEUROMODULATION SYSTEMS ==========
        std::unique_ptr<Dopamine> dopamine;
        std::unique_ptr<Curiosity> curiosity;
        std::unique_ptr<PredictionError> predictionError;
        std::unique_ptr<Novelty> novelty;
        
        // Phase 2: Real neural computation components
        std::unique_ptr<SpikeSystem> spikeSystem;
        std::unique_ptr<STDP> stdp;
        std::unique_ptr<Hebbian> hebbian;
        std::unique_ptr<StructuralPlasticity> structuralPlasticity;
        
        // Simulation parameters
        TimestepDuration timestep;
        SimulationStep currentStep;
        Timestamp currentTime;
        
        // Statistics
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        
        // Sensory neurons for input injection
        std::vector<Neuron*> sensoryNeurons;
        std::vector<Neuron*> motorNeurons;
        
        // Integration state
        bool isResting;  // For sleep/rest cycle
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        size_t consolidationInterval;
        
        // Checkpoint system
        std::unique_ptr<CheckpointManager> checkpointManager;
        
        Impl(std::shared_ptr<Config> cfg)
            : config(cfg)
            , rng(nullptr)
            , developmentalStage(DevelopmentalStage::Initial)
            , nextRegionId(1)
            , timestep(0.001)
            , currentStep(0)
            , currentTime(0.0)
            , totalSpikesThisStep(0)
            , totalSpikesTotal(0)
            , isResting(false)
            , stepsSinceLastEpisode(0)
            , replayInterval(100)      // Replay every 100 steps
            , consolidationInterval(1000)  // Consolidate every 1000 steps
        {
            // Validate configuration
            NLM_VALIDATE_NON_NULL(cfg, "Brain configuration is null in constructor");
            
            // Initialize random generator with seed from config
            uint64_t seed = 42;  // Default seed
            if (auto seedOpt = config->get<uint64_t>("random_seed")) {
                seed = *seedOpt;
                ValidationHelper::validateNonNegative(seed, "Random seed cannot be negative");
            }
            rng = std::make_unique<RandomGenerator>(seed);
            
            // Initialize plasticity systems
            spikeSystem = std::make_unique<SpikeSystem>();
            stdp = std::make_unique<STDP>();
            hebbian = std::make_unique<Hebbian>();
            structuralPlasticity = std::make_unique<StructuralPlasticity>();
            
            // ========== INITIALIZE INTEGRATED SYSTEMS ==========
            
            // Initialize memory systems
            workingMemory = std::make_unique<NeuralWorkingMemory>();
            episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            
            // Initialize prediction system
            predictionSystem = std::make_unique<PredictionSystem>();
            
            // Initialize cognition systems
            planner = std::make_unique<NeuralPlanner>();
            conceptFormation = std::make_unique<ConceptFormation>();
            attention = std::make_unique<AttentionalSelection>();
            
            // Initialize development system
            developmentSystem = std::make_unique<DevelopmentSystem>();
            
            // Initialize neuromodulation systems
            dopamine = std::make_unique<Dopamine>();
            curiosity = std::make_unique<Curiosity>();
            predictionError = std::make_unique<PredictionError>();
            novelty = std::make_unique<Novelty>();
            
            // Configure STDP parameters with validation
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            ValidationHelper::validateRange(ltpWeight, 0.0f, 1.0f, "LTP weight must be between 0.0 and 1.0");
            ValidationHelper::validateRange(ltdWeight, 0.0f, 1.0f, "LTD weight must be between 0.0 and 1.0");
            ValidationHelper::validateRange(tau, 1.0f, 100.0f, "STDP time constant must be between 1.0 and 100.0");
            stdp->configure(ltpWeight, ltdWeight, tau);
            
            // Configure structural plasticity
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            ValidationHelper::validateRange(synaptogenesisRate, 0.0f, 1.0f, "Synaptogenesis rate must be between 0.0 and 1.0");
            ValidationHelper::validateRange(pruningRate, 0.0f, 1.0f, "Pruning rate must be between 0.0 and 1.0");
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
            structuralPlasticity->setPruningRate(pruningRate);
            
            // Get timestep with validation
            timestep = config->getOr<double>("simulation_timestep", 0.001);
            ValidationHelper::validatePositive(timestep, "Simulation timestep must be positive");
            
            // Get integration intervals from config
            replayInterval = config->getOr<size_t>("replay_interval", 100);
            consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
            ValidationHelper::validatePositive(replayInterval, "Replay interval must be positive");
            ValidationHelper::validatePositive(consolidationInterval, "Consolidation interval must be positive");
            
            // Initialize checkpoint manager
            checkpointManager = std::make_unique<CheckpointManager>();
        }
        
        DevelopmentalStage developmentalStage;
        RegionId nextRegionId;
    };
    
    std::unique_ptr<Impl> pImpl;
    
    // Global instance counter for leak detection
    static std::atomic<size_t> activeInstanceCount;
    
    // Mutex for thread-safe instance management
    static std::mutex instanceMutex;
    
    // Friend declaration for internal implementation details
    friend class BrainInternal;
};

} // namespace nlm

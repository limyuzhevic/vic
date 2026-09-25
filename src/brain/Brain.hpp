#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"

// Phase 2: Real neural computation components
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Neural memory systems
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralSemanticMemory;
class NeuralProceduralMemory;
class NeuralAssociativeMemory;

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class Dopamine;
class Novelty;

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

// ========== SLEEP/REST CYCLE ==========

// Sleep cycle state enumeration
enum class SleepPhase {
    Awake,
    LightSleep,    // Shallow sleep
    DeepSleep,     // Slow-wave sleep
    REM           // Rapid eye movement sleep
};

// Sleep cycle state structure
struct SleepCycleState {
    SleepPhase phase;
    float phaseDuration;  // Duration of current phase in steps
    float nextPhaseTransition;  // Step when phase should change
    float sleepProgress;  // Progress through current sleep cycle (0-1)
    float synapticDownscaling;  // Current level of synaptic normalization
    float memoryReorganization;  // Current level of memory integration
    float consolidationStrength;  // Strength of memory consolidation processes
    float age;  // Simulated age of the brain
    
    SleepCycleState() 
        : phase(SleepPhase::Awake)
        , phaseDuration(0.0f)
        , nextPhaseTransition(0.0f)
        , sleepProgress(0.0f)
        , synapticDownscaling(1.0f)
        , memoryReorganization(0.0f)
        , consolidationStrength(0.5f)
        , age(0.0f) {}
};

// Memory prioritization weights structure
struct MemoryPriorityWeights {
    float recencyWeight;      // How much recency matters (0-1)
    float rewardWeight;       // How much reward matters (0-1)
    float noveltyWeight;      // How much novelty matters (0-1)
    float coherenceWeight;    // How much coherence matters (0-1)
    float importanceWeight;   // How much intrinsic importance matters (0-1)
    
    MemoryPriorityWeights()
        : recencyWeight(0.3f)
        , rewardWeight(0.25f)
        , noveltyWeight(0.2f)
        , coherenceWeight(0.1f)
        , importanceWeight(0.15f) {}
};

// Synaptic homeostasis structure
struct SynapticHomeostasis {
    float synapticStrengthTotal;      // Total synaptic strength across all synapses
    float synapticStrengthTarget;     // Target total synaptic strength (homeostatic bound)
    float normalizationFactor;        // Current normalization factor
    float saturationThreshold;        // Threshold for synaptic saturation
    float adaptationRate;             // Rate at which synaptic weights adapt
    
    SynapticHomeostasis()
        : synapticStrengthTotal(0.0f)
        , synapticStrengthTarget(100.0f)  // Baseline target
        , normalizationFactor(1.0f)
        , saturationThreshold(2.0f)
        , adaptationRate(0.01f) {}
};

// ========== BRAIN CLASS ==========

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
    
    // Semantic memory - knowledge and facts
    NeuralSemanticMemory* getSemanticMemory();
    
    // Procedural memory - skills and habits
    NeuralProceduralMemory* getProceduralMemory();
    
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
    
    // Memory system integration
    void initializeMemorySystems();
    void updateMemorySystems();
    void resetMemorySystems();
    void consolidateMemorySystems();
    void replayImportantMemories();
    void integrateMemorySystems();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

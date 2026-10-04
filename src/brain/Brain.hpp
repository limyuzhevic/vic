#include "../core/Types/Types.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <vector>
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class NeuralRegion;
class Action;
class EpisodicMemoryItem;
class ActionType;

// Sleep/Consolidation Cycle Integration
class SleepConsolidationCycle;

// Checkpoint data structures for integrated systems
struct WorkingMemoryCheckpointData {
    std::vector<NeuronId> memoryNeurons;
    std::vector<float> neuronActivations;
    std::vector<float> memoryStrengths;
};

struct EpisodicMemoryCheckpointData {
    std::vector<EpisodicMemoryItem> episodes;
};

struct AssociativeMemoryCheckpointData {
    std::vector<std::pair<NeuronId, NeuronId>> associations;
    std::vector<float> associationStrengths;
};

struct PredictionSystemCheckpointData {
    std::vector<float> errorHistory;
    std::vector<float> confidenceHistory;
    std::vector<float> modelParameters;
};

struct CognitionSystemCheckpointData {
    // NeuralPlanner data
    std::vector<ActionType> plannerActionHistory;
    std::vector<float> plannerConfidenceHistory;
    
    // ConceptFormation data
    std::vector<size_t> conceptIds;
    std::vector<std::string> conceptCategoryHints;
    std::vector<std::vector<float>> conceptPrototypes;
    
    // AttentionalSelection data
    std::vector<NeuronId> attentionWinners;
    std::vector<float> attentionSalience;
    std::vector<float> attentionBiases;
};

struct NeuromodulationCheckpointData {
    float dopamineLevel;
    float curiosityLevel;
    float predictionErrorLevel;
    float noveltyLevel;
    std::vector<float> plasticityFactors;
};

struct DevelopmentSystemCheckpointData {
    DevelopmentalStage developmentalStage;
    double developmentalAge;
    std::vector<float> plasticityModifiers;
};

struct AttentionSystemCheckpointData {
    std::vector<NeuronId> winnerNeurons;
    std::vector<float> winnerActivations;
    std::vector<float> biases;
    std::vector<float> saliences;
};

struct CheckpointManagerCheckpointData {
    std::string checkpointDir;
    uint64_t saveIntervalSteps;
    size_t maxCheckpoints;
    bool compress;
    uint64_t lastSaveStep;
};

// Additional checkpoint data structures
struct SimulationState {
    uint64_t currentStep;
    double currentTime;
    DevelopmentalStage developmentalStage;
};

struct RandomState {
    uint64_t seed;
    std::vector<uint64_t> state;  // RNG state
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
    
    // Get sleep/consolidation state
    bool isResting() const { return pImpl->isResting; }
    float getSleepProgress() const;
    bool shouldEnterSleep() const;
    
    // Sleep and memory consolidation
    void sleepConsolidationCycle();
    void enterSleep();
    void exitSleep();
    void consolidateMemoriesDuringSleep();
    void replayMemoriesDuringSleep();
    void resetNeuromodulationDuringSleep();
    void developmentDuringSleep();
    
    // Sleep state
    void setSleepParameters(double checkInterval, double duration);
    void setSleepTriggers(float minNovelty, size_t minMemoryLoad, float minStress);
    
    // Reset sleep system
    void resetSleepConsolidation();
    
    // Sleep/Consolidation Cycle Integration
    SleepConsolidationCycle sleepConsolidation_;
    
    // Sleep state variables
    bool isSleeping_;
    double sleepTimer_;
    double lastSleepCheck_;
    float preSleepNovelty_;
    float preSleepDopamine_;
    float preSleepCuriosity_;
    
    // Sleep parameters
    double sleepInterval_;
    double sleepDuration_;
    float minNoveltyForSleep_;
    size_t minMemoryLoadForSleep_;
    float minStressForSleep_;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

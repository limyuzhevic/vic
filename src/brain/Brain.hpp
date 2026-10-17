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
    
// Checkpoint-related methods
    void saveNeurons(NeuronCheckpointData& data) const;
    void saveSynapses(SynapseCheckpointData& data) const;
    void saveWorkingMemory(NeuralWorkingMemory& memory) const;
    void saveEpisodicMemory(NeuralEpisodicMemory& memory) const;
    void saveAssociativeMemory(NeuralAssociativeMemory& memory) const;
    void savePredictionSystem(PredictionSystem& system) const;
    void saveNeuralPlanner(NeuralPlanner& planner) const;
    void saveConceptFormation(ConceptFormation& formation) const;
    void saveAttention(AttentionalSelection& attention) const;
    void saveDevelopmentSystem(DevelopmentSystem& system) const;
    void saveDopamine(Dopamine& dopamine) const;
    void saveCuriosity(Curiosity& curiosity) const;
    void saveNovelty(Novelty& novelty) const;
    void savePredictionError(PredictionError& error) const;
    void saveSimulationState() const;
    void saveRandomState(RandomGenerator& rng) const;
    
    // Load methods
    void loadNeurons(const NeuronCheckpointData& data);
    void loadSynapses(const SynapseCheckpointData& data);
    void loadWorkingMemory(NeuralWorkingMemory& memory);
    void loadEpisodicMemory(NeuralEpisodicMemory& memory);
    void loadAssociativeMemory(NeuralAssociativeMemory& memory);
    void loadPredictionSystem(PredictionSystem& system);
    void loadNeuralPlanner(NeuralPlanner& planner);
    void loadConceptFormation(ConceptFormation& formation);
    void loadAttention(AttentionalSelection& attention);
    void loadDevelopmentSystem(DevelopmentSystem& system);
    void loadDopamine(Dopamine& dopamine);
    void loadCuriosity(Curiosity& curiosity);
    void loadNovelty(Novelty& novelty);
    void loadPredictionError(PredictionError& error);
    void loadSimulationState();
    void loadRandomState(RandomGenerator& rng);
    
    // Prediction system integration methods
    void applyPredictionsToNetwork(const std::vector<float>& predictions);
    void computePredictionErrors(const std::vector<const EpisodicMemoryItem*>& episodes,
                                 const std::vector<float>& predictions,
                                 std::vector<float>& errors);
    void applyPredictionErrorsToPlasticity(const std::vector<float>& errors);
    void computePredictionErrors(const std::vector<const EpisodicMemoryItem*>& episodes,
                                 const std::vector<float>& predictions,
                                 std::vector<float>& errors);
    void applyPredictionErrorsToPlasticity(const std::vector<float>& errors);
    void extractSensoryContext(std::vector<float>& context);
    
    // Neural planning and action selection methods
    void extractPlanningState(std::vector<float>& state);
    ActionType getImmediateAction();
    float evaluateActionWithPrediction(ActionType action, const std::vector<float>& state);
    float calculateActionSuccessProbability(ActionType action);
    float getNeuromodulationActionBias();
    ActionType selectBestAction(const std::vector<std::pair<ActionType, float>>& options);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

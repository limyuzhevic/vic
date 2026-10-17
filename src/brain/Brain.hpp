#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

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

// Firing info for tracking spike events
struct FiringInfo {
    Neuron* neuron;
    Timestamp currentTime;
    SimulationStep currentStep;
    NeuronState state;
    
    FiringInfo() : neuron(nullptr), currentTime(0.0), currentStep(0) {}
    FiringInfo(Neuron* n, Timestamp t, SimulationStep s, const NeuronState& st)
        : neuron(n), currentTime(t), currentStep(s), state(st) {}
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
    
private:
    // Helper methods for refactored step function
    struct FiringInfo;
    
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateNeurons(Timestamp currentTime);
    void updateNeuronPopulation(const std::vector<std::unique_ptr<NeuralPopulation>>& populations,
                               Timestamp currentTime);
    void handleSpikes(SimulationStep currentStep, Timestamp currentTime);
    void collectFiringNeurons(const std::vector<std::unique_ptr<NeuralPopulation>>& populations,
                              std::vector<FiringInfo>& firingNeurons,
                              Timestamp currentTime);
    bool isNeuronJustFired(const NeuronState& state, Timestamp currentTime) const;
    void processFiringNeurons(const std::vector<FiringInfo>& firingNeurons,
                              SimulationStep currentStep, Timestamp currentTime);
    void handleNeuronalFiring(Neuron* neuron, const FiringInfo& firing,
                              SimulationStep currentStep, Timestamp currentTime);
    void recordPostSynapticPlasticity(NeuralRegion* region, NeuronId neuronId, Timestamp currentTime);
    void scheduleDelayedSpikes(NeuralRegion* region, Neuron* neuron,
                               SimulationStep neuronStep, Timestamp neuronTime,
                               SimulationStep currentStep, Timestamp currentTime);
    void ScheduleDelayedSpike(NeuralRegion* region, Synapse* syn, Neuron* neuron,
                             SimulationStep neuronStep, Timestamp neuronTime,
                             SimulationStep currentStep, Timestamp currentTime);
    void updateWorkingMemoryWithFiredNeuron(Neuron* neuron, const NeuronState& state);
    NeuralRegion* getRegionFromNeuron(Neuron* neuron);
    
    void updateMemorySystems();
    void updateNeuromodulation();
    void updateNoveltySystem();
    void updateCuriositySystem();
    void updateDopamineSystem();
    void modulateNeuralExcitability(float dopamineLevel);
    
    void applyPlasticity();
    float getPlasticityModulator() const;
    void applyPlasticityToRegion(NeuralRegion* region, float plasticityMod);
    void applyPlasticityToSynapse(Synapse* syn, float plasticityMod);
    void applySTDPWithModulation(Synapse* syn, float plasticityMod);
    void applyHebbianLearning(Synapse* syn);
    
    void updateEpisodicMemory();
    void storeCurrentEpisodicMemory();
    void collectActiveNeurons(EpisodicMemoryItem& episode);
    
    void updatePredictionSystem();
    void updateCognitionSystems();
    void updateAttentionSystem();
    void updateConceptFormationSystem();
    
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayMemories(SimulationStep currentStep);
    void applyDevelopment(SimulationStep currentStep);
    void updateDevelopmentPlasticityRates();
    float getDevelopmentPlasticityModifier(DevelopmentalStage stage) const;
    
    void consolidateMemories(SimulationStep currentStep);
    void updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime);
    
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

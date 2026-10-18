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
    
    // Validate configuration before initialization
    static bool validateBrainConfiguration(const std::shared_ptr<const Config>& config);
    
    // Get validation report for configuration
    static nlm::ValidationResult getConfigurationValidationReport(const std::shared_ptr<const Config>& config);
    
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
    
    // ========== ADVANCED NEURAL ACCESS ==========
    
    // Get IDs of all neurons for advanced research
    std::vector<NeuronId> getAllNeuronIds() const;
    
    // Get synaptic connectivity matrix (for small networks)
    std::map<std::pair<NeuronId, NeuronId>, float> getConnectionMatrix() const;
    
    // Inject external signals at specific times
    void setExternalInput(NeuronId neuronId, float current, Timestamp time);
    
    // Get neural activity trace for analysis
    std::vector<std::pair<Timestamp, float>> getNeuralTrace() const;
    
    // Get detailed connection statistics
    std::map<std::string, float> getConnectionStatistics() const;
    
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
    // Refactored step method components
    void runPhase1_ProcessPendingSpikes(SimulationStep currentStep, Timestamp currentTime);
    void runPhase2_UpdateNeuralDynamics(SimulationStep currentStep, Timestamp currentTime);
    void runPhase3_DetectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime);
    void runPhase4_UpdateWorkingMemory();
    void runPhase5_UpdateNeuromodulation();
    void runPhase6_ApplyPlasticityRules();
    void runPhase7_UpdateEpisodicMemory();
    void runPhase8_UpdatePredictionSystem();
    void runPhase9_UpdateAttentionSystem();
    void runPhase10_UpdateConceptFormation();
    void runPhase11_UpdateStructuralPlasticity(SimulationStep currentStep);
    void runPhase12_ReplayImportantMemories(SimulationStep currentStep);
    void runPhase13_ApplyDevelopmentEffects(SimulationStep currentStep);
    void runPhase14_ConsolidateMemories(SimulationStep currentStep);
    void runPhase15_UpdateCheckpointManager(SimulationStep currentStep, Timestamp currentTime);
    void runPhase16_CollectStatistics();
    
    // Helper methods for spike processing
    void processDelayedSpikeEvents(SimulationStep currentStep, Timestamp currentTime);
    void scheduleNeuronSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateSpikingNeurons(SimulationStep currentStep, Timestamp currentTime);
    void deliverDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Helper methods for memory systems
    void storeToWorkingMemory(Neuron* neuron, float activation);
    void updateEpisodicMemoryWithCurrentState(SimulationStep currentStep);
    void createEpisodicMemoryItem(SimulationStep currentStep, EpisodicMemoryItem& episode);
    
    // Helper methods for neuromodulation
    void updateDopamineModulation();
    void updateCuriosityModulation();
    void updateNoveltyModulation();
    void updatePredictionErrorModulation();
    
    // Helper methods for plasticity
    void applySTDPPlasticity(SimulationStep currentStep, Timestamp currentTime);
    void applyHebbianPlasticity(SimulationStep currentStep, Timestamp currentTime);
    
    // Helper methods for development
    void updateDevelopmentalStage(SimulationStep currentStep);
    void updatePlasticityWithDevelopment();
    
    // Helper methods for statistics
    void updateSpikeCounts();
    void updateMemoryStats();
    void updateNeuromodulationStats();
    
    // Core neural computation methods
    void updateNeuronMembranePotentials(Timestamp currentTime, Timestamp timestep);
    void detectAndScheduleSpikes(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime);
    void processNeuronSpikes(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime);
    void applySynapticPlasticity(Synapse* synapse, SimulationStep currentStep, Timestamp currentTime);
    
struct Impl;
    Impl* pImpl;
};;

} // namespace nlm

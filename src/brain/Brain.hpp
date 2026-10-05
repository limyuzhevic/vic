#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <functional>
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
    struct Impl;
    Impl* pImpl;
    
    // ========== REFACTORED STEP FUNCTION COMPONENTS ==========
    
    // Step 1: Process pending delayed spikes (deliver synaptic input)
    void processStep1_DeliveredSynapticInput(SimulationStep currentStep, Timestamp currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    void processStep2_ThyLIFDynamics(Timestamp currentTime);
    
    // Step 3: Detect spikes and schedule outgoing spike events
    void processStep3_DetectAndScheduleSpikes(Timestamp currentTime, SimulationStep currentStep);
    
    // Step 4: Update working memory (maintenance and competition)
    void processStep4_UpdateWorkingMemory();
    
    // Step 5: Apply neuromodulation effects on neural excitability
    void processStep5_ApplyNeuromodulationEffects();
    
    // Step 6: Apply plasticity rules (STDP, Hebbian)
    void processStep6_ApplyPlasticityRules();
    
    // Step 7: Update episodic memory with current experience
    void processStep7_UpdateEpisodicMemory();
    
    // Step 8: Update prediction system
    void processStep8_UpdatePredictionSystem();
    
    // Step 9: Update attention system
    void processStep9_UpdateAttentionSystem();
    
    // Step 10: Update concept formation
    void processStep10_UpdateConceptFormation();
    
    // Step 11: Apply structural plasticity (synaptogenesis, pruning)
    void processStep11_ApplyStructuralPlasticity(Timestamp currentTime, SimulationStep currentStep);
    
    // Step 12: Replay important memories (during rest or periodically)
    void processStep12_ReplayMemories(Timestamp currentTime, SimulationStep currentStep);
    
    // Step 13: Apply development effects
    void processStep13_ApplyDevelopmentEffects(Timestamp currentTime, SimulationStep currentStep);
    
    // Step 14: Collect statistics
    void processStep14_CollectStatistics();
    
    // Helper methods
    void updateNeuronPopulations(Timestamp currentTime);
    void detectNeuronSpikes(Timestamp currentTime, SimulationStep currentStep);
    void applyNeuromodulationToNeurons();
    void updateAllMemorySystems();
    void updateAllCognitiveSystems();
    void applyPlasticityToAllSynapses(float plasticityMod);
    bool shouldReplayMemories(Timestamp currentTime, SimulationStep currentStep);
    bool shouldUpdateDevelopment(Timestamp currentTime, SimulationStep currentStep);
    bool shouldConsolidateMemories(Timestamp currentTime, SimulationStep currentStep);
    void applyDevelopmentEffects(Timestamp currentTime, SimulationStep currentStep);
    
    // Sensory integration
    void integrateSensoryInput();
    
    // Motor integration
    void integrateMotorOutput();
    
    // Checkpoint management
    void updateCheckpointManager(Timestamp currentTime, SimulationStep currentStep);
    
    // Statistics collection
    void collectStepStatistics();
    void logIntegrationStatus() const;
    
    // Validation and error handling
    void validateStepParameters(SimulationStep currentStep, Timestamp currentTime) const;
    void handleStepErrors(const std::exception& e) const;
};

} // namespace nlm

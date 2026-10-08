#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../plasticity/PlasticityRule.hpp"
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
class Serotonin;
class Acetylcholine;
class Endorphins;
class Norepinephrine;

// Neuromodulation integration configuration
struct NeuromodulationIntegrationConfig {
    // Neuromodulator activation flags
    bool dopamineEnabled;
    bool curiosityEnabled;
    bool noveltyEnabled;
    bool predictionErrorEnabled;
    bool serotoninEnabled;
    bool acetylcholineEnabled;
    bool endorphinsEnabled;
    bool norepinephrineEnabled;
    
    // Integration weights
    float neuromodulationToPlasticityWeight;
    float neuromodulationToMemoryWeight;
    float neuromodulationToAttentionWeight;
    float neuromodulationToMotivationWeight;
    
    // Cross-modulator interaction weights
    float neuromodulatorInteractionStrength;
    
    // Learning system integration
    float rewardPredictionErrorIntegration;
    float curiosityExplorationIntegration;
    float noveltyPatternMemoryIntegration;
    
    NeuromodulationIntegrationConfig() {
        // Default configuration
        dopamineEnabled = true;
        curiosityEnabled = true;
        noveltyEnabled = true;
        predictionErrorEnabled = true;
        serotoninEnabled = true;
        acetylcholineEnabled = true;
        endorphinsEnabled = true;
        norepinephrineEnabled = true;
        
        neuromodulationToPlasticityWeight = 0.8f;
        neuromodulationToMemoryWeight = 0.6f;
        neuromodulationToAttentionWeight = 0.7f;
        neuromodulationToMotivationWeight = 0.7f;
        
        neuromodulatorInteractionStrength = 0.5f;
        
        rewardPredictionErrorIntegration = 0.8f;
        curiosityExplorationIntegration = 0.7f;
        noveltyPatternMemoryIntegration = 0.6f;
    }
};

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
// and integrated memory, prediction, cognition, and advanced neuromodulation systems

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
    PlasticityRule* getAdvancedPlasticityRule();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Apply neuromodulation to system
    void applyAdvancedNeuromodulation();
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Update neuromodulation integration
    void updateNeuromodulationIntegration();
    
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
    
    // Advanced neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    Serotonin* getSerotonin();
    Acetylcholine* getAcetylcholine();
    Endorphins* getEndorphins();
    Norepinephrine* getNorepinephrine();
    
    // Get neuromodulation integration configuration
    NeuromodulationIntegrationConfig getNeuromodulationIntegrationConfig() const;
    
    // Set neuromodulation integration configuration
    void setNeuromodulationIntegrationConfig(const NeuromodulationIntegrationConfig& config);
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // Calculate system-wide neuromodulation factors
    float calculateGlobalNeuromodulationFactor() const;
    
    // Apply neuromodulation to specific brain systems
    void applyNeuromodulationToPlasticity();
    void applyNeuromodulationToMemory();
    void applyNeuromodulationToAttention();
    void applyNeuromodulationToMotivation();
    
    // Update neuromodulator interactions
    void updateNeuromodulatorInteractions();
    
    // Get neuromodulator state for advanced users
    std::string getNeuromodulatorStates() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

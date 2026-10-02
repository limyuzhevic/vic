#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <optional>

namespace nlm {

// Forward declarations for external dependencies
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
    
    // Spike system access (returns shared_ptr for better memory management)
    std::shared_ptr<SpikeSystem> getSpikeSystem() const;
    std::shared_ptr<STDP> getSTDP() const;
    std::shared_ptr<Hebbian> getHebbian() const;
    std::shared_ptr<StructuralPlasticity> getStructuralPlasticity() const;
    
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
    
    // Region management with validation
    RegionId addRegion(const std::string& name = "");
    std::shared_ptr<NeuralRegion> getRegion(RegionId id) const;
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
    std::shared_ptr<NeuralWorkingMemory> getWorkingMemory() const;
    
    // Episodic memory - experience storage
    std::shared_ptr<NeuralEpisodicMemory> getEpisodicMemory() const;
    
    // Associative memory - pattern associations
    std::shared_ptr<NeuralAssociativeMemory> getAssociativeMemory() const;
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    std::shared_ptr<PredictionSystem> getPredictionSystem() const;
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    std::shared_ptr<NeuralPlanner> getPlanner() const;
    
    // Concept formation for pattern discovery
    std::shared_ptr<ConceptFormation> getConceptFormation() const;
    
    // Attentional selection for focus
    std::shared_ptr<AttentionalSelection> getAttention() const;
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system for structural changes
    std::shared_ptr<DevelopmentSystem> getDevelopmentSystem() const;
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    std::shared_ptr<Dopamine> getDopamine() const;
    
    // Curiosity - exploration motivation
    std::shared_ptr<Curiosity> getCuriosity() const;
    
    // Novelty - novelty detection
    std::shared_ptr<Novelty> getNovelty() const;
    
    // Prediction error signal
    std::shared_ptr<PredictionError> getPredictionErrorSignal() const;
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    std::shared_ptr<RandomGenerator> getRandomGenerator() const;
    
    // Logging
    void logStatus() const;
    
    // Validation and utility methods
    bool isRegionValid(RegionId id) const;
    std::optional<float> getRegionExcitationInhibitionRatio(RegionId id) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

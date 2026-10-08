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
    [[nodiscard]] bool initialize();
    
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
    [[nodiscard]] SpikeSystem* getSpikeSystem() noexcept;
    [[nodiscard]] const SpikeSystem* getSpikeSystem() const noexcept;
    
    // Plasticity system access
    [[nodiscard]] STDP* getSTDP() noexcept;
    [[nodiscard]] Hebbian* getHebbian() noexcept;
    [[nodiscard]] StructuralPlasticity* getStructuralPlasticity() noexcept;
    
    // Statistics
    [[nodiscard]] float getExcitationInhibitionRatio() const noexcept;
    [[nodiscard]] size_t getTotalSpikeCount() const noexcept;
    [[nodiscard]] size_t getPendingSpikeEventCount() const noexcept;
    
    // Produce motor/action output based on motor neuron activity
    [[nodiscard]] std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset() noexcept;
    
    // Save brain state to file (checkpointing)
    [[nodiscard]] bool save(const std::string& filepath) const;
    
    // Load brain state from file
    [[nodiscard]] bool load(const std::string& filepath);
    
    // Region management
    [[nodiscard]] RegionId addRegion(const std::string& name = "");
    [[nodiscard]] NeuralRegion* getRegion(RegionId id) noexcept;
    [[nodiscard]] const NeuralRegion* getRegion(RegionId id) const noexcept;
    [[nodiscard]] size_t getRegionCount() const noexcept;
    [[nodiscard]] std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    [[nodiscard]] const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const noexcept;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    [[nodiscard]] size_t getTotalNeuronCount() const noexcept;
    [[nodiscard]] size_t getTotalSynapseCount() const noexcept;
    [[nodiscard]] size_t getActiveNeuronCount() const noexcept;
    [[nodiscard]] size_t getFiringNeuronCount() const noexcept;
    [[nodiscard]] float getAverageFiringRate() const noexcept;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    [[nodiscard]] NeuralWorkingMemory* getWorkingMemory() noexcept;
    
    // Episodic memory - experience storage
    [[nodiscard]] NeuralEpisodicMemory* getEpisodicMemory() noexcept;
    
    // Associative memory - pattern associations
    [[nodiscard]] NeuralAssociativeMemory* getAssociativeMemory() noexcept;
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    [[nodiscard]] PredictionSystem* getPredictionSystem() noexcept;
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    [[nodiscard]] NeuralPlanner* getPlanner() noexcept;
    
    // Concept formation for pattern discovery
    [[nodiscard]] ConceptFormation* getConceptFormation() noexcept;
    
    // Attentional selection for focus
    [[nodiscard]] AttentionalSelection* getAttention() noexcept;
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system
    [[nodiscard]] DevelopmentSystem* getDevelopmentSystem() noexcept;
    
    // Developmental stage
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const noexcept;
    void setDevelopmentalStage(DevelopmentalStage stage) noexcept;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    [[nodiscard]] Dopamine* getDopamine() noexcept;
    
    // Curiosity - exploration motivation
    [[nodiscard]] Curiosity* getCuriosity() noexcept;
    
    // Novelty - novelty detection
    [[nodiscard]] Novelty* getNovelty() noexcept;
    
    // Prediction error signal
    [[nodiscard]] PredictionError* getPredictionErrorSignal() noexcept;
    
    // Get current configuration
    [[nodiscard]] std::shared_ptr<const Config> getConfig() const noexcept;
    
    // Get random generator
    [[nodiscard]] RandomGenerator* getRandomGenerator() noexcept;
    
    // Logging
    void logStatus() const;
    
private:
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

// Forward declaration needed for Pimpl implementation
namespace nlm {
    struct Brain::Impl {
        std::shared_ptr<Config> config;
        std::unique_ptr<RandomGenerator> randomGenerator;
        std::unique_ptr<SimulationClock> simulationClock;
        std::unique_ptr<Logger> logger;
        std::vector<std::unique_ptr<NeuralRegion>> regions;
        std::vector<InterRegionConnection> interRegionConnections;
        
        std::unique_ptr<SpikeSystem> spikeSystem;
        std::unique_ptr<STDP> stdp;
        std::unique_ptr<Hepbian> hebbian;
        std::unique_ptr<StructuralPlasticity> structuralPlasticity;
        
        // Memory systems
        std::unique_ptr<NeuralWorkingMemory> workingMemory;
        std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
        std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
        
        // Prediction system
        std::unique_ptr<PredictionSystem> predictionSystem;
        
        // Cognition systems
        std::unique_ptr<NeuralPlanner> planner;
        std::unique_ptr<ConceptFormation> conceptFormation;
        std::unique_ptr<AttentionalSelection> attentionalSelection;
        
        // Development system
        std::unique_ptr<DevelopmentSystem> developmentSystem;
        DevelopmentalStage developmentalStage;
        
        // Neuromodulation systems
        std::unique_ptr<Dopamine> dopamine;
        std::unique_ptr<Curiosity> curiosity;
        std::unique_ptr<Novelty> novelty;
        std::unique_ptr<PredictionError> predictionError;
        
        Impl() noexcept : developmentalStage(DevelopmentalStage::Phase2) {}
        
        // Move constructor
        Impl(Impl&& other) noexcept = default;
        
        // Disable copy
        Impl(const Impl&) = delete;
        Impl& operator=(const Impl&) = delete;
    };
}

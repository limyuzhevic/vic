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
    // Executes the complete 15-step integrated brain simulation (Phase 6)
    // Step-by-step execution:
    // 1. Process pending delayed spikes (deliver synaptic input)
    // 2. Update all neurons (LIF dynamics)
    // 3. Detect spikes and schedule spike events
    // 4. Update working memory (maintenance and competition)
    // 5. Apply neuromodulation effects on neural excitability
    // 6. Apply plasticity rules (STDP, Hebbian)
    // 7. Update episodic memory with current experience
    // 8. Update prediction system
    // 9. Update attention system
    // 10. Update concept formation
    // 11. Apply structural plasticity (synaptogenesis, pruning)
    // 12. Replay important memories (during rest or periodically)
    // 13. Apply development effects
    // 14. Collect statistics
    // 15. Checkpoint management
    void step(SimulationStep currentStep);
    
    // Simulation time step
    // Extended step with explicit timestamp for time-based calculations
    // Calls the main 15-step simulation loop
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
    // Returns the integrated working memory system (Phase 6: non-nullptr implementation)
    // Working memory maintains active neural traces and short-term storage
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // Returns the integrated episodic memory system (Phase 6: non-nullptr implementation)
    // Episodic memory stores compressed episodes of neural activity and experiences
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // Returns the integrated associative memory system (Phase 6: non-nullptr implementation)
    // Associative memory forms connections between patterns and experiences
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // Returns the integrated prediction system (Phase 6: non-nullptr implementation)
    // Prediction system computes predicted sensory states and prediction errors
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // Returns the integrated neural planner system (Phase 6: non-nullptr implementation)
    // Neural planner generates action plans based on current state and goals
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // Returns the integrated concept formation system (Phase 6: non-nullptr implementation)
    // Concept formation discovers patterns and generalizes from experiences
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // Returns the integrated attentional selection system (Phase 6: non-nullptr implementation)
    // Attentional selection prioritizes information and focuses cognitive resources
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system for neurodevelopment
    // Returns the integrated development system (Phase 6: non-nullptr implementation)
    // Development system manages brain development stages and plasticity modulation
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // Returns the integrated dopamine system (Phase 6: non-nullptr implementation)
    // Dopamine encodes reward prediction errors and modulates neural excitability
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // Returns the integrated curiosity system (Phase 6: non-nullptr implementation)
    // Curiosity drives exploration and learning through intrinsic motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // Returns the integrated novelty system (Phase 6: non-nullptr implementation)
    // Novelty detects unexpected stimuli and signals learning opportunities
    Novelty* getNovelty();
    
    // Prediction error signal
    // Returns the integrated prediction error system (Phase 6: non-nullptr implementation)
    // Prediction error signals the difference between expected and actual sensory input
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
};

} // namespace nlm

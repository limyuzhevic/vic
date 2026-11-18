#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
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
// This is the main class for NLM (Neural Learning Machine) simulation.

class Brain {
public:
    // Create brain with configuration
    // @param config Brain configuration parameters (neuron count, plasticity rules, etc.)
    // @return Brain instance ready for initialization
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // @return true if initialization succeeded, false otherwise
    // @note Must be called before simulation steps
    bool initialize();
    
    // Main simulation step
    // @param currentStep Current simulation step number
    // @note Performs one simulation step with all neural dynamics, plasticity, and memory operations
    void step(SimulationStep currentStep);
    
    // Simulation time step
    // @param currentStep Current simulation step number
    // @param currentTime Current simulation time in seconds
    // @note Alternative step method that includes explicit timing information
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // @param input Sensory data from the environment (vision, audio, touch, etc.)
    // @note Injects current into sensory neurons based on input pattern
    // @details This is how the brain receives information from the world
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // @param neuron Target neuron ID
    // @param current Membrane current to inject (in pA)
    // @details Used for external stimulation or direct control
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // @param type Neuron type to stimulate (sensory, motor, etc.)
    // @param current Membrane current to inject (in pA)
    // @details Used for driving entire neural populations
    
    // Spike system access
    // @return Pointer to the spike propagation system
    // @details Handles event-driven spike transmission with synaptic delays
    SpikeSystem* getSpikeSystem();
    
    // Spike system access (const version)
    // @return Const pointer to the spike propagation system
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // @return Pointer to STDP plasticity rule implementation
    STDP* getSTDP();
    
    // Plasticity system access
    // @return Pointer to Hebbian plasticity rule implementation
    Hebbian* getHebbian();
    
    // Plasticity system access
    // @return Pointer to structural plasticity rule implementation
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    // @return Excitation-to-inhibition ratio (E/I balance)
    float getExcitationInhibitionRatio() const;
    
    // Statistics
    // @return Total spike count across all neurons
    size_t getTotalSpikeCount() const;
    
    // Statistics
    // @return Number of pending spike events (delayed spikes)
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // @return Unique pointer to Action object representing the brain's decision
    // @details Converts neural activity into executable actions
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // @param signal Neuromodulation signal (dopamine, curiosity, novelty, etc.)
    // @details Integrates neuromodulatory effects into brain dynamics
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // @details Applies STDP, Hebbian, and structural plasticity rules
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // @details Updates developmental stage and plasticity rates
    void develop();
    
    // Reset brain state
    // @details Resets all internal state variables to initial conditions
    void reset();
    
    // Save brain state to file (checkpointing)
    // @param filepath Path to save the checkpoint file
    // @return true if save succeeded, false otherwise
    // @details Saves neural weights, membrane potentials, firing rates, and pending spikes
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // @param filepath Path to load the checkpoint file
    // @return true if load succeeded, false otherwise
    bool load(const std::string& filepath);
    
    // Region management
    // @param name Optional region name (for labeling)
    // @return New region ID
    RegionId addRegion(const std::string& name = "");
    
    // Get neural region by ID
    // @param id Region identifier
    // @return Pointer to the neural region
    NeuralRegion* getRegion(RegionId id);
    
    // Get neural region by ID (const version)
    // @param id Region identifier
    // @return Const pointer to the neural region
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get number of regions
    // @return Number of neural regions
    size_t getRegionCount() const;
    
    // Get all region IDs
    // @return Vector of all region IDs
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // @return Reference to vector of all neural regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // @param source Source region ID
    // @param target Target region ID
    // @param weight Connection weight (default: 0.0)
    // @param delay Synaptic delay in steps (default: 1)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove inter-region connection
    // @param source Source region ID
    // @param target Target region ID
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // @return Total number of neurons across all regions
    size_t getTotalNeuronCount() const;
    
    // Global statistics
    // @return Total number of synapses across all regions
    size_t getTotalSynapseCount() const;
    
    // Global statistics
    // @return Number of neurons that are currently active
    size_t getActiveNeuronCount() const;
    
    // Global statistics
    // @return Number of neurons that are currently firing
    size_t getFiringNeuronCount() const;
    
    // Global statistics
    // @return Average firing rate across all neurons (spikes per second)
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // @return Pointer to working memory system
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // @return Pointer to episodic memory system
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // @return Pointer to associative memory system
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // @return Pointer to prediction system
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // @return Pointer to neural planner
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // @return Pointer to concept formation system
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // @return Pointer to attentional selection system
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
// ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // @return Pointer to dopamine neuromodulation system
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // @return Pointer to curiosity neuromodulation system
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // @return Pointer to novelty detection system
    Novelty* getNovelty();
    
    // Prediction error signal
    // @return Pointer to prediction error signal system
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    // @return Shared pointer to configuration object
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // @return Pointer to random number generator
    RandomGenerator* getRandomGenerator();
    
    // Learned preference trait (Phase 5): a single scalar, -1..1,
    // updated only by sandbox reward through AgentBrain, saved in the
    // checkpoint (SimulationState section) and used as a small bias in
    // later action choice. This is the measured "personality" step: a
    // preference learned from experience that survives a reload and
    // changes later behaviour. It is not a language-model persona and
    // it does not, on its own, make the brain a human brain.
    // @return Current learned preference value (-1 to +1)
    float getLearnedPreference() const;
    
    // Set learned preference trait
    // @param preference New preference value (-1 to +1)
    void setLearnedPreference(float preference);
    
    // Logging
    // @details Logs brain status to the global logger
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

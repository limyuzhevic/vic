#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../plugin/IPlugin.hpp"
#include "../plugin/PluginWrappers.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>

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
class PluginLoader;

// Plugin system version
#define NLM_PLUGIN_VERSION "1.0.0"

// Plugin status enum
enum class PluginStatus {
    UNLOADED,
    LOADING,
    LOADED,
    ERROR,
    UNLOADING
};

// Plugin category for organization
enum class PluginCategory {
    PLASTICITY,
    NEUROMODULATION,
    REWARD,
    DEVELOPMENT,
    UTILITY,
    EXTERNAL
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<IPlugin>(const std::string& name)>;

// Plugin registration system
class PluginRegistration {
public:
    static PluginRegistration& getInstance();
    
    // Plugin factory registration
    static bool registerPluginFactory(const std::string& name, 
                                      PluginFactory factory,
                                      const std::string& version = "1.0.0",
                                      const std::string& description = "",
                                      PluginCategory category = PluginCategory::UTILITY);
    
    // Plugin discovery
    static std::vector<std::string> getAvailablePlugins() const;
    static bool hasPlugin(const std::string& name) const;
    
    // Get plugin metadata
    static std::string getPluginVersion(const std::string& name);
    static std::string getPluginDescription(const std::string& name);
    static PluginCategory getPluginCategory(const std::string& name);
    
private:
    PluginRegistration() = default;
    ~PluginRegistration() = default;
    PluginRegistration(const PluginRegistration&) = delete;
    PluginRegistration& operator=(const PluginRegistration&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin system configuration
struct PluginSystemConfig {
    // Loading configuration
    std::string pluginDirectory = "plugins/";
    std::string defaultConfigFile = "plugins/plugin_config.json";
    float loadingTimeout = 30.0f;
    
    // Runtime configuration
    bool autoLoadEnabled = true;
    bool autoSaveEnabled = true;
    bool pluginValidationEnabled = true;
    
    // Plugin-specific configuration
    std::map<std::string, std::map<std::string, std::string>> pluginConfigs;
    
    // Performance configuration
    size_t maxConcurrentPlugins = 10;
    size_t pluginQueueSize = 100;
    float pluginReloadInterval = 300.0f; // 5 minutes
    
    // Security configuration
    bool sandboxEnabled = false;
    std::vector<std::string> trustedPluginPaths;
};
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
//
// The Brain class implements a complete artificial developmental brain that integrates
// multiple specialized systems into a coherent closed-loop controller. It serves as the
// central hub for all neural processing, managing regions, populations, synapses, and
// coordinating with higher-level systems for memory, prediction, cognition, and
// neuromodulation.
//
// Key Features:
// - Hierarchical organization through regions and populations
// - Real spiking neural computation using Leaky Integrate-and-Fire (LIF) dynamics
// - Event-driven dynamics with efficient spike propagation
// - Integration of memory, prediction, cognition, and neuromodulation systems
// - Developmental plasticity with structural synapse formation/removal
// - Checkpoint save/load for persistence and replay capabilities
// - Real-time processing with configurable timestep and performance optimizations
//
// Architecture:
// ┌─────────────────────────────────────────────────────────────────┐
// │                         NLM Brain                                │
// ├─────────────────────────────────────────────────────────────────┤
// │                           │    │    │                           │
// │      ┌─────────────┐ ┌─────────────┐ ┌─────────────┐            │
// │      │   Sensory   │ │    Memory   │ │ Neuromo...   │            │
// │      │  Systems   │ │  Systems   │ │  Signals    │            │
// │      └─────────────┘ └─────────────┘ └─────────────┘            │
// │                              │                                 │
// │                              ▼                                 │
// │                      ┌─────────────┐                         │
// │                      │ Prediction │                         │
// │                      │   System   │                         │
// │                      └─────────────┘                         │
// │                              │                                 │
// │                              ▼                                 │
// │                      ┌─────────────┐                         │
// │                      │ Development │                         │
// │                      │   System    │                         │
// │                      └─────────────┘                         │
// │                           │    │    │                          │
// │                     ┌─────────────┐ ┌─────────────┐          │
// │                     │ Regions    │ │ Populations │          │
// │                     │ (Neuronal  │ │ (Neuron     │          │
// │                     │  Clusters) │ │  Groups)    │          │
// │                     └─────────────┘ └─────────────┘          │
// │                              │                                 │
// │                              ▼                                 │
// │                       ┌─────────────┐                       │
// │                       │  Neural    │                       │
// │                       │   Layer    │                       │
// │                       └─────────────┘                       │
// └─────────────────────────────────────────────────────────────────┘
//
// Data Flow:
// 1. WORLD → SENSORY INPUT → BRAIN (sensory processing)
// 2. BRAIN → NEURAL PROCESSING → MOTOR OUTPUT → WORLD
// 3. WORLD CONSEQUENCE → REWARD/ERROR → NEUROMODULATION
// 4. NEUROMODULATION → PLASTICITY → DEVELOPMENT → BEHAVIOR CHANGE
//
// The Brain class manages the complete neural lifecycle including initialization,
// step processing, synaptic plasticity, development, and state persistence.

class Brain {
public:
    // Create brain with configuration
    // 
    // @param config Shared configuration containing all brain parameters,
    //              including neuron count, region layout, synaptic parameters,
    //              plasticity rules, and performance settings.
    // @note Configuration typically comes from experiment demos or user settings.
    //       Memory management is handled internally with pImpl pattern.
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    // 
    // @note Brain manages complex resources including neural regions, populations,
    //       and internal pointers, making copying unsafe and unnecessary.
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // 
    // @return true if initialization succeeded, false if configuration invalid
    //         or resources insufficient.
    // @note This method creates neural regions, populations, neurons, and synapses
    //       based on configuration parameters. All neurons are assigned random
    //       parameters following specified distributions.
    // @warning Must be called after construction before any processing can occur.
    bool initialize();
    
    // Main simulation step
    // 
    // @param currentStep Current simulation step index (used for timing-dependent
    //                     plasticity and development).
    // @note This is the primary processing method that executes one complete
    //       simulation timestep across the entire brain. It processes all regions,
    //       neurons, and synapses in the correct order:
    //       1. Step all neurons (LIF dynamics, spike detection)
    //       2. Propagate spikes through synapses
    //       3. Update plasticity rules (STDP, Hebbian, reward-modulated)
    //       4. Update structural plasticity (synaptogenesis/pruning)
    //       5. Update development system (stage progression)
    //       6. Update neuromodulation effects
    // @warning Should be called at regular, consistent intervals for stable simulation.
    void step(SimulationStep currentStep);
    
    // Simulation time step
    // 
    // @param currentStep Current simulation step index.
    // @param currentTime Current simulation time (seconds, with millisecond precision).
    // @note This variant provides more precise time tracking for timing-dependent
    //       processes like STDP plasticity and spike transmission delays.
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // 
    // @param input SensoryInput containing processed environmental data including
    //              visual, auditory, internal, and proprioceptive information.
    // @note This method injects external and internal current into sensory neurons
    //       based on the input pattern. It typically targets the sensory regions
    //       of the brain, particularly neurons tuned to specific modalities.
    // @warning Should be called once per simulation step before processing.
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // 
    // @param neuron ID of the target neuron.
    // @param current Membrane current to inject (microsiemens).
    // @note Direct current injection is used for neuromodulation, teaching signals,
    //       or testing specific neural responses. The current affects the neuron's
    //       membrane potential directly.
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // 
    // @param type Neuron type (e.g., Sensory, Motor, Internal, etc.).
    // @param current Membrane current to inject (microsiemens).
    // @note This is useful for applying neuromodulatory signals or testing network
    //       responses across a population of neurons.
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // 
    // @return Pointer to the SpikeSystem for low-level spike processing and
    //         event-driven dynamics. The SpikeSystem handles spike generation,
    //         propagation, and event scheduling.
    // @note This provides access to the underlying spike processing machinery
    //       for advanced debugging or system monitoring.
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // 
    // @return Pointers to the three plasticity systems:
    //         - STDP: Spike-Timing-Dependent Plasticity for spike-based learning
    //         - Hebbian: Activity-based co-activation learning
    //         - StructuralPlasticity: Synapse formation/removal
    // @note These systems work together to enable both synaptic weight changes
    //       and structural connectivity changes over time.
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    // 
    // @return Excitation-to-inhibition ratio, useful for analyzing network
    //         balance and stability.
    float getExcitationInhibitionRatio() const;
    
    // @return Total number of spikes generated across all neurons in simulation.
    size_t getTotalSpikeCount() const;
    
    // @return Number of spikes scheduled for future processing (delay buffers).
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // 
    // @return Unique action representing the brain's motor command. The action
    //         is derived from motor neuron population activity through the
    //         MotorSystem, encoding behavior based on learned patterns.
    // @note This is the interface between the internal neural system and the
    //       external world, converting neural activity into executable actions.
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // 
    // @param signal Neuromodulator containing signals like dopamine, curiosity,
    //              novelty, or prediction error.
    // @note Neuromodulators affect neural excitability, plasticity, and learning
    //       rates, providing the motivational and error-correction signals needed
    //       for adaptive behavior.
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // 
    // @note This method updates all plasticity mechanisms including STDP,
    //       Hebbian learning, and reward-modulated plasticity based on current
    //       neural activity patterns. It's typically called automatically
    //       during the main simulation step.
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // 
    // @note This method advances the development system, updating properties
    //       like plasticity rates, structural connectivity, and neural
    //       excitability based on the current developmental stage.
    // @warning Development affects learning capabilities and should be managed
    //         carefully in adaptive systems.
    void develop();
    
    // Reset brain state
    // 
    // @note This method resets all neural components to their initial state,
    //       clearing spike history, synaptic weights, and developmental progress.
    // @warning This effectively restarts learning from scratch and is useful
    //         for experiments or debugging.
    void reset();
    
    // Save brain state to file (checkpointing)
    // 
    // @param filepath Path to save file (typically JSON or binary format).
    // @return true if save succeeded, false if file access failed or encoding error.
    // @note This serializes the entire brain state including regions, neurons,
    //       synapses, and all associated parameters for later restoration.
    // @warning Large brain states may require significant storage space.
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // 
    // @param filepath Path to load file.
    // @return true if load succeeded, false if file not found or corruption.
    // @note This restores all neural components from a previously saved state,
    //       allowing continuation of learning or experimentation.
    // @warning Requires compatible configuration; loaded state may not be
    //         compatible with current settings.
    bool load(const std::string& filepath);
    
    // Region management
    // 
    // @param name Optional descriptive name for the region (defaults to empty).
    // @return New unique RegionId for the created region.
    // @note Regions are the highest-level organizational units of the brain,
    //       containing multiple populations of neurons with specialized functions.
    // @warning Each region ID must be unique within the brain.
    RegionId addRegion(const std::string& name = "");
    
    // Get region by ID
    // 
    // @param id Region ID to retrieve.
    // @return Pointer to NeuralRegion or nullptr if region not found.
    // @note Provides direct access to a specific neural region for specialized
    //       operations or debugging.
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // @return Total number of regions in the brain.
    size_t getRegionCount() const;
    
    // @return Vector of all RegionIds in the brain, useful for iteration.
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // 
    // @return Vector of unique pointers to all NeuralRegion objects in the brain.
    // @note This provides read-only access to all regions; regions are owned by
    //       the brain and should not be modified directly.
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // 
    // @param source Source region ID.
    // @param target Target region ID.
    // @param weight Connection strength (0.0 to 1.0 typically).
    // @param delay Transmission delay in simulation steps.
    // @note Long-range connections allow regions to communicate directly, bypassing
    //       local neural circuits. These connections support plasticity and can be
    //       learned through neuromodulation.
    // @warning Requires both source and target regions to exist.
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove inter-region connection
    // 
    // @param source Source region ID.
    // @param target Target region ID.
    // @note This removes the connection entirely, affecting long-range communication
    //       between regions.
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // 
    // @return Total number of neurons across all regions and populations.
    size_t getTotalNeuronCount() const;
    
    // @return Total number of synapses across all regions.
    size_t getTotalSynapseCount() const;
    
    // @return Number of neurons currently in active state (membrane potential above threshold).
    size_t getActiveNeuronCount() const;
    
    // @return Number of neurons that have fired in the current simulation step.
    size_t getFiringNeuronCount() const;
    
    // @return Average firing rate across all neurons (Hz).
    float getAverageFiringRate() const;
    
    // @return Average synaptic weight across all synapses (useful for analyzing network strength).
    float getAverageSynapticWeight() const;
    
    // @return Maximum synaptic weight in the network.
    float getMaxSynapticWeight() const;
    
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
};

} // namespace nlm

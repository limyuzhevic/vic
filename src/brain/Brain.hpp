#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../plugin/NLMPlugin.hpp"
#include "../plugin/PluginManager.hpp"
#include "../performance/Performance.hpp"
#include "../performance/monitor/PerformanceMonitor.hpp"
#include "../performance/optimization/OptimizationHints.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for performance components
class PerformanceMonitor;
class MemoryProfiler;
class CPUMonitor;
class BenchmarkSuite;

/**
 * Performance configuration options
 */
struct PerformanceConfig {
    // Performance level settings
    enum class Level {
        BASIC,          // Basic functionality, minimal optimizations
        OPTIMIZED,      // Standard optimizations enabled
        MAXIMUM,        // Maximum performance optimizations
        ADAPTIVE        // Adaptive optimizations based on runtime conditions
    };
    
    Level performanceLevel = Level::OPTIMIZED;
    bool enableSIMD = true;
    bool enableMultithreading = true;
    bool enableMemoryPools = true;
    bool enableEventQueue = true;
    bool enableVectorization = true;
    bool enableProfiler = false;
    bool enableBenchmarking = false;
    
    // Threading configuration
    size_t numThreads = 0;  // 0 = auto-detect
    
    // Memory pool configuration
    size_t neuronPoolSize = 1024;
    size_t synapsePoolSize = 4096;
    size_t eventQueuePoolSize = 16384;
    
    // SIMD configuration
    enum class SIMDLevel {
        NONE,           // No SIMD
        SSE,            // SSE only
        AVX,            // AVX only
        AVX512,         // AVX512 only
        AUTO            // Auto-detect best available
    };
    SIMDLevel simdLevel = SIMDLevel::AUTO;
    
    // Event queue configuration
    size_t spikeQueueCapacity = 16384;
    size_t delayedSpikeBuckets = 1024;
    uint64_t maxDelaySteps = 100;
    
    // Profiling configuration
    std::string profileOutputFile;
    std::string benchmarkOutputFile;
    double profileInterval = 1.0;     // Profile interval in seconds
    double benchmarkInterval = 60.0; // Benchmark interval in seconds
    
    // Optimization configuration
    bool enableCacheOptimization = true;
    bool enableBranchPrediction = true;
    bool enableDataLayoutOptimization = true;
    bool enableLoopUnrolling = true;
    bool enableFunctionInlining = true;
    
    // Performance monitoring configuration
    bool enableMemoryMonitoring = true;
    bool enableCPUMonitoring = true;
    bool enableNetworkMonitoring = false;
    
    PerformanceConfig() = default;
    
    // Update configuration from string values
    void updateFromString(const std::string& key, const std::string& value);
    
    // Get configuration string
    std::string toString() const;
    
    // Check if configuration is valid
    bool isValid() const;
};

// Additional component forward declarations
class NeuralPopulation;
class Synapse;
class NeuralConnection;
class Action;
class PlasticityRule;
class SensoryInput;
class SensoryModality;
class MemorySystem;
class WorkingMemory;
class EpisodicMemory;
class AssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Neuromodulator;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class RandomGenerator;
class CheckpointManager;
class CheckpointReader;
class CheckpointWriter;
class Config;

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

/**
 * @class Brain
 * @brief The central coordinator of the neural system
 * @details Implements real spiking neural computation with event-driven dynamics
 *         and integrated memory, prediction, cognition, and neuromodulation systems
 *         
 * This is the main class that orchestrates all neural operations in the NLM (熙然)
 * artificial brain. It serves as the primary interface between sensory input and
 * motor output, coordinating all lower-level neural components including:
 * 
 * - **Neural Computation**: Event-driven spiking neural dynamics with LIF neurons
 * - **Memory Systems**: Working memory, episodic memory, and associative memory
 * - **Prediction Systems**: Forward and inverse models for temporal prediction
 * - **Cognition Systems**: Attention, planning, concept formation, and self-model
 * - **Neuromodulation Systems**: Dopamine, curiosity, novelty, and prediction error
 * - **Development System**: Neural maturation and stage-based plasticity
 * - **Plasticity Systems**: STDP, Hebbian, and structural plasticity
 * 
 * The Brain class provides a unified interface for simulating neural computation
 * and integrating all cognitive capabilities into a coherent artificial brain.
 * 
 * @note Phase 6: This class represents the final integration of all previous phases
 *       into a unified, coherent artificial brain system.
 * 
 * @author NLM Research Team
 * @date Version 0.1.0
 * @ingroup brain
 */

class Brain {
public:
    // Create brain with configuration
    /**
     * @brief Construct a new Brain object with configuration
     * @param config Shared pointer to configuration object containing system parameters
     * @note The Brain takes ownership of the configuration and will manage its lifetime
     * @warning Configuration must contain valid parameters for all required components
     * @throws std::bad_alloc if memory allocation fails
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object
     * @details Cleans up all allocated resources including neural components,
     *          memory systems, and other integrated subsystems.
     */
    ~Brain();
    
    public:
    // Plugin system integration
    /**
     * @brief Get access to the plugin manager
     * @return Reference to the plugin manager
     * @details Provides access to the plugin system for loading, configuring,
     *          and managing custom components.
     * 
     * @return PluginManager& plugin manager reference
     * @see PluginManager class for plugin management details
     */
    PluginManager& getPluginManager();
    
    /**
     * @brief Initialize plugin system
     * @return true if plugin system initialization successful
     * @details Initializes the plugin system and loads configured plugins.
     *          Should be called after Brain construction but before simulation.
     * 
     * @return bool true if initialization successful
     * @see PluginManager::initialize()
     */
    bool initializePlugins();
    
    /**
     * @brief Load a plugin from a shared library
     * @param filepath Path to the plugin shared library
     * @return true if plugin loaded successfully
     * @details Loads a plugin from a shared library file. The plugin must be
     *          compatible with the NLM plugin system and its dependencies must be resolved.
     * 
     * @param filepath Plugin library file path
     * @return bool true if plugin loaded successfully
     * @see PluginManager::loadPlugin()
     */
    bool loadPlugin(const std::string& filepath);
    
    /**
     * @brief Unload a plugin from the system
     * @param pluginName Name of the plugin to unload
     * @return true if plugin unloaded successfully
     * @details Unloads a plugin and removes it from the system. Any references
     *          to the plugin's components will become invalid.
     * 
     * @param pluginName Name of the plugin to unload
     * @return bool true if plugin unloaded successfully
     * @see PluginManager::unloadPlugin()
     */
    bool unloadPlugin(const std::string& pluginName);
    
    /**
     * @brief Enable a plugin
     * @param pluginName Name of the plugin to enable
     * @return true if plugin enabled successfully
     * @details Enables a plugin so it can provide its components to the brain.
     *          Enabled plugins are used by default when creating new components.
     * 
     * @param pluginName Name of the plugin to enable
     * @return bool true if plugin enabled successfully
     * @see PluginManager::enablePlugin()
     */
    bool enablePlugin(const std::string& pluginName);
    
    /**
     * @brief Disable a plugin
     * @param pluginName Name of the plugin to disable
     * @return true if plugin disabled successfully
     * @details Disables a plugin so it does not provide its components to the brain.
     *          Disabled plugins are not used when creating new components.
     * 
     * @param pluginName Name of the plugin to disable
     * @return bool true if plugin disabled successfully
     * @see PluginManager::disablePlugin()
     */
    bool disablePlugin(const std::string& pluginName);
    
    /**
     * @brief Get plugin factory for creating custom components
     * @param pluginName Name of the plugin
     * @param componentType Type of component (Region, Neuron, Plasticity, etc.)
     * @return void* Pointer to the factory function (or nullptr if not available)
     * @details Provides access to plugin factories for creating custom components.
     *          The returned pointer should be cast to the appropriate function pointer
     *          based on the componentType.
     * 
     * @param pluginName Name of the plugin
     * @param componentType Type of component to get factory for
     * @return void* Factory function pointer or nullptr
     * @see PluginManager::getRegionFactory()
     */
    void* getPluginFactory(const std::string& pluginName, const std::string& componentType) const;
    
    /**
     * @brief Configure a plugin with settings
     * @param pluginName Name of the plugin
     * @param config Configuration string (JSON or key=value format)
     * @return true if plugin configured successfully
     * @details Configures a plugin with the provided settings. The configuration
     *          format depends on the plugin implementation.
     * 
     * @param pluginName Name of the plugin
     * @param config Configuration string
     * @return bool true if plugin configured successfully
     * @see PluginManager::configurePlugin()
     */
    bool configurePlugin(const std::string& pluginName, const std::string& config);
    
    /**
     * @brief Get list of available plugins
     * @return Vector of plugin names
     * @details Returns the names of all loaded plugins in the system.
     *          This includes both enabled and disabled plugins.
     * 
     * @return std::vector<std::string> List of plugin names
     * @see PluginManager::getPluginNames()
     */
    std::vector<std::string> getAvailablePlugins() const;
    
    /**
     * @brief Get plugin status
     * @param pluginName Name of the plugin
     * @return Plugin status (Unknown, Loaded, Active, Error, etc.)
     * @details Returns the current status of a plugin in the system.
     *          Status indicates whether the plugin is loaded, configured, active, etc.
     * 
     * @param pluginName Name of the plugin
     * @return PluginStatus Plugin status
     * @see PluginManager::getPluginStatus()
     */
    PluginStatus getPluginStatus(const std::string& pluginName) const;
    
    /**
     * @brief Get plugin metadata
     * @param pluginName Name of the plugin
     * @return Plugin metadata (name, version, description, etc.)
     * @details Returns the metadata for a plugin, which includes its name, version,
     *          description, author, category, and dependencies.
     * 
     * @param pluginName Name of the plugin
     * @return PluginMetadata Plugin metadata
     * @see PluginMetadata structure
     */
    PluginMetadata getPluginMetadata(const std::string& pluginName) const;
    
    /**
     * @brief Create a custom region using plugin system
     * @param regionType Type of region to create
     * @param name Optional name for the region
     * @return Pointer to the created region (or nullptr if creation failed)
     * @details Creates a region using the plugin system. The region type determines
     *          which plugin provides the region factory. This allows custom region
     *          types to be created at runtime.
     * 
     * @param regionType Type of region to create
     * @param name Optional region name
     * @return NeuralRegion* Pointer to created region
     * @note This method should be used instead of addRegion() when you need
     *       custom region types provided by plugins.
     */
    NeuralRegion* createCustomRegion(const std::string& regionType, const std::string& name = "");
    
    /**
     * @brief Create a custom neuron using plugin system
     * @param neuronType Type of neuron to create
     * @param id Neuron ID
     * @param neuronTypeEnum Optional neuron type enum (Sensory, Motor, Internal)
     * @return Pointer to the created neuron (or nullptr if creation failed)
     * @details Creates a neuron using the plugin system. The neuron type determines
     *          which plugin provides the neuron factory. This allows custom neuron
     *          models (beyond LIF) to be created at runtime.
     * 
     * @param neuronType Type of neuron to create
     * @param id Neuron ID
     * @param neuronTypeEnum Optional neuron type enum
     * @return Neuron* Pointer to created neuron
     * @note This method should be used instead of addPopulation() when you need
     *       custom neuron types provided by plugins.
     */
    Neuron* createCustomNeuron(const std::string& neuronType, NeuronId id, 
                               NeuronType neuronTypeEnum = NeuronType::Internal);
    
    /**
     * @brief Create a custom plasticity rule using plugin system
     * @param plasticityType Type of plasticity rule to create
     * @return Pointer to the created plasticity rule (or nullptr if creation failed)
     * @details Creates a plasticity rule using the plugin system. The plasticity type
     *          determines which plugin provides the plasticity factory. This allows
     *          custom learning algorithms (beyond STDP and Hebbian) to be created.
     * 
     * @param plasticityType Type of plasticity rule to create
     * @return PlasticityRule* Pointer to created plasticity rule
     * @note This method should be used instead of direct STDP/Hebbian creation
     *       when you need custom plasticity rules provided by plugins.
     */
    PlasticityRule* createCustomPlasticityRule(const std::string& plasticityType);
    
    /**
     * @brief Create a custom sensory modality using plugin system
     * @param sensoryType Type of sensory modality to create
     * @return Pointer to the created sensory modality (or nullptr if creation failed)
     * @details Creates a sensory modality using the plugin system. The sensory type
     *          determines which plugin provides the sensory factory. This allows
     *          custom sensory modalities (beyond vision, audio, etc.) to be created.
     * 
     * @param sensoryType Type of sensory modality to create
     * @return SensoryInput* Pointer to created sensory modality
     * @note This method should be used instead of direct vision/audio creation
     *       when you need custom sensory modalities provided by plugins.
     */
    SensoryInput* createCustomSensoryModality(const std::string& sensoryType);
    
    /**
     * @brief Create a custom memory system using plugin system
     * @param memoryType Type of memory system to create
     * @return Pointer to the created memory system (or nullptr if creation failed)
     * @details Creates a memory system using the plugin system. The memory type
     *          determines which plugin provides the memory factory. This allows
     *          custom memory architectures (beyond working/episodic/associative) to be created.
     * 
     * @param memoryType Type of memory system to create
     * @return void* Pointer to created memory system (type depends on memoryType)
     * @note This method should be used instead of direct memory system creation
     *       when you need custom memory systems provided by plugins.
     */
    void* createCustomMemorySystem(const std::string& memoryType);
    
    /**
     * @brief Create a custom neuromodulator using plugin system
     * @param neuromodulatorType Type of neuromodulator to create
     * @return Pointer to the created neuromodulator (or nullptr if creation failed)
     * @details Creates a neuromodulator using the plugin system. The neuromodulator type
     *          determines which plugin provides the neuromodulation factory. This allows
     *          custom neuromodulators (beyond dopamine, curiosity, novelty, etc.) to be created.
     * 
     * @param neuromodulatorType Type of neuromodulator to create
     * @return Neuromodulator* Pointer to created neuromodulator
     * @note This method should be used instead of direct neuromodulator creation
     *       when you need custom neuromodulators provided by plugins.
     */
    Neuromodulator* createCustomNeuromodulator(const std::string& neuromodulatorType);
    
    // Initialize brain with configuration
    /**
     * @brief Initialize the brain with configured components
     * @return true if initialization successful, false otherwise
     * @details Creates and configures all neural components including:
     *          - Regions and neural populations
     *          - Neurons with LIF dynamics
     *          - Synaptic connections and weights
     *          - All integrated memory systems (working, episodic, associative)
     *          - Prediction and cognition systems
     *          - Neuromodulation systems
     *          - Spike event system for real-time processing
     *          - Memory pools and event queues for performance
     * 
     * @note Initialization follows a phased approach:
     *       1. Core neural components (regions, neurons, synapses)
     *       2. Plasticity systems (STDP, Hebbian, structural)
     *       3. Memory systems
     *       4. Prediction and cognition
     *       5. Neuromodulation and development
     *       6. Event-driven spike processing setup
     * 
     * @pre Configuration must contain valid parameters
     * @post All components are initialized and ready for simulation
     * @throws std::runtime_error if initialization fails due to invalid configuration
     * @see getTotalNeuronCount()
     * @see getTotalSynapseCount()
     */
    bool initialize();
    
    // Main simulation step (with default time calculation)
    /**
     * @brief Execute one simulation step with automatic time calculation
     * @param currentStep Current simulation step number
     * @details Performs a complete neural computation cycle including:
     *          - Delayed spike processing and synaptic input delivery
     *          - Neuron state updates (LIF dynamics)
     *          - Spike detection and event queuing
     *          - Event-driven spike processing
     *          - Working memory updates and maintenance
     *          - Neuromodulation effects on neural excitability
     *          - Plasticity rule application (STDP, Hebbian)
     *          - Memory system updates and consolidation
     *          - Prediction system updates
     *          - Attention system updates
     *          - Concept formation
     *          - Structural plasticity (every 100 steps)
     *          - Memory replay (every 100 steps)
     *          - Development system updates (every 1000 steps)
     *          - Memory consolidation (every 1000 steps)
     *          - Checkpoint management
     * 
     * @note This method implements the main neural computation loop for Phase 6
     *       where all systems are integrated into a coherent brain. The step
     *       duration is calculated from the timestep parameter in the configuration.
     * 
     * @see step(SimulationStep, Timestamp) for alternative with explicit time
     * @see getTotalNeuronCount() for neuron count after initialization
     * @see getTotalSpikeCount() for total spike count
     */
    void step(SimulationStep currentStep);
    
    // Main simulation step (with explicit time)
    /**
     * @brief Execute one simulation step with explicit time
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (in seconds)
     * @details Performs the same computation as step(SimulationStep) but with
     *          explicit time information for time-dependent computations.
     * 
     * @note Use this method when you need precise time information for:
     *       - Time-dependent plasticity rules
     *       - Temporal prediction systems
     *       - Reward prediction error computation
     *       - Time-coded neural responses
     * 
     * @see step(SimulationStep) for version with automatic time calculation
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    /**
     * @brief Process sensory input from the environment
     * @param input Sensory input data containing perceptual information
     * @details Injects current into sensory neurons based on the input pattern,
     *          effectively encoding environmental percepts as neural activity.
     *          This is the primary input pathway for the artificial brain.
     * 
     * @note This method implements the sensory transduction step in the brain loop:
     *       1. Environment produces percepts (sensory input)
     *       2. Brain receives and encodes these percepts as neural activity
     *       3. Neural computation transforms sensory information
     *       4. Brain produces actions based on transformed information
     * 
     * @param input Contains vision, audio, proprioception, and internal signals
     * @see injectCurrent() for direct neuron current injection
     * @see getTotalNeuronCount() for available sensory neurons count
     * @see getWorkingMemory() for memory-based processing
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    /**
     * @brief Inject current into a specific neuron
     * @param neuron ID of the neuron to receive current
     * @param current Current value to inject (in mV)
     * @details Directly injects current into a specific neuron, bypassing the
     *          sensory processing pathway. Useful for targeted neural stimulation
     *          or implementing specific experimental conditions.
     * 
     * @note This method provides low-level control over neural excitation,
     *       useful for:
     *       - Implementing specific stimulation protocols
     *       - Testing neuron response properties
     *       - Creating artificial neural patterns
     *       - Debugging neural computation
     * 
     * @param neuron Neuron identifier (must exist in brain)
     * @param current Current value in millivolts
     * @see injectCurrentToNeurons() for injecting to all neurons of a type
     * @see receiveSensoryInput() for high-level sensory input processing
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type Neuron type to target (Sensory, Motor, Internal, etc.)
     * @param current Current value to inject (in mV)
     * @details Injects current into all neurons of the specified type across
     *          all regions. This is more efficient than injecting to individual neurons
     *          when you need to affect an entire population.
     * 
     * @note Useful for:
     *       - Population-level stimulation
     *       - Testing type-specific responses
     *       - Implementing broad neural modulation
     *       - System-wide control signals
     * 
     * @param type Neuron type classification
     * @param current Current value in millivolts
     * @see injectCurrent() for single neuron injection
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    /**
     * @brief Get access to the spike processing system
     * @return Pointer to the SpikeSystem object
     * @details Accesses the event-driven spike processing system that handles
     *          real-time spike detection, delivery, and synaptic event management.
     *          Essential for understanding neural timing and spike propagation.
     * 
     * @return SpikeSystem* pointer, or nullptr if spike system not available
     * @see getSTDP() for plasticity system access
     * @see getHebbian() for Hebbian learning access
     */
    SpikeSystem* getSpikeSystem();
    /**
     * @brief Get constant access to the spike processing system
     * @return Const pointer to the SpikeSystem object
     * @see getSpikeSystem() for non-const version
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    /**
     * @brief Get access to the Spike-Timing-Dependent Plasticity system
     * @return Pointer to the STDP object
     * @details Accesses the STDP system that implements spike-timing-dependent
     *          learning rules where synaptic strength depends on the precise timing
     *          between pre- and post-synaptic spikes.
     * 
     * @return STDP* pointer, or nullptr if STDP system not available
     * @see getHebbian() for Hebbian learning access
     * @see getStructuralPlasticity() for structural plasticity access
     */
    STDP* getSTDP();
    /**
     * @brief Get access to the Hebbian learning system
     * @return Pointer to the Hebbian object
     * @details Accesses the Hebbian learning system that implements
     *          co-activation-based learning where synapses are strengthened
     *          when pre- and post-synaptic neurons fire together.
     * 
     * @return Hebbian* pointer, or nullptr if Hebbian system not available
     * @see getSTDP() for STDP access
     * @see getStructuralPlasticity() for structural plasticity access
     */
    Hebbian* getHebbian();
    /**
     * @brief Get access to the structural plasticity system
     * @return Pointer to the StructuralPlasticity object
     * @details Accesses the structural plasticity system that manages synapse
     *          formation (synaptogenesis) and removal (pruning), enabling the
     *          brain to develop its connectivity over time.
     * 
     * @return StructuralPlasticity* pointer, or nullptr if structural plasticity not available
     * @see getSTDP() for synaptic weight plasticity
     * @see getHebbian() for Hebbian learning
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    /**
     * @brief Get the excitation/inhibition ratio of the brain
     * @return Ratio of total excitatory weight to total inhibitory weight
     * @details Computes the overall balance of excitation and inhibition in the brain,
     *          which is crucial for stable neural computation and preventing runaway excitation.
     * 
     * @return float ratio (excitatory/inhibitory), returns infinity if only excitation, 0 if only inhibition
     * @see getTotalNeuronCount() for total neuron count
     * @see getTotalSynapseCount() for total synapse count
     */
    float getExcitationInhibitionRatio() const;
    /**
     * @brief Get the total number of spikes generated by the brain
     * @return Total spike count since initialization
     * @details Returns the cumulative number of spikes generated by all neurons
     *          in the brain since initialization. Useful for measuring neural activity
     *          and computational intensity.
     * 
     * @return size_t total spike count
     * @see getPendingSpikeEventCount() for pending spike events
     * @see getFiringNeuronCount() for currently firing neurons
     */
    size_t getTotalSpikeCount() const;
    /**
     * @brief Get the number of pending spike events
     * @return Number of spike events waiting to be processed
     * @details Returns the count of spike events that have been detected but not yet
     *          delivered to their target neurons. This is useful for understanding
     *          neural timing and event queue depth.
     * 
     * @return size_t number of pending spike events
     * @see getTotalSpikeCount() for total spikes generated
     * @see getSpikeSystem() for spike system access
     */
    size_t getPendingSpikeEventCount() const;
    /**
     * @brief Produce an action based on neural activity
     * @return Unique pointer to an Action object
     * @details Generates an action based on the activity of motor neurons in the brain.
     *          The action selection process involves competitive selection among motor
     *          neuron populations, considering factors like firing rate, coherence, and
     *          learned preferences.
     * 
     * @note This is the primary output pathway of the brain - converting neural activity
     *       into actionable behavior. The specific action type depends on which motor
     *       neuron population has the highest activity.
     * 
     * @return std::unique_ptr<Action> action object, or nullptr if no action selected
     * @see ActionType enum for available action types
     * @see getMotorNeurons() for motor neuron population
     */
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
    
    /**
     * @brief Get access to the dopamine neuromodulation system
     * @return Pointer to the Dopamine object
     * @details Accesses the dopamine system which implements reward and reinforcement
     *          signaling. Dopamine modulates neural excitability and plasticity,
     *          particularly in reward prediction error contexts.
     * 
     * @return Dopamine* pointer, or nullptr if dopamine system not available
     * @see getCuriosity() for curiosity system access
     * @see getNovelty() for novelty detection access
     * @see getPredictionErrorSignal() for prediction error access
     */
    Dopamine* getDopamine();
    /**
     * @brief Get access to the curiosity neuromodulation system
     * @return Pointer to the Curiosity object
     * @details Accesses the curiosity system which implements exploration motivation
     *          and drives the brain to seek novel and informative experiences.
     *          Curiosity modulates learning and exploration behavior.
     * 
     * @return Curiosity* pointer, or nullptr if curiosity system not available
     * @see getDopamine() for reward system access
     * @see getNovelty() for novelty detection access
     */
    Curiosity* getCuriosity();
    /**
     * @brief Get access to the novelty detection system
     * @return Pointer to the Novelty object
     * @details Accesses the novelty detection system which identifies unexpected
     *          or surprising events in the environment, driving attention and learning.
     *          Novelty signals capture the difference between predicted and actual input.
     * 
     * @return Novelty* pointer, or nullptr if novelty system not available
     * @see getCuriosity() for exploration motivation access
     * @see getDopamine() for reward signaling access
     */
    Novelty* getNovelty();
    /**
     * @brief Get access to the prediction error signal system
     * @return Pointer to the PredictionError object
     * @details Accesses the prediction error system which computes the difference
     *          between predicted and actual outcomes. This error signal drives learning
     *          and is fundamental to temporal difference learning.
     * 
     * @return PredictionError* pointer, or nullptr if prediction error system not available
     * @see getDopamine() for reward modulation access
     * @see getCuriosity() for exploration motivation access
     */
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    /**
     * @brief Get the brain's configuration
     * @return Shared pointer to constant configuration object
     * @details Provides read-only access to the configuration that governs the brain's
     *          behavior, parameters, and system settings. Useful for querying current
     *          configuration values or inspecting system setup.
     * 
     * @return std::shared_ptr<const Config> configuration object
     * @see Config class for available configuration parameters
     */
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    /**
     * @brief Get access to the random number generator
     * @return Pointer to the RandomGenerator object
     * @details Accesses the deterministic random number generator used throughout
     *          the brain for tasks like weight initialization, stochastic processes,
     *          and reproducible experiments.
     * 
     * @return RandomGenerator* pointer, or nullptr if random generator not available
     * @note The random seed can be set in the configuration to ensure reproducible results
     */
    RandomGenerator* getRandomGenerator();
    
    // Logging
    /**
     * @brief Log the current brain status
     * @details Logs comprehensive information about the brain's current state including:
     *          - Total neuron and synapse counts
     *          - Firing statistics and activity levels
     *          - Memory system status
     *          - Plasticity system state
     *          - Neuromodulation levels
     *          - Development stage
     * 
     * @note This is useful for debugging, monitoring, and understanding brain state.
     *       Log level should be set appropriately (info, debug, etc.) in configuration.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Plugin manager for custom component support
    PluginManager pluginManager;
};
};

} // namespace nlm

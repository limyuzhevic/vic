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
    RegionId sourceRegion;      // Source region ID for long-range connections
    RegionId targetRegion;      // Target region ID for long-range connections
    float weight;              // Connection weight (-1.0 to 1.0)
    Delay delay;               // Synaptic delay in simulation steps
    PlasticityFlags plasticityFlags;  // Plasticity flags for this connection
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system (Phase 6: Integrated Artificial Brain)
 * 
 * This class implements the complete integrated brain simulation with all Phase 6 systems:
 * - Core neural computation (LIF neurons, synapses, regions, populations)
 * - Working memory system with persistent activity and competition
 * - Episodic memory with experience encoding and replay
 * - Associative memory with Hebbian pattern associations
 * - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
 * - Prediction system integrated into brain loop
 * - Cognition systems (neural planner, concept formation, attentional selection)
 * - Development system with plasticity rate modulation
 * - Structural plasticity (synaptogenesis, pruning, maturation)
 * - Checkpoint save/load functionality
 * - Replay and consolidation mechanisms
 * 
 * The Brain class orchestrates all systems to create a coherent artificial brain loop:
 * WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE → MEMORY/PREDICTION →
 * MOTIVATION/NEUROMODULATION → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE →
 * REWARD/SURPRISE/ERROR → PLASTICITY → MEMORY/DEVELOPMENT → CHANGED BRAIN →
 * CHANGED FUTURE BEHAVIOR
 * 
 * This class represents the culmination of all previous phases and provides the
 * foundation for Phase 6 integration experiments. All systems are connected and
 * coordinated to function as a unified artificial brain.
 */
class Brain {
public:
    /// @brief Constructs a new brain instance with configuration
    /// @param config Shared pointer to configuration object containing all simulation parameters
    /// @details Creates a new brain instance with the provided configuration. The configuration
    ///         specifies all simulation parameters including neuron count, region count, connection
    ///         probability, synaptic parameters, and system settings. The brain is initialized but not
    ///         yet populated with neurons or regions - this happens in the initialize() method.
    explicit Brain(std::shared_ptr<Config> config);
    
    /// @brief Destructor - cleans up allocated memory for all brain systems
    ~Brain();
    
    // Disable copying, enable moving for performance and memory safety
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /// @brief Initializes the brain with all neural components and integrated systems
    /// @return bool true if initialization succeeds, false if configuration is invalid
    /// @details Initializes the complete Phase 6 integrated brain system:
    ///         - Creates neural regions and populations
    ///         - Sets up synaptic connectivity between neurons
    ///         - Initializes all memory systems (working, episodic, associative)
    ///         - Configures neuromodulation systems (dopamine, curiosity, novelty, prediction error)
    ///         - Sets up cognition systems (neural planner, concept formation, attention)
    ///         - Configures development system parameters
    ///         - Registers spike handlers for event-driven processing
    ///         - Configures checkpoint manager
    ///         - Sets up sensory and motor neuron collections
    bool initialize();
    
    /// @brief Performs one simulation step with default timestep (1ms)
    /// @param currentStep The current simulation step (incremented each call)
    /// @details This method orchestrates the complete integrated brain loop for one timestep:
    ///         - Processes pending delayed spike events (deliver synaptic input)
    ///         - Updates all neuron membrane potentials (LIF dynamics)
    ///         - Detects spikes and schedules outgoing spike events
    ///         - Updates working memory (maintenance and competition)
    ///         - Applies neuromodulation effects on neural excitability
    ///         - Applies plasticity rules (STDP, Hebbian)
    ///         - Updates episodic memory with current experience
    ///         - Updates prediction system
    ///         - Updates attention system
    ///         - Updates concept formation
    ///         - Applies structural plasticity (synaptogenesis, pruning)
    ///         - Replays important memories (during rest or periodically)
    ///         - Applies development effects
    ///         - Collects statistics
    ///         This is the main simulation entry point.
    void step(SimulationStep currentStep);
    
    /// @brief Performs one simulation step with custom timestep
    /// @param currentStep The current simulation step (incremented each call)
    /// @param currentTime The current simulation time in milliseconds (ms)
    /// @details Alternative simulation step method with explicit time parameter. This is useful
    ///         for simulations with non-uniform timesteps or when precise timing control is needed.
    ///         The implementation delegates to the step(SimulationStep) method for consistency.
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /// @brief Receives sensory input from the environment and processes it
    /// @param input The sensory input containing environmental data
    /// @details Processes sensory information and injects currents into sensory neurons
    ///         based on the input pattern. This implements the sensory processing stage of the
    ///         brain loop where external stimuli are converted to neural activity. The sensory
    ///         input is distributed across available sensory neurons to create patterns of
    ///         neural activation that can be stored in working memory and used for action selection.
    void receiveSensoryInput(const class SensoryInput& input);
    
    /// @brief Injects current directly into a specific neuron
    /// @param neuron The neuron ID to receive the current injection
    /// @param current The amount of current to inject (in nanoamps)
    /// @details Direct current injection is used for external control of neural activity,
    ///         such as testing specific neural responses or implementing external interventions.
    ///         This is different from synaptic input which occurs through synapses.
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /// @brief Injects current into all neurons of a specific type
    /// @param type The neuron type to target (Sensory, Internal, or Motor)
    /// @param current The current amount to inject to each neuron (in nanoamps)
    /// @details Bulk current injection for efficiency when multiple neurons need to be
    ///         affected simultaneously. This is more efficient than individual neuron injections
    ///         and is useful for implementing global modulatory effects or experimental manipulations.
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /// @brief Gets access to the spike system for event-driven spike processing
    /// @return SpikeSystem* Pointer to the spike system object, or nullptr if not available
    /// @details The spike system handles event-driven spike propagation and timing.
    ///         This provides access to spike-related functionality for advanced users who need
    ///         to manipulate spike events directly.
    SpikeSystem* getSpikeSystem();
    
    /// @brief Gets read-only access to the spike system
    /// @return const SpikeSystem* Const pointer to the spike system object, or nullptr if not available
    const SpikeSystem* getSpikeSystem() const;
    
    /// @brief Gets access to the spike-timing-dependent plasticity system
    /// @return STDP* Pointer to the STDP system object, or nullptr if not available
    STDP* getSTDP();
    
    /// @brief Gets access to the Hebbian plasticity system
    /// @return Hebbian* Pointer to the Hebbian system object, or nullptr if not available
    Hebbian* getHebbian();
    
    /// @brief Gets access to the structural plasticity system
    /// @return StructuralPlasticity* Pointer to the structural plasticity system object, or nullptr
    StructuralPlasticity* getStructuralPlasticity();
    
    /// @brief Calculates the excitation-to-inhibition ratio of the network
    /// @return float The ratio of total excitation to total inhibition, or infinity if no inhibition
    /// @details Computes the balance between excitatory and inhibitory influences in the network.
    ///         This is an important metric for network stability and function. A ratio close to
    ///         1.0 indicates balanced excitation and inhibition, which is typical for healthy neural networks.
    float getExcitationInhibitionRatio() const;
    
    /// @brief Gets the total number of spikes that have occurred in the simulation
    /// @return size_t Total spike count since brain initialization
    size_t getTotalSpikeCount() const;
    
    /// @brief Gets the number of pending spike events (spikes scheduled but not yet delivered)
    /// @return size_t Number of pending spike events in the spike system
    size_t getPendingSpikeEventCount() const;
    
    /// @brief Produces motor/action output based on motor neuron activity
    /// @return std::unique_ptr<class Action> Unique pointer to an action object
    /// @details Selects an action based on motor neuron activity. The motor neuron population
    ///         with the highest average activity determines the selected action. Currently, this
    ///         returns a simple action based on whether any motor neurons are firing, but can be
    ///         extended to support more complex action selection based on neural competition and
    ///         decision-making mechanisms.
    std::unique_ptr<class Action> produceAction();
    
    /// @brief Applies neuromodulatory signals to affect brain function
    /// @param signal The neuromodulator containing level and other parameters
    /// @details Applies neuromodulatory effects on plasticity and neural excitability. Neuromodulators
    ///         like dopamine, acetylcholine, and others affect learning rates, attention, and other
    ///         brain functions in a global manner. This is a key mechanism for adaptive behavior and
    ///         learning based on environmental feedback.
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
        
        // Validation methods for checkpoint operations
        bool validateBeforeSave() const;
        bool isValidNeuronState(const NeuronCheckpointData& neuronData, size_t idx) const;
        bool isValidSynapseParameters(float weight, Delay delay, size_t idx, 
                                      const SynapseCheckpointData& synapseData) const;
    
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
};

} // namespace nlm

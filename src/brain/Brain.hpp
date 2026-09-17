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

/**
 * @file Brain.hpp
 * @brief Brain class interface for Neural Learning Machine (NLM)
 * 
 * This file contains the Brain class interface, which serves as the central coordinator
 * of the neural system. The brain integrates multiple specialized subsystems including
 * memory systems, prediction systems, cognitive functions, and neuromodulation mechanisms
 * to create a complete artificial brain capable of real-time neural computation and learning.
 * 
 * @details
 * Phase 6 Implementation: This represents the integrated artificial brain architecture
 * featuring:
 * - 1,000+ neurons with configurable connectivity
 * - Real spiking neural computation (LIF dynamics)
 * - Event-driven synaptic transmission with delays
 * - Working memory for active information maintenance
 * - Episodic memory for experience storage and replay
 * - Prediction system for forward modeling
 * - Cognitive planning and concept formation
 * - Neuromodulation (dopamine, curiosity, novelty)
 * - Spike-timing dependent plasticity (STDP)
 * - Structural plasticity (synaptogenesis, pruning)
 * - Development stages (initial, critical period, maturation, adult)
 * - Checkpoint system for state saving/loading
 * 
 * @note
 * This is a complex, large-scale neural simulation engine designed to emulate
 * aspects of biological brain function for artificial intelligence applications.
 */

/**
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * This is the main brain class that coordinates all subsystems.
 * 
 * @ingroup Brain
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Configuration object containing simulation parameters
     *               including neuron count, region count, connection probability,
     *               timestep settings, and various plasticity parameters.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain instance and clean up resources
     */
    ~Brain();
    
    /**
     * @brief Disable copying, enable moving
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * This method sets up the brain with all neural components including regions,
     * neurons, connectivity, and integrated systems. It performs:
     * - Region creation and neuron population setup
     * - Random connectivity initialization
     * - Memory system initialization
     * - Prediction system setup
     * - Cognitive system initialization
     * - Neuromodulation system setup
     * - Spike system registration
     * - Checkpoint configuration
     * 
     * @return true if initialization succeeded, false otherwise
     * 
     * @throw std::bad_alloc if memory allocation fails
     * 
     * @ingroup Brain
     */
    bool initialize();
    
    /**
     * @brief Main simulation step method (overloaded for convenience)
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step implementation (Phase 6 integrated brain loop)
     * 
     * This is the complete integrated brain simulation loop that executes in order:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     * 
     * This implementation represents the complete integrated artificial brain
     * with all major neural computation systems operating in coordination.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (in milliseconds)
     * 
     * @ingroup Brain
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment and inject into sensory neurons
     * 
     * Processes sensory input data and distributes it to sensory neurons for processing.
     * This implements the interface between external environment and neural activity.
     * 
     * @param input Contains the sensory data values to be injected into the brain
     * 
     * @note This is a simple mapping implementation. More sophisticated sensory encoding
     *       could be implemented in Phase 2.
     * 
     * @ingroup Brain
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Searches through all brain regions to find the target neuron and injects
     * the specified current directly into its membrane.
     * 
     * @param neuron ID of the neuron to inject current into
     * @param current Current value to inject (positive for excitatory, negative for inhibitory)
     * 
     * @throws std::out_of_range if the neuron ID is not found (behavior may be undefined)
     * 
     * @ingroup Brain
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Searches through all brain regions and populations to find all neurons
     * of the specified type and injects current into each one.
     * 
     * @param type Neuron type to target (Sensory, Internal, or Motor)
     * @param current Current value to inject (positive for excitatory, negative for inhibitory)
     * 
     * @ingroup Brain
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get the spike system pointer
     * 
     * Provides access to the spike system for managing spike events and timing.
     * This is essential for event-driven neural computation.
     * 
     * @return Pointer to the spike system instance
     * 
     * @ingroup Brain
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get the spike system pointer (const version)
     * 
     * Provides read-only access to the spike system.
     * 
     * @return Const pointer to the spike system instance
     * 
     * @ingroup Brain
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get the STDP plasticity system pointer
     * 
     * Provides access to the spike-timing dependent plasticity system for learning
     * through temporal correlations between pre- and post-synaptic spikes.
     * 
     * @return Pointer to the STDP instance
     * 
     * @ingroup Brain
     */
    STDP* getSTDP();
    
    /**
     * @brief Get the Hebbian plasticity system pointer
     * 
     * Provides access to the Hebbian learning system for association-based
     * learning through simultaneous pre- and post-synaptic activity.
     * 
     * @return Pointer to the Hebbian instance
     * 
     * @ingroup Brain
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get the structural plasticity system pointer
     * 
     * Provides access to the structural plasticity system for synapse creation
     * (synaptogenesis) and elimination (pruning) based on activity patterns.
     * 
     * @return Pointer to the StructuralPlasticity instance
     * 
     * @ingroup Brain
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Calculate the excitation/inhibition ratio of the brain network
     * 
     * Computes the ratio of total excitatory synaptic weights to total inhibitory
     * synaptic weights, providing a measure of network balance.
     * 
     * @return E/I ratio (float). Returns infinity if no inhibition, 0 if no excitation
     * 
     * @ingroup Brain
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain initialization
     * 
     * Provides cumulative statistics of neural activity, useful for monitoring
     * overall brain engagement and learning progress.
     * 
     * @return Total number of spikes recorded across all neurons
     * 
     * @ingroup Brain
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events (immediate + delayed)
     * 
     * Counts active spike events that haven't yet been processed, useful for
     * monitoring temporal dynamics and system load.
     * 
     * @return Number of pending spike events
     * 
     * @ingroup Brain
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Implements simple action selection based on motor neuron population activity.
     * The motor neuron group with highest activity determines the action output.
     * This provides the interface between neural activity and physical world interaction.
     * 
     * @return Action object containing the selected action type
     * 
     * @note This is a placeholder implementation. Phase 2 will implement
     *       more sophisticated action selection using attention and planning systems.
     * 
     * @ingroup Brain
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to the brain network
     * 
     * Processes neuromodulatory signals (e.g., dopamine, acetylcholine) and applies
     * their effects on neural excitability and plasticity. This is central to
     * reinforcement learning and adaptive behavior.
     * 
     * @param signal Neuromodulator containing the signal level and type
     * 
     * @note Primarily modulates STDP learning rates based on signal strength.
     *       Other neuromodulators (novelty, curiosity) are updated during each step.
     * 
     * @ingroup Brain
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * This method is maintained for API compatibility. Plasticity is now applied
     * during each simulation step within the main step() method, eliminating
     * the need for a separate plasticity update call.
     * 
     * @deprecated Use the step() method instead where plasticity is applied
     * 
     * @ingroup Brain
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes to the brain
     * 
     * Updates the developmental system and adjusts structural plasticity parameters
     * based on the current developmental stage. This implements critical period
     * phenomena and synaptic maturation.
     * 
     * @note Developmental effects are applied during specific steps (every 1000 steps).
     *       Different developmental stages have different plasticity characteristics:
     *       - Initial: High plasticity, rapid learning
     *       - Critical period: Reduced plasticity, specialization
     *       - Maturation: Moderate plasticity, refinement
     *       - Adult: Low plasticity, stability
     * 
     * @ingroup Brain
     */
    void develop();
    
    /**
     * @brief Reset brain to initial state
     * 
     * Clears all neural activity, memory traces, and internal state variables.
     * Used for experiments requiring a clean slate or between trials.
     * 
     * @details This performs a complete reset including:
     * - Clear all simulation counters and timers
     * - Reset all neurons and synapses to resting state
     * - Clear memory systems (working, episodic, associative)
     * - Reset neuromodulation systems
     * - Reset developmental stage to Initial
     * 
     * @ingroup Brain
     */
    void reset();
    
    /**
     * @brief Save brain state to checkpoint file for persistence
     * 
     * Serializes the complete brain state including neurons, synapses, and
     * internal variables to a checkpoint file for later restoration.
     * 
     * @param filepath Path to the checkpoint file to write
     * 
     * @return true if save succeeded, false on error
     * 
     * @details The checkpoint contains:
     * - Neuron states (membrane potentials, thresholds, firing states)
     * - Synapse states (weights, delays, types)
     * - Simulation counters (step, time)
     * - Brain metadata (neuron/synapse counts)
     * 
     * @throws std::exception if file operations fail
     * 
     * @ingroup Brain
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from checkpoint file
     * 
     * Restores brain state from a previously saved checkpoint file.
     * 
     * @param filepath Path to the checkpoint file to load
     * 
     * @return true if load succeeded, false on error
     * 
     * @details This method reconstructs the brain state from checkpoint data.
     * Note that synapse reconnection is complex and requires mapping between
     * saved synapse IDs and current neural connectivity. For now, only neuron
     * states are restored, and synapse counts are logged.
     * 
     * @throws std::exception if file operations fail
     * 
     * @ingroup Brain
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * Creates a new NeuralRegion with the specified name and assigns it a unique
     * RegionId. Regions represent functionally distinct neural assemblies.
     * 
     * @param name Optional name for the region (defaults to empty string)
     * @return RegionId The unique identifier for the created region
     * 
     * @ingroup Brain
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a region by its ID
     * 
     * Searches through all regions to find the one matching the specified ID.
     * 
     * @param id RegionId to search for
     * @return Pointer to the found NeuralRegion, or nullptr if not found
     * 
     * @ingroup Brain
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a region by its ID (const version)
     * 
     * Const version that returns a const pointer to the found region.
     * 
     * @param id RegionId to search for
     * @return Const pointer to the found NeuralRegion, or nullptr if not found
     * 
     * @ingroup Brain
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return Number of regions
     * 
     * @ingroup Brain
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector containing all RegionId values
     * 
     * @ingroup Brain
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions (const version)
     * 
     * @return Const reference to vector of region pointers
     * 
     * @ingroup Brain
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection
     * 
     * Creates a long-range connection between two regions with specified weight and delay.
     * Inter-region connections enable communication between different neural assemblies.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (positive = excitatory, negative = inhibitory)
     * @param delay Connection delay in simulation steps
     * 
     * @ingroup Brain
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * Removes the connection between specified source and target regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * 
     * @ingroup Brain
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * @return Total number of neurons in the brain
     * 
     * @ingroup Brain
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * @return Total number of synapses in the brain (including inter-region connections)
     * 
     * @ingroup Brain
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count (neurons with non-zero membrane potential)
     * 
     * @return Number of neurons currently showing activity
     * 
     * @ingroup Brain
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count (neurons that spiked in current step)
     * 
     * @return Number of neurons that fired in the current simulation step
     * 
     * @ingroup Brain
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all regions
     * 
     * @return Average firing rate (Hz) across all regions
     * 
     * @ingroup Brain
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get working memory pointer
     * 
     * Provides access to the working memory system for maintaining active information
     * and supporting short-term memory functions.
     * 
     * @return Pointer to the working memory instance
     * 
     * @ingroup Brain
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory pointer
     * 
     * Provides access to the episodic memory system for storing and replaying
     * experience episodes.
     * 
     * @return Pointer to the episodic memory instance
     * 
     * @ingroup Brain
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory pointer
     * 
     * Provides access to the associative memory system for pattern completion
     * and recall based on partial cues.
     * 
     * @return Pointer to the associative memory instance
     * 
     * @ingroup Brain
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get the prediction system pointer
     * 
     * Provides access to the prediction system for forward modeling and error computation.
     * This enables the brain to predict sensory input and compute prediction errors.
     * 
     * @return Pointer to the prediction system instance
     * 
     * @ingroup Brain
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get the neural planner pointer
     * 
     * Provides access to the neural planning system for action selection and decision making.
     * This enables complex, forward-looking behavior based on environmental goals.
     * 
     * @return Pointer to the neural planner instance
     * 
     * @ingroup Brain
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get the concept formation system pointer
     * 
     * Provides access to the concept formation system for discovering and organizing
     * patterns from neural activity. This enables abstraction and generalization.
     * 
     * @return Pointer to the concept formation instance
     * 
     * @ingroup Brain
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get the attentional selection system pointer
     * 
     * Provides access to the attentional selection system for prioritizing information
     * and selecting which neural activity patterns receive processing resources.
     * 
     * @return Pointer to the attentional selection instance
     * 
     * @ingroup Brain
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Get the development system pointer
     * 
     * Provides access to the development system for managing developmental stages,
     * critical periods, and synaptic maturation over the brain's lifetime.
     * 
     * @return Pointer to the development system instance
     * 
     * @ingroup Brain
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage
     * 
     * Returns the current developmental stage of the brain, which influences
     * plasticity characteristics and learning capabilities.
     * 
     * @return Current developmental stage
     * 
     * @ingroup Brain
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage
     * 
     * Changes the developmental stage, which affects plasticity rates and learning
     * characteristics. This is used to implement critical periods and maturation.
     * 
     * @param stage The new developmental stage to set
     * 
     * @ingroup Brain
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Get the dopamine neuromodulator pointer
     * 
     * Provides access to the dopamine system, which encodes reward prediction errors
     * and modulates learning, motivation, and motor control.
     * 
     * @return Pointer to the dopamine instance
     * 
     * @ingroup Brain
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get the curiosity neuromodulator pointer
     * 
     * Provides access to the curiosity system, which drives exploration and
     * motivates learning in novel environments.
     * 
     * @return Pointer to the curiosity instance
     * 
     * @ingroup Brain
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get the novelty detection system pointer
     * 
     * Provides access to the novelty detection system, which identifies unexpected
     * patterns and triggers learning responses.
     * 
     * @return Pointer to the novelty detection instance
     * 
     * @ingroup Brain
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get the prediction error signal pointer
     * 
     * Provides access to the prediction error system, which computes the difference
     * between expected and actual sensory input, driving learning.
     * 
     * @return Pointer to the prediction error instance
     * 
     * @ingroup Brain
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get the configuration object pointer
     * 
     * Provides read-only access to the configuration object containing all
     * simulation parameters and settings.
     * 
     * @return Shared pointer to the configuration object
     * 
     * @ingroup Brain
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get the random number generator pointer
     * 
     * Provides access to the random number generator used for initializing
     * neural parameters, connectivity, and stochastic processes.
     * 
     * @return Pointer to the random generator instance
     * 
     * @ingroup Brain
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log comprehensive brain status information
     * 
     * Generates a detailed status report including statistics on neurons, synapses,
     * memory systems, neuromodulation, and developmental state. Useful for
     * monitoring simulation health and performance.
     * 
     * @ingroup Brain
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure for Brain
     * 
     * This structure contains all the internal data members of the Brain class.
     * Using the pImpl pattern to separate interface from implementation reduces
     * compilation dependencies and improves encapsulation.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#endif // BRAIN_HPP

} // namespace nlm

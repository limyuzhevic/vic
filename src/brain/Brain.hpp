// Brain.hpp

/**
 * @file Brain.hpp
 * @brief Central neural simulation brain class for NLM Phase 6
 * 
 * This file defines the Brain class, which serves as the main neural simulation
 * component for the NLM (熙然) artificial developmental brain project. The Brain
 * class integrates all neural systems including memory, neuromodulation,
 * prediction, cognition, and development into a coherent artificial brain loop.
 * 
 * @author NLM Research Team
 * @date Phase 6: Final Integration
 * @version 0.1.0
 */

#ifndef NLM_BRAIN_HPP
#define NLM_BRAIN_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/Attention.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

/**
 * @class Brain
 * @brief Central neural simulation brain class implementing Phase 6 integration
 * 
 * The Brain class represents the complete artificial neural substrate that:
 * - Integrates over 20 neural systems into a coherent brain loop
 * - Supports real-time neuromodulation and plasticity
 * - Implements multi-stage developmental progression
 * - Provides checkpoint persistence for long-term learning
 * - Connects to external environments via sensory/motor interfaces
 * 
 * The brain operates as a closed-loop system processing sensory input,
 * generating actions, and learning from experience through synaptic plasticity.
 * 
 * @note This is Phase 6: Final Integration. Previous phases built individual
 *       components; Phase 6 ensures they work together as a unified system.
 */
class Brain {
private:
    struct Impl;
    Impl* pImpl;

public:
    /**
     * @brief Construct a new Brain object with configuration
     * 
     * @param config Shared pointer to configuration object containing
     *              brain parameters such as neuron_count, region_count,
     *              simulation_timestep, and plasticity settings.
     */
    Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object and clean up resources
     */
    ~Brain();
    
    // Prevent copy operations
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor for Brain objects
     * 
     * Transfers ownership of the internal implementation from another brain
     * to this one, leaving the source brain in a valid but unspecified state.
     * 
     * @param other The Brain object to move from
     */
    Brain(Brain&& other) noexcept;
    
    /**
     * @brief Move assignment operator for Brain objects
     * 
     * Transfers ownership of the internal implementation from another brain
     * to this one, cleaning up any existing resources.
     * 
     * @param other The Brain object to move from
     * @return Brain& Reference to this brain after assignment
     */
    Brain& operator=(Brain&& other) noexcept;

    // ========== INITIALIZATION ==========
    /**
     * @brief Initialize the brain with configuration parameters
     * 
     * This method sets up the brain structure, creates neural regions and
     * neurons, establishes initial connectivity, and initializes all
     * integrated systems (memory, neuromodulation, plasticity, etc.).
     * 
     * @return bool True if initialization succeeds, false otherwise
     * @throws std::runtime_error if required configuration parameters are missing
     */
    bool initialize();
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * Resets all neural states, clears working memory, resets spike systems,
     * and resets developmental stage to Initial. Useful for running multiple
     * independent experiments.
     */
    void reset();

    // ========== SIMULATION ==========
    /**
     * @brief Perform a simulation step with current step only
     * 
     * Updates the brain state for one timestep using the current step number.
     * The actual time is calculated as currentStep * timestep.
     * 
     * @param currentStep The current simulation step (discrete timestep)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform a simulation step with current step and time
     * 
     * Updates the brain state for one timestep using both the current step
     * number and explicit timestamp. This allows for non-uniform timesteps
     * and time-based calculations.
     * 
     * @param currentStep The current simulation step
     * @param currentTime The current simulation time in milliseconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    // ========== SENSORY/MOTOR ==========
    /**
     * @brief Inject sensory input into the brain's sensory neurons
     * 
     * Processes external sensory data (vision, audio, internal signals) and
     * maps it to neural activity through sensory encoding mechanisms.
     * 
     * @param input The sensory input to process
     */
    void receiveSensoryInput(const SensoryInput& input);
    
    /**
     * @brief Inject current into a specific neuron
     * 
     * Directly modulates the membrane potential of a specific neuron with
     * excitatory or inhibitory current.
     * 
     * @param neuron The ID of the neuron to inject current into
     * @param current The current to inject (positive for excitation,
     *               negative for inhibition)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Targets all neurons of a given type (sensory, motor, internal, etc.)
     * with the same current injection.
     * 
     * @param type The type of neurons to inject current into
     * @param current The current to inject
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Generate motor action based on neural activity
     * 
     * Implements action selection by analyzing motor neuron activity patterns
     * and converting them to motor commands for the environment.
     * 
     * @return std::unique_ptr<Action> The selected motor action, or nullptr if
     *                               no action is selected
     */
    std::unique_ptr<Action> produceAction();

    // ========== NEURAL SYSTEMS ==========
    /**
     * @brief Get access to the spike system for event-driven processing
     * 
     * The spike system handles all spike-related events including spike
     * generation, propagation, and delayed synaptic input.
     * 
     * @return SpikeSystem* Pointer to the spike system, or nullptr if not initialized
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get read-only access to the spike system
     * 
     * @return const SpikeSystem* Constant pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the STDP (Spike-Timing-Dependent Plasticity) system
     * 
     * @return STDP* Pointer to the STDP system, or nullptr if not initialized
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to the Hebbian system, or nullptr if not initialized
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get access to the working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get access to the prediction system
     * 
     * @return PredictionSystem* Pointer to the prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get access to the neural planner system
     * 
     * @return NeuralPlanner* Pointer to the neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to the concept formation system
     * 
     * @return ConceptFormation* Pointer to the concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to the attentional selection system
     * 
     * @return AttentionalSelection* Pointer to the attentional selection system
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Get access to the development system
     * 
     * @return DevelopmentSystem* Pointer to the development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get access to the dopamine neuromodulator system
     * 
     * @return Dopamine* Pointer to the dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to the curiosity neuromodulator system
     * 
     * @return Curiosity* Pointer to the curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to the novelty neuromodulator system
     * 
     * @return Novelty* Pointer to the novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to the prediction error neuromodulator system
     * 
     * @return PredictionError* Pointer to the prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get access to the random number generator
     * 
     * @return RandomGenerator* Pointer to the random generator
     */
    RandomGenerator* getRandomGenerator();

    // ========== REGION MANAGEMENT ==========
    /**
     * @brief Add a new neural region to the brain
     * 
     * Creates a new neural region with the specified name and initializes it
     * with the default neuron populations.
     * 
     * @param name Optional name for the region (defaults to "Region_1", "Region_2", etc.)
     * @return RegionId The unique ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a neural region by its ID
     * 
     * @param id The unique ID of the region to retrieve
     * @return NeuralRegion* Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a neural region by its ID (const version)
     * 
     * @param id The unique ID of the region to retrieve
     * @return const NeuralRegion* Constant pointer to the region
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the total number of regions in the brain
     * 
     * @return size_t The number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get the IDs of all regions in the brain
     * 
     * @return std::vector<RegionId> Vector containing all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all neural regions in the brain
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add a connection between two neural regions
     * 
     * Establishes an inter-region synaptic connection with specified weight
     * and delay.
     * 
     * @param source The ID of the source region
     * @param target The ID of the target region
     * @param weight The synaptic weight of the connection
     * @param delay The synaptic delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay);
    
    /**
     * @brief Remove a connection between two neural regions
     * 
     * @param source The ID of the source region
     * @param target The ID of the target region
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    // ========== STATISTICS ==========
    /**
     * @brief Get the total number of neurons across all regions
     * 
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses across all regions
     * 
     * @return size_t Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of active neurons (firing or near threshold)
     * 
     * @return size_t Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently firing
     * 
     * @return size_t Number of firing neurons in the current timestep
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the total number of spikes generated by the brain
     * 
     * @return size_t Cumulative spike count
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events (spikes scheduled but not yet delivered)
     * 
     * @return size_t Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons
     * 
     * @return float Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the excitation/inhibition balance ratio
     * 
     * @return float Ratio of total excitatory weight to total inhibitory weight
     */
    float getExcitationInhibitionRatio() const;

    // ========== DEVELOPMENT ==========
    /**
     * @brief Get the current developmental stage of the brain
     * 
     * @return DevelopmentalStage The current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage of the brain
     * 
     * @param stage The developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // ========== MEMORY ACCESS ==========
    /**
     * @brief Get access to the working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();

    // ========== CHECKPOINT ==========
    /**
     * @brief Save the current brain state to a checkpoint file
     * 
     * Serializes the complete brain state including neuron states, synaptic
     * connections, and system parameters to a persistent storage medium.
     * 
     * @param filepath Path to the checkpoint file to save
     * @return bool True if save succeeds, false otherwise
     * @throws std::runtime_error if file cannot be written or serialized
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load a brain state from a checkpoint file
     * 
     * Deserializes brain state from persistent storage and restores the
     * neural network to its previous state.
     * 
     * @param filepath Path to the checkpoint file to load
     * @return bool True if load succeeds, false otherwise
     * @throws std::runtime_error if file cannot be read or validation fails
     */
    bool load(const std::string& filepath);

    // ========== CONFIGURATION ==========
    /**
     * @brief Get the configuration object used by this brain
     * 
     * @return std::shared_ptr<const Config> Shared pointer to the configuration
     */
    std::shared_ptr<const Config> getConfig() const;

    // ========== STATUS LOGGING ==========
    /**
     * @brief Log comprehensive status information about the brain
     * 
     * Outputs detailed information about brain state including neuron counts,
     * synaptic activity, memory system status, neuromodulation levels, and
     * developmental stage. Useful for debugging and monitoring.
     */
    void logStatus() const;

    // ========== NEUROMODULATION ==========
    /**
     * @brief Apply neuromodulation signal to plasticity mechanisms
     * 
     * Integrates neuromodulatory signals (dopamine, curiosity, prediction error)
     * into plasticity rules, affecting learning rates and synaptic modifications.
     * 
     * @param signal The neuromodulator signal to apply
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    /**
     * @brief Update plasticity mechanisms based on current brain state
     * 
     * Applies all plasticity rules (STDP, Hebbian, structural) based on recent
     * neural activity patterns.
     */
    void updatePlasticity();
    
    /**
     * @brief Update developmental processes
     * 
     * Advances developmental processes based on current simulation time,
     * affecting plasticity rates, connectivity, and system properties.
     */
    void develop();
};

} // namespace nlm

#endif // NLM_BRAIN_HPP
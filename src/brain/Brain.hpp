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
 * @brief The central coordinator of the neural system
 * 
 * The Brain class implements real spiking neural computation with event-driven dynamics
 * and integrates multiple cognitive and neurobiological systems. It serves as the main
 * computation engine for the NLM (Neural Learning Machine) framework.
 * 
 * Key features:
 * - Leaky Integrate-and-Fire (LIF) neuron dynamics
 * - Event-driven spike propagation with synaptic delays
 * - Multiple plasticity mechanisms (STDP, Hebbian, structural)
 * - Integrated memory systems (working, episodic, associative)
 * - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
 * - Cognitive systems (neural planner, concept formation, attention)
 * - Developmental system for plasticity modulation
 * - Checkpoint/restore functionality for persistent simulation
 * - Complete closed-loop interaction with environment
 * 
 * The brain operates as a complete biological-inspired system where:
 * 1. Sensory input is processed and integrated
 * 2. Neural dynamics generate activity patterns
 * 3. Working memory maintains active representations
 * 4. Prediction and cognitive systems guide behavior
 * 5. Neuromodulation regulates learning and exploration
 * 6. Motor output is produced based on neural activity
 * 7. Environmental feedback updates internal models
 * 8. Plasticity rules modify synaptic connections
 * 9. Memory systems consolidate and replay experiences
 * 10. Development modulates plasticity over time
 * 
 * The system is designed to support complex behaviors through emergent properties
 * rather than pre-programmed responses, allowing for adaptive and self-organizing behavior.
 */

class Brain {
public:
    /**
     * @brief Create a brain with the specified configuration
     * 
     * @param config Configuration for the brain (neuron count, regions, connectivity, etc.)
     * @throws std::runtime_error if configuration is invalid
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept = default;
    Brain& operator=(Brain&&) noexcept = default;
    
    // === Core Brain Interface ===
    
    /**
     * @brief Initialize the brain with configuration
     * 
     * Creates neurons, regions, populations, and initializes all integrated systems
     * including memory, plasticity, neuromodulation, and cognition.
     * 
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Execute one simulation step with default timestep
     * 
     * @param currentStep Current simulation step number
     * 
     * This performs the complete brain loop for one timestep, processing:
     * - Spike delivery from delayed spike events
     * - LIF neuron dynamics and spiking
     * - Spike event detection and queueing
     * - Working memory updates
     * - Neuromodulation effects
     * - Plasticity updates (STDP, Hebbian)
     * - Episodic memory updates
     * - Attention and concept formation
     * - Structural plasticity
     * - Memory replay
     * - Development effects
     * - Checkpoint management
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute one simulation step with specified timestep
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (seconds)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Process sensory input from the environment
     * 
     * @param input Sensory input data (vision, touch, internal signals, etc.)
     * 
     * Injects current into sensory neurons based on input patterns, enabling
     * the brain to perceive and respond to environmental stimuli.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron ID of the neuron to inject current into
     * @param current Current to inject (microamps)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Type of neurons to inject current into
     * @param current Current to inject (microamps)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // === System Access ===
    
    /**
     * @brief Get access to the spike system for event-driven processing
     */
    SpikeSystem* getSpikeSystem() noexcept;
    const SpikeSystem* getSpikeSystem() const noexcept;
    
    /**
     * @brief Get access to the STDP plasticity system
     */
    STDP* getSTDP() noexcept;
    
    /**
     * @brief Get access to the Hebbian plasticity system
     */
    Hebbian* getHebbian() noexcept;
    
    /**
     * @brief Get access to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity() noexcept;
    
    // === Statistics and Monitoring ===
    
    /**
     * @brief Get the excitation/inhibition ratio of the network
     * 
     * @return E/I ratio (excitatory / inhibitory synaptic weights)
     */
    float getExcitationInhibitionRatio() const noexcept;
    
    /**
     * @brief Get total spike count across all neurons
     */
    size_t getTotalSpikeCount() const noexcept;
    
    /**
     * @brief Get count of pending spike events (immediate and delayed)
     */
    size_t getPendingSpikeEventCount() const noexcept;
    
    /**
     * @brief Produce motor action based on current neural activity
     * 
     * @return Action object representing the motor command
     * 
     * Simple action selection based on motor neuron activity levels.
     * Can be overridden by neuromodulation or cognitive systems.
     */
    std::unique_ptr<class Action> produceAction();
    
    // === Neuromodulation ===
    
    /**
     * @brief Apply neuromodulatory signal to affect plasticity and behavior
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // === Plasticity Management ===
    
    /**
     * @brief Explicitly trigger plasticity updates
     * 
     * @note Plasticity is normally applied automatically during each step
     * This method is kept for API compatibility and advanced use cases
     */
    void updatePlasticity();
    
    // === Development Management ===
    
    /**
     * @brief Apply developmental changes to the brain
     * 
     * Modifies structural plasticity rates based on developmental stage,
     * making the brain more stable as it matures.
     */
    void develop();
    
    // === State Management ===
    
    /**
     * @brief Reset brain to initial state
     * 
     * Clears all internal states, memories, and neural configurations
     * for a fresh simulation start.
     */
    void reset();
    
    /**
     * @brief Save brain state to a checkpoint file
     * 
     * @param filepath Path to the checkpoint file
     * @return true if save succeeded, false otherwise
     * 
     * Saves neuron states, synaptic weights, memory contents, and
     * simulation progress for later restoration.
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from a checkpoint file
     * 
     * @param filepath Path to the checkpoint file
     * @return true if load succeeded, false otherwise
     * 
     * Restores all neural and cognitive state from a previous checkpoint.
     */
    bool load(const std::string& filepath);
    
    // === Region Management ===
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * @param name Human-readable name for the region (optional)
     * @return ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a neural region by ID
     */
    NeuralRegion* getRegion(RegionId id) noexcept;
    const NeuralRegion* getRegion(RegionId id) const noexcept;
    
    /**
     * @brief Get the number of regions in the brain
     */
    size_t getRegionCount() const noexcept;
    
    /**
     * @brief Get all region IDs in the brain
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const noexcept;
    
    // === Inter-Regional Connections ===
    
    /**
     * @brief Create a connection between two brain regions
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove a connection between two brain regions
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // === Global Statistics ===
    
    /**
     * @brief Get total number of neurons across all regions
     */
    size_t getTotalNeuronCount() const noexcept;
    
    /**
     * @brief Get total number of synapses across all regions
     */
    size_t getTotalSynapseCount() const noexcept;
    
    /**
     * @brief Get count of currently active neurons (firing or near threshold)
     */
    size_t getActiveNeuronCount() const noexcept;
    
    /**
     * @brief Get count of neurons that fired in the last step
     */
    size_t getFiringNeuronCount() const noexcept;
    
    /**
     * @brief Get average firing rate across all neurons (Hz)
     */
    float getAverageFiringRate() const noexcept;
    
    // === Memory System Access ===
    
    /**
     * @brief Get access to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory() noexcept;
    
    /**
     * @brief Get access to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory() noexcept;
    
    /**
     * @brief Get access to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory() noexcept;
    
    // === Prediction System ===
    
    /**
     * @brief Get access to the prediction system
     */
    PredictionSystem* getPredictionSystem() noexcept;
    
    // === Cognition System Access ===
    
    /**
     * @brief Get access to the neural planner
     */
    NeuralPlanner* getPlanner() noexcept;
    
    /**
     * @brief Get access to the concept formation system
     */
     */
    ConceptFormation* getConceptFormation() noexcept;
    
    /**
     * @brief Get access to the attentional selection system
     */
    AttentionalSelection* getAttention() noexcept;
    
    // === Development System ===
    
    /**
     * @brief Get access to the development system
     */
    DevelopmentSystem* getDevelopmentSystem() noexcept;
    
    /**
     * @brief Get the current developmental stage of the brain
     */
    DevelopmentalStage getDevelopmentalStage() const noexcept;
    
    /**
     * @brief Set the developmental stage (typically done automatically)
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // === Neuromodulation System Access ===
    
    /**
     * @brief Get access to the dopamine neuromodulation system
     */
    Dopamine* getDopamine() noexcept;
    
    /**
     * @brief Get access to the curiosity neuromodulation system
     */
    Curiosity* getCuriosity() noexcept;
    
    /**
     * @brief Get access to the novelty neuromodulation system
     */
    Novelty* getNovelty() noexcept;
    
    /**
     * @brief Get access to the prediction error neuromodulation system
     */
    PredictionError* getPredictionErrorSignal() noexcept;
    
    // === Configuration and Runtime ===
    
    /**
     * @brief Get the configuration used to create this brain
     */
    std::shared_ptr<const Config> getConfig() const noexcept;
    
    /**
     * @brief Get access to the random number generator
     */
    RandomGenerator* getRandomGenerator() noexcept;
    
    /**
     * @brief Get access to the spike system
     */
    SpikeSystem* getSpikeSystem() noexcept;
    const SpikeSystem* getSpikeSystem() const noexcept;
    
    // === Logging and Diagnostics ===
    
    /**
     * @brief Print detailed status information about the brain
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation details (pimpl idiom)
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

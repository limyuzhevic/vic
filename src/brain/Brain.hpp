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

// Create brain with configuration
    // 
    // Constructs a new Brain instance with the specified configuration.
    // The brain will initialize all neural subsystems (neurons, synapses, memory,
    // prediction, cognition, neuromodulation) based on the configuration parameters.
    // 
    // Args:
    //   config: Shared pointer to the configuration object containing all
    //           brain parameters including neuron count, connectivity, learning rates,
    //           simulation parameters, and subsystem settings.
    // 
    // Returns:
    //   None, but the configuration is stored for initialization later.
    explicit Brain(std::shared_ptr<Config> config);

    ~Brain();

    // Disable copying, enable moving
    // 
    // These operators prevent copying the Brain class (which contains
    // complex internal state and smart pointers) while allowing move operations
    // for efficient transfer of ownership.
    
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;

    // Initialize brain with configuration
    // 
    // Prepares the brain for simulation by:
    // 1. Creating and configuring neural regions
    // 2. Setting up neurons and synapses with random initial connections
    // 3. Initializing all integrated systems (memory, prediction, cognition, neuromodulation)
    // 4. Setting up the spike system and event queues
    // 5. Configuring checkpoint management
    // 
    // Returns:
    //   true if initialization succeeded, false otherwise
    bool initialize();

    // Main simulation step (single timestep)
    // 
    // Performs one simulation step at the default timestep. This is a convenience
    // wrapper for the two-parameter version that uses the configured timestep.
    // 
    // Args:
    //   currentStep: The current simulation step number
    void step(SimulationStep currentStep);

    // Simulation time step with precise timing
    // 
    // The main brain loop that updates all systems for one simulation timestep.
    // This implements the complete integrated brain simulation:
    // 1. Process pending delayed spike events (deliver synaptic input)
    // 2. Update all neurons (LIF dynamics)
    // 3. Detect spikes and schedule spike events
    // 4. Update working memory with current neural activity
    // 5. Apply neuromodulation effects on neural excitability
    // 6. Apply plasticity rules (STDP, Hebbian) with neuromodulation
    // 7. Store current state in episodic memory
    // 8. Update prediction system
    // 9. Update attention system
    // 10. Update concept formation
    // 11. Apply structural plasticity (synaptogenesis/pruning)
    // 12. Replay important memories
    // 13. Apply development effects
    // 14. Periodic memory consolidation
    // 15. Checkpoint management
    // 
    // Args:
    //   currentStep: The current simulation step number
    //   currentTime: The current simulation time in seconds
    void step(SimulationStep currentStep, Timestamp currentTime);

    // Receive sensory input from environment
    // 
    // Processes sensory input from the environment by injecting appropriate currents
    // into sensory neurons. This is the entry point for perception - all sensory
    // data flows through this method into the brain.
    // 
    // Args:
    //   input: The sensory input object containing perception data (vision, touch,
    //          internal signals, proprioception, etc.)
    void receiveSensoryInput(const class SensoryInput& input);

    // Inject current directly into a specific neuron
    // 
    // Manually injects current into a specific neuron, bypassing the normal
    // sensory processing pathway. Useful for direct neural stimulation or
    // testing specific neural responses.
    // 
    // Args:
    //   neuron: The ID of the neuron to inject current into
    //   current: The amount of current to inject (in membrane potential units)
    void injectCurrent(NeuronId neuron, MembranePotential current);

    // Inject current into all neurons of a specific type
    // 
    // Broadcasts current injection to all neurons of a given type (e.g., all
    // sensory, motor, or internal neurons). Used for global neural modulation
    // or testing system-wide effects.
    // 
    // Args:
    //   type: The neuron type to inject current into
    //   current: The amount of current to inject to each neuron
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // Spike system access
    // 
    // Returns pointers to the spike system for advanced manipulation of spike
    // events and timing. The spike system handles event-driven spike generation,
    // propagation with delays, and spike recording for plasticity.
    
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;

    // Plasticity system access
    // 
    // Returns pointers to the various plasticity systems that implement
    // learning rules:
    // - STDP: Spike-timing dependent plasticity
    // - Hebbian: Correlational Hebbian learning
    // - StructuralPlasticity: Long-term structural changes (synaptogenesis/pruning)
    
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();

    // Statistics
    // 
    // Various statistical measures about the current brain state:
    // - Excitation/Inhibition ratio: Balance of excitatory vs inhibitory connections
    // - Total spike count: Number of spikes generated in this simulation
    // - Pending spike events: Number of spikes scheduled but not yet delivered
    
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;

    // Produce motor/action output based on motor neuron activity
    // 
    // Interprets the current activity of motor neurons and produces an Action
    // object representing the brain's motor command. This is the output of
    // the brain that can be used by the agent or environment.
    // 
    // Returns:
    //   A unique pointer to an Action object (e.g., MoveForward, TurnLeft, Interact)
    std::unique_ptr<class Action> produceAction();

    // Apply neuromodulatory signals
    // 
    // Applies neuromodulatory signals (dopamine, curiosity, novelty, prediction error)
    // to the brain, affecting neural excitability and plasticity. This is how the
    // brain uses reward prediction errors and other neuromodulators to guide learning
    // and behavior.
    // 
    // Args:
    //   signal: The neuromodulator signal to apply
    void applyNeuromodulation(const class Neuromodulator& signal);

    // Update plasticity rules (called automatically in step)
    // 
    // This method is maintained for backward compatibility. Plasticity is now
    // applied automatically during each brain step. This method still exists
    // but does nothing when called directly.
    void updatePlasticity();

    // Apply developmental changes (called automatically in step)
    // 
    // This method is maintained for backward compatibility. Development is now
    // applied automatically during each brain step. This method still exists
    // but does nothing when called directly.
    void develop();

    // Reset brain state
    // 
    // Resets the brain to its initial state. This clears all neural activity,
    // resets memory systems, resets developmental stage to Initial, and
    // reinitializes the spike system. Note that this does NOT reset the
    // configuration or neural architecture.
    void reset();

    // Save brain state to file (checkpointing)
    // 
    // Saves the complete brain state (neurons, synapses, memory, development,
    // neuromodulation, etc.) to a file for persistence. This allows the brain
    // to resume from a saved state later, enabling long-term learning across
    // multiple simulation sessions.
    // 
    // Args:
    //   filepath: Path to the file where the brain state will be saved
    // 
    // Returns:
    //   true if save succeeded, false otherwise
    bool save(const std::string& filepath) const;

    // Load brain state from file
    // 
    // Loads a previously saved brain state from a file, restoring all neural
    // and system state. This is the counterpart to save().
    // 
    // Args:
    //   filepath: Path to the file containing the brain state to load
    // 
    // Returns:
    //   true if load succeeded, false otherwise
    bool load(const std::string& filepath);

    // Region management
    // 
    // Manages neural regions, which are groups of neurons with similar properties
    // that can be organized spatially or functionally. Regions help organize
    // the brain's neural architecture.
    
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;

    // Get all regions
    // 
    // Returns a const reference to all regions in the brain. This provides
    // read-only access to the brain region's organizational structure.
    
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    // Inter-region connection management
    // 
    // Manages long-range connections between different neural regions. These
    // connections allow for communication between spatially or functionally
    // distinct brain areas, enabling more complex information integration.
    
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);

    // Global statistics
    // 
    // High-level statistics about the brain's current state and performance:
    // - Total neuron count: Number of neurons in the brain
    // - Total synapse count: Number of synaptic connections
    // - Active neuron count: Neurons currently in active state
    // - Firing neuron count: Neurons currently firing
    // - Average firing rate: Population firing rate in Hz
    // - Excitation/Inhibition ratio: Network balance metric
    
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
    
    // Get current curiosity level
    float getCuriosityLevel() const;
    
    // Get current neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get current novelty level
    float getNoveltyLevel() const;
    
    // Get current prediction error
    float getPredictionError() const;
    
    // Get prediction error signal object
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

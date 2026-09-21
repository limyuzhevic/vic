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
// 
// This class represents the core neural simulation engine implementing
// real spiking neural computation with event-driven dynamics. It integrates
// multiple advanced cognitive and neural systems including:
// - Real spiking neurons with LIF dynamics
// - Synaptic plasticity (STDP, Hebbian, Structural)
// - Integrated memory systems (working, episodic, associative)
// - Prediction and cognition systems
// - Neuromodulation (dopamine, curiosity, novelty)
// - Developmental stages and plasticity modulation
// - Checkpoint and state management
// 
// The Brain orchestrates all neural computation and provides the interface
// between sensory input, internal processing, and motor output.
// 
// The brain operates in discrete timesteps with simulation time tracking.
// Each timestep updates neural dynamics, processes plasticity, updates
// memory systems, and applies neuromodulation effects.
// 
// Key properties:
// - Distributed across neural regions
// - Event-driven spike processing
// - Integrated cognitive architecture
// - Configurable through Config object
// - Checkpoint/restore capable
// 
// Typical workflow:
// 1. Initialize brain with configuration
// 2. Reset to initial state
// 3. For each timestep:
//    - Receive sensory input
//    - Process internal dynamics (spikes, plasticity)
//    - Apply neuromodulation
//    - Produce motor actions
//    - Save state if needed
// 
// Example usage in Python:
//   config = nlm.createDefaultConfig()
//   config.set("neuron_count", 1000)
//   config.set("region_count", 2)
//   brain = nlm.createBrain(config)
//   brain.initialize()
//   brain.reset()
//   
//   # Simulation loop
//   for step in range(100):
//       # Create sensory input
//       vision = nlm.Vision(16, 16)
//       brain.receiveSensoryInput(vision)
//       brain.step(step)
//       action = brain.produceAction()
// 
// Note: This is Phase 6 (Integrated Artificial Brain) of the NLM system.

class Brain {
public:
    // Create brain with configuration
     // 
     // Constructor creates a brain instance with the given configuration.
     // The configuration controls neural architecture, simulation parameters,
     // and integration settings for all cognitive systems.
     // 
     // @param config Shared configuration for the brain
     // 
     // Example:
     //   auto config = std::make_shared<Config>();
     //   config->set("neuron_count", 1000);
     //   Brain brain(config);
     
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    //! Destructor cleans up brain resources and saves final state
    //! 
    //! This destructor ensures proper cleanup of all internal systems
    //! including memory, regions, and neural components.

    ~Brain();
    
    //! @brief Disable copying - Brain cannot be copied (pointer-based architecture)
    
    Brain(const Brain&) = delete;
    //! @brief Disable assignment - Brain cannot be assigned (pointer-based architecture)

    Brain& operator=(const Brain&) = delete;
    //! @brief Enable move construction - Transfer ownership of pImpl

    Brain(Brain&&) noexcept;
    //! @brief Enable move assignment - Transfer ownership of pImpl

    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
     // 
     // Sets up the neural architecture and all integrated systems based on
     // configuration parameters. Creates regions, neurons, populations,
     // and initializes all cognitive and memory systems.
     // 
     // This must be called before any simulation steps. Creates a complete
     // neural architecture with:
     // - Neural regions (configurable)
     // - Neuron populations (sensory, internal, motor)
     // - Random connectivity matrix
     // - All memory system components
     // - All cognition systems
     // - Neuromodulation systems
     // - Spike system for event-driven processing
     // 
     // Returns:
     //   bool true if initialization succeeded, false otherwise
     // 
     // Example:
     //   if (brain.initialize()) {
     //       logStatus();  // Log brain statistics
     //   } else {
     //       logError("Brain initialization failed");
     //   }
     // 
     // Note: Configuration validation occurs during initialization.

    bool initialize();
    
// Main simulation step
     // 
     // Advances neural dynamics by one timestep. This is the core simulation loop
     // that implements the complete integrated brain computation (Phase 6).
     // The timestep performs:
     // - Processing of delayed spike events
     // - LIF neuron dynamics updates
     // - Spike detection and event scheduling
     // - Working memory updates
     // - Neuromodulation effects
     // - Plasticity updates (STDP, Hebbian)
     // - Memory system updates
     // - Prediction system updates
     // - Attention system updates
     // - Concept formation updates
     // - Structural plasticity
     // - Memory replay
     // - Development updates
     // - Statistics collection
     // 
     // Parameters:
     //   currentStep: Discrete simulation step number (monotonically increasing)
     // 
     // Example:
     //   for (SimulationStep step = 0; step < 1000; ++step) {
     //       brain.step(step);
     //   }

    void step(SimulationStep currentStep);

    //! @brief Simulation time step with explicit time parameter
    //! 
    //! Full simulation step with explicit timestamp for precise timing.
    //! This variant is used when precise time tracking is required.
    //!
    //! Parameters:
    //!   currentStep: Discrete simulation step number
    //!   currentTime: Absolute simulation time in seconds

    void step(SimulationStep currentStep, Timestamp currentTime);

    //! @brief Receive sensory input from environment
    //! 
    //! Injects current into sensory neurons based on the provided sensory input.
    //! This implements the sensory transduction process where external or
    //! internal stimuli are converted to neural current.
    //!
    //! Parameters:
    //!   input: SensoryInput containing the sensory data (Vision, Audio, etc.)
    //!
    //! The input is distributed across sensory neurons based on its values.
    //! Higher input values result in stronger neural activation.
    //!
    //! Example:
    //!   Vision vision(16, 16);
    //!   // Fill vision with visual data
    //!   brain.receiveSensoryInput(vision);

    void receiveSensoryInput(const class SensoryInput& input);

    //! @brief Inject current directly into a specific neuron
    //! 
    //! Delivers current directly to a specific neuron, bypassing normal
    //! synaptic transmission. Used for targeted neuromodulation or direct
    //! excitation/inhibition of specific neurons.
    //!
    //! Parameters:
    //!   neuron: Target neuron identifier
    //!   current: Current value to inject (positive = excitation, negative = inhibition)
    //!
    //! Note: Current is in membrane potential units (mV).
    //!
    //! Example:
    //!   brain.injectCurrent(NeuronId(42), 5.0f);  // Excite neuron 42

    void injectCurrent(NeuronId neuron, MembranePotential current);

    //! @brief Inject current into all neurons of a specific type
    //! 
    //! Delivers current to all neurons sharing the specified type. This is useful
    //! for broad neuromodulation effects, such as affecting all excitatory neurons
    //! or all sensory neurons simultaneously.
    //!
    //! Parameters:
    //!   type: Neuron type to target (Excitatory, Inhibitory, Sensory, Motor, etc.)
    //!   current: Current value to inject
    //!
    //! Note: This is more efficient than individual neuron injection.
    //!
    //! Example:
    //!   brain.injectCurrentToNeurons(NeuronType::Sensory, 2.0f);  // Excite all sensory neurons

    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
     // 
     // Provides access to the spike system for querying spike statistics
     // and registering custom spike handlers.
     // 
     // Returns:
     //   SpikeSystem* Pointer to the spike system (may be nullptr)
     // 
     // Example:
     //   auto* spikeSystem = brain.getSpikeSystem();
     //   if (spikeSystem) {
     //       size_t spikeCount = spikeSystem->getPendingSpikeCount();
     //   }

    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;

    // Plasticity system access
     // 
     // Provides access to various plasticity system components for
     // querying and modifying learning parameters.
     // 
     // Returns:
     //   STDP* Pointer to STDP system
     //   Hebbian* Pointer to Hebbian learning system
     //   StructuralPlasticity* Pointer to structural plasticity system
     // 
     // Example:
     //   auto* stdp = brain.getSTDP();
     //   if (stdp) {
     //       stdp->setLTPWeight(0.01f);
     //   }

    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
// Statistics
     // 
     // Provides access to key brain statistics for monitoring and analysis.
     // 
     // Returns:
     //   float Excitation/inhibition ratio (E/I ratio) - crucial for neural balance
     //   size_t Total spike count accumulated over entire simulation
     //   size_t Pending spike event count in the spike scheduler
     // 
     // Example:
     //   float eiRatio = brain.getExcitationInhibitionRatio();
     //   size_t totalSpikes = brain.getTotalSpikeCount();

    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;

    // Produce motor/action output based on neural activity
     // 
     // Analyzes motor neuron activity and produces a motor Action based on
     // the current neural state. Implements simple action selection based on
     // which motor neurons are firing.
     // 
     // Returns:
     //   std::unique_ptr<Action> Action to perform, or null Action if no neurons firing
     // 
     // Note: This produces Actions from ActionType enum (MoveForward, TurnLeft, etc.)
     // and can include parameters for fine-grained control.
     // 
     // Example:
     //   auto action = brain.produceAction();
     //   if (action && action->getType() == ActionType::MoveForward) {
     //       executeMoveForward(action->getParameters());
     //   }

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
    
// Apply neuromodulatory signals
     // 
     // Applies neuromodulation to plasticity systems. Neuromodulators such as
     // dopamine affect synaptic learning rates, creating reinforcement learning.
     // This is crucial for reward-based learning and behavioral adaptation.
     // 
     // Parameters:
     //   signal: Neuromodulator containing level and type of modulation
     // 
     // Example:
     //   Neuromodulator rewardSignal(DopamineLevel, 0.8f);
     //   brain.applyNeuromodulation(rewardSignal);
     //   // This scales STDP learning rates by 0.8

    void applyNeuromodulation(const class Neuromodulator& signal);

    //! @brief Update plasticity rules
    //! 
    //! This method is called during each simulation step to update synaptic
    //! plasticity according to the rules implemented by the plasticity systems
    //! (STDP, Hebbian, etc.). Plasticity enables learning and memory formation.
    //! 
    //! Note: In the current implementation, plasticity is applied within the
    //! step() method, so this method is kept for API compatibility.

    void updatePlasticity();

    //! @brief Apply developmental changes
    //! 
    //! Updates brain development according to current developmental stage.
    //! Development affects neural properties, plasticity rates, and cognitive
    //! capabilities. Different developmental stages (Initial, CriticalPeriod,
    //! Maturation, Adult, Aging) have different characteristics.
    //! 
    //! This is called during each simulation step when the developmental stage
    //! requires updates (typically every 1000 steps).

    void develop();

    //! @brief Reset brain state
    //! 
    //! Resets the brain to its initial state. This resets:
    //! - All neural regions and neurons
    //! - Spike system state
    //! - Developmental stage to Initial
    //! - All memory systems (working, episodic, associative)
    //! - Attention system
    //! - Neuromodulation state
    //! 
    //! After reset, the brain is ready for a new episode/simulation.
    //! 
    //! Example:
    //!   brain.reset();
    //!   // Brain is now at initial state, ready for new episode

    void reset();

    //! @brief Save brain state to file
    //! 
    //! Saves the current brain state (checkpointing) for later recovery.
    //! This includes:
    //! - All neuron membrane potentials and states
    //! - All synapse weights and delays
    //! - Inter-region connections
    //! - Memory system contents
    //! - Neuromodulation state
    //! 
    //! Parameters:
    //!   filepath: Path to save the checkpoint file
    //! 
    //! Returns:
    //!   bool true if save succeeded, false on error
    //! 
    //! Example:
    //!   if (brain.save("./checkpoints/episode_001.chk")) {
    //!       logInfo("Checkpoint saved successfully");
    //!   } else {
    //!       logError("Failed to save checkpoint");
    //!   }

    bool save(const std::string& filepath) const;

    //! @brief Load brain state from file
    //! 
    //! Restores brain state from a checkpoint file. This loads all neural,
    //! synaptic, memory, and system state from the saved file.
    //! 
    //! Parameters:
    //!   filepath: Path to the checkpoint file
    //! 
    //! Returns:
    //!   bool true if load succeeded, false on error
    //! 
    //! Example:
    //!   if (brain.load("./checkpoints/episode_001.chk")) {
    //!       logInfo("Checkpoint loaded successfully");
    //!   } else {
    //!       logError("Failed to load checkpoint");
    //!   }
    //!   // Brain is now at the saved state

    bool load(const std::string& filepath);

    //! @brief Add a new neural region
    //! 
    //! Creates a new neural region with a unique ID. Regions organize neurons
    //! into functional units (e.g., sensory, motor, association areas).
    //! Each region contains multiple neuron populations.
    //! 
    //! Parameters:
    //!   name: Human-readable name for the region (optional, auto-generated if empty)
    //! 
    //! Returns:
    //!   RegionId Unique identifier for the created region
    //! 
    //! Example:
    //!   RegionId sensoryRegionId = brain.addRegion("Sensory_Area");
    //!   auto* region = brain.getRegion(sensoryRegionId);
    //!   if (region) {
    //!       // Add neurons and populations to this region
    //!   }

    RegionId addRegion(const std::string& name = "");

    //! @brief Get neural region by ID
    //! 
    //! Retrieves a neural region using its unique identifier.
    //! 
    //! Parameters:
    //!   id: RegionId of the region to retrieve
    //! 
    //! Returns:
    //!   NeuralRegion* Pointer to the region, or nullptr if not found
    //! 
    //! Example:
    //!   RegionId targetRegion = RegionId(5);
    //!   NeuralRegion* region = brain.getRegion(targetRegion);
    //!   if (region) {
    //!       size_t neuronCount = region->getTotalNeuronCount();
    //!   }

    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;

    //! @brief Get the number of neural regions
    //! 
    //! Returns:
    //!   size_t Number of regions in the brain
    //! 
    //! Example:
    //!   size_t regionCount = brain.getRegionCount();
    //!   logInfo(std::format("Brain has {} regions", regionCount));

    size_t getRegionCount() const;

    //! @brief Get all region IDs
    //! 
    //! Returns:
    //!   std::vector<RegionId> All region IDs in the brain
    //! 
    //! Example:
    //!   auto regionIds = brain.getRegionIds();
    //!   for (const auto& id : regionIds) {
    //!       logInfo(std::format("Region: {}", id.index()));
    //!   }

    std::vector<RegionId> getRegionIds() const;

    //! @brief Get all regions
    //! 
    //! Returns:
    //!   const std::vector<std::unique_ptr<NeuralRegion>>& All neural regions
    //! 
    //! Note: Use getRegion(RegionId) for direct access by ID.
    //! 
    //! Example:
    //!   const auto& regions = brain.getRegions();
    //!   for (const auto& region : regions) {
    //!       logInfo(std::format("Region {} has {} neurons", 
    //!                          region->getId().index(), 
    //!                          region->getTotalNeuronCount()));
    //!   }

    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    //! @brief Add an inter-region connection
    //! 
    //! Creates a connection between two neural regions. These long-range
    //! connections enable communication and integration across different
    //! brain areas. Critical for higher cognitive functions and coordination.
    //! 
    //! Parameters:
    //!   source: Source region ID
    //!   target: Target region ID
    //!   weight: Connection strength (positive = excitation, negative = inhibition)
    //!   delay: Transmission delay in simulation steps
    //! 
    //! Example:
    //!   brain.addInterRegionConnection(sensoryRegion, associationRegion, 0.5f, 10);

    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);

    //! @brief Remove an inter-region connection
    //! 
    //! Removes a connection between neural regions. Used to disable or
    //! reconfigure long-range connectivity.
    //! 
    //! Parameters:
    //!   source: Source region ID
    //!   target: Target region ID

    void removeInterRegionConnection(RegionId source, RegionId target);

    //! @brief Get total neuron count across all regions
    //! 
    //! Returns:
    //!   size_t Total number of neurons in the brain
    //! 
    //! Example:
    //!   size_t totalNeurons = brain.getTotalNeuronCount();
    //!   logInfo(std::format("Brain has {} neurons total", totalNeurons));

    size_t getTotalNeuronCount() const;

    //! @brief Get total synapse count across all regions
    //! 
    //! Returns:
    //!   size_t Total number of synapses in the brain (including inter-region connections)
    //! 
    //! Example:
    //!   size_t totalSynapses = brain.getTotalSynapseCount();
    //!   logInfo(std::format("Brain has {} synapses total", totalSynapses));

    size_t getTotalSynapseCount() const;

    //! @brief Get count of active neurons
    //! 
    //! Returns:
    //!   size_t Number of neurons that are currently active (firing or near threshold)
    //! 
    //! Example:
    //!   size_t activeCount = brain.getActiveNeuronCount();
    //!   logInfo(std::format("Brain has {} active neurons", activeCount));

    size_t getActiveNeuronCount() const;

    //! @brief Get count of currently firing neurons
    //! 
    //! Returns:
    //!   size_t Number of neurons that are firing spikes this timestep
    //! 
    //! Example:
    //!   size_t firingCount = brain.getFiringNeuronCount();
    //!   logInfo(std::format("Brain has {} firing neurons this step", firingCount));

    size_t getFiringNeuronCount() const;

    //! @brief Get average firing rate across all neurons
    //! 
    //! Returns:
    //!   float Average firing rate (spikes per second) across all neurons
    //! 
    //! Example:
    //!   float avgRate = brain.getAverageFiringRate();
    //!   logInfo(std::format("Average firing rate: {:.2f} Hz", avgRate));

    float getAverageFiringRate() const;

} // namespace nlm

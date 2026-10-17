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
// 
// This class represents a complete artificial brain implementing Phase 6 integration.
// It coordinates all neural subsystems including working memory, episodic memory,
// prediction systems, cognition, neuromodulation, and development.
// 
// Key Features:
// - Real LIF (Leaky Integrate-and-Fire) neuron dynamics
// - Event-driven spike propagation with synaptic delays
// - Integrated memory systems (working, episodic, associative)
// - Prediction and error correction mechanisms
// - Cognitive functions (planning, concept formation, attention)
// - Neuromodulation (dopamine, curiosity, novelty detection)
// - Developmental plasticity and structural changes
// - Checkpoint save/load for persistence
// - Replay and consolidation mechanisms
// 
// The brain operates as a closed-loop system processing sensory input,
// generating motor output, and learning from rewards through plasticity.

class Brain {
public:
    /**
     * @brief Construct a new Brain object with the given configuration
     * 
     * @param config Configuration parameters for brain initialization including
     *               neuron count, region count, connection probability, etc.
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @throws std::invalid_argument if configuration is invalid
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object and clean up resources
     */
    ~Brain();
    
    /**
     * @brief Copy constructor (deleted - brains cannot be copied)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted - brains cannot be copied)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * @param other Brain to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * 
     * @param other Brain to move from
     * @return Brain& Reference to this brain
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with configuration parameters
     * 
     * This method creates neurons, populations, and initializes all integrated systems
     * including memory systems, prediction systems, cognition systems, neuromodulation,
     * and developmental systems. It establishes the initial neural architecture and
     * configures plasticity parameters.
     * 
     * @return bool true if initialization succeeded, false otherwise
     * 
     * @note This method should be called after brain construction but before simulation.
     * @throws std::runtime_error if critical initialization fails
     */
    bool initialize();
    
    /**
     * @brief Execute one simulation step (time-discrete version)
     * 
     * @param currentStep The current simulation step number (integer)
     * 
     * @note This is a convenience wrapper for the time-based version with
     *       timestep = 0.001. Use the two-parameter version for precise timing.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute one simulation step with precise timing
     * 
     * This is the core simulation method that advances the brain by one timestep.
     * It implements the complete integrated brain loop as described in Phase 6:
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
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time in seconds
     * 
     * @note This method implements the complete Phase 6 integrated brain loop.
     *       All integrated systems are updated in each step.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from the environment and inject into sensory neurons
     * 
     * This method processes external sensory data and converts it into neural current
     * injection for sensory neurons. It implements basic sensory encoding where
     * environmental input is scaled and distributed across the sensory neuron population.
     * 
     * @param input Sensory input data containing environmental observations
     * 
     * @note Sensory neurons are automatically created during brain initialization.
     *       This input is used for perceptual processing and motor control.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron by ID
     * 
     * @param neuron Neuron identifier to receive current injection
     * @param current Amount of current to inject (in nanoamps)
     * 
     * @note Used for targeted stimulation or external input.
     * @throws std::out_of_range if neuron ID is not found
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * This is useful for modeling structured input patterns such as
     * coordinated sensory stimulation or motor commands.
     * 
     * @param type Neuron type to receive current (Sensory, Internal, or Motor)
     * @param current Amount of current to inject to each neuron (in nanoamps)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Access the spike system for event-driven spike propagation
     * 
     * The spike system manages real-time spike events, including delayed spikes
     * for synaptic transmission. This is essential for the event-driven nature
     * of the neural simulation.
     * 
     * @return SpikeSystem* Pointer to the spike system, or nullptr if not initialized
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Constant version of getSpikeSystem()
     * 
     * @return const SpikeSystem* Constant pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the spike-timing-dependent plasticity (STDP) system
     * 
     * STDP implements synaptic strengthening (LTP) when presynaptic neurons
     * fire before postsynaptic neurons, and synaptic weakening (LTD) when the
     * reverse temporal order occurs.
     * 
     * @return STDP* Pointer to the STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Access the Hebbian plasticity system
     * 
     * Hebbian learning implements the principle "neurons that fire together,
     * wire together." This system strengthens synapses based on simultaneous
     * activation of pre- and postsynaptic neurons.
     * 
     * @return Hebbian* Pointer to the Hebbian plasticity system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access the structural plasticity system
     * 
     * Structural plasticity implements the creation (synaptogenesis) and
     * elimination (pruning) of synaptic connections over time, mimicking
     * biological brain development and learning.
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
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

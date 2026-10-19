#pragma once

/**
 * @file Brain.hpp
 * @brief Main brain class implementing Phase 6: Integrated Artificial Brain
 * @author NLM Framework
 * @version 6.0
 * @date 2024
 * 
 * @details
 * The Brain class represents the central coordinator of the neural system,
 * implementing real spiking neural computation with event-driven dynamics and
 * integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * This is Phase 6 of the NLM framework, representing the complete integration
 * of all neural subsystems into a unified artificial brain architecture.
 * 
 * @section architecture Architecture Overview
 * The Brain implements a complete integrated brain simulation with the following
 * key components:
 * 
 * 1. **Phase 2: Real Neural Computation**
 *    - Spiking neural dynamics with LIF neurons
 *    - Event-driven spike transmission
 *    - Synaptic plasticity (STDP, Hebbian, neuromodulated)
 * 
 * 2. **Memory Systems Integration**
 *    - Working memory: Transient active information storage
 *    - Episodic memory: Experience-based learning and replay
 *    - Associative memory: Pattern association and generalization
 * 
 * 3. **Prediction System**
 *    - Sensory prediction and error computation
 *    - Predictive coding and Bayesian inference
 * 
 * 4. **Cognition Systems**
 *    - Neural planning for goal-directed behavior
 *    - Concept formation for pattern discovery
 *    - Attentional selection for focus and filtering
 * 
 * 5. **Development System**
 *    - Activity-dependent structural changes
 *    - Plasticity rate modulation through developmental stages
 *    - Synaptogenesis and pruning
 * 
 * 6. **Neuromodulation Systems**
 *    - Dopamine: Reward prediction error and reinforcement learning
 *    - Curiosity: Exploration motivation and intrinsic reward
 *    - Novelty: Novelty detection for exploration
 *    - Prediction Error: General prediction error signals
 * 
 * @section brain_loop Brain Simulation Loop
 * The main simulation loop (Brain::step) implements the complete integrated
 * brain processing pipeline:
 * 
 * 1. **STEP 1**: Process pending delayed spikes (deliver synaptic input)
 * 2. **STEP 2**: Update all neurons (LIF dynamics)
 * 3. **STEP 3**: Detect spikes and schedule spike events
 * 4. **STEP 4**: Update working memory (maintenance and competition)
 * 5. **STEP 5**: Apply neuromodulation effects on neural excitability
 * 6. **STEP 6**: Apply plasticity rules (STDP and Hebbian with neuromodulation)
 * 7. **STEP 7**: Update episodic memory with current experience
 * 8. **STEP 8**: Update prediction system
 * 9. **STEP 9**: Update attention system
 * 10. **STEP 10**: Update concept formation
 * 11. **STEP 11**: Apply structural plasticity (synaptogenesis and pruning)
 * 12. **STEP 12**: Replay important memories (during rest or periodically)
 * 13. **STEP 13**: Apply development effects
 * 14. **STEP 14**: Periodic memory consolidation
 * 15. **STEP 15**: Checkpoint management
 * 
 * @section thread_safety Thread Safety
 * The Brain class is **NOT thread-safe**. All access to brain state must be
 * synchronized by external synchronization mechanisms. The brain maintains
 * internal state that should not be modified concurrently.
 * 
 * @section performance Performance Considerations
 * - Event-driven spike processing for efficiency
 * - Integration of multiple systems reduces memory overhead
 * - Configurable checkpointing for long simulations
 * - Adaptive plasticity based on neuromodulation
 * 
 * @section configuration Configuration
 * Configuration is loaded from file or command line arguments via the Config
 * class. Key parameters include neuron count, region count, connection
 * probability, simulation timestep, and various subsystem parameters.
 * 
 * @note This implementation requires C++11 or later for modern C++ features.
 */

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations for integrated systems
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

/**
 * @brief Long-range connection between neural regions
 * 
 * Represents inter-regional connectivity that enables communication between
 * different brain regions. These connections can have delays and plasticity flags
 * that control learning dynamics across regions.
 */
struct InterRegionConnection {
    /** Source region ID */
    RegionId sourceRegion;
    /** Target region ID */
    RegionId targetRegion;
    /** Synaptic weight of the connection */
    float weight;
    /** Transmission delay in simulation steps */
    Delay delay;
    /** Plasticity rules applicable to this connection */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     * Creates a zero-weight connection with default delay and no plasticity.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Initial synaptic weight (default: 0.0)
     * @param d Transmission delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Main brain class implementing Phase 6: Integrated Artificial Brain
 * @details
 * The Brain class serves as the central coordinator of the neural system,
 * managing all integrated subsystems and coordinating their interaction.
 * 
 * Key responsibilities:
 * - Initialize and manage all neural subsystems
 * - Process the complete brain simulation loop
 * - Handle sensory input and motor output
 * - Manage memory systems (working, episodic, associative)
 * - Coordinate prediction, cognition, development, and neuromodulation
 * - Provide checkpointing and state persistence
 * - Log system status and statistics
 * 
 * The brain uses a pImpl pattern for implementation hiding, with all complex
 * internal state encapsulated in the private Impl class.
 */
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
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

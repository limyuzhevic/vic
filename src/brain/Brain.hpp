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
// ============================================================================
// BIOLOGICAL INSPIRATION AND MATHEMATICAL MODELS
// ============================================================================
//
// This implementation is based on several key biological principles:
//
// 1. **Leaky Integrate-and-Fire (LIF) Neurons**:
//    - Models membrane potential dynamics with leak current
//    - Simple yet biologically plausible approximation of neuronal behavior
//    - Captures threshold-based firing with refractory periods
//    - Equation: τ_v * dv/dt = -v + R*I, where τ_v is membrane time constant
//
// 2. **Spike-Timing Dependent Plasticity (STDP)**:
//    - Asymmetric Hebbian learning based on spike timing
//    - Pre-before-post: Long-Term Potentiation (LTP)
//    - Post-before-pre: Long-Term Depression (LTD)
//    - Core principle: Neurons that fire together strengthen connections
//      if they fire at optimal times, but weaken if timing is wrong
//    - Mathematical model: Weight change = A_plus * exp(-Δt/τ_plus) - A_minus * exp(Δt/τ_minus)
//
// 3. **Hebbian Learning**:
//    - Co-activation based learning: "Neurons that fire together wire together"
//    - Strengthens connections between simultaneously active neurons
//    - Basis for associative memory and pattern recognition
//
// 4. **Neuromodulation**:
//    - Dopamine: Reward prediction error and reinforcement learning
//    - Curiosity: Intrinsic motivation and exploration
//    - Novelty: Detection of unexpected events
//    - Prediction Error: Learning from prediction mismatches
//
// 5. **Working Memory**:
//    - Active maintenance of information over short timescales
//    - Competitive dynamics for winner-take-all selection
//    - Inspired by prefrontal cortex and basal ganglia circuits
//
// 6. **Episodic Memory**:
//    - Storage of specific experiences with temporal context
//    - Implements memory consolidation and replay mechanisms
//
// 7. **Prediction Systems**:
//    - Forward models predicting sensory input
//    - Comparison with actual input generates prediction errors
//    - Drives learning and motor control
//
// 8. **Cognition Systems**:
//    - Planning (NeuralPlanner): Action selection with future consequences
//    - Concept Formation: Abstraction and categorization
//    - Attentional Selection: Information filtering and prioritization
//
// 9. **Structural Plasticity**:
//    - Dynamic rewiring of neural connections
//    - Synaptogenesis: Formation of new connections
//    - Pruning: Elimination of weak or unused connections
//
// 10. **Development**:
//     - Experience-dependent maturation of neural circuits
//     - Critical period plasticity windows
//     - Adaptation to environmental demands
//
// ============================================================================
// INTEGRATED BRAIN LOOP ARCHITECTURE
// ============================================================================
//
// The brain operates as a 15-step event-driven loop that integrates
// multiple computational modules in a tightly coupled system:
//
// Step 1: Process pending delayed spike events (deliver synaptic input)
// Step 2: Update all neuron membrane potentials (LIF dynamics)
// Step 3: Detect spikes and schedule outgoing spike events
// Step 4: Update working memory (maintenance and competition)
// Step 5: Apply neuromodulation effects on neural excitability
// Step 6: Apply plasticity rules (STDP, Hebbian)
// Step 7: Update episodic memory with current experience
// Step 8: Update prediction system
// Step 9: Update attention system
// Step 10: Update concept formation
// Step 11: Apply structural plasticity (synaptogenesis, pruning)
// Step 12: Replay important memories (during rest or periodically)
// Step 13: Apply development effects
// Step 14: Collect statistics
//
// This loop represents the complete neural computation cycle from input
// processing through learning and memory consolidation.
//
// ============================================================================
// PERFORMANCE AND SCALABILITY CONSIDERATIONS
// ============================================================================
//
// - Event-driven architecture ensures computational efficiency
// - Parallel processing of neurons within and across regions
// - Memory systems integrate with neural dynamics for active maintenance
// - Neuromodulation provides global control and adaptive behavior
// - Structural plasticity allows for ongoing circuit remodeling
//
// ============================================================================
// ERROR HANDLING AND ROBUSTNESS
// ============================================================================
//
// The implementation includes comprehensive error checking:
// - Null pointer validation for all system components
// - Parameter range checking and clamping
// - Graceful degradation when subsystems fail
// - Detailed logging for debugging and monitoring
// - Recovery mechanisms for partial initialization failures

class Brain {

public:
    /**
     * @brief Create brain with configuration
     * @param config Shared pointer to configuration object containing all brain parameters
     * @details Constructs the complete integrated brain system with all subsystems.
     * Configuration includes neuron counts, connectivity parameters, plasticity rates,
     * neuromodulation settings, and developmental parameters.
     */
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
    
// ========== PHASE 6: INTEGRATED BRAIN LOOP HELPER METHODS ==========

// Helper methods for the integrated brain simulation loop
// These methods break down the main step() function into focused, testable components
// following the Phase 6 integrated brain architecture

private:
    // ========== STEP 1: PROCESS DELAYED SPIKES ==========
    /**
     * @brief Process pending delayed spike events to deliver synaptic input
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @details Implements spike timing dependent plasticity (STDP) delivery
     * @note This is Step 1 in the 15-step integrated brain loop
     */
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== STEP 2: UPDATE NEURON DYNAMICS ==========
    /**
     * @brief Update all neuron membrane potentials (LIF dynamics)
     * @param currentTime Current simulation time
     * @details Implements Leaky Integrate-and-Fire (LIF) neuron dynamics
     * including membrane potential updates, refractory periods, and current injection
     * @note This is Step 2 in the 15-step integrated brain loop
     */
    void updateNeuronDynamics(Timestamp currentTime);
    
    // ========== STEP 3: DETECT AND PROCESS SPIKES ==========
    /**
     * @brief Detect neuron spikes and schedule spike events
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @details Scans all neurons for spiking, queues spike events, and records
     * synaptic plasticity spikes for pre- and post-synaptic neurons
     * @note This is Step 3 in the 15-step integrated brain loop
     */
    void detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== STEP 4: UPDATE WORKING MEMORY ==========
    /**
     * @brief Update working memory dynamics and competition
     * @param timestep Current timestep duration
     * @details Maintains transient active information through competitive
     * activation of memory traces. Implements the working memory subsystem
     * @note This is Step 4 in the 15-step integrated brain loop
     */
    void updateWorkingMemory(double timestep);
    
    // ========== STEPS 5-8: NEUROMODULATION AND PLASTICITY ==========
    /**
     * @brief Apply neuromodulatory effects and plasticity rules
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @param timestep Current timestep duration
     * @details Integrates multiple neuromodulation systems (dopamine, curiosity,
     * novelty, prediction error) with neural plasticity mechanisms
     * @note Combines Steps 5-8: Neuromodulation effects, STDP, Hebbian plasticity,
     * episodic memory, and prediction system updates
     */
    void applyNeuromodulation(SimulationStep currentStep, Timestamp currentTime, double timestep);
    
    // ========== STEP 6: APPLY PLASTICITY RULES ==========
    /**
     * @brief Apply STDP and Hebbian plasticity rules
     * @param currentTime Current simulation time
     * @param timestep Current timestep duration
     * @details Implements spike-timing dependent plasticity (STDP) and
     * Hebbian co-activation learning with neuromodulatory modulation
     * @note This is Step 6 in the 15-step integrated brain loop
     */
    void applyPlasticityRules(Timestamp currentTime, double timestep);
    
    // ========== STEP 7: UPDATE EPISODIC MEMORY ==========
    /**
     * @brief Update episodic memory with current experience
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @details Captures current brain state as episodic episodes, storing
     * active neurons and reward information for later replay and consolidation
     * @note This is Step 7 in the 15-step integrated brain loop
     */
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== STEP 8: UPDATE PREDICTION SYSTEM ==========
    /**
     * @brief Update prediction system with sensory observations
     * @param currentTime Current simulation time
     * @details Maintains predictive models of sensory input and computes
     * prediction errors for learning and adaptation
     * @note This is Step 8 in the 15-step integrated brain loop
     */
    void updatePredictionSystem(Timestamp currentTime);
    
    // ========== STEPS 9-10: COGNITION SYSTEMS ==========
    /**
     * @brief Update cognition systems (attention, concept formation)
     * @param timestep Current timestep duration
     * @details Integrates attentional selection and concept formation for
     * higher-order cognitive processing and information filtering
     * @note Combines Steps 9-10: Attention system and concept formation
     */
    void updateCognitionSystems(double timestep);
    
    // ========== STEP 9: UPDATE ATTENTION SYSTEM ==========
    /**
     * @brief Update attention system dynamics
     * @param timestep Current timestep duration
     * @details Implements attentional selection mechanism for focusing
     * resources on relevant information and suppressing irrelevant inputs
     */
    void updateAttentionSystem(double timestep);
    
    // ========== STEP 10: UPDATE CONCEPT FORMATION ==========
    /**
     * @brief Update concept formation system
     * @param currentTime Current simulation time
     * @details Discovers and learns abstract concepts from neural activity
     * patterns and sensory inputs
     */
    void updateConceptFormation(Timestamp currentTime);
    
    // ========== STEP 11: APPLY STRUCTURAL PLASTICITY ==========
    /**
     * @brief Apply structural plasticity (synaptogenesis, pruning)
     * @param currentStep Current simulation step
     * @param rng Random number generator
     * @details Modifies brain topology through new synapse formation and
     * elimination of weak connections (structural rewiring)
     * @note This is Step 11 in the 15-step integrated brain loop
     */
    void applyStructuralPlasticity(SimulationStep currentStep, const RandomGenerator& rng);
    
    // ========== STEP 12: REPLAY MEMORIES ==========
    /**
     * @brief Replay important memories for consolidation
     * @param currentStep Current simulation step
     * @details Reactivates significant memory traces during rest or
     * periodically to strengthen connections and integrate information
     * @note This is Step 12 in the 15-step integrated brain loop
     */
    void replayMemories(SimulationStep currentStep);
    
    // ========== STEP 13: APPLY DEVELOPMENT EFFECTS ==========
    /**
     * @brief Apply developmental stage effects
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @param timestep Current timestep duration
     * @details Modifies plasticity rates and system properties based on
     * developmental stage (initial, critical period, maturation, adult)
     * @note This is Step 13 in the 15-step integrated brain loop
     */
    void applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime, double timestep);
    
    // ========== STEP 14: COLLECT STATISTICS ==========
    /**
     * @brief Collect neural and system statistics
     * @details Gathers performance metrics, spike statistics, and system health
     * indicators for monitoring and analysis
     * @note This is Step 14 in the 15-step integrated brain loop
     */
    void collectStatistics();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

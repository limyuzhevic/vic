#ifndef BRAIN_HPP
#define BRAIN_HPP

#include "../core/Types/Types.hpp"
#include "BrainAbstractions.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <unordered_map>

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
class Neuromodulator;
class SensoryInput;
class Action;

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

class Brain {
public:
    // Create brain with configuration
    /**
     * 
     * **Brain Constructor**
     * ======================
     * 
     * Creates a new integrated brain system.
     * 
     * **Parameters**
     * -------------
     * config: Shared configuration for all brain subsystems
     * 
     * **Initialization**
     * -----------------
     * The constructor performs Phase 1 initialization:
     * - Sets up core infrastructure (config, timing, random number generator)
     * - Creates placeholder implementations for all integrated systems
     * - Configures default parameters and settings
     * 
     * **System Architecture**
     * -----------------------
     * The brain is organized into major subsystems:
     * 
     * **Core Infrastructure**
     * ----------------------
     * - Configuration management
     * - Random number generation
     * - Simulation timing and clocking
     * 
     * **Memory Systems**
     * ------------------
     * - NeuralWorkingMemory: Short-term active information
     * - NeuralEpisodicMemory: Experience storage and replay
     * - NeuralAssociativeMemory: Pattern completion
     * 
     * **Computation Systems**
     * ----------------------
     * - PredictionSystem: Forward models and prediction error
     * - NeuralPlanner: Action planning and decision making
     * - ConceptFormation: Pattern discovery and abstraction
     * - AttentionalSelection: Focus and resource allocation
     * 
     * **Development System**
     * ----------------------
     * - DevelopmentSystem: Neural maturation and learning trajectories
     * 
     * **Neuromodulation Systems**
     * -----------------------------
     * - Dopamine: Reward prediction error and reinforcement
     * - Curiosity: Exploration motivation
     * - PredictionError: Novelty detection and learning signals
     * - Novelty: Unexpected event detection
     * 
     * **Plasticity Systems**
     * -----------------------
     * - SpikeSystem: Spike generation and event handling
     * - STDP: Spike-timing dependent plasticity
     * - Hebbian: Co-activation learning
     * - StructuralPlasticity: Network topology changes
     * 
     * **Lifecycle**
     * ----------
     * - Uses pImpl idiom for efficient memory management
     * - Supports move semantics for transfer between contexts
     * - Designed for long-running simulation processes
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    /**
     * 
     * **Memory Management**
     * ====================
     * 
     * **Move Semantics**
     * -----------------
     * The brain uses move semantics for efficient transfer:
     * 
     * **Move Constructor**: Transfers ownership of implementation
     * **Move Assignment**: Transfers ownership, cleans up old
     * 
     * **Copy Prevention**
     * ------------------
     * Copying is disabled because:
     * - Brain contains complex internal state
     * - Multiple brains would conflict
     * - Move semantics provide sufficient flexibility
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
// Initialize brain with configuration
    /**
     * 
     * **Brain Initialization**
     * ========================
     * 
     * **initialize()**
     * -----------
     * 
     * Completes brain initialization by setting up neural regions,
     * populations, connectivity, and all integrated systems.
     * 
     * **Initialization Overview**
     * ---------------------------
     * Three-phase initialization process:
     * 1. **Region Setup**: Create anatomical brain regions
     * 2. **Connectivity Setup**: Establish synaptic connections
     * 3. **System Integration**: Enable all supporting systems
     * 
     * **Biological Architecture**
     * ---------------------------
     * Mirroring biological organization:
     * - **Sensory regions**: External/internal stimulus detection
     * - **Internal regions**: Information processing and storage
     * - **Motor regions**: Response generation
     * - **Cross-region connections**: Inter-regional communication
     * 
     * **Returns**
     * -------
     * bool: true if initialization successful, false on errors
     * 
     * **Error Handling**
     * -----------------
     * - Checks for NULL implementation
     * - Validates configuration
     * - Logs detailed error messages
     * 
     * **Configuration Parameters**
     * ----------------------------
     * - neuron_count: Total neurons (default: 1000)
     * - region_count: Number of regions (default: 1)
     * - connection_probability: Connectivity density (default: 0.1f)
     * - stdp_ltp_weight: LTP learning rate (default: 0.01f)
     * - stdp_ltd_weight: LTD learning rate (default: 0.012f)
     * - synaptogenesis_rate: New synapse creation rate
     * - pruning_rate: Synapse elimination rate
     * 
     * **Neuron Distribution**
     * ----------------------
     * Per region:
     * - Sensory neurons: 25% (input encoding)
     * - Internal neurons: 50% (processing/storage)
     * - Motor neurons: 25% (output generation)
     */
    bool initialize();
    
    // Main simulation step
    /**
     * 
     * **Simulation Step**
     * ===================
     * 
     * **step(SimulationStep currentStep)**
     * -------------------------------
     * 
     * Performs one simulation step using the current step index.
     * 
     * **Uses**
     * -----
     * Standard timestep (1ms) from configuration
     * 
     * **Parameters**
     * -------------
     * currentStep: Zero-based step index
     */
    void step(SimulationStep currentStep);
    
    // Simulation time step
    /**
     * 
     * **Simulation Step with Time**
     * =============================
     * 
     * **step(SimulationStep currentStep, Timestamp currentTime)**
     * -------------------------------------------------------------
     * 
     * Performs one simulation step with explicit time.
     * 
     * **Complete Integration Loop**
     * -----------------------------
     * This implements Phase 6: The complete integrated brain simulation:
     * 
     * **15-Step Integration Cycle**
     * -----------------------------
     * 1. **Process delayed spike events** (deliver synaptic input)
     * 2. **Update neuron membrane potentials** (LIF dynamics)
     * 3. **Detect spikes and schedule events** (spike generation)
     * 4. **Update working memory** (active trace maintenance)
     * 5. **Apply neuromodulation** (excitability modulation)
     * 6. **Apply plasticity rules** (STDP, Hebbian)
     * 7. **Update episodic memory** (experience storage)
     * 8. **Update prediction system** (forward model updates)
     * 9. **Update attention system** (resource allocation)
     * 10. **Update concept formation** (pattern abstraction)
     * 11. **Apply structural plasticity** (network topology changes)
     * 12. **Replay important memories** (memory consolidation)
     * 13. **Apply development effects** (learning trajectory)
     * 14. **Periodic memory consolidation** (episodic memory)
     * 15. **Checkpoint management** (state persistence)
     * 
     * **Parameters**
     * -------------
     * currentStep: Zero-based simulation step
     * currentTime: Current simulation time in milliseconds
     * 
     * **Event-Driven Processing**
     * --------------------------
     * - Spike generation queues events for delivery
     * - Delayed spike events maintain spike transmission timing
     * - All processing happens atomically per step
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    /**
     * 
     * **Sensory Input**
     * ================
     * 
     * **receiveSensoryInput(const SensoryInput& input)**
     * ----------------------------------------------------
     * 
     * Processes external/internal sensory input and injects into neurons.
     * 
     * **Sensory Encoding**
     * -------------------
     * - Input data normalized to [-10, 10] mV range
     * - Distributed across sensory neuron population
     * - Weighted by neuron importance/activity
     * 
     * **Parameters**
     * -------------
     * input: Sensory input containing data vector
     * 
     * **Processing**
     * -------------
     * 1. Normalize input values
     * 2. Distribute to sensory neurons
     * 3. Store in working memory (if above threshold)
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    /**
     * 
     * **Direct Current Injection**
     * ===========================
     * 
     * **injectCurrent(NeuronId neuron, MembranePotential current)**
     * -------------------------------------------------------------
     * 
     * Injects current directly into a specific neuron.
     * 
     * **Use Cases**
     * -----------
     * - Sensory input encoding (bypassing synapses)
     * - Motor command generation
     * - Experimental stimulation
     * - Test input patterns
     * 
     * **Parameters**
     * -------------
     * neuron: Target neuron identifier
     * current: Current in nA (converted by neuron)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    /**
     * 
     * **Bulk Current Injection**
     * =========================
     * 
     * **injectCurrentToNeurons(NeuronType type, MembranePotential current)**
     * -------------------------------------------------------------------
     * 
     * Injects current into all neurons of a given type.
     * 
     * **Neuron Types**
     * ---------------
     * - SENSORY: Input encoding neurons
     * - INTERNAL: Processing/storage neurons
     * - MOTOR: Output generation neurons
     * 
     * **Parameters**
     * -------------
     * type: Neuron type to target
     * current: Current per neuron in nA
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    /**
     * 
     * **Spike System**
     * ===============
     * 
     * The spike system handles event-driven neural processing:
     * - Real-time spike detection
     * - Delayed spike delivery
     * - Plasticity event triggering
     * - Statistics collection
     * 
     * **getSpikeSystem()** / **getSpikeSystem() const**
     * ----------------------------------------------------
     * 
     * Gets the spike system pointer for direct access.
     * 
     * **Returns**
     * -------
     * SpikeSystem*: Mutable or const spike system
     * 
     * **Plasticity System Access**
     * --------------------------
     * Similar access methods for other plasticity systems:
     * - STDP (Spike-Timing-Dependent Plasticity)
     * - Hebbian (co-activation learning)
     * - StructuralPlasticity (network topology changes)
     */
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    /**
     * 
     * **Brain Statistics**
     * ===================
     * 
     * **getExcitationInhibitionRatio() const**
     * ----------------------------------------
     * 
     * Gets the excitation/inhibition balance.
     * 
     * **Returns**
     * -------
     * float: E/I ratio (positive = excitation dominant)
     * - > 0: Excitatory connectivity dominates
     * - == 0: Balanced excitation/inhibition
     * - < 0: Inhibitory connectivity dominates
     * 
     * **getTotalSpikeCount() const**
     * -----------------------------
     * 
     * Gets total spike count since initialization.
     * 
     * **Returns**
     * -------
     * size_t: Cumulative spike count
     */
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    /**
     * 
     * **Action Selection**
     * ===================
     * 
     * **produceAction()**
     * -----------------------
     * 
     * Selects and produces an action based on motor neuron activity.
     * 
     * **Selection Algorithm**
     * ----------------------
     * 1. Check if motor neurons exist
     * 2. Count firing motor neurons
     * 3. Select action based on activity:
     *   - Any firing motor: MoveForward
     *   - No firing motor: Wait
     * 
     * **Returns**
     * -------
     * std::unique_ptr<Action>: Selected action
     * 
     * **Action Types**
     * --------------
     * - MoveForward: Progress toward goal
     * - Wait: No movement (idle)
     */
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    /**
     * 
     * **Neuromodulation**
     * ===================
     * 
     * **applyNeuromodulation(const Neuromodulator& signal)**
     * ---------------------------------------------------------
     * 
     * Applies neuromodulatory signal effects on plasticity.
     * 
     * **Neuromodulators**
     * ------------------
     * - Dopamine: Reward prediction error
     * - Acetylcholine: Attention modulation
     * - Serotonin: Mood and motivation
     * - Norepinephrine: Arousal and vigilance
     * 
     * **Plasticity Effects**
     * ---------------------
     * - Scales STDP learning rates
     * - Modifies synaptic eligibility traces
     * - Gates learning based on behavioral relevance
     * 
     * **Parameters**
     * -------------
     * signal: Neuromodulator containing level and type
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    /**
     * 
     * **Plasticity Update**
     * ====================
     * 
     * **updatePlasticity()**
     * ------------------------
     * 
     * Updates plasticity rules (for API compatibility).
     * 
     * **Note**
     * -----
     * Plasticity is now applied automatically during each simulation step.
     * This method is kept for backward compatibility.
     */
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    /**
     * 
     * **Development**
     * =============
     * 
     * **develop()**
     * ----------
     * 
     * Applies developmental changes to neural circuitry.
     * 
     * **Developmental Stages**
     * ----------------------
     * - **Initial**: High plasticity, rapid learning
     * - **CriticalPeriod**: Reduced but ongoing plasticity
     * - **Maturation**: Activity-dependent refinement
     * *Adult*: Stable, optimized network
     * 
     * **Effects**
     * ---------
     * - Synaptogenesis: Creates new connections
     * - Pruning: Eliminates weak connections
     * - Myelination: Increases conduction speed
     * - Receptor changes: Modifies synaptic efficacy
     */
    void develop();
    
    // Reset brain state
    /**
     * 
     * **Brain Reset**
     * =============
     * 
     * **reset()**
     * --------
     * 
     * Resets the brain to its initial state.
     * 
     * **Reset Actions**
     * -----------------
     * - Clears all neural regions
     * - Resets simulation clock
     * - Clears spike statistics
     * - Resets memory systems
     * - Resets neuromodulation
     * - Resets developmental stage
     * - Logs reset completion
     * 
     * **Use Cases**
     * -----------
     * - Episode boundary reset
     * - Experiment restart
     * - Error recovery
     * - State restoration
     */
    void reset();
    
    // Save brain state to file (checkpointing)
    /**
     * 
     * **State Persistence**
     * ====================
     * 
     * **save(const std::string& filepath) const**
     * ----------------------------------------
     * 
     * Saves brain state to a checkpoint file.
     * 
     * **Checkpoint Format**
     * --------------------
     * - Binary format for efficiency
     * - Compressed for storage
     * - Includes metadata:
     *   - Neuron count
     *   - Synapse count
     *   - Region count
     *   - Current step
     *   - Current time
     * 
     * **Contents Saved**
     * -----------------
     * - Neuron states (membrane potential, threshold, etc.)
     * - Synapse states (weight, delay, type)
     * - Plasticity traces
     * - Memory system states
     * - Neuromodulation levels
     * 
     * **Returns**
     * -------
     * bool: true if save successful, false on errors
     * 
     * **Parameters**
     * -------------
     * filepath: Output file path
     * 
     * **Compression**
     * --------------
     * Uses balanced compression (CPU/memory trade-off)
     */
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    /**
     * 
     * **State Restoration**
     * ====================
     * 
     * **load(const std::string& filepath)**
     * ----------------------------------
     * 
     * Loads brain state from a checkpoint file.
     * 
     * **Validation**
     * -------------
     * - File existence and accessibility
     * - Format compatibility
     * - State consistency
     * - Version matching
     * 
     * **Restoration Process**
     * ----------------------
     * 1. Open and validate checkpoint file
     * 2. Read neuron states
     * 3. Read synapse states
     * 4. Apply states to neural structures
     * 5. Update system states
     * 6. Validate restoration
     * 
     * **Returns**
     * -------
     * bool: true if load successful, false on errors
     * 
     * **Parameters**
     * -------------
     * filepath: Input file path
     */
    bool load(const std::string& filepath);
    
    // Region management
    /**
     * 
     * **Region Management**
     * ====================
     * 
     * **addRegion(const std::string& name = "")**
     * -----------------------------------------
     * 
     * Creates a new brain region.
     * 
     * **Parameters**
     * -------------
     * name: Region name (optional, auto-generated if empty)
     * 
     * **Returns**
     * -------
     * RegionId: New region identifier
     */
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    /**
     * 
     * **Region Access**
     * ================
     * 
     * **getRegions() const**
     * ---------------------
     * 
     * Gets all brain regions.
     * 
     * **Returns**
     * -------
     * const std::vector<std::unique_ptr<NeuralRegion>>&: All regions
     * 
     * **Use Cases**
     * -----------
     * - Region-wide operations
     * - Statistics aggregation
     * - Parallel processing
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    /**
     * 
     * **Long-Range Connectivity**
     * ===========================
     * 
     * **addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1)**
     * --------------------------------------------------------------------------------------------------------------------
     * 
     * Creates a connection between brain regions.
     * 
     * **Parameters**
     * -------------
     * source: Source region identifier
     * target: Target region identifier
     * weight: Connection strength (default: 0.0f)
     * delay: Transmission delay in steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    /**
     * 
     * **Global Statistics**
     * ====================
     * 
     * **getTotalNeuronCount() const**
     * -------------------------------
     * 
     * Gets total neuron count.
     * 
     * **Returns**
     * -------
     * size_t: Total neurons in all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * 
     * **getTotalSynapseCount() const**
     * -------------------------------
     * 
     * Gets total synapse count.
     * 
     * **Returns**
     * -------
     * size_t: Total synapses (local + inter-region)
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * 
     * **getActiveNeuronCount() const**
     * -------------------------------
     * 
     * Gets number of active neurons.
     * 
     * **Returns**
     * -------
     * size_t: Neurons with membrane potential != resting
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * 
     * **getFiringNeuronCount() const**
     * ------------------------------
     * 
     * Gets number of firing neurons this step.
     * 
     * **Returns**
     * -------
     * size_t: Neurons that fired in current step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * 
     * **getAverageFiringRate() const**
     * ------------------------------
     * 
     * Gets average firing rate across all neurons.
     * 
     * **Returns**
     * -------
     * float: Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    /**
     * 
     * **Memory System Accessors**
     * ===========================
     * 
     * **getWorkingMemory()**
     * ---------------------
     * 
     * Gets working memory system.
     * 
     * **Returns**
     * -------
     * NeuralWorkingMemory*: Working memory pointer
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    /**
     * 
     * **getEpisodicMemory()**
     * ----------------------
     * 
     * Gets episodic memory system.
     * 
     * **Returns**
     * -------
     * NeuralEpisodicMemory*: Episodic memory pointer
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    /**
     * 
     * **getAssociativeMemory()**
     * --------------------------
     * 
     * Gets associative memory system.
     * 
     * **Returns**
     * -------
     * NeuralAssociativeMemory*: Associative memory pointer
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    /**
     * 
     * **getPredictionSystem()**
     * -------------------------
     * 
     * Gets prediction system.
     * 
     * **Returns**
     * -------
     * PredictionSystem*: Prediction system pointer
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    /**
     * 
     * **getPlanner()**
     * --------------
     * 
     * Gets neural planner.
     * 
     * **Returns**
     * -------
     * NeuralPlanner*: Neural planner pointer
     */
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    /**
     * 
     * **getConceptFormation()**
     * ------------------------
     * 
     * Gets concept formation system.
     * 
     * **Returns**
     * -------
     * ConceptFormation*: Concept formation pointer
     */
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    /**
     * 
     * **getAttention()**
     * -----------------
     * 
     * Gets attentional selection system.
     * 
     * **Returns**
     * -------
     * AttentionalSelection*: Attention system pointer
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * 
     * **getDevelopmentSystem()**
     * ------------------------
     * 
     * Gets development system.
     * 
     * **Returns**
     * -------
     * DevelopmentSystem*: Development system pointer
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * 
     * **getDevelopmentalStage() const**
     * --------------------------------
     * 
     * Gets current developmental stage.
     * 
     * **Returns**
     * -------
     * DevelopmentalStage: Current stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * 
     * **setDevelopmentalStage(DevelopmentalStage stage)**
     * ----------------------------------------------------
     * 
     * Sets developmental stage.
     * 
     * **Parameters**
     * -------------
     * stage: Target developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    /**
     * 
     * **getDopamine()**
     * ----------------
     * 
     * Gets dopamine system.
     * 
     * **Returns**
     * -------
     * Dopamine*: Dopamine pointer
     */
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    /**
     * 
     * **getCuriosity()**
     * ----------------
     * 
     * Gets curiosity system.
     * 
     * **Returns**
     * -------
     * Curiosity*: Curiosity pointer
     */
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    /**
     * 
     * **getNovelty()**
     * --------------
     * 
     * Gets novelty detection system.
     * 
     * **Returns**
     * -------
     * Novelty*: Novelty pointer
     */
    Novelty* getNovelty();
    
    // Prediction error signal
    /**
     * 
     * **getPredictionErrorSignal()**
     * -------------------------------
     * 
     * Gets prediction error system.
     * 
     * **Returns**
     * -------
     * PredictionError*: Prediction error pointer
     */
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    /**
     * 
     * **getConfig() const**
     * --------------------
     * 
     * Gets configuration.
     * 
     * **Returns**
     * -------
     * std::shared_ptr<const Config>: Configuration pointer
     */
class DefaultBrainComponentFactory : public BrainComponentFactory {
public:
    std::unique_ptr<Neuron> createNeuron(NeuronId id) override {
        return std::make_unique<Neuron>(id);
    }
    
    std::unique_ptr<Synapse> createSynapse() override {
        return std::make_unique<Synapse>();
    }
    
    std::unique_ptr<NeuralRegion> createRegion(const std::string& name = "") override {
        return std::make_unique<NeuralRegion>(name);
    }
    
    std::unique_ptr<NeuralPopulation> createPopulation() override {
        return std::make_unique<NeuralPopulation>();
    }
};

// Concrete implementation of brain architecture
class DefaultBrainArchitecture : public IBrainArchitecture {
private:
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    std::unordered_map<RegionId, std::vector<InterRegionConnection>> interRegionConnections_;
    
public:
    void initialize() override {
        regions_.clear();
        interRegionConnections_.clear();
    }
    
    void step(SimulationStep step) override {
        // Implementation for stepping regions
    }
    
    void reset() override {
        regions_.clear();
        interRegionConnections_.clear();
    }
    
    void addRegion(std::unique_ptr<NeuralRegion> region) override {
        regions_.push_back(std::move(region));
    }
    
    NeuralRegion* getRegion(RegionId id) override {
        for (auto& region : regions_) {
            if (region->getId() == id) return region.get();
        }
        return nullptr;
    }
    
    const NeuralRegion* getRegion(RegionId id) const override {
        for (const auto& region : regions_) {
            if (region->getId() == id) return region.get();
        }
        return nullptr;
    }
    
    size_t getRegionCount() const override {
        return regions_.size();
    }
    
    void connectRegions(RegionId source, RegionId target, float weight = 0.0f) override {
        interRegionConnections_[source].push_back(InterRegionConnection(source, target, weight));
    }
};

// Concrete implementation of brain
class DefaultBrainImplementation : public IBrainImplementation {
private:
    std::unique_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    std::unique_ptr<SimulationClock> clock_;
    std::unique_ptr<Logger> logger_;
    
    // Integrated systems
    std::unique_ptr<IBrainArchitecture> architecture_;
    std::unique_ptr<ISpikeSystem> spikeSystem_;
    std::unique_ptr<IPlasticitySystem> plasticitySystem_;
    std::unique_ptr<INeuromodulationSystem> neuromodulationSystem_;
    std::unique_ptr<IMemorySystem> memorySystem_;
    std::unique_ptr<IDevelopmentSystem> developmentSystem_;
    std::unique_ptr<ISensorySystem> sensorySystem_;
    std::unique_ptr<IMotorSystem> motorSystem_;
    std::unique_ptr<IPredictionSystem> predictionSystem_;
    std::unique_ptr<ICognitionSystem> cognitionSystem_;
    std::unique_ptr<IEnvironment> environment_;
    
    std::vector<std::unique_ptr<NeuralRegion>> regions_;
    std::vector<InterRegionConnection> interRegionConnections_;
    
    size_t totalSpikes_ = 0;
    SimulationStep currentStep_ = 0;
    Timestamp currentTime_ = 0;
    
public:
    DefaultBrainImplementation(std::unique_ptr<Config> config)
        : config_(std::move(config)), currentStep_(0), currentTime_(0) {
        initializeComponents();
    }
    
    ~DefaultBrainImplementation() override = default;
    
    void initialize(const Config& config) override {
        config_ = std::make_unique<Config>(config);
        currentStep_ = 0;
        currentTime_ = 0;
        totalSpikes_ = 0;
        
        // Initialize all integrated systems
        architecture_ = std::make_unique<DefaultBrainArchitecture>();
        
        // Initialize other systems...
        architecture_->initialize();
        logger_->log("Brain initialized successfully");
    }
    
    void step(SimulationStep step, Timestamp time = 0) override {
        currentStep_ = step;
        currentTime_ = time;
        
        // Process all integrated systems
        spikeSystem_->update(step, time);
        plasticitySystem_->update(step, time);
        neuromodulationSystem_->update(step);
        memorySystem_->update(step);
        developmentSystem_->update(step);
        sensorySystem_->update(step);
        predictionSystem_->update(step);
        cognitionSystem_->update(step);
        architecture_->step(step);
        
        totalSpikes_ += spikeSystem_->getTotalSpikeCount();
    }
    
    void reset() override {
        // Reset all systems
        architecture_->reset();
        spikeSystem_->reset();
        plasticitySystem_->reset();
        neuromodulationSystem_->reset();
        memorySystem_->reset();
        developmentSystem_->reset();
        
        regions_.clear();
        interRegionConnections_.clear();
        currentStep_ = 0;
        currentTime_ = 0;
        totalSpikes_ = 0;
    }
    
    float getExcitationInhibitionRatio() const override {
        return spikeSystem_->getExcitationInhibitionRatio();
    }
    
    size_t getTotalNeuronCount() const override {
        size_t count = 0;
        for (const auto& region : regions_) {
            count += region->getPopulationCount();
        }
        return count;
    }
    
    size_t getTotalSynapseCount() const override {
        size_t count = 0;
        for (const auto& region : regions_) {
            count += region->getTotalSynapseCount();
        }
        return count + interRegionConnections_.size();
    }
    
    size_t getActiveNeuronCount() const override {
        return spikeSystem_->getActiveNeuronCount();
    }
    
    size_t getFiringNeuronCount() const override {
        return spikeSystem_->getFiringNeuronCount();
    }
    
    float getAverageFiringRate() const override {
        return spikeSystem_->getAverageFiringRate();
    }
    
    // Additional methods for brain operations
    void addRegion(std::unique_ptr<NeuralRegion> region) {
        regions_.push_back(std::move(region));
    }
    
    NeuralRegion* getRegion(RegionId id) {
        for (auto& region : regions_) {
            if (region->getId() == id) return region.get();
        }
        return nullptr;
    }
    
    const NeuralRegion* getRegion(RegionId id) const {
        for (const auto& region : regions_) {
            if (region->getId() == id) return region.get();
        }
        return nullptr;
    }
    
    size_t getRegionCount() const {
        return regions_.size();
    }
    
    void connectRegions(RegionId source, RegionId target, float weight = 0.0f) {
        interRegionConnections_.push_back(InterRegionConnection(source, target, weight));
    }
    
    // Brain-specific methods (keeping original interface for compatibility)
    bool initialize() {
        if (!config_) {
            logger_->error("Cannot initialize brain: configuration not provided");
            return false;
        }
        
        initializeComponents();
        return true;
    }
    
    void step(SimulationStep currentStep) {
        step(currentStep, static_cast<Timestamp>(currentStep * 1000));
    }
    
    void receiveSensoryInput(const SensoryInput& input) {
        sensorySystem_->processInput(input);
    }
    
    void injectCurrent(NeuronId neuron, MembranePotential current) {
        // Implementation for direct current injection
    }
    
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) {
        // Implementation for bulk current injection
    }
    
    SpikeSystem* getSpikeSystem() {
        return spikeSystem_.get();
    }
    
    const SpikeSystem* getSpikeSystem() const {
        return spikeSystem_.get();
    }
    
    STDP* getSTDP() {
        return plasticitySystem_->getSTDP();
    }
    
    Hebbian* getHebbian() {
        return plasticitySystem_->getHebbian();
    }
    
    StructuralPlasticity* getStructuralPlasticity() {
        return plasticitySystem_->getStructuralPlasticity();
    }
    
    // ... other getter methods
    
private:
    void initializeComponents() {
        // Initialize core infrastructure
        config_ = std::make_unique<Config>(*config_);
        rng_ = std::make_unique<RandomGenerator>();
        clock_ = std::make_unique<SimulationClock>();
        logger_ = std::make_unique<Logger>();
        
        // Initialize integrated systems
        architecture_ = std::make_unique<DefaultBrainArchitecture>();
        spikeSystem_ = std::make_unique<SpikeSystem>();
        plasticitySystem_ = std::make_unique<PlasticitySystem>();
        neuromodulationSystem_ = std::make_unique<NeuromodulationSystem>();
        memorySystem_ = std::make_unique<MemorySystem>();
        developmentSystem_ = std::make_unique<DevelopmentSystem>();
        sensorySystem_ = std::make_unique<SensorySystem>();
        motorSystem_ = std::make_unique<MotorSystem>();
        predictionSystem_ = std::make_unique<PredictionSystem>();
        cognitionSystem_ = std::make_unique<CognitionSystem>();
        environment_ = std::make_unique<Environment>();
        
        logger_->log("All brain components initialized");
    }
};

// Default implementations of auxiliary interfaces
class DefaultNeuromodulationSystem : public INeuromodulationSystem {
public:
    void applySignal(const Neuromodulator& signal) override {
        // Implementation
    }
    
    void update(SimulationStep step) override {
        // Implementation
    }
    
    void reset() override {
        // Implementation
    }
    
    float getModulationLevel(const std::string& type) const override {
        return 0.0f;
    }
};

class DefaultMemorySystem : public IMemorySystem {
public:
    void update(SimulationStep step) override {
        // Implementation
    }
    
    void reset() override {
        // Implementation
    }
    
    void store(const MemoryItem& item) override {
        // Implementation
    }
    
    MemoryItem retrieve(const MemoryId& id) override {
        return MemoryItem();
    }
    
    std::vector<MemoryId> getRecentMemories() const override {
        return std::vector<MemoryId>();
    }
};

class DefaultPlasticitySystem : public IPlasticitySystem {
public:
    void update(SimulationStep step, Timestamp time) override {
        // Implementation
    }
    
    void reset() override {
        // Implementation
    }
    
    void applyPlasticity() override {
        // Implementation
    }
    
    void configurePlasticity(const PlasticityConfig& config) override {
        // Implementation
    }
    
    STDP* getSTDP() {
        return nullptr;
    }
    
    Hebbian* getHebbian() {
        return nullptr;
    }
    
    StructuralPlasticity* getStructuralPlasticity() {
        return nullptr;
    }
};

class DefaultDevelopmentSystem : public IDevelopmentSystem {
public:
    void update(SimulationStep step) override {
        // Implementation
    }
    
    void reset() override {
        // Implementation
    }
    
    DevelopmentalStage getCurrentStage() const override {
        return DevelopmentalStage::Adult;
    }
    
    void setStage(DevelopmentalStage stage) override {
        // Implementation
    }
};

class DefaultSensorySystem : public ISensorySystem {
public:
    void processInput(const SensoryInput& input) override {
        // Implementation
    }
    
    std::vector<float> getSensoryActivity() const override {
        return std::vector<float>();
    }
    
    void reset() override {
        // Implementation
    }
};

class DefaultMotorSystem : public IMotorSystem {
public:
    std::unique_ptr<Action> produceAction() override {
        // Implementation
        return std::make_unique<Action>();
    }
    
    void update(SimulationStep step) override {
        // Implementation
    }
    
    void reset() override {
        // Implementation
    }
};

class DefaultPredictionSystem : public IPredictionSystem {
public:
    void update(SimulationStep step) override {
        // Implementation
    }
    
    void reset() override {
        // Implementation
    }
    
    float getPredictionError() const override {
        return 0.0f;
    }
    
    void setPrediction(const std::vector<float>& prediction) override {
        // Implementation
    }
};

class DefaultCognitionSystem : public ICognitionSystem {
public:
    std::unique_ptr<Action> planAction() override {
        // Implementation
        return std::make_unique<Action>();
    }
    
    void update(SimulationStep step) override {
        // Implementation
    }
    
    void reset() override {
        // Implementation
    }
    
    std::vector<float> getAttentionWeights() const override {
        return std::vector<float>();
    }
};

class DefaultEnvironment : public IEnvironment {
public:
    void update(SimulationStep step) override {
        // Implementation
    }
    
    SensoryInput getObservation() const override {
        return SensoryInput();
    }
    
    void applyAction(const Action& action) override {
        // Implementation
    }
    
    float getReward() const override {
        return 0.0f;
    }
    
    bool isDone() const override {
        return false;
    }
    
    void reset() override {
        // Implementation
    }
};

// Original Brain class (pImpl implementation)
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
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
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
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    void addInterRegionConnection(RegionId source, RegionId target, 
                                 float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Core utilities
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    SimulationClock* getSimulationClock();
    Logger* getLogger();
    
    // Status logging
    void logStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // BRAIN_HPP
    
    private:
    struct Impl;
    neuron_impl_ptr pImpl;
};

} // namespace nlm

#endif // BRAIN_HPP

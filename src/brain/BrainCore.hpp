#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations for integrated systems
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Forward declarations for other brain classes
class Brain;
class SensoryInput;
class Action;
class Neuromodulator;

class BrainCore {
public:
    /**
     * @brief Construct a new BrainCore object
     * 
     * Initializes the brain core with a configuration object and sets up internal
     * data structures. The actual initialization of brain regions and neurons
     * is performed later in the initialize() method.
     * 
     * @param config Shared pointer to the configuration object containing brain
     *               parameters such as neuron count, region count, connection
     *               probability, and various system settings
     */
    BrainCore(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the BrainCore object
     * 
     * Cleans up all internal resources and performs finalization of the brain
     * core components. This includes freeing memory allocated for regions,
     * connections, and all integrated systems (memory, prediction, cognition,
     * development, neuromodulation, plasticity, and spike processing).
     */
    ~BrainCore();
    
    // Disable copying, enable moving
    BrainCore(const BrainCore&) = delete;
    BrainCore& operator=(const BrainCore&) = delete;
    BrainCore(BrainCore&&) noexcept;
    BrainCore& operator=(BrainCore&&) noexcept;
    
    // Initialize brain with configuration
    /**
     * @brief Initializes the brain core with configuration settings
     * 
     * This method performs the complete initialization of the brain core by:
     * 1. Creating brain regions based on configuration
     * 2. Populating regions with neurons (sensory, internal, motor populations)
     * 3. Establishing random synaptic connectivity within regions
     * 4. Initializing all integrated systems (memory, prediction, cognition,
     *    development, neuromodulation, plasticity, spike processing)
     * 5. Setting up event handlers and checkpoint managers
     * 6. Registering spike handlers for event-driven synaptic input delivery
     * 
     * The method reads configuration parameters including neuron count,
     * region count, connection probability, and various system settings.
     * After initialization, the brain core is ready to process simulation steps.
     * 
     * @return bool true if initialization succeeds, false otherwise
     *             Returns false if configuration is invalid or initialization fails
     */
    bool initialize();
    
    // Region management
    /**
     * @brief Creates a new neural region with the specified name
     * 
     * Creates a new neural region and adds it to the brain core's region collection.
     * Each region contains neurons organized into populations (sensory, internal, motor).
     * If no name is provided, the region will be named "Region_N" where N is the index.
     * 
     * @param name Optional name for the region. If empty, auto-generates "Region_N"
     * @return RegionId Unique identifier for the created region
     */
    RegionId addRegion(const std::string& name = "");
    /**
     * @brief Retrieves a neural region by its ID
     * 
     * Searches for a neural region using the provided RegionId and returns a pointer
     * to the region if found. Uses a hash map for O(1) lookup performance.
     * 
     * @param id The unique identifier of the region to retrieve
     * @return NeuralRegion* Pointer to the region if found, nullptr otherwise
     */
    NeuralRegion* getRegion(RegionId id);
    /**
     * @brief Retrieves a const neural region by its ID
     * 
     * Thread-safe version of getRegion that returns a const pointer to the neural region.
     * Used when accessing regions in read-only contexts or from const methods.
     * 
     * @param id The unique identifier of the region to retrieve
     * @return const NeuralRegion* Const pointer to the region if found, nullptr otherwise
     */
    const NeuralRegion* getRegion(RegionId id) const;
    /**
     * @brief Gets the total number of regions in the brain core
     * 
     * @return size_t Number of regions currently in the brain core
     */
    size_t getRegionCount() const;
    /**
     * @brief Retrieves all region IDs in the brain core
     * 
     * Returns a vector containing all RegionId values for regions that have been
     * added to the brain core. The order corresponds to the internal storage order.
     * 
     * @return std::vector<RegionId> Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Basic statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== CORE SYSTEM ACCESSORS ==========
    
    // Configuration and random generator
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    const RandomGenerator* getRandomGenerator() const;
    
    // Spike system
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system
    STDP* getSTDP();
    const STDP* getSTDP() const;
    Hebbian* getHebbian();
    const Hebbian* getHebbian() const;
    StructuralPlasticity* getStructuralPlasticity();
    const StructuralPlasticity* getStructuralPlasticity() const;
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== BASIC INTEGRATION ACCESSORS ==========
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    const PredictionSystem* getPredictionSystem() const;
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    const NeuralPlanner* getPlanner() const;
    ConceptFormation* getConceptFormation();
    const ConceptFormation* getConceptFormation() const;
    AttentionalSelection* getAttention();
    const AttentionalSelection* getAttention() const;
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    const Dopamine* getDopamine() const;
    Curiosity* getCuriosity();
    const Curiosity* getCuriosity() const;
    Novelty* getNovelty();
    const Novelty* getNovelty() const;
    PredictionError* getPredictionErrorSignal();
    const PredictionError* getPredictionErrorSignal() const;
    
    // Simulation parameters
    TimestepDuration getTimestep() const;
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    
    // Integration state
    bool isResting() const;
    size_t getStepsSinceLastEpisode() const;
    double getDevelopmentalAge() const;
    
    // Reset functionality
    /**
     * @brief Resets the entire brain core to its initial state
     * 
     * Performs a complete reset of all brain state variables and neural components:
     * - Resets simulation counters (step, time, spike counts)
     * - Resets developmental tracking variables
     * - Clears all neural regions and their neurons/synapses
     * - Resets the spike system
     * - Clears all memory systems (working, episodic, associative memory)
     * - Resets attentional selection system
     * 
     * This method is typically called at the beginning of a new episode or when
     * restarting the simulation. After reset, the brain core can be re-initialized
     * and used for a fresh simulation run.
     */
    void reset();
    
    // Sensory neuron access for input injection
    const std::vector<Neuron*>& getSensoryNeurons() const;
    const std::vector<Neuron*>& getMotorNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

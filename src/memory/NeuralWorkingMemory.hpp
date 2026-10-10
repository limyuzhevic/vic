#pragma once

/**
 * @file NeuralWorkingMemory.hpp
 * @brief Working memory implementation using neural dynamics
 * 
 * This header defines the NeuralWorkingMemory class for the brain's working memory system.
 * The neural working memory implements working memory using persistent neural activity
 * and recurrent connections, rather than simple storage vectors. It provides the
 * foundation for maintaining active information and supporting cognitive processes
 * that require temporary information retention.
 * 
 * Working Memory Theory:
 * - Working memory uses persistent neural activity to maintain information
 * - Recurrent connections provide self-sustaining activity patterns
 * - Activity-dependent short-term plasticity enables dynamic regulation
 * - Neural competition selects which information is actively maintained
 * - Integration with attention controls information selection and maintenance
 * 
 * Working Memory Functions:
 * - Information storage: Encodes and maintains neural activity patterns
 * - Active retrieval: Provides continuous access to stored information
 * - Dynamic updating: Modifies stored information based on new input
 * - Competition management: Resolves conflicts between multiple memory traces
 * - Decay regulation: Controls temporal forgetting and refresh cycles
 * 
 * Integration Points:
 * - Connected to attention system for information selection
 * - Interacts with episodic memory for memory consolidation
 * - Modulated by neuromodulation systems (acetylcholine, norepinephrine)
 * - Works with planning system for goal maintenance
 * - Coordinates with development system for age-appropriate capacity
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

// Forward declarations
class NeuralWorkingMemory;
class AttentionalSelection;
class DevelopmentSystem;

/**
 * @brief Working memory using neural dynamics and persistent activity
 * 
 * Implements the brain's working memory using neural dynamics rather than
 * simple storage vectors. The neural working memory uses persistent neural
 * activity, recurrent synaptic connections, and neural competition to maintain
 * information over short time scales. This provides a biologically realistic
 * model of working memory that supports dynamic information maintenance and
 * cognitive processing.
 * 
 * Neural Working Memory Architecture:
 * 1. Encoding: Information is transformed into neural activity patterns
 * 2. Maintenance: Recurrent connections sustain activity (reverberation)
 * 3. Competition: Neural populations compete for active representation
 * 4. Updating: New input can update or replace existing representations
 * 5. Decay: Activity naturally decays over time (temporal forgetting)
 * 
 * Working Memory Dynamics:
 * - Persistent activity: Self-sustaining neural representations
 * - Recurrent connectivity: Provides positive feedback for maintenance
 * - Activity-dependent plasticity: Modifies connections based on usage
 * - Population coding: Information encoded across neural populations
 * - Dynamic regulation: Continuously adapts to changing information needs
 * 
 * Thread Safety: NeuralWorkingMemory is thread-safe for concurrent access
 * from different brain components. Memory operations are protected
 * with mutexes.
 * 
 * Memory Management: NeuralWorkingMemory uses pImpl pattern with pointer storage
 * for efficient memory usage and to reduce compilation dependencies.
 * 
 * @note NeuralWorkingMemory is essential for cognitive processes that require
 *       temporary information maintenance, such as planning, reasoning, and
 *       language comprehension. It works in conjunction with long-term memory
 *       systems for comprehensive memory management.
 * @see AttentionalSelection for attention-based information selection
 * @see DevelopmentSystem for developmental aspects of working memory
 * @see EpisodicMemory for memory consolidation
 */
class NeuralWorkingMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a neural working memory system with initial state. The neural
     * working memory implementation uses the pImpl pattern for efficient memory usage
     * and to reduce compilation dependencies.
     */
    NeuralWorkingMemory();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of NeuralWorkingMemory's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~NeuralWorkingMemory();
    
    /**
     * @brief Initialize neural working memory with brain reference
     * 
     * Initializes the neural working memory with reference to the brain,
     * enabling access to brain's attentional system, development system,
     * and other components for context-aware working memory operation.
     * 
     * @param brain Pointer to the brain that contains this working memory
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Store information in working memory
     * 
     * Stores information in working memory by encoding it as a neural activity
     * pattern. The information is maintained through persistent neural activity
     * and recurrent connections.
     * 
     * Storage Algorithm:
     * 1. Acquire thread mutex for safe concurrent access
     * 2. Encode information into neural activity pattern
     * 3. Create recurrent connections for maintenance
     * 4. Add to active memory traces
     * 5. Apply competition for limited capacity
     * 6. Store timestamps and strength information
     * 
     * @param pattern Neural activity pattern to store
     * @param strength Initial strength of memory trace (0.0-1.0)
     * 
     * @note Thread-safe with mutex protection.
     *       Information is encoded as neural activity patterns that are
     *       maintained through recurrent connections and persistent activity.
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    /**
     * @brief Store specific neuron activation
     * 
     * Stores activation levels for specific neurons in working memory.
     * This allows for fine-grained control over which neurons are active
     * and their activation levels.
     * 
     * @param neuron Neuron ID to store activation for
     * @param activation Activation level for neuron (0.0-1.0)
     */
    void storeToNeuron(NeuronId neuron, float activation);
    
    /**
     * @brief Retrieve current working memory content as activity levels
     * 
     * Returns the current working memory content as neural activity levels.
     * This represents the active representation currently maintained
     * in working memory.
     * 
     * @return Vector of neural activity levels representing current content
     * 
     * @note Returns the winning population's activation pattern, representing
     *       the information currently being actively maintained.
     */
    std::vector<float> retrieve() const;
    
    /**
     * @brief Check if specific neuron is part of working memory
     * 
     * Determines whether a specific neuron is currently part of the active
     * working memory representation. This is used for accessing and modifying
     * individual neuron contributions to working memory.
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron is part of working memory, false otherwise
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * @brief Get activation level of a specific neuron in working memory
     * 
     * Returns the activation level of a specific neuron within the current
     * working memory representation. This allows access to individual
     * neuron contributions.
     * 
     * @param neuron Neuron ID to get activation for
     * @return Activation level of neuron (0.0-1.0)
     */
    float getNeuronActivation(NeuronId neuron) const;
    
    /**
     * @brief Update working memory (maintenance and decay)
     * 
     * Updates the working memory system by maintaining active traces,
     * applying decay, and updating recurrent connections. This is called
     * periodically to simulate the dynamic nature of working memory.
     * 
     * Update Algorithm:
     * 1. Acquire thread mutex for safe concurrent access
     * 2. Update recurrent connections for maintenance
     * 3. Decay weak memory traces based on decay rate
     * 4. Run competition between remaining traces
     * 5. Apply attention modulation if available
     * 6. Update activity levels and timestamps
     * 
     * @param dt Time step duration (simulation time units)
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Clear working memory
     * 
     * Clears all working memory content, removing all active memory traces
     * and resetting the system to empty state. This is used to reset
     * working memory or when information should be discarded.
     * 
     * @note Thread-safe with mutex protection.
     *       This operation resets the entire working memory system.
     */
    void clear();
    
    /**
     * @brief Get number of active memory traces
     * 
     * Returns the number of active memory traces currently maintained
     * in working memory. This reflects the current information load
     * and capacity utilization.
     * 
     * @return Number of active memory traces
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    /**
     * @brief Get capacity
     * 
     * Returns the current capacity of working memory (number of neurons
     * or traces that can be actively maintained). This may be developmental
     * and change over time.
     * 
     * @return Working memory capacity
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * @brief Set capacity
     * 
     * Sets the capacity of working memory. This may be developmental
     * and can change based on age or training.
     * 
     * @param cap New capacity
     */
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    /**
     * @brief Get decay rate for memory traces
     * 
     * Returns the current decay rate for memory traces. Higher decay rates
     * mean information is forgotten more quickly.
     * 
     * @return Decay rate (0.0-1.0 typical range)
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * @brief Set decay rate for memory traces
     * 
     * Sets the decay rate for memory traces. This controls how quickly
     * information fades from working memory over time.
     * 
     * @param rate New decay rate (0.0-1.0)
     */
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    /**
     * @brief Get neurons currently in working memory
     * 
     * Returns the vector of neurons currently maintained in working memory.
     * This represents the active population that is currently being used
     * for cognitive processing.
     * 
     * @return Vector of neuron IDs currently in working memory
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    /**
     * @brief Strengthen working memory representation (for rehearsal)
     * 
     * Strengthens the working memory representation by increasing activation
     * levels of neurons in the current memory trace. This simulates rehearsal
     * or practice that enhances memory retention.
     * 
     * @param factor Strength multiplication factor (typically >1.0)
     */
    void strengthenMemory(float factor);
    
    /**
     * @brief Competition between memory traces
     * 
     * Runs competition between memory traces to select the winning population.
     * This simulates the competitive process that determines which information
     * is actively maintained in working memory.
     * 
     * @note Competition is based on activation levels, strength, and
     *       attention modulation. The winning population receives enhanced
     *       maintenance support.
     */
    void runCompetition();
    
    /**
     * @brief Is this neuron part of the winning population?
     * 
     * Determines whether a specific neuron is part of the winning population
     * in working memory competition. The winning population receives priority
     * for maintenance resources.
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron is part of winning population, false otherwise
     */
    bool isWinning(NeuronId neuron) const;
    
    /**
     * @brief Get overall memory activity level
     * 
     * Returns the overall activity level of the working memory system.
     * This is a measure of how engaged the working memory system is
     * and can be used for monitoring system state.
     * 
     * @return Overall activity level (0.0-1.0)
     */
    float getMemoryActivity() const;
    
    /**
     * @brief Get current developmental stage multiplier
     * 
     * Returns how the current developmental stage affects working memory capacity
     * and processing. Different developmental stages have different working memory
     * characteristics and capacities.
     * 
     * @return Developmental stage multiplier for working memory
     */
    float getDevelopmentalStageMultiplier() const;
    
    /**
     * @brief Get current working memory parameters
     * 
     * Returns current working memory parameters.
     * 
     * @return Vector containing current parameters [capacity, decayRate, strengtheningFactor]
     */
    std::vector<float> getParameters() const;
    
    /**
     * @brief Set working memory parameters
     * 
     * Sets internal parameters for working memory processing.
     * 
     * @param capacity Memory capacity
     * @param decayRate Decay rate for memory traces
     * @param strengtheningFactor Factor for memory strengthening
     */
    void setParameters(size_t capacity, float decayRate, float strengtheningFactor);
    
private:
    /**
     * @brief Create recurrent connection for maintenance
     * 
     * Creates a recurrent connection between neurons to support persistent
     * activity maintenance. This is the core mechanism for working memory
     * stability.
     * 
     * @param from Source neuron ID
     * @param to Target neuron ID
     * @param strength Connection strength
     */
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    /**
     * @brief Update recurrent connections for maintenance
     * 
     * Updates recurrent connections to maintain persistent activity.
     * This simulates the dynamic maintenance of working memory through
     * synaptic plasticity and recurrent dynamics.
     */
    void updateRecurrentConnections();
    
    /**
     * @brief Decay weak memory traces
     * 
     * Decays weak memory traces based on decay rate and strength,
     * removing traces that have become too weak to maintain.
     * 
     * This simulates the natural forgetting process in working memory.
     */
    void decayWeakTraces();
    
    /**
     * @brief Implementation details (pImpl pattern)
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Reference to brain for working memory context
     */
    Brain* brain_;
    
    /**
     * @brief Working memory capacity
     */
    size_t capacity_;
    
    /**
     * @brief Decay rate for memory traces
     */
    float decayRate_;
    
    /**
     * @brief Memory content
     */
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    /**
     * @brief Active memory traces
     */
    std::vector<size_t> activeTraces_;
    
    /**
     * @brief Recurrent connections for maintenance
     */
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    /**
     * @brief Winner neurons (for competition)
     */
    std::vector<NeuronId> winners_;
};

// AttentionalSelection: Neural attention through competition
// NOT Transformer attention - uses lateral inhibition and gain modulation
//
// Mechanisms:
// - Competition between neural populations
// - Salience-based selection
// - Recurrent amplification of winners
// - Inhibitory surround suppression

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Process competing sensory inputs and select winners
    // Returns IDs of winning neural populations
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);

    // Apply attention to a specific region (focus processing there)
    void focusOnRegion(RegionId region);

    // Release attention (allow all regions equal processing)
    void releaseAttention();

    // Get current attention focus
    std::vector<RegionId> getAttendedRegions() const;

    // Set attention parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);

    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;

    // Update attention dynamics
    void update(TimestepDuration dt);

    // Is this neuron currently attended?
    bool isAttended(NeuronId neuron) const;

    // Get attention winners
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Top-down attention (goal-directed)
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Bottom-up attention (stimulus-driven)
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    // Reset attention
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};

} // namespace nlm
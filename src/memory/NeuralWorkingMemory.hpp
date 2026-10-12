#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention

/**
 * @class NeuralWorkingMemory
 * @brief Implements working memory using persistent neural activity and recurrent connections
 * @details This working memory system is fundamentally different from traditional
 *         vector-based storage. Instead, it uses persistent neural activity maintained
 *         through recurrent connections and short-term plasticity mechanisms.
 * 
 * @note Unlike standard working memory implementations that store information
 *       as vectors or patterns, this neural working memory:
 *       - Encodes information as patterns of persistent neural firing
 *       - Uses recurrent connections to maintain information over time
 *       - Implements competitive dynamics for selective retention
 *       - Leverages activity-dependent plasticity for memory stabilization
 * 
 * @warning This is a neural working memory, not a simple storage vector. Information
 *         is encoded as neural activity patterns, not abstract representations.
 * 
 * @author NLM Research Team
 * @date Version 0.1.0
 * @ingroup memory
 * @implements neural_memory_system
 */

class NeuralWorkingMemory {
public:
    // Default constructor
    /**
     * @brief Construct a new NeuralWorkingMemory object
     * @details Creates a neural working memory with default parameters:
     *          - Capacity: 100 memory traces
     *          - Decay rate: 0.01 per timestep (1% decay per step)
     *          - No initial brain reference
     * 
     * @note The working memory must be initialized with a Brain reference
     *       before it can be used. Call initialize() with a valid Brain pointer.
     */
    NeuralWorkingMemory();
    
    /**
     * @brief Destroy the NeuralWorkingMemory object
     * @details Cleans up all memory traces, recurrent connections, and temporary buffers.
     *          The working memory will be in an uninitialized state after destruction.
     */
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    /**
     * @brief Initialize the working memory with a brain reference
     * @param brain Pointer to the Brain object
     * @details Initializes the working memory system with a reference to the
     *          neural brain. This connection is essential for:
     *          - Accessing neurons for memory encoding
     *          - Using brain-wide recurrent connections
     *          - Integrating with brain's plasticity systems
     *          - Synchronizing with brain's simulation clock
     * 
     * @note This must be called before any other working memory operations.
     *       The working memory takes a reference to the brain and will use it
     *       for memory encoding and maintenance operations.
     * 
     * @param brain Pointer to Brain object, must not be nullptr
     * @pre Brain must be fully initialized before calling this method
     * @post Working memory is ready to store and retrieve information
     * @see store() for storing information in working memory
     * @see retrieve() for retrieving working memory content
     */
    void initialize(Brain* brain);

    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);

    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;

    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);

    // Clear working memory
    void clear();

    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }

    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);

    // Competition between memory traces
    void runCompetition();

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Decay weak memory traces
    void decayWeakTraces();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
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
#pragma once

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * @class NeuralWorkingMemory
 * @brief Working memory using neural dynamics and persistent activity
 * @details NeuralWorkingMemory implements working memory through persistent neural
 * activity rather than temporary storage. It uses reverberating activity patterns
 * maintained by recurrent connections, allowing information to be held "in mind"
 * during cognitive processing.
 * 
 * Working Memory Mechanisms:
 * - Persistent activity: Recurrent connections maintain firing patterns
 * - Short-term plasticity: Dynamic modulation of synaptic strength
 * - Neural competition: Winner-take-all for selective attention
 * - Activity-dependent decay: Forgetting based on neural activity
 * 
 * Neural Architecture:
 * - Memory traces: Distributed neural population codes
 * - Recurrent loops: Feedback connections maintain activity
 * - Competition layers: Lateral inhibition selects representations
 * - Maintenance circuits: Sustained firing through network dynamics
 * 
 * Thread Safety: Thread-safe for concurrent access. Working memory is updated
 * during simulation steps and accessed by cognitive components.
 * 
 * Performance: O(n) for store/retrieve operations where n is number of neurons.
 * O(log n) for competition and decay operations.
 * 
 * Capacity:
 * - Neural capacity: Limited by recurrent network properties
 * - Number of traces: Determined by competition and capacity limits
 * - Trace duration: Exponential decay with characteristic time constant
 * - Interference: Cross-talk between traces reduces memory fidelity
 * 
 * Biological Plausibility:
 * - Mirrors prefrontal cortex persistent activity
 * - Implements working memory through sustained firing
 * - Supports object working memory and spatial working memory
 * - Compatible with delayed-response tasks
 * 
 * @code
 * // Initialize working memory
 * NeuralWorkingMemory wmem;
 * wmem.initialize(brain);
 * wmem.setCapacity(10);      // Number of items
 * wmem.setDecayRate(0.01f);   // 1% decay per timestep
 * 
 * // Store information
 * vector<float> pattern = {0.8f, 0.3f, 0.6f, 0.1f};
 * wmem.store(pattern, 1.0f);
 * 
 * // Or store specific neuron activation
 * wmem.storeToNeuron(42, 0.9f);  // Neuron 42 fires strongly
 * 
 * // Retrieve current content
 * vector<float> content = wmem.retrieve();
 * 
 * // Update with decay and competition
 * wmem.update(dt);
 * wmem.runCompetition();
 * 
 * // Use for cognitive tasks
 * if (wmem.isWinning(targetNeuron)) {
 *     // This representation is currently dominant
 *     // Use for planning or decision making
 * }
 * @endcode
 * 
 * Working Memory Functions:
 * - Maintenance of information across cognitive stages
 * - Active manipulation of stored representations
 * - Integration with attention and working memory load
 * - Support for sequential and simultaneous processing
 * 
 * @see AttentionalSelection
 * @see Brain
 * @see SimulationClock
 * @see NeuralEpisodicMemory
 * @see NeuralAssociativeMemory
 */
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);

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
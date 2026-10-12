#pragma once

/**
 * NeuralWorkingMemory: Implements working memory using neural dynamics
 * NOT a simple storage vector - uses persistent activity and recurrent connections
 *
 * Key mechanisms:
 * - Persistent neural activity (reverberating activity)
 * - Recurrent synaptic connections for maintaining information
 * - Activity-dependent short-term plasticity
 * - Neural competition for selective retention
 *
 * This working memory system is biologically inspired, using actual neural
 * populations to store and maintain information through persistent spiking
 * and recurrent connectivity patterns.
 *
 * Working memory in NLM serves as the "working memory" component of the
 * integrated brain architecture, maintaining active representations of
 * current sensory inputs, goals, and recent experiences while allowing
 * for selection among competing representations through neural competition.
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * Working Memory Implementation Details
 * 
 * The NeuralWorkingMemory class implements working memory as a dynamical system
 * rather than a static storage buffer. Information is encoded as persistent
 * neural activity patterns that are maintained through:
 * 
 * 1. **Persistent Activity**: Individual neurons can maintain elevated firing
 *    rates to represent stored information
 * 2. **Recurrent Connections**: Networks of neurons form loops that reinforce
 *    each other's activity, creating stable attractor states
 * 3. **Activity-Dependent Plasticity**: Synaptic weights are modified based on
 *    co-activity, strengthening important connections
 * 4. **Competitive Selection**: Multiple memory traces compete, with the strongest
 *    winning through mutual inhibition and excitation
 * 
 * The system is designed to:
 * - Maintain information for seconds to minutes (matching biological working memory)
 * - Allow multiple items to be stored simultaneously with capacity limits
 * - Decay irrelevant information over time
 * - Select the most relevant item for use by other brain systems
 * - Integrate with other memory systems (episodic, associative)
 */

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    /**
     * Initialize working memory with brain reference and set up neural substrates
     * 
     * This method establishes the neural populations that will serve as the
     * substrate for working memory storage. It creates connections and configures
     * plasticity rules needed for persistent activity patterns.
     * 
     * @param brain Pointer to the parent brain instance - provides access to
     *              neural populations, plasticity systems, and other brain functions
     */
    void initialize(Brain* brain);
    
    /**
     * Store information in working memory by encoding it as neural activity patterns
     * 
     * This method converts an external pattern (vector of activation values) into
     * a distributed neural representation. The pattern is encoded by injecting
     * current into appropriate neural populations based on the pattern values.
     * 
     * @param pattern Vector of activation values representing the information
     *               to be stored (typically normalized between 0 and 1)
     * @param strength Overall strength of the memory trace (default: 1.0)
     * @note The pattern is encoded into approximately min(pattern.size(), 4) neurons
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    /**
     * Store specific neuron activation in working memory
     * 
     * This method creates a direct neural representation of an existing neuron's
     * activation level, useful for incorporating ongoing neural activity into
     * working memory.
     * 
     * @param neuron ID of the neuron whose activation should be stored
     * @param activation Current activation level of the neuron (0.0 to 1.0)
     */
    void storeToNeuron(NeuronId neuron, float activation);
    
    /**
     * Retrieve current working memory content as activity levels
     * 
     * This method converts the neural activity patterns back into an external
     * format for use by other brain systems or for analysis.
     * 
     * @return Vector of activation values representing the current memory contents
     */
    std::vector<float> retrieve() const;
    
    /**
     * Check if specific neuron is part of working memory
     * 
     * @param neuron ID of the neuron to check
     * @return true if the neuron is actively maintaining a memory trace
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * Get activation level of a specific neuron in working memory
     * 
     * @param neuron ID of the neuron to query
     * @return Current activation level (0.0 if neuron not in memory)
     */
    float getNeuronActivation(NeuronId neuron) const;
    
    /**
     * Update working memory dynamics (maintenance and decay)
     * 
     * This method implements the working memory update cycle:
     * - Reinforces active memory neurons through recurrent connections
     * - Ages and decays old traces
     * - Runs competition to select winners
     * - Applies external inputs and modulates based on neuromodulators
     * 
     * @param dt Time step duration for continuous-time simulation
     */
    void update(TimestepDuration dt);
    
    /**
     * Clear working memory completely
     * 
     * This removes all memory traces and recurrent connections, effectively
     * resetting working memory to an empty state.
     */
    void clear();
    
    /**
     * Get number of active memory traces
     * 
     * @return Number of neurons currently maintaining memory traces
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    /**
     * Get working memory capacity (maximum number of memory traces)
     * 
     * @return Memory capacity in number of traces
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * Set working memory capacity
     * 
     * @param cap New capacity (must be positive)
     */
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    /**
     * Get decay rate for memory traces (0.0 = no decay, 1.0 = complete decay)
     * 
     * @return Decay rate per timestep
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * Set decay rate for memory traces
     * 
     * @param rate New decay rate (0.0 to 1.0)
     */
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    /**
     * Get neurons currently in working memory
     * 
     * @return Vector of neuron IDs that are maintaining memory traces
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    /**
     * Strengthen working memory representation (for rehearsal)
     * 
     * This method increases the activation of all memory traces, simulating
     * the process of rehearsal that helps maintain information in working memory.
     * 
     * @param factor Multiplier for current activations (typically > 1.0)
     */
    void strengthenMemory(float factor);
    
    /**
     * Run competition between memory traces
     * 
     * This method implements competitive selection among memory traces, where
     * stronger traces inhibit weaker ones through mutual inhibition and excite
     * themselves through recurrent connections.
     * 
     * @note This method is called automatically during the update cycle
     */
    void runCompetition();
    
    /**
     * Check if neuron is part of the winning population
     * 
     * @param neuron ID of the neuron to check
     * @return true if the neuron is a winner in the competition
     */
    bool isWinning(NeuronId neuron) const;
    
    /**
     * Get overall memory activity level
     * 
     * @return Average activation across all memory traces (0.0 = no activity)
     */
    float getMemoryActivity() const;
    
private:
    /**
     * Create recurrent connection for maintenance
     * 
     * This method establishes a direct synaptic connection between two neurons
     * to help maintain their activity through mutual excitation, forming a loop
     * that can sustain persistent activity.
     * 
     * @param from Source neuron ID
     * @param to Target neuron ID
     * @param strength Synaptic strength of the connection
     */
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    /**
     * Update recurrent connections for maintenance
     * 
     * This method applies maintenance currents through recurrent connections
     * to strengthen persistent activity patterns.
     * 
     * @note Called during the update cycle to maintain active traces
     */
    void updateRecurrentConnections();
    
    /**
     * Decay weak memory traces
     * 
     * This method gradually reduces the activation of weak or unused memory traces,
     * effectively implementing the forgetting function of working memory.
     * 
     * @note Called automatically during the update cycle
     */
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
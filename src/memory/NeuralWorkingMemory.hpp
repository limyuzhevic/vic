#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

namespace nlm {

/**
 * @file NeuralWorkingMemory.hpp
 * @brief Header file for the NeuralWorkingMemory and AttentionalSelection classes
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the NeuralWorkingMemory and AttentionalSelection class definitions,
 * which implement working memory using neural dynamics and attentional selection
 * through neural competition. These classes represent advanced cognitive
 * capabilities for information maintenance, selective attention, and competition
 * between neural representations.
 */

/**
 * @class NeuralWorkingMemory
 * @brief Implements working memory using persistent neural activity and recurrent connections
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The NeuralWorkingMemory class implements working memory using neural dynamics
 * rather than simple storage vectors. It maintains information through persistent
 * neural activity, recurrent synaptic connections, activity-dependent short-term
 * plasticity, and neural competition for selective retention.
 *
 * Key mechanisms:
 * - Persistent neural activity (reverberating activity)
 * - Recurrent synaptic connections for maintaining information
 * - Activity-dependent short-term plasticity
 * - Neural competition for selective retention
 *
 * This working memory system is essential for:
 * - Maintaining active representations of task-relevant information
 * - Supporting goal-directed behavior through active maintenance
 * - Implementing short-term memory processes in neural systems
 * - Integrating with attention systems for selective focus
 *
 * @note This class uses the Pimpl idiom for implementation hiding, providing
 * efficient compilation times and encapsulation. All neural mechanisms are
 * managed through private implementation details.
 *
 * @warning Working memory capacity is limited. Information exceeding capacity
 * may be lost or overwritten.
 *
 * @see Brain, Curiosity, AttentionalSelection
 */
class NeuralWorkingMemory {
public:
    /**
     * @brief Constructor for the NeuralWorkingMemory class
     * @post Creates working memory system with initial state
     * @note Working memory system is ready for information storage after construction
     */
    NeuralWorkingMemory();
    
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    /**
     * @brief Initialize working memory system with brain reference
     * @param brain Pointer to brain for system integration
     * @post Working memory system is initialized and ready for operation
     * @note Required for proper system integration and neural connectivity
     */
    void initialize(Brain* brain);
    
    // Store information in working memory
    // The information is encoded as neural activity pattern
    /**
     * @brief Store information pattern in working memory
     * @param pattern Vector of activation values to store
     * @param strength Initial strength of memory trace (default: 1.0f)
     * @post Information is encoded as persistent neural activity pattern
     * @note Uses recurrent connections to maintain information over time
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    // Store specific neuron activation
    /**
     * @brief Store activation to specific neuron in working memory
     * @param neuron Neuron identifier to activate
     * @param activation Activation level to set (typically 0.0 to 1.0)
     * @post Specific neuron is activated in working memory
     * @note More efficient for targeted memory storage
     */
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content as activity levels
    /**
     * @brief Retrieve current working memory content
     * @return std::vector<float> Vector of activation levels for all memory neurons
     * @post Returns current working memory state as neural activity patterns
     * @note Provides read-only access to current memory content
     */
    std::vector<float> retrieve() const;
    
    // Check if specific neuron is part of working memory
    /**
     * @brief Check if neuron is currently part of working memory
     * @param neuron Neuron identifier to check
     * @return bool true if neuron is in working memory, false otherwise
     * @post Returns membership status in working memory
     */
    bool contains(NeuronId neuron) const;
    
    // Get activation level of a specific neuron in working memory
    /**
     * @brief Get activation level of specific neuron in working memory
     * @param neuron Neuron identifier to query
     * @return float Activation level of neuron in working memory
     * @post Returns current activation value
     */
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update working memory (maintenance and decay)
    /**
     * @brief Update working memory state for one simulation step
     * @param dt Timestep duration for temporal integration
     * @post Working memory is updated through recurrent dynamics and decay
     * @note Maintains information through recurrent connections and applies decay
     */
    void update(TimestepDuration dt);
    
    // Clear working memory
    /**
     * @brief Clear working memory completely
     * @post All memory traces and neural activity are cleared
     * @note Removes all stored information and resets system state
     */
    void clear();
    
    // Get number of active memory traces
    /**
     * @brief Get number of active memory traces
     * @return size_t Count of currently active memory representations
     * @post Returns count of active working memory items
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    // Get capacity
    /**
     * @brief Get working memory capacity
     * @return size_t Maximum number of memory items that can be stored
     * @post Returns current capacity limit
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * @brief Set working memory capacity
     * @param cap New capacity limit
     * @post Capacity is updated
     * @note May trigger automatic cleanup of excess memory items
     */
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Decay rate for memory traces
    /**
     * @brief Get current decay rate
     * @return float Decay rate per timestep (typically 0.0 to 1.0)
     * @post Returns current decay rate
     * @note Controls how quickly memory traces fade
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * @brief Set decay rate for memory traces
     * @param rate New decay rate per timestep
     * @post Decay rate is updated
     * @note Higher values cause faster memory decay
     */
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    // Get neurons currently in working memory
    /**
     * @brief Get identifiers of neurons currently holding working memory
     * @return const std::vector<NeuronId>& Vector of neuron identifiers
     * @post Returns all neurons currently in working memory
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    // Strengthen working memory representation (for rehearsal)
    /**
     * @brief Strengthen working memory representation
     * @param factor Strength amplification factor
     * @post Memory traces are strengthened by specified factor
     * @note Used for memory rehearsal and consolidation
     */
    void strengthenMemory(float factor);
    
    // Competition between memory traces
    /**
     * @brief Run competition between memory traces
     * @post Winner memory traces are selected through competitive processes
     * @note Implements neural competition for limited working memory resources
     */
    void runCompetition();
    
    // Is this neuron part of the winning population?
    /**
     * @brief Check if neuron is part of winning memory population
     * @param neuron Neuron identifier to check
     * @return bool true if neuron is in winning population, false otherwise
     * @post Returns competition outcome for specified neuron
     */
    bool isWinning(NeuronId neuron) const;
    
    // Get overall memory activity level
    /**
     * @brief Get overall activity level of working memory
     * @return float Normalized activity level (0.0 to 1.0)
     * @post Returns aggregated activity across all memory traces
     */
    float getMemoryActivity() const;
    
private:
    // Create recurrent connection for maintenance
    /**
     * @brief Create recurrent connection to maintain memory trace
     * @param from Source neuron identifier
     * @param to Target neuron identifier
     * @param strength Connection strength
     * @post Recurrent connection is established for memory maintenance
     */
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    // Update recurrent connections for maintenance
    /**
     * @brief Update all recurrent connections for memory maintenance
     * @post Recurrent connections are updated to maintain memory activity
     * @note Supports persistent activity through recurrent dynamics
     */
    void updateRecurrentConnections();
    
    // Decay weak memory traces
    /**
     * @brief Decay weak memory traces based on strength and decay rate
     * @post Weak memory traces are gradually faded out
     * @note Implements forgetting and memory decay processes
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

/**
 * @class AttentionalSelection
 * @brief Neural attentional selection through competition and inhibition
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The AttentionalSelection class implements neural attentional selection through
 * competition between neural populations, lateral inhibition, and gain modulation.
 * Unlike Transformer attention, this system uses biologically plausible mechanisms
 * including recurrent amplification and inhibitory surround suppression to
 * select relevant information and focus processing resources.
 *
 * Key mechanisms:
 * - Competition between neural populations
 * - Salience-based selection
 * - Recurrent amplification of winners
 * - Inhibitory surround suppression
 *
 * This attentional system is essential for:
 * - Selectively allocating processing resources
 * - Filtering sensory information
 * - Implementing goal-directed attention
 * - Managing winner-take-all competition
 *
 * @note This class uses the Pimpl idiom for implementation hiding and efficient
 * compilation. All attentional mechanisms are managed through private
 * implementation details.
 *
 * @warning Attentional selection parameters significantly affect attention
 * allocation and competition dynamics.
 *
 * @see Brain, NeuralWorkingMemory
 */
class AttentionalSelection {
public:
    /**
     * @brief Constructor for the AttentionalSelection class
     * @post Creates attentional selection system with initial state
     * @note System is ready for attention allocation after construction
     */
    AttentionalSelection();
    
    ~AttentionalSelection();
    
    // Initialize with brain reference
    /**
     * @brief Initialize attentional selection system with brain reference
     * @param brain Pointer to brain for system integration
     * @post Attentional selection system is initialized and ready for operation
     * @note Required for proper system integration and neural connectivity
     */
    void initialize(Brain* brain);
    
    // Process competing sensory inputs and select winners
    // Returns IDs of winning neural populations
    /**
     * @brief Process competition among neural populations
     * @param competitors Vector of competing neuron identifiers
     * @param globalInhibition Global inhibition strength (default: 0.5f)
     * @return std::vector<NeuronId> Vector of winning neuron identifiers
     * @post Competition is processed and winners are selected
     * @note Implements winner-take-all competition with lateral inhibition
     */
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);
    
    // Apply attention to a specific region (focus processing there)
    /**
     * @brief Apply attention to specific brain region
     * @param region Region identifier to attend to
     * @post Attention is focused on specified region
     * @note Overrides competition to prioritize specific region
     */
    void focusOnRegion(RegionId region);
    
    // Release attention (allow all regions equal processing)
    /**
     * @brief Release attention (allow equal processing)
     * @post Attention is released to all regions
     * @note Allows all competing populations to process equally
     */
    void releaseAttention();
    
    // Get current attention focus
    /**
     * @brief Get currently attended regions
     * @return std::vector<RegionId> Vector of currently attended regions
     * @post Returns list of regions receiving attention
     */
    std::vector<RegionId> getAttendedRegions() const;
    
    // Set attention parameters
    /**
     * @brief Set inhibition strength for attentional competition
     * @param strength Inhibition strength (typically 0.0 to 1.0)
     * @post Inhibition strength is updated
     * @note Higher values increase competitive suppression
     */
    void setInhibitionStrength(float strength);
    
    /**
     * @brief Set excitation strength for attentional enhancement
     * @param strength Excitation strength (typically 0.0 to 1.0)
     * @post Excitation strength is updated
     * @note Higher values enhance winning populations
     */
    void setExcitationStrength(float strength);
    
    /**
     * @brief Set competition threshold for winner selection
     * @param threshold Competition threshold (typically 0.0 to 1.0)
     * @post Competition threshold is updated
     * @note Affects how strongly competition selects winners
     */
    void setCompetitionThreshold(float threshold);
    
    // Get inhibition level for a neuron
    /**
     * @brief Get inhibition level for specific neuron
     * @param neuron Neuron identifier to query
     * @return float Inhibition level for neuron
     * @post Returns inhibition value for specified neuron
     */
    float getInhibitionFor(NeuronId neuron) const;
    
    // Get excitation level for a neuron
    /**
     * @brief Get excitation level for specific neuron
     * @param neuron Neuron identifier to query
     * @return float Excitation level for neuron
     * @post Returns excitation value for specified neuron
     */
    float getExcitationFor(NeuronId neuron) const;
    
    // Update attention dynamics
    /**
     * @brief Update attentional dynamics for one simulation step
     * @param dt Timestep duration for temporal integration
     * @post Attentional weights and states are updated
     * @note Implements attentional dynamics and competition
     */
    void update(TimestepDuration dt);
    
    // Is this neuron currently attended?
    /**
     * @brief Check if neuron is currently attended
     * @param neuron Neuron identifier to check
     * @return bool true if neuron is currently attended, false otherwise
     * @post Returns attentional status for specified neuron
     */
    bool isAttended(NeuronId neuron) const;
    
    // Get attention winners
    /**
     * @brief Get current attention winners
     * @return const std::vector<NeuronId>& Vector of currently attended neurons
     * @post Returns list of neurons that won attentional competition
     */
    const std::vector<NeuronId>& getWinners() const { return winners_; }
    
    // Top-down attention (goal-directed)
    /**
     * @brief Apply top-down attention bias
     * @param neuron Neuron identifier to bias
     * @param biasStrength Bias strength (typically 0.0 to 1.0)
     * @post Top-down bias is applied to specified neuron
     * @note Implements goal-directed attentional modulation
     */
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    
    // Bottom-up attention (stimulus-driven)
    /**
     * @brief Apply bottom-up attention salience
     * @param neuron Neuron identifier to modulate
     * @param salienceStrength Salience strength (typically 0.0 to 1.0)
     * @post Bottom-up salience is applied to specified neuron
     * @note Implements stimulus-driven attentional modulation
     */
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
    // Reset attention
    /**
     * @brief Reset attentional system to initial state
     * @post All attentional weights and biases are cleared
     * @note Resets attentional state and clears saliency maps
     */
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
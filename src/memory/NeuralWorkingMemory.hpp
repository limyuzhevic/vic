#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * @class NeuralWorkingMemory
 * @brief Implements working memory using persistent neural activity
 * @description Provides neural working memory through reverberating activity patterns
 *              and recurrent connections. Unlike simple storage, working memory maintains
 *              information as persistent neural firing patterns that can be refreshed
 *              through sensory input or internal rehearsal.
 * @note Working memory is essential for cognitive functions like planning,
 *       decision-making, and maintaining goals. It has limited capacity and decays
 *       over time without reinforcement.
 * @warning Currently in development phase - some methods are PLACEHOLDERs
 * @version 1.0.0
 * @copyright Copyright (c) 2024 NLM Team
 */
class NeuralWorkingMemory {
public:
    /**
     * @brief Construct working memory system
     * @description Creates an empty working memory system. Requires initialization
     *              with a Brain instance before use. Memory content is initially empty.
     */
    NeuralWorkingMemory();
    
    /**
     * @virtual Destructor
     * @description Cleans up working memory resources and neural connections.
     *              Releases memory traces and recurrent connections.
     */
    ~NeuralWorkingMemory();
    
    /**
     * @brief Initialize with brain reference
     * @param brain Pointer to brain system
     * @description Initializes working memory with access to brain components
     *              such as neurons, regions, and the spike system. This is required
     *              for creating and managing neural representations.
     * @warning Brain pointer must remain valid throughout working memory lifetime
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Store information in working memory
     * @param pattern Neural activity pattern representing the information
     * @param strength Initial activation strength (default: 1.0)
     * @description Encodes information as a persistent neural activity pattern.
     *              The pattern is transformed into neural firing patterns that maintain
     *              information through recurrent connections.
     * @note Patterns are typically sensory data, motor plans, or cognitive goals
     * @see retrieve
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    /**
     * @brief Store specific neuron activation
     * @param neuron Neuron ID to store activation in
     * @param activation Neural activation level (0.0 to 1.0)
     * @description Directly sets activation of a specific neuron as part of
     *              working memory representation. Used for targeted memory updates.
     * @note Can create or modify neural patterns directly
     */
    void storeToNeuron(NeuronId neuron, float activation);
    
    /**
     * @brief Retrieve current working memory content as activity levels
     * @return Vector of neural activation levels representing memory content
     * @description Returns the current working memory state as neural activity levels.
     *              This represents what information is currently being maintained.
     * @note Returns empty vector if no memory traces exist
     * @see store
     */
    std::vector<float> retrieve() const;
    
    /**
     * @brief Check if specific neuron is part of working memory
     * @param neuron Neuron ID to check
     * @return true if neuron is part of active working memory traces
     * @description Returns true if the specified neuron is currently maintaining
     *              memory information. Useful for determining which neurons are
     *              actively involved in working memory tasks.
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * @brief Get activation level of a specific neuron in working memory
     * @param neuron Neuron ID to query
     * @return Current activation level (0.0 to 1.0) or 0.0 if not in memory
     * @description Returns the strength of the specific neuron's contribution
     *              to working memory representation. Used for monitoring and control.
     * @note Returns 0.0 if neuron is not currently in working memory
     */
    float getNeuronActivation(NeuronId neuron) const;
    
    /**
     * @brief Update working memory (maintenance and decay)
     * @param dt Time step duration in seconds
     * @description Updates working memory dynamics including decay of weak traces,
     *              maintenance of strong traces through recurrent connections, and
     *              competition between memory representations.
     * @note This method is called each simulation step to maintain memory integrity
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Clear working memory
     * @description Removes all working memory content and resets neural patterns.
     *              All neurons' working memory activations are set to zero.
     * @note Also clears recurrent connections used for maintenance
     */
    void clear();
    
    /**
     * @brief Get number of active memory traces
     * @return Count of active memory representations
     * @description Returns the number of distinct working memory items or patterns
     *              currently being maintained. Reflects memory load.
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    /**
     * @brief Get memory capacity
     * @return Maximum number of memory items that can be stored
     * @description Returns the working memory capacity limit. Working memory can
     *              typically hold 4-7 items, though this varies with neural dynamics.
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * @brief Set memory capacity
     * @param cap New capacity limit
     * @description Configures the maximum number of memory items that can be
     *              simultaneously maintained. Affects eviction policies.
     */
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    /**
     * @brief Get decay rate for memory traces
     * @return Decay rate (typically 0.0-0.1 per second)
     * @description Returns the rate at which unattended memory traces decay.
     *              Higher values cause faster decay of unattended information.
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * @brief Set decay rate
     * @param rate New decay rate
     * @description Configures the decay rate for unattended memory traces.
     *              Can be adjusted based on task demands and neural dynamics.
     */
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    /**
     * @brief Get neurons currently in working memory
     * @return Const reference to vector of neuron IDs
     * @description Returns the list of neurons that are currently participating
     *              in working memory representations. Useful for monitoring and analysis.
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    /**
     * @brief Strengthen working memory representation (for rehearsal)
     * @param factor Multiplicative factor for strengthening (typically > 1.0)
     * @description Increases activation of all memory traces by the specified factor.
     *              Used for active rehearsal and maintenance of important information.
     * @note Can be used to boost weak memory traces or refresh fading memories
     */
    void strengthenMemory(float factor);
    
    /**
     * @brief Run competition between memory traces
     * @description Executes the competitive process that selects the strongest
     *              memory traces and suppresses weaker ones. Implements resource
     *              allocation and winner-take-all dynamics.
     * @note Competition determines which memories get priority access to neural resources
     */
    void runCompetition();
    
    /**
     * @brief Check if neuron is part of the winning population
     * @param neuron Neuron ID to check
     * @return true if neuron is among the winning traces
     * @description Returns true if the specified neuron is part of the competition
     *              winner group that receives enhanced resources and processing.
     */
    bool isWinning(NeuronId neuron) const;
    
    /**
     * @brief Get overall memory activity level
     * @return Aggregate activity level across all memory traces (0.0 to 1.0)
     * @description Computes the overall level of activity in working memory,
     *              reflecting total memory load and accessibility.
     */
    float getMemoryActivity() const;
    
private:
    /**
     * @brief Create recurrent connection for maintenance
     * @param from Source neuron ID
     * @param to Target neuron ID
     * @param strength Connection strength (typically positive)
     * @description Creates a recurrent synaptic connection that helps maintain
     *              memory traces through reverberating activity. Essential for
     *              persistent memory storage.
     * @note These connections are created when memory traces are formed
     */
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    /**
     * @brief Update recurrent connections for maintenance
     * @description Updates the strengths and timing of recurrent connections
     *              based on current activity patterns. Adjusts for decay and
     *              strengthens successful traces.
     * @note Maintains memory persistence while allowing flexibility
     */
    void updateRecurrentConnections();
    
    /**
     * @brief Decay weak memory traces
     * @description Reduces activation of memory traces that are not being
     *              reinforced or attended. Implements the time-dependent decay
     *              characteristic of working memory.
     * @note Weaker traces are evicted based on capacity constraints
     */
    void decayWeakTraces();
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    /**
     * @brief Memory content with optimized data structures
     * @note Multiple data structures provide different access patterns:
     *       - memoryNeurons_: Ordered for sequential access
     *       - memoryActivations_: Parallel with neuron IDs
     *       - memoryTimestamps_: For age tracking
     *       - neuronActivationMap_: O(1) lookup
     *       - neuronAgeMap_: For age tracking
     */
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    /**
     * @brief Optimized lookup maps for O(1) access
     * @note These hash maps provide fast lookups and updates by neuron ID,
     *       crucial for performance with large numbers of neurons.
     */
    std::unordered_map<NeuronId, float> neuronActivationMap_;
    std::unordered_map<NeuronId, SimulationStep> neuronAgeMap_;
    
    /**
     * @brief Active memory traces
     * @note These indices track which memory items are currently active
     *       and eligible for competition and decay processing.
     */
    std::vector<size_t> activeTraces_;
    
    /**
     * @brief LRU tracking for eviction - maintains insertion order
     * @note Used for least-recently-used eviction when capacity is exceeded.
     *       Allows the system to retain frequently accessed memories.
     */
    std::list<NeuronId> lruOrder_;
    std::unordered_map<NeuronId, std::list<NeuronId>::iterator> lruIndex_;
    
    /**
     * @brief Winner neurons (for competition)
     * @note These neurons receive enhanced processing resources during competition.
     *       The winnersMap_ provides O(1) lookup for isWinning() queries.
     */
    std::vector<NeuronId> winners_;
    std::unordered_map<NeuronId, bool> winnersMap_;  // Optimized for O(1) isWinning() lookups
};

/**
 * @class AttentionalSelection
 * @brief Neural attention through competition between representations
 * @description Implements attentional selection using neural competition mechanisms
 *              including lateral inhibition, gain modulation, and winner-take-all dynamics.
 *              Attention selects which neural representations receive enhanced processing
 *              resources, implementing selective attention at the neural level.
 * @note Different from transformer attention - uses biological neural mechanisms
 * @warning Currently in development phase
 * @version 1.0.0
 */
class AttentionalSelection {
public:
    /**
     * @brief Construct attentional selection system
     * @description Creates attentional selection system without brain reference.
     *              Requires initialization before use.
     */
    AttentionalSelection();
    
    /**
     * @virtual Destructor
     * @description Cleans up attentional selection resources and neural connections.
     */
    ~AttentionalSelection();
    
    /**
     * @brief Initialize with brain reference
     * @param brain Pointer to brain system
     * @description Initializes attentional selection with access to brain components
     *              for implementing neural competition and attention mechanisms.
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Process competing sensory inputs and select winners
     * @param competitors Vector of neuron IDs representing competing inputs
     * @param globalInhibition Global inhibition strength (0.0 to 1.0, default: 0.5)
     * @return Vector of winning neuron IDs (typically 1-3 winners)
     * @description Processes competing inputs through lateral inhibition and gain
     *              modulation to select the most salient or relevant representations.
     * @note Implements winner-take-all competition with multiple winners possible
     */
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);
    
    /**
     * @brief Apply attention to a specific region
     * @param region Region ID to receive enhanced processing
     * @description Allocates enhanced processing resources to the specified region,
     *              increasing gain and reducing inhibition for neurons within that region.
     * @note Useful for top-down attention (goal-directed focus)
     */
    void focusOnRegion(RegionId region);
    
    /**
     * @brief Release attention (allow all regions equal processing)
     * @description Returns attentional resources to a balanced state where all regions
     *              receive equal processing without enhancement or suppression.
     * @note Useful for broad attentional focus or resetting attention
     */
    void releaseAttention();
    
    /**
     * @brief Get current attention focus
     * @return Vector of region IDs currently receiving enhanced attention
     * @description Returns the list of regions that are currently being attended to
     *              and receiving enhanced neural processing resources.
     */
    std::vector<RegionId> getAttendedRegions() const;
    
    /**
     * @brief Set attention parameters
     * @param strength New inhibition strength
     * @description Configures the strength of lateral inhibition between competing
     *              representations. Higher values create sharper competition.
     */
    void setInhibitionStrength(float strength);
    
    /**
     * @brief Set attention parameters
     * @param strength New excitation strength
     * @description Configures the baseline excitation level for attended representations.
     *              Higher values enhance processing of attended items.
     */
    void setExcitationStrength(float strength);
    
    /**
     * @brief Set attention parameters
     * @param threshold New competition threshold
     * @description Configures the threshold for winning competition. Higher thresholds
     *              require stronger activation to win competition.
     */
    void setCompetitionThreshold(float threshold);
    
    /**
     * @brief Get inhibition level for a neuron
     * @param neuron Neuron ID to query
     * @return Inhibition level (0.0 to 1.0)
     * @description Returns the amount of lateral inhibition applied to the specified neuron,
     *              which reduces its processing capability relative to others.
     */
    float getInhibitionFor(NeuronId neuron) const;
    
    /**
     * @brief Get excitation level for a neuron
     * @param neuron Neuron ID to query
     * @return Excitation level (0.0 to 1.0)
     * @description Returns the amount of enhanced processing applied to the specified neuron,
     *              which increases its activation and processing capability.
     */
    float getExcitationFor(NeuronId neuron) const;
    
    /**
     * @brief Update attention dynamics
     * @param dt Time step duration in seconds
     * @description Updates attentional dynamics including time-varying inhibition,
     *              adaptation, and response to changing stimulus saliency.
     * @note Called each simulation step to maintain attention
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Check if neuron is currently attended
     * @param neuron Neuron ID to check
     * @return true if neuron is receiving enhanced processing
     * @description Returns true if the specified neuron is currently being attended to
     *              and receiving enhanced neural processing resources.
     */
    bool isAttended(NeuronId neuron) const;
    
    /**
     * @brief Get attention winners
     * @return Const reference to vector of winning neuron IDs
     * @description Returns the neurons that won the last competition for attentional resources.
     *              These are the representations that received enhanced processing.
     */
    const std::vector<NeuronId>& getWinners() const { return winners_; }
    
    /**
     * @brief Apply top-down attention (goal-directed)
     * @param neuron Neuron ID to enhance
     * @param biasStrength Bias strength (typically 0.0 to 1.0)
     * @description Applies goal-directed bias to enhance processing of relevant representations
     *              based on current goals and expectations.
     */
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    
    /**
     * @brief Apply bottom-up attention (stimulus-driven)
     * @param neuron Neuron ID to enhance
     * @param salienceStrength Salience strength (typically 0.0 to 1.0)
     * @description Applies stimulus-driven bias to enhance processing of salient or unexpected
     *              inputs based on sensory input properties.
     */
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
    /**
     * @brief Reset attention
     * @description Resets attentional state and biases, returning to neutral state
     *              without enhanced or suppressed processing of any representations.
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
    
    /**
     * @brief Biases/salience stored by NeuronId for correct lookup in processCompetition
     * @note These hash maps provide O(1) lookup for attention and salience queries
     *       by neuron ID, crucial for performance with large numbers of neurons.
     */
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};

} // namespace nlm
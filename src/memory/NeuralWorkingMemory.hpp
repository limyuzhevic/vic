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
 * @brief Implements working memory using neural dynamics and persistent activity
 * 
 * The NeuralWorkingMemory class implements working memory as a neural system rather than
 * simple storage. It uses persistent neural activity and recurrent connections to maintain
 * information across timesteps, making it biologically plausible and robust.
 * 
 * **Core Mechanisms:**
 * - **Persistent Activity**: Neuronal assemblies maintain firing through recurrent connections
 * - **Synaptic Short-Term Plasticity**: Dynamic modulation of synaptic strength during activity
 * - **Neural Competition**: Winner-take-all dynamics for selective memory retention
 * - **Activity-Dependent Maintenance**: Stronger representations are maintained longer
 * 
 * **Working Memory Properties:**
 * - **Transient Nature**: Memories decay naturally when not rehearsed
 * - **Capacity Limits**: Determined by available neural assemblies
 * - **Activity-Based Encoding**: Information stored as neural firing patterns
 * - **Content Addressable**: Retrieval based on similarity to stored patterns
 * 
 * **Neural Implementation:**
 * - Recurrent connections form attractor networks
 * - Memory traces are self-sustaining neural activity
 * - Competition between memory traces prevents interference
 * - Short-term plasticity modulates trace strength and persistence
 * 
 * **Applications:**
 * - Maintaining task information during multi-step procedures
 * - Holding perceptual representations for recognition
 * - Storing goal states and behavioral plans
 * - Working memory in cognitive architectures
 * 
 * @note Integrates with brain system for neural connectivity
 * @see WorkingMemory for simple storage alternative
 * @see EpisodicMemory for long-term storage
 * @see Brain for neural system integration
 * 
 * @warning Thread safety required for concurrent access
 * @warning Memory content may be corrupted by interference without proper competition
 */

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    /**
     * @brief Initialize working memory with brain system reference
     * 
     * @param brain Pointer to the brain system for neural connectivity
     * @post Working memory initialized with brain reference
     * @pre Brain pointer must be valid and properly initialized
     * @pre Brain system must have neural populations and synapses
     * 
     * @note Establishes connections to brain's neural populations
     * @see Brain for system integration
     * @see store() for usage after initialization
     */
    void initialize(Brain* brain);

    // Store information in working memory
    /**
     * @brief Store information as a neural activity pattern in working memory
     * 
     * @param pattern Vector of activation levels for target neurons
     * @param strength Initial strength of the memory trace (0.0 to 1.0)
     * @post Pattern stored as persistent neural activity
     * @pre Brain system must be initialized
     * @pre Pattern length must match available neural populations
     * @pre Strength must be within valid range [0.0, 1.0]
     * 
     * @note Information is encoded as distributed neural activity patterns
     * @note Creates attractor dynamics for memory maintenance
     * @see storeToNeuron() for targeted storage
     * @see retrieve() to access stored patterns
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation
    /**
     * @brief Store information by directly activating a specific neuron
     * 
     * @param neuron Neuron identifier to activate
     * @param activation Activation level for the neuron (0.0 to 1.0)
     * @post Neuron activated with specified level
     * @pre Brain system must be initialized
     * @pre Neuron ID must be valid and belong to brain's neural populations
     * @pre Activation must be within valid range [0.0, 1.0]
     * 
     * @note Provides targeted storage for specific memory items
     * @note Creates direct recurrent connections for maintenance
     * @see store() for pattern-based storage
     * @see getNeuronActivation() to retrieve activation level
     */
    void storeToNeuron(NeuronId neuron, float activation);

    // Retrieve current working memory content as activity levels
    /**
     * @brief Retrieve the complete working memory content as neural activity levels
     * 
     * @return std::vector<float> Vector of activation levels for all memory neurons
     * @post Returns current memory state as neural activity
     * @pre Brain system must be initialized
     * @note Order corresponds to internal neuron mapping
     * @see store() to create memory content
     * @see getNeuronActivation() for specific neuron activation
     */
    std::vector<float> retrieve() const;

    // Check if specific neuron is part of working memory
    /**
     * @brief Check if a specific neuron is part of the working memory trace
     * 
     * @param neuron Neuron identifier to check
     * @return bool true if neuron is active in working memory
     * @post Does not modify memory state
     * @pre Brain system must be initialized
     * @pre Neuron ID must be valid
     * 
     * @note Indicates whether information is stored for this neuron
     * @see storeToNeuron() to add neurons to memory
     * @see getNeuronActivation() to get activation level
     */
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    /**
     * @brief Get the activation level of a specific neuron in working memory
     * 
     * @param neuron Neuron identifier to query
     * @return float Current activation level (0.0 to 1.0)
     * @throws std::out_of_range if neuron not in working memory
     * @pre Brain system must be initialized
     * @pre Neuron must be part of working memory
     * 
     * @note Represents the strength of the memory trace for this neuron
     * @see storeToNeuron() to set activation
     * @see contains() to check if neuron is in memory
     */
    float getNeuronActivation(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    /**
     * @brief Update working memory dynamics for one simulation timestep
     * 
     * @param dt Time step duration (seconds)
     * @post Memory traces updated according to dynamics
     * @pre Brain system must be initialized
     * @pre dt must be positive and valid
     * 
     * @note Performs several operations:
     *       - Maintains persistent activity through recurrent connections
     *       - Applies decay to weak memory traces
     *       - Runs competition between memory traces
     *       - Updates recurrent connection strengths
     * @see store() to create initial memory
     * @see decayWeakTraces() for internal decay mechanism
     */
    void update(TimestepDuration dt);

    // Clear working memory
    /**
     * @brief Clear all working memory content
     * 
     * @post All neural activity patterns cleared, working memory becomes empty
     * @see store() to add memory content
     * @note Typically called when starting new tasks
     */
    void clear();

    // Get number of active memory traces
    /**
     * @brief Get the count of active memory traces in working memory
     * 
     * @return size_t Number of memory traces currently active
     * @post Does not modify memory state
     * @note Represents number of distinct information items stored
     * @see store() to add new traces
     * @see getMemoryActivity() for activity level
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }

    // Get capacity
    /**
     * @brief Get the maximum capacity of working memory
     * 
     * @return size_t Maximum number of memory traces working memory can hold
     * @see setCapacity() to modify capacity
     * @see getCurrentSize() to get current memory usage
     * @note Capacity limits the number of concurrent memory traces
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * @brief Set the maximum capacity of working memory
     * 
     * @param cap New maximum capacity
     * @post Capacity updated, may discard excess traces if over capacity
     * @pre cap must be greater than 0
     * @see getCapacity() to retrieve current capacity
     * @see getCurrentSize() to check current usage
     */
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Decay rate for memory traces
    /**
     * @brief Get the decay rate for memory traces
     * 
     * @return float Decay rate per timestep (0.0 to 1.0)
     * @see setDecayRate() to modify decay rate
     * @see update() for decay application
     * @note Higher decay rates cause faster forgetting
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * @brief Set the decay rate for memory traces
     * 
     * @param rate New decay rate per timestep (0.0 to 1.0)
     * @post Decay rate updated
     * @pre rate must be within valid range [0.0, 1.0]
     * @see getDecayRate() to retrieve current decay rate
     * @see update() for decay application
     */
    void setDecayRate(float rate) { decayRate_ = rate; }

    // Get neurons currently in working memory
    /**
     * @brief Get all neurons currently storing memory information
     * 
     * @return const std::vector<NeuronId>& Vector of neuron IDs in working memory
     * @post Returns reference to internal memory neuron list
     * @see storeToNeuron() to add neurons
     * @see getNeuronActivation() for specific neuron activation
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    // Strengthen working memory representation (for rehearsal)
    /**
     * @brief Strengthen memory traces for better retention
     * 
     * @param factor Multiplication factor for trace strengths (typically > 1.0)
     * @post Memory trace strengths increased
     * @pre factor must be positive
     * @note Used during rehearsal to consolidate memories
     * @see update() to apply strengthening over time
     */
    void strengthenMemory(float factor);

    // Competition between memory traces
    /**
     * @brief Run competition between memory traces to resolve interference
     * 
     * @post Competition results in selection of winning traces
     * @see update() for when competition is run
     * @see isWinning() to check competition results
     * @see winners_ to access winning neurons
     */
    void runCompetition();

    // Is this neuron part of the winning population?
    /**
     * @brief Check if a neuron is part of the winning memory trace
     * 
     * @param neuron Neuron identifier to check
     * @return bool true if neuron won the competition
     * @post Does not modify memory state
     * @see runCompetition() to run competition
     * @see getWinners() to get all winning neurons
     */
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    /**
     * @brief Get the overall activity level of working memory
     * 
     * @return float Combined activity level (0.0 to 1.0)
     * @post Calculated from all active memory traces
     * @see getActiveTraces() to get number of traces
     * @see update() to update activity level
     */
    float getMemoryActivity() const;

private:
    // Create recurrent connection for maintenance
    /**
     * @brief Create a recurrent connection between neurons for memory maintenance
     * 
     * @param from Source neuron identifier
     * @param to Target neuron identifier  
     * @param strength Initial connection strength (0.0 to 1.0)
     * @post Recurrent connection created and added
     * @pre Both neurons must be valid and belong to brain system
     * @pre strength must be within valid range [0.0, 1.0]
     * 
     * @note Forms the basis of persistent activity
     * @see updateRecurrentConnections() to update all connections
     */
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    /**
     * @brief Update all recurrent connections for memory maintenance
     * 
     * @post Recurrent connections strengthened based on activity
     * @note Implements synaptic facilitation for trace maintenance
     * @see createRecurrentConnection() to create new connections
     * @see update() for when this is called
     */
    void updateRecurrentConnections();

    // Decay weak memory traces
    /**
     * @brief Decay weak memory traces below threshold
     * 
     * @post Weak traces removed, strong traces maintained
     * @note Implements resource competition and selective retention
     * @see update() to run this decay process
     * @see activeTraces_ for trace management
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
    /**
     * @brief Initialize attentional selection with brain system reference
     * 
     * @param brain Pointer to the brain system for neural integration
     * @post Attentional selection system initialized
     * @pre Brain pointer must be valid and initialized
     * @pre Brain system must have neural populations
     */
    void initialize(Brain* brain);

    // Process competing sensory inputs and select winners
    // Returns IDs of winning neural populations
    /**
     * @brief Process competing inputs and select attentional winners
     * 
     * @param competitors Vector of neuron IDs competing for attention
     * @param globalInhibition Level of global inhibition (0.0 to 1.0)
     * @return std::vector<NeuronId> Vector of winning neuron IDs
     * @post Competition results in winner selection
     * @pre Brain system must be initialized
     * @pre competitors must contain valid neuron IDs
     * @pre globalInhibition must be within valid range [0.0, 1.0]
     * 
     * @note Implements winner-take-all competition with lateral inhibition
     * @note Global inhibition controls overall excitability
     * @see focusOnRegion() to apply top-down attention
     * @see releaseAttention() to disable competition
     */
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);

    // Apply attention to a specific region (focus processing there)
    /**
     * @brief Apply top-down attention to focus processing on a specific region
     * 
     * @param region Region identifier to attend to
     * @post Region marked as attentional focus
     * @pre Brain system must be initialized
     * @pre region must be a valid region ID
     * 
     * @note Enhances processing in attended region
     * @see releaseAttention() to disable attentional focus
     * @see focusOnRegion() to focus on specific regions
     */
    void focusOnRegion(RegionId region);

    // Release attention (allow all regions equal processing)
    /**
     * @brief Release attentional focus, allowing equal processing across regions
     * 
     * @post All regions processed equally, no attentional bias
     * @see focusOnRegion() to apply attentional focus
     * @see initialize() to reset attention system
     */
    void releaseAttention();

    // Get current attention focus
    /**
     * @brief Get the regions currently receiving attentional focus
     * 
     * @return std::vector<RegionId> Vector of attended region IDs
     * @post Returns copy of attended regions
     * @see focusOnRegion() to set attentional focus
     * @see releaseAttention() to clear focus
     */
    std::vector<RegionId> getAttendedRegions() const;

    // Set attention parameters
    /**
     * @brief Set inhibition strength for attentional competition
     * 
     * @param strength New inhibition strength (0.0 to 1.0)
     * @post Inhibition strength updated
     * @pre strength must be within valid range [0.0, 1.0]
     * @see setExcitationStrength() to set excitation
     * @see setCompetitionThreshold() to set threshold
     */
    void setInhibitionStrength(float strength);
    
    /**
     * @brief Set excitation strength for attentional competition
     * 
     * @param strength New excitation strength (0.0 to 1.0)
     * @post Excitation strength updated
     * @pre strength must be within valid range [0.0, 1.0]
     * @see setInhibitionStrength() to set inhibition
     * @see setCompetitionThreshold() to set threshold
     */
    void setExcitationStrength(float strength);
    
    /**
     * @brief Set competition threshold for attentional selection
     * 
     * @param threshold New competition threshold (0.0 to 1.0)
     * @post Competition threshold updated
     * @pre threshold must be within valid range [0.0, 1.0]
     * @see setInhibitionStrength() to set inhibition
     * @see setExcitationStrength() to set excitation
     */
    void setCompetitionThreshold(float threshold);

    // Get inhibition level for a neuron
    /**
     * @brief Get the inhibition level applied to a specific neuron
     * 
     * @param neuron Neuron identifier to query
     * @return float Current inhibition level (0.0 to 1.0)
     * @post Does not modify attention state
     * @pre Brain system must be initialized
     * @pre neuron must be a valid neuron ID
     * 
     * @note Inhibition level affects neural excitability
     * @see setInhibitionStrength() to set global inhibition
     */
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron
    /**
     * @brief Get the excitation level applied to a specific neuron
     * 
     * @param neuron Neuron identifier to query
     * @return float Current excitation level (0.0 to 1.0)
     * @post Does not modify attention state
     * @pre Brain system must be initialized
     * @pre neuron must be a valid neuron ID
     * 
     * @note Excitation level affects neural activation
     * @see setExcitationStrength() to set global excitation
     */
    float getExcitationFor(NeuronId neuron) const;

    // Update attention dynamics
    /**
     * @brief Update attentional dynamics for one simulation timestep
     * 
     * @param dt Time step duration (seconds)
     * @post Attention weights and dynamics updated
     * @pre Brain system must be initialized
     * @pre dt must be positive and valid
     * 
     * @note Implements dynamic attentional modulation
     * @see processCompetition() to run competition
     * @see update() for when this is called
     */
    void update(TimestepDuration dt);

    // Is this neuron currently attended?
    /**
     * @brief Check if a neuron is currently receiving attentional focus
     * 
     * @param neuron Neuron identifier to check
     * @return bool true if neuron is attended
     * @post Does not modify attention state
     * @pre Brain system must be initialized
     * @pre neuron must be a valid neuron ID
     * 
     * @note Attended neurons receive enhanced processing
     * @see processCompetition() to select attended neurons
     * @see focusOnRegion() to set attentional focus
     */
    bool isAttended(NeuronId neuron) const;

    // Get attention winners
    /**
     * @brief Get the neurons that won the attentional competition
     * 
     * @return const std::vector<NeuronId>& Vector of winning neuron IDs
     * @post Returns reference to internal winners list
     * @see processCompetition() to run competition
     * @see isAttended() to check individual neuron attention
     */
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Top-down attention (goal-directed)
    /**
     * @brief Apply top-down attentional bias to a specific neuron
     * 
     * @param neuron Neuron identifier to bias
     * @param biasStrength Strength of bias (0.0 to 1.0)
     * @post Neuron's attentional bias updated
     * @pre Brain system must be initialized
     * @pre neuron must be a valid neuron ID
     * @pre biasStrength must be within valid range [0.0, 1.0]
     * 
     * @note Represents goal-directed attentional modulation
     * @see applyBottomUpSalience() for stimulus-driven attention
     * @see topDownBias_ for stored biases
     */
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Bottom-up attention (stimulus-driven)
    /**
     * @brief Apply bottom-up attentional salience to a specific neuron
     * 
     * @param neuron Neuron identifier to bias
     * @param salienceStrength Strength of salience (0.0 to 1.0)
     * @post Neuron's attentional salience updated
     * @pre Brain system must be initialized
     * @pre neuron must be a valid neuron ID
     * @pre salienceStrength must be within valid range [0.0, 1.0]
     * 
     * @note Represents stimulus-driven attentional modulation
     * @see applyTopDownBias() for goal-directed attention
     * @see bottomUpSalience_ for stored salience
     */
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    // Reset attention
    /**
     * @brief Reset attentional system to initial state
     * 
     * @post All attention biases and salience cleared
     * @see initialize() to re-initialize system
     * @see clear() for memory clearing
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
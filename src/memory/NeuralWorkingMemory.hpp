#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

/**
 * @file NeuralWorkingMemory.hpp
 * 
 * Neural working memory and attentional selection implementations for NLM
 * 
 * This file provides neural implementations of working memory and attentional
 * selection systems in NLM. Unlike the high-level memory interfaces, these
 * are implemented using actual neural dynamics and mechanisms:
 * - NeuralWorkingMemory: Uses persistent neural activity and recurrent connections
 * - AttentionalSelection: Implements neural attention through competition
 * 
 * These neural implementations bridge the gap between high-level memory
 * concepts and actual neural computation, providing biologically realistic
 * models of cognitive function.
 * 
 * Key features:
 * - NeuralWorkingMemory: Persistent activity, recurrent connections, competition
 * - AttentionalSelection: Neural competition, lateral inhibition, gain modulation
 * - Integration with brain architecture
 * - Real neural dynamics and computation
 * - Neural competition and selection mechanisms
 * 
 * @note These are the neural implementations of the high-level memory interfaces
 * @see Memory.hpp - For high-level memory interfaces
 * @see Brain.hpp - For brain-level integration
 * @see Config.hpp - For configuration parameters
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class NeuralWorkingMemory
 * 
 * Neural implementation of working memory using persistent neural activity.
 * 
 * This class implements working memory using actual neural dynamics, not as a
 * simple storage vector. It uses several key neural mechanisms:
 * - Persistent neural activity (reverberating activity)
 * - Recurrent synaptic connections for maintaining information
 * - Activity-dependent short-term plasticity
 * - Neural competition for selective retention
 * 
 * Working memory in NLM is implemented as a distributed neural phenomenon
 * where information is maintained through ongoing neural activity patterns.
 * This is more biologically realistic than simple storage-and-retrieve models
 * and enables complex interactions between working memory and other neural systems.
 * 
 * NeuralWorkingMemory mechanisms:
 * - **Persistent activity**: Neurons maintain firing to represent information
 * - **Recurrent connections**: Self-exciting connections maintain activity
 * - **Competition**: Neural populations compete for representation dominance
 * - **Plasticity**: Synaptic weights adjust based on activity
 * - **Rehearsal**: Information is rehearsed to maintain it
 * 
 * @note This is the neural implementation of the high-level WorkingMemory interface
 * @see Memory.hpp::WorkingMemory - High-level interface
 * @see Brain.hpp - For brain-level integration
 * @see Config.hpp - For configuration parameters
 * 
 * @author NLM Development Team
 */
class NeuralWorkingMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new neural working memory instance with default parameters.
     * The neural working memory is initialized with default capacity and
     * decay characteristics for neural activity maintenance.
     * 
     * @post Neural working memory created with default settings
     * @post Ready for neural activity storage and maintenance
     * 
     * @see initialize() - For neural initialization
     * @see setCapacity() - For setting capacity
     * @see setDecayRate() - For setting decay rate
     */
    NeuralWorkingMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up neural working memory resources and performs any necessary
     * cleanup operations. The neural working memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Neural working memory cannot be used after destruction
     */
    ~NeuralWorkingMemory();
    
    // Disable copying - PImpl idiom
    NeuralWorkingMemory(const NeuralWorkingMemory&) = delete;
    NeuralWorkingMemory& operator=(const NeuralWorkingMemory&) = delete;
    NeuralWorkingMemory(NeuralWorkingMemory&&) noexcept;
    NeuralWorkingMemory& operator=(NeuralWorkingMemory&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the neural working memory with a reference to the brain.
     * This is essential for neural connectivity and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and populations
     * - Establishing recurrent connections
     * - Integrating with brain dynamics
     * - Coordinating with other memory systems
     * 
     * @param brain Pointer to brain instance for neural integration
     * 
     * @pre brain must be valid and initialized
     * @post Neural working memory initialized with brain reference
     * @post Ready for neural activity storage and maintenance
     * @post Neural connectivity established
     * 
     * @note Brain reference is required for neural operations
     * @see Brain.hpp - For brain class
     * @see store() - For storing neural activity
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Store information in working memory
     * 
     * Stores information in neural working memory by encoding it as a neural
     * activity pattern. The information is distributed across neurons and
     * maintained through persistent activity. This is the neural implementation
     * of working memory storage that:
     * - Encodes information as neural activation patterns
     * - Distributes information across neural populations
     * - Maintains information through recurrent connections
     * - Enables pattern completion and reconstruction
     * 
     * @param pattern Neural activation pattern representing the information
     * @param strength Strength of the memory trace (default: 1.0f)
     * 
     * @pre brain must be initialized
     * @pre pattern must be valid and non-empty
     * @post Information stored as neural activity pattern
     * @post Memory trace established and maintained
     * @post Recurrent connections created for persistence
     * 
     * @note Information is encoded as distributed neural activity
     * @see retrieve() - For retrieving neural activity patterns
     * @see storeToNeuron() - For individual neuron storage
     * @see strengthenMemory() - For memory strengthening
     */
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    
    /**
     * @brief Store specific neuron activation
     * 
     * Stores activation directly in specific neurons for fine-grained
     * control over working memory. This is useful for:
     * - Direct neuron manipulation
     * - Targeted memory storage
     * - Testing individual neurons
     * - Experimental control
     * 
     * @param neuron Neuron ID to store activation in
     * @param activation Activation level to store
     * 
     * @pre brain must be initialized
     * @pre neuron must be valid
     * @post Neuron activation stored directly
     * @post Working memory state updated
     * 
     * @see store() - For pattern-based storage
     * @see getNeuronActivation() - For retrieving activation
     */
    void storeToNeuron(NeuronId neuron, float activation);
    
    /**
     * @brief Retrieve current working memory content as activity levels
     * 
     * Retrieves the current working memory content as neural activity levels
     * across all active memory traces. This is essential for:
     * - Accessing current memory representations
     * - Supporting ongoing cognitive processes
     * - Monitoring memory content
     * - Implementing memory-dependent operations
     * 
     * @return Neural activity pattern representing current memory content
     * 
     * @post Current memory content retrieved
     * @post Neural activity pattern reconstructed
     * @post Memory traces read out
     * 
     * @return Neural activation pattern
     * @retval pattern Current working memory neural activity
     * @retval empty If no memory content
     * 
     * @note Returns distributed neural activity pattern
     * @see store() - For storing neural activity
     * @see getMemoryActivity() - For overall memory activity
     */
    std::vector<float> retrieve() const;
    
    /**
     * @brief Check if specific neuron is part of working memory
     * 
     * Determines whether a specific neuron is part of the current working
     * memory content. This is useful for:
     * - Checking memory membership
     * - Implementing memory-dependent operations
     * - Managing memory access
     * - Error handling
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron is part of working memory, false otherwise
     * 
     * @pre brain must be initialized
     * @pre neuron must be valid
     * @post Return value indicates memory membership
     * @complexity O(1) for lookup
     * 
     * @see storeToNeuron() - For storing neuron activation
     * @see retrieve() - For retrieving memory content
     */
    bool contains(NeuronId neuron) const;
    
    /**
     * @brief Get activation level of a specific neuron in working memory
     * 
     * Returns the activation level of a specific neuron in working memory.
     * This is essential for:
     * - Accessing individual memory representations
     * - Monitoring memory content
     * - Implementing memory-dependent operations
     * - Memory analysis
     * 
     * @param neuron Neuron ID to query
     * @return Activation level of neuron in working memory
     * 
     * @pre brain must be initialized
     * @pre neuron must be valid
     * @post Return value stable during operation
     * @complexity O(1) for lookup
     * 
     * @note Returns 0.0f if neuron not in working memory
     * @see storeToNeuron() - For storing neuron activation
     * @see retrieve() - For retrieving memory content
     */
    float getNeuronActivation(NeuronId neuron) const;
    
    /**
     * @brief Update working memory (maintenance and decay)
     * 
     * Updates the neural working memory by maintaining and decaying memory
     * traces. This is essential for:
     * - Maintaining persistent neural activity
     * - Implementing forgetting mechanisms
     * - Updating recurrent connections
     * - Managing memory dynamics
     * 
     * @param dt Time step duration
     * 
     * @pre brain must be initialized
     * @pre dt must be positive
     * @post Working memory updated for new timestep
     * @post Recurrent connections updated
     * @post Memory traces maintained or decayed
     * 
     * @note This is called once per simulation timestep
     * @see store() - For storing information
     * @see decay() - For memory decay
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Clear working memory
     * 
     * Clears all working memory content, removing all neural activity
     * patterns. This is useful for:
     * - Resetting working memory
     * - Implementing memory-dependent tasks
     * - Managing working memory state
     * - Error recovery
     * 
     * @post All working memory cleared
     * @post Neural activity patterns removed
     * @post Recurrent connections reset
     * @post Working memory ready for new operations
     * 
     * @see store() - For storing information
     * @see retrieve() - For retrieving information
     */
    void clear();
    
    /**
     * @brief Get number of active memory traces
     * 
     * Returns the number of active memory traces currently stored in working
     * memory. This is an important statistic for understanding memory load
     * and dynamics.
     * 
     * @return Number of active memory traces
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     * 
     * @note Each trace represents a stored information pattern
     * @see getMemoryNeurons() - For getting memory neurons
     */
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    /**
     * @brief Get working memory capacity
     * 
     * Returns the maximum capacity of working memory in terms of number
     * of memory traces. This is a key statistic for understanding memory
     * limitations and constraints.
     * 
     * @return Maximum working memory capacity
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     * 
     * @note Capacity affects memory dynamics
     * @see setCapacity() - For setting capacity
     * @see getCurrentSize() - For current usage
     */
    size_t getCapacity() const { return capacity_; }
    
    /**
     * @brief Set working memory capacity
     * 
     * Sets the working memory capacity in terms of number of memory traces.
     * This is useful for:
     * - Configuring memory limits
     * - Experimental control
     * - Memory management
     * - Testing memory constraints
     * 
     * @param cap New working memory capacity
     * 
     * @pre cap must be positive
     * @post Working memory capacity updated
     * @post Memory limits enforced
     */
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    /**
     * @brief Get decay rate for memory traces
     * 
     * Returns the decay rate for memory traces. This is an important parameter
     * for understanding memory dynamics and stability.
     * 
     * @return Decay rate (0.0 to 1.0)
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     * 
     * @note Higher decay rate means faster forgetting
     * @see setDecayRate() - For setting decay rate
     */
    float getDecayRate() const { return decayRate_; }
    
    /**
     * @brief Set decay rate for memory traces
     * 
     * Sets the decay rate for memory traces. This is useful for:
     * - Configuring memory dynamics
     * - Experimental control
     * @param rate New decay rate (0.0 to 1.0)
     * 
     * @pre 0.0 <= rate <= 1.0
     * @post Decay rate updated
     * @post Memory dynamics changed
     */
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    /**
     * @brief Get neurons currently in working memory
     * 
     * Returns a vector of neurons currently holding memory representations.
     * This is essential for:
     * - Understanding memory content
     * - Analyzing memory dynamics
     * - Implementing memory-dependent operations
     * - Memory analysis
     * 
     * @return Vector of neurons in working memory
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     * 
     * @note Each neuron represents an active memory trace
     * @see getActiveTraces() - For number of active traces
     */
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    /**
     * @brief Strengthen working memory representation (for rehearsal)
     * 
     * Strengthens working memory representations by increasing activation
     * levels and reinforcing recurrent connections. This is essential for:
     * - Rehearsing information
     * - Maintaining memory stability
     * - Improving memory retention
     * - Implementing working memory rehearsal
     * 
     * @param factor Strength factor (typically > 1.0 for strengthening)
     * 
     * @post Memory representations strengthened
     * @post Recurrent connections reinforced
     * @post Memory stability improved
     */
    void strengthenMemory(float factor);
    
    /**
     * @brief Competition between memory traces
     * 
     * Runs competition between memory traces to determine which representations
     * are dominant. This is essential for:
     * - Selecting memory representations
     * - Implementing neural competition
     * - Managing memory content
     * - Memory selection
     * 
     * @post Competition run
     * @post Winning traces selected
     * @post Losing traces suppressed
     */
    void runCompetition();
    
    /**
     * @brief Is this neuron part of the winning population?
     * 
     * Determines whether a specific neuron is part of the winning memory
     * population. This is essential for:
     * - Understanding memory selection
     * - Implementing memory-based decisions
     * - Memory competition analysis
     * - Memory selection mechanisms
     * 
     * @param neuron Neuron ID to check
     * @return true if neuron is part of winning population, false otherwise
     * 
     * @post Return value indicates winning status
     * @complexity O(1) for lookup
     */
    bool isWinning(NeuronId neuron) const;
    
    /**
     * @brief Get overall memory activity level
     * 
     * Returns the overall activity level of working memory, representing the
     * total strength of all memory traces. This is an important statistic
     * for understanding memory dynamics and load.
     * 
     * @return Overall memory activity level (0.0 to 1.0)
     * 
     * @post Return value stable during operation
     * @complexity O(n) where n is number of neurons
     */
    float getMemoryActivity() const;
    
private:
    /**
     * @brief Create recurrent connection for maintenance
     * 
     * Creates a recurrent connection between two neurons to maintain
     * persistent activity. This is essential for:
     * - Implementing working memory persistence
     * - Maintaining neural representations
     * - Establishing recurrent loops
     * - Supporting memory maintenance
     * 
     * @param from Source neuron ID
     * @param to Target neuron ID
     * @param strength Connection strength
     */
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);
    
    /**
     * @brief Update recurrent connections for maintenance
     * 
     * Updates recurrent connections to maintain persistent activity and
     * strengthen memory traces. This is essential for:
     * - Maintaining working memory
     * - Updating connection strengths
     * - Supporting memory persistence
     * - Managing recurrent dynamics
     */
    void updateRecurrentConnections();
    
    /**
     * @brief Decay weak memory traces
     * 
     * Decays weak memory traces to remove low-strength representations.
     * This is essential for:
     * - Managing memory capacity
     * - Forgetting mechanisms
     * - Memory optimization
     * - Selecting dominant memories
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

/**
 * @class AttentionalSelection
 * 
 * Neural implementation of attentional selection through competition.
 * 
 * This class implements attentional selection using neural competition
 * mechanisms. Unlike Transformer-style attention, it uses biological
 * neural mechanisms including:
 * - Competition between neural populations
 * - Salience-based selection
 * - Recurrent amplification of winners
 * - Inhibitory surround suppression
 * 
 * AttentionalSelection in NLM implements:
 * - Neural competition for processing resources
 * - Salience-based selection of stimuli
 * - Recurrent amplification of selected representations
 * - Inhibitory mechanisms for selection focus
 * - Bottom-up and top-down attentional control
 * 
 * Key neural mechanisms include:
 * - **Lateral inhibition**: Competition between neural populations
 * - **Salience**: Detection of stimulus relevance
 * - **Amplification**: Recurrent connections for winner-take-all
 * - **Suppression**: Inhibitory connections for selection focus
 * 
 * @note This is the neural implementation of attentional selection
 * @see Brain.hpp - For brain-level integration
 * @see Config.hpp - For configuration parameters
 * 
 * @author NLM Development Team
 */
class AttentionalSelection {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new attentional selection instance with default parameters.
     * The attentional selection is initialized with default parameters
     * for neural competition and selection.
     * 
     * @post Attentional selection created with default settings
     * @post Ready for neural competition and selection
     */
    AttentionalSelection();
    
    /**
     * @brief Destructor
     * 
     * Cleans up attentional selection resources and performs any necessary
     * cleanup operations. The attentional selection is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Attentional selection cannot be used after destruction
     */
    ~AttentionalSelection();
    
    // Disable copying - PImpl idiom
    AttentionalSelection(const AttentionalSelection&) = delete;
    AttentionalSelection& operator=(const AttentionalSelection&) = delete;
    AttentionalSelection(AttentionalSelection&&) noexcept;
    AttentionalSelection& operator=(AttentionalSelection&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the attentional selection with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neural populations
     * - Implementing competition
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     * 
     * @param brain Pointer to brain instance for neural integration
     * 
     * @pre brain must be valid and initialized
     * @post Attentional selection initialized with brain reference
     * @post Ready for neural competition and selection
     * @post Neural integration established
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Process competing sensory inputs and select winners
     * 
     * Processes competing sensory inputs and selects winning neural
     * populations. This implements neural attentional selection through:
     * - Competition between neural populations
     * - Salience-based ranking
     * - Recurrent amplification
     * - Inhibitory suppression
     * 
     * @param competitors Vector of competing neuron IDs
     * @param globalInhibition Global inhibition level (default: 0.5f)
     * @return Vector of winning neuron IDs
     * 
     * @pre brain must be initialized
     * @pre competitors must be valid
     * @post Competition processed
     * @post Winning neurons selected
     * @post Selection mechanism updated
     * 
     * @return Winning neurons
     * @retval winners Vector of selected winners
     * @retval empty If no winners
     * 
     * @note Implements neural attentional selection
     * @see getWinners() - For getting selected winners
     * @see processCompetition() - For competition processing
     */
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);
    
    /**
     * @brief Apply attention to a specific region
     * 
     * Applies attention to a specific neural region, focusing processing
     * resources on that region. This is used for:
     * - Target-focused attention
     * - Region-specific processing
     * - Selective attention
     * - Goal-directed attention
     * 
     * @param region Region ID to focus attention on
     * 
     * @pre brain must be initialized
     * @pre region must be valid
     * @post Attention focused on region
     * @post Processing resources allocated
     */
    void focusOnRegion(RegionId region);
    
    /**
     * @brief Release attention
     * 
     * Releases attention, allowing all regions equal processing resources.
     * This is used for:
     * - Distraction from focus
     * - Equal resource allocation
     * - Attentional release
     * - Reset attention state
     * 
     * @post Attention released
     * @post Resources reallocated equally
     */
    void releaseAttention();
    
    /**
     * @brief Get current attention focus
     * 
     * Returns the current attention focus, representing which regions
     * are currently being attended to. This is essential for:
     * - Understanding attentional state
     * - Monitoring attentional resources
     * - Implementing attentional-dependent operations
     * - Attention analysis
     * 
     * @return Vector of attended region IDs
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     */
    std::vector<RegionId> getAttendedRegions() const;
    
    /**
     * @brief Set attention parameters
     * 
     * Sets attentional parameters that control competition and selection.
     * This is used for:
     * - Configuring attentional mechanisms
     * - Experimental control
     * - Tuning attention parameters
     * - Testing attention dynamics
     */
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);
    
    /**
     * @brief Get inhibition level for a neuron
     * 
     * Returns the inhibition level for a specific neuron. This is
     * essential for:
     * - Understanding attentional inhibition
     * - Monitoring attentional resources
     * - Implementing attentional-dependent operations
     * - Inhibition analysis
     */
    float getInhibitionFor(NeuronId neuron) const;
    
    /**
     * @brief Get excitation level for a neuron
     * 
     * Returns the excitation level for a specific neuron. This is
     * essential for:
     * - Understanding attentional excitation
     * - Monitoring attentional resources
     * - Implementing attentional-dependent operations
     * - Excitation analysis
     */
    float getExcitationFor(NeuronId neuron) const;
    
    /**
     * @brief Update attention dynamics
     * 
     * Updates attentional dynamics, managing competition, salience,
     * and selection. This is essential for:
     * - Maintaining attentional state
     * - Updating attentional resources
     * - Managing attentional competition
     * - Implementing attentional updates
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Check if neuron is currently attended
     * 
     * Determines whether a specific neuron is currently being attended to.
     * This is essential for:
     * - Understanding attentional state
     * - Monitoring attentional resources
     * - Implementing attentional-dependent operations
     * - Attentional analysis
     */
    bool isAttended(NeuronId neuron) const;
    
    /**
     * @brief Get attention winners
     * 
     * Returns the winning neurons selected by attentional competition.
     * This is essential for:
     * - Understanding attentional selection
     * - Monitoring attentional resources
     * - Implementing attentional-dependent operations
     * - Selection analysis
     */
    const std::vector<NeuronId>& getWinners() const { return winners_; }
    
    /**
     * @brief Apply top-down attention (goal-directed)
     * 
     * Applies top-down attention bias to a specific neuron, implementing
     * goal-directed attentional control. This is used for:
     * - Goal-directed attention
     * - Task-based attentional control
     * - Top-down attentional modulation
     * - Intentional attention
     */
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    
    /**
     * @brief Apply bottom-up attention (stimulus-driven)
     * 
     * Applies bottom-up attention salience to a specific neuron, implementing
     * stimulus-driven attentional control. This is used for:
     * - Stimulus-driven attention
     * - Salient stimulus detection
     * - Bottom-up attentional modulation
     * - Salient attention
     */
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
    /**
     * @brief Reset attention
     * 
     * Resets attentional state, clearing all attentional biases and
     * settings. This is useful for:
     * - Resetting attentional state
     * - Managing attentional resources
     * - Error recovery
     * - Attentional reset
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

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>

namespace nlm {

/**
 * @file NeuralEpisodicMemory.hpp
 * 
 * Neural episodic and associative memory implementations for NLM
 * 
 * This file provides neural implementations of episodic and associative
 * memory systems in NLM. Unlike the high-level memory interfaces, these
 * are implemented using actual neural dynamics and mechanisms:
 * - NeuralEpisodicMemory: Stores experiences as neural activity patterns
 * - NeuralAssociativeMemory: Creates and retrieves relationships between patterns
 * 
 * These neural implementations bridge the gap between high-level memory
 * concepts and actual neural computation, providing biologically realistic
 * models of memory and learning.
 * 
 * Key features:
 * - NeuralEpisodicMemory: Experience storage, retrieval, replay, consolidation
 * - NeuralAssociativeMemory: Pattern association, spread activation, similarity
 * - Integration with brain architecture
 * - Real neural dynamics and computation
 * - Memory consolidation and replay mechanisms
 * 
 * @note These are the neural implementations of the high-level episodic and
 * associative memory interfaces
 * @see Memory.hpp - For high-level episodic and associative memory interfaces
 * @see Brain.hpp - For brain-level integration
 * @see Config.hpp - For configuration parameters
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @struct EpisodicMemoryItem
 * 
 * Complete episodic memory item representing a stored experience.
 * 
 * This structure encapsulates a complete episodic memory item, representing
 * a single experience in the NLM system. The episode is encoded as neural
 * activity patterns and associated contextual information. This is the
 * neural representation of episodic memory that:
 * - Stores sensory state at time of experience
 * - Records location and orientation
 * - Captures actions taken
 * - Tracks internal states (reward, energy, novelty)
 * - Stores consequences and neural activity
 * 
 * Key features:
 * - **Experience recording**: Stores what happened and when
 * - **Neural encoding**: Encodes experiences as activity patterns
 * - **Contextual richness**: Includes location, action, state
 * - **Temporal tracking**: Maintains episode timing and age
 * - **Consequence tracking**: Stores action outcomes
 * 
 * @note This is the neural representation of episodic memory
 * @see NeuralEpisodicMemory - For episodic memory management
 * @see Config.hpp - For configuration parameters
 * 
 * @author NLM Development Team
 */
struct EpisodicMemoryItem {
    /** Timestamp when the experience occurred */
    SimulationStep timestamp;
    
    /** Sensory state at time of experience (encoded as neural activity pattern) */
    std::vector<float> sensoryState;
    
    /** Location information (x, y coordinates and orientation) */
    float positionX;
    float positionY;
    float orientation;
    
    /** Action taken (if any) */
    ActionType action;
    
    /** Internal state information */
    float reward;
    float energy;
    float novelty;
    
    /** Consequences of the action */
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    /** Associated neural activity pattern (which neurons were firing) */
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    /** Time since this episode (for aging and consolidation) */
    SimulationStep age;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty episodic memory item with default values.
     * This represents an uninitialized or empty memory entry with
     * default values for all fields.
     * 
     * @post Item created with default values
     * @post Timestamp set to 0
     * @post All other fields initialized to defaults
     * 
     * @see EpisodicMemoryItem(SimulationStep, std::vector<float>, float, float, float, ActionType, float, float, float, std::vector<float>, float, std::vector<NeuronId>, std::vector<float>, SimulationStep) - Full constructor
     */
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0) {}
};

/**
 * @class NeuralEpisodicMemory
 * 
 * Neural episodic memory implementation storing experiences as neural patterns.
 * 
 * This class implements episodic memory using actual neural dynamics. Instead
 * of storing textual descriptions, it stores experiences as neural activity
 * patterns that can be retrieved through pattern completion. This provides:
 * - **Experience storage**: Records what happened and when
 * - **Neural encoding**: Encodes experiences as distributed neural patterns
 * - **Pattern completion**: Retrieves similar experiences through neural patterns
 * - **Temporal ordering**: Maintains chronological sequence of events
 * - **Replay mechanisms**: Reactivates experiences for consolidation
 * 
 * NeuralEpisodicMemory mechanisms:
 * - **Storage**: Experiences stored as neural activity patterns
 * - **Retrieval**: Pattern completion and similarity matching
 * - **Replay**: Reactivation of stored neural patterns
 * - **Consolidation**: Integration and strengthening of memories
 * - **Aging**: Tracking and management of episode age
 * 
 * Key features:
 * - **Neural representation**: Experiences stored as neural patterns
 * - **Pattern completion**: Retrieve similar experiences
 * - **Temporal ordering**: Maintain chronological sequence
 * - **Replay**: Reactivate experiences for consolidation
 * - **Relevance tracking**: Manage episode importance
 * 
 * @note This is the neural implementation of the high-level EpisodicMemory interface
 * @see Memory.hpp::EpisodicMemory - High-level episodic memory interface
 * @see NeuralAssociativeMemory - For associative memory implementation
 * @see Brain.hpp - For brain-level integration
 * 
 * @author NLM Development Team
 */
class NeuralEpisodicMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new neural episodic memory instance with default parameters.
     * The neural episodic memory is initialized with default capacity and
     * replay settings for neural activity storage and retrieval.
     * 
     * @post Neural episodic memory created with default settings
     * @post Ready for experience storage and retrieval
     * 
     * @see initialize() - For neural initialization
     * @see setMaxEpisodes() - For setting capacity
     */
    NeuralEpisodicMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up neural episodic memory resources and performs any necessary
     * cleanup operations. The neural episodic memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Neural episodic memory cannot be used after destruction
     */
    ~NeuralEpisodicMemory();
    
    // Disable copying - PImpl idiom
    NeuralEpisodicMemory(const NeuralEpisodicMemory&) = delete;
    NeuralEpisodicMemory& operator=(const NeuralEpisodicMemory&) = delete;
    NeuralEpisodicMemory(NeuralEpisodicMemory&&) noexcept;
    NeuralEpisodicMemory& operator=(NeuralEpisodicMemory&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the neural episodic memory with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other memory systems
     * 
     * @param brain Pointer to brain instance for neural integration
     * 
     * @pre brain must be valid and initialized
     * @post Neural episodic memory initialized with brain reference
     * @post Ready for experience storage and retrieval
     * @post Neural integration established
     * 
     * @note Brain reference is required for neural operations
     * @see Brain.hpp - For brain class
     * @see storeEpisode() - For storing experiences
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Store a new experience
     * 
     * Stores a new experience in neural episodic memory by encoding it as a
     * neural activity pattern. The experience is stored with full contextual
     * information including sensory state, location, action, and consequences.
     * This is the neural implementation of episodic memory storage that:
     * - Encodes experiences as neural activity patterns
     * - Records temporal and spatial context
     * - Captures action and outcome information
     * - Maintains neural activity patterns
     * - Enables future retrieval through pattern completion
     * 
     * @param episode Complete episodic memory item to store
     * 
     * @pre brain must be initialized
     * @pre episode must be valid
     * @post Experience stored as neural activity pattern
     * @post Temporal and spatial context recorded
     * @post Neural activity pattern established
     * 
     * @note Experiences are stored as distributed neural patterns
     * @see retrieveSimilar() - For retrieving similar experiences
     * @see retrieveTemporal() - For temporal retrieval
     * @see replayEpisode() - For replaying experiences
     */
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    /**
     * @brief Retrieve episodes similar to a query pattern
     * 
     * Retrieves episodes similar to a query neural activity pattern.
     * This implements pattern completion where similar neural patterns
     * retrieve related experiences. This is essential for:
     * - Retrieving related experiences
     * - Pattern completion and reconstruction
     * - Memory-based reasoning
     * - Experience-based processing
     * 
     * @param sensoryPattern Query neural activity pattern
     * @param maxResults Maximum number of results to return
     * @return Vector of similar episode pointers
     * 
     * @pre brain must be initialized
     * @pre sensoryPattern must be valid
     * @post Similar episodes retrieved
     * @post Episodes ordered by relevance
     * @post Neural patterns matched
     * 
     * @return Similar episodes
     * @retval episodes Vector of episode pointers
     * @retval empty If no similar episodes
     * 
     * @note Uses pattern completion to find similar experiences
     * @see storeEpisode() - For storing experiences
     * @see computeSimilarity() - For similarity computation
     */
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           size_t maxResults = 5) const;
    
    /**
     * @brief Retrieve episodes from a specific time window
     * 
     * Retrieves episodes from a specific temporal window. This is used for:
     * - Temporal navigation and exploration
     * - Analyzing temporal sequences
     * - Time-based memory access
     * - Temporal reasoning
     * 
     * @param startTime Start of temporal window
     * @param endTime End of temporal window
     * @param maxResults Maximum number of results to return
     * @return Vector of episodes in time window
     * 
     * @pre brain must be initialized
     * @pre startTime <= endTime
     * @post Episodes in time window retrieved
     * @post Episodes ordered by timestamp
     * 
     * @return Episodes in temporal window
     * @retval episodes Vector of episode pointers
     * @retval empty If no episodes in window
     */
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                            SimulationStep endTime,
                                                            size_t maxResults = 10) const;
    
    /**
     * @brief Retrieve episodes by location
     * 
     * Retrieves episodes that occurred at specific location coordinates.
     * This is used for:
     * - Spatial memory access
     * - Location-based retrieval
     * - Spatial navigation
     * - Location-based reasoning
     * 
     * @param x X coordinate
     * @param y Y coordinate
     * @param radius Search radius
     * @param maxResults Maximum number of results to return
     * @return Vector of episodes near location
     * 
     * @pre brain must be initialized
     * @pre radius > 0
     * @post Episodes near location retrieved
     * @post Episodes ordered by distance
     * 
     * @return Episodes near location
     * @retval episodes Vector of episode pointers
     * @retval empty If no episodes near location
     */
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(float x, float y,
                                                             float radius,
                                                             size_t maxResults = 5) const;
    
    /**
     * @brief Retrieve episodes following a specific action
     * 
     * Retrieves episodes that followed a specific action. This is used for:
     * - Action-based memory access
     * - Sequence retrieval
     * - Action-based reasoning
     * - Temporal sequence analysis
     * 
     * @param action Action to search for
     * @param maxResults Maximum number of results to return
     * @return Vector of episodes after action
     * 
     * @pre brain must be initialized
     * @post Episodes after action retrieved
     * @post Episodes ordered by timestamp
     * 
     * @return Episodes after action
     * @retval episodes Vector of episode pointers
     * @retval empty If no episodes after action
     */
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(ActionType action,
                                                               size_t maxResults = 5) const;
    
    /**
     * @brief Replay an episode
     * 
     * Replays an episode by reactivating the neural patterns associated with it.
     * This allows the brain to "relive" the experience, which is essential for:
     * - Memory consolidation
     * - Learning reinforcement
     * - Experience reactivation
     * - Neural plasticity
     * 
     * @param episode Episode to replay
     * 
     * @pre brain must be initialized
     * @pre episode must be a valid episode pointer
     * @post Episode neural patterns reactivated
     * @post Brain state affected by replay
     * 
     * @note Replay is essential for memory consolidation
     * @see getEpisodesForReplay() - For getting episodes for replay
     * @see replaySequence() - For replaying multiple episodes
     */
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    /**
     * @brief Update episode relevance
     * 
     * Updates the relevance of an episode based on recent use and other factors.
     * This is essential for:
     * - Managing episode importance
     * - Prioritizing memory consolidation
     * - Memory relevance management
     * - Memory optimization
     */
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);
    
    /**
     * @brief Get episode by index
     * 
     * Returns the episode at the specified index. This is used for:
     * - Direct access to specific episodes
     * - Sequential processing
     * - Memory indexing
     * - Memory analysis
     */
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    
    /**
     * @brief Get total number of episodes
     * 
     * Returns the total number of episodes stored in neural episodic memory.
     * This is an important statistic for understanding memory capacity and load.
     * 
     * @return Total number of episodes
     * 
     * @post Return value stable during operation
     * @complexity O(1)
     * 
     * @see getRecentEpisodes() - For getting recent episodes
     */
    size_t getEpisodeCount() const { return episodes_.size(); }
    
    /**
     * @brief Get recent episodes
     * 
     * Returns recent episodes from neural episodic memory. This is useful for:
     * - Accessing recent history
     * - Analyzing recent experiences
     * - Recent memory access
     * - Recency-based retrieval
     */
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    /**
     * @brief Get average reward from episodes
     * 
     * Returns the average reward value across all stored episodes. This is
     * an important statistic for understanding memory content and quality.
     */
    float getAverageReward() const;
    
    /**
     * @brief Clear old episodes
     * 
     * Clears old episodes based on relevance threshold. This is used for:
     * - Managing memory capacity
     * - Memory consolidation
     * - Memory optimization
     * - Relevance-based cleanup
     */
    void consolidate(float relevanceThreshold);
    
    /**
     * @brief Clear all episodes
     * 
     * Clears all episodes from neural episodic memory, removing all stored
     * experiences. This is useful for:
     * - Resetting episodic memory
     * - Managing memory capacity
     * - Memory reset
     * - Error recovery
     */
    void clear();
    
    /**
     * @brief Enable/disable replay
     * 
     * Enables or disables episode replay. This is used for:
     * - Controlling replay behavior
     * - Managing replay resources
     * - Experimental control
     * - Replay optimization
     */
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    /**
     * @brief Get episodes for replay
     * 
     * Returns episodes selected for replay based on relevance and recency.
     * This is used for:
     * - Selecting episodes for replay
     * - Memory consolidation
     * - Replay optimization
     * - Sequence selection
     */
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    /**
     * @brief Replay multiple episodes
     * 
     * Replays multiple episodes for sleep-like consolidation. This is used for:
     * - Batch processing of episodes
     * - Sleep-like consolidation
     * - Multiple episode replay
     * - Consolidation sequences
     */
    void replaySequence(const std::vector<size_t>& episodeIds);
    
private:
    /**
     * @brief Compute similarity between query and stored episode
     * 
     * Computes the similarity between a query pattern and a stored episode.
     * This is essential for:
     * - Pattern completion
     * - Experience retrieval
     * - Similarity matching
     * - Memory analysis
     */
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;
    
    /**
     * @brief Check if episode matches criteria
     * 
     * Checks if an episode matches specified criteria. This is used for:
     * - Filtering episodes
     * - Retrieval criteria
     * - Memory selection
     * - Pattern matching
     */
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                        const std::vector<float>* sensoryQuery,
                        SimulationStep* startTime,
                        SimulationStep* endTime,
                        ActionType* actionQuery) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
};

/**
 * @class NeuralAssociativeMemory
 * 
 * Neural associative memory implementation creating relationships between patterns.
 * 
 * This class implements associative memory using actual neural dynamics.
 * Instead of storing text descriptions, it stores associations between neural
 * patterns and creates connection patterns. This provides:
 * - **Pattern association**: Creates connections between neural patterns
 * - **Spread activation**: Propagates activation through association chains
 * - **Memory completion**: Retrieves related patterns through association
 * - **Connection learning**: Learns associations through co-activation
 * - **Pattern similarity**: Computes similarity between neural patterns
 * 
 * NeuralAssociativeMemory mechanisms:
 * - **Association storage**: Stores connections between patterns
 * - **Pattern completion**: Retrieves related patterns
 * - **Spread activation**: Propagates activation through chains
 * - **Similarity computation**: Measures pattern similarity
 * - **Learning**: Forms associations through co-activation
 * 
 * Key features:
 * - **Neural representation**: Associations stored as neural connections
 * - **Pattern completion**: Retrieve related patterns
 * - **Spread activation**: Propagate activation through networks
 * - **Similarity matching**: Match similar patterns
 * - **Connection learning**: Learn new associations
 * 
 * @note This is the neural implementation of associative memory
 * @see Memory.hpp::AssociativeMemory - High-level associative memory interface
 * @see NeuralEpisodicMemory - For episodic memory implementation
 * @see Brain.hpp - For brain-level integration
 * 
 * @author NLM Development Team
 */
class NeuralAssociativeMemory {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new neural associative memory instance with default parameters.
     * The neural associative memory is initialized with default settings
     * for neural pattern association and retrieval.
     * 
     * @post Neural associative memory created with default settings
     * @post Ready for pattern association and retrieval
     */
    NeuralAssociativeMemory();
    
    /**
     * @brief Destructor
     * 
     * Cleans up neural associative memory resources and performs any necessary
     * cleanup operations. The neural associative memory is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Neural associative memory cannot be used after destruction
     */
    ~NeuralAssociativeMemory();
    
    // Disable copying - PImpl idiom
    NeuralAssociativeMemory(const NeuralAssociativeMemory&) = delete;
    NeuralAssociativeMemory& operator=(const NeuralAssociativeMemory&) = delete;
    NeuralAssociativeMemory(NeuralAssociativeMemory&&) noexcept;
    NeuralAssociativeMemory& operator=(NeuralAssociativeMemory&&) noexcept;
    
    /**
     * @brief Initialize with brain reference
     * 
     * Initializes the neural associative memory with a reference to the brain.
     * This is essential for neural integration and interaction with other
     * neural systems. The brain reference is used for:
     * - Accessing neurons and neural populations
     * - Establishing neural connections
     * - Integrating with brain dynamics
     * - Coordinating with other systems
     */
    void initialize(Brain* brain);
    
    /**
     * @brief Create association between two neural patterns
     * 
     * Creates an association between two neural patterns. This is essential for:
     * - Building associative networks
     * - Storing connections between patterns
     * - Pattern completion
     * - Memory association
     * - Neural connectivity
     */
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    
    /**
     * @brief Create association based on experience
     * 
     * Creates an association based on a complete episodic memory item.
     * This is essential for:
     * - Learning associations from experiences
     * - Building experience-based associations
     * - Experience integration
     * - Associative memory
     */
    void associateFromExperience(const EpisodicMemoryItem& episode);
    
    /**
     * @brief Retrieve patterns associated with a query pattern
     * 
     * Retrieves patterns associated with a query pattern. This is used for:
     * - Pattern completion
     * - Associative retrieval
     * - Memory-based association
     * - Pattern completion
     */
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;
    
    /**
     * @brief Get association strength between two patterns
     * 
     * Returns the strength of the association between two patterns.
     * This is essential for:
     * - Measuring association strength
     * - Controlling associative influence
     * - Managing association dynamics
     * - Association-based processing
     */
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;
    
    /**
     * @brief Update association based on outcome
     * 
     * Updates the association based on pattern outcome. This is used for:
     * - Modifying association strength
     * - Learning associations
     * - Managing association dynamics
     * - Association-based processing
     */
    void updateAssociation(const std::vector<float>& patternA,
                           const std::vector<float>& patternB,
                           float delta);
    
    /**
     * @brief Spread activation from cue pattern
     * 
     * Spreads activation from a cue pattern, returning patterns that become
     * activated through association chains. This is used for:
     * - Spread activation
     * - Association chaining
     * - Pattern completion
     * - Associative propagation
     */
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;
    
    /**
     * @brief Clear associations
     * 
     * Clears all associations from neural associative memory, removing all
     * stored connections. This is useful for:
     * - Resetting associative memory
     * - Managing memory capacity
     * - Association management
     * - Error recovery
     */
    void clear();
    
    /**
     * @brief Get number of associations
     * 
     * Returns the number of associations stored in neural associative memory.
     * This is an important statistic for understanding memory capacity and load.
     */
    size_t getAssociationCount() const { return associations_.size(); }
    
private:
    /**
     * @brief Find pattern neuron or create one
     * 
     * Finds or creates a neural pattern neuron for a given pattern. This is
     * used for:
     * - Pattern storage
     * - Pattern retrieval
     * - Memory association
     * - Neural pattern management
     */
    NeuronId findPatternNeuron(const std::vector<float>& pattern);
    
    /**
     * @brief Compute pattern similarity
     * 
     * Computes the similarity between two neural patterns. This is used for:
     * - Pattern matching
     * - Pattern comparison
     * - Association formation
     * - Memory similarity
     */
    float computeSimilarity(const std::vector<float>& a,
                            const std::vector<float>& b) const;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm

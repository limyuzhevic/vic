#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>

namespace nlm {

/**
 * @brief EpisodicMemoryItem: Represents a stored experience
 * 
 * NOT a textual record - stores neural activity patterns and associations.
 * 
 * An episodic memory item represents a single experience or event in the
 * agent's lifetime. Unlike traditional database entries, episodic memories
 * in this neural system are encoded as patterns of neural activity, making them
 * directly accessible to the neural computation systems.
 * 
 * The episodic memory system stores experiences in a way that interacts with
 * the neural substrate, allowing for pattern completion, temporal ordering,
 * and relevance-weighted recall.
 */
struct EpisodicMemoryItem {
    SimulationStep timestamp;           ///< Time when the experience occurred
    
    // Sensory state at time of experience (encoded as activity pattern)
    std::vector<float> sensoryState;
    
    // Location/position information
    float positionX;
    float positionY;
    float orientation;
    
    // Action taken (if any)
    ActionType action;
    
    // Internal state (energy, reward, etc.)
    float reward;
    float energy;
    float novelty;
    
    // Consequence of action
    std::vector<float> resultingSensoryState;
    float resultingReward;
    
    // Associated neural activity pattern (who was firing)
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    
    // Time since this episode
    SimulationStep age;
    
    /**
     * @brief Default constructor
     * 
     * Initializes with default values:
     * - timestamp: 0
     * - position: (0, 0, 0)
     * - action: Wait
     * - reward, energy, novelty: 0.0
     * - resultingReward: 0.0
     * - age: 0
     */
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0) {}
};

/**
 * @brief NeuralEpisodicMemory: Stores experiences in a way that interacts with neural substrate
 * 
 * NOT a database - episodes are encoded as synaptic patterns and can influence neural activity.
 * 
 * Episodic memory is crucial for an agent's ability to:
 * - Learn from past experiences
 * - Predict future outcomes
 * - Plan actions based on remembered scenarios
 * - Develop temporal awareness
 * 
 * Key mechanisms:
 * - Episodes stored as neural activity patterns
 * - Retrieval via pattern completion (Hopfield-like)
 * - Temporal ordering through timestamps
 * - Relevance-weighted recall
 * - Replay during offline periods
 * 
 * @note This implementation represents Phase 3 of the NLM architecture,
 * providing experience-based learning capabilities.
 */
class NeuralEpisodicMemory {
public:
    /**
     * @brief Default constructor
     */
    NeuralEpisodicMemory();
    
    /**
     * @brief Destructor
     */
    ~NeuralEpisodicMemory();

    /**
     * @brief Initialize with brain reference
     * 
     * @param brain Pointer to the brain instance that this memory system belongs to
     */
    void initialize(Brain* brain);

    /**
     * @brief Store a new experience
     * 
     * Encodes the experience as neural activity and can influence future processing.
     * The experience is stored along with its context (sensory state, action, reward, etc.)
     * for later retrieval and use.
     * 
     * @param episode The experience to store
     */
    void storeEpisode(const EpisodicMemoryItem& episode);

    /**
     * @brief Retrieve episodes similar to a query pattern
     * 
     * Uses pattern completion mechanisms to find episodes that are similar to
     * the given sensory pattern. Results are ordered by similarity.
     * 
     * @param sensoryPattern Query pattern to match against
     * @param maxResults Maximum number of results to return (default: 5)
     * @return Vector of pointers to retrieved episodes, ordered by relevance
     */
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           size_t maxResults = 5) const;

    /**
     * @brief Retrieve episodes from a specific time window
     * 
     * Filters episodes based on their timestamps to find experiences
     * within a given time range.
     * 
     * @param startTime Start of time window
     * @param endTime End of time window
     * @param maxResults Maximum number of results to return (default: 10)
     * @return Vector of pointers to retrieved episodes, most recent first
     */
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                             SimulationStep endTime,
                                                             size_t maxResults = 10) const;

    /**
     * @brief Retrieve episodes by location
     * 
     * Finds experiences that occurred at or near a specific location.
     * Useful for spatial memory and navigation planning.
     * 
     * @param x X coordinate of location
     * @param y Y coordinate of location
     * @param radius Search radius around the location
     * @param maxResults Maximum number of results to return (default: 5)
     * @return Vector of pointers to retrieved episodes, closest first
     */
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(float x, float y,
                                                              float radius,
                                                              size_t maxResults = 5) const;

    /**
     * @brief Retrieve episodes following a specific action
     * 
     * Finds experiences that occurred after a specific action was taken.
     * 
     * @param action Action type to search for
     * @param maxResults Maximum number of results to return (default: 5)
     * @return Vector of pointers to retrieved episodes, most recent first
     */
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(ActionType action,
                                                               size_t maxResults = 5) const;

    /**
     * @brief Replay an episode - reactivates the neural patterns associated with it
     * 
     * This allows the brain to "relive" the experience, which can help with
     * consolidation, planning, and learning. The neural patterns are reactivated
     * to strengthen the memory trace and integrate the experience into the
     * agent's knowledge base.
     * 
     * @param episode Pointer to the episode to replay
     */
    void replayEpisode(const EpisodicMemoryItem* episode);

    /**
     * @brief Update episode relevance based on recent use
     * 
     * Adjusts the importance of an episode based on how recently it has been
     * accessed or used. More recently accessed episodes typically have higher
     * relevance for future processing.
     * 
     * @param episodeId Index of the episode to update
     * @param relevanceDelta Amount to change relevance by (positive = more relevant)
     */
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);

    /**
     * @brief Get episode by index
     * 
     * @param index Index of the episode to retrieve (0-based)
     * @return Pointer to the episode, or nullptr if index is out of range
     */
    const EpisodicMemoryItem* getEpisode(size_t index) const;

    /**
     * @brief Get total number of episodes
     * 
     * @return Number of episodes currently stored
     */
    size_t getEpisodeCount() const { return episodes_.size(); }

    /**
     * @brief Get recent episodes
     * 
     * @param count Number of recent episodes to retrieve
     * @return Vector of pointers to recent episodes
     */
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;

    /**
     * @brief Get average reward from episodes
     * 
     * @return Average reward value across all stored episodes
     */
    float getAverageReward() const;

    /**
     * @brief Clear old episodes
     * 
     * Implements memory consolidation by removing less relevant or older episodes
     * when memory capacity is reached. This is a simplified implementation.
     * 
     * @param relevanceThreshold Threshold below which episodes are considered for removal
     */
    void consolidate(float relevanceThreshold);

    /**
     * @brief Clear all episodes
     */
    void clear();

    /**
     * @brief Enable/disable replay
     * 
     * @param enable Whether replay should be enabled
     */
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    
    /**
     * @brief Check if replay is enabled
     * 
     * @return true if replay is enabled, false otherwise
     */
    bool isReplayEnabled() const { return replayEnabled_; }

    /**
     * @brief Get episodes for replay (selection based on relevance and recency)
     * 
     * Selects episodes for replay during sleep or consolidation periods based on:
     * 1. Recency (more recent episodes)
     * 2. Reward (higher reward episodes)
     * 3. Unpredictability (higher prediction error episodes)
     * 
     * @param count Number of episodes to select for replay
     * @return Vector of pointers to episodes selected for replay
     */
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;

    /**
     * @brief Replay multiple episodes (for sleep-like consolidation)
     * 
     * Replays a sequence of episodes in order, which can help with memory
     * consolidation and integration. This simulates the replay that occurs
     * during sleep in biological systems.
     * 
     * @param episodeIds Vector of episode indices to replay
     */
    void replaySequence(const std::vector<size_t>& episodeIds);

private:
    /**
     * @brief Compute similarity between query and stored episode
     * 
     * Uses cosine similarity to measure how similar a query pattern is to a
     * stored episode's sensory state.
     * 
     * @param query Query pattern to compare
     * @param episode Episode to compare against
     * @return Similarity score (0.0 to 1.0, where 1.0 = identical)
     */
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;

    /**
     * @brief Check if episode matches criteria
     * 
     * Determines whether an episode matches a set of search criteria
     * (sensory pattern, time window, action, etc.).
     * 
     * @param episode Episode to check
     * @param sensoryQuery Sensory pattern to match against
     * @param startTime Start of time window (null = no restriction)
     * @param endTime End of time window (null = no restriction)
     * @param actionQuery Action type to match (null = no restriction)
     * @return true if episode matches all criteria, false otherwise
     */
    bool matchesCriteria(const EpisodicMemoryItem& episode,
                        const std::vector<float>* sensoryQuery,
                        SimulationStep* startTime,
                        SimulationStep* endTime,
                        ActionType* actionQuery) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation (PIMPL pattern)

    Brain* brain_;                   ///< Reference to parent brain
    std::deque<EpisodicMemoryItem> episodes_;  ///< Stored episodic memories
    size_t maxEpisodes_;             ///< Maximum number of episodes to store
    bool replayEnabled_;            ///< Whether replay functionality is enabled
};

/**
 * @brief NeuralAssociativeMemory: Creates and retrieves relationships between experiences
 * 
 * NOT a lookup table - associations are encoded in synaptic connectivity.
 * 
 * Associative memory allows the agent to form connections between related
 * experiences and retrieve them based on partial cues. This is implemented
 * using neural mechanisms where co-activating patterns form stronger connections
 * through Hebbian plasticity.
 * 
 * Key mechanisms:
 * - Co-activating neurons become associated via Hebbian plasticity
 * - Stronger associations form with repeated co-activation
 * - Can retrieve related items given a partial cue
 * - Spreading activation for associative recall
 */
class NeuralAssociativeMemory {
public:
    /**
     * @brief Default constructor
     */
    NeuralAssociativeMemory();
    
    /**
     * @brief Destructor
     */
    ~NeuralAssociativeMemory();

    /**
     * @brief Initialize with brain reference
     * 
     * @param brain Pointer to the brain instance
     */
    void initialize(Brain* brain);

    /**
     * @brief Create association between two neural patterns
     * 
     * Both patterns should be from actual neural activity. This creates a
     * bidirectional association that can be used for pattern completion.
     * 
     * @param patternA First pattern
     * @param patternB Second pattern
     * @param strength Initial association strength (0.0 to 1.0, default: 1.0)
     */
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    /**
     * @brief Create association based on experience
     * 
     * Forms associations between the sensory state before an action and
     * the resulting sensory state after the action, encoding the causal
     * relationship in the associative memory.
     * 
     * @param episode Episode containing the experience
     */
    void associateFromExperience(const EpisodicMemoryItem& episode);

    /**
     * @brief Retrieve patterns associated with a query pattern
     * 
     * Uses spreading activation to find patterns that are associated with
     * a given query pattern. Results are ordered by association strength.
     * 
     * @param queryPattern Query pattern to find associations for
     * @param maxResults Maximum number of results to return (default: 5)
     * @return Vector of associated patterns
     */
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                             size_t maxResults = 5) const;

    /**
     * @brief Get association strength between two patterns
     * 
     * @param patternA First pattern
     * @param patternB Second pattern
     * @return Association strength (0.0 if no association exists)
     */
    float getAssociationStrength(const std::vector<float>& patternA,
                                 const std::vector<float>& patternB) const;

    /**
     * @brief Update association based on outcome
     * 
     * Modifies the strength of an existing association based on feedback
     * or experience. This allows learning from outcomes.
     * 
     * @param patternA First pattern
     * @param patternB Second pattern
     * @param delta Change in association strength
     */
    void updateAssociation(const std::vector<float>& patternA,
                           const std::vector<float>& patternB,
                           float delta);

    /**
     * @brief Spread activation from cue pattern
     * 
     * Performs multi-step spreading activation to find patterns that become
     * activated through association chains. This implements the ability to
     * make connections across multiple experiences.
     * 
     * @param cuePattern Initial pattern that triggers spreading
     * @param steps Number of spreading activation steps (default: 2)
     * @return Vector of patterns that become activated through association chains
     */
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;

    /**
     * @brief Clear associations
     */
    void clear();

    /**
     * @brief Get number of associations
     * 
     * @return Number of associative relationships stored
     */
    size_t getAssociationCount() const { return associations_.size(); }

private:
    /**
     * @brief Find pattern neuron or create one
     * 
     * Looks for an existing neuron that represents a similar pattern,
     * or creates a new one if no similar pattern exists.
     * 
     * @param pattern Pattern to find or create
     * @return Neuron ID that represents the pattern
     */
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    /**
     * @brief Compute pattern similarity
     * 
     * Measures how similar two patterns are using cosine similarity.
     * Used for finding similar patterns and computing association strength.
     * 
     * @param a First pattern
     * @param b Second pattern
     * @return Similarity score (0.0 to 1.0)
     */
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation (PIMPL pattern)

    Brain* brain_;                                      ///< Reference to parent brain
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;  ///< Neurons representing patterns
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;      ///< Associative relationships
};

} // namespace nlm
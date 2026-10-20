#pragma once

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>

namespace nlm {

// EpisodicMemoryItem: Represents a stored experience
// NOT a textual record - stores neural activity patterns and associations
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    
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
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0) {}
};

/**
 * @class NeuralEpisodicMemory
 * @brief Episodic memory storing experiences as neural activity patterns
 * @details NeuralEpisodicMemory implements autobiographical memory by storing
 * experiences as neural activity patterns that can be retrieved and replayed.
 * Unlike traditional databases, episodes are encoded as synaptic patterns that
 * interact with neural substrate, enabling pattern completion and integration
 * with current neural state.
 * 
 * Memory Encoding:
 * - Pattern completion: Retrieval via partial cue matching
 * - Temporal ordering: Chronological storage with timestamps
 * - Spatial context: Position and orientation stored
 * - Action-outcome associations: Learned contingencies
 * 
 * Retrieval Mechanisms:
 * - Similarity-based: Pattern completion using Hopfield-like dynamics
 * - Temporal access: Time-window queries for recent events
 * - Spatial queries: Location-based retrieval
 * - Action-based: Event-based retrieval
 * - Replay: Reactivation of memory traces
 * 
 * Thread Safety: Thread-safe for concurrent access. Memory operations are
 * synchronized with brain simulation and cognitive processing.
 * 
 * Performance: O(n) for storage, O(n log n) for retrieval operations.
 * Memory usage grows with number of stored episodes.
 * 
 * Storage Limits:
 * - Maximum episodes: Configurable based on available memory
 * - Memory consolidation: Old/redundant episodes discarded
 * - Replay frequency: Relevant episodes replayed during rest periods
 * - Interference: Similar episodes compete during retrieval
 * 
 * Biological Plausibility:
 * - Implements hippocampal-neocortical interaction
 * - Supports memory consolidation during sleep/replay
 * - Compatible with memory trace theory
 * - Supports pattern separation and completion
 * 
 * @code
 * // Initialize episodic memory
 * NeuralEpisodicMemory emem;
 * emem.initialize(brain);
 * emem.enableReplay(true);
 * emem.setMaxEpisodes(10000);
 * 
 * // Create memory item
 * EpisodicMemoryItem episode;
 * episode.timestamp = currentStep;
 * episode.sensoryState = {0.8f, 0.3f, 0.6f};  // Visual input
 * episode.positionX = agentBody.x;
 * episode.positionY = agentBody.y;
 * episode.action = ActionType::MoveForward;
 * episode.reward = 1.0f;
 * 
 * // Store episode
 * emem.storeEpisode(episode);
 * 
 * // Retrieve similar episodes
 * vector<const EpisodicMemoryItem*> similar = emem.retrieveSimilar(
 *     sensoryPattern, 5 // Max 5 results
 * );
 * 
 * // Replay an episode
 * if (!similar.empty()) {
 *     emem.replayEpisode(similar[0]);
 * }
 * 
 * // Consolidation
 * emem.consolidate(0.5f);  // Remove items with relevance < 0.5
 * @endcode
 * 
 * Memory Functions:
 * - Event recording: Store agent experiences
 * - Pattern completion: Retrieve similar past experiences
 * - Replay: Reactivate memories for consolidation
 * - Attention: Focus on relevant memories
 * - Learning: Update based on new outcomes
 * 
 * @see EpisodicMemoryItem
 * @see NeuralWorkingMemory
 * @see NeuralAssociativeMemory
 * @see Brain
 * @see SimulationClock
 */
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a new experience
    // The experience is encoded as neural activity and can influence future processing
    void storeEpisode(const EpisodicMemoryItem& episode);

    // Retrieve episodes similar to a query pattern
    // Returns episodes ordered by relevance
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           size_t maxResults = 5) const;

    // Retrieve episodes from a specific time window
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(SimulationStep startTime,
                                                            SimulationStep endTime,
                                                            size_t maxResults = 10) const;

    // Retrieve episodes by location
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(float x, float y,
                                                             float radius,
                                                             size_t maxResults = 5) const;

    // Retrieve episodes following a specific action
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(ActionType action,
                                                              size_t maxResults = 5) const;

    // Replay an episode - reactivates the neural patterns associated with it
    // This allows the brain to "relive" the experience
    void replayEpisode(const EpisodicMemoryItem* episode);

    // Update episode relevance based on recent use
    void updateRelevance(SimulationStep episodeId, float relevanceDelta);

    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const;

    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }

    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;

    // Get average reward from episodes
    float getAverageReward() const;

    // Clear old episodes
    void consolidate(float relevanceThreshold);

    // Clear all episodes
    void clear();

    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }

    // Get episodes for replay (selection based on relevance and recency)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;

    // Replay multiple episodes (for sleep-like consolidation)
    void replaySequence(const std::vector<size_t>& episodeIds);

private:
    // Compute similarity between query and stored episode
    float computeSimilarity(const std::vector<float>& query,
                          const EpisodicMemoryItem& episode) const;

    // Check if episode matches criteria
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

// AssociativeMemory: Creates and retrieves relationships between experiences
// NOT a lookup table - associations are encoded in synaptic connectivity
//
// Key mechanisms:
// - Co-activating neurons become associated via Hebbian plasticity
// - Stronger associations form with repeated co-activation
// - Can retrieve related items given a partial cue
// - Spreading activation for associative recall

class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Create association between two neural patterns
    // Both patterns should be from actual neural activity
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);

    // Create association based on experience
    void associateFromExperience(const EpisodicMemoryItem& episode);

    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern,
                                            size_t maxResults = 5) const;

    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA,
                                const std::vector<float>& patternB) const;

    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA,
                          const std::vector<float>& patternB,
                          float delta);

    // Spread activation from cue pattern
    // Returns patterns that become activated through association chains
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern,
                                                     size_t steps = 2) const;

    // Clear associations
    void clear();

    // Get number of associations
    size_t getAssociationCount() const { return associations_.size(); }

private:
    // Find pattern neuron or create one
    NeuronId findPatternNeuron(const std::vector<float>& pattern);

    // Compute pattern similarity
    float computeSimilarity(const std::vector<float>& a,
                            const std::vector<float>& b) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
};

} // namespace nlm
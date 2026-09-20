#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>
#include <unordered_map>

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
    
    // Enhanced memory fields for neuroplasticity
    float relevance;                    // Relevance score (0-1)
    float consolidationStrength;        // How strong this episode is for consolidation
    bool isConsolidated;                 // Whether episode is in long-term storage
    float systemsConsolidationProgress; // Progress through systems consolidation (0-1)
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0)
        , relevance(0.0f), consolidationStrength(0.0f)
        , isConsolidated(false), systemsConsolidationProgress(0.0f) {}
};

// NeuralEpisodicMemory: Stores experiences in a way that interacts with neural substrate
// NOT a database - episodes are encoded as synaptic patterns and can influence neural activity
//
// Key mechanisms:
// - Episodes stored as neural activity patterns
// - Retrieval via pattern completion (Hopfield-like)
// - Temporal ordering through timestamps
// - Relevance-weighted recall
// - Replay during offline periods
// - Synaptic tagging and capture for consolidation
// - Memory consolidation through replay
// - Systems consolidation (hippocampal-neocortical interaction)

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a new experience
    // The experience is encoded as neural activity and can influence future processing
    // With enhanced neuroplasticity integration
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Enhanced store with neuroplasticity integration
    void storeEpisodeWithPlasticity(const EpisodicMemoryItem& episode, 
                                  const std::vector<NeuronId>& involvedNeurons);
    
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
    
    // Enhanced replay with neuroplasticity mechanisms
    void enhancedReplayEpisode(const EpisodicMemoryItem* episode,
                              float replayStrength = 1.0f,
                              bool isOffline = false);
    
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
    
    // Enhanced consolidation with neuroplasticity mechanisms
    void enhancedConsolidation();
    
    // Clear all episodes
    void clear();
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    // Get episodes for replay (selection based on relevance and recency)
    std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t count) const;
    
    // Get episodes for offline consolidation (during sleep)
    std::vector<const EpisodicMemoryItem*> getEpisodesForOfflineReplay(size_t count) const;
    
    // Get long-term memory strength for episode
    float getLongTermMemoryStrength(const EpisodicMemoryItem& episode) const;
    
    // Update long-term memory storage through systems consolidation
    void updateLongTermStorage(const EpisodicMemoryItem& episode, float replayStrength);
    
    // Get number of offline consolidation steps processed
    size_t getConsolidationStepsProcessed() const;
    
    // Get neural replay statistics
    struct NeuralReplayStats {
        size_t replayCount;
        size_t replaySteps;
        float avgReplayStrength;
        float avgPatternCompletion;
    };
    
    NeuralReplayStats getReplayStats() const;
    
    // Reset replay statistics
    void resetReplayStats();
    
    // Check if offline consolidation is active
    bool isOfflineConsolidationActive() const { return offlineConsolidationActive_; }
    
    // Get current consolidation phase (0.0 to 1.0)
    float getConsolidationPhase() const { return consolidationPhase_; }
    
    // Begin offline consolidation phase (simulates sleep)
    void beginOfflineConsolidation();
    
    // End offline consolidation phase
    void endOfflineConsolidation();
    
    // Force consolidation of specific episode
    bool consolidateEpisode(NeuronId neuronId);
    
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
    
    // Enhanced replay mechanisms
    void enhancedReplayEpisode(const EpisodicMemoryItem* episode, 
                              float replayStrength = 1.0f,
                              bool isOffline = false);
    
    // Offline consolidation processing
    void processOfflineConsolidation();
    
    // Systems consolidation - transfer from episodic to long-term memory
    void systemsConsolidation();
    
    // Replay-based plasticity enhancement
    void applyReplayInducedPlasticity(const EpisodicMemoryItem* episode,
                                    float replayStrength);
    
    // Calculate replay efficacy
    float calculateReplayEfficacy(const EpisodicMemoryItem* episode) const;
    
    // Get neural similarity between episodes (for pattern completion)
    float getNeuralPatternSimilarity(const EpisodicMemoryItem& episode1,
                                    const EpisodicMemoryItem& episode2) const;
    
    // Spontaneous replay (during offline periods)
    void spontaneousReplay();
    
    // Replay sequencing optimization
    std::vector<size_t> optimizeReplaySequence(const std::vector<size_t>& episodeIds) const;
    
    // Create synaptic tags for consolidation
    void createSynapticTags(const EpisodicMemoryItem& episode, float tagStrength);
    
    // Capture synaptic resources for consolidation
    void captureSynapticResources(const EpisodicMemoryItem& episode);
    
    // Integrate captured resources into long-term memory
    void integrateConsolidationResources(const EpisodicMemoryItem& episode);
    
    // Calculate memory relevance based on neurobiological factors
    float calculateMemoryRelevance(const EpisodicMemoryItem& episode) const;
    
    // Update memory consolidation status
    void updateConsolidationStatus(EpisodicMemoryItem& episode, float replayStrength);
    
    // Get episodes scheduled for replay (for consolidation)
    std::vector<const EpisodicMemoryItem*> getEpisodesForOfflineReplay(size_t count) const;

private:
    // Enhanced memory state
    struct Impl {
        Brain* brain;
        
        // Pattern neurons for episodic recall
        std::vector<NeuronId> episodeNeurons;
        
        // Synaptic tagging and capture state
        std::unordered_map<NeuronId, float> synapticTags;        // NeuronId -> tag strength (0-1)
        std::unordered_map<NeuronId, float> capturedResources;  // NeuronId -> captured resources
        std::unordered_map<NeuronId, float> consolidationDelay; // NeuronId -> remaining delay
        
        // Offline consolidation state
        bool offlineConsolidationActive;
        float consolidationPhase;
        size_t consolidationStepsProcessed;
        SimulationStep offlineConsolidationStartTime;
        
        // Systems consolidation state
        std::unordered_map<size_t, float> longTermMemoryStrength; // episodeId -> strength (0-1)
        
        // Neural replay statistics
        struct ReplayStats {
            size_t replayCount;
            size_t replaySteps;
            float totalReplayStrength;
            float totalPatternCompletion;
        } replayStats;
        
        // Replay timing and sequencing
        struct ReplayEvent {
            size_t episodeId;
            float strength;
            SimulationStep timestamp;
            bool isOffline;
            float predictedCompletion;
        };
        
        std::deque<ReplayEvent> replayQueue;
        SimulationStep lastReplayTime;
        
        Impl() : brain(nullptr),
                 offlineConsolidationActive(false),
                 consolidationPhase(0.0f),
                 consolidationStepsProcessed(0),
                 offlineConsolidationStartTime(0) {
            replayStats.replayCount = 0;
            replayStats.replaySteps = 0;
            replayStats.totalReplayStrength = 0.0f;
            replayStats.totalPatternCompletion = 0.0f;
        }
    };
    
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    // Offline consolidation state
    bool offlineConsolidationActive_;
    float consolidationPhase_;
    size_t consolidationStepsProcessed_;
    
    // Systems consolidation state
    std::unordered_map<size_t, float> longTermMemoryStrength_; // episodeId -> strength (0-1)
    
    // Neural replay statistics
    struct ReplayStats {
        size_t replayCount;
        size_t replaySteps;
        float totalReplayStrength;
        float totalPatternCompletion;
    } replayStats_;
    
    // Replay timing and sequencing
    struct ReplayEvent {
        size_t episodeId;
        float strength;
        SimulationStep timestamp;
        bool isOffline;
        float predictedCompletion;
    };
    
    std::deque<ReplayEvent> replayQueue_;
    SimulationStep lastReplayTime_;
    
    // Neural replay patterns
    static constexpr float OFFLINE_CONSOLIDATION_DURATION = 1000.0f;  // Simulation steps
    static constexpr float CONSOLIDATION_UPDATE_INTERVAL = 10.0f;
    static constexpr float SYSTEMS_CONSOLIDATION_RATE = 0.01f;  // Rate of transfer to long-term
    static constexpr float REPLAY_STRENGTH_DECAY = 0.95f;  // Strength decays between replays
    static constexpr size_t MAX_REPLAY_QUEUE_SIZE = 50;
    
    // Replay-induced plasticity parameters
    static constexpr float REPLAY_PLASTICITY_THRESHOLD = 0.6f;  // Threshold for effective replay
    static constexpr float REPLAY_SYNAPTIC_STRENGTH_INCREASE = 0.005f;
    static constexpr float REPLAY_SYNAPTIC_EFFICACY_INCREASE = 0.01f;
    static constexpr float REPLAY_ELICITATION_TRACE_POTENTIATION = 0.1f;
    static constexpr float SYNAPTIC_TAGging_STRENGTH = 0.2f;  // Strength of synaptic tagging
    static constexpr float SYNAPTIC_CAPTURE_RATE = 0.3f;       // Rate at which synapses capture resources
    static constexpr float SYNAPTIC_INTEGRATION_RATE = 0.1f;   // Rate at which resources are integrated
};

} // namespace nlm
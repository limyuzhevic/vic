#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>

namespace nlm {

// EpisodicMemoryItem: Represents a stored experience
// Enhanced version with comprehensive temporal and system state information
struct EpisodicMemoryItem {
    SimulationStep timestamp;
    
    // Real-time sensory integration - store raw sensory input patterns
    std::vector<float> sensoryState;
    
    // System state beyond neural activity
    std::vector<float> systemState;  // Additional system-wide state variables
    float systemEnergy;              // Global energy level
    float systemTemperature;         // Neural temperature/viscosity
    float globalExcitability;        // Overall excitability level
    
    // Enhanced location/position information with context
    float positionX;
    float positionY;
    float positionZ;
    float orientation;
    std::vector<float> bodyVelocity; // Movement context
    
    // Action-context binding
    ActionType action;
    std::vector<float> actionContext;    // Context in which action was taken
    std::vector<float> actionParameters; // Specific parameters of action
    std::vector<float> actionOutcomes;   // Immediate outcomes of action
    
    // Enhanced internal state and metadata
    float reward;
    float energy;
    float novelty;
    float predictionError;        // Prediction error for this experience
    float curiosityGain;          // How much this experience increased curiosity
    float salience;               // Importance/salience of the memory
    float emotionalValence;       // Positive/negative emotional content
    
    // Cross-system integration
    std::vector<float> workingMemoryPattern; // What was active in working memory
    std::vector<float> predictionTarget;    // What the prediction system predicted
    std::vector<float> attentionFocus;      // What attention was focused on
    
    // Consequence of action (enhanced)
    std::vector<float> resultingSensoryState;
    float resultingReward;
    std::vector<float> resultingSystemState;
    float resultingEnergyChange;
    
    // Associated neural activity pattern (who was firing)
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    std::vector<float> neuronLifespans;   // How long each neuron was active
    
    // Enhanced temporal dynamics
    SimulationStep age;
    float decayFactor;            // How quickly this episode decays
    float consolidationProgress; // How consolidated the memory is (0-1)
    SimulationStep replayCount;   // How many times this has been replayed
    SimulationStep lastReplayStep; // When it was last replayed
    SimulationStep sleepOnset;    // When this memory was first stored during sleep
    
    // Sleep/rest cycle integration
    bool wasConsolidatedDuringSleep;     // Whether memory was consolidated during rest
    SimulationStep sleepConsolidationStep; // Step when consolidation occurred
    float sleepConsolidationGain;         // Effectiveness of sleep consolidation
    
    // Memory scoring for prioritization
    float memoryScore;           // Composite score for importance
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), positionZ(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0), predictionError(0), curiosityGain(0)
        , salience(0), emotionalValence(0), resultingReward(0), resultingEnergyChange(0)
        , age(0), decayFactor(0.9f), consolidationProgress(0.0f)
        , replayCount(0), lastReplayStep(0), sleepOnset(0)
        , wasConsolidatedDuringSleep(false), sleepConsolidationStep(0)
        , sleepConsolidationGain(0.0f), memoryScore(0.0f) {}
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

    // Store episode with comprehensive system state information
    // New API for real-time integration with sensory and system inputs
    void storeEpisodeWithContext(const EpisodicMemoryItem& episode,
                                const std::vector<float>& systemState,
                                float systemEnergy,
                                float globalExcitability,
                                const std::vector<float>& bodyVelocity,
                                const std::vector<float>& workingMemoryPattern,
                                const std::vector<float>& predictionTarget,
                                const std::vector<float>& attentionFocus,
                                float predictionError,
                                float curiosityGain,
                                float salience,
                                float emotionalValence);
    
    // Store action with context and outcomes
    // Enhanced action-context binding
    void storeActionContext(const EpisodicMemoryItem& episode,
                           const std::vector<float>& actionContext,
                           const std::vector<float>& actionParameters,
                           const std::vector<float>& actionOutcomes,
                           const std::vector<float>& resultingSystemState,
                           float resultingEnergyChange);
    
    // Connect to prediction system for prediction errors
    // Cross-system integration with prediction
    void updatePredictionError(SimulationStep episodeId, float predictionError);
    
    // Integrate with working memory for pattern completion
    // Cross-system integration with working memory
    void integrateWithWorkingMemory(const std::vector<float>& workingMemoryPattern);
    
    // Support sleep/rest cycles for consolidation
    // Sleep/rest cycle integration
    void storeDuringSleep(SimulationStep step);
    void consolidateDuringSleep();
    void markConsolidatedDuringSleep(SimulationStep episodeId, float gain);
    
    // Enhanced replay with replay integration
    // Better APIs for accessing and replaying memories
    void replayEpisodeWithMeta(const EpisodicMemoryItem* episode,
                              const std::vector<float>& replayContext);
    
    // Store not just neurons but also system state and metadata
    // Enhanced temporal dynamics
    void storeSystemState(const std::vector<float>& systemState,
                         float systemEnergy,
                         float globalExcitability,
                         const std::vector<float>& bodyVelocity);
    
    // Get enhanced metadata for episodes
    float getPredictionError(SimulationStep episodeId) const;
    float getCuriosityGain(SimulationStep episodeId) const;
    float getSalience(SimulationStep episodeId) const;
    float getEmotionalValence(SimulationStep episodeId) const;
    bool wasConsolidatedDuringSleep(SimulationStep episodeId) const;
    float getSleepConsolidationGain(SimulationStep episodeId) const;
    float getMemoryScore(SimulationStep episodeId) const;
    
    // Update memory scores based on all factors
    void updateMemoryScores();
    
    // Enhanced retrieval based on cross-system factors
    // Retrieve memories by prediction error magnitude
    std::vector<const EpisodicMemoryItem*> retrieveByPredictionError(float threshold, size_t maxResults) const;
    
    // Retrieve memories by curiosity gain
    std::vector<const EpisodicMemoryItem*> retrieveByCuriosityGain(float threshold, size_t maxResults) const;
    
    // Retrieve memories by salience
    std::vector<const EpisodicMemoryItem*> retrieveBySalience(float threshold, size_t maxResults) const;
    
    // Retrieve memories by emotional valence
    std::vector<const EpisodicMemoryItem*> retrieveByEmotionalValence(float threshold, size_t maxResults) const;
    
    // Get all episodes stored during sleep
    std::vector<const EpisodicMemoryItem*> getEpisodesConsolidatedDuringSleep() const;
    
    // Get replay statistics
    size_t getReplayCount(SimulationStep episodeId) const;
    SimulationStep getLastReplayStep(SimulationStep episodeId) const;
    
    // Enhanced replay that considers sleep consolidation
    void replayConsolidatedEpisode(const EpisodicMemoryItem* episode, bool enhancedReplay = false);

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
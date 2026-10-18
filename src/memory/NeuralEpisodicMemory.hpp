#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
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

    // Replay multiple episodes (for sleep-like consolidation)
    void replaySequence(const std::vector<size_t>& episodeIds);

    // === ADVANCED REPLAY SYSTEMS ===

    // Sleep/Offline Replay - Replay during rest/sleep cycles for consolidation
    void replayDuringSleep(float sleepIntensity = 1.0f);
    bool isSleeping() const { return isSleeping_; }
    void enterSleepState() { isSleeping_ = true; }
    void exitSleepState() { isSleeping_ = false; }

    // Synaptic Replay - Reactivate synaptic connections during replay for strengthening
    void replaySynapticPatterns(const std::vector<size_t>& episodeIds, float synapticStrength = 1.0f);

    // Priority-Based Replay - Replay based on memory importance, novelty, reward
    std::vector<size_t> selectPriorityEpisodes(size_t count,
                                               const std::vector<float>& neuromodulatorWeights = {}) const;
    float computeMemoryPriority(const EpisodicMemoryItem& episode) const;

    // Sequence Replay - Replay temporal sequences for pattern completion
    void replayTemporalSequences(const std::vector<size_t>& episodeIds,
                                bool forwardOnly = true);
    std::vector<std::vector<size_t>> extractTemporalSequences(size_t sequenceLength = 5) const;

    // Development-Stage Replay - Different replay strategies for different developmental stages
    void replayForDevelopmentalStage(DevelopmentalStage stage,
                                      float plasticityModulator = 1.0f);
    float getDevelopmentalReplayPriority(const EpisodicMemoryItem& episode,
                                         DevelopmentalStage stage) const;

    // Neuromodulator-Gated Replay - Control replay timing and strength with neuromodulators
    void neuromodulatorGatedReplay(const Neuromodulator& modulator, float gateThreshold = 0.5f);
    bool isNeuromodulatorGatedReplayEnabled(const Neuromodulator& modulator) const;
    float getNeuromodulatorGatingStrength(const Neuromodulator& modulator) const;

    // Metaplastic Replay Effects - Replay-induced metaplasticity for learning adaptation
    void applyMetaplasticReplayEffects(const std::vector<size_t>& episodeIds,
                                      const std::vector<float>& learningHistory);
    void updateMetaplasticThresholds(const std::vector<size_t>& episodeIds);

    // Multi-System Replay - Replay across working memory, episodic memory, and conceptual memory
    void crossSystemReplay(const std::vector<size_t>& episodeIds,
                          NeuralWorkingMemory* workingMemory,
                          ConceptFormation* conceptFormation = nullptr);
    void strengthenCrossSystemConnections(const std::vector<size_t>& episodeIds);

    // Consolidation-Graduated Replay - Progressive strengthening during consolidation
    void graduatedConsolidationReplay(size_t consolidationStage,
                                     float strengtheningFactor = 1.0f);
    float getConsolidationProgress(size_t episodeId) const;

    // Advanced replay integration with attention and prediction
    void attentionGuidedReplay(const std::vector<size_t>& episodeIds,
                              const std::vector<float>& attentionWeights);
    void predictionErrorGuidedReplay(const std::vector<size_t>& episodeIds,
                                    float predictionErrorThreshold);

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

    // Advanced replay state management
    bool isSleeping_;
    std::unordered_map<const Neuromodulator*, bool> neuromodulatorGatedReplay_;
    std::unordered_map<const Neuromodulator*, float> neuromodulatorGatingStrength_;
    std::deque<size_t> sleepReplayQueue_;
    std::vector<std::vector<size_t>> temporalSequences_;
    std::unordered_map<size_t, float> consolidationProgress_;
    std::deque<float> metaplasticThresholdHistory_;
    std::vector<size_t> developmentalStageReplays_;

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
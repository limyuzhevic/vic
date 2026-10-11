#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "NeuralWorkingMemory.hpp"
#include "AttentionalSelection.hpp"
#include "PredictionSystem.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations for neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Novelty;
class Curiosity;
class PredictionError;
class DevelopmentSystem;

// Enhanced EpisodicMemoryItem: Stores rich neural experience data
// NOT a textual record - stores neural activity patterns, attention weights, prediction data
//
// Key mechanisms:
// - Episodes stored as neural activity patterns with metadata
// - Retrieval via pattern completion (Hopfield-like)
// - Temporal ordering through timestamps
// - Relevance-weighted recall using attention and prediction
// - Replay during offline periods (sleep/rest cycles)
// - Integration with working memory for pattern completion
// - Neuromodulation-based emotional significance tagging

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
    
    // ENHANCED DATA FOR SOPHISTICATED EPISODIC MEMORY
    
    // Attention relevance weights (from attentional selection system)
    std::vector<float> attentionWeights;
    
    // Prediction accuracy and error for this episode
    float predictionAccuracy;
    float predictionError;
    
    // Neuromodulation-based emotional significance tags
    float emotionalTag;
    NeuromodulatorType neuromodulatorType;  // Which neuromodulator dominated this experience
    
    // Working memory integration metrics
    float patternCompletionScore;
    std::vector<float> workingMemoryPattern;
    
    // Stress-based memory prioritization
    float stressPriority;
    
    // Replay-specific data
    SimulationStep lastReplayTime;
    ReplayType replayType;  // Forward, backward, or standard replay
    bool replayStrengthened;
    
    // Development system integration
    DevelopmentalStage developmentalStage;
    SimulationStep conceptId;
    
    // Consolidation state
    float consolidationScore;
    bool isConsolidated;
    SimulationStep consolidationTime;
    
    // Pattern similarity for selective storage
    std::vector<float> patternHash;
    float patternStability;
    
    EpisodicMemoryItem()
        : timestamp(0), positionX(0), positionY(0), orientation(0)
        , action(ActionType::Wait)
        , reward(0), energy(0), novelty(0)
        , resultingReward(0), age(0)
        , predictionAccuracy(0.5f), predictionError(0.0f)
        , emotionalTag(0.0f), neuromodulatorType(NeuromodulatorType::None)
        , patternCompletionScore(0.0f), stressPriority(1.0f), lastReplayTime(0), replayType(ReplayType::Standard)
        , replayStrengthened(false), developmentalStage(DevelopmentalStage::Initial)
        , conceptId(0), consolidationScore(0.0f), isConsolidated(false)
        , patternStability(0.0f) {};

// Replay type enumeration
enum class ReplayType {
    Standard,
    Forward,  // Forward temporal replay
    Backward  // Backward temporal replay
};

// Neuromodulator type enumeration
enum class NeuromodulatorType {
    None,
    Dopamine,
    Acetylcholine,
    Norepinephrine,
    Serotonin,
    Novelty,
    Curiosity,
    PredictionError
};

// Enhanced NeuralEpisodicMemory: Sophisticated episodic memory system
// Stores experiences as neural patterns with rich metadata
//
// Key mechanisms:
// - Rich metadata storage with attention, prediction, neuromodulation data
// - Enhanced retrieval with attention and prediction guidance
// - Strategic replay with sleep/rest cycle integration
// - Working memory pattern completion for recall
// - Neuromodulation-based emotional significance tagging
// - Development-dependent replay dynamics
// - Selective consolidation based on importance

class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Enhanced store episode with integration to working memory and prediction systems
    void storeEpisode(const EpisodicMemoryItem& episode,
                     const std::vector<float>* workingMemoryPattern = nullptr,
                     float attentionScore = 0.0f,
                     float predictionError = 0.0f,
                     NeuromodulatorType neuromodulatorType = NeuromodulatorType::None,
                     DevelopmentalStage stage = DevelopmentalStage::Initial,
                     size_t conceptId = 0);

    // Enhanced storage with attention and prediction guidance
    void storeEpisodeWithAttention(const EpisodicMemoryItem& episode,
                                  const AttentionalSelection& attention,
                                  const PredictionSystem& prediction,
                                  const Curiosity& curiosity,
                                  const Dopamine& dopamine);

    // Enhanced retrieval with attention and prediction guidance
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(const std::vector<float>& sensoryPattern,
                                                           const AttentionalSelection* attention = nullptr,
                                                           const PredictionSystem* prediction = nullptr,
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

    // Enhanced replay with attention and prediction integration
    void replayEpisode(const EpisodicMemoryItem* episode,
                      const AttentionalSelection& attention,
                      const PredictionSystem& prediction,
                      ReplayType type = ReplayType::Standard);

    // Replay with attention guidance for focused replay
    void replayWithAttention(const EpisodicMemoryItem* episode,
                            const AttentionalSelection& attention,
                            const std::vector<NeuronId>& attendedNeurons);

    // Update episode relevance based on recent use and importance
    void updateRelevance(SimulationStep episodeId, float relevanceDelta,
                       const AttentionalSelection& attention,
                       const PredictionSystem& prediction);

    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const;

    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }

    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;

    // Get average reward from episodes
    float getAverageReward() const;

    // Enhanced consolidation with sleep/rest cycles
    void consolidate(float relevanceThreshold = 0.3f,
                    const std::vector<float>& neuromodulationLevels = std::vector<float>(),
                    SimulationStep currentStep = 0);

    // Clear old episodes
    void clear();

    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }

    // Store metadata information with current step
    // Used by concept formation to influence episodic memory encoding
    void storeMetaInfo(size_t conceptId, SimulationStep currentStep) {
        if (brain_ && conceptId > 0 && conceptId < episodes_.size()) {
            // Find the episode by timestamp (conceptId represents episode index in this context)
            for (auto& ep : episodes_) {
                if (ep.timestamp == conceptId) {
                    ep.conceptId = conceptId;
                    ep.developmentalStage = developmentalStage_;
                    ep.consolidationScore = 0.7f;  // Strong consolidation for concept-linked episodes
                    ep.isConsolidated = true;
                    ep.consolidationTime = currentStep;
                    break;
                }
            }
        }
    }

    // Apply sleep/rest cycle to memory consolidation
    void applySleepConsolidation(SimulationStep currentStep) {
        startSleepCycle();
        sleepConsolidation(currentStep);
    }

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

    // Enhanced attention-based scoring for episode selection
    float calculateAttentionBasedScore(const EpisodicMemoryItem& episode,
                                      const AttentionalSelection* attention,
                                      const PredictionSystem* prediction) const;

    // Calculate pattern stability for selective storage
    float calculatePatternStability(const EpisodicMemoryItem& episode) const;

    // Apply neuromodulation effects to episode encoding
    void applyNeuromodulationToEpisode(EpisodicMemoryItem& episode,
                                      const Dopamine& dopamine,
                                      const Acetylcholine& acetylcholine,
                                      const Norepinephrine& norepinephrine,
                                      const Serotonin& serotonin,
                                      const Novelty& novelty,
                                      const Curiosity& curiosity,
                                      const PredictionError& predictionError);

    // Sleep/rest cycle replay consolidation
    void sleepConsolidation(SimulationStep currentStep);

    // Forward replay for temporal sequence learning
    void forwardReplay(const EpisodicMemoryItem& episode,
                      const AttentionalSelection& attention,
                      const PredictionSystem& prediction,
                      SimulationStep currentStep);

    // Backward replay for error correction
    void backwardReplay(const EpisodicMemoryItem& episode,
                       const AttentionalSelection& attention,
                       const PredictionSystem& prediction,
                       SimulationStep currentStep);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    
    // Sleep/rest cycle management
    bool isResting_;
    size_t sleepReplayInterval_;
    size_t sleepConsolidationInterval_;
    size_t stepsSinceLastSleepReplay_;
    size_t stepsSinceLastSleepConsolidation_;
    
    // Development
    DevelopmentalStage developmentalStage_;
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
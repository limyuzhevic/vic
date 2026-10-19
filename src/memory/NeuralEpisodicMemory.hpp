#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <array>
#include <tuple>
#include <set>
#include <queue>

namespace nlm {

// Enhanced EpisodicMemoryItem with rich contextual binding
struct EpisodicMemoryItem {
    uint64_t id;
    SimulationStep timestamp;
    SimulationStep age;
    SimulationStep lastAccess;
    
    // Rich contextual binding (what, where, when, how)
    // What: Action and outcome
    ActionType action;
    std::vector<float> sensoryState;          // What was perceived
    std::vector<float> resultingSensoryState; // What happened as a result
    float reward;
    float energy;
    float novelty;
    float predictionError;
    
    // Where: Spatial context
    float positionX;
    float positionY;
    float orientation;
    float spatialContextRadius;
    
    // When: Temporal context
    SimulationStep temporalContextStart;
    SimulationStep temporalContextEnd;
    
    // How: Causal and procedural context
    std::vector<NeuronId> activeNeurons;      // Who was firing
    std::vector<float> neuronActivations;
    std::vector<float> synapticWeights;
    std::vector<float> causalStrengths;
    
    // Self-referential encoding
    bool selfReferential;
    float selfImportance;
    std::vector<float> selfRepresentation;
    
    // Source monitoring and attribution
    uint64_t sourceId;
    std::string sourceType;
    float sourceConfidence;
    std::vector<float> sourceEncoding;
    
    // Memory specificity and precision
    float precision;
    float specificity;
    std::vector<float> featureWeights;
    std::vector<std::string> featureDescriptors;
    
    // Schema-based integration
    std::vector<std::string> associatedSchemas;
    std::vector<float> schemaActivation;
    
    // Emotional and reward associations
    float emotionalValence;
    float emotionalArousal;
    float rewardPrediction;
    
    // Interference and conflict
    std::unordered_set<uint64_t> interferers;
    float interferenceScore;
    
    // Consolidation state
    bool consolidated;
    bool active;
    SimulationStep consolidationTime;
    
    // Replay and consolidation markers
    SimulationStep lastReplay;
    float replayCount;
    float consolidationStrength;
    
    // Hippocampal-neocortical integration
    std::vector<NeuronId> hippocampalPattern;
    std::vector<NeuronId> neocorticalPattern;
    std::vector<float> hippocampalActivation;
    std::vector<float> neocorticalActivation;
    
    EpisodicMemoryItem(uint64_t itemId = 0)
        : id(itemId), timestamp(0), age(0), lastAccess(0), action(ActionType::Wait),
          reward(0), energy(0), novelty(0), predictionError(0),
          positionX(0), positionY(0), orientation(0), spatialContextRadius(0),
          temporalContextStart(0), temporalContextEnd(0), selfReferential(false),
          selfImportance(0), sourceConfidence(0), precision(0.5f), specificity(0.5f),
          emotionalValence(0), emotionalArousal(0), rewardPrediction(0),
          interferenceScore(0), consolidated(false), active(true),
          consolidationTime(0), lastReplay(0), replayCount(0),
          consolidationStrength(0.5f) {
        sensoryState.reserve(100);
        resultingSensoryState.reserve(100);
        neuronActivations.reserve(100);
        synapticWeights.reserve(100);
        causalStrengths.reserve(100);
        selfRepresentation.reserve(50);
        sourceEncoding.reserve(50);
        featureWeights.reserve(50);
        schemaActivation.reserve(20);
        hippocampalPattern.reserve(50);
        neocorticalPattern.reserve(200);
        hippocampalActivation.reserve(50);
        neocorticalActivation.reserve(200);
    }
};

// Temporal compression/expansion structure
struct TemporalCompression {
    SimulationStep originalStart;
    SimulationStep originalEnd;
    SimulationStep compressedStart;
    SimulationStep compressedEnd;
    float compressionRatio;
    float expansionFactor;
    
    TemporalCompression()
        : originalStart(0), originalEnd(0), compressedStart(0), compressedEnd(0),
          compressionRatio(1.0f), expansionFactor(1.0f) {}
};

// Memory prioritization structure
struct MemoryPriority {
    float temporalPriority;      // Importance based on recency
    float rewardPriority;        // Importance based on reward value
    float noveltyPriority;       // Importance based on novelty
    float selfPriority;          // Importance based on self-relevance
    float emotionalPriority;     // Importance based on emotional valence/arousal
    float schemaPriority;        // Importance based on schema activation
    float interferencePriority;  // Importance based on interference reduction
    
    float getOverallPriority() const {
        return (temporalPriority * 0.2f + rewardPriority * 0.2f + 
                noveltyPriority * 0.15f + selfPriority * 0.15f +
                emotionalPriority * 0.15f + schemaPriority * 0.1f +
                interferencePriority * 0.05f);
    }
    
    MemoryPriority()
        : temporalPriority(0), rewardPriority(0), noveltyPriority(0),
          selfPriority(0), emotionalPriority(0), schemaPriority(0),
          interferencePriority(0) {}
};

// Pattern completion state
struct PatternCompletionState {
    std::vector<float> currentPattern;
    std::vector<float> attractorState;
    std::vector<float> reconstruction;
    std::vector<float> completionConfidence;
    std::vector<uint64_t> completedMemories;
    bool converged;
    SimulationStep completionSteps;
    
    PatternCompletionState()
        : converged(false), completionSteps(0) {
        currentPattern.reserve(100);
        attractorState.reserve(100);
        reconstruction.reserve(100);
        completionConfidence.reserve(100);
    }
};

// NeuralEpisodicMemory: Sophisticated biological episodic memory system
// Implements hippocampal-neocortical interactions, replay during sleep/wake,
// rich contextual binding, pattern completion, and memory updating
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a new experience with rich contextual encoding
    // Implements event binding (what, where, when, how)
    void storeEpisode(const EpisodicMemoryItem& episode);
    
    // Retrieve episodes similar to a query pattern with pattern completion
    // Returns episodes ordered by contextual relevance and specificity
    std::vector<const EpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 10,
        float minRelevance = 0.3f) const;
    
    // Retrieve episodes from a specific time window with temporal expansion
    std::vector<const EpisodicMemoryItem*> retrieveTemporal(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 10) const;
    
    // Retrieve episodes by location with spatial context
    std::vector<const EpisodicMemoryItem*> retrieveByLocation(
        float x, float y, float radius,
        size_t maxResults = 10) const;
    
    // Retrieve episodes following a specific action with causal context
    std::vector<const EpisodicMemoryItem*> retrieveAfterAction(
        ActionType action, size_t maxResults = 10) const;
    
    // Retrieve episodes with specific contextual features
    std::vector<const EpisodicMemoryItem*> retrieveByContext(
        const std::vector<std::string>& featureDescriptors,
        float threshold = 0.5f,
        size_t maxResults = 10) const;
    
    // Replay an episode - reactivates neural patterns and triggers consolidation
    void replayEpisode(const EpisodicMemoryItem* episode);
    
    // Replay multiple episodes (for sleep-like consolidation)
    void replaySequence(const std::vector<uint64_t>& episodeIds);
    
    // Update episode relevance based on recent use and reactivation
    void updateRelevance(uint64_t episodeId, float relevanceDelta);
    
    // Update episode with new information (reconsolidation)
    void reconsolidate(uint64_t episodeId, const EpisodicMemoryItem& newInfo);
    
    // Get episode by index
    const EpisodicMemoryItem* getEpisode(size_t index) const;
    
    // Get episode by ID
    const EpisodicMemoryItem* getEpisodeById(uint64_t episodeId) const;
    
    // Get total number of episodes
    size_t getEpisodeCount() const { return episodes_.size(); }
    
    // Get recent episodes
    std::vector<const EpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    
    // Get average reward from episodes
    float getAverageReward() const;
    
    // Get most valuable episodes (for priority replay)
    std::vector<uint64_t> getHighPriorityEpisodes(size_t count) const;
    
    // Get episodes for replay (selection based on relevance and recency)
    std::vector<uint64_t> getEpisodesForReplay(size_t count) const;
    
    // Clear old episodes based on consolidation threshold
    void consolidate(float relevanceThreshold = 0.7f);
    
    // Clear all episodes
    void clear();
    
    // Enable/disable replay
    void enableReplay(bool enable) { replayEnabled_ = enable; }
    bool isReplayEnabled() const { return replayEnabled_; }
    
    // Enable/disable consolidation
    void enableConsolidation(bool enable) { consolidationEnabled_ = enable; }
    bool isConsolidationEnabled() const { return consolidationEnabled_; }
    
    // Set memory capacity
    void setCapacity(size_t capacity) { maxEpisodes_ = capacity; }
    size_t getCapacity() const { return maxEpisodes_; }
    
    // Configure temporal compression/expansion
    void setTemporalCompression(bool enable) { temporalCompressionEnabled_ = enable; }
    bool isTemporalCompressionEnabled() const { return temporalCompressionEnabled_; }
    void setExpansionFactor(float factor) { expansionFactor_ = factor; }
    
    // Configure hippocampal-neocortical interactions
    void setHippocampalNeocorticalMode(bool enable) { 
        hippocampalNeocorticalEnabled_ = enable; 
    }
    bool isHippocampalNeocorticalModeEnabled() const { return hippocampalNeocorticalEnabled_; }
    void setReplayFrequency(float frequency) { replayFrequency_ = frequency; }
    
    // Configure memory prioritization
    void setPrioritizationEnabled(bool enable) { prioritizationEnabled_ = enable; }
    bool isPrioritizationEnabled() const { return prioritizationEnabled_; }
    void setPriorityWeights(const MemoryPriority& weights) { priorityWeights_ = weights; }
    
    // Configure self-referential processing
    void setSelfReferentialProcessing(bool enable) { selfReferentialProcessing_ = enable; }
    bool isSelfReferentialProcessingEnabled() const { return selfReferentialProcessing_; }
    void setSelfImportanceThreshold(float threshold) { selfImportanceThreshold_ = threshold; }
    
    // Configure emotional integration
    void setEmotionalIntegrationEnabled(bool enable) { emotionalIntegrationEnabled_ = enable; }
    bool isEmotionalIntegrationEnabled() const { return emotionalIntegrationEnabled_; }
    
    // Configure pattern completion
    void setPatternCompletionEnabled(bool enable) { patternCompletionEnabled_ = enable; }
    bool isPatternCompletionEnabled() const { return patternCompletionEnabled_; }
    void setCompletionThreshold(float threshold) { completionThreshold_ = threshold; }
    
    // Configure interference management
    void setInterferenceManagementEnabled(bool enable) { interferenceManagementEnabled_ = enable; }
    bool isInterferenceManagementEnabled() const { return interferenceManagementEnabled_; }
    void setInterferenceThreshold(float threshold) { interferenceThreshold_ = threshold; }
    
    // Configure schema-based integration
    void setSchemaIntegrationEnabled(bool enable) { schemaIntegrationEnabled_ = enable; }
    bool isSchemaIntegrationEnabled() const { return schemaIntegrationEnabled_; }
    void setSchemaMatchingEnabled(bool enable) { schemaMatchingEnabled_ = enable; }
    bool isSchemaMatchingEnabled() const { return schemaMatchingEnabled_; }
    
    // Update memory system
    void update(SimulationStep step, Timestamp time);
    
    // Neural integration methods
    void integrateWithNeurons();
    void synchronizeWithHippocampalSystem();
    void updateNeocorticalIntegrations();
    
    // Statistics and diagnostics
    size_t getConsolidatedCount() const;
    size_t getActiveCount() const;
    float getAverageStrength() const;
    float getAveragePrecision() const;
    float getAverageRecall() const;
    
    // Debugging and utilities
    void logMemoryState() const;
    void printMemoryStats() const;
    std::string getMemorySummary() const;
    
private:
    // Core data structures
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core state
    Brain* brain_;
    std::deque<EpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    bool replayEnabled_;
    bool consolidationEnabled_;
    bool temporalCompressionEnabled_;
    bool hippocampalNeocorticalEnabled_;
    bool prioritizationEnabled_;
    bool selfReferentialProcessing_;
    bool emotionalIntegrationEnabled_;
    bool patternCompletionEnabled_;
    bool interferenceManagementEnabled_;
    bool schemaIntegrationEnabled_;
    bool schemaMatchingEnabled_;
    
    // Configuration parameters
    float expansionFactor_;
    float replayFrequency_;
    float completionThreshold_;
    float interferenceThreshold_;
    float selfImportanceThreshold_;
    MemoryPriority priorityWeights_;
    
    // Neural dynamics
    float currentReconstructionEnergy_;
    SimulationStep lastReplayStep_;
    SimulationStep lastConsolidationStep_;
    
    // Temporal compression/expansion management
    std::deque<TemporalCompression> temporalCompressions_;
    
    // Pattern completion state
    PatternCompletionState patternCompletionState_;
    
    // Source monitoring and attribution
    std::unordered_map<uint64_t, std::vector<float>> sourceTrajectories_;
    
    // Schema integration
    std::unordered_map<std::string, std::vector<float>> schemaPrototypes_;
    std::vector<std::pair<uint64_t, std::string>> schemaBindings_;
    
    // Statistical tracking
    size_t totalConsolidations_;
    size_t totalReplays_;
    size_t totalUpdates_;
    float averageEncodingStrength_;
    float averageRecallAccuracy_;
    
    // Neural integration methods
    float computeContextualBinding(const EpisodicMemoryItem& episode) const;
    void encodeEventWithContext(const EpisodicMemoryItem& episode);
    void updateTemporalContext(EpisodicMemoryItem& episode) const;
    void updateSpatialContext(EpisodicMemoryItem& episode) const;
    void updateSelfReferentialContext(EpisodicMemoryItem& episode) const;
    void updateEmotionalContext(EpisodicMemoryItem& episode) const;
    void updateSchemaContext(EpisodicMemoryItem& episode) const;
    
    // Memory retrieval methods
    float computeContextualSimilarity(
        const std::vector<float>& query,
        const EpisodicMemoryItem& episode) const;
    
    bool matchesContextualCriteria(
        const EpisodicMemoryItem& episode,
        const std::vector<float>* sensoryQuery,
        const std::vector<std::string>* featureQuery,
        SimulationStep* startTime,
        SimulationStep* endTime,
        ActionType* actionQuery,
        float minRelevance) const;
    
    // Memory consolidation and replay
    void triggerReplay(EpisodicMemoryItem& episode);
    void consolidateWithNeocortex(EpisodicMemoryItem& episode);
    void updateMemoryTrace(EpisodicMemoryItem& episode);
    
    // Pattern completion and attractor dynamics
    void activateAttractorDynamics(const std::vector<float>& pattern);
    void completePattern(const std::vector<float>& partialPattern,
                        std::vector<float>& completed) const;
    
    // Interference management
    void detectAndResolveInterference(EpisodicMemoryItem& episode);
    float computeInterferenceScore(const EpisodicMemoryItem& episode) const;
    void reduceInterference(const EpisodicMemoryItem& source, 
                           EpisodicMemoryItem& target) const;
    
    // Temporal compression/expansion
    void applyTemporalCompression(EpisodicMemoryItem& episode);
    void applyTemporalExpansion(EpisodicMemoryItem& episode) const;
    
    // Self-referential processing
    void encodeSelfReferentialContent(EpisodicMemoryItem& episode);
    float computeSelfImportance(const EpisodicMemoryItem& episode) const;
    
    // Schema-based integration
    void matchToSchemas(EpisodicMemoryItem& episode);
    void activateSchemas(const std::vector<std::string>& schemas,
                         EpisodicMemoryItem& episode);
    
    // Emotional integration
    void integrateEmotionalContent(EpisodicMemoryItem& episode);
    float computeEmotional salience(const EpisodicMemoryItem& episode) const;
    
    // Hippocampal-neocortical interactions
    void transferToHippocampalIndex(const EpisodicMemoryItem& episode);
    void updateNeocorticalRepresentation(const EpisodicMemoryItem& episode);
    void synchronizeWithNeocorticalActivity();
    
    // Prioritization and gating
    float computeMemoryPriority(const EpisodicMemoryItem& episode) const;
    void updateMemoryPriority(EpisodicMemoryItem& episode);
    void gateMemoryStorage(EpisodicMemoryItem& episode);
    
    // Neural encoding and plasticity
    void encodeAsNeuralPattern(EpisodicMemoryItem& episode);
    void updateSynapticStrength(const EpisodicMemoryItem& source,
                               const EpisodicMemoryItem& target);
    void applyPatternCompletion(const std::vector<float>& pattern,
                               EpisodicMemoryItem& episode);
    
    // Statistical tracking
    void updateStatistics();
    void trackEncodingQuality(const EpisodicMemoryItem& episode);
    void trackRecallAccuracy(const std::vector<const EpisodicMemoryItem*>& retrieved) const;
    
    // Cleanup and maintenance
    void cleanupInactiveEpisodes();
    void compressOldEpisodes();
    void updateEpisodeAges();
    
    // Memory management
    void enforceCapacityLimits();
    bool shouldStoreEpisode(const EpisodicMemoryItem& episode) const;
    void removeEpisode(size_t index);
    void removeEpisodeById(uint64_t episodeId);
    
    // Random number generation utilities
    static float getRandomFloat(float min, float max) {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(rng);
    }
    
    static int getRandomInt(int min, int max) {
        static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }
};
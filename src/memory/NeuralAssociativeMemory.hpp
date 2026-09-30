#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include <queue>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

// Enhanced episodic memory item with multimodal components for Phase 6
struct EnhancedEpisodicMemoryItem {
    SimulationStep timestamp;
    
    // Modalities: sensory, motor, reward, emotional
    struct {
        std::vector<float> sensoryState;    // Visual, auditory, tactile, proprioceptive
        std::vector<float> motorState;     // Action executed
        float reward;                      // Reward prediction error
        float emotion;                     // Emotional valence
        float novelty;                     // Novelty detection
        float energy;                      // Energy levels
        float attention;                   // Attentional focus
    } modalities;
    
    // Temporal information
    SimulationStep episodeAge;
    std::vector<SimulationStep> temporalSequence;
    
    // Context information
    float positionX;
    float positionY;
    float orientation;
    std::string locationTag;
    
    // Neural representation
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    std::vector<NeuronId> replayNeurons;
    
    // Meta-information
    float relevanceScore;
    float consolidationStrength;
    bool isConsolidated;
    SimulationStep lastReplayStep;
    std::vector<size_t> relatedEpisodeIds;
    
    EnhancedEpisodicMemoryItem()
        : timestamp(0), episodeAge(0), relevanceScore(0.0f), 
          consolidationStrength(0.0f), isConsolidated(false),
          lastReplayStep(0), positionX(0), positionY(0), orientation(0) {
        modalities.reward = 0.0f;
        modalities.emotion = 0.0f;
        modalities.novelty = 0.0f;
        modalities.energy = 0.0f;
        modalities.attention = 0.0f;
    }
};

// Associative memory link between neural patterns and concepts
struct NeuralAssociation {
    std::vector<float> patternA;
    std::vector<float> patternB;
    std::string conceptA;
    std::string conceptB;
    float strength;
    float lastActivated;
    std::vector<float> activationHistory;
    
    NeuralAssociation(const std::vector<float>& a, const std::vector<float>& b,
                      const std::string& cA, const std::string& cB, float s)
        : patternA(a), patternB(b), conceptA(cA), conceptB(cB), 
          strength(s), lastActivated(0.0f) {}
};

// NeuralEpisodicMemory: Enhanced with real-time capture and temporal compression
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    void initialize(Brain* brain);
    
    // Enhanced episodic memory storage with real-time capture
    void storeEpisode(const EnhancedEpisodicMemoryItem& episode);
    
    // Multimodal retrieval with temporal compression
    std::vector<const EnhancedEpisodicMemoryItem*> retrieveSimilar(
        const std::vector<float>& sensoryPattern,
        size_t maxResults = 5,
        float relevanceThreshold = 0.3f) const;
    
    // Context-dependent retrieval
    std::vector<const EnhancedEpisodicMemoryItem*> retrieveByContext(
        float timeWindow = 0.0f,
        float spatialRadius = 0.0f,
        const std::string& locationTag = "",
        const std::string& actionTag = "",
        size_t maxResults = 10) const;
    
    // Temporal sequencing and chronological ordering
    std::vector<const EnhancedEpisodicMemoryItem*> retrieveTemporalSequence(
        SimulationStep startTime,
        SimulationStep endTime,
        size_t maxResults = 20) const;
    
    // Enhanced replay with pattern completion
    void replayEpisode(const EnhancedEpisodicMemoryItem* episode);
    void replaySequence(const std::vector<size_t>& episodeIds);
    
    // Temporal compression algorithms for efficient storage
    void applyTemporalCompression();
    void compressRecentEpisodes();
    
    // Relevance-based prioritization and filtering
    void updateRelevance(size_t episodeId, float delta);
    void filterByRelevance(float threshold);
    void prioritizeRecentEpisodes(float decayFactor);
    
    // Automatic consolidation and cleanup
    void consolidate(float relevanceThreshold);
    void cleanupOldEpisodes(size_t maxEpisodes);
    void consolidateSleepCycles();
    
    // Memory optimization
    void compressMemory() { applyTemporalCompression(); }
    void optimizeRetrieval() { optimizeIndexStructures(); }
    
    // Get episodic statistics
    const EnhancedEpisodicMemoryItem* getEpisode(size_t index) const;
    size_t getEpisodeCount() const { return episodes_.size(); }
    float getTotalStorageSize() const { return totalStorageSize_; }
    
    // Query and monitoring
    std::vector<const EnhancedEpisodicMemoryItem*> getRecentEpisodes(size_t count) const;
    std::vector<const EnhancedEpisodicMemoryItem*> getRelevantEpisodes(float minRelevance) const;
    float getAverageReward() const;
    float getMemoryActivityLevel() const;
    
    // Memory-guided prediction
    std::vector<float> predictFuturePattern(const EnhancedEpisodicMemoryItem* lastEpisode) const;
    
    // Integration with working memory
    void updateWorkingMemoryFromEpisode(size_t episodeId, NeuralWorkingMemory* workingMem);
    
    // Schema formation and abstraction
    void extractSchemas();
    void createGeneralizations();
    
    // Emotional regulation through memory processing
    void processEmotions();
    void regulateEmotionalResponses();
    
    // Clearing and reset
    void clear() { episodes_.clear(); replayIndex_.clear(); }
    void resetCompressionStructures() { compressedTemporalIndex_.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::deque<EnhancedEpisodicMemoryItem> episodes_;
    size_t maxEpisodes_;
    float temporalCompressionRatio_;
    float relevanceDecayRate_;
    bool replayEnabled_;
    
    // Temporal indexing for efficient retrieval
    std::vector<std::pair<SimulationStep, size_t>> temporalIndex_;
    std::unordered_map<std::string, std::vector<size_t>> spatialIndex_;
    std::unordered_map<std::string, std::vector<size_t>> actionIndex_;
    
    // Compression structures
    std::deque<size_t> compressedTemporalIndex_;
    std::vector<std::pair<float, size_t>> relevanceIndex_;
    
    // Total storage metrics
    float totalStorageSize_;
    float peakStorageSize_;
    SimulationStep lastConsolidationStep_;
    
    // Helper methods
    void buildTemporalIndex();
    void buildSpatialIndex();
    void buildActionIndex();
    void optimizeIndexStructures();
    void compressEpisode(EnhancedEpisodicMemoryItem& episode);
    void mergeSimilarEpisodes();
    void generateSchemaFromEpisodes();
};

// NeuralWorkingMemory: Enhanced with episodic integration
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    void initialize(Brain* brain);
    
    // Enhanced working memory with episodic competition
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Integration with episodic memory for pattern completion
    void integrateWithEpisodicMemory(NeuralEpisodicMemory* episodicMem);
    void retrieveEpisodicSupport();
    
    // Enhanced rehearsal mechanisms
    void rehearse(EpisodeSupportType type = EpisodeSupportType::ActiveRehearsal);
    void strengthenMemory(float factor);
    void updateRehearsalStrength();
    
    // Novelty-based attentional capture from episodic memory
    void detectNoveltyFromEpisodic(NeuralEpisodicMemory* episodicMem);
    void applyNoveltyBoost(float noveltyLevel);
    
    // Memory-guided prediction and planning
    void predictFromEpisodic(NeuralEpisodicMemory* episodicMem);
    void planBasedOnPastExperience();
    
    // Feedback loops from working memory to episodic storage
    void updateEpisodicRelevance(NeuralEpisodicMemory* episodicMem);
    void storeToEpisodicMemory(NeuralEpisodicMemory* episodicMem);
    
    // Temporal sequencing and pattern completion
    void maintainTemporalSequence();
    void completePatternsFromEpisodic();
    
    std::vector<float> retrieve() const;
    bool contains(NeuronId neuron) const;
    float getNeuronActivation(NeuronId neuron) const;
    void update(TimestepDuration dt);
    void clear();
    
    // Competition and selection mechanisms
    void runCompetition();
    void selectWinner();
    void applyInhibition();
    void enhanceWinners(float factor);
    
    // Capacity management
    void enforceCapacity();
    void evictWeakTraces();
    void compressWorkingMemory();
    
    // Metrics and statistics
    size_t getActiveTraces() const { return activeTraces_.size(); }
    size_t getCapacity() const { return capacity_; }
    float getMemoryActivity() const;
    float getNoveltyLevel() const { return noveltyLevel_; }
    std::vector<NeuronId> getMemoryNeurons() const { return memoryNeurons_; }
    
    // State management
    void pauseRehearsal() { rehearsalPaused_ = true; }
    void resumeRehearsal() { rehearsalPaused_ = false; }
    bool isRehearsalPaused() const { return rehearsalPaused_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    float noveltyDecayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<SimulationStep> rehearsalTimestamps_;
    std::vector<float> rehearsalStrengths_;
    
    // Competition and attention
    std::vector<NeuronId> winners_;
    std::vector<float> noveltySignals_;
    float noveltyLevel_;
    std::vector<float> attentionalWeights_;
    
    // Temporal sequencing
    std::vector<size_t> temporalSequence_;
    std::vector<float> patternCompletionScores_;
    
    // Episodic integration
    NeuralEpisodicMemory* episodicMemory_;
    std::vector<size_t> supportingEpisodicIds_;
    std::vector<float> episodicSupportStrengths_;
    
    // Control mechanisms
    bool rehearsalPaused_;
    bool noveltyBoostEnabled_;
    bool patternCompletionEnabled_;
    float patternCompletionThreshold_;
};

// NeuralAssociativeMemory: Cross-episode associations and pattern completion
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    void initialize(Brain* brain);
    
    // Enhanced association creation with cross-episode linking
    void associate(const std::vector<float>& patternA,
                   const std::vector<float>& patternB,
                   float strength = 1.0f);
    void associateFromExperience(const EnhancedEpisodicMemoryItem& episode);
    
    // Pattern completion using distributed representations
    std::vector<std::vector<float>> retrieve(
        const std::vector<float>& queryPattern,
        size_t maxResults = 5,
        float similarityThreshold = 0.3f) const;
    
    // Spreading activation for associative recall
    std::vector<std::vector<float>> spreadActivation(
        const std::vector<float>& cuePattern,
        size_t steps = 3,
        float activationThreshold = 0.1f) const;
    
    // Cross-episode semantic abstraction
    std::vector<std::string> extractConcepts(
        const std::vector<const EnhancedEpisodicMemoryItem*>& episodes) const;
    void createSemanticLinks(const std::vector<std::string>& concepts);
    
    // Schema formation from associated patterns
    void buildSchemaFromAssociations();
    std::vector<std::vector<float>> getSchemaPatterns() const;
    
    // Memory-guided exploration and exploitation
    std::vector<float> suggestNovelPattern() const;
    std::vector<float> suggestExploitationPattern() const;
    
    // Metacognitive monitoring using memory
    float monitorLearningProgress() const;
    float evaluatePredictionAccuracy() const;
    
    // Memory-based decision making
    size_t selectBasedOnMemory(const std::vector<std::vector<float>>& options) const;
    float evaluateOptionWithMemory(const std::vector<float>& option) const;
    
    // Adaptive strategies based on memory statistics
    void adaptStrategyBasedOnMemoryStats();
    void balanceExplorationExploitation();
    
    // Clearing and reset
    void clear();
    void resetAssociationWeights();
    
    // Metrics and statistics
    size_t getAssociationCount() const { return associations_.size(); }
    float getAverageAssociationStrength() const;
    std::vector<std::string> getActiveConcepts() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    std::vector<NeuralAssociation> associations_;
    std::vector<std::vector<float>> patternBank_;
    std::unordered_map<std::string, std::vector<std::vector<float>>> conceptPatterns_;
    std::vector<std::string> activeConcepts_;
    
    // Spreading activation network
    std::unordered_map<size_t, float> activationLevels_;
    std::vector<std::pair<size_t, float>> activationQueue_;
    
    // Schema and concept management
    std::vector<std::vector<float>> schemas_;
    std::unordered_map<std::string, std::vector<size_t>> conceptToPatternMap_;
    
    // Learning metrics
    float totalAssociationStrength_;
    SimulationStep lastLearningStep_;
    float explorationBias_;
    float exploitationBias_;
    
    // Helper methods
    size_t findPatternNeuron(const std::vector<float>& pattern) const;
    float computeSimilarity(const std::vector<float>& a,
                           const std::vector<float>& b) const;
    void updateActivationLevels(const std::vector<size_t>& activated);
    void buildConceptAssociations();
    void updateSchemaFromAssociations();
    void balanceExplorationExploitation();
};

} // namespace nlm
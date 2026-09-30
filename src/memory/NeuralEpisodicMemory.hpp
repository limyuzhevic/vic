#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <deque>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

// EnhancedEpisodicMemoryItem: Represents a stored experience with multimodal components
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

// NeuralEpisodicMemory: Enhanced with real-time capture and temporal compression
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    void initialize(Brain* brain);
    
    // Enhanced episodic memory storage with real-time capture
    void storeEpisode(const EnhancedEpisodicMemoryItem& episode);
    void storeWithCompression(const EnhancedEpisodicMemoryItem& episode);
    
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
    void replayWithConsolidation(const std::vector<size_t>& episodeIds);
    
    // Temporal compression algorithms for efficient storage
    void applyTemporalCompression();
    void compressRecentEpisodes();
    void compressBasedOnSimilarity();
    
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
    float getCompressionRatio() const { return compressionRatio_; }
    
    // Memory-guided prediction
    std::vector<float> predictFuturePattern(const EnhancedEpisodicMemoryItem* lastEpisode) const;
    
    // Integration with working memory
    void updateWorkingMemoryFromEpisode(size_t episodeId, NeuralWorkingMemory* workingMem);
    void replayForWorkingMemory(NeuralWorkingMemory* workingMem);
    
    // Schema formation and abstraction
    void extractSchemas();
    void createGeneralizations();
    std::vector<std::string> getExtractedSchemas() const { return extractedSchemas_; }
    
    // Emotional regulation through memory processing
    void processEmotions();
    void regulateEmotionalResponses();
    float getAverageEmotionalValence() const;
    
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
    float compressionRatio_;
    
    // Schema and concept tracking
    std::vector<std::string> extractedSchemas_;
    std::unordered_map<std::string, std::vector<size_t>> schemaEpisodeMap_;
    
    // Helper methods
    void buildTemporalIndex();
    void buildSpatialIndex();
    void buildActionIndex();
    void optimizeIndexStructures();
    void compressEpisode(EnhancedEpisodicMemoryItem& episode);
    void mergeSimilarEpisodes();
    void generateSchemaFromEpisodes();
    void updateCompressionMetrics();
    void consolidateWithReplay();
};

} // namespace nlm
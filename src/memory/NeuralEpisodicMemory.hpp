#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <random>
#include <cmath>

namespace nlm {

// Enhanced episodic memory with replay mechanisms
class EpisodicMemory {
public:
    EpisodicMemory();
    ~EpisodicMemory();
    
    // Store episode with rich contextual information
    void storeEpisode(const EpisodicMemoryItem& episode);
    void storeEpisodeWithContext(const std::vector<NeuronId>& neurons, 
                                const std::vector<float>& values,
                                const std::string& context = "",
                                SimulationStep currentStep = 0);
    
    // Enhanced retrieval capabilities
    EpisodicMemoryItem retrieveEpisode(size_t index) const;
    std::vector<EpisodicMemoryItem> retrieveEpisodesByTime(SimulationStep startTime, 
                                                           SimulationStep endTime) const;
    std::vector<EpisodicMemoryItem> retrieveEpisodesByContext(const std::string& context) const;
    std::vector<EpisodicMemoryItem> retrieveSimilarEpisodes(const std::vector<NeuronId>& pattern, 
                                                            float similarityThreshold = 0.7f) const;
    
    // Get episode statistics
    size_t getEpisodeCount() const;
    SimulationStep getOldestEpisodeTime() const;
    SimulationStep getNewestEpisodeTime() const;
    float getTemporalCoherence() const;
    
    // Enhanced consolidation with multiple mechanisms
    void consolidate(float relevanceThreshold, float noveltyThreshold = 0.5f);
    void replayRecentEpisodes(float replayStrength = 0.8f);
    void replayWithStrengthening(float baseStrength = 0.3f);
    
    // Interference management
    void resolveTemporalInterference();
    void resolveContentInterference();
    
    // Recent episodes management
    std::vector<EpisodicMemoryItem> getRecentEpisodes(size_t count) const;
    void pruneOldEpisodes(size_t maxCount);
    
    // Statistical analysis
    float getMeanNeuronActivation(const std::string& context) const;
    std::vector<float> getActivationStatistics(const std::string& context) const;
    
    // Clear all
    void clear();
    
    // Memory statistics
    float getConsolidationProgress() const;
    float getReplayEffectiveness() const;
    float getTemporalCompressionRatio() const;
    
    // Configuration
    void setMaxEpisodes(size_t max);
    void setConsolidationInterval(SimulationStep interval);
    void setReplayFrequency(float frequency);
    void setSimilarityThreshold(float threshold);
    
private:
    struct EpisodicMemoryItemEnhanced {
        SimulationStep timestamp;
        std::vector<NeuronId> neurons;
        std::vector<float> values;
        std::string context;
        float noveltyScore;
        float relevanceScore;
        size_t accessCount;
        SimulationStep lastAccessTime;
        float temporalCompression;
        
        EpisodicMemoryItemEnhanced() : timestamp(0), noveltyScore(0.0f), relevanceScore(0.0f), 
                                       accessCount(0), lastAccessTime(0), temporalCompression(1.0f) {}
    };
    
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
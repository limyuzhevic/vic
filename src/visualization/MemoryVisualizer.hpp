#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <deque>
#include <algorithm>
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

// Working memory activation pattern
struct WorkingMemoryActivation {
    Timestamp timestamp;
    MemoryId id;
    std::string content;
    float activationLevel;
    float stability;
    float decayRate;
    std::vector<NeuronId> activeNeurons;
    std::map<NeuronId, float> activationByNeuron;
    std::string originRegion;
};

// Episodic memory episode
struct EpisodicEpisode {
    Timestamp timestamp;
    EpisodeId id;
    std::string description;
    float importance;
    float strength;
    std::vector<MemoryId> relatedMemories;
    std::vector<NeuronId> encodingNeurons;
    std::vector<NeuronId> recallNeurons;
    Timestamp firstRecallTime;
    Timestamp lastRecallTime;
    uint32_t recallCount;
    float consolidationLevel;
    std::string context;
};

// Associative memory representation
struct AssociativeMemory {
    MemoryId id;
    std::string key;
    std::string value;
    float strength;
    float elasticity;
    float activationThreshold;
    std::vector<NeuronId> keyNeurons;
    std::vector<NeuronId> valueNeurons;
    std::map<std::string, float> metadata;
    Timestamp lastAccessed;
    uint32_t accessCount;
    float decayFactor;
};

// Memory system statistics
struct MemoryStatistics {
    Timestamp timestamp;
    float workingMemoryUtilization;
    float episodicMemoryConsolidation;
    float associativeMemoryActivation;
    uint32_t totalWorkingMemories;
    uint32_t totalEpisodicEpisodes;
    uint32_t totalAssociativeMemories;
    float averageMemoryStability;
    float memoryCohesion; // Measure of memory integration
    std::map<RegionId, float> regionMemoryContribution;
};

class MemoryVisualizer {
public:
    MemoryVisualizer();
    ~MemoryVisualizer();
    
    // Initialize visualizer
    bool initialize();
    
    // Update memory data
    void update(double deltaTime);
    
    // Record working memory activation
    void recordWorkingMemoryActivation(const WorkingMemoryActivation& activation);
    void recordWorkingMemoryActivations(const std::vector<WorkingMemoryActivation>& activations);
    
    // Record episodic memory episode
    void recordEpisodicEpisode(const EpisodicEpisode& episode);
    void recordEpisodicEpisodes(const std::vector<EpisodicEpisode>& episodes);
    
    // Record associative memory
    void recordAssociativeMemory(const AssociativeMemory& memory);
    void recordAssociativeMemories(const std::vector<AssociativeMemory>& memories);
    
    // Update memory statistics
    void updateMemoryStatistics(const MemoryStatistics& stats);
    
    // Render working memory
    void renderWorkingMemory(const std::vector<WorkingMemoryActivation>& activations);
    void renderWorkingMemory() { renderWorkingMemory(currentWorkingMemories); }
    
    // Render episodic memory
    void renderEpisodicMemory(const std::vector<EpisodicEpisode>& episodes);
    void renderEpisodicMemory() { renderEpisodicMemory(currentEpisodicEpisodes); }
    
    // Render associative memory
    void renderAssociativeMemory(const std::vector<AssociativeMemory>& memories);
    void renderAssociativeMemory() { renderAssociativeMemory(currentAssociativeMemories); }
    
    // Render memory system statistics
    void renderMemoryStatistics(const MemoryStatistics& stats);
    void renderMemoryStatistics() { renderMemoryStatistics(currentMemoryStats); }
    
    // Configuration
    void setWorkingMemoryHistorySize(size_t size);
    void setEpisodicMemoryHistorySize(size_t size);
    void setAssociativeMemoryHistorySize(size_t size);
    void setAutoUpdate(bool autoUpdate);
    
    // Data access
    const std::vector<WorkingMemoryActivation>& getCurrentWorkingMemories() const { return currentWorkingMemories; }
    const std::vector<EpisodicEpisode>& getCurrentEpisodicEpisodes() const { return currentEpisodicEpisodes; }
    const std::vector<AssociativeMemory>& getCurrentAssociativeMemories() const { return currentAssociativeMemories; }
    const MemoryStatistics& getCurrentMemoryStatistics() const { return currentMemoryStats; }
    
    // Statistics
    float getWorkingMemoryUtilization() const;
    float getEpisodicMemoryConsolidation() const;
    float getAssociativeMemoryActivation() const;
    uint32_t getTotalMemoryItems() const;
    float getMemoryCohesion() const;
    
    // Export capabilities
    bool exportWorkingMemoryData(const std::string& filename);
    bool exportEpisodicMemoryData(const std::string& filename);
    bool exportAssociativeMemoryData(const std::string& filename);
    bool exportMemoryStatistics(const std::string& filename);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
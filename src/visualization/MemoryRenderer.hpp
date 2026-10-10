#pragma once

#include "Brain.hpp"
#include "ColorManager.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

class MemoryRenderer {
public:
    enum class MemoryViewMode {
        WorkingMemory,
        EpisodicMemory,
        SemanticMemory,
        ProceduralMemory,
        AssociativeMemory,
        Consolidated
    };
    
    enum class MemoryAnalysisType {
        None,
        Capacity,
        Strength,
        Recency,
        Frequency,
        Coherence
    };
    
    struct MemoryEntry {
        enum class Type {
            Working,
            Episodic,
            Semantic,
            Procedural,
            Associative
        };
        
        Type type;
        std::string id;
        std::string content;
        float strength;
        Timestamp timestamp;
        SimulationStep step;
        size_t neuronId;
        std::string sourceId;
        std::string targetId;
        
        MemoryEntry() : type(Type::Working), strength(0.0f), timestamp(0.0f), step(0), neuronId(0) {}
        MemoryEntry(Type t, const std::string& id, const std::string& content, 
                   float strength, Timestamp ts, SimulationStep s, size_t nid = 0)
            : type(t), id(id), content(content), strength(strength), 
              timestamp(ts), step(s), neuronId(nid) {}
    };
    
    struct MemoryStats {
        size_t totalEntries;
        float totalStrength;
        float avgStrength;
        size_t workingMemoryCount;
        size_t episodicMemoryCount;
        size_t semanticMemoryCount;
        size_t proceduralMemoryCount;
        size_t associativeMemoryCount;
        float capacityUsage;
        float consolidationLevel;
        
        MemoryStats() : totalEntries(0), totalStrength(0.0f), avgStrength(0.0f),
                       workingMemoryCount(0), episodicMemoryCount(0), semanticMemoryCount(0),
                       proceduralMemoryCount(0), associativeMemoryCount(0),
                       capacityUsage(0.0f), consolidationLevel(0.0f) {}
    };
    
    MemoryRenderer();
    ~MemoryRenderer() = default;
    
    void setMemoryViewMode(MemoryViewMode mode) { memoryViewMode_ = mode; }
    void setMemoryAnalysisType(MemoryAnalysisType analysis) { memoryAnalysisType_ = analysis; }
    void setShowMemoryLabels(bool show) { showMemoryLabels_ = show; }
    void setShowStrengthIndicators(bool show) { showStrengthIndicators_ = show; }
    void setShowAgeGradient(bool show) { showAgeGradient_ = show; }
    void setConsolidationThreshold(float threshold) { consolidationThreshold_ = threshold; }
    
    void render(const Brain& brain);
    void update(const Brain& brain);
    
    // Working memory visualization
    void renderWorkingMemory(const Brain& brain);
    void renderMemoryTraces(const Brain& brain);
    void renderCompetitionDynamics(const Brain& brain);
    
    // Episodic memory visualization
    void renderEpisodicMemory(const Brain& brain);
    void renderEpisodeTimeline(const Brain& brain);
    void renderMemoryReplay(const Brain& brain);
    
    // Semantic memory visualization
    void renderSemanticMemory(const Brain& brain);
    void renderConceptNetwork(const Brain& brain);
    void renderKnowledgeGraph(const Brain& brain);
    
    // Procedural memory visualization
    void renderProceduralMemory(const Brain& brain);
    void renderSkillHierarchy(const Brain& brain);
    void renderActionSequences(const Brain& brain);
    
    // Associative memory visualization
    void renderAssociativeMemory(const Brain& brain);
    void renderAssociationWeights(const Brain& brain);
    void renderPatternCompletion(const Brain& brain);
    
    // Consolidated memory visualization
    void renderConsolidatedMemory(const Brain& brain);
    void renderMemoryConsolidation(const Brain& brain);
    void renderMemoryIntegration(const Brain& brain);
    
    // Interactive features
    void setSelectedMemoryEntry(const std::string& entryId) { selectedMemoryEntry_ = entryId; }
    void highlightMemoryEntry(const std::string& entryId) { highlightedMemoryEntries_.insert(entryId); }
    void clearHighlights() { highlightedMemoryEntries_.clear(); }
    void setShowOnlySelected(bool show) { showOnlySelected_ = show; }
    
    // Data management
    void collectMemoryData(const Brain& brain);
    void updateMemoryStatistics();
    void updateMemoryTimeline();
    
    // Export and analysis
    std::string exportMemoryData() const;
    void generateMemoryAnalysis() const;
    
    // Configuration
    void loadConfiguration(const std::string& configFile);
    void saveConfiguration() const;
    
    // Utility functions
    float getCurrentMemoryUsage() const { return currentMemoryUsage_; }
    size_t getTotalMemoryEntries() const { return memoryEntries_.size(); }
    float getConsolidationProgress() const { return consolidationProgress_; }
    
private:
    MemoryViewMode memoryViewMode_;
    MemoryAnalysisType memoryAnalysisType_;
    bool showMemoryLabels_;
    bool showStrengthIndicators_;
    bool showAgeGradient_;
    float consolidationThreshold_;
    
    std::vector<MemoryEntry> memoryEntries_;
    MemoryStats memoryStats_;
    
    // Interactive state
    std::string selectedMemoryEntry_;
    std::unordered_set<std::string> highlightedMemoryEntries_;
    bool showOnlySelected_;
    
    // Timeline and progression
    std::vector<Timestamp> memoryTimeline_;
    std::vector<float> memoryStrength_;
    float currentMemoryUsage_;
    float consolidationProgress_;
    
    // Configuration
    size_t maxMemoryEntries_;
    float memoryDecayRate_;
    bool autoConsolidation_;
    std::string colorScheme_;
    
    void initializeDefaultConfiguration();
    void calculateMemoryStatistics();
    void updateMemoryTimeline();
    std::string getMemoryEntryColor(const MemoryEntry& entry) const;
    std::string getMemoryStrengthColor(float strength) const;
    std::string getMemoryAgeColor(float age) const;
    
    // Rendering helpers
    void renderMemoryGrid(const std::vector<MemoryEntry>& entries, int x, int y, int width, int height);
    void renderMemoryTree(const std::vector<MemoryEntry>& entries, int x, int y);
    void renderMemoryNetwork(const std::vector<MemoryEntry>& entries, int x, int y);
    void renderMemoryTimeline(const std::vector<MemoryEntry>& entries);
    void renderMemoryStats() const;
    
    // Analysis functions
    void analyzeMemoryCapacity(const Brain& brain);
    void analyzeMemoryStrength(const Brain& brain);
    void analyzeMemoryRecency(const Brain& brain);
    void analyzeMemoryCoherence(const Brain& brain);
    
    // Data extraction from brain memory systems
    void extractWorkingMemoryData(const Brain& brain);
    void extractEpisodicMemoryData(const Brain& brain);
    void extractSemanticMemoryData(const Brain& brain);
    void extractProceduralMemoryData(const Brain& brain);
    void extractAssociativeMemoryData(const Brain& brain);
    
    // Utility functions
    float calculateMemoryAge(Timestamp timestamp) const;
    float calculateMemoryRecencyWeight(Timestamp timestamp) const;
    std::string formatMemoryContent(const MemoryEntry& entry) const;
};

} // namespace nlm

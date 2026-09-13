// ============================================================================
// Memory Visualization Components
// ============================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../core/Types/Types.hpp"
#include "../memory/Memory.hpp"
#include "VisualizationBase.hpp"

namespace nlm {

/**
 * @class MemoryVisualizer
 * @brief Memory system state visualization
 * 
 * This visualizer provides comprehensive visualization of memory system state,
 * including memory capacity, consolidation, retrieval, and interference patterns.
 */
class MemoryVisualizer : public VisualizationBase {
public:
    MemoryVisualizer();
    ~MemoryVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // MemoryVisualizer specific methods
    void setShowMemoryCapacity(bool show) { showMemoryCapacity_ = show; }
    void setShowMemoryTraces(bool show) { showMemoryTraces_ = show; }
    void setShowConsolidation(bool show) { showConsolidation_ = show; }
    void setShowInterference(bool show) { showInterference_ = show; }
    void setShowReplay(bool show) { showReplay_ = show; }
    
    // Data access
    double getMemoryUtilization() const { return memoryUtilization_; }
    uint64_t getTotalMemoryEntries() const { return totalMemoryEntries_; }
    uint64_t getConsolidationEvents() const { return consolidationEvents_; }
    uint32_t getReplayFrequency() const { return replayFrequency_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    class MemorySystem* memory_ = nullptr;
    
    // Visualization state
    bool showMemoryCapacity_ = true;
    bool showMemoryTraces_ = true;
    bool showConsolidation_ = true;
    bool showInterference_ = true;
    bool showReplay_ = true;
    
    // Memory statistics
    double memoryUtilization_ = 0.0;
    uint64_t totalMemoryEntries_ = 0;
    uint64_t consolidationEvents_ = 0;
    uint32_t replayFrequency_ = 0;
    uint32_t replayHistorySize_ = 100;
    std::vector<uint32_t> replayHistory_;
};

/**
 * @class WorkingMemoryVisualizer
 * @brief Active memory traces visualization
 * 
 * This visualizer focuses on the dynamic nature of working memory,
 * showing active traces, their strength, decay, and interference patterns.
 */
class WorkingMemoryVisualizer : public VisualizationBase {
public:
    WorkingMemoryVisualizer();
    ~WorkingMemoryVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // WorkingMemoryVisualizer specific methods
    void setShowActiveTraces(bool show) { showActiveTraces_ = show; }
    void setShowTraceStrength(bool show) { showTraceStrength_ = show; }
    void setShowTraceDecay(bool show) { showTraceDecay_ = show; }
    void setShowInterference(bool show) { showInterference_ = show; }
    void setShowChunking(bool show) { showChunking_ = show; }
    void setShowCapacityLimits(bool show) { showCapacityLimits_ = show; }
    
    // Data access
    uint64_t getActiveTraceCount() const { return activeTraceCount_; }
    double getWorkingMemoryLoad() const { return workingMemoryLoad_; }
    double getMemorySpan() const { return memorySpan_; }
    uint64_t getChunkCount() const { return chunkCount_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    class MemorySystem* memory_ = nullptr;
    
    // Visualization state
    bool showActiveTraces_ = true;
    bool showTraceStrength_ = true;
    bool showTraceDecay_ = true;
    bool showInterference_ = true;
    bool showChunking_ = true;
    bool showCapacityLimits_ = true;
    
    // Working memory statistics
    uint64_t activeTraceCount_ = 0;
    double workingMemoryLoad_ = 0.0;
    double memorySpan_ = 0.0;
    uint64_t chunkCount_ = 0;
    uint32_t decayRate_ = 1;
    std::vector<double> traceStrengths_;
};

/**
 * @class EpisodicMemoryVisualizer
 * @brief Memory episodes and replay visualization
 * 
 * This visualizer provides visualization of episodic memory formation,
 * storage, retrieval, and replay dynamics.
 */
class EpisodicMemoryVisualizer : public VisualizationBase {
public:
    EpisodicMemoryVisualizer();
    ~EpisodicMemoryVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // EpisodicMemoryVisualizer specific methods
    void setShowEpisodes(bool show) { showEpisodes_ = show; }
    void setShowReplayTimeline(bool show) { showReplayTimeline_ = show; }
    void setShowMemoryIndexing(bool show) { showMemoryIndexing_ = show; }
    void setShowSensoryDetails(bool show) { showSensoryDetails_ = show; }
    void setShowTemporalSequence(bool show) { showTemporalSequence_ = show; }
    
    // Data access
    uint64_t getEpisodeCount() const { return episodeCount_; }
    uint64_t getReplayEvents() const { return replayEvents_; }
    double getMemoryStability() const { return memoryStability_; }
    uint32_t getReplayFrequency() const { return replayFrequency_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    class MemorySystem* memory_ = nullptr;
    
    // Visualization state
    bool showEpisodes_ = true;
    bool showReplayTimeline_ = true;
    bool showMemoryIndexing_ = true;
    bool showSensoryDetails_ = true;
    bool showTemporalSequence_ = true;
    
    // Episodic memory statistics
    uint64_t episodeCount_ = 0;
    uint64_t replayEvents_ = 0;
    double memoryStability_ = 0.0;
    uint32_t replayFrequency_ = 0;
    uint32_t episodeHistorySize_ = 100;
    std::vector<uint64_t> episodeTimestamps_;
    std::vector<double> episodeStrengths_;
};

/**
 * @class SemanticMemoryVisualizer
 * @brief Conceptual associations visualization
 * 
 * This visualizer provides visualization of semantic memory structure,
 * conceptual associations, spreading activation, and knowledge networks.
 */
class SemanticMemoryVisualizer : public VisualizationBase {
public:
    SemanticMemoryVisualizer();
    ~SemanticMemoryVisualizer() override = default;
    
    // Core visualization interface implementation
    void attachToBrain(class Brain* brain) override;
    void attachToMemory(class MemorySystem* memory) override;
    void attachToPlasticity(class PlasticitySystem* plasticity) override;
    
    // SemanticMemoryVisualizer specific methods
    void setShowConcepts(bool show) { showConcepts_ = show; }
    void setShowAssociations(bool show) { showAssociations_ = show; }
    void setShowSpreadingActivation(bool show) { showSpreadingActivation_ = show; }
    void setShowKnowledgeGraph(bool show) { showKnowledgeGraph_ = show; }
    void setShowSemanticDistance(bool show) { showSemanticDistance_ = show; }
    void setShowActivationThresholds(bool show) { showActivationThresholds_ = show; }
    
    // Data access
    uint64_t getConceptCount() const { return conceptCount_; }
    uint64_t getAssociationCount() const { return associationCount_; }
    double getAverageActivationLevel() const { return averageActivationLevel_; }
    double getSemanticCohesion() const { return semanticCohesion_; }
    uint32_t getActivationSpread() const { return activationSpread_; }
    
protected:
    bool initializeImpl(const VisualizationConfig& config) override;
    void updateImpl() override;
    void renderImpl() override;
    void closeImpl() override;
    void resetImpl() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attached systems
    class Brain* brain_ = nullptr;
    class MemorySystem* memory_ = nullptr;
    
    // Visualization state
    bool showConcepts_ = true;
    bool showAssociations_ = true;
    bool showSpreadingActivation_ = true;
    bool showKnowledgeGraph_ = true;
    bool showSemanticDistance_ = true;
    bool showActivationThresholds_ = true;
    
    // Semantic memory statistics
    uint64_t conceptCount_ = 0;
    uint64_t associationCount_ = 0;
    double averageActivationLevel_ = 0.0;
    double semanticCohesion_ = 0.0;
    uint32_t activationSpread_ = 0;
    double activationSpreadRadius_ = 5.0;
    uint32_t activationUpdateInterval_ = 10;
    uint32_t activationHistorySize_ = 100;
    std::vector<double> activationHistory_;
};

} // namespace nlm

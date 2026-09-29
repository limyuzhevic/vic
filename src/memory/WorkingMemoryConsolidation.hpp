#pragma once

#include "EnhancedWorkingMemory.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <chrono>
#include <unordered_map>

namespace nlm {

class WorkingMemoryConsolidation {
public:
    WorkingMemoryConsolidation() : consolidationInterval_(100), replayInterval_(1000),
                                  importanceDecayRate_(0.95f), strengthIncreaseRate_(1.05f) {}
    
    ~WorkingMemoryConsolidation() {}
    
    // Perform memory consolidation based on importance
    struct ConsolidationResult {
        size_t tracesConsolidated;
        size_t tracesReplayed;
        size_t tracesEvicted;
        float totalImportanceGained;
        std::chrono::nanoseconds consolidationTime;
    };
    
    ConsolidationResult consolidate(std::priority_queue<EnhancedMemoryTrace>& traces,
                                    const std::unordered_map<uint64_t, MemoryLevel>& neuronLevels,
                                    const std::unordered_map<uint64_t, float>& neuronImportances,
                                    size_t targetCapacity,
                                    SimulationStep currentStep) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        ConsolidationResult result;
        result.tracesConsolidated = 0;
        result.tracesReplayed = 0;
        result.tracesEvicted = 0;
        result.totalImportanceGained = 0.0f;
        
        // Extract all traces for processing
        std::vector<EnhancedMemoryTrace> allTraces;
        while (!traces.empty()) {
            allTraces.push_back(traces.top());
            traces.pop();
        }
        
        // Phase 1: Rehearse and strengthen important traces
        std::vector<EnhancedMemoryTrace> consolidatedTraces;
        consolidatedTraces.reserve(allTraces.size());
        
        for (auto& trace : allTraces) {
            // Update importance based on stability and recency
            updateTraceImportance(trace, neuronLevels, neuronImportances, currentStep);
            
            // Determine if trace should be promoted to stable memory
            if (shouldPromoteToStable(trace, neuronLevels)) {
                trace.isConsolidated = true;
                trace.isTransient = false;
                trace.stability = std::min(1.0f, trace.stability * 1.1f);
            }
            
            // Perform rehearsal if this is a replay cycle
            if (shouldReplayTrace(trace, currentStep)) {
                rehearseTrace(trace);
                result.tracesReplayed++;
                trace.importanceScore *= strengthIncreaseRate_;
                result.totalImportanceGained += (trace.importanceScore - 1.0f);
            }
            
            // Keep trace if within capacity
            if (consolidatedTraces.size() < targetCapacity) {
                consolidatedTraces.push_back(trace);
                result.tracesConsolidated++;
            } else {
                // Evict low-importance traces
                if (trace.importanceScore < 0.1f) {
                    result.tracesEvicted++;
                } else {
                    // Replace lowest importance trace
                    auto lowestIt = std::min_element(consolidatedTraces.begin(), consolidatedTraces.end(),
                                                     [](const EnhancedMemoryTrace& a, const EnhancedMemoryTrace& b) {
                                                         return a.importanceScore < b.importanceScore;
                                                     });
                    if (lowestIt->importanceScore < trace.importanceScore) {
                        *lowestIt = trace;
                        result.tracesEvicted++;
                    }
                }
            }
        }
        
        // Rebuild priority queue with consolidated traces
        for (const auto& trace : consolidatedTraces) {
            traces.push(trace);
        }
        
        // Decay importance scores over time
        decayImportanceScores(traces);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.consolidationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        
        return result;
    }
    
    // Schedule replay of important traces
    std::vector<EnhancedMemoryTrace> scheduleReplay(
        const std::priority_queue<EnhancedMemoryTrace>& traces,
        SimulationStep currentStep,
        size_t maxReplayCount = 10) const {
        
        std::vector<EnhancedMemoryTrace> replayQueue;
        
        // Extract all traces
        std::priority_queue<EnhancedMemoryTrace> temp = traces;
        std::vector<EnhancedMemoryTrace> allTraces;
        while (!temp.empty()) {
            allTraces.push_back(temp.top());
            temp.pop();
        }
        
        // Select traces for replay based on importance, stability, and recency
        std::vector<std::tuple<float, float, SimulationStep, EnhancedMemoryTrace>> scoredTraces;
        
        for (const auto& trace : allTraces) {
            // Compute replay score
            float importanceScore = trace.importanceScore;
            float stabilityScore = trace.stability;
            float recencyScore = 1.0f / (currentStep - trace.timestamp + 1);
            
            float replayScore = (importanceScore * 0.5f + stabilityScore * 0.3f + recencyScore * 0.2f);
            
            // Only replay if score is above threshold and trace is not already consolidated
            if (replayScore > 0.3f && !trace.isConsolidated) {
                scoredTraces.emplace_back(replayScore, stabilityScore, trace.timestamp, trace);
            }
        }
        
        // Sort by replay score (descending)
        std::sort(scoredTraces.begin(), scoredTraces.end(),
                  [](const auto& a, const auto& b) {
                      return std::get<0>(a) > std::get<0>(b);
                  });
        
        // Select top traces for replay
        size_t count = std::min(maxReplayCount, scoredTraces.size());
        replayQueue.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            replayQueue.push_back(std::get<3>(scoredTraces[i]));
        }
        
        return replayQueue;
    }
    
    // Strengthen a specific trace through replay
    void strengthenTrace(EnhancedMemoryTrace& trace, float factor = 1.2f) {
        trace.importanceScore = std::min(1.0f, trace.importanceScore * factor);
        trace.stability = std::min(1.0f, trace.stability * factor);
        
        // Increase activation level
        trace.activation = std::min(1.0f, trace.activation * factor);
    }
    
    // Decay importance scores across all traces
    void decayImportanceScores(std::priority_queue<EnhancedMemoryTrace>& traces) {
        std::vector<EnhancedMemoryTrace> updatedTraces;
        
        while (!traces.empty()) {
            EnhancedMemoryTrace trace = traces.top();
            traces.pop();
            
            // Decay importance
            trace.importanceScore *= importanceDecayRate_;
            
            // Update stability
            trace.stability *= importanceDecayRate_;
            
            updatedTraces.push_back(trace);
        }
        
        // Rebuild priority queue
        for (const auto& trace : updatedTraces) {
            traces.push(trace);
        }
    }
    
    // Check if a trace should be promoted to stable memory
    bool shouldPromoteToStable(const EnhancedMemoryTrace& trace,
                               const std::unordered_map<uint64_t, MemoryLevel>& neuronLevels) const {
        if (trace.isConsolidated) return true;
        
        auto levelIt = neuronLevels.find(trace.neuronId.value);
        if (levelIt == neuronLevels.end()) return false;
        
        MemoryLevel level = levelIt->second;
        
        // Promote based on level and importance
        if (level == MemoryLevel::EPISODIC || level == MemoryLevel::SEMANTIC) {
            return trace.importanceScore > 0.7f;
        } else if (level == MemoryLevel::WORKING) {
            return trace.importanceScore > 0.8f && trace.stability > 0.9f;
        }
        
        return false;
    }
    
    // Check if a trace should be replayed
    bool shouldReplayTrace(const EnhancedMemoryTrace& trace, SimulationStep currentStep) const {
        // Replay every replayInterval_ steps
        return (currentStep % replayInterval_) == 0 && 
               trace.importanceScore > 0.5f && 
               trace.stability > 0.5f;
    }
    
    // Rehearse a trace (strengthen through repeated activation)
    void rehearseTrace(EnhancedMemoryTrace& trace) {
        trace.activation = std::min(1.0f, trace.activation * 1.1f);
        trace.stability = std::min(1.0f, trace.stability * 1.05f);
    }
    
    // Update trace importance based on various factors
    void updateTraceImportance(EnhancedMemoryTrace& trace,
                               const std::unordered_map<uint64_t, MemoryLevel>& neuronLevels,
                               const std::unordered_map<uint64_t, float>& neuronImportances,
                               SimulationStep currentStep) {
        // Base importance from current score
        float importance = trace.importanceScore;
        
        // Add stability factor
        importance *= trace.stability;
        
        // Add recency factor (more recent traces are more important)
        float age = static_cast<float>(currentStep - trace.timestamp);
        float recencyFactor = std::max(0.1f, 1.0f - age / 1000.0f);
        importance *= recencyFactor;
        
        // Add memory level factor
        auto levelIt = neuronLevels.find(trace.neuronId.value);
        if (levelIt != neuronLevels.end()) {
            MemoryLevel level = levelIt->second;
            float levelFactor = 1.0f;
            switch (level) {
                case MemoryLevel::SENSORY:
                    levelFactor = 0.8f;
                    break;
                case MemoryLevel::WORKING:
                    levelFactor = 1.0f;
                    break;
                case MemoryLevel::EPISODIC:
                    levelFactor = 1.2f;
                    break;
                case MemoryLevel::SEMANTIC:
                    levelFactor = 1.5f;
                    break;
                case MemoryLevel::STRATEGIC:
                    levelFactor = 2.0f;
                    break;
            }
            importance *= levelFactor;
        }
        
        // Update the trace's importance
        trace.importanceScore = std::min(1.0f, importance);
    }
    
    // Get consolidation statistics
    struct ConsolidationStats {
        size_t totalTraces;
        size_t consolidatedTraces;
        size_t replayedTraces;
        size_t evictedTraces;
        float averageImportance;
        float averageStability;
        std::chrono::nanoseconds lastConsolidationTime;
    };
    
    ConsolidationStats getStats() const {
        ConsolidationStats stats;
        stats.totalTraces = totalTraces_;
        stats.consolidatedTraces = consolidatedTraces_;
        stats.replayedTraces = replayedTraces_;
        stats.evictedTraces = evictedTraces_;
        stats.averageImportance = averageImportance_;
        stats.averageStability = averageStability_;
        stats.lastConsolidationTime = lastConsolidationTime_;
        return stats;
    }
    
    void recordConsolidationEvent(size_t tracesConsolidated, size_t tracesReplayed,
                                   size_t tracesEvicted, float importanceGained,
                                   std::chrono::nanoseconds duration) {
        consolidatedTraces_ += tracesConsolidated;
        replayedTraces_ += tracesReplayed;
        evictedTraces_ += tracesEvicted;
        totalTraces_ += tracesConsolidated;
        
        // Update averages
        if (consolidationCount_ > 0) {
            averageImportance_ = (averageImportance_ * (consolidationCount_ - 1) + importanceGained) / consolidationCount_;
        } else {
            averageImportance_ = importanceGained;
        }
        
        averageStability_ = (averageStability_ * (consolidationCount_ - 1) + 1.0f) / consolidationCount_;
        lastConsolidationTime_ = duration;
        consolidationCount_++;
    }
    
private:
    // Consolidation statistics
    size_t totalTraces_;
    size_t consolidatedTraces_;
    size_t replayedTraces_;
    size_t evictedTraces_;
    float averageImportance_;
    float averageStability_;
    std::chrono::nanoseconds lastConsolidationTime_;
    size_t consolidationCount_;
    
    // Configuration
    size_t consolidationInterval_;
    size_t replayInterval_;
    float importanceDecayRate_;
    float strengthIncreaseRate_;
};

} // namespace nlm
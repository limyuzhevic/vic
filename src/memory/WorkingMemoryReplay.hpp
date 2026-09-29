#pragma once

#include "EnhancedWorkingMemory.hpp"
#include "../core/Types/Types.hpp"
#include <chrono>
#include <unordered_map>
#include <queue>

namespace nlm {

// Replay and rehearsal mechanisms for memory consolidation
class WorkingMemoryReplay {
public:
    WorkingMemoryReplay() : replayQueueSize_(10), replayInterval_(100),
                           rehearsalGain_(1.5f), consolidationGain_(2.0f) {}
    
    ~WorkingMemoryReplay() {}
    
    // Replay mechanism - reactivates memory traces
    struct ReplaySession {
        SimulationStep startStep;
        std::vector<EnhancedMemoryTrace> replayedTraces;
        float totalReplayStrength;
        std::chrono::nanoseconds replayDuration;
        std::vector<NeuronId> reactivatedNeurons;
    };
    
    // Initiate a replay session for important traces
    ReplaySession replay(EnhancedWorkingMemory& workingMemory,
                         SimulationStep currentStep,
                         size_t maxReplayCount = 10,
                         bool useReplayQueue = true) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        ReplaySession session;
        session.startStep = currentStep;
        session.totalReplayStrength = 0.0f;
        
        // Get traces for replay
        std::vector<EnhancedMemoryTrace> tracesToReplay;
        
        if (useReplayQueue && !replayQueue_.empty()) {
            tracesToReplay = replayQueue_;
            replayQueue_.clear();
        } else {
            tracesToReplay = workingMemory.getAllSorted();
        }
        
        // Select traces for replay based on priority
        std::vector<EnhancedMemoryTrace> selectedTraces;
        selectedTraces.reserve(std::min(maxReplayCount, tracesToReplay.size()));
        
        // Sort by replay priority (importance * stability * recency)
        std::sort(tracesToReplay.begin(), tracesToReplay.end(),
                  [currentStep](const EnhancedMemoryTrace& a, const EnhancedMemoryTrace& b) {
                      float priorityA = a.importanceScore * a.stability * (1.0f / (currentStep - a.timestamp + 1));
                      float priorityB = b.importanceScore * b.stability * (1.0f / (currentStep - b.timestamp + 1));
                      return priorityA > priorityB;
                  });
        
        // Select top traces
        size_t count = std::min(maxReplayCount, tracesToReplay.size());
        for (size_t i = 0; i < count; ++i) {
            selectedTraces.push_back(tracesToReplay[i]);
        }
        
        // Perform replay
        for (auto& trace : selectedTraces) {
            replayTrace(workingMemory, trace, currentStep);
            session.replayedTraces.push_back(trace);
            session.totalReplayStrength += trace.importanceScore * consolidationGain_;
            
            // Track reactivated neurons
            session.reactivatedNeurons.push_back(trace.neuronId);
        }
        
        // Schedule next replay
        scheduleNextReplay(currentStep + replayInterval_);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        session.replayDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        
        return session;
    }
    
    // Rehearse a trace (strengthen through repeated activation)
    void rehearseTrace(EnhancedWorkingMemory& workingMemory,
                       const EnhancedMemoryTrace& trace,
                       SimulationStep currentStep,
                       float gain = 1.0f) {
        // Reconstruct trace from working memory
        if (workingMemory.contains(trace.neuronId)) {
            float currentActivation = workingMemory.getNeuronActivation(trace.neuronId);
            float newActivation = currentActivation * (1.0f + gain * rehearsalGain_);
            
            // Update trace
            workingMemory.storeToNeuron(trace.neuronId, newActivation);
            
            // Update trace importance
            EnhancedMemoryTrace& enhancedTrace = const_cast<EnhancedMemoryTrace&>(trace);
            enhancedTrace.importanceScore = std::min(1.0f, enhancedTrace.importanceScore * (1.0f + gain * 0.1f));
        }
    }
    
    // Reconstruct trace from multiple traces
    std::vector<float> reconstructTrace(const std::vector<EnhancedMemoryTrace>& traces) const {
        if (traces.empty()) return std::vector<float>();
        
        size_t patternSize = traces[0].neuronId.index() > 1000 ? 100 : 50;  // Estimate pattern size
        std::vector<float> reconstructed(patternSize, 0.0f);
        
        float totalWeight = 0.0f;
        
        for (const auto& trace : traces) {
            float weight = trace.importanceScore * trace.stability;
            totalWeight += weight;
            
            // Distribute activation across neurons (simplified)
            size_t neuronIndex = trace.neuronId.index() % patternSize;
            if (neuronIndex < reconstructed.size()) {
                reconstructed[neuronIndex] += trace.activation * weight;
            }
        }
        
        // Normalize
        if (totalWeight > 0.0f) {
            for (size_t i = 0; i < reconstructed.size(); ++i) {
                reconstructed[i] /= totalWeight;
            }
        }
        
        return reconstructed;
    }
    
    // Consolidate traces through replay
    void consolidateThroughReplay(EnhancedWorkingMemory& workingMemory,
                                  SimulationStep currentStep,
                                  float consolidationThreshold = 0.7f) {
        // Get all traces
        std::vector<EnhancedMemoryTrace> allTraces = workingMemory.getAllSorted();
        
        // Group traces by similarity
        std::vector<std::vector<EnhancedMemoryTrace>> traceGroups;
        
        for (const auto& trace : allTraces) {
            bool foundGroup = false;
            
            for (auto& group : traceGroups) {
                if (!group.empty() && areSimilar(group[0], trace)) {
                    group.push_back(trace);
                    foundGroup = true;
                    break;
                }
            }
            
            if (!foundGroup) {
                std::vector<EnhancedMemoryTrace> newGroup;
                newGroup.push_back(trace);
                traceGroups.push_back(newGroup);
            }
        }
        
        // Replay each group as a consolidated trace
        for (auto& group : traceGroups) {
            if (group.size() >= 2) {  // Only consolidate groups with multiple traces
                std::vector<float> reconstructed = reconstructTrace(group);
                
                // Store consolidated trace
                if (!reconstructed.empty()) {
                    workingMemory.store(reconstructed, consolidationGain_, false);  // Not transient
                }
                
                // Strengthen individual traces
                for (auto& trace : group) {
                    rehearseTrace(workingMemory, trace, currentStep, 0.5f);
                }
            }
        }
    }
    
    // Check if two traces are similar
    bool areSimilar(const EnhancedMemoryTrace& trace1, const EnhancedMemoryTrace& trace2) const {
        // Similarity based on neuron proximity, importance overlap, and stability
        float neuronSimilarity = 1.0f / (1.0f + std::abs(static_cast<int>(trace1.neuronId.value - trace2.neuronId.value)));
        float importanceSimilarity = 1.0f - std::abs(trace1.importanceScore - trace2.importanceScore);
        float stabilitySimilarity = 1.0f - std::abs(trace1.stability - trace2.stability);
        
        return (neuronSimilarity + importanceSimilarity + stabilitySimilarity) / 3.0f > 0.6f;
    }
    
    // Schedule a trace for replay
    void scheduleForReplay(const EnhancedMemoryTrace& trace) {
        replayQueue_.push_back(trace);
        
        // Maintain queue size
        if (replayQueue_.size() > replayQueueSize_) {
            replayQueue_.erase(replayQueue_.begin());
        }
    }
    
    // Clear the replay queue
    void clearReplayQueue() {
        replayQueue_.clear();
    }
    
    // Schedule next replay
    void scheduleNextReplay(SimulationStep step) {
        nextReplayStep_ = step;
    }
    
    // Check if replay should occur at this step
    bool shouldReplay(SimulationStep currentStep) const {
        return currentStep >= nextReplayStep_;
    }
    
    // Get current replay queue
    const std::vector<EnhancedMemoryTrace>& getReplayQueue() const {
        return replayQueue_;
    }
    
    // Get replay statistics
    struct ReplayStats {
        size_t totalReplaySessions;
        size_t tracesReplayed;
        float averageReplayStrength;
        size_t queueSize;
        float replayEfficiency;
        std::chrono::nanoseconds totalReplayTime;
    };
    
    ReplayStats getStats() const {
        ReplayStats stats;
        stats.totalReplaySessions = totalReplaySessions_;
        stats.tracesReplayed = tracesReplayed_;
        stats.averageReplayStrength = averageReplayStrength_;
        stats.queueSize = replayQueue_.size();
        stats.replayEfficiency = tracesReplayed_ / (totalReplaySessions_ * 10);  // Approximate
        stats.totalReplayTime = totalReplayTime_;
        return stats;
    }
    
    void recordReplaySession(size_t tracesReplayed, float totalStrength,
                             std::chrono::nanoseconds duration) {
        totalReplaySessions_++;
        tracesReplayed_ += tracesReplayed;
        
        // Update average
        if (totalReplaySessions_ > 0) {
            averageReplayStrength_ = (averageReplayStrength_ * (totalReplaySessions_ - 1) + totalStrength) / totalReplaySessions_;
        }
        
        totalReplayTime_ += duration;
    }

private:
    // Replay queue
    std::vector<EnhancedMemoryTrace> replayQueue_;
    
    // Replay scheduling
    SimulationStep nextReplayStep_;
    
    // Statistics
    size_t totalReplaySessions_;
    size_t tracesReplayed_;
    float averageReplayStrength_;
    std::chrono::nanoseconds totalReplayTime_;
    
    // Configuration
    size_t replayQueueSize_;
    size_t replayInterval_;
    float rehearsalGain_;
    float consolidationGain_;
};

} // namespace nlm
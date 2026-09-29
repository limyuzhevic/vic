#pragma once

#include "EnhancedWorkingMemory.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <chrono>

namespace nlm {

class WorkingMemoryPriorityQueue {
public:
    WorkingMemoryPriorityQueue() : lastEvictionTime_(0), evictionRate_(0.1f) {}
    
    ~WorkingMemoryPriorityQueue() {}
    
    // Add a memory trace to the queue
    void push(const EnhancedMemoryTrace& trace) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        activeTraces_.push(trace);
        
        // Check if we need to evict low-importance traces
        consolidateLowImportanceTraces();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    }
    
    // Get the highest priority trace (front of priority queue)
    EnhancedMemoryTrace top() const {
        return activeTraces_.top();
    }
    
    // Remove the highest priority trace
    void pop() {
        if (!activeTraces_.empty()) {
            activeTraces_.pop();
        }
    }
    
    // Check if the queue is empty
    bool empty() const {
        return activeTraces_.empty();
    }
    
    // Get the size of the queue
    size_t size() const {
        return activeTraces_.size();
    }
    
    // Get all traces sorted by importance
    std::vector<EnhancedMemoryTrace> getAllSorted() const {
        std::vector<EnhancedMemoryTrace> result;
        result.reserve(activeTraces_.size());
        
        // Create a copy of the priority queue and extract elements
        std::priority_queue<EnhancedMemoryTrace> temp = activeTraces_;
        while (!temp.empty()) {
            result.push_back(temp.top());
            temp.pop();
        }
        
        // Sort by importance (descending)
        std::sort(result.begin(), result.end(),
                  [](const EnhancedMemoryTrace& a, const EnhancedMemoryTrace& b) {
                      return a.importanceScore > b.importanceScore;
                  });
        
        return result;
    }
    
    // Update importance scores for all traces
    void updateAllImportanceScores(const std::function<float(const EnhancedMemoryTrace&)>& scoreFunc) {
        std::vector<EnhancedMemoryTrace> updatedTraces;
        updatedTraces.reserve(activeTraces_.size());
        
        // Extract all traces
        while (!activeTraces_.empty()) {
            updatedTraces.push_back(activeTraces_.top());
            activeTraces_.pop();
        }
        
        // Update importance scores
        for (auto& trace : updatedTraces) {
            trace.importanceScore = scoreFunc(trace);
        }
        
        // Rebuild priority queue
        for (const auto& trace : updatedTraces) {
            activeTraces_.push(trace);
        }
    }
    
    // Get traces above a certain importance threshold
    std::vector<EnhancedMemoryTrace> getAboveImportance(float threshold) const {
        std::vector<EnhancedMemoryTrace> result;
        
        // Create a copy and extract elements
        std::priority_queue<EnhancedMemoryTrace> temp = activeTraces_;
        while (!temp.empty()) {
            const auto& trace = temp.top();
            if (trace.importanceScore >= threshold) {
                result.push_back(trace);
            }
            temp.pop();
        }
        
        return result;
    }
    
    // Get traces for replay based on recency and importance
    std::vector<EnhancedMemoryTrace> getForReplay(size_t count, float importanceThreshold = 0.5f) const {
        std::vector<EnhancedMemoryTrace> result;
        
        // Get all traces above importance threshold
        std::vector<EnhancedMemoryTrace> candidates = getAboveImportance(importanceThreshold);
        
        if (candidates.size() <= count) {
            return candidates;
        }
        
        // Sort by combination of importance and recency
        std::sort(candidates.begin(), candidates.end(),
                  [](const EnhancedMemoryTrace& a, const EnhancedMemoryTrace& b) {
                      // Higher importance gets priority
                      if (a.importanceScore != b.importanceScore) {
                          return a.importanceScore > b.importanceScore;
                      }
                      // Then by recency (more recent = higher priority)
                      return a.timestamp > b.timestamp;
                  });
        
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(candidates[i]);
        }
        
        return result;
    }
    
    // Clean up expired traces (based on timestamp)
    void cleanupExpiredTraces(SimulationStep currentStep, SimulationStep maxAge) {
        std::vector<EnhancedMemoryTrace> validTraces;
        validTraces.reserve(activeTraces_.size());
        
        // Create a copy and filter out expired traces
        std::priority_queue<EnhancedMemoryTrace> temp = activeTraces_;
        while (!temp.empty()) {
            const auto& trace = temp.top();
            if (currentStep - trace.timestamp <= maxAge) {
                validTraces.push_back(trace);
            }
            temp.pop();
        }
        
        // Rebuild priority queue
        activeTraces_ = std::priority_queue<EnhancedMemoryTrace>(validTraces.begin(), validTraces.end());
    }
    
    // Get memory statistics
    struct QueueStats {
        size_t totalTraces;
        size_t evictionCount;
        float averageImportance;
        float importanceVariance;
        std::chrono::nanoseconds averageOperationTime;
    };
    
    QueueStats getStats() const {
        QueueStats stats;
        stats.totalTraces = activeTraces_.size();
        stats.evictionCount = evictionCount_;
        
        if (!activeTraces_.empty()) {
            std::vector<EnhancedMemoryTrace> allTraces = getAllSorted();
            float sum = 0.0f;
            float sumSq = 0.0f;
            
            for (const auto& trace : allTraces) {
                sum += trace.importanceScore;
                sumSq += trace.importanceScore * trace.importanceScore;
            }
            
            stats.averageImportance = sum / allTraces.size();
            stats.importanceVariance = (sumSq / allTraces.size()) - (stats.averageImportance * stats.averageImportance);
        } else {
            stats.averageImportance = 0.0f;
            stats.importanceVariance = 0.0f;
        }
        
        return stats;
    }
    
    // Get access to the underlying priority queue for consolidation
    std::priority_queue<EnhancedMemoryTrace>& getPriorityQueue() {
        return activeTraces_;
    }
    
    // Get const access to the underlying priority queue
    const std::priority_queue<EnhancedMemoryTrace>& getPriorityQueue() const {
        return activeTraces_;
    }
    
private:
    // Consolidated traces with low importance
    void consolidateLowImportanceTraces() {
        auto now = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
        
        // If we haven't evicted recently, consider eviction
        if (now - lastEvictionTime_ > 100) {  // Every 100 seconds
            auto traces = getAllSorted();
            size_t toRemove = static_cast<size_t>(static_cast<float>(activeTraces_.size()) * evictionRate_);
            
            if (toRemove > 0 && traces.size() > toRemove) {
                // Keep the top N traces, remove the rest
                std::vector<EnhancedMemoryTrace> kept;
                kept.reserve(traces.size() - toRemove);
                for (size_t i = 0; i < traces.size() - toRemove; ++i) {
                    kept.push_back(traces[i]);
                }
                
                // Rebuild priority queue
                activeTraces_ = std::priority_queue<EnhancedMemoryTrace>(kept.begin(), kept.end());
                evictionCount_ += toRemove;
                lastEvictionTime_ = now;
            }
        }
    }
    
    // Priority queue storing traces sorted by importance
    std::priority_queue<EnhancedMemoryTrace> activeTraces_;
    
    // Statistics
    size_t evictionCount_;
    int64_t lastEvictionTime_;
    float evictionRate_;
};

} // namespace nlm
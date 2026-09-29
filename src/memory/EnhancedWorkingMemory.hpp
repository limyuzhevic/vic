#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <queue>
#include <set>
#include <chrono>

namespace nlm {

// Forward declarations
class WorkingMemoryPriorityQueue;
class WorkingMemoryPatternMatcher;
class WorkingMemoryConsolidation;

// Enhanced Memory Trace structure with importance tracking
struct EnhancedMemoryTrace {
    NeuronId neuronId;
    float activation;
    SimulationStep timestamp;
    float importanceScore;
    float stability;
    bool isTransient;
    bool isConsolidated;
    
    EnhancedMemoryTrace(NeuronId id, float act, SimulationStep ts)
        : neuronId(id), activation(act), timestamp(ts), importanceScore(1.0f),
          stability(1.0f), isTransient(true), isConsolidated(false) {}
    
    // For priority queue comparison (higher importance first)
    bool operator<(const EnhancedMemoryTrace& other) const {
        return importanceScore < other.importanceScore;
    }
};

// Hierarchical memory organization levels
enum class MemoryLevel {
    SENSORY,    // Raw sensory input
    WORKING,    // Active working memory
    EPISODIC,   // Episodic memory traces
    SEMANTIC,   // Semantic knowledge
    STRATEGIC   // High-level strategies
};

// Meta-memory for resource management
class MetaMemoryManager {
public:
    MetaMemoryManager() : totalCapacity_(1000000), usedCapacity_(0), consolidationThreshold_(0.8f) {}
    
    // Allocate capacity for memory operations
    bool allocateCapacity(size_t bytes) {
        if (usedCapacity_ + bytes <= totalCapacity_) {
            usedCapacity_ += bytes;
            return true;
        }
        return false;
    }
    
    // Release allocated capacity
    void releaseCapacity(size_t bytes) {
        if (bytes <= usedCapacity_) {
            usedCapacity_ -= bytes;
        }
    }
    
    // Check if consolidation is needed
    bool needsConsolidation() const {
        return (float)usedCapacity_ / totalCapacity_ > consolidationThreshold_;
    }
    
    // Get current usage statistics
    float getUsageRatio() const {
        return (float)usedCapacity_ / totalCapacity_;
    }
    
    // Get free capacity
    size_t getFreeCapacity() const {
        return totalCapacity_ - usedCapacity_;
    }
    
private:
    size_t totalCapacity_;
    size_t usedCapacity_;
    float consolidationThreshold_;
};

// Enhanced Working Memory interface - replaces NeuralWorkingMemory
class EnhancedWorkingMemory {
public:
    EnhancedWorkingMemory();
    ~EnhancedWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store information with importance scoring
    void store(const std::vector<float>& pattern, float strength = 1.0f, bool isTransient = true);
    
    // Store specific neuron activation with automatic index tracking
    void storeToNeuron(NeuronId neuron, float activation);
    
    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;
    
    // Check if specific neuron is part of working memory (O(1) lookup)
    bool contains(NeuronId neuron) const;
    
    // Get activation level of a specific neuron (O(1) lookup)
    float getNeuronActivation(NeuronId neuron) const;
    
    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);
    
    // Clear working memory
    void clear();
    
    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }
    
    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);
    
    // Enhanced competition with priority-based selection
    void runCompetition();
    
    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;
    
    // Get overall memory activity level
    float getMemoryActivity() const;
    
    // Get importance score for a neuron
    float getNeuronImportance(NeuronId neuron) const;
    
    // Get memory level for a neuron
    MemoryLevel getMemoryLevel(NeuronId neuron) const;
    
    // Promote neuron from transient to stable memory
    void promoteToStable(NeuronId neuron);
    
    // Demote neuron from stable to transient memory
    void demoteToTransient(NeuronId neuron);
    
    // Perform memory consolidation
    void consolidateMemory();
    
    // Get memory statistics
    struct MemoryStats {
        size_t totalTraces;
        size_t transientTraces;
        size_t stableTraces;
        float averageImportance;
        float consolidationRatio;
    };
    
    MemoryStats getMemoryStats() const;
    
    // Get meta-memory usage statistics
    float getMetaMemoryUsage() const { return metaMemoryManager_.getUsageRatio(); }
    
private:
    // Enhanced implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core enhanced data structures
    // Priority queue for active traces (O(log n) operations)
    std::priority_queue<EnhancedMemoryTrace> activeTraces_;
    
    // Hash maps for O(1) neuron lookup
    std::unordered_map<uint64_t, float> neuronActivations_;  // Key: NeuronId.value, Value: activation
    std::unordered_map<uint64_t, SimulationStep> neuronTimestamps_;  // Key: NeuronId.value, Value: timestamp
    std::unordered_map<uint64_t, float> neuronImportances_;  // Key: NeuronId.value, Value: importance
    std::unordered_map<uint64_t, MemoryLevel> neuronLevels_;  // Key: NeuronId.value, Value: MemoryLevel
    
    // Ordered set for efficient iteration and removal
    std::set<NeuronId> orderedNeurons_;  // Sorted by activation
    
    // Sparse representation for efficiency
    std::unordered_map<size_t, std::vector<float>> sparseActivations_;  // Block-based storage
    
    // Legacy vector for compatibility
    std::vector<NeuronId> memoryNeurons_;
    
    // Brain and configuration
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Meta-memory manager
    MetaMemoryManager metaMemoryManager_;
    
    // Competition and winners tracking
    std::vector<NeuronId> winners_;
    
    // Development system integration
    size_t replayCount_;
    
    // Interface to other components
    WorkingMemoryPriorityQueue* priorityQueue_;
    WorkingMemoryPatternMatcher* patternMatcher_;
    WorkingMemoryConsolidation* consolidationSystem_;
};

// Performance monitoring and optimization
class WorkingMemoryPerformanceMonitor {
public:
    struct PerformanceMetrics {
        size_t storageOperations;
        size_t lookupOperations;
        size_t competitionOperations;
        size_t consolidationOperations;
        size_t replayOperations;
        double averageStorageTime;
        double averageLookupTime;
        double averageCompetitionTime;
        double memoryUsageMB;
        size_t evictionCount;
    };
    
    WorkingMemoryPerformanceMonitor() {
        metrics_.storageOperations = 0;
        metrics_.lookupOperations = 0;
        metrics_.competitionOperations = 0;
        metrics_.consolidationOperations = 0;
        metrics_.replayOperations = 0;
        metrics_.averageStorageTime = 0.0;
        metrics_.averageLookupTime = 0.0;
        metrics_.averageCompetitionTime = 0.0;
        metrics_.memoryUsageMB = 0.0;
        metrics_.evictionCount = 0;
    }
    
    void recordStorageOperation(double timeNs) {
        metrics_.storageOperations++;
        metrics_.averageStorageTime = (metrics_.averageStorageTime * (metrics_.storageOperations - 1) + timeNs) / metrics_.storageOperations;
    }
    
    void recordLookupOperation(double timeNs) {
        metrics_.lookupOperations++;
        metrics_.averageLookupTime = (metrics_.averageLookupTime * (metrics_.lookupOperations - 1) + timeNs) / metrics_.lookupOperations;
    }
    
    void recordCompetitionOperation(double timeNs) {
        metrics_.competitionOperations++;
        metrics_.averageCompetitionTime = (metrics_.averageCompetitionTime * (metrics_.competitionOperations - 1) + timeNs) / metrics_.competitionOperations;
    }
    
    void recordConsolidationOperation() {
        metrics_.consolidationOperations++;
    }
    
    void recordReplayOperation() {
        metrics_.replayOperations++;
    }
    
    void recordEviction() {
        metrics_.evictionCount++;
    }
    
    const PerformanceMetrics& getMetrics() const { return metrics_; }
    
    void reset() {
        metrics_ = PerformanceMetrics();
    }
    
private:
    PerformanceMetrics metrics_;
};

} // namespace nlm
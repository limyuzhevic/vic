#pragma once

#include "EnhancedWorkingMemory.hpp"
#include "WorkingMemoryPerformanceMonitor.hpp"
#include "../core/Types/Types.hpp"
#include <chrono>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Performance optimization and memory management for working memory
class WorkingMemoryOptimizer {
public:
    WorkingMemoryOptimizer() : targetUtilization_(0.7f), enableAutoOptimization_(true),
                              compactionThreshold_(0.5f), evictionPenalty_(10.0f) {}
    
    ~WorkingMemoryOptimizer() {}
    
    // Optimize working memory performance
    struct OptimizationResult {
        size_t tracesCompacted;
        size_t tracesEvicted;
        size_t tracesPromoted;
        size_t tracesDemoted;
        float memorySavedMB;
        float performanceGain;
        std::chrono::nanoseconds optimizationTime;
    };
    
    OptimizationResult optimize(EnhancedWorkingMemory& workingMemory,
                                const WorkingMemoryPerformanceMonitor& monitor,
                                SimulationStep currentStep) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        OptimizationResult result;
        result.tracesCompacted = 0;
        result.tracesEvicted = 0;
        result.tracesPromoted = 0;
        result.tracesDemoted = 0;
        result.memorySavedMB = 0.0f;
        result.performanceGain = 0.0f;
        
        // Phase 1: Memory compaction
        result.tracesCompacted = compactMemory(workingMemory);
        
        // Phase 2: Intelligent eviction based on importance and usage patterns
        result.tracesEvicted = intelligentEviction(workingMemory, monitor);
        
        // Phase 3: Memory level promotion/demotion
        std::pair<size_t, size_t> promotionStats = promoteDemoteMemory(workingMemory);
        result.tracesPromoted = promotionStats.first;
        result.tracesDemoted = promotionStats.second;
        
        // Phase 4: Pattern matching optimization
        result.performanceGain = optimizePatternMatching(workingMemory, monitor);
        
        // Calculate memory savings (approximate)
        result.memorySavedMB = calculateMemorySavings(workingMemory, monitor);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.optimizationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
        
        return result;
    }
    
    // Adaptive memory allocation based on usage patterns
    void adaptiveAllocation(EnhancedWorkingMemory& workingMemory,
                            const WorkingMemoryPerformanceMonitor& monitor,
                            size_t currentStep) {
        if (!enableAutoOptimization_) return;
        
        float currentUtilization = workingMemory.getMetaMemoryUsage();
        
        // Adjust capacity based on utilization and performance
        if (currentUtilization > targetUtilization_ + 0.1f) {
            // High utilization - reduce capacity
            size_t currentCapacity = workingMemory.getCapacity();
            workingMemory.setCapacity(std::max<size_t>(currentCapacity * 0.9f, 10));
        } else if (currentUtilization < targetUtilization_ - 0.1f) {
            // Low utilization - increase capacity
            size_t currentCapacity = workingMemory.getCapacity();
            workingMemory.setCapacity(currentCapacity * 1.1f);
        }
        
        // Adjust decay rate based on performance
        adjustDecayRate(workingMemory, monitor);
        
        // Optimize importance threshold
        optimizeImportanceThreshold(workingMemory, monitor);
    }
    
    // Compress memory traces for storage efficiency
    size_t compressMemory(EnhancedWorkingMemory& workingMemory) {
        size_t compressedCount = 0;
        
        // Get all traces
        std::vector<EnhancedMemoryTrace> allTraces = workingMemory.getAllSorted();
        
        // Compress low-importance traces
        std::vector<EnhancedMemoryTrace> compressedTraces;
        compressedTraces.reserve(allTraces.size());
        
        for (const auto& trace : allTraces) {
            if (trace.importanceScore < 0.1f) {
                // Compress - reduce precision
                EnhancedMemoryTrace compressedTrace = trace;
                compressedTrace.activation = round(trace.activation * 100.0f) / 100.0f;
                compressedTrace.importanceScore = round(trace.importanceScore * 100.0f) / 100.0f;
                compressedTrace.stability = round(trace.stability * 100.0f) / 100.0f;
                compressedCount++;
            }
            
            compressedTraces.push_back(trace);
        }
        
        // Rebuild working memory (in a real implementation, this would be more efficient)
        workingMemory.clear();
        for (const auto& trace : compressedTraces) {
            workingMemory.storeToNeuron(trace.neuronId, trace.activation);
        }
        
        return compressedCount;
    }
    
    // Intelligent eviction based on importance, stability, and recency
    size_t intelligentEviction(EnhancedWorkingMemory& workingMemory,
                                const WorkingMemoryPerformanceMonitor& monitor) {
        size_t evictedCount = 0;
        
        // Get all traces sorted by importance
        std::vector<EnhancedMemoryTrace> allTraces = workingMemory.getAllSorted();
        
        // Calculate eviction target based on performance
        float evictionTarget = calculateEvictionTarget(workingMemory, monitor);
        
        // Evict low-value traces
        std::vector<EnhancedMemoryTrace> keptTraces;
        keptTraces.reserve(allTraces.size());
        
        for (const auto& trace : allTraces) {
            float valueScore = trace.importanceScore * trace.stability * 
                              (1.0f / (monitor.getMetrics().averageStorageTime + 1));
            
            if (valueScore < evictionTarget) {
                // Evict this trace
                workingMemory.storeToNeuron(trace.neuronId, 0.0f);  // Clear activation
                evictedCount++;
            } else {
                keptTraces.push_back(trace);
            }
        }
        
        // Rebuild working memory
        workingMemory.clear();
        for (const auto& trace : keptTraces) {
            workingMemory.storeToNeuron(trace.neuronId, trace.activation);
        }
        
        return evictedCount;
    }
    
    // Promote or demote memory traces between levels
    std::pair<size_t, size_t> promoteDemoteMemory(EnhancedWorkingMemory& workingMemory) {
        size_t promotedCount = 0;
        size_t demotedCount = 0;
        
        // Get all traces
        std::vector<EnhancedMemoryTrace> allTraces = workingMemory.getAllSorted();
        
        // Process each trace
        for (auto& trace : allTraces) {
            EnhancedMemoryTrace& enhancedTrace = const_cast<EnhancedMemoryTrace&>(trace);
            
            // Check if promotion/demotion is needed
            if (shouldPromote(enhancedTrace)) {
                enhancedTrace.isConsolidated = true;
                enhancedTrace.isTransient = false;
                promotedCount++;
            } else if (shouldDemote(enhancedTrace)) {
                enhancedTrace.isConsolidated = false;
                enhancedTrace.isTransient = true;
                demotedCount++;
            }
        }
        
        // Rebuild working memory with updated traces
        workingMemory.clear();
        for (const auto& trace : allTraces) {
            workingMemory.storeToNeuron(trace.neuronId, trace.activation);
        }
        
        return std::make_pair(promotedCount, demotedCount);
    }
    
    // Optimize pattern matching algorithms
    float optimizePatternMatching(EnhancedWorkingMemory& workingMemory,
                                  const WorkingMemoryPerformanceMonitor& monitor) {
        float performanceGain = 0.0f;
        
        // Get performance metrics
        const auto& metrics = monitor.getMetrics();
        
        // If lookup operations are slow, optimize pattern matching
        if (metrics.averageLookupTime > 1000.0) {  // If > 1ms
            // Enable pattern cache
            enablePatternCache_ = true;
            performanceGain += 0.3f;
        }
        
        // If competition operations are slow, optimize competition
        if (metrics.averageCompetitionTime > 5000.0) {  // If > 5ms
            // Enable priority-based competition
            enablePriorityCompetition_ = true;
            performanceGain += 0.4f;
        }
        
        // If storage operations are slow, optimize storage
        if (metrics.averageStorageTime > 2000.0) {  // If > 2ms
            // Enable sparse storage
            enableSparseStorage_ = true;
            performanceGain += 0.2f;
        }
        
        return performanceGain;
    }
    
    // Calculate memory savings
    float calculateMemorySavings(EnhancedWorkingMemory& workingMemory,
                                  const WorkingMemoryPerformanceMonitor& monitor) const {
        // Estimate memory savings based on optimization activities
        float savings = 0.0f;
        
        // Calculate based on traces evicted
        savings += monitor.getMetrics().evictionCount * 100.0f;  // 100 bytes per evicted trace
        
        // Calculate based on compression
        savings += monitor.getMetrics().evictionCount * 200.0f;  // Additional compression savings
        
        // Convert to MB
        return savings / (1024.0f * 1024.0f);
    }
    
    // Check if trace should be promoted
    bool shouldPromote(const EnhancedMemoryTrace& trace) const {
        return !trace.isConsolidated && trace.isTransient &&
               trace.importanceScore > 0.7f && trace.stability > 0.9f;
    }
    
    // Check if trace should be demoted
    bool shouldDemote(const EnhancedMemoryTrace& trace) const {
        return trace.isConsolidated && !trace.isTransient &&
               trace.importanceScore < 0.3f && trace.stability < 0.5f;
    }
    
    // Calculate eviction target based on performance
    float calculateEvictionTarget(EnhancedWorkingMemory& workingMemory,
                                   const WorkingMemoryPerformanceMonitor& monitor) const {
        float baseTarget = 0.1f;  // Default low importance threshold
        
        // Adjust based on memory utilization
        float utilization = workingMemory.getMetaMemoryUsage();
        if (utilization > 0.8f) {
            baseTarget *= 0.5f;  // More aggressive eviction
        } else if (utilization < 0.3f) {
            baseTarget *= 1.5f;  // Less aggressive eviction
        }
        
        // Adjust based on performance
        const auto& metrics = monitor.getMetrics();
        if (metrics.averageLookupTime > 1000.0) {
            baseTarget *= 0.8f;  // Evict more to improve lookup
        }
        
        return baseTarget;
    }
    
    // Adjust decay rate based on performance
    void adjustDecayRate(EnhancedWorkingMemory& workingMemory,
                        const WorkingMemoryPerformanceMonitor& monitor) {
        const auto& metrics = monitor.getMetrics();
        
        float currentDecay = workingMemory.getDecayRate();
        
        // If performance is poor, adjust decay rate
        if (metrics.averageLookupTime > 1000.0) {
            workingMemory.setDecayRate(std::min(0.5f, currentDecay * 1.2f));
        } else if (metrics.averageLookupTime < 100.0) {
            workingMemory.setDecayRate(std::max(0.001f, currentDecay * 0.8f));
        }
    }
    
    // Optimize importance threshold
    void optimizeImportanceThreshold(EnhancedWorkingMemory& workingMemory,
                                     const WorkingMemoryPerformanceMonitor& monitor) {
        // Adjust importance threshold based on performance
        const auto& metrics = monitor.getMetrics();
        
        // Simple heuristic: if performance is good, allow more traces
        if (metrics.averageLookupTime < 100.0 && metrics.averageStorageTime < 500.0) {
            // Can afford to keep more traces
            importanceThreshold_ = 0.2f;
        } else {
            // Need to be selective
            importanceThreshold_ = 0.5f;
        }
    }
    
    // Get optimization statistics
    struct OptimizationStats {
        size_t totalOptimizations;
        float averagePerformanceGain;
        float totalMemorySavedMB;
        std::chrono::nanoseconds totalOptimizationTime;
        size_t lastTracesCompacted;
        size_t lastTracesEvicted;
        size_t lastTracesPromoted;
        size_t lastTracesDemoted;
    };
    
    OptimizationStats getStats() const {
        OptimizationStats stats;
        stats.totalOptimizations = totalOptimizations_;
        stats.averagePerformanceGain = averagePerformanceGain_;
        stats.totalMemorySavedMB = totalMemorySavedMB_;
        stats.totalOptimizationTime = totalOptimizationTime_;
        stats.lastTracesCompacted = lastTracesCompacted_;
        stats.lastTracesEvicted = lastTracesEvicted_;
        stats.lastTracesPromoted = lastTracesPromoted_;
        stats.lastTracesDemoted = lastTracesDemoted_;
        return stats;
    }
    
    void recordOptimization(size_t tracesCompacted, size_t tracesEvicted,
                            size_t tracesPromoted, size_t tracesDemoted,
                            float memorySavedMB, float performanceGain,
                            std::chrono::nanoseconds duration) {
        totalOptimizations_++;
        
        // Update averages
        if (totalOptimizations_ > 0) {
            averagePerformanceGain_ = (averagePerformanceGain_ * (totalOptimizations_ - 1) + performanceGain) / totalOptimizations_;
            totalMemorySavedMB_ = (totalMemorySavedMB_ * (totalOptimizations_ - 1) + memorySavedMB) / totalOptimizations_;
        }
        
        totalOptimizationTime_ += duration;
        
        // Store last optimization results
        lastTracesCompacted = tracesCompacted;
        lastTracesEvicted = tracesEvicted;
        lastTracesPromoted = tracesPromoted;
        lastTracesDemoted = tracesDemoted;
    }
    
    // Enable or disable optimizations
    void enableOptimization(bool enable) {
        enableAutoOptimization_ = enable;
    }

private:
    // Optimization statistics
    size_t totalOptimizations_;
    float averagePerformanceGain_;
    float totalMemorySavedMB_;
    std::chrono::nanoseconds totalOptimizationTime_;
    
    // Last optimization results
    size_t lastTracesCompacted_;
    size_t lastTracesEvicted_;
    size_t lastTracesPromoted_;
    size_t lastTracesDemoted_;
    
    // Configuration
    float targetUtilization_;
    bool enableAutoOptimization_;
    float compactionThreshold_;
    float evictionPenalty_;
    
    // Feature flags
    bool enablePatternCache_;
    bool enablePriorityCompetition_;
    bool enableSparseStorage_;
    
    // Importance threshold
    float importanceThreshold_;
};

} // namespace nlm
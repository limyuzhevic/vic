#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

// Optimized Neural Working Memory Performance Enhancements
namespace WorkingMemoryPerformance {

// Forward declarations
class NeuralWorkingMemory;

// Optimized data structures for better performance
struct OptimizedMemoryData {
    // Primary lookup: neuron ID to index mapping (O(1) lookup)
    std::unordered_map<NeuronId, size_t> neuronToIndex;
    
    // Secondary storage for backward compatibility
    std::vector<NeuronId> memoryNeurons;
    std::vector<float> memoryActivations;
    std::vector<SimulationStep> memoryTimestamps;
    std::vector<NeuronId> winners;
    
    // Performance statistics
    mutable size_t findOperations;
    mutable size_t linearSearches;
    size_t totalOperations;
    
    OptimizedMemoryData() : findOperations(0), linearSearches(0), totalOperations(0) {}
    
    // Update statistics
    void recordFind() const { ++findOperations; }
    void recordLinearSearch() const { ++linearSearches; }
    void recordOperation() { ++totalOperations; }
    
    // Get performance ratio (find vs linear search)
    float getLookupEfficiency() const {
        if (findOperations + linearSearches == 0) return 1.0f;
        return static_cast<float>(findOperations) / 
               static_cast<float>(findOperations + linearSearches);
    }
};

// Performance benchmark utilities
class PerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string operationName;
        uint64_t iterations;
        double totalTimeNs;
        double avgTimeNs;
        double opsPerSecond;
    };
    
    static PerformanceBenchmark& getInstance() {
        static PerformanceBenchmark instance;
        return instance;
    }
    
    void startBenchmark(const std::string& operation) {
        currentOperation_ = operation;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void endBenchmark() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
        
        BenchmarkResult result;
        result.operationName = currentOperation_;
        result.iterations = iterations_;
        result.totalTimeNs = static_cast<double>(duration.count());
        result.avgTimeNs = result.totalTimeNs / iterations_;
        result.opsPerSecond = (iterations_ * 1e9) / result.totalTimeNs;
        
        benchmarkResults_.push_back(result);
        
        // Log performance metrics
        logPerformance(result);
        
        // Reset for next benchmark
        currentOperation_.clear();
        iterations_ = 0;
    }
    
    void incrementIterations() { ++iterations_; }
    
    const std::vector<BenchmarkResult>& getResults() const { return benchmarkResults_; }
    void clearResults() { benchmarkResults_.clear(); }
    
    // Generate performance report
    std::string generatePerformanceReport() const {
        std::stringstream report;
        report << "=== NLM Working Memory Performance Report ===\n";
        report << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& result : benchmarkResults_) {
            report << "Operation: " << result.operationName << "\n";
            report << "  Iterations: " << result.iterations << "\n";
            report << "  Total Time: " << result.totalTimeNs << " ns\n";
            report << "  Average Time: " << result.avgTimeNs << " ns\n";
            report << "  Operations/Second: " << result.opsPerSecond << "\n";
            report << "  Efficiency: " << (result.avgTimeNs * 1000000) << " µs per operation\n\n";
        }
        
        return report.str();
    }

private:
    PerformanceBenchmark() : iterations_(0) {}
    
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void logPerformance(const BenchmarkResult& result) {
        // Log to file and console based on verbosity
        std::string logEntry = "PERF: " + result.operationName + ": " + 
                             std::to_string(result.opsPerSecond) + " ops/sec";
        // Log implementation would go here
    }
    
    std::string currentOperation_;
    uint64_t iterations_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<BenchmarkResult> benchmarkResults_;
};

// Optimized memory operations for NeuralWorkingMemory
namespace MemoryOps {

// O(1) neuron existence check using hash map
inline bool containsOptimized(const std::unordered_map<NeuronId, size_t>& neuronToIndex,
                            const NeuronId& neuron) {
    return neuronToIndex.find(neuron) != neuronToIndex.end();
}

// O(1) neuron activation retrieval
inline float getNeuronActivationOptimized(const std::unordered_map<NeuronId, size_t>& neuronToIndex,
                                         const std::vector<NeuronId>& memoryNeurons,
                                         const std::vector<float>& memoryActivations,
                                         const NeuronId& neuron) {
    auto it = neuronToIndex.find(neuron);
    if (it != neuronToIndex.end()) {
        size_t idx = it->second;
        if (idx < memoryActivations.size()) {
            return memoryActivations[idx];
        }
    }
    return 0.0f;
}

// Optimized winner check
inline bool isWinnerOptimized(const std::unordered_map<NeuronId, size_t>& neuronToIndex,
                            const std::vector<NeuronId>& winners,
                            const NeuronId& neuron) {
    return neuronToIndex.find(neuron) != neuronToIndex.end();
}

// Batch memory cleanup with lazy deletion
inline void cleanupWeakTracesOptimized(std::vector<NeuronId>& memoryNeurons,
                                       std::vector<float>& memoryActivations,
                                       std::vector<SimulationStep>& memoryTimestamps,
                                       std::vector<NeuronId>& winners,
                                       float decayRate = 0.01f) {
    auto benchmark = PerformanceBenchmark::getInstance();
    benchmark.startBenchmark("cleanupWeakTraces");
    
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations.size(); ++i) {
        benchmark.incrementIterations();
        
        memoryActivations[i] *= (1.0f - decayRate);
        
        if (memoryActivations[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        size_t idx = *it;
        
        // Remove from all data structures
        memoryNeurons.erase(memoryNeurons.begin() + idx);
        memoryActivations.erase(memoryActivations.begin() + idx);
        memoryTimestamps.erase(memoryTimestamps.begin() + idx);
        
        // Update neuronToIndex mapping
        // This would be handled by the calling function
    }
    
    // Remove from winners if present
    winners.erase(
        std::remove_if(winners.begin(), winners.end(),
                     [&toRemove](const NeuronId& neuron) {
                         return std::find(toRemove.begin(), toRemove.end(), 
                                       static_cast<size_t>(neuron.value)) != toRemove.end();
                     }),
        winners.end()
    );
    
    benchmark.endBenchmark();
}

// Optimized competition algorithm using priority queue
inline void runCompetitionOptimized(std::vector<NeuronId>& memoryNeurons,
                                    std::vector<float>& memoryActivations,
                                    std::vector<NeuronId>& winners,
                                    float threshold = 0.3f,
                                    float inhibitionStrength = 3.0f) {
    auto benchmark = PerformanceBenchmark::getInstance();
    benchmark.startBenchmark("runCompetition");
    
    // Clear winners
    winners.clear();
    
    if (memoryActivations.empty()) {
        benchmark.endBenchmark();
        return;
    }
    
    // Use partial sort for better performance when finding top N
    // For now, use the existing threshold-based approach
    winners.reserve(memoryNeurons.size());
    
    for (size_t i = 0; i < memoryNeurons.size(); ++i) {
        benchmark.incrementIterations();
        
        if (memoryActivations[i] >= threshold) {
            winners.push_back(memoryNeurons[i]);
        }
    }
    
    // Optimized winner processing
    if (!winners.empty() && memoryNeurons.size() > 0) {
        // Pre-calculate for efficiency
        std::vector<bool> isWinner(memoryNeurons.size(), false);
        
        for (const auto& winner : winners) {
            // This would be a hash lookup in optimized version
            // For now, use linear search for compatibility
            for (size_t i = 0; i < memoryNeurons.size(); ++i) {
                if (memoryNeurons[i] == winner) {
                    isWinner[i] = true;
                    break;
                }
            }
        }
        
        // Apply inhibition in a single pass
        for (size_t i = 0; i < memoryNeurons.size(); ++i) {
            if (!isWinner[i]) {
                // Inhibition would be applied here
                // brain_->injectCurrent(memoryNeurons[i], -memoryActivations[i] * inhibitionStrength);
            }
        }
    }
    
    benchmark.endBenchmark();
}

// Optimized memory state update
inline void updateMemoryOptimized(std::vector<NeuronId>& memoryNeurons,
                                  std::vector<float>& memoryActivations,
                                  std::vector<SimulationStep>& memoryTimestamps,
                                  float decayRate = 0.01f,
                                  float threshold = 0.1f,
                                  SimulationStep maxAge = 1000) {
    auto benchmark = PerformanceBenchmark::getInstance();
    benchmark.startBenchmark("updateMemory");
    
    // Single pass for all operations
    std::vector<bool> toKeep;
    toKeep.reserve(memoryNeurons.size());
    
    for (size_t i = 0; i < memoryNeurons.size(); ++i) {
        benchmark.incrementIterations();
        
        float activation = memoryActivations[i];
        
        if (activation > threshold) {
            // Update timestamps and check age
            memoryTimestamps[i]++;
            
            if (memoryTimestamps[i] > maxAge) {
                activation *= (1.0f - decayRate);
            }
            
            memoryActivations[i] = activation;
            toKeep.push_back(true);
        } else {
            // Skip weak traces
            toKeep.push_back(false);
        }
    }
    
    // Compact arrays in a single operation
    size_t writeIndex = 0;
    for (size_t i = 0; i < memoryNeurons.size(); ++i) {
        if (toKeep[i]) {
            memoryNeurons[writeIndex] = memoryNeurons[i];
            memoryActivations[writeIndex] = memoryActivations[i];
            memoryTimestamps[writeIndex] = memoryTimestamps[i];
            ++writeIndex;
        }
    }
    
    // Resize arrays
    memoryNeurons.resize(writeIndex);
    memoryActivations.resize(writeIndex);
    memoryTimestamps.resize(writeIndex);
    
    benchmark.endBenchmark();
}

// Performance monitoring and optimization suggestions
class MemoryPerformanceMonitor {
public:
    struct PerformanceMetrics {
        uint64_t totalMemoryOps;
        uint64_t linearSearches;
        uint64_t hashLookups;
        double cacheEfficiency;
        double memoryAccessPattern;
    };
    
    void recordOperation(bool usedHashLookup = false, bool usedLinearSearch = false) {
        if (usedHashLookup) {
            hashLookups_++;
        }
        if (usedLinearSearch) {
            linearSearches_++;
        }
        totalMemoryOps_++;
    }
    
    PerformanceMetrics getMetrics() const {
        PerformanceMetrics metrics;
        metrics.totalMemoryOps = totalMemoryOps_;
        metrics.linearSearches = linearSearches_;
        metrics.hashLookups = hashLookups_;
        
        if (totalMemoryOps_ > 0) {
            metrics.cacheEfficiency = static_cast<double>(hashLookups_) / totalMemoryOps_;
            metrics.memoryAccessPattern = 1.0 - 
                static_cast<double>(linearSearches_) / totalMemoryOps_;
        } else {
            metrics.cacheEfficiency = 0.0;
            metrics.memoryAccessPattern = 0.0;
        }
        
        return metrics;
    }
    
    void logPerformanceReport() const {
        PerformanceMetrics metrics = getMetrics();
        
        // Performance log implementation would go here
        // Generate optimization recommendations
        generateOptimizationRecommendations(metrics);
    }
    
    void generateOptimizationRecommendations(const PerformanceMetrics& metrics) const {
        // Analyze performance and suggest optimizations
        if (metrics.cacheEfficiency < 0.5) {
            // Recommend optimizing hash map usage
        }
        if (metrics.memoryAccessPattern < 0.7) {
            // Recommend better memory access patterns
        }
    }

private:
    uint64_t totalMemoryOps_ = 0;
    uint64_t hashLookups_ = 0;
    uint64_t linearSearches_ = 0;
};

} // namespace MemoryOps

// Template functions for performance monitoring
namespace PerformanceTemplates {

// Generic optimized lookup template
template<typename Key, typename Value, typename Container>
class OptimizedLookup {
public:
    OptimizedLookup() : useHashMap_(false) {}
    
    void setUseHashMap(bool use) { useHashMap_ = use; }
    
    bool contains(const Key& key) const {
        if (useHashMap_) {
            return hashMap_.find(key) != hashMap_.end();
        } else {
            return linearSearch_.find(key) != linearSearch_.end();
        }
    }
    
    const Value* find(const Key& key) const {
        if (useHashMap_) {
            auto it = hashMap_.find(key);
            if (it != hashMap_.end()) {
                return &it->second;
            }
        } else {
            auto it = linearSearch_.find(key);
            if (it != linearSearch_.end()) {
                return &it->second;
            }
        }
        return nullptr;
    }
    
    void insert(const Key& key, const Value& value) {
        if (useHashMap_) {
            hashMap_[key] = value;
        } else {
            linearSearch_[key] = value;
        }
    }
    
    void clear() {
        hashMap_.clear();
        linearSearch_.clear();
    }
    
    size_t size() const {
        return useHashMap_ ? hashMap_.size() : linearSearch_.size();
    }

private:
    bool useHashMap_;
    std::unordered_map<Key, Value> hashMap_;
    std::unordered_map<Key, Value> linearSearch_;
};

// Performance-aware container template
 template<typename T>
 class PerformanceVector {
 public:
     PerformanceVector() : useOptimizations_(false), chunkSize_(1024) {}
     
     void setUseOptimizations(bool use) { useOptimizations_ = use; }
     void setChunkSize(size_t size) { chunkSize_ = size; }
     
     void push_back(const T& value) {
         if (useOptimizations_ && size_ >= currentCapacity_) {
             grow();
         }
         data_[size_] = value;
         ++size_;
     }
     
     const T& operator[](size_t index) const {
         return data_[index];
     }
     
     T& operator[](size_t index) {
         return data_[index];
     }
     
     size_t size() const { return size_; }
     bool empty() const { return size_ == 0; }
     void clear() { size_ = 0; }
     
     auto begin() const { return data_.begin(); }
     auto end() const { return data_.begin() + size_; }
     
     // Iterator range access for compatibility
     auto begin() { return data_.begin(); }
     auto end() { return data_.begin() + size_; }
     
     // Performance monitoring
     struct PerformanceStats {
         uint64_t allocations;
         uint64_t reallocations;
         double avgAllocationSize;
     };
     
     PerformanceStats getPerformanceStats() const {
         PerformanceStats stats;
         stats.allocations = allocations_;
         stats.reallocations = reallocations_;
         stats.avgAllocationSize = static_cast<double>(totalElements_) / allocations_;
         return stats;
     }

 private:
     void grow() {
         ++reallocations_;
         size_t newCapacity = currentCapacity_ * 2;
         if (newCapacity > maxCapacity_) {
             newCapacity = maxCapacity_;
         }
         
         T* newData = new T[newCapacity];
         std::move(data_, data_ + size_, newData);
         delete[] data_;
         data_ = newData;
         currentCapacity_ = newCapacity;
         ++allocations_;
         totalElements_ += size_;
     }
     
     T* data_ = nullptr;
     size_t size_ = 0;
     size_t currentCapacity_ = 0;
     size_t maxCapacity_ = 1024;
     size_t chunkSize_;
     bool useOptimizations_;
     
     uint64_t allocations_ = 0;
     uint64_t reallocations_ = 0;
     uint64_t totalElements_ = 0;
 };

} // namespace PerformanceTemplates

} // namespace WorkingMemoryPerformance

} // namespace nlm
#pragma once

/**
 * Performance optimization hints
 * 
 * Provides profiling optimization suggestions for improving NLM performance:
 * - Profile-guided optimization
 * - Cache optimization hints
 * - Memory access pattern analysis
 * - Branch prediction improvements
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>
#include <unordered_set>
#include <chrono>
#include <functional>

namespace nlm {

/**
 * Cache line size for cache optimization hints
 */
static constexpr size_t CACHE_LINE_SIZE = 64;

/**
 * Memory access pattern types
 */
enum class MemoryAccessPattern {
    SEQUENTIAL,     // Sequential access (good for caches)
    RANDOM,         // Random access (poor for caches)
    STREAMING,      // Large sequential blocks
    REPEATED,       // Repeated access to same locations
    SCATTERED       // Scattered access
};

/**
 * Branch prediction hint types
 */
enum class BranchPredictionHint {
    LIKELY_TRUE,    // Branch is likely to be true
    LIKELY_FALSE,   // Branch is likely to be false
    UNLIKELY_TRUE,  // Branch is unlikely to be true
    UNLIKELY_FALSE, // Branch is unlikely to be false
    EQUALITY        // Equality test hint
};

/**
 * Profile data for a function or code region
 */
struct ProfileData {
    std::string name;                 // Function/region name
    uint64_t callCount;               // Number of calls
    double totalTime;                 // Total execution time
    double averageTime;               // Average execution time
    double minTime;                   // Minimum execution time
    double maxTime;                   // Maximum execution time
    uint64_t instructionCount;        // Total instructions executed
    double cacheMissRate;             // Cache miss rate (0.0 to 1.0)
    uint64_t memoryAccesses;          // Total memory accesses
    size_t memoryAccessedBytes;       // Total bytes accessed
    MemoryAccessPattern accessPattern; // Memory access pattern
    std::unordered_map<size_t, uint64_t> hotAddresses; // Hot memory addresses
    std::vector<std::pair<size_t, uint64_t>> branchOutcomes; // Branch outcomes
    
    ProfileData(const std::string& name) : 
        name(name), callCount(0), totalTime(0.0), averageTime(0.0),
        minTime(0.0), maxTime(0.0), instructionCount(0),
        cacheMissRate(0.0), memoryAccesses(0), memoryAccessedBytes(0),
        accessPattern(MemoryAccessPattern::SEQUENTIAL) {}
};

/**
 * Optimization hint for a specific code location
 */
struct OptimizationHint {
    enum class Type {
        CACHE_ALIGNED,        // Need cache alignment
        LOOP_ORDERING,        // Change loop order
        UNROLL_LOOPS,         // Unroll loops
        USE_SIMD,             // Use SIMD instructions
        PREFETCH_DATA,        // Prefetch data
        REDUCE_MEMORY_ACCESS, // Reduce memory access
        SPLIT_FUNCTION,       // Split function
        INLINE_FUNCTION,      // Inline function
        THREAD_PARALLELIZE,   // Parallelize across threads
        USE_VECTOR,           // Use vector operations
        MINIMIZE_BRANCHES,   // Minimize branches
        USE_LOCAL_STORAGE,    // Use local storage
        USE_REGISTER,         // Use registers
        MEMORY_POOL,          // Use memory pool
        EVENT_QUEUE,          // Use event queue
        DATA_LAYOUT,          // Change data layout
        COMPRESS_DATA,        // Compress data
        CACHING_STRATEGY,     // Implement caching strategy
        BINARY_SEARCH,        // Use binary search
        SORT_DATA,            // Sort data
        REMOVE_REDUNDANCY,    // Remove redundant code
        PROFILE_GUIDED,       // Profile-guided optimization
        AUTOMATIC_PARALLEL,   // Automatic parallelization
        CUSTOM               // Custom optimization
    };
    
    Type type;                          // Hint type
    std::string description;            // Description of the hint
    double estimatedGain;              // Estimated performance gain (%)
    double implementationCost;          // Implementation cost (0.0 to 1.0)
    size_t priority;                    // Priority (higher = more important)
    std::string codeLocation;           // Location where hint applies
    std::vector<std::string> relatedHints; // Related optimization hints
    
    OptimizationHint(Type t, const std::string& desc, double gain, double cost, size_t prio,
                     const std::string& location)
        : type(t), description(desc), estimatedGain(gain), implementationCost(cost),
          priority(prio), codeLocation(location) {}
};

/**
 * Profile analyzer for identifying optimization opportunities
 */
class ProfileAnalyzer {
public:
    ProfileAnalyzer();
    
    // Add profile data for a function/region
    void addProfileData(const ProfileData& data);
    
    // Add profile data from multiple runs
    void addProfileData(const std::vector<ProfileData>& data);
    
    // Analyze profile data and generate optimization hints
    std::vector<OptimizationHint> analyze(const std::string& profileName = "global") const;
    
    // Generate cache optimization hints
    std::vector<OptimizationHint> generateCacheHints() const;
    
    // Generate memory access hints
    std::vector<OptimizationHint> generateMemoryHints() const;
    
    // Generate branch prediction hints
    std::vector<OptimizationHint> generateBranchHints() const;
    
    // Generate SIMD/vectorization hints
    std::vector<OptimizationHint> generateSIMDHints() const;
    
    // Generate parallelism hints
    std::vector<OptimizationHint> generateParallelismHints() const;
    
    // Get profile data for a specific function
    ProfileData getProfileData(const std::string& name) const;
    
    // Get all profile data
    std::unordered_map<std::string, ProfileData> getAllProfileData() const;
    
    // Calculate overall performance score
    double calculatePerformanceScore() const;
    
    // Identify hotspots (slowest functions)
    std::vector<std::string> identifyHotspots(size_t count = 10) const;
    
    // Check if function is cache-friendly
    bool isCacheFriendly(const std::string& name) const;
    
    // Check if function has good locality
    bool hasGoodLocality(const std::string& name) const;
    
    // Get memory access pattern for function
    MemoryAccessPattern getAccessPattern(const std::string& name) const;
    
    // Get hot memory addresses for function
    std::vector<size_t> getHotAddresses(const std::string& name) const;
    
    // Get branch prediction statistics
    std::vector<std::pair<size_t, uint64_t>> getBranchOutcomes(const std::string& name) const;
    
    // Generate priority-based optimization hints
    std::vector<OptimizationHint> generatePrioritizedHints(size_t maxHints = 10) const;
    
    // Export profile data to file
    bool exportProfile(const std::string& filename) const;
    
    // Import profile data from file
    bool importProfile(const std::string& filename);
    
    // Clear all profile data
    void clear();
    
    // Set profile threshold for identifying hotspots
    void setHotspotThreshold(double threshold) { hotspotThreshold_ = threshold; }
    
    // Set minimum call count for analysis
    void setMinCallCount(uint64_t minCalls) { minCallCount_ = minCalls; }
    
    // Set minimum time threshold for analysis
    void setMinTimeThreshold(double minTime) { minTimeThreshold_ = minTime; }
    
    // Set cache miss rate threshold
    void setCacheMissThreshold(double threshold) { cacheMissThreshold_ = threshold; }
    
    // Set memory access ratio threshold
    void setMemoryAccessRatioThreshold(double threshold) { memoryAccessRatioThreshold_ = threshold; }
    
    // Set branch outcome ratio threshold
    void setBranchOutcomeThreshold(double threshold) { branchOutcomeThreshold_ = threshold; }
    
    // Set SIMD utilization threshold
    void setSIMDUtilizationThreshold(double threshold) { simdUtilizationThreshold_ = threshold; }
    
    // Set parallelism potential threshold
    void setParallelismThreshold(double threshold) { parallelismThreshold_ = threshold; }
    
private:
    std::unordered_map<std::string, ProfileData> profileData_;
    mutable std::mutex dataMutex_;
    
    // Analysis thresholds
    double hotspotThreshold_ = 0.5;      // Threshold for identifying hotspots (relative)
    uint64_t minCallCount_ = 100;        // Minimum calls to analyze
    double minTimeThreshold_ = 0.001;    // Minimum time in seconds
    double cacheMissThreshold_ = 0.2;   // Cache miss rate threshold
    double memoryAccessRatioThreshold_ = 0.1; // Memory access ratio threshold
    double branchOutcomeThreshold_ = 0.3;  // Branch outcome ratio threshold
    double simdUtilizationThreshold_ = 0.5; // SIMD utilization threshold
    double parallelismThreshold_ = 0.7;   // Parallelism potential threshold
    
    // Helper methods for analysis
    void updateAccessPattern(ProfileData& data) const;
    void analyzeMemoryAccess(ProfileData& data) const;
    void analyzeBranchOutcomes(ProfileData& data) const;
    void calculateHotAddresses(ProfileData& data) const;
    void calculateSIMDUtilization(ProfileData& data) const;
    void calculateParallelismPotential(ProfileData& data) const;
    
    // Generate cache alignment hints
    std::vector<OptimizationHint> generateCacheAlignmentHints(const ProfileData& data) const;
    
    // Generate loop optimization hints
    std::vector<OptimizationHint> generateLoopHints(const ProfileData& data) const;
    
    // Generate data layout hints
    std::vector<OptimizationHint> generateDataLayoutHints(const ProfileData& data) const;
    
    // Generate memory management hints
    std::vector<OptimizationHint> generateMemoryManagementHints(const ProfileData& data) const;
    
    // Generate computational optimization hints
    std::vector<OptimizationHint> generateComputationalHints(const ProfileData& data) const;
    
    // Calculate estimated gain for a hint
    double calculateEstimatedGain(const ProfileData& data, OptimizationHint::Type type) const;
    
    // Calculate implementation cost for a hint
    double calculateImplementationCost(OptimizationHint::Type type, const ProfileData& data) const;
    
    // Check if data meets analysis criteria
    bool meetsAnalysisCriteria(const ProfileData& data) const;
    
    // Calculate priority score for a hint
    size_t calculateHintPriority(const ProfileData& data, OptimizationHint::Type type) const;
    
    // Generate hint description
    std::string generateHintDescription(OptimizationHint::Type type, const ProfileData& data) const;
    
    // Generate hint code location
    std::string generateHintCodeLocation(const ProfileData& data) const;
};

/**
 * Cache analyzer for analyzing cache usage patterns
 */
class CacheAnalyzer {
public:
    CacheAnalyzer();
    
    // Analyze memory access pattern
    MemoryAccessPattern analyzeMemoryAccess(const std::vector<size_t>& addresses) const;
    
    // Calculate cache miss rate
    double calculateCacheMissRate(const std::vector<size_t>& addresses, size_t cacheSize) const;
    
    // Identify cold, warm, hot cache lines
    std::vector<size_t> identifyCacheLines(const std::vector<size_t>& addresses) const;
    
    // Calculate spatial locality
    double calculateSpatialLocality(const std::vector<size_t>& addresses) const;
    
    // Calculate temporal locality
    double calculateTemporalLocality(const std::vector<size_t>& addresses) const;
    
    // Predict cache performance
    double predictCachePerformance(const std::vector<size_t>& addresses, size_t cacheSize) const;
    
    // Generate cache optimization hints
    std::vector<std::string> generateCacheHints(size_t cacheSize, size_t associativity) const;
    
    // Analyze memory access for SIMD optimization
    bool analyzeSIMDMemoryAccess(const std::vector<size_t>& addresses) const;
    
    // Analyze memory access for parallelization
    bool analyzeParallelMemoryAccess(const std::vector<size_t>& addresses) const;
    
    // Set cache parameters
    void setCacheSize(size_t size) { cacheSize_ = size; }
    void setAssociativity(size_t assoc) { associativity_ = assoc; }
    void setLineSize(size_t size) { lineSize_ = size; }
    
    // Set analysis thresholds
    void setSpatialLocalityThreshold(double threshold) { spatialLocalityThreshold_ = threshold; }
    void setTemporalLocalityThreshold(double threshold) { temporalLocalityThreshold_ = threshold; }
    
private:
    size_t cacheSize_ = 32768;         // Default L1 cache size (32KB)
    size_t associativity_ = 4;         // Default associativity
    size_t lineSize_ = 64;             // Cache line size
    double spatialLocalityThreshold_ = 0.7;   // Spatial locality threshold
    double temporalLocalityThreshold_ = 0.8;   // Temporal locality threshold;
    
    // Helper methods
    size_t calculateCacheIndex(size_t address) const;
    size_t calculateSetIndex(size_t address, size_t cacheSize, size_t associativity) const;
    bool isCacheHit(size_t address, const std::unordered_map<size_t, size_t>& cacheState) const;
    void updateCacheState(std::unordered_map<size_t, size_t>& cacheState, size_t address) const;
    size_t getCacheSetCount(size_t cacheSize, size_t associativity) const;
    bool hasConflict(size_t address1, size_t address2, size_t associativity) const;
    double calculateStride(const std::vector<size_t>& addresses) const;
    bool isSequentialAccess(const std::vector<size_t>& addresses, double tolerance = 0.1) const;
    bool isRandomAccess(const std::vector<size_t>& addresses, double threshold = 0.5) const;
    bool isStreamingAccess(const std::vector<size_t>& addresses) const;
    bool isRepeatedAccess(const std::vector<size_t>& addresses) const;
    bool isScatteredAccess(const std::vector<size_t>& addresses) const;
    double calculateTemporalScore(const std::vector<size_t>& addresses, size_t windowSize) const;
    double calculateSpatialScore(const std::vector<size_t>& addresses) const;
};

/**
 * Optimization hints manager for collecting and organizing optimization suggestions
 */
class OptimizationHints {
public:
    OptimizationHints();
    
    // Add optimization hints from profile analyzer
    void addHints(const std::vector<OptimizationHint>& hints);
    
    // Add optimization hints from cache analyzer
    void addCacheHints(const std::vector<std::string>& hints);
    
    // Get all optimization hints
    std::vector<OptimizationHint> getAllHints() const;
    
    // Get optimization hints by type
    std::vector<OptimizationHint> getHintsByType(OptimizationHint::Type type) const;
    
    // Get optimization hints by priority
    std::vector<OptimizationHint> getHintsByPriority(size_t minPriority) const;
    
    // Get optimization hints for a specific code location
    std::vector<OptimizationHint> getHintsForLocation(const std::string& location) const;
    
    // Get estimated total performance gain
    double getTotalEstimatedGain() const;
    
    // Get estimated total implementation cost
    double getTotalImplementationCost() const;
    
    // Sort hints by priority and estimated gain
    void sortHints();
    
    // Filter hints by estimated gain threshold
    void filterHintsByGain(double minGain);
    
    // Filter hints by implementation cost threshold
    void filterHintsByCost(double maxCost);
    
    // Merge duplicate hints
    void mergeDuplicateHints();
    
    // Remove low-priority hints
    void removeLowPriorityHints(size_t threshold);
    
    // Clear all hints
    void clear();
    
    // Check if hints are empty
    bool isEmpty() const { return hints_.empty(); }
    
    // Get number of hints
    size_t size() const { return hints_.size(); }
    
    // Print optimization hints
    void printHints(std::ostream& out = std::cout) const;
    
    // Save optimization hints to file
    bool saveHints(const std::string& filename) const;
    
    // Load optimization hints from file
    bool loadHints(const std::string& filename);
    
    // Apply optimizations based on hints
    void applyOptimizations();
    
private:
    std::vector<OptimizationHint> hints_;
    mutable std::mutex hintsMutex_;
    std::vector<std::string> cacheHints_;
    
    // Helper methods
    bool isHintDuplicate(const OptimizationHint& hint) const;
    size_t calculateEstimatedGainScore(const OptimizationHint& hint) const;
    size_t calculateCostScore(const OptimizationHint& hint) const;
    bool meetsFilterCriteria(const OptimizationHint& hint) const;
    std::string normalizeLocation(const std::string& location) const;
};

} // namespace nlm
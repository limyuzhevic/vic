// Memory analyzer for neural simulation memory usage analysis
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <fstream>

namespace nlm {

struct MemoryRegion {
    std::string regionName;
    size_t capacity;
    size_t currentUsage;
    size_t peakUsage;
    float utilization;
    std::vector<size_t> usageHistory;
    std::chrono::steady_clock::time_point lastAccess;
    bool isCritical;
    std::string allocationPattern;
    std::vector<std::string> allocationSources;
};

struct MemoryAllocation {
    void* address;
    size_t size;
    std::string allocationType;
    std::string sourceFile;
    int sourceLine;
    std::string allocationFunction;
    SimulationStep allocationStep;
    std::chrono::steady_clock::time_point timestamp;
    bool isFreed;
    SimulationStep deallocationStep;
    std::chrono::steady_clock::time_point deallocationTimestamp;
    std::string stackTrace;
};

struct MemoryLeak {
    std::string leakType;
    std::string sourceComponent;
    std::string memoryPool;
    size_t leakSize;
    SimulationStep firstDetected;
    SimulationStep lastObserved;
    std::vector<MemoryAllocation> involvedAllocations;
    std::vector<SimulationStep> leakPoints;
    float growthRate;
    bool isPersistent;
    bool isCatastrophic;
    std::string description;
    std::string remediation;
    std::string confidence;
};

struct MemoryFragmentation {
    size_t totalMemory;
    size_t fragmentedMemory;
    float fragmentationRatio;
    std::vector<size_t> freeBlockSizes;
    size_t largestFreeBlock;
    bool isSevere;
    std::string fragmentationType;
};

struct MemoryUsageProfile {
    SimulationStep step;
    Timestamp timestamp;
    size_t totalMemoryUsage;
    size_t peakMemoryUsage;
    float memoryEfficiency;
    std::vector<MemoryRegion> memoryRegions;
    std::vector<MemoryAllocation> activeAllocations;
    MemoryFragmentation fragmentation;
    std::vector<MemoryLeak> detectedLeaks;
    bool hasLeaks;
    bool hasFragmentation;
    std::string memoryHealth;
};

struct AllocationPattern {
    std::string patternName;
    float frequency;
    size_t averageSize;
    SimulationStep averageInterval;
    std::string patternType;
    bool isNormal;
    bool isSuspicious;
    std::string description;
};

class MemoryAnalyzer {
public:
    MemoryAnalyzer();
    ~MemoryAnalyzer();
    
    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    void update(const VisualizationData& data);
    
    // Memory tracking
    void recordAllocation(void* address, size_t size, 
                         const std::string& allocationType,
                         const std::string& sourceFile, int sourceLine,
                         const std::string& allocationFunction,
                         const std::string& stackTrace,
                         SimulationStep step);
    void recordDeallocation(void* address, SimulationStep step);
    void updateMemoryUsage(size_t usage);
    
    // Memory analysis
    MemoryUsageProfile getCurrentMemoryUsage() const;
    std::vector<MemoryUsageProfile> getMemoryUsageHistory() const;
    std::vector<MemoryLeak> detectMemoryLeaks() const;
    bool hasMemoryLeaks() const;
    MemoryFragmentation analyzeFragmentation() const;
    std::vector<AllocationPattern> analyzeAllocationPatterns() const;
    std::string assessMemoryHealth() const;
    
    // Region-specific analysis
    MemoryRegion analyzeMemoryRegion(const std::string& regionName) const;
    std::vector<MemoryRegion> getAllMemoryRegions() const;
    size_t getRegionMemoryUsage(const std::string& regionName) const;
    
    // Leak detection
    void enableMemoryLeakDetection(bool enable);
    bool isMemoryLeakDetectionEnabled() const;
    void setLeakDetectionThreshold(size_t threshold);
    size_t getLeakDetectionThreshold() const;
    
    // Fragmentation detection
    void enableFragmentationDetection(bool enable);
    bool isFragmentationDetectionEnabled() const;
    void setFragmentationThreshold(float threshold);
    float getFragmentationThreshold() const;
    
    // Pattern analysis
    void enablePatternAnalysis(bool enable);
    bool isPatternAnalysisEnabled() const;
    void setPatternAnalysisThreshold(float threshold);
    float getPatternAnalysisThreshold() const;
    
    // Configuration
    void setMaxMemoryHistory(size_t maxHistory);
    void setMaxMemoryBlocks(size_t maxBlocks);
    
    // Configuration getters
    size_t getMaxMemoryHistory() const;
    size_t getMaxMemoryBlocks() const;
    
    // Export
    void exportMemoryProfile(const std::string& filepath, ExportFormat format = ExportFormat::JSON) const;
    void exportLeakAnalysis(const std::string& filepath) const;
    void exportFragmentationAnalysis(const std::string& filepath) const;
    void exportAllocationPatterns(const std::string& filepath) const;
    
    // Visualization preparation
    void prepareMemoryDataForVisualization(std::vector<float>& memoryData) const;
    void prepareLeakDataForVisualization(std::vector<std::string>& leakData) const;
    void prepareFragmentationDataForVisualization(std::vector<float>& fragmentData) const;
    void prepareRegionDataForVisualization(std::vector<std::string>& regionData) const;

private:
    std::shared_ptr<Brain> brain_;
    
    // Configuration
    bool enableMemoryLeakDetection_;
    bool enableFragmentationDetection_;
    bool enablePatternAnalysis_;
    size_t memoryLeakDetectionThreshold_;
    float fragmentationThreshold_;
    float patternAnalysisThreshold_;
    size_t maxMemoryHistory_;
    size_t maxMemoryBlocks_;
    
    // Memory tracking
    size_t currentMemoryUsage_;
    size_t peakMemoryUsage_;
    std::vector<size_t> memoryUsageHistory_;
    std::vector<MemoryAllocation> memoryAllocations_;
    std::vector<MemoryRegion> memoryRegions_;
    
    // Analysis
    std::vector<MemoryLeak> detectedLeaks_;
    MemoryFragmentation currentFragmentation_;
    std::vector<AllocationPattern> allocationPatterns_;
    
    // Tracking
    std::map<std::string, size_t> regionAllocations_;
    std::chrono::steady_clock::time_point lastCleanupTime_;
    
    // Analysis helpers
    void updateMemoryRegions();
    void trackMemoryAllocations() const;
    void detectAllocationLeaks() const;
    void analyzeFragmentation() const;
    void analyzeAllocationPatterns() const;
    void cleanupMemoryBlocks() const;
    void updateMemoryHealth() const;
    
    // Leak detection helpers
    void detectAllocationLeaksInternal(std::vector<MemoryLeak>& leaks) const;
    void detectReferenceLeaksInternal(std::vector<MemoryLeak>& leaks) const;
    void detectPoolLeaksInternal(std::vector<MemoryLeak>& leaks) const;
    
    // Fragmentation analysis helpers
    void computeFragmentation() const;
    void classifyFragmentationType() const;
    void analyzeFreeBlockDistribution() const;
    
    // Pattern analysis helpers
    void identifyAllocationPatterns() const;
    void classifyPatternNormalcy() const;
    void computePatternStatistics() const;
    
    // Region analysis helpers
    void discoverMemoryRegions() const;
    void analyzeRegionUsage(const std::string& regionName) const;
    void trackRegionAllocations(const std::string& regionName, size_t size) const;
    
    // Health assessment helpers
    void assessOverallMemoryHealth() const;
    void evaluateMemoryEfficiency() const;
    void checkMemoryStability() const;
    
    // Export helpers
    void exportMemoryProfileToJSON(const std::string& filepath) const;
    void exportMemoryProfileToCSV(const std::string& filepath) const;
    void exportMemoryProfileToText(const std::string& filepath) const;
    
    mutable std::mutex mutex_;
};

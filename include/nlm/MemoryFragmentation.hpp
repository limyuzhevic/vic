pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <chrono>
#include <iostream>

namespace nlm {

// Fragmentation analysis types
enum class FragmentationLevel {
    None,
    Low,
    Medium,
    High,
    Critical
};

struct MemoryBlockInfo {
    uint64_t address;
    size_t size;
    bool isAllocated;
    uint64_t threadId;
    uint64_t timestamp;
    std::string allocationName;
    
    MemoryBlockInfo(uint64_t addr, size_t sz, bool allocated, uint64_t tid, uint64_t ts, const std::string& name = "")
        : address(addr), size(sz), isAllocated(allocated), threadId(tid), timestamp(ts), allocationName(name) {}
};

struct FragmentationStats {
    FragmentationLevel level;
    size_t totalExternalFragmentation;
    size_t totalInternalFragmentation;
    size_t largestFreeBlock;
    size_t smallestAllocatedBlock;
    float fragmentationRatio;
    uint64_t freeBlockCount;
    uint64_t allocatedBlockCount;
    std::vector<size_t> freeBlockSizes;
    std::vector<size_t> allocatedBlockSizes;
    
    FragmentationStats() : level(FragmentationLevel::None), totalExternalFragmentation(0),
                           totalInternalFragmentation(0), largestFreeBlock(0),
                           smallestAllocatedBlock(0), fragmentationRatio(0.0f),
                           freeBlockCount(0), allocatedBlockCount(0) {}
};

// Memory heap analyzer
class MemoryHeapAnalyzer {
private:
    std::mutex mutex;
    std::vector<MemoryBlockInfo> allBlocks;
    std::map<uint64_t, std::vector<size_t>> blocksByAddress;
    std::vector<uint64_t> freeRegions;
    std::vector<uint64_t> allocatedRegions;
    uint64_t totalHeapSize;
    uint64_t usedHeapSize;
    
public:
    MemoryHeapAnalyzer() : totalHeapSize(0), usedHeapSize(0) {}
    
    void addBlock(uint64_t address, size_t size, bool isAllocated, 
                  const std::string& allocationName = "", uint64_t threadId = 0) {
        std::lock_guard<std::mutex> lock(mutex);
        
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        allBlocks.emplace_back(address, size, isAllocated, threadId, timestamp, allocationName);
        
        if (isAllocated) {
            allocatedRegions.push_back(address);
        } else {
            freeRegions.push_back(address);
        }
        
        // Update stats
        if (size > totalHeapSize) {
            totalHeapSize = size;
        }
        
        if (isAllocated) {
            usedHeapSize += size;
        }
        
        blocksByAddress[address] = std::vector<size_t>();
    }
    
    void removeBlock(uint64_t address) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Find and remove block
        auto it = std::find_if(allBlocks.begin(), allBlocks.end(),
                               [address](const MemoryBlockInfo& block) { return block.address == address; });n
        if (it != allBlocks.end()) {
            if (it->isAllocated) {
                usedHeapSize -= it->size;
                auto allocIt = std::find(allocatedRegions.begin(), allocatedRegions.end(), address);
                if (allocIt != allocatedRegions.end()) {
                    allocatedRegions.erase(allocIt);
                }
            } else {
                auto freeIt = std::find(freeRegions.begin(), freeRegions.end(), address);
                if (freeIt != freeRegions.end()) {
                    freeRegions.erase(freeIt);
                }
            }
            allBlocks.erase(it);
        }
    }
    
    FragmentationStats analyze() {
        std::lock_guard<std::mutex> lock(mutex);
        FragmentationStats stats;
        
        // Sort regions
        std::sort(freeRegions.begin(), freeRegions.end());
        std::sort(allocatedRegions.begin(), allocatedRegions.end());
        
        // Analyze free regions
        stats.freeBlockCount = freeRegions.size();
        
        if (!freeRegions.empty()) {
            stats.largestFreeBlock = freeRegions.back();
            for (size_t addr : freeRegions) {
                stats.freeBlockSizes.push_back(addr);
            }
        }
        
        // Analyze allocated regions
        stats.allocatedBlockCount = allocatedRegions.size();
        
        if (!allocatedRegions.empty()) {
            stats.smallestAllocatedBlock = allocatedRegions.front();
            for (size_t addr : allocatedRegions) {
                stats.allocatedBlockSizes.push_back(addr);
            }
        }
        
        // Calculate external fragmentation
        stats.totalExternalFragmentation = calculateExternalFragmentation();
        
        // Calculate internal fragmentation
        stats.totalInternalFragmentation = calculateInternalFragmentation();
        
        // Calculate fragmentation ratio
        if (totalHeapSize > 0) {
            stats.fragmentationRatio = static_cast<float>(stats.totalExternalFragmentation) / totalHeapSize;
        }
        
        // Determine fragmentation level
        stats.level = determineFragmentationLevel(stats.fragmentationRatio, stats.largestFreeBlock);
        
        return stats;
    }
    
    std::string getFragmentationReport() const {
        std::stringstream ss;
        ss << "=== Memory Fragmentation Analysis ===\n";
        
        FragmentationStats stats = const_cast<MemoryHeapAnalyzer*>(this)->analyze();
        
        ss << "Heap Usage:\n";
        ss << "  Total Heap Size: " << totalHeapSize << " bytes\n";
        ss << "  Used Heap Size: " << usedHeapSize << " bytes\n";
        ss << "  Utilization: " << (totalHeapSize > 0 ? (usedHeapSize * 100.0f / totalHeapSize) : 0.0f) << "%\n\n";
        
        ss << "Fragmentation Statistics:\n";
        ss << "  Fragmentation Level: " << fragmentationLevelToString(stats.level) << "\n";
        ss << "  External Fragmentation: " << stats.totalExternalFragmentation << " bytes\n";
        ss << "  Internal Fragmentation: " << stats.totalInternalFragmentation << " bytes\n";
        ss << "  Fragmentation Ratio: " << (stats.fragmentationRatio * 100.0f) << "%\n";
        ss << "  Largest Free Block: " << stats.largestFreeBlock << " bytes\n";
        ss << "  Free Blocks: " << stats.freeBlockCount << "\n";
        ss << "  Allocated Blocks: " << stats.allocatedBlockCount << "\n\n";
        
        ss << "Block Size Distribution:\n";
        
        if (!stats.freeBlockSizes.empty()) {
            ss << "  Free Block Sizes:\n";
            std::sort(stats.freeBlockSizes.begin(), stats.freeBlockSizes.end());
            for (size_t size : stats.freeBlockSizes) {
                ss << "    " << size << " bytes\n";
            }
            ss << "\n";
        }
        
        if (!stats.allocatedBlockSizes.empty()) {
            ss << "  Allocated Block Sizes:\n";
            std::sort(stats.allocatedBlockSizes.begin(), stats.allocatedBlockSizes.end());
            for (size_t size : stats.allocatedBlockSizes) {
                ss << "    " << size << " bytes\n";
            }
            ss << "\n";
        }
        
        // Provide recommendations
        ss << "Recommendations:\n";
        if (stats.level == FragmentationLevel::Critical) {
            ss << "  - CRITICAL: High fragmentation detected! Consider compaction or reallocation.\n";
        } else if (stats.level == FragmentationLevel::High) {
            ss << "  - High fragmentation detected. Consider compaction or reallocation.\n";
        } else if (stats.level == FragmentationLevel::Medium) {
            ss << "  - Moderate fragmentation. Monitor and consider optimization.\n";
        } else {
            ss << "  - Fragmentation levels are acceptable.\n";
        }
        
        if (stats.largestFreeBlock < 1024) { // Less than 1KB
            ss << "  - Large free space fragmentation detected. Consider memory compaction.\n";
        }
        
        return ss.str();
    }
    
private:
    size_t calculateExternalFragmentation() {
        if (freeRegions.size() < 2) return 0;
        
        size_t totalExternal = 0;
        for (size_t i = 0; i < freeRegions.size() - 1; ++i) {
            size_t gap = freeRegions[i + 1] - (freeRegions[i] + allBlocks[i].size);
            if (gap > 0) {
                totalExternal += gap;
            }
        }
        
        return totalExternal;
    }
    
    size_t calculateInternalFragmentation() {
        size_t totalInternal = 0;
        
        // Simple approximation of internal fragmentation
        // In a real implementation, this would track alignment requirements
        for (const auto& block : allBlocks) {
            if (block.isAllocated && block.size > 0) {
                size_t alignment = 16; // Typical alignment
                size_t remainder = block.size % alignment;
                if (remainder != 0) {
                    totalInternal += alignment - remainder;
                }
            }
        }
        
        return totalInternal;
    }
    
    FragmentationLevel determineFragmentationLevel(float ratio, size_t largestFree) {
        if (ratio < 0.05f && largestFree >= 1024 * 1024) { // <5% and >1MB free
            return FragmentationLevel::None;
        } else if (ratio < 0.15f && largestFree >= 256 * 1024) { // <15% and >256KB free
            return FragmentationLevel::Low;
        } else if (ratio < 0.30f) { // <30%
            return FragmentationLevel::Medium;
        } else if (ratio < 0.50f) { // <50%
            return FragmentationLevel::High;
        } else {
            return FragmentationLevel::Critical;
        }
    }
    
    std::string fragmentationLevelToString(FragmentationLevel level) {
        switch (level) {
            case FragmentationLevel::None: return "None";
            case FragmentationLevel::Low: return "Low";
            case FragmentationLevel::Medium: return "Medium";
            case FragmentationLevel::High: return "High";
            case FragmentationLevel::Critical: return "Critical";
            default: return "Unknown";
        }
    }
};

// Memory compaction analyzer
class MemoryCompactor {
public:
    struct CompactionInfo {
        uint64_t freeSpaceDefragmented;
        uint64_t allocatedSpaceMoved;
        uint64_t timeRequired;
        float compactionRatio;
        bool wasCompacted;
        
        CompactionInfo() : freeSpaceDefragmented(0), allocatedSpaceMoved(0),
                          timeRequired(0), compactionRatio(0.0f), wasCompacted(false) {}
    };
    
    // Simulate memory compaction
    CompactionInfo simulateCompaction(size_t targetFragmentation = 0.10f) {
        CompactionInfo info;
        
        // In a real implementation, this would:
        // 1. Scan the memory for allocated blocks
        // 2. Move allocated blocks to reduce fragmentation
        // 3. Update metadata
        // 4. Report compaction statistics
        
        // For simulation, we'll just return a realistic compaction estimate
        info.freeSpaceDefragmented = 1024 * 1024; // 1MB
        info.allocatedSpaceMoved = 512 * 1024;   // 512KB
        info.timeRequired = 50;                  // 50ms (estimated)
        info.compactionRatio = 0.8f;             // 80% effective
        info.wasCompacted = (targetFragmentation < 0.20f); // Compact if fragmentation > 20%
        
        return info;
    }
    
    std::string getCompactionRecommendation(const FragmentationStats& stats) {
        std::stringstream ss;
        ss << "=== Memory Compaction Recommendation ===\n\n";
        
        if (stats.level == FragmentationLevel::None) {
            ss << "Current fragmentation is optimal. No compaction needed.\n";
            return ss.str();
        }
        
        if (stats.level == FragmentationLevel::Low) {
            ss << "Fragmentation is low. Consider monitoring and minor optimizations.\n";
            return ss.str();
        }
        
        ss << "Compaction Analysis:\n";
        ss << "  Current fragmentation level: " << fragmentationLevelToString(stats.level) << "\n";
        ss << "  Largest free block: " << stats.largestFreeBlock << " bytes\n";
        ss << "  Fragmentation ratio: " << (stats.fragmentationRatio * 100.0f) << "%\n\n";
        
        CompactionInfo info = simulateCompaction();
        
        if (!info.wasCompacted) {
            ss << "Recommendation: Defer compaction. Fragmentation levels are manageable.\n";
        } else {
            ss << "Compaction Benefits:\n";
            ss << "  - Defragments " << info.freeSpaceDefragmented << " bytes of free space\n";
            ss << "  - Moves " << info.allocatedSpaceMoved << " bytes of allocated space\n";
            ss << "  - Estimated time: " << info.timeRequired << "ms\n";
            ss << "  - Compaction ratio: " << (info.compactionRatio * 100.0f) << "%\n\n";
            ss << "Recommendation: Perform compaction to improve memory efficiency.\n";
        }
        
        return ss.str();
    }
    
private:
    std::string fragmentationLevelToString(FragmentationLevel level) {
        switch (level) {
            case FragmentationLevel::None: return "None";
            case FragmentationLevel::Low: return "Low";
            case FragmentationLevel::Medium: return "Medium";
            case FragmentationLevel::High: return "High";
            case FragmentationLevel::Critical: return "Critical";
            default: return "Unknown";
        }
    }
};

// Global fragmentation analyzer instances
inline MemoryHeapAnalyzer& getHeapAnalyzer() {
    static MemoryHeapAnalyzer analyzer;
    return analyzer;
}

inline MemoryCompactor& getCompactor() {
    static MemoryCompactor compactor;
    return compactor;
}

// Convenience functions for fragmentation analysis
inline void addMemoryBlock(uint64_t address, size_t size, bool isAllocated,
                          const std::string& allocationName = "") {
    getHeapAnalyzer().addBlock(address, size, isAllocated, allocationName);
}

inline void removeMemoryBlock(uint64_t address) {
    getHeapAnalyzer().removeBlock(address);
}

inline std::string analyzeMemoryFragmentation() {
    return getHeapAnalyzer().getFragmentationReport();
}

inline std::string getCompactionRecommendation() {
    FragmentationStats stats = getHeapAnalyzer().analyze();
    return getCompactor().getCompactionRecommendation(stats);
}

inline void takeFragmentationSnapshot() {
    // Record current memory state for fragmentation tracking
    // This would integrate with the memory tracking system
    // For now, it's a placeholder
    NLM_LOG_INFO("Fragmentation snapshot taken")
}

} // namespace nlm

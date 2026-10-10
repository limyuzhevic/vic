#include "MemoryPoolManager.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace nlm {

class MemoryPoolManager::Impl {
public:
    Impl() : isEnabled_(true), verboseLogging_(false), allocationThreshold_(4096), 
             fragmentationThreshold_(0.1), fragmentationRecovery_(true),
             totalMemoryUsage_(0), totalFragmentation_(0),
             allocationCount_(0), deallocationCount_(0) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Impl() = default;
    
    void createPool(const std::string& poolName, size_t blockSize, size_t initialCapacity) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (poolExists(poolName)) {
            if (verboseLogging_) {
                // std::cout << "Warning: Pool " << poolName << " already exists" << std::endl;
            }
            return;
        }
        
        PoolData pool;
        pool.poolName = poolName;
        pool.blockSize = blockSize;
        pool.capacity = initialCapacity > 0 ? initialCapacity : calculateDefaultCapacity(blockSize);
        pool.allocatedBlocks = 0;
        pool.freeBlocks.clear();
        pool.allocations.clear();
        pool.deallocations.clear();
        pool.fragmentationLevel = 0;
        pool.allocationCount = 0;
        pool.deallocationCount = 0;
        pool.utilization = 0.0;
        pool.fragmentationRatio = 0.0;
        pool.totalAllocatedSize = 0;
        pool.peakAllocatedSize = 0;
        pool.lastAllocation = 0.0;
        pool.lastDeallocation = 0.0;
        
        // Initialize free blocks
        for (size_t i = 0; i < pool.capacity; ++i) {
            pool.freeBlocks.push_back(i);
        }
        
        pools_[poolName] = pool;
        
        if (verboseLogging_) {
            // std::cout << "Created pool " << poolName << " with block size " << blockSize 
            //           << " and capacity " << pool.capacity << std::endl;
        }
    }
    
    bool destroyPool(const std::string& poolName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return false;
        }
        
        // Clean up allocated blocks
        for (const auto& blockId : it->second.allocatedBlocks) {
            // In a real implementation, we would free the memory here
        }
        
        pools_.erase(it);
        
        if (verboseLogging_) {
            // std::cout << "Destroyed pool " << poolName << std::endl;
        }
        
        return true;
    }
    
    bool poolExists(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pools_.find(poolName) != pools_.end();
    }
    
    std::vector<std::string> getPoolNames() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(pools_.size());
        for (const auto& pair : pools_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void* allocate(const std::string& poolName, size_t size,
                   const std::string& sourceFile,
                   int sourceLine,
                   const std::string& functionName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isEnabled_ || !poolExists(poolName)) {
            return nullptr;
        }
        
        auto& pool = pools_[poolName];
        
        // Check allocation threshold
        if (size > allocationThreshold_) {
            if (verboseLogging_) {
                // std::cout << "Large allocation request: " << size << " bytes in pool " << poolName << std::endl;
            }
        }
        
        // Find suitable free block (simplified best-fit)
        size_t blockIndex = findBestFit(size, pool.freeBlocks);
        
        if (blockIndex == (size_t)-1) {
            if (verboseLogging_) {
                // std::cout << "Pool " << poolName << " is full" << std::endl;
            }
            return nullptr;
        }
        
        // Remove block from free list
        pool.freeBlocks.erase(std::remove(pool.freeBlocks.begin(), pool.freeBlocks.end(), blockIndex), pool.freeBlocks.end());
        pool.allocatedBlocks.push_back(blockIndex);
        pool.allocatedSize[blockIndex] = size;
        pool.allocationCount++;
        pool.totalAllocatedSize += size;
        pool.peakAllocatedSize = std::max(pool.peakAllocatedSize, pool.totalAllocatedSize);
        pool.lastAllocation = getCurrentTimestamp();
        
        // Create allocation record
        AllocationRequest alloc;
        alloc.size = size;
        alloc.sourceFile = sourceFile;
        alloc.sourceLine = sourceLine;
        alloc.functionName = functionName;
        alloc.timestamp = pool.lastAllocation;
        alloc.allocationId = generateAllocationId();
        alloc.typeName = "memory_block";
        alloc.alignment = 0; // Simplified
        
        pool.allocations[alloc.allocationId] = alloc;
        
        void* address = reinterpret_cast<void*>(blockIndex);
        
        if (verboseLogging_) {
            // std::cout << "Allocated " << size << " bytes from pool " << poolName 
            //           << " block index " << blockIndex << " (" << pool.allocatedBlocks.size() << "/" << pool.capacity << ")" << std::endl;
        }
        
        return address;
    }
    
    void deallocate(const std::string& poolName, void* address) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isEnabled_ || !poolExists(poolName) || address == nullptr) {
            return;
        }
        
        auto& pool = pools_[poolName];
        
        // Convert address back to block index
        size_t blockIndex = reinterpret_cast<size_t>(address);
        
        // Find and remove from allocated blocks
        auto it = std::find(pool.allocatedBlocks.begin(), pool.allocatedBlocks.end(), blockIndex);
        if (it == pool.allocatedBlocks.end()) {
            if (verboseLogging_) {
                // std::cout << "Warning: Attempted to deallocate unknown block in pool " << poolName << std::endl;
            }
            return;
        }
        
        pool.allocatedBlocks.erase(it);
        pool.freeBlocks.push_back(blockIndex);
        pool.deallocationCount++;
        pool.totalAllocatedSize -= pool.allocatedSize[blockIndex];
        pool.lastDeallocation = getCurrentTimestamp();
        
        // Remove allocated size tracking
        pool.allocatedSize.erase(blockIndex);
        
        if (verboseLogging_) {
            // std::cout << "Deallocated block index " << blockIndex << " from pool " << poolName << std::endl;
        }
    }
    
    bool deallocate(const std::string& poolName, const std::string& allocationId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isEnabled_ || !poolExists(poolName)) {
            return false;
        }
        
        auto& pool = pools_[poolName];
        
        auto it = pool.allocations.find(allocationId);
        if (it == pool.allocations.end()) {
            return false;
        }
        
        // Convert allocation to address
        size_t blockIndex = std::hash<std::string>{}(allocationId) % pool.capacity;
        
        // Deallocate block
        deallocate(poolName, reinterpret_cast<void*>(blockIndex));
        
        // Remove allocation record
        pool.allocations.erase(it);
        
        return true;
    }
    
    void trackAllocation(const std::string& poolName, const AllocationRequest& request) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!poolExists(poolName)) return;
        
        auto& pool = pools_[poolName];
        pool.allocations[request.allocationId] = request;
        pool.allocationCount++;
        pool.totalAllocatedSize += request.size;
        pool.peakAllocatedSize = std::max(pool.peakAllocatedSize, pool.totalAllocatedSize);
        pool.lastAllocation = request.timestamp;
    }
    
    void trackDeallocation(const std::string& poolName, const std::string& allocationId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!poolExists(poolName)) return;
        
        auto& pool = pools_[poolName];
        auto it = pool.allocations.find(allocationId);
        if (it != pool.allocations.end()) {
            pool.totalAllocatedSize -= it->second.size;
            pool.deallocationCount++;
            pool.lastDeallocation = getCurrentTimestamp();
        }
    }
    
    PoolStats getPoolStats(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        PoolStats stats;
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return stats;
        }
        
        const auto& pool = it->second;
        
        stats.allocatedBlocks = pool.allocatedBlocks.size();
        stats.totalBlocks = pool.capacity;
        stats.fragmentationLevel = pool.fragmentationLevel;
        stats.allocationCount = pool.allocationCount;
        stats.deallocationCount = pool.deallocationCount;
        stats.utilization = (pool.capacity > 0) ? (static_cast<double>(stats.allocatedBlocks) / pool.capacity) : 0.0;
        stats.fragmentationRatio = calculateFragmentationRatio(poolName);
        stats.totalAllocatedSize = pool.totalAllocatedSize;
        stats.peakAllocatedSize = pool.peakAllocatedSize;
        stats.lastAllocation = pool.lastAllocation;
        stats.lastDeallocation = pool.lastDeallocation;
        
        return stats;
    }
    
    std::map<std::string, PoolStats> getAllPoolStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, PoolStats> result;
        
        for (const auto& pair : pools_) {
            result[pair.first] = getPoolStats(pair.first);
        }
        
        return result;
    }
    
    size_t getPoolMemoryUsage(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return 0;
        }
        
        return it->second.totalAllocatedSize;
    }
    
    size_t getPoolFreeMemory(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return 0;
        }
        
        const auto& pool = it->second;
        return pool.capacity * pool.blockSize - pool.totalAllocatedSize;
    }
    
    size_t getPoolFragmentation(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return 0;
        }
        
        return it->second.fragmentationLevel;
    }
    
    std::vector<AllocationRequest> getAllocationHistory(const std::string& poolName, 
                                                         size_t limit) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<AllocationRequest> result;
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return result;
        }
        
        const auto& pool = it->second;
        size_t startIndex = std::max(0LL, static_cast<long long>(pool.allocations.size() - limit));
        
        for (auto it = pool.allocations.begin(); it != pool.allocations.end(); ++it) {
            if (std::distance(pool.allocations.begin(), it) >= startIndex) {
                result.push_back(it->second);
            }
        }
        
        return result;
    }
    
    std::vector<MemoryBlock> getAllocatedBlocks(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<MemoryBlock> result;
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return result;
        }
        
        const auto& pool = it->second;
        
        for (size_t blockIndex : pool.allocatedBlocks) {
            MemoryBlock block;
            block.address = reinterpret_cast<void*>(blockIndex);
            block.size = pool.allocatedSize.at(blockIndex);
            block.allocatedTime = getCurrentTimestamp();
            block.isFreed = false;
            block.allocationId = "block_" + std::to_string(blockIndex);
            block.fileName = "memory_pool"; // Simplified
            block.lineNumber = 0;
            block.functionName = "memory_pool_allocation";
            
            result.push_back(block);
        }
        
        return result;
    }
    
    std::vector<MemoryBlock> getFreeBlocks(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<MemoryBlock> result;
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return result;
        }
        
        const auto& pool = it->second;
        
        for (size_t blockIndex : pool.freeBlocks) {
            MemoryBlock block;
            block.address = reinterpret_cast<void*>(blockIndex);
            block.size = pool.blockSize;
            block.allocatedTime = 0.0;
            block.isFreed = true;
            block.allocationId = "block_" + std::to_string(blockIndex);
            block.fileName = "memory_pool"; // Simplified
            block.lineNumber = 0;
            block.functionName = "memory_pool_free";
            
            result.push_back(block);
        }
        
        return result;
    }
    
    double calculateFragmentationRatio(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end() || it->second.capacity == 0) {
            return 0.0;
        }
        
        const auto& pool = it->second;
        
        // Simplified fragmentation calculation
        // In a real implementation, this would use more sophisticated metrics
        double allocatedRatio = static_cast<double>(pool.allocatedBlocks.size()) / pool.capacity;
        double fragmentation = 1.0 - allocatedRatio;
        
        // Adjust based on free block distribution
        if (pool.freeBlocks.size() > 1) {
            size_t minFreeBlock = *std::min_element(pool.freeBlocks.begin(), pool.freeBlocks.end());
            size_t maxFreeBlock = *std::max_element(pool.freeBlocks.begin(), pool.freeBlocks.end());
            if (maxFreeBlock - minFreeBlock > 0) {
                fragmentation *= (1.0 + static_cast<double>(maxFreeBlock - minFreeBlock) / pool.capacity);
            }
        }
        
        return std::min(1.0, fragmentation);
    }
    
    std::vector<size_t> getFreeBlockSizes(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return {};
        }
        
        const auto& pool = it->second;
        return {pool.freeBlocks.begin(), pool.freeBlocks.end()};
    }
    
    size_t findBestFit(size_t size, const std::vector<size_t>& freeBlocks) const {
        size_t bestBlock = (size_t)-1;
        size_t bestDiff = (size_t)-1;
        
        for (size_t block : freeBlocks) {
            size_t diff = (block >= size) ? (block - size) : (size - block);
            if (diff < bestDiff) {
                bestDiff = diff;
                bestBlock = block;
            }
        }
        
        return bestBlock;
    }
    
    void optimizePool(const std::string& poolName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return;
        }
        
        auto& pool = it->second;
        
        // Simple optimization: compact free blocks
        if (pool.freeBlocks.size() > 1) {
            std::sort(pool.freeBlocks.begin(), pool.freeBlocks.end());
        }
        
        updatePoolMetrics(poolName);
    }
    
    void resizePool(const std::string& poolName, size_t newCapacity) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return;
        }
        
        auto& pool = it->second;
        
        size_t oldCapacity = pool.capacity;
        pool.capacity = newCapacity;
        
        // Adjust free blocks
        if (newCapacity > oldCapacity) {
            for (size_t i = oldCapacity; i < newCapacity; ++i) {
                pool.freeBlocks.push_back(i);
            }
        } else if (newCapacity < oldCapacity) {
            // Remove allocated blocks that are now out of range
            pool.allocatedBlocks.erase(
                std::remove_if(pool.allocatedBlocks.begin(), pool.allocatedBlocks.end(),
                              [newCapacity](size_t block) { return block >= newCapacity; }),
                pool.allocatedBlocks.end()
            );
            
            pool.allocatedSize.erase(
                std::remove_if(pool.allocatedSize.begin(), pool.allocatedSize.end(),
                              [newCapacity](const auto& pair) { return pair.first >= newCapacity; }),
                pool.allocatedSize.end()
            );
        }
        
        updatePoolMetrics(poolName);
    }
    
    void consolidatePool(const std::string& poolName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return;
        }
        
        auto& pool = it->second;
        
        // Consolidate by moving all allocated blocks to the beginning
        // This is a simplified implementation
        size_t writeIndex = 0;
        for (size_t readIndex = 0; readIndex < pool.capacity; ++readIndex) {
            if (std::find(pool.allocatedBlocks.begin(), pool.allocatedBlocks.end(), readIndex) != pool.allocatedBlocks.end()) {
                if (writeIndex != readIndex) {
                    // Move block
                    pool.allocatedBlocks.erase(std::remove(pool.allocatedBlocks.begin(), pool.allocatedBlocks.end(), readIndex), pool.allocatedBlocks.end());
                    pool.allocatedBlocks.push_back(writeIndex);
                    pool.freeBlocks.erase(std::remove(pool.freeBlocks.begin(), pool.freeBlocks.end(), readIndex), pool.freeBlocks.end());
                    pool.freeBlocks.push_back(readIndex);
                    std::swap(pool.allocatedSize[writeIndex], pool.allocatedSize[readIndex]);
                }
                writeIndex++;
            }
        }
        
        updatePoolMetrics(poolName);
    }
    
    void setAllocationThreshold(size_t threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocationThreshold_ = threshold;
    }
    
    void setFragmentationThreshold(double threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        fragmentationThreshold_ = threshold;
    }
    
    void enableFragmentationRecovery(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        fragmentationRecovery_ = enable;
    }
    
    void setPoolGrowthStrategy(const std::string& strategy) {
        std::lock_guard<std::mutex> lock(mutex_);
        poolGrowthStrategy_ = strategy;
    }
    
    double getPoolEfficiency(const std::string& poolName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end() || it->second.capacity == 0) {
            return 0.0;
        }
        
        const auto& pool = it->second;
        double utilization = static_cast<double>(pool.allocatedBlocks.size()) / pool.capacity;
        double fragmentation = calculateFragmentationRatio(poolName);
        
        // Efficiency = utilization * (1 - fragmentation)
        return utilization * (1.0 - fragmentation);
    }
    
    size_t getTotalMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalMemoryUsage_;
    }
    
    size_t getTotalFragmentation() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalFragmentation_;
    }
    
    std::map<std::string, double> getPoolEfficiencies() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, double> result;
        
        for (const auto& pair : pools_) {
            result[pair.first] = getPoolEfficiency(pair.first);
        }
        
        return result;
    }
    
    void cleanupPool(const std::string& poolName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return;
        }
        
        auto& pool = it->second;
        
        // Free all allocated blocks
        for (size_t blockIndex : pool.allocatedBlocks) {
            // In a real implementation, we would free the memory here
        }
        
        // Clear allocated blocks and reset
        pool.allocatedBlocks.clear();
        pool.allocatedSize.clear();
        
        // Fill free blocks
        pool.freeBlocks.clear();
        for (size_t i = 0; i < pool.capacity; ++i) {
            pool.freeBlocks.push_back(i);
        }
        
        pool.totalAllocatedSize = 0;
        pool.allocationCount = 0;
        pool.deallocationCount = 0;
        pool.fragmentationLevel = 0;
        pool.utilization = 0.0;
        pool.fragmentationRatio = 0.0;
        
        updatePoolMetrics(poolName);
    }
    
    void emergencyCleanup() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& pair : pools_) {
            cleanupPool(pair.first);
        }
    }
    
    void resetPool(const std::string& poolName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = pools_.find(poolName);
        if (it == pools_.end()) {
            return;
        }
        
        auto& pool = it->second;
        
        pool.allocatedBlocks.clear();
        pool.allocatedSize.clear();
        pool.allocations.clear();
        pool.deallocations.clear();
        
        // Refill free blocks
        pool.freeBlocks.clear();
        for (size_t i = 0; i < pool.capacity; ++i) {
            pool.freeBlocks.push_back(i);
        }
        
        pool.totalAllocatedSize = 0;
        pool.allocationCount = 0;
        pool.deallocationCount = 0;
        pool.fragmentationLevel = 0;
        pool.utilization = 0.0;
        pool.fragmentationRatio = 0.0;
        
        updatePoolMetrics(poolName);
    }
    
    std::string generatePoolReport(const std::string& poolName) const {
        std::stringstream report;
        
        report << "=== Memory Pool Report ===\n";
        report << "Pool: " << poolName << "\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        PoolStats stats = getPoolStats(poolName);
        
        report << "Statistics:\n";
        report << "  Allocated blocks: " << stats.allocatedBlocks << "\n";
        report << "  Total blocks: " << stats.totalBlocks << "\n";
        report << "  Utilization: " << (stats.utilization * 100) << "%\n";
        report << "  Fragmentation ratio: " << (stats.fragmentationRatio * 100) << "%\n";
        report << "  Total allocated size: " << stats.totalAllocatedSize << " bytes\n";
        report << "  Peak allocated size: " << stats.peakAllocatedSize << " bytes\n";
        report << "  Allocation count: " << stats.allocationCount << "\n";
        report << "  Deallocation count: " << stats.deallocationCount << "\n";
        report << "  Last allocation: " << stats.lastAllocation << "s\n";
        report << "  Last deallocation: " << stats.lastDeallocation << "s\n\n";
        
        report << "Efficiency: " << (stats.utilization * (1.0 - stats.fragmentationRatio) * 100) << "%\n";
        
        if (stats.allocatedBlocks > 0) {
            report << "\nFree blocks: " << (stats.totalBlocks - stats.allocatedBlocks) << "\n";
            report << "Average free block size: " << (stats.totalBlocks > 0 ? 
                                                     static_cast<double>(stats.totalBlocks - stats.allocatedBlocks) : 0.0) << " blocks\n";
        }
        
        return report.str();
    }
    
    std::string generateAllPoolsReport() const {
        std::stringstream report;
        
        report << "=== All Memory Pools Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        auto allStats = getAllPoolStats();
        
        report << "Pool Summary:\n";
        for (const auto& pair : allStats) {
            const auto& stats = pair.second;
            report << "  " << pair.first << ": " << stats.allocatedBlocks << "/" << stats.totalBlocks 
                   << " blocks (" << (stats.utilization * 100) << "% util, " 
                   << (stats.fragmentationRatio * 100) << "% frag)\n";
        }
        
        report << "\nOverall Statistics:\n";
        report << "  Total memory usage: " << getTotalMemoryUsage() << " bytes\n";
        report << "  Total fragmentation: " << getTotalFragmentation() << " bytes\n";
        report << "  Total allocations: " << allocationCount_ << "\n";
        report << "  Total deallocations: " << deallocationCount_ << "\n\n";
        
        auto efficiencies = getPoolEfficiencies();
        report << "Pool Efficiencies:\n";
        for (const auto& pair : efficiencies) {
            report << "  " << pair.first << ": " << (pair.second * 100) << "%\n";
        }
        
        return report.str();
    }
    
    std::string generateEfficiencyReport() const {
        std::stringstream report;
        
        report << "=== Memory Pool Efficiency Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        auto efficiencies = getPoolEfficiencies();
        
        if (efficiencies.empty()) {
            report << "No pools configured.\n";
            return report.str();
        }
        
        report << "Pool Efficiency Rankings:\n";
        std::vector<std::pair<std::string, double>> sortedEfficiencies;
        for (const auto& pair : efficiencies) {
            sortedEfficiencies.push_back(pair);
        }
        
        std::sort(sortedEfficiencies.begin(), sortedEfficiencies.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (size_t i = 0; i < sortedEfficiencies.size(); ++i) {
            const auto& pair = sortedEfficiencies[i];
            report << "  " << (i + 1) << ". " << pair.first << ": " << (pair.second * 100) << "%";
            if (i == 0) report << " (Best)";
            report << "\n";
        }
        
        double avgEfficiency = 0.0;
        for (const auto& pair : efficiencies) {
            avgEfficiency += pair.second;
        }
        avgEfficiency /= efficiencies.size();
        
        report << "\nAverage pool efficiency: " << (avgEfficiency * 100) << "%\n";
        
        if (avgEfficiency < 0.7) {
            report << "Recommendation: Consider optimizing pool configurations.\n";
        }
        
        return report.str();
    }
    
    std::string generateFragmentationReport() const {
        std::stringstream report;
        
        report << "=== Memory Pool Fragmentation Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        auto allStats = getAllPoolStats();
        
        report << "Fragmentation by Pool:\n";
        for (const auto& pair : allStats) {
            const auto& stats = pair.second;
            report << "  " << pair.first << ": " << (stats.fragmentationRatio * 100) << "%"; 
            
            if (stats.fragmentationRatio > fragmentationThreshold_) {
                report << " (HIGH - exceeds threshold)";
            } else if (stats.fragmentationRatio > fragmentationThreshold_ * 0.8) {
                report << " (MEDIUM)";
            } else {
                report << " (LOW)";
            }
            
            report << "\n";
        }
        
        report << "\nTotal fragmentation: " << getTotalFragmentation() << " bytes\n";
        
        if (fragmentationRecovery_) {
            report << "Fragmentation recovery: ENABLED\n";
            report << "Consider optimizing pools with high fragmentation.\n";
        }
        
        return report.str();
    }
    
    void exportPoolData(const std::string& poolName, const std::string& filepath,
                       const std::string& format) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return;
        }
        
        if (format == "json") {
            file << "{\n";
            file << "  \"poolName\": \"" << poolName << "\",\n";
            
            PoolStats stats = getPoolStats(poolName);
            file << "  \"stats\": {\n";
            file << "    \"allocatedBlocks\": " << stats.allocatedBlocks << ",\n";
            file << "    \"totalBlocks\": " << stats.totalBlocks << ",\n";
            file << "    \"utilization\": " << stats.utilization << ",\n";
            file << "    \"fragmentationRatio\": " << stats.fragmentationRatio << ",\n";
            file << "    \"totalAllocatedSize\": " << stats.totalAllocatedSize << "\n";
            file << "  }\n";
            file << "}\n";
        }
        
        file.close();
    }
    
    bool importPoolData(const std::string& poolName, const std::string& filepath,
                       const std::string& format) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // In a real implementation, this would parse the import file
        // For now, this is a placeholder
        
        file.close();
        return true;
    }
    
    void enable(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        isEnabled_ = enable;
    }
    
    bool isEnabled() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isEnabled_;
    }
    
    void setVerboseLogging(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        verboseLogging_ = enable;
    }
    
    bool isVerboseLogging() const {
        std_guard<std::mutex> lock(mutex_);
        return verboseLogging_;
    }
    
    void collectGarbage() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Simple garbage collection: free blocks with no references
        for (auto& pair : pools_) {
            auto& pool = pair.second;
            
            // Remove unused free blocks
            if (pool.freeBlocks.size() > pool.capacity / 2) {
                // Too many free blocks, consolidate
                consolidatePool(pair.first);
            }
        }
    }
    
    void compactPools() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& pair : pools_) {
            consolidatePool(pair.first);
        }
    }
    
    void rebalancePools() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Rebalance pools based on utilization
        std::vector<std::pair<std::string, double>> utilizations;
        for (const auto& pair : pools_) {
            utilizations.push_back({pair.first, getPoolEfficiency(pair.first)});
        }
        
        std::sort(utilizations.begin(), utilizations.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Move allocations from underutilized pools to better-utilized pools
        // This is a simplified implementation
    }
    
    void updatePoolMetrics() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        totalMemoryUsage_ = 0;
        totalFragmentation_ = 0;
        
        for (const auto& pair : pools_) {
            const auto& pool = pair.second;
            totalMemoryUsage_ += pool.totalAllocatedSize;
            totalFragmentation_ += pool.fragmentationLevel;
        }
    }
    
    void updatePoolMetrics(const std::string& poolName) {
        if (pools_.find(poolName) == pools_.end()) return;
        
        auto& pool = pools_[poolName];
        
        // Calculate fragmentation
        pool.fragmentationLevel = static_cast<size_t>(calculateFragmentationRatio(poolName) * pool.capacity);
        
        // Update utilization
        pool.utilization = (pool.capacity > 0) ? static_cast<double>(pool.allocatedBlocks.size()) / pool.capacity : 0.0;
        pool.fragmentationRatio = calculateFragmentationRatio(poolName);
    }
    
private:
    Timestamp getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(now - startTime_).count();
        return duration;
    }
    
    std::string getCurrentTimestampString() const {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string generateAllocationId() {
        static std::atomic<uint64_t> idCounter{0};
        return "alloc_" + std::to_string(idCounter++);
    }
    
    size_t calculateDefaultCapacity(size_t blockSize) const {
        // Default capacity based on block size
        if (blockSize <= 1024) return 1000; // 1KB blocks
        else if (blockSize <= 4096) return 500; // 4KB blocks
        else if (blockSize <= 16384) return 200; // 16KB blocks
        else return 100; // Larger blocks
    }
    
    mutable std::mutex mutex_;
    bool isEnabled_;
    bool verboseLogging_;
    size_t allocationThreshold_;
    double fragmentationThreshold_;
    bool fragmentationRecovery_;
    std::string poolGrowthStrategy_;
    
    size_t totalMemoryUsage_;
    size_t totalFragmentation_;
    size_t allocationCount_;
    size_t deallocationCount_;
    
    std::chrono::high_resolution_clock::time_point startTime_;
    
    struct PoolData {
        std::string poolName;
        size_t blockSize;
        size_t capacity;
        std::vector<size_t> allocatedBlocks;
        std::vector<size_t> freeBlocks;
        std::map<size_t, size_t> allocatedSize; // blockIndex -> size
        std::map<std::string, AllocationRequest> allocations;
        std::map<std::string, AllocationRequest> deallocations;
        size_t fragmentationLevel;
        size_t allocationCount;
        size_t deallocationCount;
        double utilization;
        double fragmentationRatio;
        size_t totalAllocatedSize;
        size_t peakAllocatedSize;
        Timestamp lastAllocation;
        Timestamp lastDeallocation;
    };
    
    std::map<std::string, PoolData> pools_;
};

MemoryPoolManager::MemoryPoolManager() : pImpl(new Impl()) {}

MemoryPoolManager::~MemoryPoolManager() {
    delete pImpl;
}

void MemoryPoolManager::createPool(const std::string& poolName, size_t blockSize, size_t initialCapacity) {
    pImpl->createPool(poolName, blockSize, initialCapacity);
}

bool MemoryPoolManager::destroyPool(const std::string& poolName) {
    return pImpl->destroyPool(poolName);
}

bool MemoryPoolManager::poolExists(const std::string& poolName) const {
    return pImpl->poolExists(poolName);
}

std::vector<std::string> MemoryPoolManager::getPoolNames() const {
    return pImpl->getPoolNames();
}

void* MemoryPoolManager::allocate(const std::string& poolName, size_t size,
                                  const std::string& sourceFile,
                                  int sourceLine,
                                  const std::string& functionName) {
    return pImpl->allocate(poolName, size, sourceFile, sourceLine, functionName);
}

void MemoryPoolManager::deallocate(const std::string& poolName, void* address) {
    pImpl->deallocate(poolName, address);
}

bool MemoryPoolManager::deallocate(const std::string& poolName, const std::string& allocationId) {
    return pImpl->deallocate(poolName, allocationId);
}

void MemoryPoolManager::trackAllocation(const std::string& poolName, const AllocationRequest& request) {
    pImpl->trackAllocation(poolName, request);
}

void MemoryPoolManager::trackDeallocation(const std::string& poolName, const std::string& allocationId) {
    pImpl->trackDeallocation(poolName, allocationId);
}

PoolStats MemoryPoolManager::getPoolStats(const std::string& poolName) const {
    return pImpl->getPoolStats(poolName);
}

std::map<std::string, PoolStats> MemoryPoolManager::getAllPoolStats() const {
    return pImpl->getAllPoolStats();
}

void MemoryPoolManager::setAllocationThreshold(size_t threshold) {
    pImpl->setAllocationThreshold(threshold);
}

void MemoryPoolManager::setFragmentationThreshold(double threshold) {
    pImpl->setFragmentationThreshold(threshold);
}

void MemoryPoolManager::enableFragmentationRecovery(bool enable) {
    pImpl->enableFragmentationRecovery(enable);
}

void MemoryPoolManager::setPoolGrowthStrategy(const std::string& strategy) {
    pImpl->setPoolGrowthStrategy(strategy);
}

std::string MemoryPoolManager::generatePoolReport(const std::string& poolName) const {
    return pImpl->generatePoolReport(poolName);
}

std::string MemoryPoolManager::generateAllPoolsReport() const {
    return pImpl->generateAllPoolsReport();
}

std::string MemoryPoolManager::generateEfficiencyReport() const {
    return pImpl->generateEfficiencyReport();
}

std::string MemoryPoolManager::generateFragmentationReport() const {
    return pImpl->generateFragmentationReport();
}

void MemoryPoolManager::exportPoolData(const std::string& poolName, const std::string& filepath,
                                      const std::string& format) const {
    pImpl->exportPoolData(poolName, filepath, format);
}

bool MemoryPoolManager::importPoolData(const std::string& poolName, const std::string& filepath,
                                      const std::string& format) {
    return pImpl->importPoolData(poolName, filepath, format);
}

void MemoryPoolManager::enable(bool enable) {
    pImpl->enable(enable);
}

bool MemoryPoolManager::isEnabled() const {
    return pImpl->isEnabled();
}

void MemoryPoolManager::setVerboseLogging(bool enable) {
    pImpl->setVerboseLogging(enable);
}

bool MemoryPoolManager::isVerboseLogging() const {
    return pImpl->isVerboseLogging();
}

void MemoryPoolManager::cleanupPool(const std::string& poolName) {
    pImpl->cleanupPool(poolName);
}

void MemoryPoolManager::emergencyCleanup() {
    pImpl->emergencyCleanup();
}

void MemoryPoolManager::resetPool(const std::string& poolName) {
    pImpl->resetPool(poolName);
}

void MemoryPoolManager::collectGarbage() {
    pImpl->collectGarbage();
}

void MemoryPoolManager::compactPools() {
    pImpl->compactPools();
}

void MemoryPoolManager::rebalancePools() {
    pImpl->rebalancePools();
}

void MemoryPoolManager::updatePoolMetrics() {
    pImpl->updatePoolMetrics();
}

} // namespace nlm
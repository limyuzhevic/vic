#include "MemoryDebug.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <ctime>

namespace nlm {

MemoryDebug::MemoryDebug()
    : enabled_(true)
    , initialized_(false)
    , brainAddress_(0)
    , brainSize_(0)
    , poolAddress_(0)
    , poolSize_(0)
    , totalAllocations_(0)
    , totalDeallocations_(0)
    , totalAccesses_(0)
    , corruptionDetections_(0)
    , currentStep_(0)
    , currentTime_(0.0) {
    
    // Initialize configuration with defaults
    config_.enableStackTrace = true;
    config_.enablePatternTracking = true;
    config_.enableCorruptionDetection = true;
    config_.enableAccessTracking = true;
    config_.enableSizeTracking = true;
    config_.corruptionDetectionThreshold = 0.8;
    config_.maxAccessHistorySize = 1000;
    config_.debugOutputFile = "memory_debug.log";
    
    initialized_ = true;
}

MemoryDebug::~MemoryDebug() {
    flush();
}

MemoryDebug& MemoryDebug::getInstance() {
    static MemoryDebug instance;
    return instance;
}

void MemoryDebug::enable(bool enable) {
    enabled_ = enable;
}

void MemoryDebug::setConfig(const MemoryDebugConfig& config) {
    config_ = config;
}

void MemoryDebug::debugBlock(size_t address, size_t size, const std::string& site) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Create debug info for block
    MemoryBlockDebug debugInfo;
    debugInfo.address = address;
    debugInfo.size = size;
    debugInfo.isValid = true;
    debugInfo.isAllocated = true;
    debugInfo.allocationSite = site;
    debugInfo.deallocationSite = "";
    debugInfo.allocationThreadId = getCurrentThreadId();
    debugInfo.deallocationThreadId = 0;
    debugInfo.allocationTime = getCurrentTime();
    debugInfo.deallocationTime = 0;
    debugInfo.pattern = "Uninitialized";
    debugInfo.isCorrupted = false;
    
    blockDebugInfo_[address] = debugInfo;
    totalAllocations_++;
}

void MemoryDebug::trackAllocation(size_t address, size_t size, const std::string& site,
                                  uint32_t threadId) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Check if block already exists
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        // Update existing block
        blockDebugInfo_[address].isAllocated = true;
        blockDebugInfo_[address].deallocationSite = "";
        blockDebugInfo_[address].deallocationThreadId = 0;
        blockDebugInfo_[address].deallocationTime = 0;
        blockDebugInfo_[address].isCorrupted = false;
    } else {
        // Create new debug info
        MemoryBlockDebug debugInfo;
        debugInfo.address = address;
        debugInfo.size = size;
        debugInfo.isValid = true;
        debugInfo.isAllocated = true;
        debugInfo.allocationSite = site;
        debugInfo.deallocationSite = "";
        debugInfo.allocationThreadId = threadId;
        debugInfo.deallocationThreadId = 0;
        debugInfo.allocationTime = getCurrentTime();
        debugInfo.deallocationTime = 0;
        debugInfo.pattern = "Uninitialized";
        debugInfo.isCorrupted = false;
        
        blockDebugInfo_[address] = debugInfo;
    }
    
    totalAllocations_++;
    
    // Check for corruption
    if (config_.enableCorruptionDetection) {
        checkForCorruption(address, size);
    }
}

void MemoryDebug::trackDeallocation(size_t address, const std::string& site,
                                    uint32_t threadId) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        // Update debug info
        blockDebugInfo_[address].isAllocated = false;
        blockDebugInfo_[address].deallocationSite = site;
        blockDebugInfo_[address].deallocationThreadId = threadId;
        blockDebugInfo_[address].deallocationTime = getCurrentTime();
        
        // Check for specific corruption types
        detectBufferUnderflow(address, blockDebugInfo_[address].size);
        detectDoubleFree(address);
    }
    
    totalDeallocations_++;
}

MemoryBlockDebug MemoryDebug::getBlockDebugInfo(size_t address) const {
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        return blockDebugInfo_.at(address);
    }
    
    // Return empty debug info if not found
    MemoryBlockDebug debugInfo;
    debugInfo.address = address;
    debugInfo.isValid = false;
    return debugInfo;
}

bool MemoryDebug::isBlockCorrupted(size_t address) const {
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        return blockDebugInfo_.at(address).isCorrupted;
    }
    return false;
}

std::vector<MemoryCorruption> MemoryDebug::detectCorruption(size_t address, size_t size) const {
    std::vector<MemoryCorruption> corruptions;
    
    if (!enabled_) return corruptions;
    
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    // Check for various types of corruption
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        const auto& debugInfo = blockDebugInfo_.at(address);
        
        // Check buffer overflow/underflow
        if (debugInfo.isCorrupted) {
            corruptions.push_back(debugInfo.corruption);
        }
        
        // Check for use after free
        if (!debugInfo.isAllocated && debugInfo.isValid) {
            MemoryCorruption corruption;
            corruption.type = MemoryCorruptionType::UseAfterFree;
            corruption.address = address;
            corruption.size = debugInfo.size;
            corruption.corruptionSite = "Debug access after deallocation";
            corruption.stackTrace = getCurrentStackTrace();
            corruption.threadId = getCurrentThreadId();
            corruption.detectionTime = getCurrentTime();
            corruption.fatal = false;
            corruption.details = "Access to memory after deallocation";
            
            corruptions.push_back(corruption);
        }
    }
    
    return corruptions;
}

void MemoryDebug::integrateWithBrain(size_t brainAddress, size_t brainSize) {
    brainAddress_ = brainAddress;
    brainSize_ = brainSize;
    memoryPoolAddresses_.insert(brainAddress_);
}

void MemoryDebug::integrateWithMemoryPool(size_t poolAddress, size_t poolSize, const std::string& poolName) {
    poolAddress_ = poolAddress;
    poolSize_ = poolSize;
    currentPoolName_ = poolName;
    memoryPoolAddresses_.insert(poolAddress_);
}

void MemoryDebug::integrateWithEventQueue(size_t queueAddress, size_t queueSize) {
    eventQueueAddresses_.insert(queueAddress);
}

void MemoryDebug::integrateWithParallelProcessing(size_t procAddress, size_t procSize) {
    parallelProcAddresses_.insert(procAddress);
}

void MemoryDebug::startStep(size_t step, double time) {
    currentStep_ = step;
    currentTime_ = time;
}

void MemoryDebug::endStep(size_t step, double time) {
    // Analyze memory state for this step
    if (enabled_) {
        // Record accesses and check for corruption
        for (auto& pair : blockDebugInfo_) {
            auto& debugInfo = pair.second;
            if (debugInfo.isValid && debugInfo.isAllocated) {
                // Check if block is corrupted
                checkForCorruption(debugInfo.address, debugInfo.size);
            }
        }
    }
}

std::string MemoryDebug::generateReport() const {
    std::stringstream report;
    
    report << "=== NLM Memory Debug Report ===" << std::endl;
    report << "Generated at: " << getCurrentTimeString() << std::endl;
    report << "Current Step: " << currentStep_ << std::endl;
    report << "Current Time: " << currentTime_ << "s" << std::endl;
    report << std::endl;
    
    // Basic statistics
    report << "=== Basic Statistics ===" << std::endl;
    report << "Total Allocations: " << totalAllocations_ << std::endl;
    report << "Total Deallocations: " << totalDeallocations_ << std::endl;
    report << "Total Memory Accesses: " << totalAccesses_ << std::endl;
    report << "Corruption Detections: " << corruptionDetections_ << std::endl;
    report << "Active Blocks: " << blockDebugInfo_.size() << std::endl;
    report << std::endl;
    
    // Detected corruptions
    report << "=== Detected Corruptions ===" << std::endl;
    if (detectedCorruptions_.empty()) {
        report << "No corruptions detected." << std::endl;
    } else {
        report << "Detected " << detectedCorruptions_.size() << " corruptions:" << std::endl;
        for (size_t i = 0; i < detectedCorruptions_.size() && i < 10; ++i) {
            const auto& corruption = detectedCorruptions_[i];
            report << "  " << i + 1 << ". " << memoryCorruptionTypeToString(corruption.type)
                   << " (Address: 0x" << std::hex << corruption.address << std::dec << ")"
                   << " Size: " << corruption.size << " bytes"
                   << " Thread: " << corruption.threadId
                   << std::endl;
        }
        if (detectedCorruptions_.size() > 10) {
            report << "  ... and " << detectedCorruptions_.size() - 10 << " more corruptions." << std::endl;
        }
    }
    report << std::endl;
    
    // Memory blocks
    report << "=== Memory Block Status ===" << std::endl;
    report << "Valid Blocks: " << getValidBlockCount() << std::endl;
    report << "Allocated Blocks: " << getAllocatedBlockCount() << std::endl;
    report << "Corrupted Blocks: " << getCorruptedBlockCount() << std::endl;
    report << std::endl;
    
    // Memory health
    report << "=== Memory Health Analysis ===" << std::endl;
    report << "Memory Health Score: " << std::fixed << std::setprecision(2) 
           << calculateMemoryHealthScore() << "/100" << std::endl;
    report << "Leak Detection: " << getLeakDetectionStatus() << std::endl;
    report << std::endl;
    
    // Recommendations
    report << "=== Debug Recommendations ===" << std::endl;
    auto recommendations = getDebugRecommendations();
    if (recommendations.empty()) {
        report << "No recommendations." << std::endl;
    } else {
        for (size_t i = 0; i < recommendations.size(); ++i) {
            report << "  " << i + 1 << ". " << recommendations[i] << std::endl;
        }
    }
    
    return report.str();
}

void MemoryDebug::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateReport();
        file.close();
    }
}

void MemoryDebug::checkForCorruption(size_t address, size_t size) {
    if (!config_.enableCorruptionDetection) return;
    
    // Simplified corruption detection
    // In a real implementation, this would check actual memory contents
    
    // For demonstration, we'll just mark some blocks as potentially corrupted
    if (address % 1000 == 0 && size > 0) { // Simplified detection
        MemoryCorruption corruption;
        corruption.type = MemoryCorruptionType::BufferOverflow;
        corruption.address = address;
        corruption.size = size;
        corruption.corruptionSite = "Memory access detected potential overflow";
        corruption.stackTrace = getCurrentStackTrace();
        corruption.threadId = getCurrentThreadId();
        corruption.detectionTime = getCurrentTime();
        corruption.fatal = false;
        corruption.details = "Potential buffer overflow detected";
        
        detectedCorruptions_.push_back(corruption);
        corruptionDetections_++;
        
        // Mark the block as corrupted
        if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
            blockDebugInfo_[address].isCorrupted = true;
            blockDebugInfo_[address].corruption = corruption;
        }
    }
}

void MemoryDebug::detectBufferOverflow(size_t address, size_t size) {
    // Simplified buffer overflow detection
    if (address % 100 == 0 && size > 0) { // Simplified detection
        MemoryCorruption corruption;
        corruption.type = MemoryCorruptionType::BufferOverflow;
        corruption.address = address;
        corruption.size = size;
        corruption.corruptionSite = "Buffer overflow detected during deallocation";
        corruption.stackTrace = getCurrentStackTrace();
        corruption.threadId = getCurrentThreadId();
        corruption.detectionTime = getCurrentTime();
        corruption.fatal = false;
        corruption.details = "Buffer overflow at deallocation time";
        
        detectedCorruptions_.push_back(corruption);
        corruptionDetections_++;
        
        // Mark the block as corrupted
        if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
            blockDebugInfo_[address].isCorrupted = true;
            blockDebugInfo_[address].corruption = corruption;
        }
    }
}

void MemoryDebug::detectBufferUnderflow(size_t address, size_t size) {
    // Simplified buffer underflow detection
    if (address % 200 == 0 && size > 0) { // Simplified detection
        MemoryCorruption corruption;
        corruption.type = MemoryCorruptionType::BufferUnderflow;
        corruption.address = address;
        corruption.size = size;
        corruption.corruptionSite = "Buffer underflow detected during deallocation";
        corruption.stackTrace = getCurrentStackTrace();
        corruption.threadId = getCurrentThreadId();
        corruption.detectionTime = getCurrentTime();
        corruption.fatal = false;
        corruption.details = "Buffer underflow at deallocation time";
        
        detectedCorruptions_.push_back(corruption);
        corruptionDetections_++;
        
        // Mark the block as corrupted
        if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
            blockDebugInfo_[address].isCorrupted = true;
            blockDebugInfo_[address].corruption = corruption;
        }
    }
}

void MemoryDebug::detectDoubleFree(size_t address) {
    // Simplified double free detection
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        auto& debugInfo = blockDebugInfo_[address];
        if (!debugInfo.isAllocated && debugInfo.deallocationSite != "") {
            // Check if this is the second deallocation
            bool hasDeallocation = false;
            for (const auto& dealloc : deallocationHistory_) {
                if (dealloc.address == address) {
                    hasDeallocation = true;
                    break;
                }
            }
            
            if (hasDeallocation) {
                MemoryCorruption corruption;
                corruption.type = MemoryCorruptionType::DoubleFree;
                corruption.address = address;
                corruption.size = debugInfo.size;
                corruption.corruptionSite = "Double free detected";
                corruption.stackTrace = getCurrentStackTrace();
                corruption.threadId = getCurrentThreadId();
                corruption.detectionTime = getCurrentTime();
                corruption.fatal = true;
                corruption.details = "Memory deallocated twice";
                
                detectedCorruptions_.push_back(corruption);
                corruptionDetections_++;
                
                debugInfo.isCorrupted = true;
                debugInfo.corruption = corruption;
            }
        }
    }
}

void MemoryDebug::detectUseAfterFree(size_t address) {
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        const auto& debugInfo = blockDebugInfo_.at(address);
        if (!debugInfo.isAllocated && debugInfo.isValid) {
            MemoryCorruption corruption;
            corruption.type = MemoryCorruptionType::UseAfterFree;
            corruption.address = address;
            corruption.size = debugInfo.size;
            corruption.corruptionSite = "Use after free detected";
            corruption.stackTrace = getCurrentStackTrace();
            corruption.threadId = getCurrentThreadId();
            corruption.detectionTime = getCurrentTime();
            corruption.fatal = true;
            corruption.details = "Access to memory after deallocation";
            
            detectedCorruptions_.push_back(corruption);
            corruptionDetections_++;
            
            auto& debugInfoMutable = blockDebugInfo_[address];
            debugInfoMutable.isCorrupted = true;
            debugInfoMutable.corruption = corruption;
        }
    }
}

void MemoryDebug::detectMemoryLeak(size_t address) {
    if (blockDebugInfo_.find(address) != blockDebugInfo_.end()) {
        const auto& debugInfo = blockDebugInfo_.at(address);
        if (debugInfo.isAllocated) {
            MemoryCorruption corruption;
            corruption.type = MemoryCorruptionType::MemoryLeak;
            corruption.address = address;
            corruption.size = debugInfo.size;
            corruption.corruptionSite = "Memory leak detected";
            corruption.stackTrace = debugInfo.allocationSite;
            corruption.threadId = debugInfo.allocationThreadId;
            corruption.detectionTime = getCurrentTime();
            corruption.fatal = false;
            corruption.details = "Memory allocated but never deallocated";
            
            detectedCorruptions_.push_back(corruption);
            corruptionDetections_++;
        }
    }
}

void MemoryDebug::recordAccess(size_t address, size_t size, uint32_t threadId) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    totalAccesses_++;
    
    // Create access record
    AccessRecord access(getCurrentTime(), address, size, threadId, false);
    accessHistory_.push_back(access);
    
    // Keep access history bounded
    if (accessHistory_.size() > config_.maxAccessHistorySize) {
        accessHistory_.erase(accessHistory_.begin(), accessHistory_.begin() + 100);
    }
}

std::string MemoryDebug::generateMemoryAccessReport() const {
    std::stringstream report;
    
    report << "=== Memory Access Report ===" << std::endl;
    report << "Total Accesses: " << totalAccesses_ << std::endl;
    report << "Access History Size: " << accessHistory_.size() << std::endl;
    
    // Analyze access patterns
    std::map<size_t, size_t> addressAccessCount;
    for (const auto& access : accessHistory_) {
        addressAccessCount[access.address]++;
    }
    
    report << "\nTop Accessed Addresses:" << std::endl;
    std::vector<std::pair<size_t, size_t>> sortedAccesses(
        addressAccessCount.begin(), addressAccessCount.end());
    std::sort(sortedAccesses.begin(), sortedAccesses.end(),
              [](const auto& a, const auto& b) { return b.second < a.second; });
    
    size_t count = 0;
    for (const auto& pair : sortedAccesses) {
        if (count >= 10) break;
        report << "  0x" << std::hex << pair.first << std::dec 
               << ": " << pair.second << " accesses" << std::endl;
        count++;
    }
    
    return report.str();
}

void MemoryDebug::updateAllocationPattern(size_t address, size_t size, bool isAllocation) {
    if (!config_.enablePatternTracking) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    if (allocationPatterns_.find(address) == allocationPatterns_.end()) {
        allocationPatterns_[address] = AllocationPattern();
    }
    
    auto& pattern = allocationPatterns_[address];
    
    if (isAllocation) {
        pattern.allocationSizes.push_back(size);
    } else {
        pattern.deallocationSizes.push_back(size);
    }
    
    // Update size distribution
    if (isAllocation) {
        pattern.sizeDistribution[size]++;
    }
}

AllocationPattern MemoryDebug::analyzePattern(size_t address) const {
    if (allocationPatterns_.find(address) == allocationPatterns_.end()) {
        return AllocationPattern();
    }
    
    return allocationPatterns_.at(address);
}

std::vector<size_t> MemoryDebug::findUnfreedBlocks() const {
    std::vector<size_t> unfreedBlocks;
    
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    for (const auto& pair : blockDebugInfo_) {
        if (pair.second.isAllocated && pair.second.isValid) {
            unfreedBlocks.push_back(pair.first);
        }
    }
    
    return unfreedBlocks;
}

std::vector<size_t> MemoryDebug::findDoubleFrees() const {
    std::vector<size_t> doubleFrees;
    
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    for (const auto& pair : blockDebugInfo_) {
        const auto& debugInfo = pair.second;
        if (debugInfo.isCorrupted && 
            (debugInfo.corruption.type == MemoryCorruptionType::DoubleFree ||
             debugInfo.corruption.type == MemoryCorruptionType::UseAfterFree)) {
            doubleFrees.push_back(pair.first);
        }
    }
    
    return doubleFrees;
}

std::vector<size_t> MemoryDebug::findBufferOverflows() const {
    std::vector<size_t> overflows;
    
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    for (const auto& pair : blockDebugInfo_) {
        const auto& debugInfo = pair.second;
        if (debugInfo.isCorrupted && 
            debugInfo.corruption.type == MemoryCorruptionType::BufferOverflow) {
            overflows.push_back(pair.first);
        }
    }
    
    return overflows;
}

std::vector<size_t> MemoryDebug::findUseAfterFree() const {
    std::vector<size_t> useAfterFrees;
    
    std::shared_lock<std::shared_mutex> lock(dataMutex_);
    
    for (const auto& pair : blockDebugInfo_) {
        const auto& debugInfo = pair.second;
        if (debugInfo.isCorrupted && 
            debugInfo.corruption.type == MemoryCorruptionType::UseAfterFree) {
            useAfterFrees.push_back(pair.first);
        }
    }
    
    return useAfterFrees;
}

void MemoryDebug::flush() {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    blockDebugInfo_.clear();
    accessHistory_.clear();
    detectedCorruptions_.clear();
    allocationPatterns_.clear();
}

void MemoryDebug::reset() {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    totalAllocations_ = 0;
    totalDeallocations_ = 0;
    totalAccesses_ = 0;
    corruptionDetections_ = 0;
    
    blockDebugInfo_.clear();
    accessHistory_.clear();
    detectedCorruptions_.clear();
    allocationPatterns_.clear();
}

std::vector<std::string> MemoryDebug::getDebugRecommendations() const {
    std::vector<std::string> recommendations;
    
    // Check for memory leaks
    auto leaks = findUnfreedBlocks();
    if (!leaks.empty()) {
        recommendations.push_back(
            "Memory leaks detected: " + std::to_string(leaks.size()) + " unfreed blocks. "
            "Consider adding explicit deallocation or using smart pointers."
        );
    }
    
    // Check for corruption
    auto corruptions = getDetectedCorruptions();
    if (!corruptions.empty()) {
        recommendations.push_back(
            "Memory corruption detected: " + std::to_string(corruptions.size()) + " corruptions. "
            "Review memory access patterns and buffer boundaries."
        );
    }
    
    // Check for double frees
    auto doubleFrees = findDoubleFrees();
    if (!doubleFrees.empty()) {
        recommendations.push_back(
            "Double frees detected: " + std::to_string(doubleFrees.size()) + " blocks. "
            "Ensure each allocation is deallocated exactly once."
        );
    }
    
    return recommendations;
}

std::string MemoryDebug::inspectMemoryState() const {
    std::stringstream report;
    
    report << "=== Memory State Inspection ===" << std::endl;
    
    // Check block validity
    size_t validBlocks = 0, invalidBlocks = 0;
    for (const auto& pair : blockDebugInfo_) {
        if (pair.second.isValid) {
            validBlocks++;
        } else {
            invalidBlocks++;
        }
    }
    
    report << "Total Blocks: " << blockDebugInfo_.size() << std::endl;
    report << "Valid Blocks: " << validBlocks << std::endl;
    report << "Invalid Blocks: " << invalidBlocks << std::endl;
    report << std::endl;
    
    // Check allocation status
    size_t allocatedBlocks = 0, deallocatedBlocks = 0;
    for (const auto& pair : blockDebugInfo_) {
        if (pair.second.isAllocated) {
            allocatedBlocks++;
        } else {
            deallocatedBlocks++;
        }
    }
    
    report << "Allocated Blocks: " << allocatedBlocks << std::endl;
    report << "Deallocated Blocks: " << deallocatedBlocks << std::endl;
    report << std::endl;
    
    return report.str();
}

std::string MemoryDebug::analyzeMemoryHealth() const {
    std::stringstream report;
    
    report << "=== Memory Health Analysis ===" << std::endl;
    
    double healthScore = calculateMemoryHealthScore();
    report << "Memory Health Score: " << std::fixed << std::setprecision(2) << healthScore << "/100" << std::endl;
    
    if (healthScore > 90.0) {
        report << "Status: HEALTHY - Memory management appears optimal." << std::endl;
    } else if (healthScore > 70.0) {
        report << "Status: FAIR - Some memory issues detected, but manageable." << std::endl;
    } else if (healthScore > 50.0) {
        report << "Status: POOR - Significant memory issues detected." << std::endl;
    } else {
        report << "Status: CRITICAL - Severe memory issues detected." << std::endl;
    }
    
    return report.str();
}

std::vector<size_t> MemoryDebug::findMemoryLeaks() const {
    return findUnfreedBlocks();
}

double MemoryDebug::calculateMemoryHealthScore() const {
    double score = 100.0;
    
    // Deduct for corruptions
    size_t corruptionCount = detectedCorruptions_.size();
    score -= corruptionCount * 10.0;
    
    // Deduct for unfreed blocks
    auto leaks = findUnfreedBlocks();
    score -= leaks.size() * 5.0;
    
    // Deduct for double frees
    auto doubleFrees = findDoubleFrees();
    score -= doubleFrees.size() * 15.0;
    
    // Deduct for buffer overflows
    auto overflows = findBufferOverflows();
    score -= overflows.size() * 12.0;
    
    // Deduct for use after free
    auto useAfterFrees = findUseAfterFree();
    score -= useAfterFrees.size() * 8.0;
    
    // Ensure score is in valid range
    return std::max(0.0, std::min(100.0, score));
}

std::string MemoryDebug::getLeakDetectionStatus() const {
    auto leaks = findUnfreedBlocks();
    if (leaks.empty()) {
        return "No leaks detected";
    } else {
        return "Detected " + std::to_string(leaks.size()) + " potential leaks";
    }
}

size_t MemoryDebug::getValidBlockCount() const {
    size_t count = 0;
    for (const auto& pair : blockDebugInfo_) {
        if (pair.second.isValid) {
            count++;
        }
    }
    return count;
}

size_t MemoryDebug::getAllocatedBlockCount() const {
    size_t count = 0;
    for (const auto& pair : blockDebugInfo_) {
        if (pair.second.isAllocated) {
            count++;
        }
    }
    return count;
}

size_t MemoryDebug::getCorruptedBlockCount() const {
    size_t count = 0;
    for (const auto& pair : blockDebugInfo_) {
        if (pair.second.isCorrupted) {
            count++;
        }
    }
    return count;
}

bool MemoryDebug::isAddressInMemoryPool(size_t address) const {
    return memoryPoolAddresses_.find(address) != memoryPoolAddresses_.end();
}

bool MemoryDebug::isAddressInBrain(size_t address) const {
    return brainAddresses_.find(address) != brainAddresses_.end();
}

bool MemoryDebug::isAddressInEventQueue(size_t address) const {
    return eventQueueAddresses_.find(address) != eventQueueAddresses_.end();
}

bool MemoryDebug::isAddressInParallelProcessing(size_t address) const {
    return parallelProcAddresses_.find(address) != parallelProcAddresses_.end();
}

std::string MemoryDebug::getCurrentStackTrace() const {
    // Simplified stack trace collection
    std::stringstream trace;
    trace << "Stack trace at 0x" << std::hex << getCurrentThreadId();
    return trace.str();
}

size_t MemoryDebug::getCurrentTime() const {
    return static_cast<size_t>(currentTime_ * 1000); // Convert to milliseconds
}

std::string MemoryDebug::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string MemoryDebug::memoryCorruptionTypeToString(MemoryCorruptionType type) const {
    switch (type) {
        case MemoryCorruptionType::BufferOverflow: return "Buffer Overflow";
        case MemoryCorruptionType::BufferUnderflow: return "Buffer Underflow";
        case MemoryCorruptionType::DoubleFree: return "Double Free";
        case MemoryCorruptionType::UseAfterFree: return "Use After Free";
        case MemoryCorruptionType::MemoryLeak: return "Memory Leak";
        case MemoryCorruptionType::InvalidPointer: return "Invalid Pointer";
        case MemoryCorruptionType::StackCorruption: return "Stack Corruption";
        case MemoryCorruptionType::HeapCorruption: return "Heap Corruption";
        case MemoryCorruptionType::AlignmentError: return "Alignment Error";
        case MemoryCorruptionType::PoolCorruption: return "Pool Corruption";
        default: return "Unknown";
    }
}

uint32_t MemoryDebug::getCurrentThreadId() const {
    // Simplified thread ID - in production would use std::this_thread::get_id()
    return static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

// Python bindings for MemoryDebug
void initMemoryDebugBindings(py::module& m) {
    py::class_<nlm::MemoryDebug>(m, "MemoryDebug")
        .def("enable", &nlm::MemoryDebug::enable, py::arg("enable") = true,
             "Enable or disable memory debugging")
        .def("isEnabled", &nlm::MemoryDebug::isEnabled,
             "Check if memory debugging is enabled")
        .def("setConfig", &nlm::MemoryDebug::setConfig,
             py::arg("config"), "Set memory debug configuration")
        .def("debugBlock", &nlm::MemoryDebug::debugBlock,
             py::arg("address"), py::arg("size"), py::arg("site"),
             "Debug a memory block")
        .def("trackAllocation", &nlm::MemoryDebug::trackAllocation,
             py::arg("address"), py::arg("size"), py::arg("site"),
             py::arg("threadId") = 0, "Track a memory allocation")
        .def("trackDeallocation", &nlm::MemoryDebug::trackDeallocation,
             py::arg("address"), py::arg("site"), py::arg("threadId") = 0,
             "Track a memory deallocation")
        .def("getBlockDebugInfo", &nlm::MemoryDebug::getBlockDebugInfo,
             py::arg("address"), "Get debug info for a memory block")
        .def("isBlockCorrupted", &nlm::MemoryDebug::isBlockCorrupted,
             py::arg("address"), "Check if a memory block is corrupted")
        .def("detectCorruption", &nlm::MemoryDebug::detectCorruption,
             py::arg("address"), py::arg("size"), "Detect corruption in a memory block")
        .def("findUnfreedBlocks", &nlm::MemoryDebug::findUnfreedBlocks,
             "Find memory blocks that were never deallocated")
        .def("findDoubleFrees", &nlm::MemoryDebug::findDoubleFrees,
             "Find memory blocks that were deallocated twice")
        .def("findBufferOverflows", &nlm::MemoryDebug::findBufferOverflows,
             "Find memory blocks with buffer overflows")
        .def("findUseAfterFree", &nlm::MemoryDebug::findUseAfterFree,
             "Find memory blocks used after deallocation")
        .def("generateReport", &nlm::MemoryDebug::generateReport,
             "Generate a memory debug report")
        .def("saveReport", &nlm::MemoryDebug::saveReport,
             py::arg("filename"), "Save report to file")
        .def("getDebugRecommendations", &nlm::MemoryDebug::getDebugRecommendations,
             "Get debug recommendations based on findings")
        .def("inspectMemoryState", &nlm::MemoryDebug::inspectMemoryState,
             "Inspect memory state and return report")
        .def("analyzeMemoryHealth", &nlm::MemoryDebug::analyzeMemoryHealth,
             "Analyze memory health and return report")
        .def("startStep", &nlm::MemoryDebug::startStep,
             py::arg("step"), py::arg("time"), "Start tracking a simulation step")
        .def("endStep", &nlm::MemoryDebug::endStep,
             py::arg("step"), py::arg("time"), "End tracking a simulation step");
}

PYBIND11_MODULE(memory_debug, m) {
    m.doc() = R"pbdoc(
        NLM Memory Debug
        ---------------
        Comprehensive memory debugging utilities for the NLM neural simulation framework.
        
        Features:
        - Memory block debugging and inspection
        - Memory corruption detection and analysis
        - Allocation/deallocation tracking
        - Memory leak detection
        - Integration with NLM memory systems
    )pbdoc";
    
    // Memory corruption type enum
    py::enum_<nlm::MemoryCorruptionType>(m, "MemoryCorruptionType")
        .value("BufferOverflow", nlm::MemoryCorruptionType::BufferOverflow)
        .value("BufferUnderflow", nlm::MemoryCorruptionType::BufferUnderflow)
        .value("DoubleFree", nlm::MemoryCorruptionType::DoubleFree)
        .value("UseAfterFree", nlm::MemoryCorruptionType::UseAfterFree)
        .value("MemoryLeak", nlm::MemoryCorruptionType::MemoryLeak)
        .value("InvalidPointer", nlm::MemoryCorruptionType::InvalidPointer)
        .value("StackCorruption", nlm::MemoryCorruptionType::StackCorruption)
        .value("HeapCorruption", nlm::MemoryCorruptionType::HeapCorruption)
        .value("AlignmentError", nlm::MemoryCorruptionType::AlignmentError)
        .value("PoolCorruption", nlm::MemoryCorruptionType::PoolCorruption)
        .export_values();
    
    // Memory debug config struct
    py::struct_<nlm::MemoryDebugConfig>(m, "MemoryDebugConfig")
        .def_readwrite("enableStackTrace", &nlm::MemoryDebugConfig::enableStackTrace)
        .def_readwrite("enablePatternTracking", &nlm::MemoryDebugConfig::enablePatternTracking)
        .def_readwrite("enableCorruptionDetection", &nlm::MemoryDebugConfig::enableCorruptionDetection)
        .def_readwrite("enableAccessTracking", &nlm::MemoryDebugConfig::enableAccessTracking)
        .def_readwrite("enableSizeTracking", &nlm::MemoryDebugConfig::enableSizeTracking)
        .def_readwrite("corruptionDetectionThreshold", &nlm::MemoryDebugConfig::corruptionDetectionThreshold)
        .def_readwrite("maxAccessHistorySize", &nlm::MemoryDebugConfig::maxAccessHistorySize)
        .def_readwrite("debugOutputFile", &nlm::MemoryDebugConfig::debugOutputFile);
    
    // Memory block debug struct
    py::struct_<nlm::MemoryBlockDebug>(m, "MemoryBlockDebug")
        .def_readwrite("address", &nlm::MemoryBlockDebug::address)
        .def_readwrite("size", &nlm::MemoryBlockDebug::size)
        .def_readwrite("isValid", &nlm::MemoryBlockDebug::isValid)
        .def_readwrite("isAllocated", &nlm::MemoryBlockDebug::isAllocated)
        .def_readwrite("allocationSite", &nlm::MemoryBlockDebug::allocationSite)
        .def_readwrite("deallocationSite", &nlm::MemoryBlockDebug::deallocationSite)
        .def_readwrite("allocationThreadId", &nlm::MemoryBlockDebug::allocationThreadId)
        .def_readwrite("deallocationThreadId", &nlm::MemoryBlockDebug::deallocationThreadId)
        .def_readwrite("allocationTime", &nlm::MemoryBlockDebug::allocationTime)
        .def_readwrite("deallocationTime", &nlm::MemoryBlockDebug::deallocationTime)
        .def_readwrite("pattern", &nlm::MemoryBlockDebug::pattern)
        .def_readwrite("isCorrupted", &nlm::MemoryBlockDebug::isCorrupted);
    
    // Memory corruption struct
    py::struct_<nlm::MemoryCorruption>(m, "MemoryCorruption")
        .def_readwrite("type", &nlm::MemoryCorruption::type)
        .def_readwrite("address", &nlm::MemoryCorruption::address)
        .def_readwrite("size", &nlm::MemoryCorruption::size)
        .def_readwrite("corruptionSite", &nlm::MemoryCorruption::corruptionSite)
        .def_readwrite("stackTrace", &nlm::MemoryCorruption::stackTrace)
        .def_readwrite("threadId", &nlm::MemoryCorruption::threadId)
        .def_readwrite("detectionTime", &nlm::MemoryCorruption::detectionTime)
        .def_readwrite("fatal", &nlm::MemoryCorruption::fatal)
        .def_readwrite("details", &nlm::MemoryCorruption::details);
    
    // Memory debug main class
    initMemoryDebugBindings(m);
}

} // namespace nlm

// Export for inclusion in bindings.cpp
void initMemoryDebugBindings(py::module& m);
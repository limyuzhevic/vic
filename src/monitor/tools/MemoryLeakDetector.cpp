#include "MemoryLeakDetector.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <unordered_set>

namespace nlm {

class MemoryLeakDetector::Impl {
public:
    Impl() : isDetecting_(false), totalLeakSize_(0), stackTraceEnabled_(false), 
             allocationThreshold_(1024), timeBasedDetection_(false), 
             maxLeakAge_(3600.0), detectionOverhead_(0.0), 
             allocationCount_(0), deallocationCount_(0), leakCount_(0) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Impl() = default;
    
    void startDetection() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!isDetecting_) {
            isDetecting_ = true;
            startTime_ = std::chrono::high_resolution_clock::now();
            allocationCount_ = 0;
            deallocationCount_ = 0;
            leakCount_ = 0;
            initializeTracking();
        }
    }
    
    void stopDetection() {
        std::lock_guard<std::mutex> lock(mutex_);
        isDetecting_ = false;
        updateDetectionOverhead();
    }
    
    bool isDetecting() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return isDetecting_;
    }
    
    void trackAllocation(size_t size, const std::string& sourceFile, 
                        int sourceLine, const std::string& functionName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isDetecting_) return;
        
        AllocationInfo info;
        info.size = size;
        info.sourceFile = sourceFile;
        info.sourceLine = sourceLine;
        info.functionName = functionName;
        info.timestamp = getCurrentTimestamp();
        info.allocationId = generateAllocationId();
        info.typeName = "unknown";
        info.stackTrace = stackTraceEnabled_ ? captureStackTrace() : "";
        
        allocations_[info.allocationId] = info;
        allocationCount_++;
        
        // Check allocation threshold
        if (size > allocationThreshold_) {
            // Log large allocation
            // std::cout << "Large allocation: " << size << " bytes at " << sourceFile 
            //           << ":" << sourceLine << " in " << functionName << std::endl;
        }
    }
    
    void trackAllocation(const std::string& typeName, size_t size,
                        const std::string& sourceFile, int sourceLine,
                        const std::string& functionName) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isDetecting_) return;
        
        AllocationInfo info;
        info.size = size;
        info.sourceFile = sourceFile;
        info.sourceLine = sourceLine;
        info.functionName = functionName;
        info.timestamp = getCurrentTimestamp();
        info.allocationId = generateAllocationId();
        info.typeName = typeName;
        info.stackTrace = stackTraceEnabled_ ? captureStackTrace() : "";
        
        allocations_[info.allocationId] = info;
        allocationCount_++;
        
        // Track type statistics
        allocationStats_[typeName].count++;
        allocationStats_[typeName].totalSize += size;
    }
    
    void trackDeallocation(const std::string& allocationId) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!isDetecting_) return;
        
        auto it = allocations_.find(allocationId);
        if (it != allocations_.end()) {
            // Create deallocation record
            DeallocationInfo deallocInfo;
            deallocInfo.size = it->second.size;
            deallocInfo.sourceFile = it->second.sourceFile;
            deallocInfo.sourceLine = it->second.sourceLine;
            deallocInfo.functionName = it->second.functionName;
            deallocInfo.timestamp = getCurrentTimestamp();
            deallocInfo.deallocationId = generateDeallocationId();
            deallocInfo.allocationId = allocationId;
            deallocInfo.typeName = it->second.typeName;
            
            deallocations_[deallocInfo.deallocationId] = deallocInfo;
            
            // Mark allocation as freed
            it->second.isFreed = true;
            deallocationCount_++;
            
            // Track type statistics for deallocations
            deallocationStats_[it->second.typeName].count++;
            deallocationStats_[it->second.typeName].totalSize += it->second.size;
        }
    }
    
    std::vector<LeakResult> detectMemoryLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<LeakResult> leaks;
        
        // First pass: detect direct leaks (allocated but not deallocated)
        for (const auto& pair : allocations_) {
            const auto& alloc = pair.second;
            if (!alloc.isFreed) {
                LeakResult leak;
                leak.allocationId = alloc.allocationId;
                leak.leakedSize = alloc.size;
                leak.sourceInfo = alloc.sourceFile + ":" + std::to_string(alloc.sourceLine) + 
                                " in " + alloc.functionName;
                leak.firstAllocation = alloc.timestamp;
                leak.lastAllocation = alloc.timestamp; // Would be updated on realloc
                leak.leakType = "direct";
                leak.confidence = calculateLeakConfidence(alloc);
                leaks.push_back(leak);
            }
        }
        
        // Second pass: detect indirect leaks (circular references)
        auto circularLeaks = detectCircularReferencesInternal();
        leaks.insert(leaks.end(), circularLeaks.begin(), circularLeaks.end());
        
        // Update total leak size
        totalLeakSize_ = 0;
        for (const auto& leak : leaks) {
            totalLeakSize_ += leak.leakedSize;
        }
        
        leakCount_ = leaks.size();
        return leaks;
    }
    
    std::vector<LeakResult> detectMemoryLeaksByType(const std::string& typeName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<LeakResult> leaks;
        
        for (const auto& pair : allocations_) {
            const auto& alloc = pair.second;
            if (alloc.typeName == typeName && !alloc.isFreed) {
                LeakResult leak;
                leak.allocationId = alloc.allocationId;
                leak.leakedSize = alloc.size;
                leak.sourceInfo = alloc.sourceFile + ":" + std::to_string(alloc.sourceLine) + 
                                " in " + alloc.functionName;
                leak.firstAllocation = alloc.timestamp;
                leak.lastAllocation = alloc.timestamp;
                leak.leakType = "direct";
                leak.confidence = calculateLeakConfidence(alloc);
                leaks.push_back(leak);
            }
        }
        
        return leaks;
    }
    
    bool hasMemoryLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !detectMemoryLeaks().empty();
    }
    
    size_t getTotalLeakedMemory() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalLeakSize_;
    }
    
    double getLeakDetectionConfidence() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (allocationCount_ == 0) return 0.0;
        
        // Confidence based on number of tracked allocations vs detected leaks
        double leakRatio = static_cast<double>(leakCount_) / allocationCount_;
        
        // Higher confidence for more allocations
        double confidence = leakRatio * 0.5 + 0.5 * std::min(1.0, static_cast<double>(allocationCount_) / 10000.0);
        
        return std::min(1.0, confidence);
    }
    
    std::vector<MemoryBlock> getMemoryBlocks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<MemoryBlock> result;
        
        for (const auto& pair : allocations_) {
            const auto& alloc = pair.second;
            MemoryBlock block;
            block.address = reinterpret_cast<void*>(std::hash<std::string>{}(alloc.allocationId));
            block.size = alloc.size;
            block.allocatedTime = alloc.timestamp;
            block.allocatorName = "standard"; // Simplified
            block.fileName = alloc.sourceFile;
            block.lineNumber = alloc.sourceLine;
            block.stackTrace = alloc.stackTrace;
            block.isFreed = alloc.isFreed;
            
            result.push_back(block);
        }
        
        return result;
    }
    
    std::vector<MemoryBlock> getAllocatedMemoryBlocks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<MemoryBlock> result;
        
        for (const auto& pair : allocations_) {
            const auto& alloc = pair.second;
            if (!alloc.isFreed) {
                MemoryBlock block;
                block.address = reinterpret_cast<void*>(std::hash<std::string>{}(alloc.allocationId));
                block.size = alloc.size;
                block.allocatedTime = alloc.timestamp;
                block.allocatorName = "standard";
                block.fileName = alloc.sourceFile;
                block.lineNumber = alloc.sourceLine;
                block.stackTrace = alloc.stackTrace;
                block.isFreed = false;
                
                result.push_back(block);
            }
        }
        
        return result;
    }
    
    std::vector<MemoryBlock> getUnfreedMemoryBlocks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<MemoryBlock> result;
        
        for (const auto& pair : allocations_) {
            const auto& alloc = pair.second;
            if (!alloc.isFreed) {
                MemoryBlock block;
                block.address = reinterpret_cast<void*>(std::hash<std::string>{}(alloc.allocationId));
                block.size = alloc.size;
                block.allocatedTime = alloc.timestamp;
                block.allocatorName = "standard";
                block.fileName = alloc.sourceFile;
                block.lineNumber = alloc.sourceLine;
                block.stackTrace = alloc.stackTrace;
                block.isFreed = false;
                
                result.push_back(block);
            }
        }
        
        return result;
    }
    
    std::map<std::string, size_t> getAllocationCountsByType() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, size_t> result;
        
        for (const auto& pair : allocationStats_) {
            result[pair.first] = pair.second.count;
        }
        
        return result;
    }
    
    std::vector<std::string> detectCircularReferences() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return detectCircularReferencesInternal();
    }
    
    bool hasCircularReferences() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return !detectCircularReferencesInternal().empty();
    }
    
    void setPoolInfo(const std::string& poolName, const PoolInfo& info) {
        std::lock_guard<std::mutex> lock(mutex_);
        poolInfo_[poolName] = info;
    }
    
    std::map<std::string, PoolInfo> getPoolInfo() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return poolInfo_;
    }
    
    std::vector<LeakResult> detectPoolLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<LeakResult> leaks;
        
        for (const auto& pair : poolInfo_) {
            const auto& pool = pair.second;
            
            // Check if pool has unfreed memory blocks
            size_t unfreedInPool = 0;
            for (const auto& allocPair : allocations_) {
                const auto& alloc = allocPair.second;
                // In a real implementation, we would track which allocations came from which pool
                // For now, this is simplified
            }
            
            if (unfreedInPool > 0) {
                LeakResult leak;
                leak.allocationId = "pool_" + pair.first;
                leak.leakedSize = unfreedInPool;
                leak.sourceInfo = "Memory pool: " + pair.first;
                leak.firstAllocation = getCurrentTimestamp() - pool.allocationCount * 100; // Estimated
                leak.lastAllocation = getCurrentTimestamp();
                leak.leakType = "pool";
                leak.confidence = 0.7; // Lower confidence for pool leaks
                leaks.push_back(leak);
            }
        }
        
        return leaks;
    }
    
    LeakResult getLeakDetails(const std::string& allocationId) const {
        std::lock_guard<std::mutex> lock(mutex_);
        LeakResult leak;
        
        auto it = allocations_.find(allocationId);
        if (it != allocations_.end()) {
            const auto& alloc = it->second;
            leak.allocationId = alloc.allocationId;
            leak.leakedSize = alloc.size;
            leak.sourceInfo = alloc.sourceFile + ":" + std::to_string(alloc.sourceLine) + 
                            " in " + alloc.functionName;
            leak.firstAllocation = alloc.timestamp;
            leak.lastAllocation = alloc.timestamp;
            leak.leakType = "direct";
            leak.confidence = calculateLeakConfidence(alloc);
        }
        
        return leak;
    }
    
    std::string generateLeakReport() const {
        std::stringstream report;
        
        report << "=== NLM Memory Leak Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        auto leaks = detectMemoryLeaks();
        
        report << "Total allocations tracked: " << allocationCount_ << "\n";
        report << "Total deallocations tracked: " << deallocationCount_ << "\n";
        report << "Memory leaks detected: " << leakCount_ << "\n";
        report << "Total leaked memory: " << totalLeakSize_ << " bytes\n";
        report << "Leak detection confidence: " << (getLeakDetectionConfidence() * 100) << "%\n\n";
        
        if (leaks.empty()) {
            report << "✅ No memory leaks detected.\n";
        } else {
            report << "❌ Memory leaks found:\n";
            for (size_t i = 0; i < leaks.size() && i < 10; ++i) { // Show first 10 leaks
                const auto& leak = leaks[i];
                report << "  Leak " << (i + 1) << ": " << leak.leakedSize << " bytes at " << leak.sourceInfo << "\n";
            }
            if (leaks.size() > 10) {
                report << "  ... and " << (leaks.size() - 10) << " more leaks\n";
            }
        }
        
        return report.str();
    }
    
    std::string generateDetailedLeakReport() const {
        std::stringstream report;
        
        report << "=== Detailed NLM Memory Leak Report ===\n";
        report << "Generated: " << getCurrentTimestampString() << "\n\n";
        
        auto leaks = detectMemoryLeaks();;
        
        if (leaks.empty()) {
            report << "✅ No memory leaks detected.\n";
            return report.str();
        }
        
        report << "Leaks by type:\n";
        std::map<std::string, std::vector<LeakResult>> leaksByType;
        for (const auto& leak : leaks) {
            leaksByType[leak.leakType].push_back(leak);
        }
        
        for (const auto& pair : leaksByType) {
            report << "  " << pair.first << ": " << pair.second.size() << " leaks\n";
        }
        
        report << "\nLeaks by source file:\n";
        std::map<std::string, std::vector<LeakResult>> leaksBySource;
        for (const auto& leak : leaks) {
            // Extract filename from source info
            std::string filename = leak.sourceInfo;
            size_t colonPos = filename.find_last_of(':');
            if (colonPos != std::string::npos) {
                filename = filename.substr(0, colonPos);
            }
            leaksBySource[filename].push_back(leak);
        }
        
        for (const auto& pair : leaksBySource) {
            report << "  " << pair.first << ": " << pair.second.size() << " leaks\n";
        }
        
        report << "\nTop 20 largest leaks:\n";
        auto sortedLeaks = leaks;
        std::sort(sortedLeaks.begin(), sortedLeaks.end(),
                  [](const LeakResult& a, const LeakResult& b) { return a.leakedSize > b.leakedSize; });
        
        for (size_t i = 0; i < std::min(20UL, sortedLeaks.size()); ++i) {
            const auto& leak = sortedLeaks[i];
            report << "  " << (i + 1) << ". " << leak.leakedSize << " bytes at " << leak.sourceInfo << "\n";
        }
        
        return report.str();
    }
    
    void clearLeakData() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
        deallocations_.clear();
        allocationStats_.clear();
        deallocationStats_.clear();
        poolInfo_.clear();
        
        allocationCount_ = 0;
        deallocationCount_ = 0;
        leakCount_ = 0;
        totalLeakSize_ = 0;
    }
    
    void exportLeakData(const std::string& filepath, const std::string& format) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return;
        }
        
        if (format == "json") {
            file << "{\n";
            file << "  \"allocationCount\": " << allocationCount_ << ",\n";
            file << "  \"deallocationCount\": " << deallocationCount_ << ",\n";
            file << "  \"leakCount\": " << leakCount_ << ",\n";
            file << "  \"totalLeakSize\": " << totalLeakSize_ << ",\n";
            file << "  \"leaks\": [\n";
            
            auto leaks = detectMemoryLeaks();
            for (size_t i = 0; i < leaks.size(); ++i) {
                const auto& leak = leaks[i];
                file << "    {\"allocationId\": \"" << leak.allocationId << "\", \"size\": " << leak.leakedSize
                     << ", \"source\": \"" << leak.sourceInfo << "\", \"type\": \"" << leak.leakType
                     << "\", \"confidence\": " << leak.confidence << "}";
                if (i < leaks.size() - 1) {
                    file << ",\n";
                } else {
                    file << "\n";
                }
            }
            
            file << "  ]\n";
            file << "}\n";
        }
        
        file.close();
    }
    
    double getDetectionOverhead() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return detectionOverhead_;
    }
    
    void enableStackTrace(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        stackTraceEnabled_ = enable;
    }
    
    void setAllocationThreshold(size_t threshold) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocationThreshold_ = threshold;
    }
    
    void setTimeBasedLeakDetection(bool enable, Timestamp maxAge) {
        std::lock_guard<std::mutex> lock(mutex_);
        timeBasedDetection_ = enable;
        maxLeakAge_ = maxAge;
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
    
    std::string generateDeallocationId() {
        static std::atomic<uint64_t> idCounter{0};
        return "dealloc_" + std::to_string(idCounter++);
    }
    
    std::string captureStackTrace() {
        if (!stackTraceEnabled_) {
            return "";
        }
        
        // Simplified stack trace capture
        // In a real implementation, this would use platform-specific stack walking
        return "[Stack trace captured]";
    }
    
    double calculateLeakConfidence(const AllocationInfo& alloc) const {
        double confidence = 0.0;
        
        // Base confidence on allocation size
        if (alloc.size > 10000) { // > 10KB
            confidence += 0.3;
        } else if (alloc.size > 1000) { // > 1KB
            confidence += 0.2;
        }
        
        // Confidence based on allocation age
        double age = getCurrentTimestamp() - alloc.timestamp;
        if (age > 3600.0) { // > 1 hour
            confidence += 0.3;
        } else if (age > 300.0) { // > 5 minutes
            confidence += 0.2;
        }
        
        // Confidence based on source location (system code less likely to leak)
        if (alloc.sourceFile.find("system") != std::string::npos ||
            alloc.sourceFile.find("kernel") != std::string::npos) {
            confidence *= 0.5; // Reduce confidence for system code
        }
        
        // Ensure confidence is within range
        return std::min(1.0, confidence);
    }
    
    std::vector<LeakResult> detectCircularReferencesInternal() const {
        std::vector<LeakResult> circularLeaks;
        
        // Simplified circular reference detection
        // In a real implementation, this would use graph algorithms or
        // garbage collection debugging APIs
        
        // For now, we just check for certain patterns
        for (const auto& pair : allocations_) {
            const auto& alloc = pair.second;
            if (!alloc.isFreed) {
                // Check for potential circular reference patterns
                if (alloc.typeName.find("node") != std::string::npos ||
                    alloc.typeName.find("object") != std::string::npos) {
                    LeakResult leak;
                    leak.allocationId = alloc.allocationId;
                    leak.leakedSize = alloc.size;
                    leak.sourceInfo = alloc.sourceFile + ":" + std::to_string(alloc.sourceLine) + 
                                    " in " + alloc.functionName;
                    leak.firstAllocation = alloc.timestamp;
                    leak.lastAllocation = alloc.timestamp;
                    leak.leakType = "circular";
                    leak.confidence = 0.6; // Moderate confidence for circular reference detection
                    circularLeaks.push_back(leak);
                }
            }
        }
        
        return circularLeaks;
    }
    
    void initializeTracking() {
        // Initialize allocation tracking data structures
        allocations_.clear();
        deallocations_.clear();
        
        allocationStats_.clear();
        deallocationStats_.clear();
        
        poolInfo_.clear();
        
        // Reset counters
        allocationCount_ = 0;
        deallocationCount_ = 0;
        leakCount_ = 0;
        totalLeakSize_ = 0;
    }
    
    void updateDetectionOverhead() {
        // Calculate detection overhead based on tracking data
        // This is a simplified calculation
        detectionOverhead_ = static_cast<double>(allocationCount_ + deallocationCount_) * 0.001; // 0.1ms per tracking
        
        // Cap at 10ms
        detectionOverhead_ = std::min(0.01, detectionOverhead_);
    }
    
    mutable std::mutex mutex_;
    bool isDetecting_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    size_t allocationCount_;
    size_t deallocationCount_;
    size_t leakCount_;
    size_t totalLeakSize_;
    double detectionOverhead_;
    
    std::map<std::string, AllocationInfo> allocations_;
    std::map<std::string, DeallocationInfo> deallocations_;
    
    struct Stats {
        size_t count = 0;
        size_t totalSize = 0;
    };
    
    std::map<std::string, Stats> allocationStats_;
    std::map<std::string, Stats> deallocationStats_;
    
    std::map<std::string, PoolInfo> poolInfo_;
    
    bool stackTraceEnabled_;
    size_t allocationThreshold_;
    bool timeBasedDetection_;
    Timestamp maxLeakAge_;
};

MemoryLeakDetector::MemoryLeakDetector() : pImpl(new Impl()) {}

MemoryLeakDetector::~MemoryLeakDetector() {
    delete pImpl;
}

void MemoryLeakDetector::startDetection() {
    pImpl->startDetection();
}

void MemoryLeakDetector::stopDetection() {
    pImpl->stopDetection();
}

bool MemoryLeakDetector::isDetecting() const {
    return pImpl->isDetecting();
}

void MemoryLeakDetector::trackAllocation(size_t size, const std::string& sourceFile,
                                         int sourceLine, const std::string& functionName) {
    pImpl->trackAllocation(size, sourceFile, sourceLine, functionName);
}

void MemoryLeakDetector::trackAllocation(const std::string& typeName, size_t size,
                                         const std::string& sourceFile, int sourceLine,
                                         const std::string& functionName) {
    pImpl->trackAllocation(typeName, size, sourceFile, sourceLine, functionName);
}

void MemoryLeakDetector::trackDeallocation(const std::string& allocationId) {
    pImpl->trackDeallocation(allocationId);
}

std::vector<LeakResult> MemoryLeakDetector::detectMemoryLeaks() const {
    return pImpl->detectMemoryLeaks();
}

std::vector<LeakResult> MemoryLeakDetector::detectMemoryLeaksByType(const std::string& typeName) const {
    return pImpl->detectMemoryLeaksByType(typeName);
}

bool MemoryLeakDetector::hasMemoryLeaks() const {
    return pImpl->hasMemoryLeaks();
}

size_t MemoryLeakDetector::getTotalLeakedMemory() const {
    return pImpl->getTotalLeakedMemory();
}

double MemoryLeakDetector::getLeakDetectionConfidence() const {
    return pImpl->getLeakDetectionConfidence();
}

std::vector<MemoryBlock> MemoryLeakDetector::getMemoryBlocks() const {
    return pImpl->getMemoryBlocks();
}

std::vector<MemoryBlock> MemoryLeakDetector::getAllocatedMemoryBlocks() const {
    return pImpl->getAllocatedMemoryBlocks();
}

std::vector<MemoryBlock> MemoryLeakDetector::getUnfreedMemoryBlocks() const {
    return pImpl->getUnfreedMemoryBlocks();
}

std::map<std::string, size_t> MemoryLeakDetector::getAllocationCountsByType() const {
    return pImpl->getAllocationCountsByType();
}

std::vector<std::string> MemoryLeakDetector::detectCircularReferences() const {
    return pImpl->detectCircularReferences();
}

bool MemoryLeakDetector::hasCircularReferences() const {
    return pImpl->hasCircularReferences();
}

void MemoryLeakDetector::setPoolInfo(const std::string& poolName, const PoolInfo& info) {
    pImpl->setPoolInfo(poolName, info);
}

std::map<std::string, PoolInfo> MemoryLeakDetector::getPoolInfo() const {
    return pImpl->getPoolInfo();
}

std::vector<LeakResult> MemoryLeakDetector::detectPoolLeaks() const {
    return pImpl->detectPoolLeaks();
}

LeakResult MemoryLeakDetector::getLeakDetails(const std::string& allocationId) const {
    return pImpl->getLeakDetails(allocationId);
}

std::string MemoryLeakDetector::generateLeakReport() const {
    return pImpl->generateLeakReport();
}

std::string MemoryLeakDetector::generateDetailedLeakReport() const {
    return pImpl->generateDetailedLeakReport();
}

void MemoryLeakDetector::clearLeakData() {
    pImpl->clearLeakData();
}

void MemoryLeakDetector::exportLeakData(const std::string& filepath, const std::string& format) const {
    pImpl->exportLeakData(filepath, format);
}

double MemoryLeakDetector::getDetectionOverhead() const {
    return pImpl->getDetectionOverhead();
}

void MemoryLeakDetector::enableStackTrace(bool enable) {
    pImpl->enableStackTrace(enable);
}

void MemoryLeakDetector::setAllocationThreshold(size_t threshold) {
    pImpl->setAllocationThreshold(threshold);
}

void MemoryLeakDetector::setTimeBasedLeakDetection(bool enable, Timestamp maxAge) {
    pImpl->setTimeBasedLeakDetection(enable, maxAge);
}

} // namespace nlm
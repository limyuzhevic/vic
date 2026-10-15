// NLM Advanced Debugging and Profiling Tools - Memory Debugger Implementation

#include "debug/MemoryDebugger.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cxxabi.h>

namespace nlm {

MemoryDebugger::MemoryDebugger(const MemoryDebugConfig& config) 
    : running_(false), allocationCount_(0), deallocationCount_(0), 
      peakMemoryUsage_(0), totalMemoryAllocated_(0) {
    updateConfig(config);
}

MemoryDebugger::~MemoryDebugger() {
    stop();
}

void MemoryDebugger::updateConfig(const MemoryDebugConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

void* MemoryDebugger::trackAllocation(size_t size, const std::string& sourceFile, 
                                      int sourceLine, const std::string& sourceFunction) {
    if (!running_) return nullptr;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Allocate memory
    void* ptr = std::malloc(size);
    if (!ptr) {
        // Allocation failed
        return nullptr;
    }
    
    // Create allocation record
    AllocationRecord record = createAllocationRecord(ptr, size, sourceFile, sourceLine, sourceFunction);
    
    // Store allocation
    allocations_[ptr] = record;
    allocationHistory_.push_back(record);
    
    // Update statistics
    allocationCount_++;
    totalMemoryAllocated_ += size;
    if (totalMemoryAllocated_ > peakMemoryUsage_) {
        peakMemoryUsage_ = totalMemoryAllocated_;
    }
    
    // Check for bounds checking
    auto boundsIt = boundsCheckingMap_.find(ptr);
    if (boundsIt != boundsCheckingMap_.end()) {
        // Memory is being monitored for bounds checking
    }
    
    // Check for use-after-free tracking
    if (useAfterFreeTrackingMap_.find(ptr) != useAfterFreeTrackingMap_.end()) {
        // Memory is being tracked for use-after-free detection
    }
    
    // Call callback
    if (allocationCallback_) {
        allocationCallback_(record);
    }
    
    // Log allocation
    logAllocation(record);
    
    return ptr;
}

void MemoryDebugger::trackDeallocation(void* address) {
    if (!running_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocations_.find(address);
    if (it == allocations_.end()) {
        // Attempting to deallocate non-allocated memory
        if (useAfterFreeTrackingMap_.find(address) != useAfterFreeTrackingMap_.end()) {
            // This is a use-after-free
            NLM_LOG_WARNING("Use-after-free detected at address: " + std::to_string(reinterpret_cast<size_t>(address)));
        }
        return;
    }
    
    // Create deallocation record
    AllocationRecord& record = it->second;
    
    // Mark as deallocated
    allocations_.erase(it);
    deallocationCount_++;
    
    // Call callback
    if (deallocationCallback_) {
        deallocationCallback_(address);
    }
    
    // Log deallocation
    logDeallocation(address);
}

bool MemoryDebugger::checkBufferOverflow(void* address, size_t accessSize) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = boundsCheckingMap_.find(address);
    if (it == boundsCheckingMap_.end()) {
        return false; // No bounds checking for this address
    }
    
    size_t allocationSize = it->second;
    return (accessSize > allocationSize);
}

void MemoryDebugger::markAllocationForBoundsChecking(void* address, size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    boundsCheckingMap_[address] = size;
}

bool MemoryDebugger::isUseAfterFree(void* address) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return allocations_.find(address) == allocations_.end() && 
           useAfterFreeTrackingMap_.find(address) != useAfterFreeTrackingMap_.end();
}

void MemoryDebugger::markAllocationForUseAfterFreeTracking(void* address) {
    std::lock_guard<std::mutex> lock(mutex_);
    useAfterFreeTrackingMap_.insert(address);
}

void* MemoryDebugger::mallocOverride(size_t size, const std::string& sourceFile, 
                                     int sourceLine, const std::string& sourceFunction) {
    return trackAllocation(size, sourceFile, sourceLine, sourceFunction);
}

void MemoryDebugger::freeOverride(void* ptr, const std::string& sourceFile, 
                                  int sourceLine, const std::string& sourceFunction) {
    trackDeallocation(ptr);
}

void* MemoryDebugger::callocOverride(size_t num, size_t size, const std::string& sourceFile, 
                                     int sourceLine, const std::string& sourceFunction) {
    size_t totalSize = num * size;
    void* ptr = trackAllocation(totalSize, sourceFile, sourceLine, sourceFunction);
    
    if (ptr) {
        std::memset(ptr, 0, totalSize);
    }
    
    return ptr;
}

void* MemoryDebugger::reallocOverride(void* ptr, size_t size, const std::string& sourceFile, 
                                      int sourceLine, const std::string& sourceFunction) {
    if (ptr) {
        // Reallocate existing memory
        trackDeallocation(ptr);
    }
    
    return trackAllocation(size, sourceFile, sourceLine, sourceFunction);
}

void MemoryDebugger::start() {
    if (!running_) {
        running_ = true;
        NLM_LOG_INFO("MemoryDebugger started");
        
        // Start leak detection thread
        if (config_.enableLeakDetection) {
            leakDetectionThread_ = std::thread(&MemoryDebugger::leakDetectionWorker, this);
        }
    }
}

void MemoryDebugger::stop() {
    if (running_) {
        running_ = false;
        
        // Stop threads
        if (leakDetectionThread_.joinable()) {
            leakDetectionThread_.join();
        }
        
        NLM_LOG_INFO("MemoryDebugger stopped");
    }
}

void MemoryDebugger::pause() {
    // Implementation for pausing memory debugging
    NLM_LOG_INFO("MemoryDebugger paused");
}

void MemoryDebugger::resume() {
    // Implementation for resuming memory debugging
    NLM_LOG_INFO("MemoryDebugger resumed");
}

std::vector<AllocationRecord> MemoryDebugger::detectMemoryLeaks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AllocationRecord> leaks;
    
    for (const auto& pair : allocations_) {
        const auto& record = pair.second;
        
        // Check if this allocation has been deallocated (use-after-free check)
        if (useAfterFreeTrackingMap_.find(pair.first) != useAfterFreeTrackingMap_.end()) {
            record.isLeaked = false;
            continue;
        }
        
        // Check allocation age (simplified leak detection)
        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - record.allocationTime).count();
        
        if (age > 300) { // 5 minutes threshold
            leaks.push_back(record);
            record.isLeaked = true;
        }
    }
    
    return leaks;
}

size_t MemoryDebugger::getTotalMemoryAllocated() const {
    return totalMemoryAllocated_;
}

size_t MemoryDebugger::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return calculateMemoryFootprint();
}

size_t MemoryDebugger::getPeakMemoryUsage() const {
    return peakMemoryUsage_;
}

std::string MemoryDebugger::getMemoryUsageReport() const {
    std::ostringstream oss;
    oss << "Memory Usage Report:\n";
    oss << "  Total allocated: " << (totalMemoryAllocated_ / 1024) << " KB\n";
    oss << "  Peak usage: " << (peakMemoryUsage_ / 1024) << " KB\n";
    oss << "  Current usage: " << (calculateMemoryFootprint() / 1024) << " KB\n";
    oss << "  Allocations: " << allocationCount_ << "\n";
    oss << "  Deallocations: " << deallocationCount_ << "\n";
    
    auto leaks = detectMemoryLeaks();
    oss << "  Memory leaks detected: " << leaks.size() << "\n";
    
    return oss.str();
}

std::string MemoryDebugger::getLeakReport() const {
    std::ostringstream oss;
    oss << "Memory Leak Report\n";
    oss << "===================\n\n";
    
    auto leaks = detectMemoryLeaks();
    
    if (leaks.empty()) {
        oss << "No memory leaks detected.\n";
        return oss.str();
    }
    
    oss << "Detected " << leaks.size() << " potential memory leaks:\n\n";
    
    for (const auto& leak : leaks) {
        oss << "Leaked allocation at " << getTimestamp() << "\n";
        oss << "  Size: " << leak.size << " bytes\n";
        oss << "  Source: " << leak.sourceFile << ":" << leak.sourceLine << " in " << leak.sourceFunction << "\n";
        oss << "  Age: " << std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - leak.allocationTime).count() << " seconds\n";
        oss << "  Stack trace:\n";
        
        // Get stack trace (simplified)
        std::vector<std::string> stack = captureStackTrace();
        for (const auto& frame : stack) {
            oss << "    " << frame << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string MemoryDebugger::getAllocationHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "Allocation History (last " << allocationHistory_.size() << " allocations):\n";
    
    size_t startIdx = std::max<size_t>(0, allocationHistory_.size() - config_.maxAllocationHistory);
    
    for (size_t i = startIdx; i < allocationHistory_.size(); ++i) {
        const auto& record = allocationHistory_[i];
        oss << i << ": " << record.size << " bytes at " << getTimestamp() << " "
            << record.sourceFile << ":" << record.sourceLine << " " << record.sourceFunction << "\n";
    }
    
    return oss.str();
}

size_t MemoryDebugger::getAllocationCount() const {
    return allocationCount_;
}

size_t MemoryDebugger::getDeallocationCount() const {
    return deallocationCount_;
}

void MemoryDebugger::attachToBrain(const std::string& brainId) {
    NLM_LOG_INFO("MemoryDebugger attached to brain: " + brainId);
}

void MemoryDebugger::attachToExperiment(const std::string& experimentName) {
    NLM_LOG_INFO("MemoryDebugger attached to experiment: " + experimentName);
}

void MemoryDebugger::setMemoryAllocationCallback(std::function<void(const AllocationRecord&)> callback) {
    allocationCallback_ = callback;
}

void MemoryDebugger::setMemoryDeallocationCallback(std::function<void(void* address)> callback) {
    deallocationCallback_ = callback;
}

void MemoryDebugger::setMemoryLeakCallback(std::function<void(const AllocationRecord&)> callback) {
    leakCallback_ = callback;
}

bool MemoryDebugger::exportAllocationHistory(const std::string& filename, const std::string& format) {
    if (format == "csv") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << "timestamp,size,source_file,source_line,source_function,allocated\n";
        
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& record : allocationHistory_) {
            file << getTimestamp() << "," << record.size << "," << record.sourceFile << 
                 "," << record.sourceLine << ",\" << record.sourceFunction << "\"," << 
                 (record.isLeaked ? "true" : "false") << "\n";
        }
        
        return true;
    }
    
    return false;
}

bool MemoryDebugger::exportLeakReport(const std::string& filename, const std::string& format) {
    if (format == "html") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << "<!DOCTYPE html>\n<html>\n<head><title>Memory Leak Report</title></head>\n<body>\n";
        file << "<h1>Memory Leak Report</h1>\n";
        file << "<pre>" << getLeakReport() << "</pre>\n";
        file << "</body>\n</html>";
        
        return true;
    }
    
    return false;
}

AllocationRecord MemoryDebugger::createAllocationRecord(void* address, size_t size,
                                                       const std::string& sourceFile, int sourceLine,
                                                       const std::string& sourceFunction) {
    AllocationRecord record(address, size, sourceFile, sourceLine, sourceFunction);
    
    // Capture stack trace
    record.tags["stack_trace"] = "";
    std::vector<std::string> stack = captureStackTrace(config_.maxStackTraceDepth);
    for (const auto& frame : stack) {
        record.tags["stack_trace"] += frame + "\n";
    }
    
    return record;
}

std::vector<std::string> MemoryDebugger::captureStackTrace(int maxDepth) const {
    std::vector<std::string> stackTrace;
    
    // Simplified stack trace capture
    // In a real implementation, use platform-specific stack walking APIs
    stackTrace.push_back("MemoryDebugger::captureStackTrace (placeholder)");
    
    return stackTrace;
}

void MemoryDebugger::cleanupDeallocatedAllocations() {
    // Remove deallocated allocations from tracking
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<void*> toRemove;
    for (const auto& pair : allocations_) {
        void* address = pair.first;
        if (useAfterFreeTrackingMap_.find(address) != useAfterFreeTrackingMap_.end()) {
            toRemove.push_back(address);
        }
    }
    
    for (void* address : toRemove) {
        allocations_.erase(address);
        useAfterFreeTrackingMap_.erase(address);
    }
}

void MemoryDebugger::checkForLeaks() const {
    auto leaks = detectMemoryLeaks();
    
    for (const auto& leak : leaks) {
        if (leakCallback_) {
            leakCallback_(leak);
        }
        
        if (config_.generateDetailedReports) {
            NLM_LOG_WARNING("Memory leak detected: " + std::to_string(leak.size) + 
                          " bytes at " + leak.sourceFile + ":" + std::to_string(leak.sourceLine));
        }
    }
}

bool MemoryDebugger::isAddressAllocated(void* address) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return allocations_.find(address) != allocations_.end();
}

void MemoryDebugger::logAllocation(const AllocationRecord& record) {
    NLM_LOG_INFO("Memory allocated: " + std::to_string(record.size) + " bytes at " + 
                record.sourceFile + ":" + std::to_string(record.sourceLine) + 
                " in " + record.sourceFunction);
}

void MemoryDebugger::logDeallocation(void* address) {
    NLM_LOG_INFO("Memory deallocated at address: " + std::to_string(reinterpret_cast<size_t>(address)));
}

std::string MemoryDebugger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

size_t MemoryDebugger::calculateMemoryFootprint() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t footprint = 0;
    for (const auto& pair : allocations_) {
        footprint += pair.second.size;
    }
    return footprint;
}

void MemoryDebugger::leakDetectionWorker() {
    while (running_ && config_.enableLeakDetection) {
        checkForLeaks();
        std::this_thread::sleep_for(std::chrono::seconds(30)); // Check every 30 seconds
    }
}

} // namespace nlm

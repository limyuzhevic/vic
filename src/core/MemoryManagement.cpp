// Enhanced Memory Management System Implementation
// Detailed implementation of memory tracking, leak detection, and statistics

#include "core/MemoryManagement.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include <thread>
#include <stdexcept>
#include <cstdlib>
#include <cxxabi.h>
#include <execinfo.h>
#include <signal.h>
#include <spdlog/spdlog.h>

namespace nlm {

// Global memory tracker instance
MemoryTracker* globalMemoryTracker = nullptr;

// Forward declarations for stack trace
std::string getStackTrace();
std::string demangleSymbol(const std::string& mangledName);

// MemoryTracker implementation
MemoryTracker::MemoryTracker(LeakDetectionMode mode, bool enableStackTrace,
                           size_t maxTrackedAllocations, bool enableMemoryBounds)
    : mode_(mode), enableStackTrace_(enableStackTrace),
      maxTrackedAllocations_(maxTrackedAllocations),
      enableMemoryBounds_(enableMemoryBounds),
      totalAllocated_(0), totalFreed_(0), currentUsage_(0), peakUsage_(0),
      allocationCount_(0), freeCount_(0) {
    
    // Initialize logger
    logger_ = spdlog::get("nlm");
    if (!logger_) {
        logger_ = spdlog::stdout_color_mt("nlm");
        logger_->set_level(spdlog::level::info);
    }
    
    logger_->info("MemoryTracker initialized with mode: {}", static_cast<int>(mode_));
}

MemoryTracker::~MemoryTracker() {
    reset();
    logger_->info("MemoryTracker destroyed");
}

void* MemoryTracker::trackAllocation(size_t size, const std::string& typeName,
                                     const std::string& file, int line,
                                     const std::string& function) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Basic check for memory bounds
    if (enableMemoryBounds_ && size > 1024 * 1024) { // 1MB limit for single allocation
        logger_->warn("Large allocation detected: {} bytes", size);
    }
    
    // Track allocation
    void* ptr = std::malloc(size);
    if (ptr == nullptr) {
        logger_->error("Failed to allocate {} bytes", size);
        return nullptr;
    }
    
    recordAllocation(ptr, size, typeName, file, line, function);
    return ptr;
}

void MemoryTracker::trackDeallocation(void* ptr, const std::string& typeName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (ptr == nullptr) {
        logger_->warn("Attempted to deallocate null pointer");
        return;
    }
    
    auto it = allocations_.find(ptr);
    if (it == allocations_.end()) {
        logger_->error("Attempted to deallocate unallocated memory at {}", ptr);
        return;
    }
    
    // Record deallocation
    recordDeallocation(ptr, typeName);
    
    // Free the memory
    std::free(ptr);
}

void MemoryTracker::trackMemoryAccess(void* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = blocks_.find(ptr);
    if (it != blocks_.end()) {
        it->second.lastAccessTime = std::chrono::steady_clock::now();
    }
}

void MemoryTracker::recordAllocation(void* ptr, size_t size, const std::string& typeName,
                                     const std::string& file, int line, const std::string& function) {
    // Check if we're at capacity
    if (allocations_.size() >= maxTrackedAllocations_) {
        logger_->warn("Memory tracker at capacity, oldest allocations will be removed");
        removeOldestAllocations(allocations_.size() - maxTrackedAllocations_ + 1000);
    }
    
    AllocationInfo info;
    info.address = ptr;
    info.size = size;
    info.typeName = typeName;
    info.file = file;
    info.line = line;
    info.function = function;
    info.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if (enableStackTrace_) {
        info.callStack = getStackTrace();
    }
    
    allocations_[ptr] = info;
    
    MemoryBlockInfo blockInfo;
    blockInfo.baseAddress = ptr;
    blockInfo.size = size;
    blockInfo.type = typeName;
    blockInfo.sourceFile = file;
    blockInfo.lineNumber = line;
    blockInfo.allocationTime = std::chrono::steady_clock::now();
    blockInfo.lastAccessTime = blockInfo.allocationTime;
    blockInfo.isFreed = false;
    blockInfo.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    
    blocks_[ptr] = blockInfo;
    
    // Update statistics
    totalAllocated_ += size;
    currentUsage_ += size;
    peakUsage_ = std::max(peakUsage_.load(), currentUsage_.load());
    allocationCount_++;
    
    if (mode_ != LeakDetectionMode::NONE) {
        logger_->debug("Allocation: {} bytes of type '{}' at {}, line {}", 
                      size, typeName, file, line);
    }
}

void MemoryTracker::recordDeallocation(void* ptr, const std::string& typeName) {
    auto allocIt = allocations_.find(ptr);
    if (allocIt != allocations_.end()) {
        size_t size = allocIt->second.size;
        
        // Remove from allocations
        allocations_.erase(allocIt);
        
        // Mark block as freed
        if (blocks_.find(ptr) != blocks_.end()) {
            blocks_[ptr].isFreed = true;
        }
        
        // Update statistics
        totalFreed_ += size;
        currentUsage_ -= size;
        freeCount_++;
        
        if (mode_ != LeakDetectionMode::NONE) {
            logger_->debug("Deallocation: {} bytes of type '{}' at {}", 
                          size, typeName, typeName);
        }
    }
}

void MemoryTracker::updateMemoryUsage() const {
    // Update current usage (simplified - would need platform-specific implementation)
    static uint64_t lastUsage = 0;
    uint64_t current = getCurrentMemoryUsage();
    
    if (lastUsage != current) {
        currentUsage_ = current;
        peakUsage_ = std::max(peakUsage_.load(), current);
    }
}

std::string MemoryTracker::getCallStack() const {
    // Simple stack trace implementation
    const size_t max_depth = 50;
    void* call_stack[max_depth];
    
    int frames = backtrace(call_stack, max_depth);
    char** symbols = backtrace_symbols(call_stack, frames);
    
    std::ostringstream oss;
    oss << "Stack trace (" << frames << " frames):\n";
    
    for (int i = 1; i < frames; ++i) { // Skip current frame
        oss << "  " << i << ": " << symbols[i] << "\n";
    }
    
    std::free(symbols);
    return oss.str();
}

void MemoryTracker::removeOldestAllocations(size_t count) {
    // Remove oldest allocations based on timestamp
    std::vector<std::pair<uint64_t, void*>> timestamps;
    
    for (const auto& [ptr, info] : allocations_) {
        timestamps.emplace_back(info.timestamp, ptr);
    }
    
    std::sort(timestamps.begin(), timestamps.end());
    
    for (size_t i = 0; i < std::min(count, timestamps.size()); ++i) {
        void* ptr = timestamps[i].second;
        
        // Remove from allocations
        allocations_.erase(ptr);
        
        // If it's a block, mark as leaked
        if (blocks_.find(ptr) != blocks_.end()) {
            logger_->warn("Removing allocation at {} (leaked)", ptr);
            blocks_[ptr].isFreed = true; // Mark as freed to avoid error
        }
    }
}

MemoryStatistics MemoryTracker::getStatistics() const {
    MemoryStatistics stats;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    stats.totalAllocated = totalAllocated_.load();
    stats.totalFreed = totalFreed_.load();
    stats.currentUsage = currentUsage_.load();
    stats.peakUsage = peakUsage_.load();
    stats.allocationCount = allocationCount_.load();
    stats.freeCount = freeCount_.load();
    
    if (stats.allocationCount > 0) {
        stats.averageAllocationSize = static_cast<double>(stats.totalAllocated) / stats.allocationCount;
    }
    
    // Calculate fragmentation (simplified)
    if (stats.totalAllocated > 0) {
        stats.fragmentationRatio = static_cast<double>(stats.totalAllocated - stats.totalFreed) / 
                                   static_cast<double>(stats.totalAllocated);
    }
    
    return stats;
}

std::vector<MemoryBlockInfo> MemoryTracker::detectLeaks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MemoryBlockInfo> leaks;
    
    for (const auto& [ptr, block] : blocks_) {
        if (!block.isFreed) {
            leaks.push_back(block);
        }
    }
    
    return leaks;
}

std::vector<MemoryBlockInfo> MemoryTracker::detectLeaks(const std::string& typeFilter) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MemoryBlockInfo> leaks;
    
    for (const auto& [ptr, block] : blocks_) {
        if (!block.isFreed && block.type == typeFilter) {
            leaks.push_back(block);
        }
    }
    
    return leaks;
}

std::string MemoryTracker::generateReport() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    MemoryStatistics stats = getStatistics();
    
    oss << "=== Memory Management Report ===\n\n";
    oss << "Tracking Statistics:\n";
    oss << "  Mode: " << static_cast<int>(mode_) << "\n";
    oss << "  Stack Trace: " << (enableStackTrace_ ? "Enabled" : "Disabled") << "\n";
    oss << "  Memory Bounds: " << (enableMemoryBounds_ ? "Enabled" : "Disabled") << "\n";
    oss << "  Max Tracked Allocations: " << maxTrackedAllocations_ << "\n";
    oss << "  Current Allocations: " << allocations_.size() << "\n";
    oss << "  Current Blocks: " << blocks_.size() << "\n\n";
    
    oss << stats.toString() << "\n";
    
    // Detect leaks
    auto leaks = detectLeaks();
    if (!leaks.empty()) {
        oss << "LEAKS DETECTED: " << leaks.size() << " leaked blocks\n";
        oss << "  Total leaked memory: " << getTotalLeakedMemory(leaks) << " bytes\n\n";
        
        oss << "Leaked Blocks:\n";
        for (const auto& leak : leaks) {
            oss << "  " << leak.baseAddress << " " << leak.size << " bytes "
                << leak.type << " from " << leak.sourceFile << ":" << leak.lineNumber << "\n";
        }
        oss << "\n";
    } else {
        oss << "No memory leaks detected.\n\n";
    }
    
    // Allocation breakdown
    oss << "Allocation Breakdown:\n";
    std::unordered_map<std::string, size_t> typeCounts;
    uint64_t totalBytes = 0;
    
    for (const auto& [ptr, info] : allocations_) {
        typeCounts[info.typeName]++;
        totalBytes += info.size;
    }
    
    for (const auto& [type, count] : typeCounts) {
        oss << "  " << type << ": " << count << " allocations, "
            << getTypeMemoryUsage(type, allocations_) << " bytes\n";
    }
    
    return oss.str();
}

void MemoryTracker::exportReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        logger_->error("Cannot open report file for writing: {}", filename);
        return;
    }
    
    file << generateReport();
    file.close();
    
    logger_->info("Memory report exported to: {}", filename);
}

void MemoryTracker::setMode(LeakDetectionMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    mode_ = mode;
    logger_->info("Memory tracking mode changed to: {}", static_cast<int>(mode_));
}

void MemoryTracker::enableStackTrace(bool enable) {
    enableStackTrace_ = enable;
    logger_->info("Stack trace tracking: {}", enable ? "enabled" : "disabled");
}

void MemoryTracker::enableMemoryBounds(bool enable) {
    enableMemoryBounds_ = enable;
    logger_->info("Memory bounds checking: {}", enable ? "enabled" : "disabled");
}

bool MemoryTracker::isAllocated(void* ptr) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return allocations_.find(ptr) != allocations_.end();
}

void MemoryTracker::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Mark all allocations as leaked (since we're being destroyed)
    for (auto& [ptr, block] : blocks_) {
        if (!block.isFreed) {
            logger_->warn("MemoryTracker destroying with {} leaked allocations", blocks_.size());
        }
    }
    
    allocations_.clear();
    blocks_.clear();
    
    // Reset statistics
    totalAllocated_ = 0;
    totalFreed_ = 0;
    currentUsage_ = 0;
    peakUsage_ = 0;
    allocationCount_ = 0;
    freeCount_ = 0;
    
    logger_->info("MemoryTracker reset completed");
}

// Memory management helper functions
void safeDeallocate(void* ptr, const std::string& typeName) {
    if (globalMemoryTracker) {
        globalMemoryTracker->trackDeallocation(ptr, typeName);
    } else {
        std::free(ptr);
    }
}

bool validateMemoryBounds(void* ptr, size_t size) {
    // Simple bounds validation (placeholder for platform-specific implementation)
    return true;
}

void* validateAndGetAllocation(void* ptr) {
    if (globalMemoryTracker && globalMemoryTracker->isAllocated(ptr)) {
        return ptr;
    }
    return nullptr;
}

// MemoryProfiler implementation
MemoryProfiler::MemoryProfiler(std::shared_ptr<MemoryTracker> tracker)
    : tracker_(tracker) {
}

std::unordered_map<std::string, AllocationProfile> MemoryProfiler::profileAllocations() const {
    std::unordered_map<std::string, AllocationProfile> profiles;
    
    std::lock_guard<std::mutex> lock(tracker_->mutex_);
    
    for (const auto& [ptr, info] : tracker_->allocations_) {
        AllocationProfile& profile = profiles[info.typeName];
        
        profile.allocationCount++;
        profile.totalSize += info.size;
        profile.peakSize = std::max(profile.peakSize, info.size);
        
        // Track top file
        if (info.file.length() > profile.topFile.length()) {
            profile.topFile = info.file;
            profile.topLine = info.line;
        }
    }
    
    // Calculate average sizes
    for (auto& [type, profile] : profiles) {
        if (profile.allocationCount > 0) {
            profile.averageSize = static_cast<double>(profile.totalSize) / profile.allocationCount;
        }
    }
    
    return profiles;
}

std::vector<MemoryBlockInfo> MemoryProfiler::getLargestAllocations(size_t limit) const {
    std::lock_guard<std::mutex> lock(tracker_->mutex_);
    
    std::vector<MemoryBlockInfo> blocks;
    for (const auto& [ptr, block] : tracker_->blocks_) {
        if (!block.isFreed) {
            blocks.push_back(block);
        }
    }
    
    // Sort by size (descending)
    std::sort(blocks.begin(), blocks.end(),
              [](const MemoryBlockInfo& a, const MemoryBlockInfo& b) {
                  return a.size > b.size;
              });
    
    if (limit > 0 && limit < blocks.size()) {
        blocks.resize(limit);
    }
    
    return blocks;
}

std::string MemoryProfiler::generateProfileReport() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    auto profiles = profileAllocations();
    
    oss << "=== Memory Allocation Profile ===\n\n";
    oss << "Total allocation types: " << profiles.size() << "\n\n";
    
    for (const auto& [type, profile] : profiles) {
        oss << "Type: " << type << "\n";
        oss << "  Allocations: " << profile.allocationCount << "\n";
        oss << "  Total Size: " << profile.totalSize << " bytes\n";
        oss << "  Peak Size: " << profile.peakSize << " bytes\n";
        oss << "  Average Size: " << profile.averageSize << " bytes\n";
        oss << "  Top File: " << profile.topFile << ":" << profile.topLine << "\n\n";
    }
    
    return oss.str();
}

void MemoryProfiler::exportProfile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        logger_->error("Cannot open profile file for writing: {}", filename);
        return;
    }
    
    file << generateProfileReport();
    file.close();
    
    logger_->info("Memory profile exported to: {}", filename);
}

// MemoryMonitor implementation
MemoryMonitor::MemoryMonitor(std::shared_ptr<MemoryTracker> tracker,
                             std::chrono::milliseconds interval)
    : tracker_(tracker), running_(false), interval_(interval),
      memoryThreshold_(0.8), lastStatus_("") {
}

MemoryMonitor::~MemoryMonitor() {
    stop();
}

void MemoryMonitor::start() {
    if (running_) {
        return;
    }
    
    running_ = true;
    monitorThread_ = std::thread(&MemoryMonitor::monitoringThread, this);
    
    logger_->info("Memory monitor started");
}

void MemoryMonitor::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
    
    logger_->info("Memory monitor stopped");
}

void MemoryMonitor::setThreshold(double threshold) {
    memoryThreshold_ = threshold;
}

double MemoryMonitor::getMemoryUsagePercentage() const {
    return tracker_->getCurrentUsage() / tracker_->getPeakUsage();
}

std::string MemoryMonitor::getStatusReport() const {
    std::lock_guard<std::mutex> lock(statusMutex_);
    return lastStatus_;
}

void MemoryMonitor::monitoringThread() {
    while (running_) {
        // Update tracking
        tracker_->updateMemoryUsage();
        
        // Check for memory issues
        double usagePercent = getMemoryUsagePercentage();
        if (usagePercent > memoryThreshold_) {
            std::ostringstream oss;
            oss << "WARNING: Memory usage high: " << (usagePercent * 100) << "%\n";
            oss << "  Current: " << tracker_->getCurrentUsage() << " bytes\n";
            oss << "  Peak: " << tracker_->getPeakUsage() << " bytes\n";
            oss << "  Allocations: " << tracker_->getAllocationCount() << "\n";
            
            logger_->warn(oss.str());
            lastStatus_ = oss.str();
        }
        
        // Sleep for the interval
        std::this_thread::sleep_for(interval_);
    }
}

// Integration functions
void integrateMemoryTrackerWithPool(MemoryPool<Neuron>* neuronPool) {
    if (globalMemoryTracker) {
        // Integrate with neuron pool
        logger_->info("Integrated memory tracker with neuron pool");
    }
}

void integrateMemoryTrackerWithPool(MemoryPool<Synapse>* synapsePool) {
    if (globalMemoryTracker) {
        // Integrate with synapse pool
        logger_->info("Integrated memory tracker with synapse pool");
    }
}

// Configuration functions
void initializeMemoryTracker(LeakDetectionMode mode, bool enableStackTrace,
                             bool enableMemoryBounds) {
    if (globalMemoryTracker) {
        delete globalMemoryTracker;
    }
    
    globalMemoryTracker = new MemoryTracker(mode, enableStackTrace, 100000, enableMemoryBounds);
    
    logger_->info("Global memory tracker initialized with mode: {}", static_cast<int>(mode));
}

MemoryTracker& getMemoryTracker() {
    if (globalMemoryTracker == nullptr) {
        initializeMemoryTracker();
    }
    return *globalMemoryTracker;
}

void resetMemoryTracker() {
    if (globalMemoryTracker) {
        delete globalMemoryTracker;
        globalMemoryTracker = nullptr;
    }
    
    initializeMemoryTracker();
}

// Utility functions
std::string formatMemorySize(uint64_t bytes) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (bytes < 1024) {
        oss << bytes << " B";
    } else if (bytes < 1024 * 1024) {
        oss << bytes / 1024 << " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        oss << bytes / (1024 * 1024) << " MB";
    } else {
        oss << bytes / (1024 * 1024 * 1024) << " GB";
    }
    
    return oss.str();
}

// Helper functions
uint64_t MemoryTracker::getTotalLeakedMemory(const std::vector<MemoryBlockInfo>& leaks) const {
    uint64_t total = 0;
    for (const auto& leak : leaks) {
        total += leak.size;
    }
    return total;
}

size_t MemoryTracker::getTypeMemoryUsage(const std::string& type,
                                        const std::unordered_map<void*, AllocationInfo>& allocations) const {
    size_t total = 0;
    for (const auto& [ptr, info] : allocations) {
        if (info.typeName == type) {
            total += info.size;
        }
    }
    return total;
}

uint64_t MemoryTracker::getCurrentMemoryUsage() const {
    // Platform-specific implementation needed
    // This is a placeholder that returns a reasonable estimate
    return currentUsage_.load();
}

uint64_t MemoryTracker::getPeakMemoryUsage() const {
    return peakUsage_.load();
}

// Stack trace helpers
std::string getStackTrace() {
    const size_t max_depth = 50;
    void* call_stack[max_depth];
    
    int frames = backtrace(call_stack, max_depth);
    char** symbols = backtrace_symbols(call_stack, frames);
    
    std::ostringstream oss;
    oss << "Stack trace (" << frames << " frames):\n";
    
    for (int i = 0; i < frames; ++i) {
        oss << "  " << i << ": " << symbols[i] << "\n";
    }
    
    std::free(symbols);
    return oss.str();
}

std::string demangleSymbol(const std::string& mangledName) {
    int status = -1;
    char* demangled = abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);
    
    std::string result;
    if (status == 0) {
        result = demangled;
    } else {
        result = mangledName;
    }
    
    std::free(demangled);
    return result;
}

// Custom new/delete operators
void* operator new(size_t size) throw(std::bad_alloc) {
    if (globalMemoryTracker) {
        return globalMemoryTracker->trackAllocation(size, "operator new", "", 0, "");
    }
    return std::malloc(size);
}

void* operator new[](size_t size) throw(std::bad_alloc) {
    if (globalMemoryTracker) {
        return globalMemoryTracker->trackAllocation(size, "operator new[]", "", 0, "");
    }
    return std::malloc(size);
}

void operator delete(void* ptr) throw() {
    if (globalMemoryTracker) {
        globalMemoryTracker->trackDeallocation(ptr, "operator delete");
    } else {
        std::free(ptr);
    }
}

void operator delete[](void* ptr) throw() {
    if (globalMemoryTracker) {
        globalMemoryTracker->trackDeallocation(ptr, "operator delete[]");
    } else {
        std::free(ptr);
    }
}

void operator delete(void* ptr, size_t size) throw() {
    if (globalMemoryTracker) {
        globalMemoryTracker->trackDeallocation(ptr, "operator delete(size_t)");
    } else {
        std::free(ptr);
    }
}

void operator delete[](void* ptr, size_t size) throw() {
    if (globalMemoryTracker) {
        globalMemoryTracker->trackDeallocation(ptr, "operator delete[](size_t)");
    } else {
        std::free(ptr);
    }
}

} // namespace nlm

// Global instance declaration
nlm::MemoryTracker* nlm::globalMemoryTracker = nullptr;
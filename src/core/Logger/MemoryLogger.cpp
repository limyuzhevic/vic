#include "Logger.hpp"
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

struct MemoryLogger::Impl {
    bool enableMemoryTracking = true;
    std::unordered_map<void*, MemoryBlock> memoryBlocks;
    std::mutex memoryMutex;
    
    // Performance tracking
    uint64_t totalAllocations = 0;
    uint64_t totalDeallocations = 0;
    uint64_t peakMemoryUsage = 0;
    uint64_t currentMemoryUsage = 0;
    
    // Category filtering
    std::unordered_map<std::string, bool> categoryEnabled;
    LogLevel level = LogLevel::Info;
    std::shared_ptr<LogFormatter> formatter;
};

MemoryLogger::MemoryLogger()
    : pImpl(std::make_unique<Impl>()) {
    // Initialize category filters
    pImpl->categoryEnabled[LogCategory::Default] = true;
    pImpl->categoryEnabled[LogCategory::Core] = true;
    pImpl->categoryEnabled[LogCategory::Memory] = true;
    pImpl->categoryEnabled[LogCategory::Brain] = true;
    pImpl->categoryEnabled[LogCategory::Motor] = true;
    pImpl->categoryEnabled[LogCategory::Sensory] = true;
    pImpl->categoryEnabled[LogCategory::Cognition] = true;
    pImpl->categoryEnabled[LogCategory::Development] = true;
    pImpl->categoryEnabled[LogCategory::Plasticity] = true;
    pImpl->categoryEnabled[LogCategory::Neuromodulation] = true;
    pImpl->categoryEnabled[LogCategory::Prediction] = true;
    pImpl->categoryEnabled[LogCategory::ErrorHandling] = true;
    pImpl->categoryEnabled[LogCategory::Config] = true;
    pImpl->categoryEnabled[LogCategory::Plugin] = true;
    pImpl->categoryEnabled[LogCategory::Performance] = true;
    pImpl->categoryEnabled[LogCategory::MemoryAllocation] = true;
    pImpl->categoryEnabled[LogCategory::Threading] = true;
    pImpl->categoryEnabled[LogCategory::Backtrace] = true;
}

MemoryLogger::~MemoryLogger() {
    logMemoryStats();
}

void MemoryLogger::log(const LogEntry& entry) {
    if (entry.level < pImpl->level) {
        return;
    }
    
    if (!isCategoryEnabled(entry.category)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    std::string formattedMessage;
    if (pImpl->formatter) {
        formattedMessage = pImpl->formatter->format(entry);
    } else {
        formattedMessage = SimpleFormatter().format(entry);
    }
    
    // Log memory stats periodically (every 100 logs)
    static uint64_t logCount = 0;
    if (++logCount % 100 == 0) {
        logMemoryStats();
    }
}

void MemoryLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel MemoryLogger::getLevel() const {
    return pImpl->level;
}

void MemoryLogger::setCategoryFilter(const std::string& category) {
    // Simple implementation: enable/disable specific category
    for (auto& pair : pImpl->categoryEnabled) {
        pair.second = (pair.first == category);
    }
}

void MemoryLogger::addCategory(const std::string& category) {
    pImpl->categoryEnabled[category] = true;
}

void MemoryLogger::removeCategory(const std::string& category) {
    pImpl->categoryEnabled.erase(category);
}

bool MemoryLogger::isCategoryEnabled(const std::string& category) const {
    auto it = pImpl->categoryEnabled.find(category);
    return (it != pImpl->categoryEnabled.end()) && it->second;
}

void MemoryLogger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
    pImpl->formatter = formatter;
}

std::shared_ptr<LogFormatter> MemoryLogger::getFormatter() const {
    return pImpl->formatter;
}

void MemoryLogger::enableMemoryTracking(bool enabled) {
    pImpl->enableMemoryTracking = enabled;
}

void MemoryLogger::trackAllocation(void* ptr, size_t size, const std::string& context) {
    if (!pImpl->enableMemoryTracking) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    MemoryBlock block;
    block.ptr = ptr;
    block.size = size;
    block.context = context;
    block.timestamp = std::chrono::system_clock::now();
    
    pImpl->memoryBlocks[ptr] = block;
    pImpl->totalAllocations++;
    pImpl->currentMemoryUsage += size;
    pImpl->peakMemoryUsage = std::max(pImpl->peakMemoryUsage, pImpl->currentMemoryUsage);
    
    // Log the allocation
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = "Memory allocated: " + std::to_string(size) + " bytes in " + context;
    entry.category = LogCategory::MemoryAllocation;
    
    log(entry);
}

void MemoryLogger::trackDeallocation(void* ptr) {
    if (!pImpl->enableMemoryTracking) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    auto it = pImpl->memoryBlocks.find(ptr);
    if (it != pImpl->memoryBlocks.end()) {
        pImpl->currentMemoryUsage -= it->second.size;
        pImpl->totalDeallocations++;
        pImpl->memoryBlocks.erase(it);
        
        // Log the deallocation
        LogEntry entry;
        entry.level = LogLevel::Info;
        entry.message = "Memory deallocated: " + std::to_string(it->second.size) + " bytes";
        entry.category = LogCategory::MemoryAllocation;
        
        log(entry);
    }
}

void MemoryLogger::logMemoryStats() {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex);
    
    std::stringstream ss;
    ss << "=== Memory Statistics ===" << std::endl;
    ss << "Total allocations: " << pImpl->totalAllocations << std::endl;
    ss << "Total deallocations: " << pImpl->totalDeallocations << std::endl;
    ss << "Current memory usage: " << pImpl->currentMemoryUsage << " bytes" << std::endl;
    ss << "Peak memory usage: " << pImpl->peakMemoryUsage << " bytes" << std::endl;
    ss << "Active memory blocks: " << pImpl->memoryBlocks.size() << std::endl;
    
    LogEntry entry;
    entry.level = LogLevel::Info;
    entry.message = ss.str();
    entry.category = LogCategory::Memory;
    
    // This would go to the real logger in a full implementation
    // For now, just store it
}
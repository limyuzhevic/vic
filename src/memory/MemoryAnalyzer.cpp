#include "MemoryAnalyzer.hpp"
#include "core/Logger/Logger.hpp"
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

namespace nlm {

struct MemoryAnalyzer::Impl {
    bool tracking = false;
    size_t currentMemoryUsage = 0;
    size_t peakMemoryUsage = 0;
    size_t totalAllocated = 0;
    size_t totalDeallocated = 0;
    size_t allocatedBlockCount = 0;
    
    // Block tracking
    std::map<void*, MemoryAllocation> allocatedBlocks;
    std::map<void*, MemoryBlockState> blockStates;
    
    // Allocation sites
    std::map<std::string, size_t> allocationSites;
    
    // Integration
    std::shared_ptr<Profiler> profiler;
};

MemoryAnalyzer::MemoryAnalyzer() : pImpl(std::make_unique<Impl>()) {
    // Override global operators for memory tracking
    std::cout << "MemoryAnalyzer initialized" << std::endl;
}

MemoryAnalyzer::~MemoryAnalyzer() {
    if (pImpl->tracking) {
        stopTracking();
    }
}

void MemoryAnalyzer::startTracking() {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    if (pImpl->tracking) {
        return;
    }
    
    pImpl->tracking = true;
    pImpl->currentMemoryUsage = 0;
    pImpl->peakMemoryUsage = 0;
    pImpl->allocatedBlockCount = 0;
    pImpl->allocatedBlocks.clear();
    pImpl->blockStates.clear();
    pImpl->allocationSites.clear();
    
    NLM_LOG_INFO("Memory tracking started");
}

void MemoryAnalyzer::stopTracking() {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    if (!pImpl->tracking) {
        return;
    }
    
    pImpl->tracking = false;
    
    // Detect any remaining leaks
    detectMemoryLeaks();
    
    NLM_LOG_INFO("Memory tracking stopped");
}

void MemoryAnalyzer::trackAllocation(void* address, size_t size, const std::string& allocationSite) {
    if (!pImpl->tracking || !address) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    // Update statistics
    pImpl->currentMemoryUsage += size;
    if (pImpl->currentMemoryUsage > pImpl->peakMemoryUsage) {
        pImpl->peakMemoryUsage = pImpl->currentMemoryUsage;
    }
    pImpl->totalAllocated += size;
    pImpl->allocatedBlockCount++;
    
    // Record allocation
    auto& allocation = pImpl->allocatedBlocks[address];
    allocation = MemoryAllocation(address, size, allocationSite);
    pImpl->blockStates[address] = MemoryBlockState::Allocated;
    
    // Track allocation site
    if (!allocationSite.empty()) {
        pImpl->allocationSites[allocationSite]++;
    }
    
    NLM_LOG_DEBUG("Memory allocation tracked: " + std::to_string(size) + 
                  " bytes at " + std::to_string(reinterpret_cast<uintptr_t>(address)) +
                  " (site: " + allocationSite + ")");
}

void MemoryAnalyzer::trackDeallocation(void* address) {
    if (!pImpl->tracking || !address) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    auto it = pImpl->allocatedBlocks.find(address);
    if (it == pImpl->allocatedBlocks.end()) {
        NLM_LOG_WARNING("Attempting to deallocate untracked memory: " + 
                       std::to_string(reinterpret_cast<uintptr_t>(address)));
        return;
    }
    
    size_t size = it->second.size;
    std::string allocationSite = it->second.allocationSite;
    
    // Update statistics
    pImpl->currentMemoryUsage -= size;
    pImpl->totalDeallocated += size;
    
    // Mark as freed
    pImpl->blockStates[address] = MemoryBlockState::Freed;
    
    NLM_LOG_DEBUG("Memory deallocation tracked: " + std::to_string(size) + 
                  " bytes at " + std::to_string(reinterpret_cast<uintptr_t>(address)) +
                  " (site: " + allocationSite + ")");
}

 size_t MemoryAnalyzer::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    return pImpl->currentMemoryUsage;
}

 size_t MemoryAnalyzer::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    return pImpl->peakMemoryUsage;
}

 size_t MemoryAnalyzer::getTotalAllocated() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    return pImpl->totalAllocated;
}

 size_t MemoryAnalyzer::getTotalDeallocated() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    return pImpl->totalDeallocated;
}

 size_t MemoryAnalyzer::getAllocatedBlockCount() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    return pImpl->allocatedBlockCount;
}

 bool MemoryAnalyzer::isBlockAllocated(void* address) const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    auto it = pImpl->blockStates.find(address);
    if (it == pImpl->blockStates.end()) {
        return false;
    }
    
    return it->second == MemoryBlockState::Allocated;
}

 size_t MemoryAnalyzer::getBlockSize(void* address) const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    auto it = pImpl->allocatedBlocks.find(address);
    if (it == pImpl->allocatedBlocks.end()) {
        return 0;
    }
    
    return it->second.size;
}

 std::string MemoryAnalyzer::getAllocationSite(void* address) const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    auto it = pImpl->allocatedBlocks.find(address);
    if (it == pImpl->allocatedBlocks.end()) {
        return "";
    }
    
    return it->second.allocationSite;
}

 void MemoryAnalyzer::detectMemoryLeaks() {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    std::vector<void*> leakedBlocks;
    
    for (const auto& pair : pImpl->blockStates) {
        if (pair.second == MemoryBlockState::Allocated) {
            leakedBlocks.push_back(pair.first);
        }
    }
    
    if (!leakedBlocks.empty()) {
        NLM_LOG_WARNING(std::to_string(leakedBlocks.size()) + 
                       " potential memory leaks detected");
        
        for (void* address : leakedBlocks) {
            auto it = pImpl->allocatedBlocks.find(address);
            if (it != pImpl->allocatedBlocks.end()) {
                NLM_LOG_WARNING("Leaked block: " + std::to_string(it->second.size) + 
                               " bytes at " + std::to_string(reinterpret_cast<uintptr_t>(address)) +
                               " (site: " + it->second.allocationSite + ")");
            }
        }
    }
}

 std::vector<void*> MemoryAnalyzer::findLeakedBlocks() const {
    std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
    
    std::vector<void*> leakedBlocks;
    
    for (const auto& pair : pImpl->blockStates) {
        if (pair.second == MemoryBlockState::Allocated) {
            leakedBlocks.push_back(pair.first);
        }
    }
    
    return leakedBlocks;
}

 std::string MemoryAnalyzer::generateReport() const {
    std::ostringstream oss;
    
    oss << "=== NLM Memory Analyzer Report ===\n";
    oss << "Tracking Active: " << (pImpl->tracking ? "Yes" : "No") << "\n";
    oss << "Current Memory Usage: " << getCurrentMemoryUsage() << " bytes\n";
    oss << "Peak Memory Usage: " << getPeakMemoryUsage() << " bytes\n";
    oss << "Total Allocated: " << getTotalAllocated() << " bytes\n";
    oss << "Total Deallocated: " << getTotalDeallocated() << " bytes\n";
    oss << "Allocated Block Count: " << getAllocatedBlockCount() << "\n";
    oss << "Net Memory Change: " << (getTotalAllocated() - getTotalDeallocated()) << " bytes\n";
    
    if (!pImpl->allocationSites.empty()) {
        oss << "\nAllocation Sites:\n";
        for (const auto& site : pImpl->allocationSites) {
            oss << "  " << site.first << ": " << site.second << " allocations\n";
        }
    }
    
    return oss.str();
}

 void MemoryAnalyzer::printReport() const {
     std::cout << generateReport() << std::endl;
 }

 double MemoryAnalyzer::compareToBaseline(const std::map<std::string, size_t>& baseline) const {
     double score = 0.0;
     
     for (const auto& baselinePair : baseline) {
         auto it = pImpl->allocationSites.find(baselinePair.first);
         if (it != pImpl->allocationSites.end()) {
             double ratio = static_cast<double>(it->second) / baselinePair.second;
             score += std::abs(ratio - 1.0);  // Deviation from baseline
         }
     }
     
     return score / baseline.size();
 }

 void MemoryAnalyzer::integrateWithProfiler(Profiler& profiler) {
     std::lock_guard<std::mutex> lock(pImpl->memoryMutex_);
     pImpl->profiler = std::make_shared<Profiler>(&profiler);
     NLM_LOG_INFO("Memory analyzer integrated with profiler");
 }

// Global memory analyzer instances
namespace {
    std::shared_ptr<MemoryAnalyzer> globalMemoryAnalyzer = nullptr;
    std::mutex globalAnalyzerMutex;
}

MemoryAnalyzer& getMemoryAnalyzer() {
    std::lock_guard<std::mutex> lock(globalAnalyzerMutex);
    if (!globalMemoryAnalyzer) {
        globalMemoryAnalyzer = std::make_shared<MemoryAnalyzer>();
    }
    return *globalMemoryAnalyzer;
}

void initializeMemoryAnalyzer() {
    std::lock_guard<std::mutex> lock(globalAnalyzerMutex);
    if (!globalMemoryAnalyzer) {
        globalMemoryAnalyzer = std::make_shared<MemoryAnalyzer>();
        NLM_LOG_INFO("Memory Analyzer initialized");
    }
}

void shutdownMemoryAnalyzer() {
    std::lock_guard<std::mutex> lock(globalAnalyzerMutex);
    if (globalMemoryAnalyzer) {
        globalMemoryAnalyzer->stopTracking();
        globalMemoryAnalyzer.reset();
        NLM_LOG_INFO("Memory Analyzer shut down");
    }
}

} // namespace nlm

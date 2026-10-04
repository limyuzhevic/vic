#include "MemoryProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

MemoryProfiler::MemoryProfiler()
    : totalAllocated_(0), totalFreed_(0), currentUsage_(0),
      memoryCheckInterval_(1000), detectMemoryLeaks_(true), leakThreshold_(1024),
      maxSnapshots_(100) {
}

MemoryProfiler::MemoryProfiler(std::shared_ptr<DebuggingSystem> debuggingSystem)
    : debuggingSystem_(debuggingSystem),
      totalAllocated_(0), totalFreed_(0), currentUsage_(0),
      memoryCheckInterval_(1000), detectMemoryLeaks_(true), leakThreshold_(1024),
      maxSnapshots_(100) {
}

MemoryProfiler::~MemoryProfiler() = default;

void MemoryProfiler::recordAllocation(size_t size, const std::string& context,
                                     const std::string& site) {
    MemoryAllocation allocation(size, context, site);
    
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    allocations_.push_back(allocation);
    
    totalAllocated_ += size;
    currentUsage_ += size;
    
    if (debuggingSystem_) {
        debuggingSystem_->recordMemoryAllocation(size, context);
    }
}

void MemoryProfiler::recordDeallocation(size_t size, const std::string& context,
                                       const std::string& site) {
    MemoryDeallocation deallocation(size, context, site);
    
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    deallocations_.push_back(deallocation);
    
    totalFreed_ += size;
    currentUsage_ -= std::min(currentUsage_, size);
    
    if (debuggingSystem_) {
        debuggingSystem_->recordMemoryDeallocation(size, context);
    }
}

void MemoryProfiler::takeSnapshot() {
    MemorySnapshot snapshot;
    
    {
        std::lock_guard<std::mutex> allocLock(allocationsMutex_);
        std::lock_guard<std::mutex> deallocLock(deallocationsMutex_);
        
        snapshot.totalAllocated = totalAllocated_;
        snapshot.totalFreed = totalFreed_;
        snapshot.currentUsage = currentUsage_;
        
        // Calculate allocations by context
        for (const auto& alloc : allocations_) {
            snapshot.allocationsByContext[alloc.context] += alloc.size;
        }
        
        // Calculate deallocations by context
        for (const auto& dealloc : deallocations_) {
            snapshot.deallocationsByContext[dealloc.context] += dealloc.size;
        }
    }
    
    std::lock_guard<std::mutex> lock(snapshotsMutex_);
    snapshots_.push_back(snapshot);
    
    // Keep only last maxSnapshots_ entries
    if (snapshots_.size() > maxSnapshots_) {
        snapshots_.erase(snapshots_.begin());
    }
    
    // Detect leaks if enabled
    if (detectMemoryLeaks_) {
        currentLeaks_ = detectMemoryLeaks();
    }
}

std::vector<MemoryLeakInfo> MemoryProfiler::detectMemoryLeaks() const {
    std::vector<MemoryLeakInfo> leaks;
    
    // Simple leak detection: look for allocations without corresponding deallocations
    std::unordered_map<std::string, std::vector<MemoryAllocation>> leaksByContext;
    
    std::lock_guard<std::mutex> allocLock(allocationsMutex_);
    std::lock_guard<std::mutex> deallocLock(deallocationsMutex_);
    
    // For each allocation, check if there's a matching deallocation
    for (const auto& alloc : allocations_) {
        size_t matchingDeallocSize = 0;
        
        for (const auto& dealloc : deallocations_) {
            if (dealloc.context == alloc.context && 
                dealloc.size >= alloc.size &&
                std::chrono::duration_cast<std::chrono::seconds>(
                    dealloc.timestamp - alloc.timestamp).count() > 0) {
                matchingDeallocSize += alloc.size;
                break;
            }
        }
        
        if (matchingDeallocSize < alloc.size) {
            leaksByContext[alloc.context].push_back(alloc);
        }
    }
    
    // Convert to MemoryLeakInfo objects
    for (const auto& pair : leaksByContext) {
        size_t totalLeaked = 0;
        for (const auto& alloc : pair.second) {
            totalLeaked += alloc.size;
        }
        
        if (totalLeaked >= leakThreshold_) {
            MemoryLeakInfo leak(pair.first, totalLeaked, pair.second.size(),
                               pair.second, getAllocationSite(pair.first));
            leaks.push_back(leak);
        }
    }
    
    return leaks;
}

std::string MemoryProfiler::getLeakReport() const {
    std::ostringstream oss;
    oss << "Memory Leak Detection Report\n";
    oss << "==========================\n\n";
    
    auto leaks = detectMemoryLeaks();
    
    if (leaks.empty()) {
        oss << "No memory leaks detected.\n";
    } else {
        oss << "Detected memory leaks: " << leaks.size() << "\n\n";
        
        for (const auto& leak : leaks) {
            oss << "Leak in context: " << leak.context << "\n";
            oss << "  Leaked size: " << leak.leakedSize << " bytes\n";
            oss << "  Allocation count: " << leak.allocationCount << "\n";
            oss << "  Suspected site: " << leak.suspectedSite << "\n";
            oss << "  First allocation: " 
                << std::chrono::duration_cast<std::chrono::seconds>(
                    leak.firstAllocation.time_since_epoch()).count() << " seconds ago\n";
            oss << "  Leak score: " << calculateLeakScore(leak) << "\n\n";
        }
    }
    
    return oss.str();
}

std::unordered_map<std::string, size_t> MemoryProfiler::getAllocationStats() const {
    std::unordered_map<std::string, size_t> stats;
    
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    for (const auto& alloc : allocations_) {
        stats[alloc.context] += alloc.size;
    }
    
    return stats;
}

std::unordered_map<std::string, size_t> MemoryProfiler::getDeallocationStats() const {
    std::unordered_map<std::string, size_t> stats;
    
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    for (const auto& dealloc : deallocations_) {
        stats[dealloc.context] += dealloc.size;
    }
    
    return stats;
}

bool MemoryProfiler::exportMemoryProfile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportMemoryProfileAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string MemoryProfiler::exportMemoryProfileAsString() const {
    std::ostringstream oss;
    oss << "Memory Profile Report\n";
    oss << "===================\n\n";
    
    oss << "Total Allocated: " << totalAllocated_ << " bytes\n";
    oss << "Total Freed: " << totalFreed_ << " bytes\n";
    oss << "Current Usage: " << currentUsage_ << " bytes\n";
    oss << "Leak Detection: " << (detectMemoryLeaks_ ? "Enabled" : "Disabled") << "\n";
    oss << "Leak Threshold: " << leakThreshold_ << " bytes\n";
    oss << "Snapshots: " << snapshots_.size() << " (max: " << maxSnapshots_ << ")\n\n";
    
    oss << "Allocations by Context:\n";
    auto allocStats = getAllocationStats();
    for (const auto& pair : allocStats) {
        oss << "  " << pair.first << ": " << pair.second << " bytes\n";
    }
    
    oss << "\nDeallocations by Context:\n";
    auto deallocStats = getDeallocationStats();
    for (const auto& pair : deallocStats) {
        oss << "  " << pair.first << ": " << pair.second << " bytes\n";
    }
    
    oss << "\nRecent Snapshots:\n";
    size_t count = std::min(static_cast<size_t>(5), snapshots_.size());
    for (size_t i = 0; i < count; ++i) {
        size_t idx = snapshots_.size() - 1 - i;
        const auto& snap = snapshots_[idx];
        oss << "  Snapshot " << idx << ": " 
            << snap.currentUsage << " bytes current, "
            << "allocated: " << snap.totalAllocated << " bytes, "
            << "freed: " << snap.totalFreed << " bytes\n";
    }
    
    oss << "\nMemory Leaks:\n";
    auto leaks = detectMemoryLeaks();
    if (leaks.empty()) {
        oss << "  No leaks detected.\n";
    } else {
        oss << "  " << leaks.size() << " leaks detected.\n";
        for (const auto& leak : leaks) {
            oss << "  - " << leak.context << ": " << leak.leakedSize << " bytes\n";
        }
    }
    
    return oss.str();
}

void MemoryProfiler::addLeakedAllocation(const MemoryAllocation& allocation) {
    std::lock_guard<std::mutex> lock(leaksMutex_);
    currentLeaks_.push_back(MemoryLeakInfo(
        allocation.context, allocation.size, 1,
        {allocation}, allocation.allocationSite));
}

void MemoryProfiler::removeLeakedAllocation(const MemoryAllocation& allocation) {
    std::lock_guard<std::mutex> lock(leaksMutex_);
    
    for (auto it = currentLeaks_.begin(); it != currentLeaks_.end(); ++it) {
        if (it->context == allocation.context &&
            it->suspectedSite == allocation.allocationSite) {
            currentLeaks_.erase(it);
            return;
        }
    }
}

void MemoryProfiler::updateStats() {
    // Update statistics from allocations and deallocations
    totalAllocated_ = 0;
    totalFreed_ = 0;
    currentUsage_ = 0;
    
    {
        std::lock_guard<std::mutex> allocLock(allocationsMutex_);
        for (const auto& alloc : allocations_) {
            totalAllocated_ += alloc.size;
            currentUsage_ += alloc.size;
        }
    }
    
    {
        std::lock_guard<std::mutex> deallocLock(deallocationsMutex_);
        for (const auto& dealloc : deallocations_) {
            totalFreed_ += dealloc.size;
            currentUsage_ -= std::min(currentUsage_, dealloc.size);
        }
    }
}

bool MemoryProfiler::isMemoryLeak(const MemoryAllocation& alloc) const {
    // Check if there's a corresponding deallocation
    std::lock_guard<std::mutex> lock(deallocationsMutex_);
    
    for (const auto& dealloc : deallocations_) {
        if (dealloc.context == alloc.context &&
            dealloc.size == alloc.size &&
            std::chrono::duration_cast<std::chrono::seconds>(
                dealloc.timestamp - alloc.timestamp).count() > 0) {
            return false;
        }
    }
    
    return true;
}

std::string MemoryProfiler::getAllocationSite(const std::string& context) const {
    // In a real implementation, this would analyze stack traces or other metadata
    return "unknown";
}

double MemoryProfiler::calculateLeakScore(const MemoryLeakInfo& leak) const {
    double score = static_cast<double>(leak.leakedSize) / leakThreshold_;
    
    // Adjust based on age
    auto age = std::chrono::system_clock::now() - leak.firstAllocation;
    double ageSeconds = std::chrono::duration_cast<std::chrono::seconds>(age).count();
    
    if (ageSeconds > 3600) { // More than 1 hour old
        score *= 1.5;
    }
    
    return score;
}

DebugVisualizer::DebugVisualizer()
    : port_(8080), host_("localhost"), enableWebInterface_(false),
      updateInterval_(0.1), serverRunning_(false) {
}

DebugVisualizer::~DebugVisualizer() {
    stopServer();
}

void DebugVisualizer::startServer() {
    if (serverRunning_ || !enableWebInterface_) return;
    
    serverRunning_ = true;
    serverThread_ = std::thread(&DebugVisualizer::monitoringLoop, this);
    
    Logger::getGlobal().info("Debug visualizer server started on " + host_ + ":" + std::to_string(port_));
}

void DebugVisualizer::stopServer() {
    if (!serverRunning_) return;
    
    serverRunning_ = false;
    
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

void DebugVisualizer::monitoringLoop() {
    while (serverRunning_) {
        // Update visualization data
        // In a real implementation, this would update websocket connections
        std::this_thread::sleep_for(std::chrono::duration<double>(updateInterval_));
    }
}

std::string DebugVisualizer::generateDashboardHTML() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>NLM Debug Dashboard</h1>\n";
    oss << "<div class='dashboard'>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Trace Events</h2>\n";
    oss << "<div id='trace-events'>" << (debuggingSystem_ ? std::to_string(debuggingSystem_->getTrace()->getEventCount()) : "0") << " events</div>\n";
    oss << "</div>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Performance</h2>\n";
    oss << "<div id='performance-metrics'>TBD</div>\n";
    oss << "</div>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Memory</h2>\n";
    oss << "<div id='memory-usage'>TBD</div>\n";
    oss << "</div>\n";
    oss << "<div class='card'>\n";
    oss << "<h2>Events</h2>\n";
    oss << "<div id='event-count'>TBD</div>\n";
    oss << "</div>\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generateTraceView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Trace Events</h1>\n";
    oss << "<div id='trace-container'>\n";
    oss << "<!-- Trace events will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generatePerformanceView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Performance Metrics</h1>\n";
    oss << "<div id='performance-container'>\n";
    oss << "<!-- Performance metrics will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generateMemoryView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Memory Usage</h1>\n";
    oss << "<div id='memory-container'>\n";
    oss << "<!-- Memory usage will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::generateEventView() const {
    std::ostringstream oss;
    oss << generateHTMLHeader();
    oss << "<h1>Event Log</h1>\n";
    oss << "<div id='event-container'>\n";
    oss << "<!-- Events will be displayed here -->\n";
    oss << "</div>\n";
    oss << generateHTMLFooter();
    return oss.str();
}

std::string DebugVisualizer::getAPIDebugInfo() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"status\": \"active\",\n";
    oss << "  \"traceEvents\": " << (debuggingSystem_ && debuggingSystem_->getTrace() ? debuggingSystem_->getTrace()->getEventCount() : 0) << ",\n";
    oss << "  \"version\": \"1.0.0\"\n";
    oss << "}\n";
    return oss.str();
}

std::string DebugVisualizer::getAPIPerformanceData() const {
    std::ostringstream oss;
    oss << "[\n";
    // In a real implementation, this would return actual performance data
    oss << "  {\"timestamp\": \"2026-01-01T00:00:00Z\", \"cpu\": 50.0, \"memory\": 25.0}\n";
    oss << "]\n";
    return oss.str();
}

std::string DebugVisualizer::getAPIMemoryData() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"currentUsage\": 0,\n";
    oss << "  \"totalAllocated\": 0,\n";
    oss << "  \"totalFreed\": 0\n";
    oss << "}\n";
    return oss.str();
}

std::string DebugVisualizer::getAPIEventsData() const {
    std::ostringstream oss;
    oss << "[]\n";
    return oss.str();
}

bool DebugVisualizer::exportVisualization(const std::string& directory) const {
    try {
        std::ofstream file(directory + "/dashboard.html");
        if (!file.is_open()) {
            return false;
        }
        
        file << generateDashboardHTML();
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

std::string DebugVisualizer::generateHTMLHeader() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "<title>NLM Debug Visualization</title>\n";
    oss << "<style>\n";
    oss << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    oss << ".dashboard { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; }\n";
    oss << ".card { border: 1px solid #ddd; padding: 20px; border-radius: 5px; background: #f9f9f9; }\n";
    oss << "h1 { color: #333; }\n";
    oss << "h2 { color: #555; }\n";
    oss << "</style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    return oss.str();
}

std::string DebugVisualizer::generateHTMLFooter() const {
    std::ostringstream oss;
    oss << "</body>\n";
    oss << "</html>\n";
    return oss.str();
}

std::string DebugVisualizer::generateEventChart(const std::vector<SimulationEvent>& events) const {
    return "<!-- Event chart would go here -->\n";
}

std::string DebugVisualizer::generateMemoryChart(const std::vector<MemorySnapshot>& snapshots) const {
    return "<!-- Memory chart would go here -->\n";
}

std::string DebugVisualizer::generatePerformanceChart(const std::vector<std::string>& operations) const {
    return "<!-- Performance chart would go here -->\n";
}

std::string DebugVisualizer::escapeHTML(const std::string& text) const {
    std::ostringstream oss;
    for (char c : text) {
        switch (c) {
            case '&': oss << "&amp;"; break;
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&#x27;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

} // namespace nlm

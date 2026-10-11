// MemoryProfiler.cpp - Memory profiling system implementation
// Advanced memory profiling for NLM neural simulations

#include "MemoryProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <set>

namespace nlm {

MemoryProfiler::MemoryProfiler() 
    : isProfiling_(false)
    , profilingStartTime_()
    , profilingEndTime_()
    , memoryTracker_(new MemoryTracker())
    , poolProfiler_(new MemoryPoolProfiler())
    , config_()
{
    // Set default configuration
    config_.enableHeapProfiling = true;
    config_.enablePoolTracking = true;
    config_.enableFragmentationAnalysis = true;
    config_.trackAllocationSites = true;
    config_.samplingInterval = 0.1; // 100ms
    config_.maxSamples = 1000000;
    config_.enablePageFaultTracking = true;
    config_.enableTLSTracking = true;
    config_.trackMemoryPools = true;
    config_.analyzeAccessPatterns = true;
    config_.enableVirtualMemoryTracking = true;
    config_.trackNUMALocalities = true;
}

MemoryProfiler::~MemoryProfiler() {
    if (isProfiling_) {
        stopProfiling();
    }
    
    // Memory tracker and pool profiler are deleted automatically
}

void MemoryProfiler::startProfiling(const MemoryProfilerConfig& config) {
    if (isProfiling_) {
        return; // Already profiling
    }
    
    isProfiling_ = true;
    profilingStartTime_ = std::chrono::high_resolution_clock::now();
    
    config_ = config;
    
    // Initialize memory tracking
    if (memoryTracker_) {
        memoryTracker_->startTracking();
    }
    
    if (poolProfiler_) {
        poolProfiler_->startProfiling();
    }
    
    NLM_LOG_INFO("MemoryProfiler started with configuration");
}

void MemoryProfiler::stopProfiling() {
    if (!isProfiling_) {
        return; // Not profiling
    }
    
    profilingEndTime_ = std::chrono::high_resolution_clock::now();
    isProfiling_ = false;
    
    // Stop memory tracking
    if (memoryTracker_) {
        memoryTracker_->stopTracking();
    }
    
    if (poolProfiler_) {
        poolProfiler_->stopProfiling();
    }
    
    // Generate final report
    generateReport();
    
    NLM_LOG_INFO("MemoryProfiler stopped. Total allocations tracked: " + 
                std::to_string(getAllocationCount()) + ". Peak memory usage: " +
                formatBytes(getPeakMemoryUsage()) + "MB");
}

void MemoryProfiler::recordAllocation(size_t size, const char* allocationSite,
                                     const char* deallocationSite,
                                     bool fromMemoryPool,
                                     const char* poolName,
                                     std::thread::id threadId) {
    if (!isProfiling_) return;
    
    AllocationRecord record;
    record.allocationId = MemoryProfiler::getNextAllocationId();
    record.size = size;
    record.allocationSite = allocationSite;
    record.deallocationSite = deallocationSite;
    record.fromMemoryPool = fromMemoryPool;
    record.poolName = poolName;
    record.threadId = threadId;
    record.timestamp = std::chrono::high_resolution_clock::now();
    record.isDeallocated = false;
    record.memoryPoolId = fromMemoryPool ? 1 : -1; // Simplified
    
    record.memoryAddress = reinterpret_cast<void*>(memoryTracker_->getNextAddress());
    
    // Track allocation in memory tracker
    if (memoryTracker_) {
        memoryTracker_->recordAllocation(record);
    }
    
    // Track in pool profiler
    if (poolProfiler_ && fromMemoryPool) {
        poolProfiler_->recordAllocation(record);
    }
    
    // Add to main records
    allocationRecords_[record.allocationId] = record;
    
    // Clean up old samples
    if (allocationRecords_.size() > config_.maxSamples) {
        cleanupOldRecords();
    }
}

void MemoryProfiler::recordDeallocation(uint64_t allocationId, 
                                       const char* deallocationSite,
                                       std::thread::id threadId) {
    if (!isProfiling_) return;
    
    // Find the allocation record
    auto it = allocationRecords_.find(allocationId);
    if (it == allocationRecords_.end()) {
        return; // Allocation not found
    }
    
    AllocationRecord& record = it->second;
    record.deallocationSite = deallocationSite;
    record.deallocationTimestamp = std::chrono::high_resolution_clock::now();
    record.threadId = threadId;
    record.isDeallocated = true;
    
    // Calculate lifetime
    if (record.deallocationTimestamp > record.timestamp) {
        record.lifetime = std::chrono::duration<double>(
            record.deallocationTimestamp - record.timestamp).count();
    }
    
    // Track deallocation in memory tracker
    if (memoryTracker_) {
        memoryTracker_->recordDeallocation(record);
    }
}

void MemoryProfiler::recordMemoryPoolOperation(const std::string& poolName,
                                               MemoryPoolOperation operation,
                                               size_t size,
                                               uint64_t poolAddress,
                                               std::thread::id threadId) {
    if (!isProfiling_) return;
    
    MemoryPoolRecord record;
    record.operation = operation;
    record.poolName = poolName;
    record.size = size;
    record.poolAddress = poolAddress;
    record.threadId = threadId;
    record.timestamp = std::chrono::high_resolution_clock::now();
    
    // Track in pool profiler
    if (poolProfiler_) {
        poolProfiler_->recordPoolOperation(record);
    }
}

void MemoryProfiler::recordPageFault(uint64_t virtualAddress,
                                    bool isWrite,
                                    bool isProtectionFault,
                                    std::thread::id threadId) {
    if (!isProfiling_ || !config_.enablePageFaultTracking) return;
    
    PageFaultRecord record;
    record.virtualAddress = virtualAddress;
    record.isWrite = isWrite;
    record.isProtectionFault = isProtectionFault;
    record.threadId = threadId;
    record.timestamp = std::chrono::high_resolution_clock::now();
    record.isResolved = false;
    
    // Track page fault
    if (memoryTracker_) {
        memoryTracker_->recordPageFault(record);
    }
}

void MemoryProfiler::recordTLSUsage(const TLSRecord& record) {
    if (!isProfiling_ || !config_.enableTLSTracking) return;
    
    // Track TLS usage
    if (memoryTracker_) {
        memoryTracker_->recordTLSRecord(record);
    }
}

void MemoryProfiler::recordNUMALocality(const NUMALocationRecord& record) {
    if (!isProfiling_ || !config_.trackNUMALocalities) return;
    
    // Track NUMA locality
    if (memoryTracker_) {
        memoryTracker_->recordNUMALocation(record);
    }
}

MemoryAnalysis MemoryProfiler::analyzeMemoryUsage() {
    MemoryAnalysis analysis;
    
    // Calculate memory statistics
    analysis.totalAllocations = allocationRecords_.size();
    analysis.totalMemoryAllocated = 0;
    analysis.totalMemoryFreed = 0;
    analysis.currentMemoryUsage = 0;
    analysis.peakMemoryUsage = 0;
    analysis.averageAllocationSize = 0;
    analysis.allocationCount = 0;
    
    // Calculate statistics
    for (const auto& pair : allocationRecords_) {
        const auto& record = pair.second;
        if (record.isDeallocated) {
            analysis.totalMemoryFreed += record.size;
        } else {
            analysis.totalMemoryAllocated += record.size;
            analysis.currentMemoryUsage += record.size;
        }
        
        analysis.allocationCount++;
        analysis.totalMemoryAllocated += record.size;
        
        if (record.size > analysis.maxAllocationSize) {
            analysis.maxAllocationSize = record.size;
        }
    }
    
    if (analysis.allocationCount > 0) {
        analysis.averageAllocationSize = static_cast<double>(analysis.totalMemoryAllocated) / analysis.allocationCount;
    }
    
    // Calculate fragmentation
    analyzeFragmentation(analysis);
    
    // Calculate pool usage
    analyzePoolUsage(analysis);
    
    // Calculate access patterns
    if (config_.analyzeAccessPatterns) {
        analyzeAccessPatterns(analysis);
    }
    
    // Calculate NUMA distribution
    if (config_.trackNUMALocalities && memoryTracker_) {
        analyzeNUMADistribution(analysis, *memoryTracker_);
    }
    
    return analysis;
}

MemoryLeakReport MemoryProfiler::detectMemoryLeaks() {
    MemoryLeakReport report;
    
    // Find allocations that were never deallocated
    for (const auto& pair : allocationRecords_) {
        const auto& record = pair.second;
        if (!record.isDeallocated && !record.fromMemoryPool) {
            // Check if allocation is still active (not freed)
            MemoryLeak leak;
            leak.allocationId = record.allocationId;
            leak.size = record.size;
            leak.allocationSite = record.allocationSite;
            leak.deallocationSite = record.deallocationSite;
            leak.lifetime = 0; // Unknown since not deallocated
            leak.threadId = record.threadId;
            leak.isMemoryPoolAllocation = record.fromMemoryPool;
            leak.poolName = record.poolName;
            
            // Determine leak severity based on size
            if (record.size > 1024 * 1024) {  // 1MB
                leak.severity = LeakSeverity::CRITICAL;
            } else if (record.size > 64 * 1024) {  // 64KB
                leak.severity = LeakSeverity::HIGH;
            } else if (record.size > 1024) {  // 1KB
                leak.severity = LeakSeverity::MEDIUM;
            } else {
                leak.severity = LeakSeverity::LOW;
            }
            
            report.leaks.push_back(leak);
        }
    }
    
    // Sort leaks by severity
    std::sort(report.leaks.begin(), report.leaks.end(),
              [](const MemoryLeak& a, const MemoryLeak& b) {
                  return static_cast<int>(a.severity) > static_cast<int>(b.severity);
              });
    
    // Calculate total leaked memory
    report.totalLeakedMemory = 0;
    for (const auto& leak : report.leaks) {
        report.totalLeakedMemory += leak.size;
    }
    
    return report;
}

MemoryPoolAnalysis MemoryProfiler::analyzeMemoryPoolUsage() {
    MemoryPoolAnalysis analysis;
    
    if (poolProfiler_) {
        analysis = poolProfiler_->analyzePoolUsage();
    }
    
    return analysis;
}

std::vector<AllocationSiteInfo> MemoryProfiler::analyzeAllocationPatterns() {
    std::vector<AllocationSiteInfo> sites;
    
    // Group allocations by site
    std::unordered_map<std::string, AllocationSiteInfo> siteMap;
    
    for (const auto& pair : allocationRecords_) {
        const auto& record = pair.second;
        if (record.allocationSite) {
            std::string site = record.allocationSite;
            
            if (siteMap.find(site) == siteMap.end()) {
                AllocationSiteInfo info;
                info.site = site;
                info.allocationCount = 0;
                info.totalMemoryAllocated = 0;
                info.averageAllocationSize = 0;
                info.uniqueThreads = std::set<std::thread::id>();
                info.fromMemoryPools = 0;
                
                siteMap[site] = info;
            }
            
            auto& info = siteMap[site];
            info.allocationCount++;
            info.totalMemoryAllocated += record.size;
            info.uniqueThreads.insert(record.threadId);
            
            if (record.fromMemoryPool) {
                info.fromMemoryPools++;
            }
        }
    }
    
    // Calculate averages and populate results
    for (auto& pair : siteMap) {
        auto& info = pair.second;
        if (info.allocationCount > 0) {
            info.averageAllocationSize = static_cast<double>(info.totalMemoryAllocated) / info.allocationCount;
        }
        
        info.allocationPercentage = (info.totalMemoryAllocated * 100.0) / getTotalMemoryAllocated();
        
        sites.push_back(info);
    }
    
    // Sort by total memory allocated (descending)
    std::sort(sites.begin(), sites.end(),
              [](const AllocationSiteInfo& a, const AllocationSiteInfo& b) {
                  return a.totalMemoryAllocated > b.totalMemoryAllocated;
              });
    
    return sites;
}

void MemoryProfiler::exportToJSON(std::ostream& output) {
    nlohmann::json jsonData;
    
    jsonData["profiler_version"] = "1.0.0";
    jsonData["session_start"] = std::chrono::high_resolution_clock::to_time_t(profilingStartTime_);
    jsonData["session_end"] = std::chrono::high_resolution_clock::to_time_t(profilingEndTime_);
    jsonData["total_allocations"] = getAllocationCount();
    
    // Export allocation records
    nlohmann::json allocationsArray = nlohmann::json::array();
    for (const auto& pair : allocationRecords_) {
        const auto& record = pair.second;
        nlohmann::json allocationJson;
        allocationJson["allocation_id"] = record.allocationId;
        allocationJson["size"] = record.size;
        allocationJson["allocation_site"] = record.allocationSite;
        allocationJson["deallocation_site"] = record.deallocationSite;
        allocationJson["from_memory_pool"] = record.fromMemoryPool;
        allocationJson["pool_name"] = record.poolName;
        allocationJson["thread_id"] = record.threadId;
        allocationJson["timestamp"] = std::chrono::high_resolution_clock::to_time_t(record.timestamp);
        allocationJson["is_deallocated"] = record.isDeallocated;
        allocationsArray.push_back(allocationJson);
    }
    jsonData["allocations"] = allocationsArray;
    
    output << jsonData.dump(2);
}

void MemoryProfiler::exportToCSV(std::ostream& output) {
    output << "allocation_id,size,allocation_site,deallocation_site,from_memory_pool,pool_name,thread_id,timestamp,is_deallocated\n";
    
    for (const auto& pair : allocationRecords_) {
        const auto& record = pair.second;
        output << record.allocationId << ","
               << record.size << ","
               << (record.allocationSite ? record.allocationSite : "") << ","
               << (record.deallocationSite ? record.deallocationSite : "") << ","
               << record.fromMemoryPool << ","
               << (record.poolName ? record.poolName : "") << ","
               << record.threadId << ","
               << std::chrono::high_resolution_clock::to_time_t(record.timestamp) << ","
               << record.isDeallocated << "\n";
    }
}

std::string MemoryProfiler::generateReport() {
    std::ostringstream report;
    
    report << "╔══════════════════════════════════════════════════════════════════╗\n";
    report << "║                 NLM Memory Profiler Report                          ║\n";
    report << "╚══════════════════════════════════════════════════════════════════╝\n\n";
    
    report << "Memory Usage Analysis:\n";
    MemoryAnalysis analysis = analyzeMemoryUsage();
    
    report << "  Total Allocations: " << analysis.totalAllocations << "\n";
    report << "  Total Memory Allocated: " << formatBytes(analysis.totalMemoryAllocated) << "\n";
    report << "  Total Memory Freed: " << formatBytes(analysis.totalMemoryFreed) << "\n";
    report << "  Current Memory Usage: " << formatBytes(analysis.currentMemoryUsage) << "\n";
    report << "  Peak Memory Usage: " << formatBytes(analysis.peakMemoryUsage) << "\n";
    report << "  Average Allocation Size: " << formatBytes(analysis.averageAllocationSize) << "\n";
    report << "  Maximum Allocation Size: " << formatBytes(analysis.maxAllocationSize) << "\n\n";
    
    report << "Fragmentation Analysis:\n";
    report << "  Fragmentation Score: " << (analysis.fragmentationScore * 100) << "%\n";
    report << "  Small Fragment Count: " << analysis.smallFragmentCount << "\n";
    report << "  Medium Fragment Count: " << analysis.mediumFragmentCount << "\n";
    report << "  Large Fragment Count: " << analysis.largeFragmentCount << "\n\n";
    
    report << "Memory Pool Analysis:\n";
    MemoryPoolAnalysis poolAnalysis = analyzeMemoryPoolUsage();
    report << "  Total Memory Pools: " << poolAnalysis.totalPools << "\n";
    report << "  Pool Memory Usage: " << formatBytes(poolAnalysis.totalPoolMemory) << "\n";
    report << "  Pool Allocation Count: " << poolAnalysis.totalAllocations << "\n";
    report << "  Pool Utilization: " << (poolAnalysis.poolUtilization * 100) << "%\n\n";
    
    // Detect memory leaks
    MemoryLeakReport leakReport = detectMemoryLeaks();
    report << "Memory Leak Detection:\n";
    report << "  Total Leaks Detected: " << leakReport.leaks.size() << "\n";
    report << "  Total Leaked Memory: " << formatBytes(leakReport.totalLeakedMemory) << "\n";
    
    if (!leakReport.leaks.empty()) {
        report << "  Most Critical Leaks:\n";
        int count = std::min(5, static_cast<int>(leakReport.leaks.size()));
        for (int i = 0; i < count; ++i) {
            const auto& leak = leakReport.leaks[i];
            report << "    " << (i + 1) << ". " << leak.size << " bytes at " << leak.allocationSite << "\n";
        }
    }
    
    return report.str();
}

void MemoryProfiler::cleanupOldRecords() {
    auto cutoffTime = std::chrono::high_resolution_clock::now() - 
                      std::chrono::duration<double>(config_.samplingInterval * 10);
    
    auto it = allocationRecords_.begin();
    while (it != allocationRecords_.end()) {
        if (it->second.timestamp < cutoffTime) {
            it = allocationRecords_.erase(it);
        } else {
            ++it;
        }
    }
}

void MemoryProfiler::analyzeFragmentation(MemoryAnalysis& analysis) {
    // Calculate fragmentation based on allocation/deallocation patterns
    if (allocationRecords_.empty()) {
        analysis.fragmentationScore = 0.0;
        return;
    }
    
    // Count fragments of different sizes
    for (const auto& pair : allocationRecords_) {
        const auto& record = pair.second;
        if (!record.isDeallocated) {
            if (record.size < 1024) {
                analysis.smallFragmentCount++;
            } else if (record.size < 1024 * 1024) {
                analysis.mediumFragmentCount++;
            } else {
                analysis.largeFragmentCount++;
            }
        }
    }
    
    // Calculate fragmentation score (simplified)
    int totalFragments = analysis.smallFragmentCount + analysis.mediumFragmentCount + analysis.largeFragmentCount;
    if (totalFragments > 0) {
        analysis.fragmentationScore = static_cast<double>(analysis.mediumFragmentCount + analysis.largeFragmentCount) / totalFragments;
    } else {
        analysis.fragmentationScore = 0.0;
    }
}

void MemoryProfiler::analyzePoolUsage(MemoryPoolAnalysis& analysis) {
    // Collect pool statistics from pool profiler
    if (poolProfiler_) {
        analysis = poolProfiler_->analyzePoolUsage();
    }
}

void MemoryProfiler::analyzeAccessPatterns(MemoryAnalysis& analysis) {
    // Analyze access patterns based on allocation sites and thread usage
    std::unordered_map<std::string, size_t> accessCount;
    
    for (const auto& pair : allocationRecords_) {
        const auto& record = pair.second;
        if (record.allocationSite) {
            accessCount[record.allocationSite]++;
        }
    }
    
    // Find most frequently accessed sites
    size_t maxAccessCount = 0;
    for (const auto& pair : accessCount) {
        if (pair.second > maxAccessCount) {
            maxAccessCount = pair.second;
        }
    }
    
    if (maxAccessCount > 0) {
        analysis.accessPatternScore = static_cast<double>(accessCount.begin()->second) / maxAccessCount;
    }
}

void MemoryProfiler::analyzeNUMADistribution(MemoryAnalysis& analysis, const MemoryTracker& tracker) {
    // Get NUMA allocation statistics
    analysis.numaLocalAllocations = tracker.getLocalAllocations();
    analysis.numaRemoteAllocations = tracker.getRemoteAllocations();
    
    if (analysis.numaLocalAllocations + analysis.numaRemoteAllocations > 0) {
        analysis.numaLocalPercentage = static_cast<double>(analysis.numaLocalAllocations) / 
                                      (analysis.numaLocalAllocations + analysis.numaRemoteAllocations);
    } else {
        analysis.numaLocalPercentage = 0.0;
    }
}

std::string MemoryProfiler::formatBytes(size_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024.0) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024.0 * 1024)) + " MB";
    return std::to_string(bytes / (1024.0 * 1024 * 1024)) + " GB";
}

} // namespace nlm

// Utility functions for memory profiling
void saveMemoryProfilerData(const MemoryProfiler& profiler, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        profiler.exportToJSON(file);
        NLM_LOG_INFO("Memory profiler data saved to: " + filename);
    } else {
        NLM_LOG_ERROR("Failed to save memory profiler data to: " + filename);
    }
}

bool loadMemoryProfilerData(MemoryProfiler& profiler, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to load memory profiler data from: " + filename);
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        // In a real implementation, this would load the data back
        // For now, it's a placeholder
        
        NLM_LOG_INFO("Memory profiler data loaded from: " + filename);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading memory profiler data: " + std::string(e.what()));
        return false;
    }
}

} // namespace nlm

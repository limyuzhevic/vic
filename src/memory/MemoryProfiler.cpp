#include "MemoryProfiler.hpp"
#include "../src/performance/MemoryPool.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/performance/PerformanceMonitor.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <numeric>
#include <ctime>

namespace nlm {

MemoryProfiler::MemoryProfiler()
    : enabled_(true)
    , initialized_(false)
    , profileType_(ProfileType::AllocationProfile)
    , profileInterval_(1.0)
    , memoryTracker_(nullptr)
    , memoryDebug_(nullptr)
    , brain_(nullptr)
    , performanceMonitor_(nullptr)
    , totalAccesses_(0)
    , totalAllocations_(0)
    , totalDeallocations_(0)
    , totalBytesAccessed_(0)
    , totalBytesAllocated_(0)
    , totalBytesDeallocated_(0)
    , currentStep_(0)
    , currentTime_(0.0) {
    
    // Initialize memory pressure
    pressure_.currentPressure = 0.0;
    pressure_.pressureThreshold = 0.8;
    pressure_.availableMemory = 0;
    pressure_.totalMemory = 0;
    pressure_.pressureIncreaseRate = 0.0;
    pressure_.isCritical = false;
    
    // Initialize current profile
    startNewProfile("Default Profile");
    
    initialized_ = true;
}

MemoryProfiler::~MemoryProfiler() {
    flush();
}

MemoryProfiler& MemoryProfiler::getInstance() {
    static MemoryProfiler instance;
    return instance;
}

void MemoryProfiler::enable(bool enable) {
    enabled_ = enable;
}

void MemoryProfiler::recordAccess(size_t address, MemoryLevel level, uint32_t threadId, bool isRead,
                                 bool isInstruction, double latency, size_t size, const std::string& callSite) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Record memory access
    MemoryAccessInfo access;
    access.timestamp = getCurrentTime();
    access.address = address;
    access.level = level;
    access.threadId = threadId;
    access.isRead = isRead;
    access.isInstruction = isInstruction;
    access.latency = latency;
    access.size = size;
    access.callSite = callSite;
    
    currentProfile_.accesses.push_back(access);
    memoryAccesses_.push_back(access);
    
    totalAccesses_++;
    totalBytesAccessed_ += size;
    
    // Update memory hierarchy stats
    for (auto& stats : hierarchyStats_) {
        if (stats.level == level) {
            if (isInCache(address, level)) {
                stats.hitCount++;
            } else {
                stats.missCount++;
            }
            break;
        }
    }
    
    // Update memory pressure
    updateMemoryPressure();
}

void MemoryProfiler::recordAllocation(size_t address, size_t size, const std::string& site,
                                     const std::string& stackTrace, uint32_t threadId) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Record allocation
    AllocationProfile alloc;
    alloc.timestamp = getCurrentTime();
    alloc.address = address;
    alloc.size = size;
    alloc.allocationSite = site;
    alloc.stackTrace = stackTrace;
    alloc.threadId = threadId;
    alloc.futureAccesses.clear();
    alloc.lifetime = 0.0;
    alloc.expectedLevel = MemoryLevel::RAM;
    alloc.isPoolAllocation = false;
    alloc.poolName = "";
    
    currentProfile_.allocations.push_back(alloc);
    allocations_.push_back(alloc);
    
    totalAllocations_++;
    totalBytesAllocated_ += size;
}

void MemoryProfiler::recordDeallocation(size_t address, const std::string& site, uint32_t threadId) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Record deallocation
    DeallocationProfile dealloc;
    dealloc.timestamp = getCurrentTime();
    dealloc.address = address;
    dealloc.deallocationSite = site;
    dealloc.threadId = threadId;
    
    // Find related allocation
    for (const auto& alloc : allocations_) {
        if (alloc.address == address) {
            dealloc.allocationInfo = alloc;
            break;
        }
    }
    
    currentProfile_.deallocations.push_back(dealloc);
    deallocations_.push_back(dealloc);
    
    totalDeallocations_++;
    
    // Update allocation lifetime
    for (auto& alloc : currentProfile_.allocations) {
        if (alloc.address == address) {
            alloc.lifetime = calculateTimeSince(alloc.timestamp);
            break;
        }
    }
}

void MemoryProfiler::recordPoolUsage(const std::string& poolName, size_t allocated, size_t available) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Find or create pool profile
    MemoryPoolProfile* poolProfile = nullptr;
    for (auto& pool : currentProfile_.poolProfiles) {
        if (pool.name == poolName) {
            poolProfile = &pool;
            break;
        }
    }
    
    if (!poolProfile) {
        MemoryPoolProfile newPool;
        newPool.name = poolName;
        newPool.totalCapacity = allocated + available;
        newPool.currentUsage = allocated;
        newPool.peakUsage = allocated;
        newPool.allocationCount = 0;
        newPool.deallocationCount = 0;
        newPool.timeSeries.clear();
        newPool.tuningRecommendations = "";
        
        currentProfile_.poolProfiles.push_back(newPool);
        poolProfiles_.push_back(newPool);
        poolProfile = &currentProfile_.poolProfiles.back();
    } else {
        poolProfile->currentUsage = allocated;
        poolProfile->peakUsage = std::max(poolProfile->peakUsage, allocated);
    }
    
    // Record time series data
    poolProfile->timeSeries.push_back(static_cast<double>(allocated));
    if (poolProfile->timeSeries.size() > 100) {
        poolProfile->timeSeries.erase(poolProfile->timeSeries.begin());
    }
}

void MemoryProfiler::recordPoolAllocation(const std::string& poolName, size_t size) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Find pool profile
    for (auto& pool : currentProfile_.poolProfiles) {
        if (pool.name == poolName) {
            pool.allocationCount++;
            pool.blockSizes.push_back(size);
            if (pool.blockSizes.size() > 100) {
                pool.blockSizes.erase(pool.blockSizes.begin());
            }
            break;
        }
    }
}

void MemoryProfiler::recordPoolDeallocation(const std::string& poolName, size_t size) {
    if (!enabled_) return;
    
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    // Find pool profile
    for (auto& pool : currentProfile_.poolProfiles) {
        if (pool.name == poolName) {
            pool.deallocationCount++;
            break;
        }
    }
}

void MemoryProfiler::updateMemoryHierarchy(const std::vector<MemoryHierarchyStats>& stats) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    hierarchyStats_ = stats;
    
    // Update current profile
    currentProfile_.hierarchyStats = stats;
}

void MemoryProfiler::updateMemoryPressure(const MemoryPressure& pressure) {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    pressure_ = pressure;
    
    // Update current profile
    currentProfile_.pressure = pressure;
    
    // Check for alerts
    checkForPressureAlerts();
}

void MemoryProfiler::startProfile(const std::string& sessionName) {
    std::lock_guard<std::mutex> lock(profileSessionMutex_);
    
    finalizeCurrentProfile();
    startNewProfile(sessionName);
}

void MemoryProfiler::endProfile(const std::string& sessionName) {
    std::lock_guard<std::mutex> lock(profileSessionMutex_);
    
    finalizeCurrentProfile();
}

ProfileSession MemoryProfiler::getCurrentProfile() const {
    return currentProfile_;
}

std::vector<ProfileSession> MemoryProfiler::getProfileHistory() const {
    return profileHistory_;
}

std::vector<MemoryOptimization> MemoryProfiler::getOptimizationRecommendations() const {
    std::vector<MemoryOptimization> recommendations;
    
    // Analyze memory usage and generate recommendations
    analyzeCurrentUsage();
    analyzeCurrentAccesses();
    analyzeCurrentPools();
    
    // Generate optimization recommendations
    generateMemoryOptimizationRecommendations(recommendations);
    generatePerformanceOptimizationRecommendations(recommendations);
    generateCacheOptimizationRecommendations(recommendations);
    generatePoolOptimizationRecommendations(recommendations);
    generateAccessOptimizationRecommendations(recommendations);
    
    return recommendations;
}

std::string MemoryProfiler::analyzeMemoryUsage() const {
    std::stringstream report;
    
    report << "=== Memory Usage Analysis ===" << std::endl;
    
    size_t workingSet = calculateWorkingSet();
    double fragmentationIndex = calculateFragmentationIndex();
    double localityScore = calculateLocalityScore();
    
    report << "Working Set Size: " << workingSet << " bytes" << std::endl;
    report << "Fragmentation Index: " << std::fixed << std::setprecision(2) << fragmentationIndex << std::endl;
    report << "Locality Score: " << std::fixed << std::setprecision(2) << localityScore << std::endl;
    report << "Total Allocations: " << totalAllocations_ << std::endl;
    report << "Total Deallocations: " << totalDeallocations_ << std::endl;
    report << "Allocation Efficiency: " << std::fixed << std::setprecision(2)
           << (totalDeallocations_ > 0 ? (static_cast<double>(totalBytesDeallocated_) / totalBytesAllocated_) : 0.0)
           << std::endl;
    
    return report.str();
}

std::string MemoryProfiler::analyzeAccessPatterns() const {
    std::stringstream report;
    
    report << "=== Access Pattern Analysis ===" << std::endl;
    
    // Analyze temporal patterns
    auto temporalPatterns = analyzeTemporalPatterns();
    report << "Temporal Patterns:" << std::endl;
    for (const auto& pair : temporalPatterns) {
        report << "  Access Size " << pair.first << ": " << std::fixed << std::setprecision(2)
               << pair.second << " accesses per time unit" << std::endl;
    }
    
    // Analyze spatial patterns
    auto spatialPatterns = analyzeSpatialPatterns();
    report << "Spatial Patterns:" << std::endl;
    for (const auto& pair : spatialPatterns) {
        report << "  Working Set Size " << pair.first << ": " << std::fixed << std::setprecision(2)
               << pair.second << "%% of total accesses" << std::endl;
    }
    
    // Find hotspots and cold spots
    auto hotspots = findHotSpots();
    auto coldspots = findColdSpots();
    
    report << "Hot Spots (frequently accessed addresses):" << std::endl;
    for (size_t i = 0; i < std::min(size_t(5), hotspots.size()); ++i) {
        report << "  " << i + 1 << ". 0x" << std::hex << hotspots[i] << std::dec << std::endl;
    }
    
    report << "Cold Spots (seldom accessed addresses):" << std::endl;
    for (size_t i = 0; i < std::min(size_t(5), coldspots.size()); ++i) {
        report << "  " << i + 1 << ". 0x" << std::hex << coldspots[i] << std::dec << std::endl;
    }
    
    return report.str();
}

std::string MemoryProfiler::analyzePoolEfficiency() const {
    std::stringstream report;
    
    report << "=== Pool Efficiency Analysis ===" << std::endl;
    
    for (const auto& pool : poolProfiles_) {
        double efficiency = calculatePoolEfficiency(pool);
        
        report << "Pool: " << pool.name << std::endl;
        report << "  Total Capacity: " << pool.totalCapacity << " bytes" << std::endl;
        report << "  Current Usage: " << pool.currentUsage << " bytes" << std::endl;
        report << "  Peak Usage: " << pool.peakUsage << " bytes" << std::endl;
        report << "  Allocation Count: " << pool.allocationCount << std::endl;
        report << "  Deallocation Count: " << pool.deallocationCount << std::endl;
        report << "  Efficiency: " << std::fixed << std::setprecision(2) << efficiency << std::endl;
        report << "  Average Block Size: " << std::fixed << std::setprecision(2)
               << (pool.allocationCount > 0 ? (static_cast<double>(pool.blockSizes.size()) / pool.allocationCount) : 0.0)
               << " bytes" << std::endl;
        
        if (!pool.tuningRecommendations.empty()) {
            report << "  Recommendations: " << pool.tuningRecommendations << std::endl;
        }
        report << std::endl;
    }
    
    return report.str();
}

std::string MemoryProfiler::analyzeMemoryHierarchy() const {
    std::stringstream report;
    
    report << "=== Memory Hierarchy Analysis ===" << std::endl;
    
    for (const auto& stats : hierarchyStats_) {
        double hitRate = stats.hitCount + stats.missCount > 0 ?
            static_cast<double>(stats.hitCount) / (stats.hitCount + stats.missCount) : 0.0;
        
        report << "Level: " << getMemoryLevelString(stats.level) << std::endl;
        report << "  Total Size: " << stats.totalSize << " bytes" << std::endl;
        report << "  Used Size: " << stats.usedSize << " bytes" << std::endl;
        report << "  Available Size: " << stats.availableSize << " bytes" << std::endl;
        report << "  Hit Rate: " << std::fixed << std::setprecision(2) << hitRate * 100.0 << "%" << std::endl;
        report << "  Average Latency: " << std::fixed << std::setprecision(2) << stats.averageLatency << " ns" << std::endl;
        
        // Show size distribution
        if (!stats.sizeDistribution.empty()) {
            report << "  Size Distribution:" << std::endl;
            size_t totalAccesses = 0;
            for (const auto& pair : stats.sizeDistribution) {
                totalAccesses += pair.second;
            }
            
            size_t displayCount = 0;
            for (const auto& pair : stats.sizeDistribution) {
                if (displayCount >= 5) break;
                double percentage = totalAccesses > 0 ? 
                    static_cast<double>(pair.second) / totalAccesses * 100.0 : 0.0;
                report << "    Size " << pair.first << " bytes: " << percentage << "%" << std::endl;
                displayCount++;
            }
        }
        report << std::endl;
    }
    
    return report.str();
}

std::string MemoryProfiler::generateProfileReport() const {
    std::stringstream report;
    
    report << generateProfileSummary() << std::endl;
    report << analyzeMemoryUsage() << std::endl;
    report << analyzeAccessPatterns() << std::endl;
    report << analyzePoolEfficiency() << std::endl;
    report << analyzeMemoryHierarchy() << std::endl;
    report << generateOptimizationAnalysis() << std::endl;
    
    return report.str();
}

void MemoryProfiler::analyzeCurrentUsage() const {
    // Analyze current memory usage
    size_t workingSet = calculateWorkingSet();
    double fragmentationIndex = calculateFragmentationIndex();
    
    if (workingSet > pressure_.totalMemory * 0.9) {
        addAlert("Memory usage approaching critical levels (" + std::to_string(workingSet) + " bytes)");
    }
    
    if (fragmentationIndex > 0.7) {
        addAlert("High memory fragmentation detected (" + std::to_string(fragmentationIndex) + ")");
    }
}

void MemoryProfiler::analyzeCurrentAccesses() const {
    // Analyze current access patterns
    if (memoryAccesses_.empty()) return;
    
    // Calculate access frequency distribution
    std::map<size_t, size_t> accessFrequency;
    for (const auto& access : memoryAccesses_) {
        accessFrequency[access.size]++;
    }
    
    // Check for unusual access patterns
    size_t totalAccesses = memoryAccesses_.size();
    if (totalAccesses > 100) {
        double largeAccessRatio = 0.0;
        size_t largeAccessThreshold = 4096; // 4KB
        for (const auto& pair : accessFrequency) {
            if (pair.first >= largeAccessThreshold) {
                largeAccessRatio += static_cast<double>(pair.second) / totalAccesses;
            }
        }
        
        if (largeAccessRatio > 0.3) { // More than 30% of accesses are large
            addAlert("High ratio of large memory accesses detected");
        }
    }
}

void MemoryProfiler::analyzeCurrentPools() const {
    // Analyze current memory pools
    for (const auto& pool : poolProfiles_) {
        double efficiency = calculatePoolEfficiency(pool);
        
        if (efficiency < 0.5) { // Low efficiency
            addAlert("Pool " + pool.name + " has low efficiency (" + 
                    std::to_string(static_cast<int>(efficiency * 100)) + "%)");
        }
        
        if (pool.currentUsage > pool.totalCapacity * 0.9) { // Near full
            addAlert("Pool " + pool.name + " is near capacity (" + std::to_string(pool.currentUsage) + "/" + 
                    std::to_string(pool.totalCapacity) + " bytes)");
        }
    }
}

void MemoryProfiler::integrateWithMemoryTracker(const nlm::MemoryTracker* tracker) {
    memoryTracker_ = tracker;
}

void MemoryProfiler::integrateWithMemoryDebug(const nlm::MemoryDebug* debugger) {
    memoryDebug_ = debugger;
}

void MemoryProfiler::integrateWithBrain(const nlm::Brain* brain) {
    brain_ = brain;
}

void MemoryProfiler::integrateWithPerformanceMonitor(const nlm::PerformanceMonitor* monitor) {
    performanceMonitor_ = monitor;
}

void MemoryProfiler::startStep(size_t step, double time) {
    currentStep_ = step;
    currentTime_ = time;
}

void MemoryProfiler::endStep(size_t step, double time) {
    // Analyze memory usage for this step
    analyzeCurrentUsage();
    analyzeCurrentAccesses();
    analyzeCurrentPools();
    
    // Update memory pressure
    updateMemoryPressure();
}

void MemoryProfiler::saveProfileReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateProfileReport();
        file.close();
    }
}

void MemoryProfiler::saveOptimizationReport(const std::string& filename) const {
    std::vector<MemoryOptimization> recommendations = getOptimizationRecommendations();
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "=== NLM Memory Optimization Report ===" << std::endl;
        file << "Generated at: " << getCurrentTimeString() << std::endl;
        file << "Total Recommendations: " << recommendations.size() << std::endl;
        file << std::endl;
        
        for (size_t i = 0; i < recommendations.size(); ++i) {
            const auto& opt = recommendations[i];
            file << i + 1 << ". " << opt.optimizationType << std::endl;
            file << "   Description: " << opt.description << std::endl;
            file << "   Potential Gain: " << std::fixed << std::setprecision(2) << opt.potentialGain << "%" << std::endl;
            file << "   Auto-fixable: " << (opt.isAutoFixable ? "Yes" : "No") << std::endl;
            file << "   Implementation Cost: " << opt.implementationCost << "/10" << std::endl;
            file << "   Implementation Plan: " << opt.implementationPlan << std::endl;
            file << std::endl;
        }
        
        file.close();
    }
}

std::vector<std::string> MemoryProfiler::getPerformanceAlerts() const {
    return performanceAlerts_;
}

void MemoryProfiler::updateMemoryPressure() {
    // Calculate memory pressure
    size_t workingSet = calculateWorkingSet();
    pressure_.currentPressure = workingSet > 0 ? 
        static_cast<double>(workingSet) / pressure_.totalMemory : 0.0;
    
    pressure_.availableMemory = pressure_.totalMemory - workingSet;
    
    // Check for critical pressure
    if (pressure_.currentPressure > 0.95) {
        pressure_.isCritical = true;
        addAlert("Critical memory pressure detected (" + std::to_string(pressure_.currentPressure * 100.0) + "%)");
    } else {
        pressure_.isCritical = false;
    }
}

void MemoryProfiler::checkForPressureAlerts() {
    // Check for memory pressure alerts
    if (pressure_.currentPressure > pressure_.pressureThreshold * 1.1) {
        addAlert("Memory pressure exceeded threshold (current: " + std::to_string(pressure_.currentPressure * 100.0) +
                "%, threshold: " + std::to_string(pressure_.pressureThreshold * 100.0) + "%)");
    }
}

size_t MemoryProfiler::calculateWorkingSet() const {
    // Simplified working set calculation
    size_t workingSet = 0;
    
    for (const auto& access : memoryAccesses_) {
        // Check if this access was recent (within last 10 time units)
        if (calculateTimeSince(access.timestamp) < 10.0) {
            workingSet += access.size;
        }
    }
    
    return workingSet;
}

double MemoryProfiler::calculateLocalityScore() const {
    // Simplified locality score calculation
    if (memoryAccesses_.empty()) return 1.0;
    
    size_t temporalClusters = 0;
    size_t totalAccesses = memoryAccesses_.size();
    
    // Count consecutive accesses to same region
    size_t consecutiveSameRegion = 0;
    size_t maxConsecutive = 0;
    
    for (size_t i = 1; i < memoryAccesses_.size(); ++i) {
        if (memoryAccesses_[i].address / 4096 == memoryAccesses_[i-1].address / 4096) {
            consecutiveSameRegion++;
            maxConsecutive = std::max(maxConsecutive, consecutiveSameRegion);
        } else {
            consecutiveSameRegion = 0;
        }
    }
    
    return static_cast<double>(maxConsecutive) / totalAccesses;
}

double MemoryProfiler::calculateFragmentationIndex() const {
    // Simplified fragmentation index calculation
    if (allocations_.empty()) return 0.0;
    
    size_t totalAllocated = 0;
    size_t smallAllocations = 0;
    size_t largeAllocations = 0;
    
    for (const auto& alloc : allocations_) {
        totalAllocated += alloc.size;
        if (alloc.size < 1024) { // < 1KB
            smallAllocations++;
        } else if (alloc.size > 1024 * 1024) { // > 1MB
            largeAllocations++;
        }
    }
    
    // Calculate fragmentation based on allocation size distribution
    double fragmentation = 0.0;
    if (totalAllocated > 0) {
        double smallRatio = static_cast<double>(smallAllocations) / allocations_.size();
        double largeRatio = static_cast<double>(largeAllocations) / allocations_.size();
        fragmentation = smallRatio * 0.7 + largeRatio * 0.3;
    }
    
    return fragmentation;
}

double MemoryProfiler::calculatePoolEfficiency(const MemoryPoolProfile& pool) const {
    if (pool.totalCapacity == 0) return 0.0;
    
    double efficiency = static_cast<double>(pool.currentUsage) / pool.totalCapacity;
    
    // Adjust based on allocation/deallocation ratio
    if (pool.allocationCount + pool.deallocationCount > 0) {
        double activityRatio = static_cast<double>(pool.allocationCount) / 
                              (pool.allocationCount + pool.deallocationCount);
        efficiency *= activityRatio;
    }
    
    return std::min(1.0, efficiency);
}

std::map<size_t, double> MemoryProfiler::analyzeTemporalPatterns() const {
    // Analyze temporal access patterns
    std::map<size_t, double> temporalPatterns;
    
    // Group accesses by size and calculate frequency
    std::map<size_t, size_t> sizeAccessCounts;
    for (const auto& access : memoryAccesses_) {
        sizeAccessCounts[access.size]++;
    }
    
    for (const auto& pair : sizeAccessCounts) {
        temporalPatterns[pair.first] = static_cast<double>(pair.second) / memoryAccesses_.size();
    }
    
    return temporalPatterns;
}

std::map<size_t, double> MemoryProfiler::analyzeSpatialPatterns() const {
    // Analyze spatial access patterns
    std::map<size_t, double> spatialPatterns;
    
    // Group accesses by working set region
    std::map<size_t, size_t> regionAccessCounts;
    for (const auto& access : memoryAccesses_) {
        size_t region = access.address / 4096; // 4KB regions
        regionAccessCounts[region]++;
    }
    
    size_t totalAccesses = memoryAccesses_.size();
    for (const auto& pair : regionAccessCounts) {
        double percentage = totalAccesses > 0 ? 
            static_cast<double>(pair.second) / totalAccesses * 100.0 : 0.0;
        spatialPatterns[pair.first * 4096] = percentage;
    }
    
    return spatialPatterns;
}

std::vector<size_t> MemoryProfiler::findHotSpots() const {
    // Find frequently accessed addresses
    std::map<size_t, size_t> accessCounts;
    for (const auto& access : memoryAccesses_) {
        accessCounts[access.address]++;
    }
    
    std::vector<std::pair<size_t, size_t>> sortedAccesses(
        accessCounts.begin(), accessCounts.end());
    std::sort(sortedAccesses.begin(), sortedAccesses.end(),
              [](const auto& a, const auto& b) { return b.second < a.second; });
    
    std::vector<size_t> hotspots;
    for (size_t i = 0; i < std::min(size_t(10), sortedAccesses.size()); ++i) {
        hotspots.push_back(sortedAccesses[i].first);
    }
    
    return hotspots;
}

std::vector<size_t> MemoryProfiler::findColdSpots() const {
    // Find seldom accessed addresses
    std::map<size_t, size_t> accessCounts;
    for (const auto& access : memoryAccesses_) {
        accessCounts[access.address]++;
    }
    
    std::vector<std::pair<size_t, size_t>> sortedAccesses(
        accessCounts.begin(), accessCounts.end());
    std::sort(sortedAccesses.begin(), sortedAccesses.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    std::vector<size_t> coldspots;
    for (size_t i = 0; i < std::min(size_t(10), sortedAccesses.size()); ++i) {
        coldspots.push_back(sortedAccesses[i].first);
    }
    
    return coldspots;
}

void MemoryProfiler::recommendPoolResizing() const {
    for (const auto& pool : poolProfiles_) {
        double efficiency = calculatePoolEfficiency(pool);
        
        if (efficiency < 0.6) { // Low efficiency
            MemoryOptimization opt;
            opt.optimizationType = "Pool Resizing";
            opt.potentialGain = (1.0 - efficiency) * 100.0;
            opt.description = "Resize memory pool " + pool.name + " for better efficiency";
            opt.isAutoFixable = true;
            opt.implementationPlan = "Adjust pool capacity based on usage patterns";
            opt.implementationCost = 3.0;
            
            // Add to recommendations
            const_cast<std::vector<MemoryOptimization>&>(recommendations_).push_back(opt);
        }
    }
}

void MemoryProfiler::recommendCacheOptimizations() const {
    // Check for cache optimization opportunities
    double cacheEfficiency = 0.0;
    if (!hierarchyStats_.empty()) {
        size_t totalHits = 0, totalAccesses = 0;
        for (const auto& stats : hierarchyStats_) {
            totalHits += stats.hitCount;
            totalAccesses += stats.hitCount + stats.missCount;
        }
        cacheEfficiency = totalAccesses > 0 ? static_cast<double>(totalHits) / totalAccesses : 0.0;
    }
    
    if (cacheEfficiency < 0.7) {
        MemoryOptimization opt;
        opt.optimizationType = "Cache Optimization";
        opt.potentialGain = (0.7 - cacheEfficiency) * 100.0;
        opt.description = "Optimize memory access patterns for better cache utilization";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Implement prefetching and access pattern optimization";
        opt.implementationCost = 5.0;
        
        const_cast<std::vector<MemoryOptimization>&>(recommendations_).push_back(opt);
    }
}

void MemoryProfiler::recommendMemoryCompression() const {
    // Check for memory compression opportunities
    size_t workingSet = calculateWorkingSet();
    if (workingSet > pressure_.totalMemory * 0.8) { // High memory usage
        MemoryOptimization opt;
        opt.optimizationType = "Memory Compression";
        opt.potentialGain = 20.0; // 20% reduction
        opt.description = "Implement memory compression to reduce pressure";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Add compression algorithms for compressible data";
        opt.implementationCost = 7.0;
        
        const_cast<std::vector<MemoryOptimization>&>(recommendations_).push_back(opt);
    }
}

void MemoryProfiler::recommendThreadLocalStorage() const {
    // Check for thread local storage opportunities
    size_t threadContention = 0;
    for (const auto& access : memoryAccesses_) {
        // Simplified check for potential thread contention
        if (access.threadId % 10 == 0) { // Arbitrary check
            threadContention++;
        }
    }
    
    if (threadContention > memoryAccesses_.size() * 0.3) { // High contention
        MemoryOptimization opt;
        opt.optimizationType = "Thread Local Storage";
        opt.potentialGain = 15.0;
        opt.description = "Implement thread local storage to reduce contention";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Add thread-local buffers and caches";
        opt.implementationCost = 4.0;
        
        const_cast<std::vector<MemoryOptimization>&>(recommendations_).push_back(opt);
    }
}

void MemoryProfiler::recommendAccessPatternOptimizations() const {
    // Check for access pattern optimization opportunities
    auto hotspots = findHotSpots();
    if (!hotspots.empty()) {
        MemoryOptimization opt;
        opt.optimizationType = "Access Pattern Optimization";
        opt.potentialGain = 10.0;
        opt.description = "Optimize access patterns for better locality";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Reorganize data structures and access sequences";
        opt.implementationCost = 6.0;
        
        const_cast<std::vector<MemoryOptimization>&>(recommendations_).push_back(opt);
    }
}

void MemoryProfiler::finalizeCurrentProfile() {
    std::lock_guard<std::mutex> lock(profileSessionMutex_);
    
    if (currentProfile_.stepCount > 0) {
        // Save current profile to history
        profileHistory_.push_back(currentProfile_);
        
        // Keep history bounded
        if (profileHistory_.size() > 100) {
            profileHistory_.erase(profileHistory_.begin());
        }
    }
}

void MemoryProfiler::startNewProfile(const std::string& sessionName) {
    currentProfile_.sessionName = sessionName;
    currentProfile_.startTime = getCurrentTime();
    currentProfile_.endTime = 0.0;
    currentProfile_.stepCount = 0;
    currentProfile_.accesses.clear();
    currentProfile_.allocations.clear();
    currentProfile_.deallocations.clear();
    currentProfile_.poolProfiles.clear();
    currentProfile_.hierarchyStats.clear();
    currentProfile_.pressure = pressure_;
    currentProfile_.alerts.clear();
}

ProfileSession MemoryProfiler::createSessionSnapshot() const {
    ProfileSession snapshot = currentProfile_;
    snapshot.endTime = getCurrentTime();
    return snapshot;
}

std::string MemoryProfiler::formatProfileReport() const {
    std::stringstream report;
    
    report << "=== NLM Memory Profile Report ===" << std::endl;
    report << "Session: " << currentProfile_.sessionName << std::endl;
    report << "Duration: " << std::fixed << std::setprecision(2) 
           << (currentProfile_.endTime - currentProfile_.startTime) << " seconds" << std::endl;
    report << "Steps: " << currentProfile_.stepCount << std::endl;
    report << std::endl;
    
    return report.str();
}

std::string MemoryProfiler::formatOptimizationReport() const {
    std::stringstream report;
    
    std::vector<MemoryOptimization> recommendations = getOptimizationRecommendations();
    
    report << "=== NLM Memory Optimization Report ===" << std::endl;
    report << "Generated at: " << getCurrentTimeString() << std::endl;
    report << "Total Recommendations: " << recommendations.size() << std::endl;
    report << std::endl;
    
    for (size_t i = 0; i < recommendations.size(); ++i) {
        const auto& opt = recommendations[i];
        report << i + 1 << ". " << opt.optimizationType << std::endl;
        report << "   Description: " << opt.description << std::endl;
        report << "   Potential Gain: " << std::fixed << std::setprecision(2) << opt.potentialGain << "%" << std::endl;
        report << "   Auto-fixable: " << (opt.isAutoFixable ? "Yes" : "No") << std::endl;
        report << "   Implementation Cost: " << opt.implementationCost << "/10" << std::endl;
        report << "   Implementation Plan: " << opt.implementationPlan << std::endl;
        report << std::endl;
    }
    
    return report.str();
}

std::string MemoryProfiler::generateProfileSummary() const {
    std::stringstream report;
    
    report << "=== NLM Memory Profile Summary ===" << std::endl;
    report << "Profile Session: " << currentProfile_.sessionName << std::endl;
    report << "Profile Type: " << getProfileTypeString(profileType_) << std::endl;
    report << "Duration: " << std::fixed << std::setprecision(2) 
           << (currentProfile_.endTime - currentProfile_.startTime) << " seconds" << std::endl;
    report << "Steps Recorded: " << currentProfile_.stepCount << std::endl;
    report << "Memory Accesses: " << memoryAccesses_.size() << std::endl;
    report << "Allocations: " << allocations_.size() << std::endl;
    report << "Deallocations: " << deallocations_.size() << std::endl;
    report << std::endl;
    
    return report.str();
}

std::string MemoryProfiler::generateMemoryUsageAnalysis() const {
    return analyzeMemoryUsage();
}

std::string MemoryProfiler::generateAccessPatternAnalysis() const {
    return analyzeAccessPatterns();
}

std::string MemoryProfiler::generatePoolEfficiencyAnalysis() const {
    return analyzePoolEfficiency();
}

std::string MemoryProfiler::generateHierarchyAnalysis() const {
    return analyzeMemoryHierarchy();
}

std::string MemoryProfiler::generateOptimizationAnalysis() const {
    std::stringstream report;
    
    std::vector<MemoryOptimization> recommendations = getOptimizationRecommendations();
    
    report << "=== Memory Optimization Analysis ===" << std::endl;
    report << "Total Optimization Recommendations: " << recommendations.size() << std::endl;
    report << std::endl;
    
    for (size_t i = 0; i < recommendations.size(); ++i) {
        const auto& opt = recommendations[i];
        report << i + 1 << ". " << opt.optimizationType << std::endl;
        report << "   Description: " << opt.description << std::endl;
        report << "   Potential Gain: " << std::fixed << std::setprecision(2) << opt.potentialGain << "%" << std::endl;
        report << "   Auto-fixable: " << (opt.isAutoFixable ? "Yes" : "No") << std::endl;
        report << "   Implementation Cost: " << opt.implementationCost << "/10" << std::endl;
        report << std::endl;
    }
    
    return report.str();
}

void MemoryProfiler::addAlert(const std::string& alert) {
    performanceAlerts_.push_back(alert);
}

void MemoryProfiler::clearAlerts() {
    performanceAlerts_.clear();
}

bool MemoryProfiler::hasCriticalAlerts() const {
    return !performanceAlerts_.empty();
}

void MemoryProfiler::generateMemoryOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const {
    // Generate memory optimization recommendations
    if (pressure_.currentPressure > 0.8) {
        MemoryOptimization opt;
        opt.optimizationType = "Memory Compression";
        opt.potentialGain = 15.0;
        opt.description = "Implement memory compression to reduce memory pressure";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Add compression algorithms for compressible data";
        opt.implementationCost = 7.0;
        recommendations.push_back(opt);
    }
    
    if (calculateFragmentationIndex() > 0.7) {
        MemoryOptimization opt;
        opt.optimizationType = "Memory Pool Tuning";
        opt.potentialGain = 10.0;
        opt.description = "Tune memory pool parameters to reduce fragmentation";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Adjust pool sizes and allocation strategies";
        opt.implementationCost = 4.0;
        recommendations.push_back(opt);
    }
}

void MemoryProfiler::generatePerformanceOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const {
    // Generate performance optimization recommendations
    if (memoryAccesses_.size() > 1000) {
        MemoryOptimization opt;
        opt.optimizationType = "Access Pattern Optimization";
        opt.potentialGain = 12.0;
        opt.description = "Optimize memory access patterns for better performance";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Reorganize data and implement prefetching";
        opt.implementationCost = 5.0;
        recommendations.push_back(opt);
    }
}

void MemoryProfiler::generateCacheOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const {
    // Generate cache optimization recommendations
    double cacheEfficiency = 0.0;
    if (!hierarchyStats_.empty()) {
        size_t totalHits = 0, totalAccesses = 0;
        for (const auto& stats : hierarchyStats_) {
            totalHits += stats.hitCount;
            totalAccesses += stats.hitCount + stats.missCount;
        }
        cacheEfficiency = totalAccesses > 0 ? static_cast<double>(totalHits) / totalAccesses : 0.0;
    }
    
    if (cacheEfficiency < 0.7) {
        MemoryOptimization opt;
        opt.optimizationType = "Cache Optimization";
        opt.potentialGain = (0.7 - cacheEfficiency) * 100.0;
        opt.description = "Optimize cache configuration for better hit rates";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Adjust cache sizes and associativity";
        opt.implementationCost = 6.0;
        recommendations.push_back(opt);
    }
}

void MemoryProfiler::generatePoolOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const {
    // Generate memory pool optimization recommendations
    for (const auto& pool : poolProfiles_) {
        double efficiency = calculatePoolEfficiency(pool);
        
        if (efficiency < 0.6) {
            MemoryOptimization opt;
            opt.optimizationType = "Pool Resizing";
            opt.potentialGain = (1.0 - efficiency) * 100.0;
            opt.description = "Resize memory pool " + pool.name + " for better efficiency";
            opt.isAutoFixable = true;
            opt.implementationPlan = "Adjust pool capacity based on usage patterns";
            opt.implementationCost = 4.0;
            recommendations.push_back(opt);
        }
    }
}

void MemoryProfiler::generateAccessOptimizationRecommendations(std::vector<MemoryOptimization>& recommendations) const {
    // Generate access optimization recommendations
    std::vector<size_t> hotspots = findHotSpots();
    std::vector<size_t> coldspots = findColdSpots();
    
    if (!hotspots.empty()) {
        MemoryOptimization opt;
        opt.optimizationType = "Hot Spot Optimization";
        opt.potentialGain = 8.0;
        opt.description = "Optimize access to frequently accessed memory locations";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Implement caching and prefetching for hot spots";
        opt.implementationCost = 5.0;
        recommendations.push_back(opt);
    }
    
    if (!coldspots.empty()) {
        MemoryOptimization opt;
        opt.optimizationType = "Cold Spot Optimization";
        opt.potentialGain = 5.0;
        opt.description = "Optimize access to rarely accessed memory locations";
        opt.isAutoFixable = true;
        opt.implementationPlan = "Implement lazy loading and eviction strategies";
        opt.implementationCost = 3.0;
        recommendations.push_back(opt);
    }
}

std::string MemoryProfiler::getProfileTypeString(ProfileType type) const {
    switch (type) {
        case ProfileType::AllocationProfile: return "Allocation Profile";
        case ProfileType::DeallocationProfile: return "Deallocation Profile";
        case ProfileType::AccessProfile: return "Access Profile";
        case ProfileType::PoolProfile: return "Pool Profile";
        case ProfileType::ThreadProfile: return "Thread Profile";
        default: return "Unknown Profile";
    }
}

std::string MemoryProfiler::getMemoryLevelString(MemoryLevel level) const {
    switch (level) {
        case MemoryLevel::L1_CACHE: return "L1 Cache";
        case MemoryLevel::L2_CACHE: return "L2 Cache";
        case MemoryLevel::L3_CACHE: return "L3 Cache";
        case MemoryLevel::RAM: return "RAM";
        case MemoryLevel::DISK: return "Disk";
        default: return "Unknown Level";
    }
}

double MemoryProfiler::calculateTimeSince(size_t timestamp) const {
    return static_cast<double>(getCurrentTime() - timestamp) / 1000.0; // Convert to seconds
}

size_t MemoryProfiler::getCurrentTime() const {
    return static_cast<size_t>(currentTime_ * 1000); // Convert to milliseconds
}

std::string MemoryProfiler::getCurrentTimeString() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

MemoryLevel MemoryProfiler::getMemoryLevel(size_t address) const {
    // Simplified memory level detection
    // In a real implementation, this would check actual memory hierarchy
    
    // Check if address is in hot cache
    for (const auto& stats : hierarchyStats_) {
        if (stats.level == MemoryLevel::L1_CACHE && isInCache(address, MemoryLevel::L1_CACHE)) {
            return MemoryLevel::L1_CACHE;
        }
    }
    
    // Check if address is in hot L2 cache
    for (const auto& stats : hierarchyStats_) {
        if (stats.level == MemoryLevel::L2_CACHE && isInCache(address, MemoryLevel::L2_CACHE)) {
            return MemoryLevel::L2_CACHE;
        }
    }
    
    // Check if address is in hot L3 cache
    for (const auto& stats : hierarchyStats_) {
        if (stats.level == MemoryLevel::L3_CACHE && isInCache(address, MemoryLevel::L3_CACHE)) {
            return MemoryLevel::L3_CACHE;
        }
    }
    
    return MemoryLevel::RAM;
}

double MemoryProfiler::getEstimatedLatency(MemoryLevel level) const {
    // Estimate latency for different memory levels
    switch (level) {
        case MemoryLevel::L1_CACHE: return 0.5; // 0.5 ns
        case MemoryLevel::L2_CACHE: return 5.0; // 5 ns
        case MemoryLevel::L3_CACHE: return 20.0; // 20 ns
        case MemoryLevel::RAM: return 100.0; // 100 ns
        case MemoryLevel::DISK: return 10000.0; // 10000 ns
        default: return 1000.0; // 1000 ns
    }
}

bool MemoryProfiler::isInCache(size_t address, MemoryLevel level) const {
    // Simplified cache presence check
    // In a real implementation, this would check actual cache contents
    
    // For demonstration, use a simple hash-based approximation
    std::hash<size_t> hasher;
    size_t hash = hasher(address);
    
    switch (level) {
        case MemoryLevel::L1_CACHE: return hash % 100 < 30; // 30% L1 cache hit rate
        case MemoryLevel::L2_CACHE: return hash % 100 < 70; // 70% L2 cache hit rate
        case MemoryLevel::L3_CACHE: return hash % 100 < 90; // 90% L3 cache hit rate
        default: return false;
    }
}

void MemoryProfiler::flush() {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    memoryAccesses_.clear();
    allocations_.clear();
    deallocations_.clear();
    poolProfiles_.clear();
    hierarchyStats_.clear();
    
    profileHistory_.clear();
    currentProfile_ = ProfileSession();
    
    performanceAlerts_.clear();
}

void MemoryProfiler::reset() {
    std::lock_guard<std::shared_mutex> lock(dataMutex_);
    
    totalAccesses_ = 0;
    totalAllocations_ = 0;
    totalDeallocations_ = 0;
    totalBytesAccessed_ = 0;
    totalBytesAllocated_ = 0;
    totalBytesDeallocated_ = 0;
    
    memoryAccesses_.clear();
    allocations_.clear();
    deallocations_.clear();
    poolProfiles_.clear();
    hierarchyStats_.clear();
    
    profileHistory_.clear();
    currentProfile_ = ProfileSession();
    
    pressure_ = MemoryPressure();
    
    performanceAlerts_.clear();
}

// Python bindings for MemoryProfiler
void initMemoryProfilerBindings(py::module& m) {
    py::class_<nlm::MemoryProfiler>(m, "MemoryProfiler")
        .def("enable", &nlm::MemoryProfiler::enable, py::arg("enable") = true,
             "Enable or disable memory profiling")
        .def("isEnabled", &nlm::MemoryProfiler::isEnabled,
             "Check if memory profiling is enabled")
        .def("setProfileType", &nlm::MemoryProfiler::setProfileType,
             py::arg("type"), "Set profiling type")
        .def("setProfileInterval", &nlm::MemoryProfiler::setProfileInterval,
             py::arg("interval"), "Set profiling interval")
        .def("recordAccess", &nlm::MemoryProfiler::recordAccess,
             py::arg("address"), py::arg("level"), py::arg("threadId"),
             py::arg("isRead"), py::arg("isInstruction"), py::arg("latency"),
             py::arg("size"), py::arg("callSite"), "Record memory access")
        .def("recordAllocation", &nlm::MemoryProfiler::recordAllocation,
             py::arg("address"), py::arg("size"), py::arg("site"),
             py::arg("stackTrace"), py::arg("threadId"), "Record allocation")
        .def("recordDeallocation", &nlm::MemoryProfiler::recordDeallocation,
             py::arg("address"), py::arg("site"), py::arg("threadId"),
             "Record deallocation")
        .def("recordPoolUsage", &nlm::MemoryProfiler::recordPoolUsage,
             py::arg("poolName"), py::arg("allocated"), py::arg("available"),
             "Record memory pool usage")
        .def("recordPoolAllocation", &nlm::MemoryProfiler::recordPoolAllocation,
             py::arg("poolName"), py::arg("size"), "Record pool allocation")
        .def("recordPoolDeallocation", &nlm::MemoryProfiler::recordPoolDeallocation,
             py::arg("poolName"), py::arg("size"), "Record pool deallocation")
        .def("updateMemoryHierarchy", &nlm::MemoryProfiler::updateMemoryHierarchy,
             py::arg("stats"), "Update memory hierarchy statistics")
        .def("updateMemoryPressure", &nlm::MemoryProfiler::updateMemoryPressure,
             py::arg("pressure"), "Update memory pressure")
        .def("startProfile", &nlm::MemoryProfiler::startProfile,
             py::arg("sessionName"), "Start profiling session")
        .def("endProfile", &nlm::MemoryProfiler::endProfile,
             py::arg("sessionName"), "End profiling session")
        .def("getCurrentProfile", &nlm::MemoryProfiler::getCurrentProfile,
             "Get current profile")
        .def("getProfileHistory", &nlm::MemoryProfiler::getProfileHistory,
             "Get profile history")
        .def("getOptimizationRecommendations", &nlm::MemoryProfiler::getOptimizationRecommendations,
             "Get optimization recommendations")
        .def("generateProfileReport", &nlm::MemoryProfiler::generateProfileReport,
             "Generate profile report")
        .def("saveProfileReport", &nlm::MemoryProfiler::saveProfileReport,
             py::arg("filename"), "Save profile report to file")
        .def("saveOptimizationReport", &nlm::MemoryProfiler::saveOptimizationReport,
             py::arg("filename"), "Save optimization report to file")
        .def("getPerformanceAlerts", &nlm::MemoryProfiler::getPerformanceAlerts,
             "Get performance alerts")
        .def("startStep", &nlm::MemoryProfiler::startStep,
             py::arg("step"), py::arg("time"), "Start tracking a simulation step")
        .def("endStep", &nlm::MemoryProfiler::endStep,
             py::arg("step"), py::arg("time"), "End tracking a simulation step");
}

PYBIND11_MODULE(memory_profiler, m) {
    m.doc() = R"pbdoc(
        NLM Memory Profiler
        -----------------
        Comprehensive memory profiling framework for the NLM neural simulation framework.
        
        Features:
        - Memory usage analysis and optimization recommendations
        - Memory hierarchy and pressure monitoring
        - Memory pool efficiency analysis
        - Access pattern analysis
        - Integration with NLM memory systems
    )pbdoc";
    
    // Memory level enum
    py::enum_<nlm::MemoryLevel>(m, "MemoryLevel")
        .value("L1_CACHE", nlm::MemoryLevel::L1_CACHE)
        .value("L2_CACHE", nlm::MemoryLevel::L2_CACHE)
        .value("L3_CACHE", nlm::MemoryLevel::L3_CACHE)
        .value("RAM", nlm::MemoryLevel::RAM)
        .value("DISK", nlm::MemoryLevel::DISK)
        .export_values();
    
    // Profile type enum
    py::enum_<nlm::ProfileType>(m, "ProfileType")
        .value("AllocationProfile", nlm::ProfileType::AllocationProfile)
        .value("DeallocationProfile", nlm::ProfileType::DeallocationProfile)
        .value("AccessProfile", nlm::ProfileType::AccessProfile)
        .value("PoolProfile", nlm::ProfileType::PoolProfile)
        .value("ThreadProfile", nlm::ProfileType::ThreadProfile)
        .export_values();
    
    // Memory access info struct
    py::struct_<nlm::MemoryAccessInfo>(m, "MemoryAccessInfo")
        .def_readwrite("timestamp", &nlm::MemoryAccessInfo::timestamp)
        .def_readwrite("address", &nlm::MemoryAccessInfo::address)
        .def_readwrite("level", &nlm::MemoryAccessInfo::level)
        .def_readwrite("threadId", &nlm::MemoryAccessInfo::threadId)
        .def_readwrite("isRead", &nlm::MemoryAccessInfo::isRead)
        .def_readwrite("isInstruction", &nlm::MemoryAccessInfo::isInstruction)
        .def_readwrite("latency", &nlm::MemoryAccessInfo::latency)
        .def_readwrite("size", &nlm::MemoryAccessInfo::size)
        .def_readwrite("callSite", &nlm::MemoryAccessInfo::callSite);
    
    // Memory hierarchy stats struct
    py::struct_<nlm::MemoryHierarchyStats>(m, "MemoryHierarchyStats")
        .def_readwrite("level", &nlm::MemoryHierarchyStats::level)
        .def_readwrite("totalSize", &nlm::MemoryHierarchyStats::totalSize)
        .def_readwrite("usedSize", &nlm::MemoryHierarchyStats::usedSize)
        .def_readwrite("availableSize", &nlm::MemoryHierarchyStats::availableSize)
        .def_readwrite("hitCount", &nlm::MemoryHierarchyStats::hitCount)
        .def_readwrite("missCount", &nlm::MemoryHierarchyStats::missCount)
        .def_readwrite("hitRate", &nlm::MemoryHierarchyStats::hitRate)
        .def_readwrite("averageLatency", &nlm::MemoryHierarchyStats::averageLatency);
    
    // Memory pressure struct
    py::struct_<nlm::MemoryPressure>(m, "MemoryPressure")
        .def_readwrite("currentPressure", &nlm::MemoryPressure::currentPressure)
        .def_readwrite("pressureThreshold", &nlm::MemoryPressure::pressureThreshold)
        .def_readwrite("availableMemory", &nlm::MemoryPressure::availableMemory)
        .def_readwrite("totalMemory", &nlm::MemoryPressure::totalMemory)
        .def_readwrite("pressureIncreaseRate", &nlm::MemoryPressure::pressureIncreaseRate)
        .def_readwrite("isCritical", &nlm::MemoryPressure::isCritical);
    
    // Memory optimization struct
    py::struct_<nlm::MemoryOptimization>(m, "MemoryOptimization")
        .def_readwrite("optimizationType", &nlm::MemoryOptimization::optimizationType)
        .def_readwrite("potentialGain", &nlm::MemoryOptimization::potentialGain)
        .def_readwrite("description", &nlm::MemoryOptimization::description)
        .def_readwrite("isAutoFixable", &nlm::MemoryOptimization::isAutoFixable)
        .def_readwrite("implementationPlan", &nlm::MemoryOptimization::implementationPlan)
        .def_readwrite("implementationCost", &nlm::MemoryOptimization::implementationCost);
    
    // Profile session struct
    py::struct_<nlm::ProfileSession>(m, "ProfileSession")
        .def_readwrite("sessionName", &nlm::ProfileSession::sessionName)
        .def_readwrite("startTime", &nlm::ProfileSession::startTime)
        .def_readwrite("endTime", &nlm::ProfileSession::endTime)
        .def_readwrite("stepCount", &nlm::ProfileSession::stepCount)
        .def_readwrite("pressure", &nlm::ProfileSession::pressure);
    
    // Main MemoryProfiler class
    initMemoryProfilerBindings(m);
}

} // namespace nlm

// Export for inclusion in bindings.cpp
void initMemoryProfilerBindings(py::module& m);
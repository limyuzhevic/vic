#include "OptimizationHints.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>

namespace nlm {

// ProfileAnalyzer implementations

ProfileAnalyzer::ProfileAnalyzer() : 
    hotspotThreshold_(0.5),
    minCallCount_(100),
    minTimeThreshold_(0.001),
    cacheMissThreshold_(0.2),
    memoryAccessRatioThreshold_(0.1),
    branchOutcomeThreshold_(0.3),
    simdUtilizationThreshold_(0.5),
    parallelismThreshold_(0.7) {
}

void ProfileAnalyzer::addProfileData(const ProfileData& data) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = profileData_.find(data.name);
    if (it != profileData_.end()) {
        // Merge with existing data
        ProfileData& existing = it->second;
        existing.callCount += data.callCount;
        existing.totalTime += data.totalTime;
        existing.instructionCount += data.instructionCount;
        existing.memoryAccesses += data.memoryAccesses;
        existing.memoryAccessedBytes += data.memoryAccessedBytes;
        
        // Update min/max times
        if (data.minTime > 0.0 && (existing.minTime == 0.0 || data.minTime < existing.minTime)) {
            existing.minTime = data.minTime;
        }
        if (data.maxTime > existing.maxTime) {
            existing.maxTime = data.maxTime;
        }
        
        // Update cache miss rate (weighted average)
        double totalInstructions = existing.instructionCount - data.instructionCount;
        if (totalInstructions > 0) {
            double currentMisses = existing.cacheMissRate * totalInstructions;
            double newMisses = data.cacheMissRate * data.instructionCount;
            existing.cacheMissRate = (currentMisses + newMisses) / (totalInstructions + data.instructionCount);
        }
        
        // Merge hot addresses
        for (const auto& pair : data.hotAddresses) {
            existing.hotAddresses[pair.first] += pair.second;
        }
        
        // Merge branch outcomes
        for (const auto& pair : data.branchOutcomes) {
            auto it = std::find_if(existing.branchOutcomes.begin(), existing.branchOutcomes.end(),
                                 [pair](const auto& p) { return p.first == pair.first; });
            if (it != existing.branchOutcomes.end()) {
                it->second += pair.second;
            } else {
                existing.branchOutcomes.push_back(pair);
            }
        }
    } else {
        profileData_[data.name] = data;
    }
}

void ProfileAnalyzer::addProfileData(const std::vector<ProfileData>& data) {
    for (const auto& datum : data) {
        addProfileData(datum);
    }
}

std::vector<OptimizationHint> ProfileAnalyzer::analyze(const std::string& profileName) const {
    std::vector<OptimizationHint> allHints;
    
    if (profileName == "global") {
        // Analyze all profile data
        for (const auto& pair : profileData_) {
            const auto& name = pair.first;
            const auto& data = pair.second;
            
            if (meetsAnalysisCriteria(data)) {
                // Generate various types of hints
                auto cacheHints = generateCacheHints(data);
                auto loopHints = generateLoopHints(data);
                auto dataLayoutHints = generateDataLayoutHints(data);
                auto memoryManagementHints = generateMemoryManagementHints(data);
                auto computationalHints = generateComputationalHints(data);
                
                allHints.insert(allHints.end(), cacheHints.begin(), cacheHints.end());
                allHints.insert(allHints.end(), loopHints.begin(), loopHints.end());
                allHints.insert(allHints.end(), dataLayoutHints.begin(), dataLayoutHints.end());
                allHints.insert(allHints.end(), memoryManagementHints.begin(), memoryManagementHints.end());
                allHints.insert(allHints.end(), computationalHints.begin(), computationalHints.end());
            }
        }
    } else {
        // Analyze specific profile
        auto it = profileData_.find(profileName);
        if (it != profileData_.end()) {
            const auto& data = it->second;
            if (meetsAnalysisCriteria(data)) {
                auto cacheHints = generateCacheHints(data);
                auto loopHints = generateLoopHints(data);
                auto dataLayoutHints = generateDataLayoutHints(data);
                auto memoryManagementHints = generateMemoryManagementHints(data);
                auto computationalHints = generateComputationalHints(data);
                
                allHints.insert(allHints.end(), cacheHints.begin(), cacheHints.end());
                allHints.insert(allHints.end(), loopHints.begin(), loopHints.end());
                allHints.insert(allHints.end(), dataLayoutHints.begin(), dataLayoutHints.end());
                allHints.insert(allHints.end(), memoryManagementHints.begin(), memoryManagementHints.end());
                allHints.insert(allHints.end(), computationalHints.begin(), computationalHints.end());
            }
        }
    }
    
    // Sort hints by priority
    std::sort(allHints.begin(), allHints.end(),
              [](const OptimizationHint& a, const OptimizationHint& b) {
                  return a.priority > b.priority;
              });
    
    return allHints;
}

std::vector<OptimizationHint> ProfileAnalyzer::generateCacheHints() const {
    std::vector<OptimizationHint> hints;
    
    for (const auto& pair : profileData_) {
        const auto& name = pair.first;
        const auto& data = pair.second;
        
        if (meetsAnalysisCriteria(data)) {
            auto cacheHints = generateCacheHints(data);
            hints.insert(hints.end(), cacheHints.begin(), cacheHints.end());
        }
    }
    
    return hints;
}

std::vector<OptimizationHint> ProfileAnalyzer::generateMemoryHints() const {
    std::vector<OptimizationHint> hints;
    
    for (const auto& pair : profileData_) {
        const auto& name = pair.first;
        const auto& data = pair.second;
        
        if (meetsAnalysisCriteria(data)) {
            auto memoryHints = generateMemoryManagementHints(data);
            hints.insert(hints.end(), memoryHints.begin(), memoryHints.end());
        }
    }
    
    return hints;
}

std::vector<OptimizationHint> ProfileAnalyzer::generateBranchHints() const {
    std::vector<OptimizationHint> hints;
    
    for (const auto& pair : profileData_) {
        const auto& name = pair.first;
        const auto& data = pair.second;
        
        if (meetsAnalysisCriteria(data) && data.branchOutcomes.size() > 0) {
            // Generate branch prediction hints
            double bias = 0.0;
            for (const auto& outcome : data.branchOutcomes) {
                bias += outcome.second;
            }
            bias /= data.branchOutcomes.size();
            
            OptimizationHint::Type type = OptimizationHint::Type::MINIMIZE_BRANCHES;
            std::string desc;
            double estimatedGain = 0.0;
            double implementationCost = 0.5;
            size_t priority = 5;
            
            if (std::abs(bias - 0.5) > branchOutcomeThreshold_) {
                // Biased branches can be optimized
                if (bias > 0.5) {
                    type = OptimizationHint::Type::LIKELY_TRUE;
                    desc = "Branch " + name + " is biased towards true, consider optimization hints";
                } else {
                    type = OptimizationHint::Type::LIKELY_FALSE;
                    desc = "Branch " + name + " is biased towards false, consider optimization hints";
                }
                estimatedGain = 0.1 * std::abs(bias - 0.5) * 100;
                priority = 7;
            }
            
            if (!desc.empty()) {
                OptimizationHint hint(type, desc, estimatedGain, implementationCost, priority,
                                     generateHintCodeLocation(data));
                hints.push_back(hint);
            }
        }
    }
    
    return hints;
}

std::vector<OptimizationHint> ProfileAnalyzer::generateSIMDHints() const {
    std::vector<OptimizationHint> hints;
    
    for (const auto& pair : profileData_) {
        const auto& name = pair.first;
        const auto& data = pair.second;
        
        if (meetsAnalysisCriteria(data) && isCacheFriendly(name)) {
            calculateSIMDUtilization(data);
            
            double simdUtilization = 0.0;
            if (data.instructionCount > 0) {
                simdUtilization = 1.0 - (data.cacheMissRate / simdUtilizationThreshold_);
            }
            
            if (simdUtilization < simdUtilizationThreshold_) {
                OptimizationHint hint(OptimizationHint::Type::USE_SIMD,
                                     "Function " + name + " has low SIMD utilization, consider vectorization",
                                     0.2 * (simdUtilizationThreshold_ - simdUtilization) * 100,
                                     0.3, 6, generateHintCodeLocation(data));
                hints.push_back(hint);
            }
        }
    }
    
    return hints;
}

std::vector<OptimizationHint> ProfileAnalyzer::generateParallelismHints() const {
    std::vector<OptimizationHint> hints;
    
    for (const auto& pair : profileData_) {
        const auto& name = pair.first;
        const auto& data = pair.second;
        
        if (meetsAnalysisCriteria(data) && hasGoodLocality(name)) {
            calculateParallelismPotential(data);
            
            double parallelPotential = 0.0;
            if (data.instructionCount > 0) {
                parallelPotential = 1.0 - (data.cacheMissRate / parallelismThreshold_);
            }
            
            if (parallelPotential > parallelismThreshold_) {
                OptimizationHint hint(OptimizationHint::Type::THREAD_PARALLELIZE,
                                     "Function " + name + " has high parallelization potential",
                                     0.3 * parallelPotential * 100,
                                     0.4, 8, generateHintCodeLocation(data));
                hints.push_back(hint);
            }
        }
    }
    
    return hints;
}

ProfileData ProfileAnalyzer::getProfileData(const std::string& name) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = profileData_.find(name);
    if (it != profileData_.end()) {
        return it->second;
    }
    
    return ProfileData(name);
}

std::unordered_map<std::string, ProfileData> ProfileAnalyzer::getAllProfileData() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return profileData_;
}

double ProfileAnalyzer::calculatePerformanceScore() const {
    double totalScore = 0.0;
    int count = 0;
    
    for (const auto& pair : profileData_) {
        const auto& data = pair.second;
        if (meetsAnalysisCriteria(data)) {
            double score = 0.0;
            
            // Time efficiency
            if (data.totalTime > 0.0) {
                score += 1.0 / data.totalTime;
            }
            
            // Cache efficiency
            if (data.instructionCount > 0) {
                score += (1.0 - data.cacheMissRate) * 10.0;
            }
            
            // Memory efficiency
            if (data.memoryAccessedBytes > 0) {
                score += 1.0 / (data.memoryAccessedBytes / 1024.0);
            }
            
            totalScore += score;
            count++;
        }
    }
    
    return count > 0 ? totalScore / count : 0.0;
}

std::vector<std::string> ProfileAnalyzer::identifyHotspots(size_t count) const {
    auto metrics = getAllProfileData();
    
    // Convert to vector for sorting
    std::vector<std::pair<std::string, double>> hotspotList;
    for (const auto& pair : metrics) {
        const auto& name = pair.first;
        const auto& data = pair.second;
        
        if (meetsAnalysisCriteria(data)) {
            double hotspotScore = 0.0;
            
            // Time-based scoring
            if (data.totalTime > 0.0) {
                hotspotScore += data.totalTime;
            }
            
            // Cache miss scoring
            if (data.instructionCount > 0) {
                hotspotScore += data.cacheMissRate * data.instructionCount;
            }
            
            // Memory access scoring
            hotspotScore += data.memoryAccessedBytes;
            
            hotspotList.emplace_back(name, hotspotScore);
        }
    }
    
    // Sort by hotspot score (descending)
    std::sort(hotspotList.begin(), hotspotList.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top N hotspots
    std::vector<std::string> hotspots;
    if (hotspotList.size() > count) {
        hotspots.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            hotspots.push_back(hotspotList[i].first);
        }
    } else {
        hotspots.reserve(hotspotList.size());
        for (const auto& pair : hotspotList) {
            hotspots.push_back(pair.first);
        }
    }
    
    return hotspots;
}

bool ProfileAnalyzer::isCacheFriendly(const std::string& name) const {
    auto it = profileData_.find(name);
    if (it == profileData_.end()) {
        return false;
    }
    
    const auto& data = it->second;
    return hasGoodLocality(name);
}

bool ProfileAnalyzer::hasGoodLocality(const std::string& name) const {
    auto it = profileData_.find(name);
    if (it == profileData_.end()) {
        return false;
    }
    
    const auto& data = it->second;
    return getAccessPattern(name) == MemoryAccessPattern::SEQUENTIAL;
}

MemoryAccessPattern ProfileAnalyzer::getAccessPattern(const std::string& name) const {
    auto it = profileData_.find(name);
    if (it == profileData_.end()) {
        return MemoryAccessPattern::RANDOM;
    }
    
    const auto& data = it->second;
    return data.accessPattern;
}

std::vector<size_t> ProfileAnalyzer::getHotAddresses(const std::string& name) const {
    std::vector<size_t> hotAddresses;
    
    auto it = profileData_.find(name);
    if (it == profileData_.end()) {
        return hotAddresses;
    }
    
    const auto& data = it->second;
    hotAddresses.reserve(data.hotAddresses.size());
    for (const auto& pair : data.hotAddresses) {
        hotAddresses.push_back(pair.first);
    }
    
    return hotAddresses;
}

std::vector<std::pair<size_t, uint64_t>> ProfileAnalyzer::getBranchOutcomes(const std::string& name) const {
    auto it = profileData_.find(name);
    if (it == profileData_.end()) {
        return std::vector<std::pair<size_t, uint64_t>>();
    }
    
    return it->second.branchOutcomes;
}

std::vector<OptimizationHint> ProfileAnalyzer::generatePrioritizedHints(size_t maxHints) const {
    std::vector<OptimizationHint> allHints;
    
    for (const auto& pair : profileData_) {
        const auto& name = pair.first;
        const auto& data = pair.second;
        
        if (meetsAnalysisCriteria(data)) {
            auto cacheHints = generateCacheHints(data);
            auto loopHints = generateLoopHints(data);
            auto dataLayoutHints = generateDataLayoutHints(data);
            auto memoryManagementHints = generateMemoryManagementHints(data);
            auto computationalHints = generateComputationalHints(data);
            
            allHints.insert(allHints.end(), cacheHints.begin(), cacheHints.end());
            allHints.insert(allHints.end(), loopHints.begin(), loopHints.end());
            allHints.insert(allHints.end(), dataLayoutHints.begin(), dataLayoutHints.end());
            allHints.insert(allHints.end(), memoryManagementHints.begin(), memoryManagementHints.end());
            allHints.insert(allHints.end(), computationalHints.begin(), computationalHints.end());
        }
    }
    
    // Sort by priority
    std::sort(allHints.begin(), allHints.end(),
              [](const OptimizationHint& a, const OptimizationHint& b) {
                  return a.priority > b.priority;
              });
    
    // Limit to maxHints
    if (allHints.size() > maxHints) {
        allHints.resize(maxHints);
    }
    
    return allHints;
}

bool ProfileAnalyzer::exportProfile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\"profile\": [" << std::endl;
    
    bool first = true;
    for (const auto& pair : profileData_) {
        if (!first) {
            file << "," << std::endl;
        }
        
        const auto& data = pair.second;
        
        file << "  {\"name\": \"" << data.name << "\",\"callCount\": " << data.callCount
             << ",\"totalTime\": " << data.totalTime
             << ",\"averageTime\": " << data.averageTime
             << ",\"minTime\": " << data.minTime
             << ",\"maxTime\": " << data.maxTime
             << ",\"instructionCount\": " << data.instructionCount
             << ",\"cacheMissRate\": " << data.cacheMissRate
             << ",\"memoryAccesses\": " << data.memoryAccesses
             << ",\"memoryAccessedBytes\": " << data.memoryAccessedBytes
             << ",\"accessPattern\": " << static_cast<int>(data.accessPattern)
             << "}";
        
        first = false;
    }
    
    file << std::endl << "]" << std::endl;
    file.close();
    
    return true;
}

bool ProfileAnalyzer::importProfile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, simple import from JSON-like format
    // In a real implementation, we'd use a proper JSON parser
    clear();
    
    std::string line;
    while (std::getline(file, line)) {
        // Simple parsing for demonstration
        if (line.find("name\": \"") != std::string::npos) {
            // Extract data and add profile
            ProfileData data("imported");
            // Add parsing logic here
            addProfileData(data);
        }
    }
    
    file.close();
    
    return true;
}

void ProfileAnalyzer::clear() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    profileData_.clear();
}

// CacheAnalyzer implementations

CacheAnalyzer::CacheAnalyzer() : 
    cacheSize_(32768),
    associativity_(4),
    lineSize_(64),
    spatialLocalityThreshold_(0.7),
    temporalLocalityThreshold_(0.8) {
}

MemoryAccessPattern CacheAnalyzer::analyzeMemoryAccess(const std::vector<size_t>& addresses) const {
    if (addresses.empty()) {
        return MemoryAccessPattern::SEQUENTIAL;
    }
    
    // Calculate stride
    double stride = calculateStride(addresses);
    
    if (std::abs(stride - 1.0) < 0.1) {
        return MemoryAccessPattern::SEQUENTIAL;
    } else if (stride < 0.1) {
        return MemoryAccessPattern::REPEATED;
    } else if (stride > 1000.0) {
        return MemoryAccessPattern::SCATTERED;
    } else if (std::abs(stride - std::floor(stride)) < 0.01) {
        return MemoryAccessPattern::STREAMING;
    } else {
        return MemoryAccessPattern::RANDOM;
    }
}

double CacheAnalyzer::calculateCacheMissRate(const std::vector<size_t>& addresses, size_t cacheSize) const {
    if (addresses.empty()) {
        return 0.0;
    }
    
    std::unordered_map<size_t, size_t> cacheState;
    double misses = 0.0;
    
    for (size_t address : addresses) {
        if (!isCacheHit(address, cacheState)) {
            misses++;
            updateCacheState(cacheState, address);
        }
    }
    
    return misses / addresses.size();
}

std::vector<size_t> CacheAnalyzer::identifyCacheLines(const std::vector<size_t>& addresses) const {
    std::vector<size_t> cacheLines;
    
    for (size_t address : addresses) {
        size_t cacheIndex = calculateCacheIndex(address);
        
        // Check if already present
        bool found = false;
        for (size_t line : cacheLines) {
            if (line == cacheIndex) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cacheLines.push_back(cacheIndex);
        }
    }
    
    return cacheLines;
}

double CacheAnalyzer::calculateSpatialLocality(const std::vector<size_t>& addresses) const {
    if (addresses.size() < 2) {
        return 1.0;
    }
    
    size_t sequentialCount = 0;
    for (size_t i = 1; i < addresses.size(); ++i) {
        if (addresses[i] == addresses[i-1] + 1 || addresses[i] == addresses[i-1] - 1) {
            sequentialCount++;
        }
    }
    
    return static_cast<double>(sequentialCount) / (addresses.size() - 1);
}

double CacheAnalyzer::calculateTemporalLocality(const std::vector<size_t>& addresses) const {
    if (addresses.size() < 2) {
        return 1.0;
    }
    
    size_t recentAccesses = 0;
    size_t temporalCount = 0;
    
    // Simple sliding window approach
    size_t windowSize = std::min(static_cast<size_t>(10), addresses.size());
    
    for (size_t i = 0; i < addresses.size(); ++i) {
        // Check if address was accessed recently
        bool wasRecent = false;
        for (size_t j = std::max(static_cast<size_t>(0), i - windowSize); j < i; ++j) {
            if (addresses[j] == addresses[i]) {
                wasRecent = true;
                break;
            }
        }
        
        if (wasRecent) {
            temporalCount++;
        }
    }
    
    return static_cast<double>(temporalCount) / addresses.size();
}

double CacheAnalyzer::predictCachePerformance(const std::vector<size_t>& addresses, size_t cacheSize) const {
    if (addresses.empty()) {
        return 1.0;
    }
    
    double missRate = calculateCacheMissRate(addresses, cacheSize);
    double spatialLocality = calculateSpatialLocality(addresses);
    double temporalLocality = calculateTemporalLocality(addresses);
    
    double score = (1.0 - missRate) * 0.5 + spatialLocality * 0.3 + temporalLocality * 0.2;
    
    return score;
}

std::vector<std::string> CacheAnalyzer::generateCacheHints(size_t cacheSize, size_t associativity) const {
    std::vector<std::string> hints;
    
    // Simple example hints based on cache parameters
    if (cacheSize < 1024) {
        hints.push_back("Use small cache-friendly data structures for better performance");
    } else if (cacheSize > 1024 * 1024) {
        hints.push_back("Use large cache blocks to reduce cache misses");
    }
    
    if (associativity < 4) {
        hints.push_back("Consider increasing cache associativity to reduce conflict misses");
    }
    
    hints.push_back("Use cache-aligned memory access for better performance");
    hints.push_back("Consider blocking/loop tiling for cache optimization");
    
    return hints;
}

bool CacheAnalyzer::analyzeSIMDMemoryAccess(const std::vector<size_t>& addresses) const {
    if (addresses.empty()) {
        return false;
    }
    
    // Check for aligned accesses
    bool allAligned = true;
    for (size_t address : addresses) {
        if (address % 16 != 0) {
            allAligned = false;
            break;
        }
    }
    
    if (!allAligned) {
        return false;
    }
    
    // Check for sequential or streaming access
    MemoryAccessPattern pattern = analyzeMemoryAccess(addresses);
    return pattern == MemoryAccessPattern::SEQUENTIAL || pattern == MemoryAccessPattern::STREAMING;
}

bool CacheAnalyzer::analyzeParallelMemoryAccess(const std::vector<size_t>& addresses) const {
    if (addresses.empty()) {
        return false;
    }
    
    // Check for scattered access (good for parallelization)
    MemoryAccessPattern pattern = analyzeMemoryAccess(addresses);
    return pattern == MemoryAccessPattern::RANDOM || pattern == MemoryAccessPattern::SCATTERED;
}

// OptimizationHints implementations

OptimizationHints::OptimizationHints() {
}

void OptimizationHints::addHints(const std::vector<OptimizationHint>& hints) {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    hints_.insert(hints_.end(), hints.begin(), hints.end());
}

void OptimizationHints::addCacheHints(const std::vector<std::string>& hints) {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    cacheHints_.insert(cacheHints_.end(), hints.begin(), hints.end());
}

std::vector<OptimizationHint> OptimizationHints::getAllHints() const {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    return hints_;
}

std::vector<OptimizationHint> OptimizationHints::getHintsByType(OptimizationHint::Type type) const {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    std::vector<OptimizationHint> filteredHints;
    for (const auto& hint : hints_) {
        if (hint.type == type) {
            filteredHints.push_back(hint);
        }
    }
    
    return filteredHints;
}

std::vector<OptimizationHint> OptimizationHints::getHintsByPriority(size_t minPriority) const {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    std::vector<OptimizationHint> filteredHints;
    for (const auto& hint : hints_) {
        if (hint.priority >= minPriority) {
            filteredHints.push_back(hint);
        }
    }
    
    return filteredHints;
}

std::vector<OptimizationHint> OptimizationHints::getHintsForLocation(const std::string& location) const {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    std::vector<OptimizationHint> filteredHints;
    for (const auto& hint : hints_) {
        if (hint.codeLocation == location) {
            filteredHints.push_back(hint);
        }
    }
    
    return filteredHints;
}

double OptimizationHints::getTotalEstimatedGain() const {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    double totalGain = 0.0;
    for (const auto& hint : hints_) {
        totalGain += hint.estimatedGain;
    }
    
    return totalGain;
}

double OptimizationHints::getTotalImplementationCost() const {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    double totalCost = 0.0;
    for (const auto& hint : hints_) {
        totalCost += hint.implementationCost;
    }
    
    return totalCost;
}

void OptimizationHints::sortHints() {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    std::sort(hints_.begin(), hints_.end(),
              [](const OptimizationHint& a, const OptimizationHint& b) {
                  return a.priority > b.priority;
              });
}

void OptimizationHints::filterHintsByGain(double minGain) {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    hints_.erase(std::remove_if(hints_.begin(), hints_.end(),
                               [minGain](const OptimizationHint& hint) {
                                   return hint.estimatedGain < minGain;
                               }),
                 hints_.end());
}

void OptimizationHints::filterHintsByCost(double maxCost) {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    hints_.erase(std::remove_if(hints_.begin(), hints_.end(),
                               [maxCost](const OptimizationHint& hint) {
                                   return hint.implementationCost > maxCost;
                               }),
                 hints_.end());
}

void OptimizationHints::mergeDuplicateHints() {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    std::vector<OptimizationHint> uniqueHints;
    std::unordered_set<std::string> seenLocations;
    
    for (const auto& hint : hints_) {
        if (seenLocations.find(hint.codeLocation) == seenLocations.end()) {
            uniqueHints.push_back(hint);
            seenLocations.insert(hint.codeLocation);
        }
    }
    
    hints_ = uniqueHints;
}

void OptimizationHints::removeLowPriorityHints(size_t threshold) {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    hints_.erase(std::remove_if(hints_.begin(), hints_.end(),
                               [threshold](const OptimizationHint& hint) {
                                   return hint.priority < threshold;
                               }),
                 hints_.end());
}

void OptimizationHints::clear() {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    hints_.clear();
    cacheHints_.clear();
}

void OptimizationHints::printHints(std::ostream& out) const {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    out << "=== NLM Optimization Hints ===" << std::endl;
    out << std::fixed << std::setprecision(1);
    
    if (!hints_.empty()) {
        out << std::left << std::setw(25) << "Location" 
            << std::left << std::setw(40) << "Description" 
            << std::right << std::setw(10) << "Gain%" 
            << std::right << std::setw(10) << "Cost" 
            << std::right << std::setw(8) << "Priority" 
            << std::endl;
        out << std::string(103, '-') << std::endl;
        
        for (const auto& hint : hints_) {
            out << std::left << std::setw(25) << hint.codeLocation
                << std::left << std::setw(40) << hint.description.substr(0, 37)
                << std::right << std::setw(10) << hint.estimatedGain
                << std::right << std::setw(10) << hint.implementationCost
                << std::right << std::setw(8) << hint.priority
                << std::endl;
        }
    }
    
    if (!cacheHints_.empty()) {
        out << std::endl << "Cache Optimization Hints:" << std::endl;
        for (const auto& hint : cacheHints_) {
            out << "  - " << hint << std::endl;
        }
    }
    
    out << std::endl;
    out << "Summary: " << hints_.size() << " optimization hints found" << std::endl;
    out << "Estimated total gain: " << getTotalEstimatedGain() << "%" << std::endl;
    out << "Estimated total implementation cost: " << getTotalImplementationCost() << std::endl;
}

bool OptimizationHints::saveHints(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    printHints(file);
    return true;
}

bool OptimizationHints::loadHints(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, simple import from text file
    // In a real implementation, we'd parse the output format
    clear();
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("Location") != std::string::npos) {
            // Skip header
            continue;
        }
        if (line.find("- ") == std::string::npos && !line.empty()) {
            // Add as cache hint
            cacheHints_.push_back(line);
        }
    }
    
    file.close();
    
    return true;
}

void OptimizationHints::applyOptimizations() {
    std::lock_guard<std::mutex> lock(hintsMutex_);
    
    for (const auto& hint : hints_) {
        // In a real implementation, this would actually apply optimizations
        // For now, just print what would be applied
        std::cout << "Applying optimization: " << hint.description << std::endl;
    }
}

} // namespace nlm
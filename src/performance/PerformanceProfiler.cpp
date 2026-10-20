// PerformanceProfiler.cpp implementation
#include "PerformanceProfiler.cpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}
PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::start(const std::string& name) {
    if (pImpl->profileStarts.find(name) == pImpl->profileStarts.end()) {
        pImpl->profileStarts[name] = std::chrono::high_resolution_clock::now();
    }
}

void PerformanceProfiler::stop(const std::string& name) {
    auto it = pImpl->profileStarts.find(name);
    if (it != pImpl->profileStarts.end()) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end - it->second).count();
        pImpl->profileTimes[name] += duration;
        pImpl->profileStarts.erase(it);
    }
}

void PerformanceProfiler::pause(const std::string& name) {
    // Store pause time for later calculation
    auto it = pImpl->profileStarts.find(name);
    if (it != pImpl->profileStarts.end()) {
        pImpl->profileStops[name] = std::chrono::high_resolution_clock::now();
    }
}

void PerformanceProfiler::resume(const std::string& name) {
    auto it = pImpl->profileStops.find(name);
    if (it != pImpl->profileStops.end()) {
        pImpl->profileStarts[name] = std::chrono::high_resolution_clock::now();
        pImpl->profileStops.erase(it);
    }
}

void PerformanceProfiler::enter(const std::string& functionName, const std::string& fileName, 
                               int lineNumber) {
    std::string key = functionName + " " + fileName + ":" + std::to_string(lineNumber);
    pImpl->profileCallCounts[key]++;
    pImpl->profileStarts[key] = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::exit(const std::string& functionName) {
    for (auto& pair : pImpl->profileStarts) {
        if (pair.first.find(functionName) != std::string::npos) {
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - pair.second).count();
            
            // Calculate actual time (accounting for pauses)
            double actualDuration = duration;
            auto stopIt = pImpl->profileStops.find(pair.first);
            if (stopIt != pImpl->profileStops.end()) {
                auto resumeIt = pImpl->profileStarts.find(pair.first);
                if (resumeIt != pImpl->profileStarts.end()) {
                    actualDuration = std::chrono::duration<double>(resumeIt->second - stopIt->second).count();
                }
            }
            
            pImpl->profileTimes[pair.first] += actualDuration;
            pImpl->profileStarts.erase(pair.first);
            if (stopIt != pImpl->profileStops.end()) {
                pImpl->profileStops.erase(stopIt);
            }
            break;
        }
    }
}

void PerformanceProfiler::recordAllocation(size_t size, const std::string& context) {
    std::string key = "alloc:" + context;
    pImpl->allocationSizes[key] += size;
    
    // Update global allocations
    pImpl->totalAllocations += size;
}

void PerformanceProfiler::recordDeallocation(size_t size, const std::string& context) {
    std::string key = "dealloc:" + context;
    pImpl->deallocationSizes[key] += size;
    
    // Update global deallocations
    pImpl->totalDeallocations += size;
}

void PerformanceProfiler::recordCPUCycles(uint64_t cycles) {
    pImpl->cpuCycles.push_back(cycles);
    pImpl->totalCPUCycles += cycles;
}

PerformanceProfiler::ProfileResult PerformanceProfiler::getCurrentProfile() const {
    ProfileResult result;
    
    // Calculate statistics for each function
    for (const auto& pair : pImpl->profileTimes) {
        ProfileData data;
        data.functionName = pair.first;
        data.totalTime = pair.second;
        
        // Calculate call count
        data.callCount = pImpl->profileCallCounts.count(pair.first) ? 
                        pImpl->profileCallCounts.at(pair.first) : 0;
        
        // Calculate self time (total time - time in children)
        data.selfTime = calculateSelfTime(pair.first);
        
        // Calculate average time
        data.averageTime = data.callCount > 0 ? data.selfTime / data.callCount : 0;
        
        // Calculate min and max time
        data.minTime = data.averageTime; // Would need per-call tracking
        data.maxTime = data.averageTime; // Would need per-call tracking
        
        // Calculate standard deviation
        data.stdDevTime = calculateStdDev(pair.first);
        
        // Memory allocation tracking
        data.memoryAllocated = pImpl->allocationSizes.count(pair.first) ? 
                               pImpl->allocationSizes.at(pair.first) : 0;
        data.memoryFreed = pImpl->deallocationSizes.count(pair.first) ? 
                           pImpl->deallocationSizes.at(pair.first) : 0;
        
        // CPU cycles
        if (!pImpl->cpuCycles.empty()) {
            uint64_t sum = std::accumulate(pImpl->cpuCycles.begin(), pImpl->cpuCycles.end(), 0ULL);
            data.cpuCycles = sum / pImpl->cpuCycles.size();
        }
        
        result.functionProfiles.push_back(data);
    }
    
    // Calculate totals
    for (const auto& data : result.functionProfiles) {
        result.totalWallTime += data.totalTime;
        result.totalCPUTime += data.cpuCycles;
        result.totalMemoryUsage += data.memoryAllocated - data.memoryFreed;
        result.totalCPUCycles += data.cpuCycles;
    }
    
    return result;
}

double PerformanceProfiler::calculateSelfTime(const std::string& functionName) const {
    double selfTime = 0;
    
    // Subtract time spent in child functions
    for (const auto& pair : pImpl->profileTimes) {
        if (pair.first.find(functionName) != std::string::npos && pair.first != functionName) {
            selfTime += pair.second;
        }
    }
    
    return pImpl->profileTimes.at(functionName) - selfTime;
}

double PerformanceProfiler::calculateStdDev(const std::string& functionName) const {
    // Simplified standard deviation calculation
    // In practice, would track all call times
    return 0.0;
}

bool PerformanceProfiler::saveProfile(const std::string& filename) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    
    // Write profile data in binary format
    size_t profileCount = pImpl->profileTimes.size();
    file.write(reinterpret_cast<const char*>(&profileCount), sizeof(size_t));
    
    for (const auto& pair : pImpl->profileTimes) {
        size_t nameLen = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(size_t));
        file.write(pair.first.c_str(), nameLen);
        file.write(reinterpret_cast<const char*>(&pair.second), sizeof(double));
    }
    
    // Write allocation data
    size_t allocCount = pImpl->allocationSizes.size();
    file.write(reinterpret_cast<const char*>(&allocCount), sizeof(size_t));
    
    for (const auto& pair : pImpl->allocationSizes) {
        size_t nameLen = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(size_t));
        file.write(pair.first.c_str(), nameLen);
        file.write(reinterpret_cast<const char*>(&pair.second), sizeof(size_t));
    }
    
    file.close();
    return true;
}

bool PerformanceProfiler::loadProfile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return false;
    
    // Clear existing data
    reset();
    
    // Read profile data
    size_t profileCount;
    file.read(reinterpret_cast<char*>(&profileCount), sizeof(size_t));
    
    for (size_t i = 0; i < profileCount; ++i) {
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(size_t));
        
        std::vector<char> buffer(nameLen + 1);
        file.read(buffer.data(), nameLen);
        buffer[nameLen] = '\0';
        
        double duration;
        file.read(reinterpret_cast<char*>(&duration), sizeof(double));
        
        pImpl->profileTimes[std::string(buffer.data())] = duration;
    }
    
    // Read allocation data
    size_t allocCount;
    file.read(reinterpret_cast<char*>(&allocCount), sizeof(size_t));
    
    for (size_t i = 0; i < allocCount; ++i) {
        size_t nameLen;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(size_t));
        
        std::vector<char> buffer(nameLen + 1);
        file.read(buffer.data(), nameLen);
        buffer[nameLen] = '\0';
        
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
        
        pImpl->allocationSizes[std::string(buffer.data())] = size;
    }
    
    file.close();
    return true;
}

std::string PerformanceProfiler::generateReport(bool detailed) const {
    std::stringstream ss;
    ss << "Performance Profile Report\n";
    ss << "===========================\n\n";
    
    ss << "Total profiles: " << pImpl->profileTimes.size() << "\n";
    ss << "Total allocations: " << pImpl->allocationSizes.size() << "\n";
    ss << "Total deallocations: " << pImpl->deallocationSizes.size() << "\n";
    ss << "CPU cycle samples: " << pImpl->cpuCycles.size() << "\n\n";
    
    // Calculate total time
    double totalTime = 0;
    for (const auto& pair : pImpl->profileTimes) {
        totalTime += pair.second;
    }
    
    ss << "Total time: " << totalTime << "s\n";
    ss << "Total allocations: " << pImpl->totalAllocations << " bytes\n";
    ss << "Total deallocations: " << pImpl->totalDeallocations << " bytes\n\n";
    
    if (detailed) {
        ss << "Detailed profile data:\n";
        for (const auto& pair : pImpl->profileTimes) {
            ss << "  " << pair.first << ": " << pair.second << "s\n";
        }
    }
    
    // Show function statistics
    if (detailed && !pImpl->profileTimes.empty()) {
        ss << "\nFunction Statistics:\n";
        for (const auto& pair : pImpl->profileTimes) {
            int callCount = pImpl->profileCallCounts.count(pair.first) ? 
                          pImpl->profileCallCounts.at(pair.first) : 0;
            double avgTime = callCount > 0 ? pair.second / callCount : 0;
            ss << "  " << pair.first << ": " << pair.second << "s (" << callCount << " calls, " 
               << avgTime << "s avg)\n";
        }
    }
    
    return ss.str();
}

std::vector<std::string> PerformanceProfiler::getProfileNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->profileTimes) {
        names.push_back(pair.first);
    }
    return names;
}

void PerformanceProfiler::clearProfile(const std::string& name) {
    if (name.empty()) {
        pImpl->profileStarts.clear();
        pImpl->profileStops.clear();
        pImpl->profileTimes.clear();
        pImpl->profileCallCounts.clear();
        pImpl->allocationSizes.clear();
        pImpl->deallocationSizes.clear();
        pImpl->cpuCycles.clear();
        pImpl->totalAllocations = 0;
        pImpl->totalDeallocations = 0;
        pImpl->totalCPUCycles = 0;
    } else {
        // Clear specific profile
        for (auto it = pImpl->profileTimes.begin(); it != pImpl->profileTimes.end(); ) {
            if (it->first.find(name) != std::string::npos) {
                it = pImpl->profileTimes.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void PerformanceProfiler::reset() {
    pImpl->profileStarts.clear();
    pImpl->profileStops.clear();
    pImpl->profileTimes.clear();
    pImpl->profileCallCounts.clear();
    pImpl->allocationSizes.clear();
    pImpl->deallocationSizes.clear();
    pImpl->cpuCycles.clear();
    pImpl->totalAllocations = 0;
    pImpl->totalDeallocations = 0;
    pImpl->totalCPUCycles = 0;
}

} // namespace nlm

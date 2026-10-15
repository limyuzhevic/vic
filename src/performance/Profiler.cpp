#include "Profiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <filesystem>

namespace nlm {

Profiler::Profiler() 
    : isProfiling_(false)
    , startTime_(0)
    , totalProfileTime_(0.0)
{
}

Profiler::~Profiler() {
    resetProfile();
}

void Profiler::startProfiling() {
    if (isProfiling_) {
        NLM_LOG_WARNING("Profiler is already running");
        return;
    }
    
    isProfiling_ = true;
    startTime_ = 0;  // Would get actual timestamp
    
    NLM_LOG_INFO("Profiler started");
}

void Profiler::stopProfiling() {
    if (!isProfiling_) {
        return;
    }
    
    isProfiling_ = false;
    auto endTime = 0;  // Would get actual timestamp
    totalProfileTime_ = endTime - startTime_;
    
    NLM_LOG_INFO("Profiler stopped");
}

void Profiler::profileLine(const std::string& fileName, int lineNumber, 
                          const std::string& functionName, double duration,
                          uint64_t memoryAllocated, uint64_t functionCallCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find existing line profile entry
    auto it = std::find_if(lineProfileData_.begin(), lineProfileData_.end(),
        [&](const LineProfileData& entry) {
            return entry.fileName == fileName && 
                   entry.lineNumber == lineNumber && 
                   entry.functionName == functionName;
        });
    
    if (it != lineProfileData_.end()) {
        // Update existing entry
        it->executionCount += functionCallCount;
        it->totalTime += duration;
        it->averageTime = it->totalTime / it->executionCount;
        it->minTime = std::min(it->minTime, duration);
        it->maxTime = std::max(it->maxTime, duration);
        it->memoryAllocated += memoryAllocated;
        it->functionCallCount += functionCallCount;
    } else {
        // Create new entry
        LineProfileData entry;
        entry.fileName = fileName;
        entry.lineNumber = lineNumber;
        entry.functionName = functionName;
        entry.executionCount = functionCallCount;
        entry.totalTime = duration;
        entry.averageTime = duration;
        entry.minTime = duration;
        entry.maxTime = duration;
        entry.memoryAllocated = memoryAllocated;
        entry.functionCallCount = functionCallCount;
        entry.timestamp = startTime_;
        
        lineProfileData_.push_back(entry);
    }
}

void Profiler::profileFunctionEntry(const std::string& functionName, 
                                   const std::string& fileName, int lineNumber) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    callStack_.push(functionName);
    
    // Find or create function entry
    auto it = std::find_if(functionProfileData_.begin(), functionProfileData_.end(),
        [&](const FunctionProfileData& entry) {
            return entry.functionName == functionName && 
                   entry.fileName == fileName && 
                   entry.lineNumber == lineNumber;
        });
    
    if (it != functionProfileData_.end()) {
        it->callCount++;
    } else {
        FunctionProfileData entry;
        entry.functionName = functionName;
        entry.fileName = fileName;
        entry.lineNumber = lineNumber;
        entry.callCount = 1;
        entry.totalTime = 0.0;
        entry.averageTime = 0.0;
        entry.cpuTime = 0.0;
        entry.memoryAllocated = 0;
        entry.memoryDeallocated = 0;
        entry.nestedCallCount = 0;
        entry.timestamp = startTime_;
        
        functionProfileData_.push_back(entry);
    }
}

void Profiler::profileFunctionExit(const std::string& functionName, double duration) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!callStack_.empty()) {
        callStack_.pop();
    }
    
    // Find function entry and update
    auto it = std::find_if(functionProfileData_.begin(), functionProfileData_.end(),
        [&](const FunctionProfileData& entry) {
            return entry.functionName == functionName;
        });
    
    if (it != functionProfileData_.end()) {
        it->totalTime += duration;
        it->averageTime = it->totalTime / it->callCount;
        it->cpuTime += duration;
    }
}

void Profiler::profileMemoryAllocation(void* address, size_t size,
                                       const std::string& allocationSite) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryAllocationProfile profile;
    profile.address = address;
    profile.size = size;
    profile.allocationSite = allocationSite;
    profile.timestamp = startTime_;
    profile.threadId = std::this_thread::get_id();
    profile.callStack = callStack_;
    profile.allocationSequence = memoryAllocationProfile_.size();
    
    memoryAllocationProfile_.push_back(profile);
}

void Profiler::profileMemoryDeallocation(void* address) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find the allocation profile
    auto it = std::find_if(memoryAllocationProfile_.begin(), memoryAllocationProfile_.end(),
        [&](const MemoryAllocationProfile& profile) {
            return profile.address == address;
        });
    
    if (it != memoryAllocationProfile_.end()) {
        // Mark as deallocated (in a real implementation, you'd remove it)
        // For now, we'll just leave it in the list
    }
}

void Profiler::profileSpike(Timestamp spikeTime, uint64_t neuronId,
                            double membranePotential, const std::string& neuronType,
                            const std::string& region, Timestamp previousSpikeTime,
                            double synapticInput) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    SpikeTimingProfile profile;
    profile.spikeTime = spikeTime;
    profile.neuronId = neuronId;
    profile.membranePotential = membranePotential;
    profile.neuronType = neuronType;
    profile.region = region;
    profile.previousSpikeTime = previousSpikeTime;
    profile.interSpikeInterval = spikeTime - previousSpikeTime;
    profile.synapticInput = synapticInput;
    profile.timestamp = startTime_;
    
    spikeProfile_.push_back(profile);
}

void Profiler::profilePlasticityEvent(Timestamp eventTime,
                                      const std::string& plasticityType,
                                      uint64_t synapseId, uint64_t preNeuronId,
                                      uint64_t postNeuronId, double weightChange,
                                      double learningRate, double eligibilityTrace) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PlasticityEventProfile profile;
    profile.eventTime = eventTime;
    profile.plasticityType = plasticityType;
    profile.synapseId = synapseId;
    profile.preNeuronId = preNeuronId;
    profile.postNeuronId = postNeuronId;
    profile.weightChange = weightChange;
    profile.learningRate = learningRate;
    profile.eligibilityTrace = eligibilityTrace;
    profile.timestamp = startTime_;
    
    plasticityProfile_.push_back(profile);
}

void Profiler::profileNeuromodulation(Timestamp timestamp,
                                       const std::string& neuromodulatorType,
                                       double level, double effectMagnitude,
                                       uint64_t targetNeuronId, 
                                       const std::string& targetRegion,
                                       Timestamp previousReleaseTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    NeuromodulationProfile profile;
    profile.timestamp = timestamp;
    profile.neuromodulatorType = neuromodulatorType;
    profile.level = level;
    profile.effectMagnitude = effectMagnitude;
    profile.targetNeuronId = targetNeuronId;
    profile.targetRegion = targetRegion;
    profile.previousReleaseTime = previousReleaseTime;
    profile.timeSincePrevious = timestamp - previousReleaseTime;
    
    neuromodulationProfile_.push_back(profile);
}

void Profiler::updateFunctionCallersCallees(const std::string& caller, const std::string& callee) {
    // Update callers and callees relationships
    // This would be called when function calls are detected
}

void Profiler::calculateFunctionStatistics(FunctionProfileData& funcData) const {
    // Calculate statistics for function profile
}

void Profiler::calculateLineStatistics(LineProfileData& lineData) const {
    // Calculate statistics for line profile
}

std::vector<LineProfileData> Profiler::getLineProfileData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lineProfileData_;
}

std::vector<FunctionProfileData> Profiler::getFunctionProfileData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return functionProfileData_;
}

std::vector<MemoryAllocationProfile> Profiler::getMemoryAllocationProfile() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memoryAllocationProfile_;
}

std::vector<SpikeTimingProfile> Profiler::getSpikeProfile() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return spikeProfile_;
}

std::vector<PlasticityEventProfile> Profiler::getPlasticityProfile() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plasticityProfile_;
}

std::vector<NeuromodulationProfile> Profiler::getNeuromodulationProfile() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return neuromodulationProfile_;
}

std::vector<HotspotInfo> Profiler::detectHotspots() const {
    std::vector<HotspotInfo> hotspots;
    
    // Detect hot spots based on line profile data
    for (const auto& lineProfile : lineProfileData_) {
        if (lineProfile.totalTime > 0.0) {
            HotspotInfo hotspot;
            hotspot.location = lineProfile.fileName + ":" + std::to_string(lineProfile.lineNumber);
            hotspot.functionName = lineProfile.functionName;
            hotspot.cumulativeTime = lineProfile.totalTime;
            hotspot.percentageOfTotal = (lineProfile.totalTime / totalProfileTime_) * 100.0;
            hotspot.severity = calculateHotspotSeverity(lineProfile.totalTime, lineProfile.executionCount);
            hotspot.recommendation = suggestOptimization(lineProfile);
            
            hotspots.push_back(hotspot);
        }
    }
    
    // Sort by cumulative time (descending)
    std::sort(hotspots.begin(), hotspots.end(),
        [](const HotspotInfo& a, const HotspotInfo& b) {
            return a.cumulativeTime > b.cumulativeTime;
        });
    
    return hotspots;
}

std::vector<MemoryLeakInfo> Profiler::detectMemoryLeaks() const {
    std::vector<MemoryLeakInfo> leaks;
    
    // Detect memory leaks based on allocation profile
    for (const auto& allocation : memoryAllocationProfile_) {
        // Check if allocation was not deallocated
        // In a real implementation, you would track deallocations
        // For now, we'll just mark all allocations as potential leaks
        if (allocation.allocationSequence % 10 == 0) {  // Simplified detection
            MemoryLeakInfo leak;
            leak.allocationSite = allocation.allocationSite;
            leak.leakedSize = allocation.size;
            leak.allocationCount = 1;  // Would be incremented in real implementation
            leak.averageAllocationSize = allocation.size;
            leak.leakProbability = 0.1;  // Would be calculated in real implementation
            leak.suggestedFix = "Check for missing deallocation or circular references";
            
            leaks.push_back(leak);
        }
    }
    
    return leaks;
}

void Profiler::exportProfile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for profile export: " + filename);
        return;
    }
    
    // Export line profile data
    file << "Line Profile Data:\n";
    for (const auto& lineProfile : lineProfileData_) {
        file << lineProfile.fileName << ":" << lineProfile.lineNumber << 
             " (" << lineProfile.functionName << "): " <<
             lineProfile.executionCount << " executions, " <<
             lineProfile.totalTime << " seconds, " <<
             lineProfile.memoryAllocated << " bytes\n";
    }
    
    file << "\nFunction Profile Data:\n";
    for (const auto& funcProfile : functionProfileData_) {
        file << funcProfile.functionName << " (" << funcProfile.fileName << ":" << 
             funcProfile.lineNumber << "): " <<
             funcProfile.callCount << " calls, " <<
             funcProfile.totalTime << " seconds, " <<
             funcProfile.memoryAllocated << " bytes allocated\n";
    }
    
    file << "\nMemory Allocation Profile:\n";
    for (const auto& allocProfile : memoryAllocationProfile_) {
        file << allocProfile.allocationSite << ": " <<
             allocProfile.size << " bytes at " << allocProfile.address << "\n";
    }
    
    file << "\nSpike Profile:\n";
    for (const auto& spikeProfile : spikeProfile_) {
        file << "Neuron " << spikeProfile.neuronId << " at time " <<
             spikeProfile.spikeTime << " ms, " <<
             spikeProfile.neuronType << " neuron\n";
    }
    
    file << "\nPlasticity Event Profile:\n";
    for (const auto& plasticityProfile : plasticityProfile_) {
        file << plasticityProfile.plasticityType << " event at time " <<
             plasticityProfile.eventTime << " ms, " <<
             "weight change: " << plasticityProfile.weightChange << "\n";
    }
    
    file << "\nNeuromodulation Profile:\n";
    for (const auto& neuromodProfile : neuromodulationProfile_) {
        file << neuromodProfile.neuromodulatorType << " level: " <<
             neuromodProfile.level << " at time " << neuromodProfile.timestamp << "\n";
    }
    
    NLM_LOG_INFO("Profile exported to: " + filename);
}

void Profiler::importProfile(const std::string& filename) {
    // Implement profile import
    NLM_LOG_WARNING("Profile import not yet implemented");
}

void Profiler::clearLineProfileData() {
    std::lock_guard<std::mutex> lock(mutex_);
    lineProfileData_.clear();
}

void Profiler::clearFunctionProfileData() {
    std::lock_guard<std::mutex> lock(mutex_);
    functionProfileData_.clear();
}

void Profiler::clearMemoryAllocationProfile() {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryAllocationProfile_.clear();
}

void Profiler::clearSpikeProfile() {
    std::lock_guard<std::mutex> lock(mutex_);
    spikeProfile_.clear();
}

void Profiler::clearPlasticityProfile() {
    std::lock_guard<std::mutex> lock(mutex_);
    plasticityProfile_.clear();
}

void Profiler::clearNeuromodulationProfile() {
    std::lock_guard<std::mutex> lock(mutex_);
    neuromodulationProfile_.clear();
}

size_t Profiler::getLineProfileCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lineProfileData_.size();
}

size_t Profiler::getFunctionProfileCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return functionProfileData_.size();
}

size_t Profiler::getMemoryAllocationCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memoryAllocationProfile_.size();
}

size_t Profiler::getSpikeProfileCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return spikeProfile_.size();
}

size_t Profiler::getPlasticityEventCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return plasticityProfile_.size();
}

size_t Profiler::getNeuromodulationEventCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return neuromodulationProfile_.size();
}

double Profiler::getTotalProfileTime() const {
    return totalProfileTime_;
}

void Profiler::resetProfile() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    lineProfileData_.clear();
    functionProfileData_.clear();
    memoryAllocationProfile_.clear();
    spikeProfile_.clear();
    plasticityProfile_.clear();
    neuromodulationProfile_.clear();
    callStack_ = std::stack<std::string>();
    
    totalProfileTime_ = 0.0;
}

int Profiler::calculateHotspotSeverity(double totalTime, uint64_t executionCount) const {
    // Calculate hotspot severity (simplified)
    if (totalTime > 1.0 && executionCount > 100) {
        return 5;
    } else if (totalTime > 0.5 && executionCount > 50) {
        return 4;
    } else if (totalTime > 0.1 && executionCount > 10) {
        return 3;
    } else if (totalTime > 0.01) {
        return 2;
    } else {
        return 1;
    }
}

std::string Profiler::suggestOptimization(const LineProfileData& lineData) const {
    // Suggest optimization (simplified)
    if (lineData.memoryAllocated > 1024 * 1024) {  // 1MB
        return "Consider reducing memory allocation at this line";
    } else if (lineData.totalTime > 0.1) {
        return "Consider optimizing this hot path";
    } else if (lineData.executionCount > 1000) {
        return "Consider caching results or reducing loop iterations";
    } else {
        return "Minor optimization opportunity";
    }
}

} // namespace nlm
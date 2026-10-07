// ProfilingDataCollector.cpp - Implementation of Profiling Data Collection

#include "ProfilingDataCollector.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <pugixml.hpp>
#include <zlib.h>
#include <lz4.h>
#include <regex>

namespace nlm {

// Helper functions for system data collection
double getSystemCPUUsage() {
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__APPLE__)
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        std::string line;
        if (std::getline(statFile, line)) {
            std::istringstream iss(line);
            std::string cpu;
            unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
            if (iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) {
                unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
                unsigned long long idleTotal = idle + iowait;
                static unsigned long long lastTotal = 0;
                static unsigned long long lastIdle = 0;
                
                if (lastTotal > 0) {
                    double idleDiff = idleTotal - lastIdle;
                    double totalDiff = total - lastTotal;
                    return (1.0 - (idleDiff / totalDiff)) * 100.0;
                }
                
                lastTotal = total;
                lastIdle = idleTotal;
            }
        }
    }
#endif
    return 0.0;
}

size_t getSystemMemoryUsage() {
#if defined(__linux__) || defined(__APPLE__)
    std::ifstream meminfoFile("/proc/meminfo");
    if (meminfoFile.is_open()) {
        std::string line;
        size_t memTotal = 0;
        size_t memAvailable = 0;
        while (std::getline(meminfoFile, line)) {
            if (line.find("MemTotal:") != std::string::npos) {
                memTotal = static_cast<size_t>(std::stoull(line.substr(line.find(":") + 1)) * 1024);
            } else if (line.find("MemAvailable:") != std::string::npos) {
                memAvailable = static_cast<size_t>(std::stoull(line.substr(line.find(":") + 1)) * 1024);
            }
        }
        return memTotal - memAvailable;
    }
#endif
    return 0;
}

size_t getSystemAvailableMemory() {
#if defined(__linux__) || defined(__APPLE__)
    std::ifstream meminfoFile("/proc/meminfo");
    if (meminfoFile.is_open()) {
        std::string line;
        while (std::getline(meminfoFile, line)) {
            if (line.find("MemAvailable:") != std::string::npos) {
                return static_cast<size_t>(std::stoull(line.substr(line.find(":") + 1)) * 1024);
            }
        }
    }
#endif
    return 0;
}

uint64_t getSystemThreadCount() {
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__APPLE__)
    return std::max(std::thread::hardware_concurrency(), static_cast<size_t>(1));
#else
    return 1;
#endif
}

uint64_t getSystemDiskIO(const std::string& file) {
#if defined(__linux__) || defined(__linux) || defined(linux)
    std::ifstream ioFile(file);
    if (ioFile.is_open()) {
        std::string line;
        while (std::getline(ioFile, line)) {
            if (line.find("read_bytes") != std::string::npos) {
                try {
                    return static_cast<uint64_t>(std::stoull(line.substr(line.find(":") + 1)));
                } catch (...) {
                    return 0;
                }
            }
        }
    }
#endif
    return 0;
}

// SystemDataCollector implementation
SystemDataCollector::SystemDataCollector(const std::string& name) : collectorName_(name), running_(false) {
}

std::vector<ProfileDataPoint> SystemDataCollector::collectData(uint64_t startTime, uint64_t endTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (const auto& state : systemHistory_) {
        if (state.timestamp >= startTime && state.timestamp <= endTime) {
            ProfileDataPoint event(state.timestamp, ProfileEventType::CPU_CYCLE, 
                                "system_monitor.cpp", 0, "sampleSystemState", "system");
            event.eventData["cpu_usage_percent"] = state.cpuUsagePercent;
            event.eventData["memory_usage_bytes"] = static_cast<int64_t>(state.memoryUsageBytes);
            event.eventData["available_memory_bytes"] = static_cast<int64_t>(state.availableMemoryBytes);
            event.eventData["active_threads"] = static_cast<size_t>(state.activeThreads);
            event.eventData["total_threads"] = static_cast<size_t>(state.totalThreads);
            event.eventData["disk_reads"] = static_cast<int64_t>(state.diskReads);
            event.eventData["disk_writes"] = static_cast<int64_t>(state.diskWrites);
            event.eventData["network_send_rate"] = state.networkSendRate;
            event.eventData["network_receive_rate"] = state.networkReceiveRate;
            event.eventData["cpu_temperature_celsius"] = state.cpuTemperatureCelsius;
            event.eventData["cpu_frequency_mhz"] = state.cpuFrequencyMHz;
            
            result.push_back(event);
        }
    }
    
    return result;
}

std::vector<ProfileDataPoint> SystemDataCollector::collectData(const std::vector<uint64_t>& timestamps) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (uint64_t timestamp : timestamps) {
        // Find the closest system state
        auto it = std::lower_bound(systemHistory_.begin(), systemHistory_.end(), timestamp,
                                 [](const SystemState& state, uint64_t ts) { return state.timestamp < ts; });n        
        SystemState state;
        if (it != systemHistory_.end()) {
            state = *it;
        } else if (!systemHistory_.empty()) {
            state = systemHistory_.back();
        } else {
            state.timestamp = timestamp;
        }
        
        ProfileDataPoint event(timestamp, ProfileEventType::CPU_CYCLE, 
                            "system_monitor.cpp", 0, "sampleSystemState", "system");
        event.eventData["cpu_usage_percent"] = state.cpuUsagePercent;
        event.eventData["memory_usage_bytes"] = static_cast<int64_t>(state.memoryUsageBytes);
        event.eventData["available_memory_bytes"] = static_cast<int64_t>(state.availableMemoryBytes);
        event.eventData["active_threads"] = static_cast<size_t>(state.activeThreads);
        event.eventData["total_threads"] = static_cast<size_t>(state.totalThreads);
        
        result.push_back(event);
    }
    
    return result;
}

std::string SystemDataCollector::getCollectorStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "System Collector Status:\n";
    ss << "  Collector Name: " << collectorName_ << "\n";
    ss << "  Running: " << (running_ ? "Yes" : "No") << "\n";
    ss << "  Data Points Collected: " << collectedCount_.load() << "\n";
    ss << "  Data Points Dropped: " << droppedCount_.load() << "\n";
    ss << "  Current Memory Usage: " << getCurrentMemoryUsage() << " bytes\n";
    ss << "  Peak Memory Usage: " << peakMemoryUsage_.load() << " bytes\n";
    
    return ss.str();
}

void SystemDataCollector::start() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
    lastDiskUpdate_ = startTime_;
    
    // Start system monitoring thread
    systemMonitorThread_ = std::thread(&SystemDataCollector::monitorSystem, this);
}

void SystemDataCollector::stop() {
    if (!running_.load()) return;
    
    running_.store(false);
    if (systemMonitorThread_.joinable()) {
        systemMonitorThread_.join();
    }
}

std::vector<ProfileDataPoint> SystemDataCollector::getRecentData(size_t maxCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= systemHistory_.size()) {
        std::vector<ProfileDataPoint> result;
        for (const auto& state : systemHistory_) {
            ProfileDataPoint event(state.timestamp, ProfileEventType::CPU_CYCLE,
                                "system_monitor.cpp", 0, "sampleSystemState", "system");
            event.eventData["cpu_usage_percent"] = state.cpuUsagePercent;
            event.eventData["memory_usage_bytes"] = static_cast<int64_t>(state.memoryUsageBytes);
            result.push_back(event);
        }
        return result;
    }
    
    size_t startIndex = systemHistory_.size() - maxCount;
    std::vector<ProfileDataPoint> result;
    for (size_t i = startIndex; i < systemHistory_.size(); ++i) {
        const auto& state = systemHistory_[i];
        ProfileDataPoint event(state.timestamp, ProfileEventType::CPU_CYCLE,
                            "system_monitor.cpp", 0, "sampleSystemState", "system");
        event.eventData["cpu_usage_percent"] = state.cpuUsagePercent;
        event.eventData["memory_usage_bytes"] = static_cast<int64_t>(state.memoryUsageBytes);
        result.push_back(event);
    }
    return result;
}

void SystemDataCollector::monitorSystem() {
    while (running_.load()) {
        sampleSystemState();
        updateDiskIOCounters();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void SystemDataCollector::sampleSystemState() {
    SystemState state;
    auto now = std::chrono::high_resolution_clock::now();
    state.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    
    state.cpuUsagePercent = getSystemCPUUsage();
    state.memoryUsageBytes = getCurrentMemoryUsage();
    state.availableMemoryBytes = getCurrentAvailableMemory();
    state.activeThreads = getSystemThreadCount();
    state.totalThreads = getSystemThreadCount();
    state.cpuTemperatureCelsius = getCPUTemperature();
    state.cpuFrequencyMHz = getCPUFrequencyMHz();
    
    std::lock_guard<std::mutex> lock(mutex_);
    systemHistory_.push_back(state);
    
    // Cleanup old data
    auto cutoffTime = now - std::chrono::minutes(60); // Keep last hour
    auto cutoffTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(cutoffTime.time_since_epoch()).count();
    
    systemHistory_.erase(std::remove_if(systemHistory_.begin(), systemHistory_.end(),
                                       [cutoffTimestamp](const SystemState& s) { return s.timestamp < cutoffTimestamp; }),
                        systemHistory_.end());
    
    collectedCount_.fetch_add(1);
}

SystemState SystemDataCollector::getCurrentSystemState() const {
    SystemState state;
    auto now = std::chrono::high_resolution_clock::now();
    state.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    
    state.cpuUsagePercent = getSystemCPUUsage();
    state.memoryUsageBytes = getCurrentMemoryUsage();
    state.availableMemoryBytes = getCurrentAvailableMemory();
    state.activeThreads = getSystemThreadCount();
    state.totalThreads = getSystemThreadCount();
    state.cpuTemperatureCelsius = getCPUTemperature();
    state.cpuFrequencyMHz = getCPUFrequencyMHz();
    
    return state;
}

void SystemDataCollector::updateDiskIOCounters() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastDiskUpdate_);
    
    if (elapsed.count() >= 1) { // Update every second
        uint64_t currentReads = getSystemDiskIO("/sys/block/sda/stat");
        uint64_t currentWrites = getSystemDiskIO("/sys/block/sda/stat");
        
        // Update counters
        lastDiskReads_.store(currentReads);
        lastDiskWrites_.store(currentWrites);
        lastDiskUpdate_ = now;
    }
}

// SimulationDataCollector implementation
SimulationDataCollector::SimulationDataCollector(const std::string& name) : collectorName_(name), running_(false) {
}

std::vector<ProfileDataPoint> SimulationDataCollector::collectData(uint64_t startTime, uint64_t endTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (const auto& event : simulationEvents_) {
        if (event.timestamp >= startTime && event.timestamp <= endTime) {
            result.push_back(event);
        }
    }
    
    return result;
}

std::vector<ProfileDataPoint> SimulationDataCollector::collectData(const std::vector<uint64_t>& timestamps) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (uint64_t timestamp : timestamps) {
        // Find events around this timestamp
        auto it = std::lower_bound(simulationEvents_.begin(), simulationEvents_.end(), timestamp,
                                 [](const ProfileDataPoint& event, uint64_t ts) { return event.timestamp < ts; });n        
        // Add the event and its neighbors (up to 5 before, 5 after)
        size_t index = std::distance(simulationEvents_.begin(), it);
        size_t startIndex = std::max(static_cast<size_t>(0), index - 5);
        size_t endIndex = std::min(simulationEvents_.size(), index + 6);
        
        for (size_t i = startIndex; i < endIndex; ++i) {
            result.push_back(simulationEvents_[i]);
        }
    }
    
    return result;
}

std::string SimulationDataCollector::getCollectorStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "Simulation Collector Status:\n";
    ss << "  Collector Name: " << collectorName_ << "\n";
    ss << "  Running: " << (running_ ? "Yes" : "No") << "\n";
    ss << "  Data Points Collected: " << collectedCount_.load() << "\n";
    ss << "  Data Points Dropped: " << droppedCount_.load() << "\n";
    ss << "  Current Timestep: " << currentTimestep_ << "\n";
    ss << "  Event Counters: ";
    bool first = true;
    for (const auto& pair : eventCounters_) {
        if (!first) ss << ", ";
        ss << pair.first << "=" << pair.second;
        first = false;
    }
    
    return ss.str();
}

void SimulationDataCollector::start() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
}

void SimulationDataCollector::stop() {
    if (!running_.load()) return;
    
    running_.store(false);
}

std::vector<ProfileDataPoint> SimulationDataCollector::getRecentData(size_t maxCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= simulationEvents_.size()) {
        return simulationEvents_;
    }
    
    size_t startIndex = simulationEvents_.size() - maxCount;
    return std::vector<ProfileDataPoint>(simulationEvents_.begin() + startIndex, simulationEvents_.end());
}

void SimulationDataCollector::recordTimestepEvent(uint64_t timestep, double timestepTimeNs,
                                                  size_t spikesGenerated, size_t spikesPropagated,
                                                  size_t activeNeurons, size_t neuronUpdates,
                                                  size_t synapseUpdates, double memoryDeltaBytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    currentTimestep_ = timestep;
    
    ProfileDataPoint event;
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    event.eventType = ProfileEventType::TIMESTEP_END;
    event.sourceFile = "simulation_manager.cpp";
    event.sourceLine = 0;
    event.functionName = "processTimestep";
    event.threadId = getCurrentThreadId();
    event.eventData["timestep"] = static_cast<uint64_t>(timestep);
    event.eventData["wall_time_ns"] = timestepTimeNs;
    event.eventData["cpu_time_ns"] = timestepTimeNs; // Simplified
    event.eventData["memory_delta_bytes"] = memoryDeltaBytes;
    event.eventData["spikes_generated"] = static_cast<uint64_t>(spikesGenerated);
    event.eventData["spikes_propagated"] = static_cast<uint64_t>(spikesPropagated);
    event.eventData["active_neurons"] = static_cast<size_t>(activeNeurons);
    event.eventData["neuron_updates"] = static_cast<size_t>(neuronUpdates);
    event.eventData["synapse_updates"] = static_cast<size_t>(synapseUpdates);
    
    simulationEvents_.push_back(event);
    eventCounters_["timestep_end"]++;
    
    collectedCount_.fetch_add(1);
}

void SimulationDataCollector::recordNeuronEvent(const std::string& neuronId, double membranePotential,
                                                double synapticInput, bool isFiring, uint64_t spikeTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileDataPoint event;
    event.timestamp = spikeTime;
    event.eventType = isFiring ? ProfileEventType::SPIKE_GENERATION : ProfileEventType::NEURON_UPDATE;
    event.sourceFile = "neuron.cpp";
    event.sourceLine = 0;
    event.functionName = "updateNeuron";
    event.threadId = getCurrentThreadId();
    event.eventData["neuron_id"] = neuronId;
    event.eventData["membrane_potential"] = membranePotential;
    event.eventData["synaptic_input"] = synapticInput;
    event.eventData["is_firing"] = isFiring;
    event.eventData["spike_time"] = static_cast<uint64_t>(spikeTime);
    
    simulationEvents_.push_back(event);
    eventCounters_[isFiring ? "spike_generated" : "neuron_updated"]++;
    
    collectedCount_.fetch_add(1);
}

void SimulationDataCollector::recordSynapseEvent(const std::string& synapseId, size_t sourceId,
                                                 size_t targetId, double weight, bool isPlastic) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileDataPoint event;
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    event.eventType = ProfileEventType::SYNAPSE_UPDATE;
    event.sourceFile = "synapse.cpp";
    event.sourceLine = 0;
    event.functionName = "updateSynapse";
    event.threadId = getCurrentThreadId();
    event.eventData["synapse_id"] = synapseId;
    event.eventData["source_id"] = static_cast<uint64_t>(sourceId);
    event.eventData["target_id"] = static_cast<uint64_t>(targetId);
    event.eventData["weight"] = weight;
    event.eventData["is_plastic"] = isPlastic;
    
    simulationEvents_.push_back(event);
    eventCounters_["synapse_updated"]++;
    
    collectedCount_.fetch_add(1);
}

void SimulationDataCollector::recordSpikeEvent(const std::string& neuronId, uint64_t spikeTime,
                                               double synapticInput) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileDataPoint event;
    event.timestamp = spikeTime;
    event.eventType = ProfileEventType::SPIKE_PROPAGATION;
    event.sourceFile = "neuron.cpp";
    event.sourceLine = 0;
    event.functionName = "propagateSpike";
    event.threadId = getCurrentThreadId();
    event.eventData["neuron_id"] = neuronId;
    event.eventData["spike_time"] = static_cast<uint64_t>(spikeTime);
    event.eventData["synaptic_input"] = synapticInput;
    
    simulationEvents_.push_back(event);
    eventCounters_["spike_propagated"]++;
    
    collectedCount_.fetch_add(1);
}

// MemoryDataCollector implementation
MemoryDataCollector::MemoryDataCollector(const std::string& name) : collectorName_(name), running_(false) {
}

std::vector<ProfileDataPoint> MemoryDataCollector::collectData(uint64_t startTime, uint64_t endTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (const auto& alloc : memoryEvents_) {
        if (alloc.timestamp >= startTime && alloc.timestamp <= endTime) {
            result.push_back(alloc);
        }
    }
    
    return result;
}

std::vector<ProfileDataPoint> MemoryDataCollector::collectData(const std::vector<uint64_t>& timestamps) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (uint64_t timestamp : timestamps) {
        // Find memory events around this timestamp
        auto it = std::lower_bound(memoryEvents_.begin(), memoryEvents_.end(), timestamp,
                                 [](const MemoryAllocationInfo& alloc, uint64_t ts) { return alloc.timestamp < ts; });n        
        // Add the event and its neighbors (up to 3 before, 3 after)
        size_t index = std::distance(memoryEvents_.begin(), it);
        size_t startIndex = std::max(static_cast<size_t>(0), index - 3);
        size_t endIndex = std::min(memoryEvents_.size(), index + 4);
        
        for (size_t i = startIndex; i < endIndex; ++i) {
            result.push_back(memoryEvents_[i]);
        }
    }
    
    return result;
}

std::string MemoryDataCollector::getCollectorStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "Memory Collector Status:\n";
    ss << "  Collector Name: " << collectorName_ << "\n";
    ss << "  Running: " << (running_ ? "Yes" : "No") << "\n";
    ss << "  Data Points Collected: " << collectedCount_.load() << "\n";
    ss << "  Data Points Dropped: " << droppedCount_.load() << "\n";
    ss << "  Current Memory Usage: " << getCurrentMemoryUsage() << " bytes\n";
    ss << "  Peak Memory Usage: " << getPeakMemoryUsage() << " bytes\n";
    ss << "  Total Allocations: " << getTotalAllocations() << "\n";
    ss << "  Total Deallocations: " << getTotalDeallocations() << "\n";
    ss << "  Memory Leaks Detected: " << getMemoryLeaksDetected() << "\n";
    
    return ss.str();
}

void MemoryDataCollector::start() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
    lastLeakCheck_ = startTime_;
}

void MemoryDataCollector::stop() {
    if (!running_.load()) return;
    
    running_.store(false);
}

std::vector<ProfileDataPoint> MemoryDataCollector::getRecentData(size_t maxCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= memoryEvents_.size()) {
        return memoryEvents_;
    }
    
    size_t startIndex = memoryEvents_.size() - maxCount;
    return std::vector<ProfileDataPoint>(memoryEvents_.begin() + startIndex, memoryEvents_.end());
}

void MemoryDataCollector::recordAllocation(size_t size, const char* file, size_t line,
                                           const char* function, bool isPool, uint32_t poolId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryAllocationInfo alloc;
    auto now = std::chrono::high_resolution_clock::now();
    alloc.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    alloc.size = size;
    alloc.file = file;
    alloc.line = line;
    alloc.function = function;
    alloc.threadId = getCurrentThreadId();
    alloc.isPool = isPool;
    alloc.poolId = poolId;
    
    memoryEvents_.push_back(alloc);
    
    // Track live allocations for leak detection
    liveAllocations_[reinterpret_cast<void*>(alloc.timestamp)] = alloc;
    
    totalAllocations_.fetch_add(1);
    currentMemoryUsage_.fetch_add(size);
    
    // Update peak memory
    size_t currentMem = currentMemoryUsage_.load();
    size_t peakMem = peakMemoryUsage_.load();
    if (currentMem > peakMem) {
        peakMemoryUsage_.store(currentMem);
    }
    
    collectedCount_.fetch_add(1);
}

void MemoryDataCollector::recordDeallocation(size_t size, const char* file, size_t line,
                                             const char* function, bool isPool, uint32_t poolId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryAllocationInfo alloc;
    auto now = std::chrono::high_resolution_clock::now();
    alloc.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    alloc.size = -static_cast<long long>(size);
    alloc.file = file;
    alloc.line = line;
    alloc.function = function;
    alloc.threadId = getCurrentThreadId();
    alloc.isPool = isPool;
    alloc.poolId = poolId;
    
    memoryEvents_.push_back(alloc);
    
    // Remove from live allocations
    for (auto it = liveAllocations_.begin(); it != liveAllocations_.end(); ) {
        if (it->second.timestamp == alloc.timestamp - 1) { // Previous allocation
            it = liveAllocations_.erase(it);
            break;
        } else {
            ++it;
        }
    }
    
    totalDeallocations_.fetch_add(1);
    currentMemoryUsage_.fetch_sub(size);
    
    collectedCount_.fetch_add(1);
}

void MemoryDataCollector::recordMemoryAccess(void* address, size_t size, bool isWrite,
                                             const char* file, size_t line, const char* function) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileDataPoint event;
    auto now = std::chrono::high_resolution_clock::now();
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    event.eventType = ProfileEventType::MEMORY_ACCESS;
    event.sourceFile = file;
    event.sourceLine = line;
    event.functionName = function;
    event.threadId = getCurrentThreadId();
    event.eventData["address"] = reinterpret_cast<uintptr_t>(address);
    event.eventData["size"] = static_cast<size_t>(size);
    event.eventData["is_write"] = isWrite;
    
    memoryAccessEvents_.push_back(event);
    collectedCount_.fetch_add(1);
}

void MemoryDataCollector::detectMemoryLeaks() {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastLeakCheck_);
    
    if (elapsed.count() >= 60) { // Check every minute
        lastLeakCheck_ = now;
        
        size_t leaks = 0;
        for (const auto& pair : liveAllocations_) {
            const auto& alloc = pair.second;
            auto allocTime = std::chrono::steady_clock::time_point(
                std::chrono::nanoseconds(alloc.timestamp));
            auto timeSinceAlloc = std::chrono::duration_cast<std::chrono::seconds>(
                now - allocTime).count();
            
            // Consider allocations older than 1 second as potential leaks
            if (timeSinceAlloc > 1.0) {
                leaks++;
            }
        }
        
        memoryLeaksDetected_.store(leaks);
    }
}

// ThreadDataCollector implementation
ThreadDataCollector::ThreadDataCollector(const std::string& name) : collectorName_(name), running_(false) {
}

std::vector<ProfileDataPoint> ThreadDataCollector::collectData(uint64_t startTime, uint64_t endTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (const auto& metrics : threadEvents_) {
        if (metrics.timestamp >= startTime && metrics.timestamp <= endTime) {
            result.push_back(metrics);
        }
    }
    
    return result;
}

std::vector<ProfileDataPoint> ThreadDataCollector::collectData(const std::vector<uint64_t>& timestamps) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (uint64_t timestamp : timestamps) {
        // Find thread events around this timestamp
        auto it = std::lower_bound(threadEvents_.begin(), threadEvents_.end(), timestamp,
                                 [](const ThreadPerformanceMetrics& metrics, uint64_t ts) { return metrics.timestamp < ts; });n        
        // Add the event and its neighbors (up to 3 before, 3 after)
        size_t index = std::distance(threadEvents_.begin(), it);
        size_t startIndex = std::max(static_cast<size_t>(0), index - 3);
        size_t endIndex = std::min(threadEvents_.size(), index + 4);
        
        for (size_t i = startIndex; i < endIndex; ++i) {
            result.push_back(threadEvents_[i]);
        }
    }
    
    return result;
}

std::string ThreadDataCollector::getCollectorStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "Thread Collector Status:\n";
    ss << "  Collector Name: " << collectorName_ << "\n";
    ss << "  Running: " << (running_ ? "Yes" : "No") << "\n";
    ss << "  Data Points Collected: " << collectedCount_.load() << "\n";
    ss << "  Data Points Dropped: " << droppedCount_.load() << "\n";
    ss << "  Total Threads: " << getTotalThreadsCount() << "\n";
    ss << "  Active Threads: " << activeThreads_.size() << "\n";
    
    return ss.str();
}

void ThreadDataCollector::start() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
    lastThreadUpdate_ = startTime_;
}

void ThreadDataCollector::stop() {
    if (!running_.load()) return;
    
    running_.store(false);
}

std::vector<ProfileDataPoint> ThreadDataCollector::getRecentData(size_t maxCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= threadEvents_.size()) {
        return threadEvents_;
    }
    
    size_t startIndex = threadEvents_.size() - maxCount;
    return std::vector<ProfileDataPoint>(threadEvents_.begin() + startIndex, threadEvents_.end());
}

void ThreadDataCollector::recordThreadCreation(const std::string& threadId, size_t stackSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ThreadPerformanceMetrics metrics(threadId, std::stoull(threadId));
    metrics.minStackSize = stackSize;
    metrics.maxStackSize = stackSize;
    metrics.currentStackSize = stackSize;
    metrics.activeTimePercent = 100.0;
    
    threadEvents_.push_back(metrics);
    activeThreads_[threadId] = metrics;
    totalThreads_.fetch_add(1);
    
    collectedCount_.fetch_add(1);
}

void ThreadDataCollector::recordThreadDestruction(const std::string& threadId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeThreads_.find(threadId);
    if (it != activeThreads_.end()) {
        threadEvents_.push_back(it->second);
        activeThreads_.erase(it);
        collectedCount_.fetch_add(1);
    }
}

void ThreadDataCollector::recordThreadSwitch(const std::string& fromThread, const std::string& toThread,
                                             double waitTimeNs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileDataPoint event;
    auto now = std::chrono::high_resolution_clock::now();
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    event.eventType = ProfileEventType::THREAD_SWITCH;
    event.sourceFile = "scheduler.cpp";
    event.sourceLine = 0;
    event.functionName = "scheduleThread";
    event.threadId = toThread;
    event.eventData["from_thread"] = fromThread;
    event.eventData["to_thread"] = toThread;
    event.eventData["wait_time_ns"] = waitTimeNs;
    
    threadAccessEvents_.push_back(event);
    collectedCount_.fetch_add(1);
}

void ThreadDataCollector::recordLockAcquisition(const std::string& threadId, const std::string& lockId,
                                                double waitTimeNs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileDataPoint event;
    auto now = std::chrono::high_resolution_clock::now();
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    event.eventType = ProfileEventType::THREAD_LOCK_ACQUIRE;
    event.sourceFile = "lock_manager.cpp";
    event.sourceLine = 0;
    event.functionName = "acquireLock";
    event.threadId = threadId;
    event.eventData["lock_id"] = lockId;
    event.eventData["wait_time_ns"] = waitTimeNs;
    
    threadAccessEvents_.push_back(event);
    collectedCount_.fetch_add(1);
}

void ThreadDataCollector::recordLockRelease(const std::string& threadId, const std::string& lockId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileDataPoint event;
    auto now = std::chrono::high_resolution_clock::now();
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    event.eventType = ProfileEventType::THREAD_LOCK_RELEASE;
    event.sourceFile = "lock_manager.cpp";
    event.sourceLine = 0;
    event.functionName = "releaseLock";
    event.threadId = threadId;
    event.eventData["lock_id"] = lockId;
    
    threadAccessEvents_.push_back(event);
    collectedCount_.fetch_add(1);
}

void ThreadDataCollector::updateThreadMetrics(const std::string& threadId, 
                                             const ThreadPerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    activeThreads_[threadId] = metrics;
    threadEvents_.push_back(metrics);
    
    collectedCount_.fetch_add(1);
}

void ThreadDataCollector::detectRaceConditions() {
    // Simple race condition detection based on lock ordering
    std::lock_guard<std::mutex> lock(mutex_);
    
    // This is a simplified implementation
    // In a real system, you would use more sophisticated algorithms
    std::unordered_set<std::string> locksHeld;
    
    for (const auto& pair : activeThreads_) {
        const auto& threadId = pair.first;
        const auto& metrics = pair.second;
        
        // Check for potential race conditions
        for (const auto& otherPair : activeThreads_) {
            if (threadId != otherPair.first) {
                const auto& otherMetrics = otherPair.second;
                // Simple check: if two threads have high lock contention on similar resources
                if (metrics.lockContentionEvents > 0 && otherMetrics.lockContentionEvents > 0) {
                    // Potential race condition detected
                    ProfileDataPoint event;
                    auto now = std::chrono::high_resolution_clock::now();
                    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
                    event.eventType = ProfileEventType::RACE_CONDITION;
                    event.sourceFile = "race_detector.cpp";
                    event.sourceLine = 0;
                    event.functionName = "detectRaceCondition";
                    event.threadId = threadId;
                    event.eventData["other_thread"] = otherPair.first;
                    event.eventData["severity"] = 0.5; // Simplified severity
                    
                    threadAccessEvents_.push_back(event);
                    collectedCount_.fetch_add(1);
                }
            }
        }
    }
}

// IODataCollector implementation
IODataCollector::IODataCollector(const std::string& name) : collectorName_(name), running_(false) {
}

std::vector<ProfileDataPoint> IODataCollector::collectData(uint64_t startTime, uint64_t endTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (const auto& metrics : ioEvents_) {
        if (metrics.timestamp >= startTime && metrics.timestamp <= endTime) {
            result.push_back(metrics);
        }
    }
    
    return result;
}

std::vector<ProfileDataPoint> IODataCollector::collectData(const std::vector<uint64_t>& timestamps) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ProfileDataPoint> result;
    
    for (uint64_t timestamp : timestamps) {
        // Find I/O events around this timestamp
        auto it = std::lower_bound(ioEvents_.begin(), ioEvents_.end(), timestamp,
                                 [](const IOMetrics& metrics, uint64_t ts) { return metrics.timestamp < ts; });n        
        // Add the event and its neighbors (up to 3 before, 3 after)
        size_t index = std::distance(ioEvents_.begin(), it);
        size_t startIndex = std::max(static_cast<size_t>(0), index - 3);
        size_t endIndex = std::min(ioEvents_.size(), index + 4);
        
        for (size_t i = startIndex; i < endIndex; ++i) {
            result.push_back(ioEvents_[i]);
        }
    }
    
    return result;
}

std::string IODataCollector::getCollectorStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "I/O Collector Status:\n";
    ss << "  Collector Name: " << collectorName_ << "\n";
    ss << "  Running: " << (running_ ? "Yes" : "No") << "\n";
    ss << "  Data Points Collected: " << collectedCount_.load() << "\n";
    ss << "  Data Points Dropped: " << droppedCount_.load() << "\n";
    ss << "  Monitored Files: " << monitoredFiles_.size() << "\n";
    ss << "  Monitored Devices: " << deviceMetrics_.size() << "\n";
    
    return ss.str();
}

void IODataCollector::start() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
    lastIOUpdate_ = startTime_;
}

void IODataCollector::stop() {
    if (!running_.load()) return;
    
    running_.store(false);
}

std::vector<ProfileDataPoint> IODataCollector::getRecentData(size_t maxCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= ioEvents_.size()) {
        return ioEvents_;
    }
    
    size_t startIndex = ioEvents_.size() - maxCount;
    return std::vector<ProfileDataPoint>(ioEvents_.begin() + startIndex, ioEvents_.end());
}

void IODataCollector::recordFileRead(const std::string& filePath, size_t bytes, double latencyNs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    monitoredFiles_.insert(filePath);
    
    ProfileDataPoint event;
    auto now = std::chrono::high_resolution_clock::now();
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    event.eventType = ProfileEventType::IO_READ;
    event.sourceFile = "file_io.cpp";
    event.sourceLine = 0;
    event.functionName = "readFile";
    event.threadId = getCurrentThreadId();
    event.eventData["file_path"] = filePath;
    event.eventData["bytes_read"] = static_cast<uint64_t>(bytes);
    event.eventData["latency_ns"] = latencyNs;
    
    ioAccessEvents_.push_back(event);
    collectedCount_.fetch_add(1);
}

void IODataCollector::recordFileWrite(const std::string& filePath, size_t bytes, double latencyNs) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    monitoredFiles_.insert(filePath);
    
    ProfileDataPoint event;
    auto now = std::chrono::high_resolution_clock::now();
    event.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    event.eventType = ProfileEventType::IO_WRITE;
    event.sourceFile = "file_io.cpp";
    event.sourceLine = 0;
    event.functionName = "writeFile";
    event.threadId = getCurrentThreadId();
    event.eventData["file_path"] = filePath;
    event.eventData["bytes_written"] = static_cast<uint64_t>(bytes);
    event.eventData["latency_ns"] = latencyNs;
    
    ioAccessEvents_.push_back(event);
    collectedCount_.fetch_add(1);
}

void IODataCollector::recordNetworkActivity(const std::string& interface, double sendRate, double receiveRate) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    IOMetrics metrics;
    auto now = std::chrono::high_resolution_clock::now();
    metrics.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    metrics.sendRate = sendRate;
    metrics.receiveRate = receiveRate;
    
    deviceMetrics_[interface] = metrics;
    ioEvents_.push_back(metrics);
    
    collectedCount_.fetch_add(1);
}

void IODataCollector::updateDeviceMetrics(const std::string& device, const IOMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    deviceMetrics_[device] = metrics;
}

// ProfilingDataCollector implementation
ProfilingDataCollector::ProfilingDataCollector(const std::string& name, 
                                             const DataCollectionConfig& config)
    : name_(name), config_(config), running_(false), paused_(false),
      totalEventsCollected_(0), totalEventsDropped_(0), memoryUsage_(0) {
    if (config_.enabled) {
        start();
    }
}

ProfilingDataCollector::~ProfilingDataCollector() {
    stop();
}

void ProfilingDataCollector::start() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
    
    // Create custom collectors
    if (config_.enabledSources.find(DataSourceType::SYSTEM) != config_.enabledSources.end()) {
        customCollectors_.push_back(std::make_unique<SystemDataCollector>(name_ + "_system"));
    }
    
    if (config_.enabledSources.find(DataSourceType::SIMULATION) != config_.enabledSources.end()) {
        customCollectors_.push_back(std::make_unique<SimulationDataCollector>(name_ + "_simulation"));
    }
    
    if (config_.enabledSources.find(DataSourceType::MEMORY) != config_.enabledSources.end()) {
        customCollectors_.push_back(std::make_unique<MemoryDataCollector>(name_ + "_memory"));
    }
    
    if (config_.enabledSources.find(DataSourceType::THREAD) != config_.enabledSources.end()) {
        customCollectors_.push_back(std::make_unique<ThreadDataCollector>(name_ + "_thread"));
    }
    
    if (config_.enabledSources.find(DataSourceType::IO) != config_.enabledSources.end()) {
        customCollectors_.push_back(std::make_unique<IODataCollector>(name_ + "_io"));
    }
    
    // Start custom collectors
    for (auto& collector : customCollectors_) {
        collector->start();
    }
}

void ProfilingDataCollector::stop() {
    if (!running_.load()) return;
    
    running_.store(false);
    paused_.store(false);
    
    // Stop custom collectors
    for (auto& collector : customCollectors_) {
        collector->stop();
    }
}

void ProfilingDataCollector::pause() {
    if (!running_.load() || paused_.load()) return;
    
    paused_.store(true);
}

void ProfilingDataCollector::resume() {
    if (!paused_.load()) return;
    
    paused_.store(false);
}

void ProfilingDataCollector::recordEvent(const ProfileDataPoint& event) {
    if (!running_.load() || paused_.load()) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if event should be dropped
    if (shouldDropEvent(event)) {
        totalEventsDropped_.fetch_add(1);
        return;
    }
    
    // Add to all indexes
    allEvents_.push_back(event);
    eventTypeIndex_[event.eventType].push_back(event);
    
    for (const auto& tag : event.tags) {
        tagIndex_[tag.first].push_back(event);
    }
    
    // Update statistics
    totalEventsCollected_.fetch_add(1);
    memoryUsage_ += sizeof(ProfileDataPoint);
    
    // Cleanup old data
    cleanupOldData();
}

void ProfilingDataCollector::recordTimestepEvent(const TimestepPerformance& ts) {
    std::lock_guard<std::mutex> lock(mutex_);
    timestepData_.push_back(ts);
}

void ProfilingDataCollector::recordMemoryAllocation(const MemoryAllocationInfo& alloc) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryAllocations_.push_back(alloc);
}

void ProfilingDataCollector::recordThreadMetrics(const ThreadPerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    threadData_.push_back(metrics);
}

void ProfilingDataCollector::recordIOMetrics(const IOMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    ioData_.push_back(metrics);
}

std::vector<ProfileDataPoint> ProfilingDataCollector::getEvents(ProfileEventType eventType,
                                                                uint64_t startTime, 
                                                                uint64_t endTime) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ProfileDataPoint> result;
    
    auto it = eventTypeIndex_.find(eventType);
    if (it != eventTypeIndex_.end()) {
        for (const auto& event : it->second) {
            if (event.timestamp >= startTime && event.timestamp <= endTime) {
                result.push_back(event);
            }
        }
    }
    
    return result;
}

std::vector<TimestepPerformance> ProfilingDataCollector::getTimestepData(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= timestepData_.size()) {
        return timestepData_;
    }
    
    size_t startIndex = timestepData_.size() - maxCount;
    return std::vector<TimestepPerformance>(timestepData_.begin() + startIndex, timestepData_.end());
}

std::vector<MemoryAllocationInfo> ProfilingDataCollector::getMemoryAllocations(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= memoryAllocations_.size()) {
        return memoryAllocations_;
    }
    
    size_t startIndex = memoryAllocations_.size() - maxCount;
    return std::vector<MemoryAllocationInfo>(memoryAllocations_.begin() + startIndex, memoryAllocations_.end());
}

std::vector<ThreadPerformanceMetrics> ProfilingDataCollector::getThreadData(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= threadData_.size()) {
        return threadData_;
    }
    
    size_t startIndex = threadData_.size() - maxCount;
    return std::vector<ThreadPerformanceMetrics>(threadData_.begin() + startIndex, threadData_.end());
}

std::vector<IOMetrics> ProfilingDataCollector::getIOData(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (maxCount == 0 || maxCount >= ioData_.size()) {
        return ioData_;
    }
    
    size_t startIndex = ioData_.size() - maxCount;
    return std::vector<IOMetrics>(ioData_.begin() + startIndex, ioData_.end());
}

double ProfilingDataCollector::getDurationSeconds() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
    return static_cast<double>(elapsed.count());
}

CollectionResult ProfilingDataCollector::getCollectionResult() const {
    CollectionResult result;
    result.totalDataPoints = totalEventsCollected_.load() + totalEventsDropped_.load();
    result.dataPointsCollected = totalEventsCollected_.load();
    result.dataPointsDropped = totalEventsDropped_.load();
    result.memoryUsage = memoryUsage_.load();
    result.collectionRate = result.totalDataPoints / getDurationSeconds();
    result.status = running_.load() ? "running" : "stopped";
    result.lastUpdate = std::chrono::high_resolution_clock::now();
    
    return result;
}

std::string ProfilingDataCollector::analyzeEventDistribution() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::unordered_map<ProfileEventType, size_t> eventCounts;
    for (const auto& event : allEvents_) {
        eventCounts[event.eventType]++;
    }
    
    std::stringstream ss;
    ss << "Event Distribution Analysis:\n";
    
    for (const auto& pair : eventCounts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " events ("
           << (static_cast<double>(pair.second) / allEvents_.size() * 100.0) << "%)" << std::endl;
    }
    
    return ss.str();
}

std::string ProfilingDataCollector::analyzeTemporalPatterns() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (allEvents_.empty()) return "No events to analyze";
    
    std::vector<uint64_t> timestamps;
    for (const auto& event : allEvents_) {
        timestamps.push_back(event.timestamp);
    }
    
    std::sort(timestamps.begin(), timestamps.end());
    
    std::stringstream ss;
    ss << "Temporal Pattern Analysis:\n";
    ss << "  Number of events: " << allEvents_.size() << "\n";
    ss << "  Time range: " << timestamps.front() << " to " << timestamps.back() << " ns\n";
    ss << "  Duration: " << (timestamps.back() - timestamps.front()) / 1e9 << " seconds\n";
    
    if (timestamps.size() > 1) {
        double meanInterval = (timestamps.back() - timestamps.front()) / (timestamps.size() - 1);
        ss << "  Mean interval: " << meanInterval << " ns\n";
        
        // Calculate standard deviation
        double sum = 0.0;
        for (size_t i = 1; i < timestamps.size(); ++i) {
            sum += timestamps[i] - timestamps[i-1];
        }
        double stdDev = std::sqrt((sum*sum - (timestamps.size()-1)*meanInterval*meanInterval) /
                                 ((timestamps.size()-1)*(timestamps.size()-2)));
        ss << "  Standard deviation: " << stdDev << " ns\n";
    }
    
    return ss.str();
}

std::string ProfilingDataCollector::analyzeResourceUtilization() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "Resource Utilization Analysis:\n";
    
    if (!timestepData_.empty()) {
        double totalWallTime = 0.0;
        double totalCPU = 0.0;
        size_t totalSpikes = 0;
        
        for (const auto& ts : timestepData_) {
            totalWallTime += ts.wallTimeNs;
            totalCPU += ts.cpuTimeNs;
            totalSpikes += ts.spikesGenerated;
        }
        
        ss << "  Timesteps analyzed: " << timestepData_.size() << "\n";
        ss << "  Total wall time: " << formatTimeNs(totalWallTime) << "\n";
        ss << "  Total CPU time: " << formatTimeNs(totalCPU) << "\n";
        ss << "  CPU utilization: " << (totalCPU / totalWallTime) * 100.0 << "%\n";
        ss << "  Total spikes generated: " << totalSpikes << "\n";
        ss << "  Average timestep time: " << formatTimeNs(totalWallTime / timestepData_.size()) << "\n";
    }
    
    if (!threadData_.empty()) {
        size_t totalThreads = 0;
        for (const auto& metrics : threadData_) {
            totalThreads += metrics.threadIdNumeric;
        }
        ss << "  Average threads per interval: " << (totalThreads / threadData_.size()) << "\n";
    }
    
    return ss.str();
}

std::string ProfilingDataCollector::analyzeErrorPatterns() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream ss;
    ss << "Error Pattern Analysis:\n";
    
    size_t exceptions = 0;
    size_t assertions = 0;
    size_t memoryLeaks = 0;
    size_t raceConditions = 0;
    
    for (const auto& event : allEvents_) {
        if (event.eventType == ProfileEventType::EXCEPTION) {
            exceptions++;
        } else if (event.eventType == ProfileEventType::ASSERTION) {
            assertions++;
        } else if (event.eventType == ProfileEventType::MEMORY_LEAK) {
            memoryLeaks++;
        } else if (event.eventType == ProfileEventType::RACE_CONDITION) {
            raceConditions++;
        }
    }
    
    ss << "  Exceptions thrown: " << exceptions << "\n";
    ss << "  Assertions failed: " << assertions << "\n";
    ss << "  Memory leaks detected: " << memoryLeaks << "\n";
    ss << "  Race conditions detected: " << raceConditions << "\n";
    
    if (exceptions + assertions + memoryLeaks + raceConditions > 0) {
        ss << "  WARNING: Performance issues detected that may impact simulation quality" << std::endl;
    }
    
    return ss.str();
}

void ProfilingDataCollector::addCustomCollector(std::unique_ptr<CustomDataCollector> collector) {
    std::lock_guard<std::mutex> lock(mutex_);
    customCollectors_.push_back(std::move(collector));
}

std::vector<std::string> ProfilingDataCollector::getCustomCollectorNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    for (const auto& collector : customCollectors_) {
        names.push_back(collector->getCollectorName());
    }
    return names;
}

bool ProfilingDataCollector::removeCustomCollector(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto it = customCollectors_.begin(); it != customCollectors_.end(); ++it) {
        if ((*it)->getCollectorName() == name) {
            customCollectors_.erase(it);
            return true;
        }
    }
    
    return false;
}

void ProfilingDataCollector::addDataProcessedCallback(std::function<void(const std::vector<ProfileDataPoint>&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataProcessedCallbacks_.push_back(callback);
}

void ProfilingDataCollector::addCollectionStatusCallback(std::function<void(const CollectionResult&)> callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    collectionStatusCallbacks_.push_back(callback);
}

void ProfilingDataCollector::processCollectedData() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Call data processed callbacks
    for (const auto& callback : dataProcessedCallbacks_) {
        callback(allEvents_);
    }
    
    // Update indexes
    updateIndexes();
}

void ProfilingDataCollector::aggregateData() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Aggregate data by time window
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoffTime = now - std::chrono::minutes(config_.dataAggregationWindow);
    
    // Remove old data
    auto cutoffTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(cutoffTime.time_since_epoch()).count();
    
    allEvents_.erase(std::remove_if(allEvents_.begin(), allEvents_.end(),
                                   [cutoffTimestamp](const ProfileDataPoint& event) { return event.timestamp < cutoffTimestamp; }),
                    allEvents_.end());
    
    memoryUsage_ = allEvents_.size() * sizeof(ProfileDataPoint);
}

void ProfilingDataCollector::updateIndexes() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Update event type index
    eventTypeIndex_.clear();
    for (const auto& event : allEvents_) {
        eventTypeIndex_[event.eventType].push_back(event);
    }
    
    // Update tag index
    tagIndex_.clear();
    for (const auto& event : allEvents_) {
        for (const auto& tag : event.tags) {
            tagIndex_[tag].push_back(event);
        }
    }
}

bool ProfilingDataCollector::shouldDropEvent(const ProfileDataPoint& event) const {
    // Check timestamp filtering
    if (event.timestamp < config_.minTimestamp || event.timestamp > config_.maxTimestamp) {
        return true;
    }
    
    // Check event type exclusion
    if (config_.excludeEventTypes.find(event.eventType) != config_.excludeEventTypes.end()) {
        return true;
    }
    
    // Check data point limit
    if (allEvents_.size() >= config_.maxDataPoints) {
        return true;
    }
    
    return false;
}

void ProfilingDataCollector::cleanupOldData() {
    auto now = std::chrono::high_resolution_clock::now();
    auto cutoffTime = now - std::chrono::hours(24); // Keep last 24 hours
    auto cutoffTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(cutoffTime.time_since_epoch()).count();
    
    // Cleanup old events
    allEvents_.erase(std::remove_if(allEvents_.begin(), allEvents_.end(),
                                   [cutoffTimestamp](const ProfileDataPoint& event) { return event.timestamp < cutoffTimestamp; }),
                    allEvents_.end());
    
    // Cleanup timestep data
    timestepData_.erase(std::remove_if(timestepData_.begin(), timestepData_.end(),
                                      [cutoffTimestamp](const TimestepPerformance& ts) { return ts.timestep == 0; }),
                       timestepData_.end());
    
    // Recalculate memory usage
    memoryUsage_ = allEvents_.size() * sizeof(ProfileDataPoint);
}

void ProfilingDataCollector::compressData() {
    if (config_.compression == "none") return;
    
    // Simplified compression - in a real implementation, use proper compression algorithms
    // For now, just estimate the compressed size
    memoryUsage_ = static_cast<size_t>(memoryUsage_ * 0.5); // Assume 50% compression ratio
}

std::string ProfilingDataCollector::exportToJSON() const {
    std::stringstream ss;
    ss << "{\n";
    
    ss << "  \"collectorName\": \"" << name_ << "\",\n";
    ss << "  \"totalEvents\": " << allEvents_.size() << ",\n";
    ss << "  \"memoryUsage\": " << memoryUsage_ << ",\n";
    ss << "  \"events\": [\n";
    
    for (size_t i = 0; i < allEvents_.size(); ++i) {
        const auto& event = allEvents_[i];
        ss << "    {\"timestamp\": " << event.timestamp << ", \"eventType\": " 
           << static_cast<int>(event.eventType) << ", \"sourceFile\": \"" 
           << event.sourceFile << \"\", \"functionName\": \"" 
           << event.functionName << \"\"";
        
        if (!event.tags.empty()) {
            ss << ", \"tags\": {";
            bool first = true;
            for (const auto& tag : event.tags) {
                if (!first) ss << ", ";
                ss << \"\" << tag.first << "\": \"" << tag.second << "\"";
                first = false;
            }
            ss << "}";
        }
        
        if (!event.eventData.empty()) {
            ss << ", \"eventData\": {";
            bool first = true;
            for (const auto& data : event.eventData) {
                if (!first) ss << ", ";
                ss << \"\" << data.first << "\": \"" << data.second << "\"";
                first = false;
            }
            ss << "}";
        }
        
        ss << (i == allEvents_.size() - 1 ? "}" : "},\n");
    }
    
    ss << "  ]\n";
    ss << "}\n";
    
    return ss.str();
}

std::string ProfilingDataCollector::exportToCSV() const {
    std::stringstream ss;
    
    // Header
    ss << "timestamp,event_type,source_file,source_line,function_name,thread_id,duration_ns\n";
    
    // Data
    for (const auto& event : allEvents_) {
        ss << event.timestamp << ","
           << static_cast<int>(event.eventType) << ","
           << event.sourceFile << ","
           << event.sourceLine << ","
           << event.functionName << ","
           << event.threadId << ","
           << event.durationNs << "\n";
    }
    
    return ss.str();
}

std::string ProfilingDataCollector::exportToYAML() const {
    YAML::Emitter out;
    out << YAML::BeginMap;
    
    out << YAML::Key << "collectorName" << YAML::Value << name_;
    out << YAML::Key << "totalEvents" << YAML::Value << allEvents_.size();
    out << YAML::Key << "memoryUsage" << YAML::Value << memoryUsage_;
    
    out << YAML::Key << "events" << YAML::Value;
    out << YAML::BeginSeq;
    
    for (const auto& event : allEvents_) {
        out << YAML::BeginMap;
        out << YAML::Key << "timestamp" << YAML::Value << event.timestamp;
        out << YAML::Key << "eventType" << YAML::Value << static_cast<int>(event.eventType);
        out << YAML::Key << "sourceFile" << YAML::Value << event.sourceFile;
        out << YAML::Key << "sourceLine" << YAML::Value << event.sourceLine;
        out << YAML::Key << "functionName" << YAML::Value << event.functionName;
        out << YAML::Key << "threadId" << YAML::Value << event.threadId;
        out << YAML::Key << "durationNs" << YAML::Value << event.durationNs;
        
        if (!event.tags.empty()) {
            out << YAML::Key << "tags" << YAML::Value;
            out << YAML::BeginMap;
            for (const auto& tag : event.tags) {
                out << YAML::Key << tag.first << YAML::Value << tag.second;
            }
            out << YAML::EndMap;
        }
        
        out << YAML::EndMap;
    }
    
    out << YAML::EndSeq;
    out << YAML::EndMap;
    
    return out.c_str();
}

std::string ProfilingDataCollector::exportToXML() const {
    pugi::xml_document doc;
    auto root = doc.append_child("ProfilingDataCollector");
    
    root.append_attribute("name") = name_.c_str();
    root.append_attribute("totalEvents") = static_cast<unsigned int>(allEvents_.size());
    root.append_attribute("memoryUsage") = static_cast<unsigned int>(memoryUsage_);
    
    auto eventsElem = root.append_child("Events");
    
    for (const auto& event : allEvents_) {
        auto eventElem = eventsElem.append_child("Event");
        eventElem.append_attribute("timestamp") = static_cast<unsigned long long>(event.timestamp);
        eventElem.append_attribute("eventType") = static_cast<int>(event.eventType);
        eventElem.append_attribute("sourceFile") = event.sourceFile.c_str();
        eventElem.append_attribute("sourceLine") = static_cast<unsigned int>(event.sourceLine);
        eventElem.append_attribute("functionName") = event.functionName.c_str();
        eventElem.append_attribute("threadId") = event.threadId.c_str();
        eventElem.append_attribute("durationNs") = static_cast<double>(event.durationNs);
        
        if (!event.tags.empty()) {
            auto tagsElem = eventElem.append_child("Tags");
            for (const auto& tag : event.tags) {
                auto tagElem = tagsElem.append_child(tag.first.c_str());
                tagElem.text() = tag.second.c_str();
            }
        }
    }
    
    std::stringstream ss;
    doc.save(ss);
    return ss.str();
}

void ProfilingDataCollector::integrateWithProfiler(NLPerformanceProfiler& profiler) {
    // Add data collection from profiler events
    // This would typically hook into the profiler's event recording system
}

void ProfilingDataCollector::integrateWithMonitor(PerformanceMonitor& monitor) {
    // Add data collection from monitor metrics
    // This would typically hook into the monitor's metric collection system
}

// DataCollectorFactory implementation
std::shared_ptr<ProfilingDataCollector> DataCollectorFactory::createCollector(
    const std::string& name, const DataCollectionConfig& config) {
    return std::make_shared<ProfilingDataCollector>(name, config);
}

std::vector<DataCollectionConfig> DataCollectorFactory::getAvailableCollectorConfigs() {
    return std::vector<DataCollectionConfig>(); // Would return predefined configs
}

std::unique_ptr<CustomDataCollector> DataCollectorFactory::createSystemCollector(const std::string& name) {
    return std::make_unique<SystemDataCollector>(name);
}

std::unique_ptr<CustomDataCollector> DataCollectorFactory::createSimulationCollector(const std::string& name) {
    return std::make_unique<SimulationDataCollector>(name);
}

std::unique_ptr<CustomDataCollector> DataCollectorFactory::createMemoryCollector(const std::string& name) {
    return std::make_unique<MemoryDataCollector>(name);
}

std::unique_ptr<CustomDataCollector> DataCollectorFactory::createThreadCollector(const std::string& name) {
    return std::make_unique<ThreadDataCollector>(name);
}

std::unique_ptr<CustomDataCollector> DataCollectorFactory::createIOCollector(const std::string& name) {
    return std::make_unique<IODataCollector>(name);
}

} // namespace nlm
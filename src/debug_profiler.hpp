// debug_profiler.hpp - Comprehensive debugging and profiling tools for NLM
//
// This provides performance profiling, memory debugging, visualization, and
// advanced debugging tools for the NLM brain simulation project.

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <thread>
#include <mutex>

// Forward declarations
namespace nlm {
    class Config;
    class Logger;
    class Brain;
    class Neuron;
    class Synapse;
    class SimulationClock;
}

// Performance measurement utilities
class PerformanceProfiler {
public:
    struct PerformanceSample {
        std::string operationName;
        double duration;  // seconds
        std::chrono::system_clock::time_point timestamp;
        uint64_t threadId;
        std::string file;
        int line;
        
        PerformanceSample(const std::string& name, double duration,
                         const std::string& file = "", int line = 0)
            : operationName(name), duration(duration), threadId(std::this_thread::get_id()),
              file(file), line(line) {
            timestamp = std::chrono::system_clock::now();
        }
    };
    
    struct PerformanceStats {
        std::string operationName;
        double totalDuration;
        double minDuration;
        double maxDuration;
        double averageDuration;
        uint64_t sampleCount;
        double totalSamplesPerSecond;
        
        PerformanceStats() : totalDuration(0.0), minDuration(1e9), maxDuration(0.0),
                           averageDuration(0.0), sampleCount(0), totalSamplesPerSecond(0.0) {}
        
        void addSample(double duration) {
            totalDuration += duration;
            minDuration = std::min(minDuration, duration);
            maxDuration = std::max(maxDuration, duration);
            sampleCount++;
            if (totalDuration > 0.0) {
                averageDuration = totalDuration / sampleCount;
            }
        }
    };
    
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
    // Profile a block of code
    class ScopedProfiler {
    public:
        ScopedProfiler(const std::string& operationName, const std::string& file = "",
                      int line = 0)
            : operationName_(operationName), file_(file), line_(line) {
            startTime_ = std::chrono::high_resolution_clock::now();
        }
        
        ~ScopedProfiler() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime_).count();
            PerformanceProfiler::getInstance().addSample(operationName_, duration, file_, line_);
        }
        
    private:
        std::string operationName_;
        std::chrono::high_resolution_clock::time_point startTime_;
        std::string file_;
        int line_;
    };
    
    // Profile a function call
    template<typename Func>
    auto profileFunction(const std::string& operationName, Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration<double>(end - start).count();
        addSample(operationName, duration);
        
        return result;
    }
    
    // Add a performance sample
    void addSample(const std::string& operationName, double duration,
                   const std::string& file = "", int line = 0) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        samples_.emplace_back(operationName, duration, file, line);
        
        // Update stats
        auto& stats = stats_[operationName];
        stats.operationName = operationName;
        stats.addSample(duration);
        
        // Limit samples to prevent memory issues
        if (samples_.size() > maxSamples_) {
            samples_.erase(samples_.begin(), samples_.begin() + (samples_.size() - maxSamples_));
        }
    }
    
    // Get performance statistics
    PerformanceStats getStats(const std::string& operationName) const {
        auto it = stats_.find(operationName);
        if (it != stats_.end()) {
            return it->second;
        }
        return PerformanceStats();
    }
    
    // Get all operation names
    std::vector<std::string> getOperationNames() const {
        std::vector<std::string> names;
        for (const auto& pair : stats_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get samples for an operation
    std::vector<PerformanceSample> getSamples(const std::string& operationName, 
                                             size_t maxSamples = 100) const {
        std::vector<PerformanceSample> result;
        for (const auto& sample : samples_) {
            if (sample.operationName == operationName) {
                result.push_back(sample);
                if (result.size() >= maxSamples) {
                    break;
                }
            }
        }
        return result;
    }
    
    // Clear all data
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.clear();
        stats_.clear();
    }
    
    // Save profiling data to file
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            logError("Failed to open profile file: " + filename);
            return;
        }
        
        file << "{\n";
        file << "  \"profilingData\": {\n";
        
        for (const auto& pair : stats_) {
            const auto& stats = pair.second;
            file << "    \"" << pair.first << "\": {\n";
            file << "      \"totalDuration\": " << stats.totalDuration << ",\n";
            file << "      \"minDuration\": " << stats.minDuration << ",\n";
            file << "      \"maxDuration\": " << stats.maxDuration << ",\n";
            file << "      \"averageDuration\": " << stats.averageDuration << ",\n";
            file << "      \"sampleCount\": " << stats.sampleCount << ",\n";
            file << "      \"samplesPerSecond\": " << stats.totalSamplesPerSecond << "\n";
            
            if (&pair != std::prev(stats_.end())) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  },\n";
        file << "  \"samples\": [\n";
        
        size_t maxSamplesToSave = std::min(samples_.size(), static_cast<size_t>(1000));
        for (size_t i = 0; i < maxSamplesToSave; ++i) {
            const auto& sample = samples_[i];
            file << "    {\n";
            file << "      \"operationName\": \"" << sample.operationName << "\",\n";
            file << "      \"duration\": " << sample.duration << ",\n";
            file << "      \"file\": \"" << sample.file << "\",\n";
            file << "      \"line\": " << sample.line << "\n";
            
            if (i < maxSamplesToSave - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        file.close();
        
        logInfo("Profile data saved to: " + filename);
    }
    
    // Get formatted profile report
    std::string getReport() const {
        std::stringstream ss;
        ss << "Performance Profile Report\n";
        ss << "==========================\n\n";
        
        ss << "Operations:\n";
        for (const auto& pair : stats_) {
            const auto& stats = pair.second;
            ss << "  " << pair.first << ":\n";
            ss << "    Total time: " << std::fixed << std::setprecision(6) << stats.totalDuration << "s\n";
            ss << "    Samples: " << stats.sampleCount << "\n";
            ss << "    Avg time: " << std::fixed << std::setprecision(6) << stats.averageDuration << "s\n";
            ss << "    Min time: " << std::fixed << std::setprecision(6) << stats.minDuration << "s\n";
            ss << "    Max time: " << std::fixed << std::setprecision(6) << stats.maxDuration << "s\n";
            ss << "    Samples/sec: " << stats.totalSamplesPerSecond << "\n\n";
        }
        
        ss << "Total samples: " << samples_.size() << "\n";
        return ss.str();
    }
    
private:
    PerformanceProfiler() : maxSamples_(10000) {}
    
    static void logInfo(const std::string& message) {
        std::cout << "[PROFILE] " << message << std::endl;
    }
    
    static void logError(const std::string& message) {
        std::cout << "[PROFILE ERROR] " << message << std::endl;
    }
    
    std::vector<PerformanceSample> samples_;
    std::map<std::string, PerformanceStats> stats_;
    std::mutex mutex_;
    size_t maxSamples_;
    
    friend class ScopedProfiler;
};

// Memory debugging utilities
class MemoryDebugger {
public:
    struct MemoryBlock {
        void* address;
        size_t size;
        std::string allocationSite;
        int allocationLine;
        std::string file;
        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
        
        MemoryBlock(void* addr, size_t size, const std::string& site, int line,
                   const std::string& file)
            : address(addr), size(size), allocationSite(site),
              allocationLine(line), file(file) {
            timestamp = std::chrono::system_clock::now();
            threadId = std::this_thread::get_id();
        }
    };
    
    static MemoryDebugger& getInstance() {
        static MemoryDebugger instance;
        return instance;
    }
    
    // Track memory allocation
    void trackAllocation(void* address, size_t size, const std::string& site = "",
                         int line = 0, const std::string& file = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[address] = MemoryBlock(address, size, site, line, file);
        totalAllocated_ += size;
        peakAllocated_ = std::max(peakAllocated_, totalAllocated_);
    }
    
    // Track memory deallocation
    void trackDeallocation(void* address) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = allocations_.find(address);
        if (it != allocations_.end()) {
            totalAllocated_ -= it->second.size;
            allocations_.erase(it);
        }
    }
    
    // Get current memory usage
    size_t getCurrentMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return totalAllocated_;
    }
    
    // Get peak memory usage
    size_t getPeakMemoryUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return peakAllocated_;
    }
    
    // Get allocation count
    size_t getAllocationCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return allocations_.size();
    }
    
    // Get allocations by file
    std::map<std::string, size_t> getAllocationsByFile() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, size_t> result;
        
        for (const auto& pair : allocations_) {
            const auto& block = pair.second;
            result[block.file] += block.size;
        }
        
        return result;
    }
    
    // Get allocations by thread
    std::map<std::thread::id, size_t> getAllocationsByThread() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::thread::id, size_t> result;
        
        for (const auto& pair : allocations_) {
            const auto& block = pair.second;
            result[block.threadId] += block.size;
        }
        
        return result;
    }
    
    // Clear all tracking data
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.clear();
        totalAllocated_ = 0;
        peakAllocated_ = 0;
    }
    
    // Save memory debugging data to file
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            logError("Failed to open memory debug file: " + filename);
            return;
        }
        
        file << "{\n";
        file << "  \"memoryDebugging\": {\n";
        file << "    \"totalAllocated\": " << totalAllocated_ << ",\n";
        file << "    \"peakAllocated\": " << peakAllocated_ << ",\n";
        file << "    \"allocationCount\": " << allocations_.size() << ",\n";
        file << "    \"allocations\": [\n";
        
        size_t maxAllocationsToSave = std::min(allocations_.size(), static_cast<size_t>(1000));
        size_t index = 0;
        for (const auto& pair : allocations_) {
            const auto& block = pair.second;
            file << "      {\n";
            file << "        \"address\": \"" << block.address << "\",\n";
            file << "        \"size\": " << block.size << ",\n";
            file << "        \"allocationSite\": \"" << block.allocationSite << "\",\n";
            file << "        \"allocationLine\": " << block.allocationLine << ",\n";
            file << "        \"file\": \"" << block.file << "\",\n";
            file << "        \"timestamp\": \"" << std::chrono::system_clock::to_time_t(block.timestamp) << "\"\n";
            
            if (++index < maxAllocationsToSave) {
                file << "      },\n";
            } else {
                file << "      }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        file.close();
        
        logInfo("Memory debugging data saved to: " + filename);
    }
    
    // Get formatted memory debugging report
    std::string getReport() const {
        std::stringstream ss;
        ss << "Memory Debugging Report\n";
        ss << "=========================\n\n";
        
        ss << "Memory Usage:\n";
        ss << "  Total allocated: " << totalAllocated_ << " bytes\n";
        ss << "  Peak allocated: " << peakAllocated_ << " bytes\n";
        ss << "  Allocations: " << allocations_.size() << "\n\n";
        
        auto allocationsByFile = getAllocationsByFile();
        ss << "Allocations by File:\n";
        for (const auto& pair : allocationsByFile) {
            ss << "  " << pair.first << ": " << pair.second << " bytes\n";
        }
        ss << "\n";
        
        auto allocationsByThread = getAllocationsByThread();n        ss << "Allocations by Thread:\n";
        for (const auto& pair : allocationsByThread) {
            ss << "  Thread " << pair.first << ": " << pair.second << " bytes\n";
        }
        
        return ss.str();
    }
    
private:
    MemoryDebugger() : totalAllocated_(0), peakAllocated_(0) {}
    
    static void logInfo(const std::string& message) {
        std::cout << "[MEMORY] " << message << std::endl;
    }
    
    static void logError(const std::string& message) {
        std::cout << "[MEMORY ERROR] " << message << std::endl;
    }
    
    std::map<void*, MemoryBlock> allocations_;
    size_t totalAllocated_;
    size_t peakAllocated_;
    mutable std::mutex mutex_;
};

// Brain-specific debugging utilities
class BrainDebugger {
public:
    static BrainDebugger& getInstance() {
        static BrainDebugger instance;
        return instance;
    }
    
    // Analyze brain structure and performance
    struct BrainAnalysis {
        size_t neuronCount;
        size_t synapseCount;
        size_t regionCount;
        double averageFiringRate;
        double maxFiringRate;
        double totalSpikeCount;
        double averageSynapticWeight;
        double maxSynapticWeight;
        double minSynapticWeight;
        double averageNeuronVoltage;
        double maxNeuronVoltage;
        double minNeuronVoltage;
        std::chrono::system_clock::time_point analysisTime;
        
        BrainAnalysis() : neuronCount(0), synapseCount(0), regionCount(0),
                         averageFiringRate(0.0), maxFiringRate(0.0),
                         totalSpikeCount(0.0), averageSynapticWeight(0.0),
                         maxSynapticWeight(0.0), minSynapticWeight(0.0),
                         averageNeuronVoltage(0.0), maxNeuronVoltage(0.0),
                         minNeuronVoltage(0.0) {
            analysisTime = std::chrono::system_clock::now();
        }
    };
    
    // Analyze brain state
    BrainAnalysis analyzeBrain(const Brain* brain) {
        BrainAnalysis analysis;
        
        // Get brain statistics
        analysis.neuronCount = brain->getTotalNeuronCount();
        analysis.synapseCount = brain->getTotalSynapseCount();
        analysis.regionCount = brain->getRegionCount();
        analysis.averageFiringRate = brain->getAverageFiringRate();
        analysis.maxFiringRate = brain->getFiringNeuronCount(); // This might need adjustment
        analysis.totalSpikeCount = brain->getTotalSpikeCount();
        
        // Analyze synaptic weights
        double totalWeight = 0.0;
        double maxWeight = -1e9;
        double minWeight = 1e9;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& synapse : region->getSynapses()) {
                float weight = synapse->getWeight();
                totalWeight += weight;
                maxWeight = std::max(maxWeight, static_cast<double>(weight));
                minWeight = std::min(minWeight, static_cast<double>(weight));
            }
        }
        
        if (analysis.synapseCount > 0) {
            analysis.averageSynapticWeight = totalWeight / analysis.synapseCount;
            analysis.maxSynapticWeight = maxWeight;
            analysis.minSynapticWeight = minWeight;
        }
        
        // Analyze neuron voltages
        double totalVoltage = 0.0;
        double maxVoltage = -1e9;
        double minVoltage = 1e9;
        
        for (const auto& region : brain->getRegions()) {
            for (const auto& neuron : region->getAllNeurons()) {
                double voltage = neuron->getMembranePotential();
                totalVoltage += voltage;
                maxVoltage = std::max(maxVoltage, voltage);
                minVoltage = std::min(minVoltage, voltage);
            }
        }
        
        if (analysis.neuronCount > 0) {
            analysis.averageNeuronVoltage = totalVoltage / analysis.neuronCount;
            analysis.maxNeuronVoltage = maxVoltage;
            analysis.minNeuronVoltage = minVoltage;
        }
        
        return analysis;
    }
    
    // Generate brain debugging report
    std::string generateBrainReport(const BrainAnalysis& analysis) {
        std::stringstream ss;
        ss << "Brain Analysis Report\n";
        ss << "====================\n\n";
        
        ss << "Basic Statistics:\n";
        ss << "  Neuron count: " << analysis.neuronCount << "\n";
        ss << "  Synapse count: " << analysis.synapseCount << "\n";
        ss << "  Region count: " << analysis.regionCount << "\n";
        ss << "  Total spikes: " << analysis.totalSpikeCount << "\n\n";
        
        ss << "Firing Activity:\n";
        ss << "  Average firing rate: " << analysis.averageFiringRate << "\n";
        ss << "  Max firing rate: " << analysis.maxFiringRate << "\n\n";
        
        ss << "Synaptic Weights:\n";
        ss << "  Average weight: " << analysis.averageSynapticWeight << "\n";
        ss << "  Max weight: " << analysis.maxSynapticWeight << "\n";
        ss << "  Min weight: " << analysis.minSynapticWeight << "\n\n";
        
        ss << "Neuron Voltages:\n";
        ss << "  Average voltage: " << analysis.averageNeuronVoltage << "\n";
        ss << "  Max voltage: " << analysis.maxNeuronVoltage << "\n";
        ss << "  Min voltage: " << analysis.minNeuronVoltage << "\n\n";
        
        ss << "Analysis Time: " << std::chrono::system_clock::to_time_t(analysis.analysisTime) << "\n";
        
        return ss.str();
    }
    
    // Profile specific brain operations
    class BrainProfiler {
    public:
        BrainProfiler(const std::string& operationName)
            : operationName_(operationName) {
            startTime_ = std::chrono::high_resolution_clock::now();
        }
        
        ~BrainProfiler() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime_).count();
            
            PerformanceProfiler::getInstance().addSample(
                "brain_" + operationName_, duration, "", 0
            );
        }
        
    private:
        std::string operationName_;
        std::chrono::high_resolution_clock::time_point startTime_;
    };
    
    // Create a brain profiler
    BrainProfiler profile(const std::string& operationName) {
        return BrainProfiler(operationName);
    }
};

// Global debugging instances
inline PerformanceProfiler& gProfiler = PerformanceProfiler::getInstance();
inline MemoryDebugger& gMemoryDebugger = MemoryDebugger::getInstance();
inline BrainDebugger& gBrainDebugger = BrainDebugger::getInstance();

// Debugging macros
#define PROFILE_SCOPE(operationName) \
    nlm::PerformanceProfiler::ScopedProfiler profiler_##__LINE__(operationName, __FILE__, __LINE__);

#define PROFILE_FUNCTION(operationName) \
    auto profiler_##__LINE__ = nlm::PerformanceProfiler::getInstance().profileFunction( \
        operationName, []() { /* user code here */ }
    );

#define MEMORY_TRACK(address, size, site, line, file) \
    nlm::MemoryDebugger::getInstance().trackAllocation(address, size, site, line, file);

#define MEMORY_UNTRACK(address) \
    nlm::MemoryDebugger::getInstance().trackDeallocation(address);

#define BRAIN_ANALYZE(brain) \
    nlm::BrainDebugger::getInstance().analyzeBrain(brain);

#define BRAIN_PROFILE(operationName) \
    nlm::BrainDebugger::BrainProfiler brainProfiler_##__LINE__(operationName);

// Debug output macros
#define DEBUG_PRINT(...) \
    do { \
        std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " " << __VA_ARGS__ << std::endl; \
    } while(0)

#define DEBUG_PRINTLN(...) \
    do { \
        std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " " << __VA_ARGS__ << std::endl; \
    } while(0)

#define INFO_PRINT(...) \
    do { \
        std::cout << "[INFO] " << __VA_ARGS__ << std::endl; \
    } while(0)

#define ERROR_PRINT(...) \
    do { \
        std::cout << "[ERROR] " << __VA_ARGS__ << std::endl; \
    } while(0)

#define WARNING_PRINT(...) \
    do { \
        std::cout << "[WARNING] " << __VA_ARGS__ << std::endl; \
    } while(0)

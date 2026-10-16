#include "MemoryProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include <thread>
#include <mutex>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <cstring>
#include <set>

namespace nlm {

struct MemoryProfiler::Impl {
    // Memory tracking
    std::vector<MemoryAllocationRecord> allocations;
    mutable std::mutex allocationsMutex;
    std::unordered_map<void*, size_t> allocatedSizes;
    mutable std::mutex allocatedSizesMutex;
    
    // Memory sampling
    std::vector<MemorySample> memorySamples;
    mutable std::mutex samplesMutex;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastSampleTime;
    std::thread samplingThread;
    bool samplingActive;
    double samplingInterval;
    
    // Memory error handling
    std::vector<int> monitoredSignals;
    
    // Configuration
    MemoryProfilerConfig config;
    
    Impl() : samplingActive(false), samplingInterval(1.0) {}
    
    ~Impl() {
        if (samplingActive) {
            samplingActive = false;
            if (samplingThread.joinable()) {
                samplingThread.join();
            }
        }
        
        // Restore original signal handlers
        for (int signal : monitoredSignals) {
            signal(signal, SIG_DFL);
        }
    }
    
    void startSampling() {
        samplingActive = true;
        samplingThread = std::thread([this]() {
            while (samplingActive) {
                MemorySample sample = collectMemorySample();
                updateMemorySample(sample);
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(samplingInterval * 1000)));
            }
        });
    }
    
    void stopSampling() {
        samplingActive = false;
        if (samplingThread.joinable()) {
            samplingThread.join();
        }
    }
    
    MemorySample collectMemorySample() {
        MemorySample sample;
        auto now = std::chrono::high_resolution_clock::now();
        
        // Get system memory info
        auto sysInfo = getSystemMemoryInfo();
        
        std::lock_guard<std::mutex> lock(allocationsMutex);
        
        // Calculate current allocated memory
        size_t totalAllocated = 0;
        size_t heapAllocated = 0;
        size_t poolAllocated = 0;
        size_t simdAllocated = 0;
        size_t gpuAllocated = 0;
        size_t allocationCount = 0;
        size_t deallocationCount = 0;
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        for (const auto& alloc : allocations) {
            if (!alloc.isFreed) {
                totalAllocated += alloc.size;
                allocationCount++;
                
                // Categorize allocation based on context
                if (alloc.context.find("SIMD") != std::string::npos || 
                    alloc.context.find("vector") != std::string::npos) {
                    simdAllocated += alloc.size;
                } else if (alloc.context.find("pool") != std::string::npos ||
                           alloc.context.find("MemoryPool") != std::string::npos) {
                    poolAllocated += alloc.size;
                } else if (alloc.context.find("GPU") != std::string::npos ||
                           alloc.context.find("cuda") != std::string::npos) {
                    gpuAllocated += alloc.size;
                } else {
                    heapAllocated += alloc.size;
                }
            }
        }
        
        // Calculate deallocation count (simplified)
        deallocationCount = allocations.size() - allocationCount;
        
        // Calculate rates
        double elapsed = std::chrono::duration<double>(currentTime - lastSampleTime).count();
        if (elapsed > 0) {
            sample.allocationRate = static_cast<double>(allocationCount) / elapsed;
            sample.deallocationRate = static_cast<double>(deallocationCount) / elapsed;
        }
        
        // Calculate memory growth rate (simplified)
        if (!memorySamples.empty()) {
            size_t oldestSample = memorySamples.front().totalAllocated;
            double growth = static_cast<double>(totalAllocated - oldestSample);
            sample.memoryGrowthRate = growth / elapsed;
        }
        
        // Update sample
        sample.timestamp = now;
        sample.totalAllocated = totalAllocated;
        sample.peakAllocated = sysInfo["total_memory"];
        sample.currentAllocated = totalAllocated;
        sample.heapAllocated = heapAllocated;
        sample.poolAllocated = poolAllocated;
        sample.simdAllocated = simdAllocated;
        sample.gpuAllocated = gpuAllocated;
        sample.allocationCount = allocationCount;
        sample.deallocationCount = deallocationCount;
        
        return sample;
    }
    
    void updateMemorySample(const MemorySample& sample) {
        std::lock_guard<std::mutex> lock(samplesMutex);
        
        // Add new sample
        memorySamples.push_back(sample);
        lastSampleTime = sample.timestamp;
        
        // Limit samples size
        if (memorySamples.size() > config.maxMemorySamples) {
            memorySamples.erase(memorySamples.begin());
        }
    }
    
    MemorySample getCurrentSample() const {
        std::lock_guard<std::mutex> lock(samplesMutex);
        if (memorySamples.empty()) {
            return MemorySample();
        }
        return memorySamples.back();
    }
    
    LeakDetectionResult detectMemoryLeaks() const {
        LeakDetectionResult result;
        
        std::lock_guard<std::mutex> lock(allocationsMutex);
        
        // Find allocations that were never deallocated
        for (const auto& alloc : allocations) {
            if (!alloc.isFreed) {
                // Check if this allocation might be a leak
                // Consider it a leak if:
                // 1. Size is above threshold
                // 2. Age is above threshold
                // 3. Not in a temporary context
                
                double age = alloc.getLifetime();
                if (alloc.size >= config.leakDetectionThreshold && age > 60.0) {
                    // Additional heuristic: check context for temporary allocations
                    bool isTemporary = false;
                    std::string lowerContext = alloc.context;
                    std::transform(lowerContext.begin(), lowerContext.end(), lowerContext.begin(), ::tolower);
                    
                    // Check for temporary contexts
                    if (lowerContext.find("temp") != std::string::npos ||
                        lowerContext.find("cache") != std::string::npos ||
                        lowerContext.find("stack") != std::string::npos) {
                        isTemporary = true;
                    }
                    
                    if (!isTemporary) {
                        result.hasLeaks = true;
                        result.totalLeakSize += alloc.size;
                        result.totalLeakCount++;
                        result.leaks.push_back(alloc);
                        
                        // Categorize leak
                        std::string category = "UNKNOWN";
                        if (alloc.context.find("neuron") != std::string::npos) {
                            category = "NEURAL_DATA";
                        } else if (alloc.context.find("simulation") != std::string::npos) {
                            category = "SIMULATION_STATE";
                        } else if (alloc.context.find("memory") != std::string::npos) {
                            category = "MEMORY_SYSTEM";
                        }
                        
                        if (std::find(result.leakCategories.begin(), result.leakCategories.end(), category) == result.leakCategories.end()) {
                            result.leakCategories.push_back(category);
                        }
                    }
                }
            }
        }
        
        // Calculate confidence score
        if (!result.leaks.empty()) {
            result.confidenceScore = static_cast<double>(result.leaks.size()) / static_cast<double>(allocations.size());
        }
        
        return result;
    }
    
    std::vector<MemoryOptimization> analyzeMemoryUsage() const {
        std::vector<MemoryOptimization> optimizations;
        
        std::lock_guard<std::mutex> lock(allocationsMutex);
        
        // Analyze allocation patterns
        std::unordered_map<std::string, size_t> contextSizes;
        std::unordered_map<std::string, size_t> contextCounts;
        
        for (const auto& alloc : allocations) {
            if (!alloc.isFreed) {
                contextSizes[alloc.context] += alloc.size;
                contextCounts[alloc.context]++;
            }
        }
        
        // Generate optimization recommendations
        for (const auto& pair : contextSizes) {
            MemoryOptimization opt;
            opt.context = pair.first;
            opt.potentialSavings = pair.second * 0.1;  // Estimate 10% savings
            opt.implementationCost = 5.0;  // Moderate cost
            opt.priority = static_cast<double>(pair.second) / static_cast<double>(contextSizes.size());
            
            // Categorize optimization
            if (pair.first.find("SIMD") != std::string::npos || 
                pair.first.find("vector") != std::string::npos) {
                opt.category = "VECTORIZATION";
                opt.recommendation = "Consider using SIMD-optimized memory blocks for vectorized operations";
            } else if (pair.first.find("pool") != std::string::npos) {
                opt.category = "POOLING";
                opt.recommendation = "Implement memory pooling for frequent small allocations";
            } else if (pair.first.find("cache") != std::string::npos) {
                opt.category = "COMPRESSION";
                opt.recommendation = "Consider memory compression for cache-friendly access patterns";
            } else {
                opt.category = "ALLOCATION";
                opt.recommendation = "Optimize allocation strategy for this context";
            }
            
            optimizations.push_back(opt);
        }
        
        return optimizations;
    }
    
    std::vector<MemoryBlockInfo> getMemoryBlocksByType(AllocationType type) const {
        std::vector<MemoryBlockInfo> blocks;
        
        std::lock_guard<std::mutex> lock(allocationsMutex);
        
        for (const auto& alloc : allocations) {
            if (!alloc.isFreed) {
                MemoryBlockInfo block;
                block.address = alloc.address;
                block.size = alloc.size;
                block.type = type;  // Note: we're filtering by the passed type, but allocation type might differ
                block.context = alloc.context;
                block.age = alloc.getLifetime();
                block.isAccessible = isMemoryAccessible(alloc.address, alloc.size);
                
                blocks.push_back(block);
            }
        }
        
        return blocks;
    }
    
    std::vector<MemoryBlockInfo> getMemoryBlocksByContext(const std::string& context) const {
        std::vector<MemoryBlockInfo> blocks;
        
        std::lock_guard<std::mutex> lock(allocationsMutex);
        
        for (const auto& alloc : allocations) {
            if (!alloc.isFreed && alloc.context == context) {
                MemoryBlockInfo block;
                block.address = alloc.address;
                block.size = alloc.size;
                block.type = AllocationType::Heap;  // Default
                block.context = alloc.context;
                block.age = alloc.getLifetime();
                block.isAccessible = isMemoryAccessible(alloc.address, alloc.size);
                
                blocks.push_back(block);
            }
        }
        
        return blocks;
    }
    
    std::vector<std::pair<std::string, size_t>> getTopMemoryConsumers(size_t limit = 10) const {
        std::unordered_map<std::string, size_t> contextSizes;
        
        std::lock_guard<std::mutex> lock(allocationsMutex);
        
        for (const auto& alloc : allocations) {
            if (!alloc.isFreed) {
                contextSizes[alloc.context] += alloc.size;
            }
        }
        
        // Convert to vector and sort
        std::vector<std::pair<std::string, size_t>> consumers;
        for (const auto& pair : contextSizes) {
            consumers.push_back(pair);
        }
        
        std::sort(consumers.begin(), consumers.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        if (consumers.size() > limit) {
            consumers.resize(limit);
        }
        
        return consumers;
    }
    
    double getMemoryGrowthRate() const {
        std::lock_guard<std::mutex> lock(samplesMutex);
        
        if (memorySamples.size() < 2) {
            return 0.0;
        }
        
        // Calculate growth rate from first and last samples
        const auto& firstSample = memorySamples.front();
        const auto& lastSample = memorySamples.back();
        
        double timeDiff = std::chrono::duration<double>(lastSample.timestamp - firstSample.timestamp).count();
        if (timeDiff == 0.0) {
            return 0.0;
        }
        
        return static_cast<double>(lastSample.totalAllocated - firstSample.totalAllocated) / timeDiff;
    }
    
    std::pair<double, double> getAllocationDeallocationRates() const {
        std::lock_guard<std::mutex> lock(samplesMutex);
        
        if (memorySamples.size() < 2) {
            return {0.0, 0.0};
        }
        
        const auto& firstSample = memorySamples.front();
        const auto& lastSample = memorySamples.back();
        
        double timeDiff = std::chrono::duration<double>(lastSample.timestamp - firstSample.timestamp).count();
        if (timeDiff == 0.0) {
            return {0.0, 0.0};
        }
        
        double allocRate = (lastSample.allocationCount - firstSample.allocationCount) / timeDiff;
        double deallocRate = (lastSample.deallocationCount - firstSample.deallocationCount) / timeDiff;
        
        return {allocRate, deallocRate};
    }
    
    std::string exportToJSON() const {
        std::ostringstream oss;
        oss << "{\n";
        
        oss << "  \"config\": {\n";
        oss << "    \"enableAllocationTracking\": " << (config.enableAllocationTracking ? "true" : "false") << ",\n";
        oss << "    \"enableStackTraces\": " << (config.enableStackTraces ? "true" : "false") << ",\n";
        oss << "    \"enableLeakDetection\": " << (config.enableLeakDetection ? "true" : "false") << ",\n";
        oss << "    \"samplingInterval\": " << config.samplingInterval << ",\n";
        oss << "    \"leakDetectionThreshold\": " << config.leakDetectionThreshold << "\n";
        oss << "  },\n";
        
        oss << "  \"systemMemory\": {\n";
        auto sysInfo = getSystemMemoryInfo();
        for (const auto& pair : sysInfo) {
            oss << "    \"" << pair.first << "\": " << pair.second << ",\n";
        }
        oss << "  },\n";
        
        oss << "  \"allocations\": [\n";
        bool firstAlloc = true;
        for (const auto& alloc : allocations) {
            if (!firstAlloc) oss << ",\n";
            firstAlloc = false;
            
            oss << "    {\n";
            oss << "      \"address\": \"" << alloc.address << "\",\n";
            oss << "      \"size\": " << alloc.size << ",\n";
            oss << "      \"type\": " << static_cast<int>(alloc.type) << ",\n";
            oss << "      \"allocationTime\": \"" << std::chrono::system_clock::now().time_since_epoch().count() << "\",\n";
            oss << "      \"context\": \"" << alloc.context << "\",\n";
            oss << "      \"function\": \"" << alloc.allocationFunction << "\",\n";
            oss << "      \"line\": " << alloc.allocationLine << ",\n";
            oss << "      \"isFreed\": " << (alloc.isFreed ? "true" : "false") << "\n";
            oss << "    }";
        }
        oss << "\n  ],\n";
        
        oss << "  \"samples\": [\n";
        bool firstSample = true;
        for (const auto& sample : memorySamples) {
            if (!firstSample) oss << ",\n";
            firstSample = false;
            
            oss << "    {\n";
            oss << "      \"timestamp\": \"" << sample.timestamp.time_since_epoch().count() << "\",\n";
            oss << "      \"totalAllocated\": " << sample.totalAllocated << ",\n";
            oss << "      \"peakAllocated\": " << sample.peakAllocated << ",\n";
            oss << "      \"currentAllocated\": " << sample.currentAllocated << ",\n";
            oss << "      \"allocationRate\": " << sample.allocationRate << ",\n";
            oss << "      \"deallocationRate\": " << sample.deallocationRate << ",\n";
            oss << "      \"memoryGrowthRate\": " << sample.memoryGrowthRate << "\n";
            oss << "    }";
        }
        oss << "\n  ]\n";
        
        oss << "}\n";
        
        return oss.str();
    }
    
    std::string exportToCSV() const {
        std::ostringstream oss;
        oss << "Timestamp,TotalAllocated,PeakAllocated,CurrentAllocated,AllocationCount,DeallocationCount,AllocationRate,DeallocationRate,MemoryGrowthRate\n";
        
        for (const auto& sample : memorySamples) {
            oss << sample.timestamp.time_since_epoch().count() << ","
                << sample.totalAllocated << ","
                << sample.peakAllocated << ","
                << sample.currentAllocated << ","
                << sample.allocationCount << ","
                << sample.deallocationCount << ","
                << sample.allocationRate << ","
                << sample.deallocationRate << ","
                << sample.memoryGrowthRate << "\n";
        }
        
        return oss.str();
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Memory Profiler Report ===\n\n";
        oss << "Memory Profiling Statistics:\n";
        oss << "  Total Allocations: " << allocations.size() << "\n";
        oss << "  Total Deallocations: " << std::count_if(allocations.begin(), allocations.end(),
                                                     [](const MemoryAllocationRecord& alloc) { return alloc.isFreed; }) << "\n";
        oss << "  Active Allocations: " << std::count_if(allocations.begin(), allocations.end(),
                                                      [](const MemoryAllocationRecord& alloc) { return !alloc.isFreed; }) << "\n";
        oss << "  Total Allocated Memory: " << std::accumulate(allocations.begin(), allocations.end(), 0,
                                                             [](size_t sum, const MemoryAllocationRecord& alloc) {
                                                                 return sum + (alloc.isFreed ? 0 : alloc.size);
                                                             }) << " bytes\n";
        oss << "  Peak Allocated Memory: " << getCurrentSample().peakAllocated << " bytes\n\n";
        
        oss << "Current Sample:\n";
        const auto& sample = getCurrentSample();
        oss << "  Total Allocated: " << sample.totalAllocated << " bytes\n";
        oss << "  Heap Allocated: " << sample.heapAllocated << " bytes\n";
        oss << "  Pool Allocated: " << sample.poolAllocated << " bytes\n";
        oss << "  SIMD Allocated: " << sample.simdAllocated << " bytes\n";
        oss << "  GPU Allocated: " << sample.gpuAllocated << " bytes\n";
        oss << "  Allocation Rate: " << sample.allocationRate << " allocations/sec\n";
        oss << "  Deallocation Rate: " << sample.deallocationRate << " deallocations/sec\n";
        oss << "  Memory Growth Rate: " << sample.memoryGrowthRate << " bytes/sec\n\n";
        
        oss << "Top Memory Consumers:\n";
        auto consumers = getTopMemoryConsumers(5);
        for (size_t i = 0; i < consumers.size(); ++i) {
            oss << "  " << (i + 1) << ". " << consumers[i].first << ": " << consumers[i].second << " bytes\n";
        }
        
        oss << "\nMemory Growth Trend:\n";
        double growthRate = getMemoryGrowthRate();
        oss << "  Memory Growth Rate: " << growthRate << " bytes/sec\n";
        if (growthRate > 1024.0) {
            oss << "  WARNING: Memory growth rate is high!\n";
        }
        
        oss << "\nAllocation/Deallocation Rates:\n";
        auto rates = getAllocationDeallocationRates();
        oss << "  Allocation Rate: " << rates.first << " allocations/sec\n";
        oss << "  Deallocation Rate: " << rates.second << " deallocations/sec\n";
        
        return oss.str();
    }
    
    std::string generateLeakReport(const LeakDetectionResult& leaks) const {
        std::ostringstream oss;
        oss << "=== NLM Memory Leak Detection Report ===\n\n";
        
        if (!leaks.hasLeaks) {
            oss << "No memory leaks detected.\n";
            return oss.str();
        }
        
        oss << "Memory Leaks Detected:\n";
        oss << "  Total Leaks: " << leaks.totalLeakCount << "\n";
        oss << "  Total Leak Size: " << leaks.totalLeakSize << " bytes\n";
        oss << "  Leak Categories: ";
        for (size_t i = 0; i < leaks.leakCategories.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << leaks.leakCategories[i];
        }
        oss << "\n";
        oss << "  Confidence Score: " << leaks.confidenceScore << "\n\n";
        
        oss << "Top 10 Suspected Leaks:\n";
        for (size_t i = 0; i < std::min(size_t(10), leaks.leaks.size()); ++i) {
            const auto& leak = leaks.leaks[i];
            oss << "  " << (i + 1) << ". " << leak.context << ": " << leak.size << " bytes (" << leak.getLifetime() << "s old)\n";
            oss << "     Allocation Site: " << leak.allocationSite << "\n";
        }
        
        oss << "\nRecommendations:\n";
        oss << "  1. Check for missing deallocations in code paths\n";
        oss << "  2. Review memory management in " << leaks.leakCategories.size() << " affected areas\n";
        oss << "  3. Consider implementing memory pooling to reduce fragmentation\n";
        oss << "  4. Add memory tracking for sensitive areas\n";
        
        return oss.str();
    }
    
    void setupMemoryErrorHandlers() {
        // Setup signal handlers for memory errors
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = memoryErrorHandler;
        
        // Monitor common memory-related signals
        monitoredSignals.push_back(SIGSEGV);
        monitoredSignals.push_back(SIGABRT);
        monitoredSignals.push_back(SIGBUS);
        
        for (int signal : monitoredSignals) {
            sigaction(signal, &sa, nullptr);
        }
    }
    
    static std::unordered_map<std::string, size_t> getSystemMemoryInfo() {
        std::unordered_map<std::string, size_t> info;
        
        struct sysinfo memInfo;
        if (sysinfo(&memInfo) == 0) {
            info["total_memory"] = memInfo.totalram * memInfo.mem_unit / 1024;  // KB
            info["free_memory"] = memInfo.freeram * memInfo.mem_unit / 1024;    // KB
            info["shared_memory"] = memInfo.sharedram * memInfo.mem_unit / 1024; // KB
            info["swap_total"] = memInfo.totalswap * memInfo.mem_unit / 1024;    // KB
            info["swap_free"] = memInfo.freeswap * memInfo.mem_unit / 1024;      // KB
        }
        
        return info;
    }
    
    static std::string generateStackTrace() {
        std::ostringstream oss;
        
        // Generate simple stack trace (simplified)
        oss << "Stack trace: [TODO: implement actual stack trace generation]";
        
        return oss.str();
    }
    
    static void memoryErrorHandler(int signal) {
        // Handle memory errors
        std::ostringstream oss;
        oss << "Memory error detected (signal " << signal << ")";
        NLM_LOG_ERROR(oss.str());
        
        // Exit with error code
        std::exit(1);
    }
    
    static bool isMemoryAccessible(void* address, size_t size) {
        // Check if memory is accessible
        volatile char* ptr = static_cast<volatile char*>(address);
        for (size_t i = 0; i < size; i += 4096) {
            volatile char test = ptr[i];
            if (std::isspace(test)) {
                return false;
            }
        }
        return true;
    }
};

MemoryProfiler::MemoryProfiler() : pImpl(std::make_unique<Impl>()) {
    initialize();
}

MemoryProfiler::~MemoryProfiler() {
    shutdown();
}

bool MemoryProfiler::initialize() {
    try {
        // Configure based on settings
        pImpl->config = config_;
        
        // Start memory sampling if enabled
        if (config_.enableAllocationTracking) {
            pImpl->startSampling();
        }
        
        // Setup memory error handlers if enabled
        if (config_.enableAllocationTracking) {
            pImpl->setupMemoryErrorHandlers();
        }
        
        NLM_LOG_INFO("Memory profiler initialized successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize memory profiler: ") + e.what());
        return false;
    }
}

void MemoryProfiler::shutdown() {
    try {
        pImpl->stopSampling();
        NLM_LOG_INFO("Memory profiler shutdown complete");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error during memory profiler shutdown: ") + e.what());
    }
}

void MemoryProfiler::recordAllocation(void* address, size_t size, AllocationType type,
                                     const std::string& context, const std::string& function,
                                     int line, const std::string& stackTrace) {
    if (!config_.enableAllocationTracking) return;
    
    MemoryAllocationRecord alloc;
    alloc.address = address;
    alloc.size = size;
    alloc.type = type;
    alloc.allocationTime = std::chrono::high_resolution_clock::now();
    alloc.allocationSite = function + ":" + std::to_string(line);
    alloc.allocationFunction = function;
    alloc.allocationLine = line;
    alloc.stackTrace = stackTrace.empty() ? generateStackTrace() : stackTrace;
    alloc.allocationContext = context;
    alloc.isFreed = false;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->allocatedSizesMutex);
        pImpl->allocatedSizes[address] = size;
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
        pImpl->allocations.push_back(alloc);
        
        // Limit allocations history
        if (pImpl->allocations.size() > config_.maxAllocationHistory) {
            pImpl->allocations.erase(pImpl->allocations.begin());
        }
    }
}

void MemoryProfiler::recordDeallocation(void* address) {
    std::lock_guard<std::mutex> lock(pImpl->allocatedSizesMutex);
    auto it = pImpl->allocatedSizes.find(address);
    if (it != pImpl->allocatedSizes.end()) {
        std::lock_guard<std::mutex> lock2(pImpl->allocationsMutex);
        for (auto& alloc : pImpl->allocations) {
            if (alloc.address == address && !alloc.isFreed) {
                alloc.deallocationTime = std::chrono::high_resolution_clock::now();
                alloc.isFreed = true;
                break;
            }
        }
        pImpl->allocatedSizes.erase(it);
    }
}

MemorySample MemoryProfiler::getMemorySample() {
    return pImpl->collectMemorySample();
}

void MemoryProfiler::updateMemorySample(const MemorySample& sample) {
    pImpl->updateMemorySample(sample);
}

std::vector<MemoryAllocationRecord> MemoryProfiler::getAllocations() const {
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    return pImpl->allocations;
}

MemoryAllocationRecord MemoryProfiler::getAllocation(void* address) const {
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    
    for (const auto& alloc : pImpl->allocations) {
        if (alloc.address == address) {
            return alloc;
        }
    }
    
    return MemoryAllocationRecord();
}

MemorySample MemoryProfiler::getCurrentSample() const {
    return pImpl->getCurrentSample();
}

std::vector<MemorySample> MemoryProfiler::getMemorySamples() const {
    std::lock_guard<std::mutex> lock(pImpl->samplesMutex);
    return pImpl->memorySamples;
}

LeakDetectionResult MemoryProfiler::detectMemoryLeaks() const {
    return pImpl->detectMemoryLeaks();
}

std::vector<MemoryOptimization> MemoryProfiler::analyzeMemoryUsage() const {
    return pImpl->analyzeMemoryUsage();
}

std::vector<MemoryBlockInfo> MemoryProfiler::getMemoryBlocksByType(AllocationType type) const {
    return pImpl->getMemoryBlocksByType(type);
}

std::vector<MemoryBlockInfo> MemoryProfiler::getMemoryBlocksByContext(const std::string& context) const {
    return pImpl->getMemoryBlocksByContext(context);
}

std::vector<std::pair<std::string, size_t>> MemoryProfiler::getTopMemoryConsumers(size_t limit) const {
    return pImpl->getTopMemoryConsumers(limit);
}

double MemoryProfiler::getMemoryGrowthRate() const {
    return pImpl->getMemoryGrowthRate();
}

std::pair<double, double> MemoryProfiler::getAllocationDeallocationRates() const {
    return pImpl->getAllocationDeallocationRates();
}

std::string MemoryProfiler::exportToJSON() const {
    return pImpl->exportToJSON();
}

std::string MemoryProfiler::exportToCSV() const {
    return pImpl->exportToCSV();
}

std::string MemoryProfiler::generateReport() const {
    return pImpl->generateReport();
}

std::string MemoryProfiler::generateLeakReport(const LeakDetectionResult& leaks) const {
    return pImpl->generateLeakReport(leaks);
}

void MemoryProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->allocationsMutex);
    pImpl->allocations.clear();
    pImpl->allocatedSizes.clear();
    
    std::lock_guard<std::mutex> lock2(pImpl->samplesMutex);
    pImpl->memorySamples.clear();
}

MemoryProfiler& MemoryProfiler::getInstance() {
    static MemoryProfiler instance;
    return instance;
}

std::string MemoryProfiler::generateStackTrace() {
    return Impl::generateStackTrace();
}

void MemoryProfiler::memoryErrorHandler(int signal) {
    Impl::memoryErrorHandler(signal);
}

bool MemoryProfiler::isMemoryAccessible(void* address, size_t size) {
    return Impl::isMemoryAccessible(address, size);
}

} // namespace nlm
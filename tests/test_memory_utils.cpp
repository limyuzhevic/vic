// test_memory_utils.cpp - Memory testing utilities
#include "TestFramework.hpp"
#include "TestFixture.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <cmath>
#include <thread>
#include <chrono>
#include <valarray>

namespace test_memory_utils {

// Memory pressure testing
class MemoryPressureTester {
public:
    struct MemoryPressureMetrics {
        size_t allocatedMemory;
        double executionTimeMs;
        size_t peakMemoryUsage;
        bool completedSuccessfully;
        std::string errorMessage;
        
        MemoryPressureMetrics(size_t alloc, double time, size_t peak, bool success = true, const std::string& error = "")
            : allocatedMemory(alloc), executionTimeMs(time), peakMemoryUsage(peak),
              completedSuccessfully(success), errorMessage(error) {}
    };
    
    static MemoryPressureTester& getInstance() {
        static MemoryPressureTester instance;
        return instance;
    }
    
    std::vector<MemoryPressureMetrics> testMemoryPressure(const std::vector<size_t>& memoryAllocations, 
                                                          nlm::SimulationStep steps) {
        std::vector<MemoryPressureMetrics> results;
        
        for (size_t allocation : memoryAllocations) {
            MemoryPressureMetrics result(allocation, 0.0, 0, false);
            
            try {
                auto config = std::make_shared<nlm::Config>();
                config->set("neuron_count", static_cast<int64_t>(allocation / 1024), nlm::ConfigSource::Default); // 1 byte = 1 neuron (simplified)
                config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
                
                nlm::Brain brain(config);
                brain.initialize();
                
                PerformanceTimer timer;
                runBrainSteps(brain, steps);
                double executionTime = timer.elapsedMs();
                
                size_t peakMemoryUsage = PerformanceMonitor::getInstance().getCurrentMemoryUsage();
                
                result = MemoryPressureMetrics(allocation, executionTime, peakMemoryUsage, true);
            } catch (const std::exception& e) {
                result.errorMessage = e.what();
            }
            
            results.push_back(result);
            
            // Small delay between tests
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        return results;
    }
    
    std::string analyzeMemoryPressure(const std::vector<MemoryPressureMetrics>& metrics) {
        std::stringstream analysis;
        analysis << "Memory Pressure Analysis\n";
        analysis << "=========================\n\n";
        
        if (metrics.empty()) {
            analysis << "No memory pressure test data available.\n";
            return analysis.str();
        }
        
        analysis << "Memory Allocation vs Performance:\n";
        for (const auto& metric : metrics) {
            if (metric.completedSuccessfully) {
                analysis << metric.allocatedMemory << " bytes -> " << metric.executionTimeMs << " ms, " 
                         << metric.peakMemoryUsage << " bytes peak\n";
            } else {
                analysis << metric.allocatedMemory << " bytes -> FAILED: " << metric.errorMessage << "\n";
            }
        }
        
        analysis << "\n\nMemory Efficiency Analysis:\n";
        double totalMemory = 0.0;
        double totalExecutionTime = 0.0;
        size_t successfulTests = 0;
        
        for (const auto& metric : metrics) {
            if (metric.completedSuccessfully) {
                totalMemory += metric.allocatedMemory;
                totalExecutionTime += metric.executionTimeMs;
                successfulTests++;
            }
        }
        
        if (successfulTests > 0) {
            double avgMemory = totalMemory / successfulTests;
            double avgExecutionTime = totalExecutionTime / successfulTests;
            double memoryEfficiency = avgMemory / avgExecutionTime; // bytes per ms
            
            analysis << "Average memory: " << avgMemory << " bytes\n";
            analysis << "Average execution time: " << avgExecutionTime << " ms\n";
            analysis << "Memory efficiency: " << memoryEfficiency << " bytes/ms\n";
        }
        
        return analysis.str();
    }
    
private:
    MemoryPressureTester() = default;
};

// Memory fragmentation testing
class MemoryFragmentationTester {
public:
    struct FragmentationMetrics {
        std::vector<size_t> allocationSizes;
        std::vector<size_t> deallocationSizes;
        double fragmentationScore;
        size_t totalAllocations;
        size_t totalDeallocations;
        
        FragmentationMetrics() : fragmentationScore(0.0), totalAllocations(0), totalDeallocations(0) {}
    };
    
    static MemoryFragmentationTester& getInstance() {
        static MemoryFragmentationTester instance;
        return instance;
    }
    
    FragmentationMetrics testFragmentation(const std::vector<size_t>& allocationSizes) {
        FragmentationMetrics result;
        result.allocationSizes = allocationSizes;
        
        // Simulate memory allocation/deallocation
        std::vector<void*> allocatedBlocks;
        
        for (size_t size : allocationSizes) {
            // Allocate memory (simulated)
            void* block = std::malloc(size);
            if (block) {
                allocatedBlocks.push_back(block);
                result.totalAllocations++;
            }
        }
        
        // Deallocate memory
        for (void* block : allocatedBlocks) {
            std::free(block);
            result.totalDeallocations++;
        }
        
        // Calculate fragmentation score (simplified)
        // Higher variance in allocation sizes = higher fragmentation
        if (!allocationSizes.empty()) {
            double meanSize = std::accumulate(allocationSizes.begin(), allocationSizes.end(), 0.0) / allocationSizes.size();
            double variance = 0.0;
            for (size_t size : allocationSizes) {
                variance += std::pow(size - meanSize, 2);
            }
            variance /= allocationSizes.size();
            result.fragmentationScore = std::sqrt(variance) / meanSize; // Coefficient of variation
        }
        
        return result;
    }
    
    std::string analyzeFragmentation(const FragmentationMetrics& metrics) {
        std::stringstream analysis;
        analysis << "Memory Fragmentation Analysis\n";
        analysis << "============================\n\n";
        
        analysis << "Allocation sizes: ";
        for (size_t size : metrics.allocationSizes) {
            analysis << size << " bytes, ";
        }
        analysis << "\n\n";
        
        analysis << "Total allocations: " << metrics.totalAllocations << "\n";
        analysis << "Total deallocations: " << metrics.totalDeallocations << "\n";
        analysis << "Fragmentation score: " << metrics.fragmentationScore << "\n";
        
        // Interpret fragmentation score
        if (metrics.fragmentationScore < 0.1) {
            analysis << "Fragmentation level: Low\n";
        } else if (metrics.fragmentationScore < 0.3) {
            analysis << "Fragmentation level: Medium\n";
        } else {
            analysis << "Fragmentation level: High\n";
        }
        
        return analysis.str();
    }
    
private:
    MemoryFragmentationTester() = default;
};

// Memory leak detection (enhanced)
class EnhancedMemoryLeakDetector {
public:
    struct LeakInfo {
        std::string allocationSite;
        size_t leakedBytes;
        std::chrono::steady_clock::time_point firstLeakTime;
        std::chrono::steady_clock::time_point lastLeakTime;
        std::vector<void*> leakedBlocks;
        
        LeakInfo(const std::string& site) 
            : allocationSite(site), leakedBytes(0), firstLeakTime(std::chrono::steady_clock::now()) {}
    };
    
    static EnhancedMemoryLeakDetector& getInstance() {
        static EnhancedMemoryLeakDetector instance;
        return instance;
    }
    
    void recordAllocation(void* ptr, size_t size, const std::string& site) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = activeAllocations_.find(ptr);
        if (it == activeAllocations_.end()) {
            // New allocation
            activeAllocations_[ptr] = AllocationInfo(size, site, std::chrono::steady_clock::now());
        } else {
            // Update existing allocation
            it->second.size = size;
            it->second.lastUpdated = std::chrono::steady_clock::now();
        }
        
        // Track in leak database
        leakDatabase_[site].leakedBytes += size;
        leakDatabase_[site].leakedBlocks.push_back(ptr);
        if (leakDatabase_[site].firstLeakTime == std::chrono::steady_clock::time_point()) {
            leakDatabase_[site].firstLeakTime = std::chrono::steady_clock::now();
        }
        leakDatabase_[site].lastLeakTime = std::chrono::steady_clock::now();
    }
    
    void recordDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = activeAllocations_.find(ptr);
        if (it != activeAllocations_.end()) {
            // Remove from active allocations
            size_t size = it->second.size;
            std::string site = it->second.site;
            activeAllocations_.erase(it);
            
            // Update leak database
            if (leakDatabase_.find(site) != leakDatabase_.end()) {
                leakDatabase_[site].leakedBytes -= size;
                
                // Remove the pointer from leaked blocks
                auto& blocks = leakDatabase_[site].leakedBlocks;
                blocks.erase(std::remove(blocks.begin(), blocks.end(), ptr), blocks.end());
                
                if (leakDatabase_[site].leakedBlocks.empty()) {
                    leakDatabase_.erase(site);
                }
            }
        }
    }
    
    std::vector<LeakInfo> getActiveLeaks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<LeakInfo> result;
        
        for (const auto& pair : leakDatabase_) {
            result.push_back(pair.second);
        }
        
        return result;
    }
    
    size_t getTotalLeakedBytes() const {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t total = 0;
        for (const auto& pair : leakDatabase_) {
            total += pair.second.leakedBytes;
        }
        return total;
    }
    
    bool hasLeaks() const {
        return !leakDatabase_.empty();
    }
    
    std::string getLeakReport() const {
        std::stringstream report;
        report << "Enhanced Memory Leak Report\n";
        report << "==========================\n\n";
        
        if (leakDatabase_.empty()) {
            report << "No memory leaks detected.\n";
            return report.str();
        }
        
        report << "Active Leaks by Site:\n";
        for (const auto& pair : leakDatabase_) {
            const auto& leakInfo = pair.second;
            report << "  Site: " << leakInfo.allocationSite << "\n";
            report << "    Leaked bytes: " << leakInfo.leakedBytes << "\n";
            report << "    First leak: " << 
                std::chrono::duration<double, std::milli>(leakInfo.firstLeakTime.time_since_epoch()).count() << " ms\n";
            report << "    Last leak: " << 
                std::chrono::duration<double, std::milli>(leakInfo.lastLeakTime.time_since_epoch()).count() << " ms\n";
            report << "    Leaked blocks: " << leakInfo.leakedBlocks.size() << "\n";
        }
        
        report << "\nTotal leaked bytes: " << getTotalLeakedBytes() << "\n";
        
        return report.str();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        activeAllocations_.clear();
        leakDatabase_.clear();
    }
    
private:
    EnhancedMemoryLeakDetector() = default;
    
    struct AllocationInfo {
        size_t size;
        std::string site;
        std::chrono::steady_clock::time_point lastUpdated;
        
        AllocationInfo(size_t sz, const std::string& site, const std::chrono::steady_clock::time_point& time)
            : size(sz), site(site), lastUpdated(time) {}
    };
    
    mutable std::mutex mutex_;
    std::unordered_map<void*, AllocationInfo> activeAllocations_;
    std::unordered_map<std::string, LeakInfo> leakDatabase_;
};

// Memory pool testing
class MemoryPoolTester {
public:
    struct PoolMetrics {
        std::string poolName;
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t currentSize;
        size_t maxSize;
        double fragmentationRatio;
        
        PoolMetrics(const std::string& name) : poolName(name), totalAllocations(0), 
                                             totalDeallocations(0), currentSize(0), maxSize(0),
                                             fragmentationRatio(0.0) {}
    };
    
    static MemoryPoolTester& getInstance() {
        static MemoryPoolTester instance;
        return instance;
    }
    
    std::vector<PoolMetrics> testMemoryPools(const std::vector<std::string>& poolNames,
                                             const std::vector<size_t>& allocationSizes,
                                             nlm::SimulationStep steps) {
        std::vector<PoolMetrics> results;
        
        for (const auto& poolName : poolNames) {
            PoolMetrics result(poolName);
            
            try {
                // Simulate memory pool operations
                std::vector<void*> allocations;
                
                for (size_t size : allocationSizes) {
                    // Allocate from pool (simulated)
                    void* block = std::malloc(size);
                    if (block) {
                        allocations.push_back(block);
                        result.totalAllocations++;
                        result.currentSize += size;
                        if (result.currentSize > result.maxSize) {
                            result.maxSize = result.currentSize;
                        }
                    }
                }
                
                // Run simulation
                runBrainSteps(brain, steps);
                
                // Deallocate from pool
                for (void* block : allocations) {
                    std::free(block);
                    result.totalDeallocations++;
                }
                
                // Calculate fragmentation ratio
                if (!allocationSizes.empty()) {
                    double totalSize = std::accumulate(allocationSizes.begin(), allocationSizes.end(), 0.0);
                    result.fragmentationRatio = static_cast<double>(result.maxSize) / totalSize;
                }
                
            } catch (const std::exception& e) {
                // Handle errors
            }
            
            results.push_back(result);
        }
        
        return results;
    }
    
    std::string analyzeMemoryPools(const std::vector<PoolMetrics>& metrics) {
        std::stringstream analysis;
        analysis << "Memory Pool Analysis\n";
        analysis << "=====================\n\n";
        
        if (metrics.empty()) {
            analysis << "No memory pool test data available.\n";
            return analysis.str();
        }
        
        analysis << "Pool Performance:\n";
        for (const auto& metric : metrics) {
            analysis << metric.poolName << ":\n";
            analysis << "  Total allocations: " << metric.totalAllocations << "\n";
            analysis << "  Total deallocations: " << metric.totalDeallocations << "\n";
            analysis << "  Current size: " << metric.currentSize << " bytes\n";
            analysis << "  Max size: " << metric.maxSize << " bytes\n";
            analysis << "  Fragmentation ratio: " << metric.fragmentationRatio << "\n";
        }
        
        return analysis.str();
    }
    
private:
    MemoryPoolTester() = default;
};

// Memory boundary testing
class MemoryBoundaryTester {
public:
    struct BoundaryTestResult {
        size_t testInput;
        bool passed;
        std::string errorMessage;
        std::chrono::steady_clock::time_point timestamp;
        
        BoundaryTestResult(size_t input) : testInput(input), passed(false) {
            timestamp = std::chrono::steady_clock::now();
        }
    };
    
    static MemoryBoundaryTester& getInstance() {
        static MemoryBoundaryTester instance;
        return instance;
    }
    
    std::vector<BoundaryTestResult> testMemoryBoundaries(const std::vector<size_t>& testInputs,
                                                         nlm::SimulationStep steps) {
        std::vector<BoundaryTestResult> results;
        
        for (size_t input : testInputs) {
            BoundaryTestResult result(input);
            
            try {
                // Test boundary conditions
                auto config = std::make_shared<nlm::Config>();
                
                // Test 1: Very small allocation
                if (input < 100) {
                    config->set("neuron_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
                }
                // Test 2: Normal allocation
                else if (input < 1000) {
                    config->set("neuron_count", static_cast<int64_t>(input / 100), nlm::ConfigSource::Default);
                }
                // Test 3: Large allocation
                else {
                    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
                }
                
                config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
                
                nlm::Brain brain(config);
                brain.initialize();
                
                runBrainSteps(brain, steps);
                
                result.passed = true;
                
            } catch (const std::exception& e) {
                result.errorMessage = e.what();
            }
            
            results.push_back(result);
        }
        
        return results;
    }
    
    std::string analyzeBoundaries(const std::vector<BoundaryTestResult>& results) {
        std::stringstream analysis;
        analysis << "Memory Boundary Analysis\n";
        analysis << "=========================\n\n";
        
        if (results.empty()) {
            analysis << "No boundary test data available.\n";
            return analysis.str();
        }
        
        analysis << "Boundary Test Results:\n";
        for (const auto& result : results) {
            analysis << "  Input: " << result.testInput << " -> " 
                     << (result.passed ? "PASSED" : "FAILED") << std::endl;
            if (!result.errorMessage.empty()) {
                analysis << "    Error: " << result.errorMessage << std::endl;
            }
        }
        
        // Find boundary between passing and failing tests
        size_t minFailing = 0;
        size_t maxPassing = 0;
        
        for (const auto& result : results) {
            if (result.passed) {
                maxPassing = std::max(maxPassing, result.testInput);
            } else {
                minFailing = std::min(minFailing, result.testInput);
            }
        }
        
        analysis << "\n\nBoundary Analysis:\n";
        if (maxPassing > 0 && minFailing > 0) {
            analysis << "  Maximum passing input: " << maxPassing << "\n";
            analysis << "  Minimum failing input: " << minFailing << "\n";
            analysis << "  Boundary region: " << maxPassing << " - " << minFailing << "\n";
        }
        
        return analysis.str();
    }
    
private:
    MemoryBoundaryTester() = default;
};

} // namespace test_memory_utils
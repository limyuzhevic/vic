# Memory Management Examples

This directory contains examples for using the NLM memory management system, including debugging, optimization, and profiling examples.

## Example 1: Basic Memory Tracking

### File: example1_basic_memory_tracking.cpp

```cpp
#include "../src/memory/MemoryTracker.hpp"
#include <iostream>
#include <chrono>

int main() {
    // Create memory tracker instance
    nlm::MemoryTracker& tracker = nlm::MemoryTracker::getInstance();
    
    // Enable memory tracking
    tracker.enable(true);
    tracker.setMaxHistorySize(10000);
    tracker.setSamplingRate(1.0);
    
    std::cout << "=== NLM Memory Tracking Example ===" << std::endl;
    std::cout << "Memory tracking enabled" << std::endl;
    std::cout << "Max history size: " << tracker.getMaxHistorySize() << std::endl;
    std::cout << "Sampling rate: " << tracker.getSamplingRate() << std::endl;
    std::cout << std::endl;
    
    // Simulate memory allocations
    std::cout << "Allocating memory..." << std::endl;
    
    // Allocate some memory blocks
    size_t block1 = 1024;
    size_t block2 = 2048;
    size_t block3 = 4096;
    
    // Track allocations with stack traces
    tracker.trackAllocation(reinterpret_cast<size_t>(&block1), block1, 
                           "example1_basic_memory_tracking.cpp:25", "Main function");
    tracker.trackAllocation(reinterpret_cast<size_t>(&block2), block2,
                           "example1_basic_memory_tracking.cpp:26", "Main function");
    tracker.trackAllocation(reinterpret_cast<size_t>(&block3), block3,
                           "example1_basic_memory_tracking.cpp:27", "Main function");
    
    // Update simulation time
    tracker.startStep(0, 0.0);
    
    std::cout << "Memory allocated successfully" << std::endl;
    std::cout << "Current memory usage: " << tracker.getCurrentMemoryUsage() << " bytes" << std::endl;
    std::cout << "Total memory allocated: " << tracker.getTotalMemoryAllocated() << " bytes" << std::endl;
    std::cout << std::endl;
    
    // Get efficiency metrics
    auto metrics = tracker.getEfficiencyMetrics();
    std::cout << "=== Efficiency Metrics ===" << std::endl;
    std::cout << "Total allocated: " << metrics.totalAllocated << " bytes" << std::endl;
    std::cout << "Current usage: " << metrics.currentMemoryUsage << " bytes" << std::endl;
    std::cout << "Peak usage: " << metrics.peakMemoryUsage << " bytes" << std::endl;
    std::cout << "Allocation efficiency: " << (metrics.allocationEfficiency * 100.0) << "%" << std::endl;
    std::cout << "Fragmentation ratio: " << metrics.fragmentationRatio << std::endl;
    std::cout << "Average allocation size: " << metrics.averageAllocationSize << " bytes" << std::endl;
    std::cout << std::endl;
    
    // Simulate deallocations
    std::cout << "Deallocating memory..." << std::endl;
    
    tracker.trackDeallocation(reinterpret_cast<size_t>(&block1), "example1_basic_memory_tracking.cpp:35");
    tracker.trackDeallocation(reinterpret_cast<size_t>(&block2), "example1_basic_memory_tracking.cpp:36");
    
    std::cout << "Memory deallocated successfully" << std::endl;
    std::cout << "Current memory usage: " << tracker.getCurrentMemoryUsage() << " bytes" << std::endl;
    std::cout << "Total memory deallocated: " << tracker.getTotalMemoryDeallocated() << " bytes" << std::endl;
    std::cout << std::endl;
    
    // Generate and save report
    std::cout << "Generating memory report..." << std::endl;
    std::string report = tracker.generateReport();
    std::cout << report << std::endl;
    
    tracker.saveReport("basic_memory_tracking_report.txt");
    
    // End simulation
    tracker.endStep(0, 1.0);
    
    // Detect memory leaks
    std::cout << "Detecting memory leaks..." << std::endl;
    auto leaks = tracker.detectMemoryLeaks();
    if (leaks.empty()) {
        std::cout << "No memory leaks detected." << std::endl;
    } else {
        std::cout << "Detected " << leaks.size() << " potential memory leaks:" << std::endl;
        for (size_t i = 0; i < leaks.size(); ++i) {
            const auto& leak = leaks[i];
            std::cout << "  Leak " << i + 1 << ": " << leak.type << " (Address: 0x"
                     << std::hex << leak.address << std::dec << ") Size: " << leak.size
                     << " bytes Confidence: " << (leak.confidenceScore * 100.0) << "%" << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Get optimization recommendations
    std::cout << "Getting optimization recommendations..." << std::endl;
    auto recommendations = tracker.getOptimizationRecommendations();
    if (recommendations.empty()) {
        std::cout << "No optimization recommendations." << std::endl;
    } else {
        std::cout << "Optimization recommendations:" << std::endl;
        for (size_t i = 0; i < recommendations.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << recommendations[i] << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Cleanup
    tracker.flush();
    std::cout << "Memory tracking completed successfully." << std::endl;
    
    return 0;
}
```

## Example 2: Memory Debugging

### File: example2_memory_debugging.cpp

```cpp
#include "../src/memory/MemoryDebug.hpp"
#include <iostream>
#include <chrono>

int main() {
    // Create memory debug instance
    nlm::MemoryDebug& debugger = nlm::MemoryDebug::getInstance();
    
    // Enable memory debugging
    debugger.enable(true);
    
    // Configure debugging options
    nlm::MemoryDebugConfig config;
    config.enableStackTrace = true;
    config.enablePatternTracking = true;
    config.enableCorruptionDetection = true;
    config.enableAccessTracking = true;
    config.enableSizeTracking = true;
    config.corruptionDetectionThreshold = 0.8;
    config.maxAccessHistorySize = 1000;
    config.debugOutputFile = "memory_debug.log";
    
    debugger.setConfig(config);
    
    std::cout << "=== NLM Memory Debugging Example ===" << std::endl;
    std::cout << "Memory debugging enabled" << std::endl;
    std::cout << "Debug output file: " << config.debugOutputFile << std::endl;
    std::cout << "Max access history size: " << config.maxAccessHistorySize << std::endl;
    std::cout << std::endl;
    
    // Simulate memory allocations with debugging
    std::cout << "Allocating and debugging memory..." << std::endl;
    
    // Allocate memory with debugging
    size_t allocatedBlock = 4096;
    
    // Track allocation with debugging
    debugger.trackAllocation(reinterpret_cast<size_t>(&allocatedBlock), allocatedBlock,
                            "example2_memory_debugging.cpp:35", 0);
    
    // Debug the allocated block
    debugger.debugBlock(reinterpret_cast<size_t>(&allocatedBlock), allocatedBlock,
                       "example2_memory_debugging.cpp:40");
    
    // Simulate memory access
    debugger.recordAccess(reinterpret_cast<size_t>(&allocatedBlock), allocatedBlock, 0);
    
    // Update simulation time
    debugger.startStep(0, 0.0);
    
    std::cout << "Memory allocated and debugged successfully" << std::endl;
    
    // Check for corruption
    std::cout << "Checking for memory corruption..." << std::endl;
    auto corruptions = debugger.detectCorruption(reinterpret_cast<size_t>(&allocatedBlock), allocatedBlock);
    if (corruptions.empty()) {
        std::cout << "No memory corruption detected." << std::endl;
    } else {
        std::cout << "Detected " << corruptions.size() << " memory corruptions:" << std::endl;
        for (size_t i = 0; i < corruptions.size(); ++i) {
            const auto& corruption = corruptions[i];
            std::cout << "  Corruption " << i + 1 << ": " << corruption.type
                     << " (Address: 0x" << std::hex << corruption.address << std::dec << ")"
                     << " Size: " << corruption.size << " bytes" << std::endl;
            std::cout << "    Site: " << corruption.corruptionSite << std::endl;
            std::cout << "    Thread: " << corruption.threadId << std::endl;
            std::cout << "    Fatal: " << (corruption.fatal ? "Yes" : "No") << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Get block debug info
    std::cout << "Getting block debug info..." << std::endl;
    auto debugInfo = debugger.getBlockDebugInfo(reinterpret_cast<size_t>(&allocatedBlock));
    if (debugInfo.isValid) {
        std::cout << "Block debug info:" << std::endl;
        std::cout << "  Address: 0x" << std::hex << debugInfo.address << std::dec << std::endl;
        std::cout << "  Size: " << debugInfo.size << " bytes" << std::endl;
        std::cout << "  Valid: " << (debugInfo.isValid ? "Yes" : "No") << std::endl;
        std::cout << "  Allocated: " << (debugInfo.isAllocated ? "Yes" : "No") << std::endl;
        std::cout << "  Allocation site: " << debugInfo.allocationSite << std::endl;
        std::cout << "  Allocation thread: " << debugInfo.allocationThreadId << std::endl;
        std::cout << "  Corrupted: " << (debugInfo.isCorrupted ? "Yes" : "No") << std::endl;
    } else {
        std::cout << "Block not found or invalid." << std::endl;
    }
    std::cout << std::endl;
    
    // Deallocate memory with debugging
    std::cout << "Deallocating memory with debugging..." << std::endl;
    debugger.trackDeallocation(reinterpret_cast<size_t>(&allocatedBlock), "example2_memory_debugging.cpp:65", 0);
    
    std::cout << "Memory deallocated and debugged successfully" << std::endl;
    std::cout << std::endl;
    
    // Generate debug report
    std::cout << "Generating debug report..." << std::endl;
    std::string report = debugger.generateReport();
    std::cout << report << std::endl;
    
    debugger.saveReport("memory_debugging_report.txt");
    
    // End simulation
    debugger.endStep(0, 1.0);
    
    // Find memory issues
    std::cout << "Finding memory issues..." << std::endl;
    
    // Find unfreed blocks
    auto unfreed = debugger.findUnfreedBlocks();
    std::cout << "Unfreed blocks: " << unfreed.size() << std::endl;
    
    // Find double frees
    auto doubleFrees = debugger.findDoubleFrees();
    std::cout << "Double frees: " << doubleFrees.size() << std::endl;
    
    // Find buffer overflows
    auto overflows = debugger.findBufferOverflows();
    std::cout << "Buffer overflows: " << overflows.size() << std::endl;
    
    // Find use after free
    auto useAfterFrees = debugger.findUseAfterFree();
    std::cout << "Use after free: " << useAfterFrees.size() << std::endl;
    std::cout << std::endl;
    
    // Get debug recommendations
    std::cout << "Getting debug recommendations..." << std::endl;
    auto recommendations = debugger.getDebugRecommendations();
    if (recommendations.empty()) {
        std::cout << "No debug recommendations." << std::endl;
    } else {
        std::cout << "Debug recommendations:" << std::endl;
        for (size_t i = 0; i < recommendations.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << recommendations[i] << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Inspect memory state
    std::cout << "Inspecting memory state..." << std::endl;
    std::string memoryState = debugger.inspectMemoryState();
    std::cout << memoryState << std::endl;
    
    // Analyze memory health
    std::cout << "Analyzing memory health..." << std::endl;
    std::string memoryHealth = debugger.analyzeMemoryHealth();
    std::cout << memoryHealth << std::endl;
    
    // Cleanup
    debugger.flush();
    std::cout << "Memory debugging completed successfully." << std::endl;
    
    return 0;
}
```

## Example 3: Memory Profiling

### File: example3_memory_profiling.cpp

```cpp
#include "../src/memory/MemoryProfiler.hpp"
#include "../src/memory/MemoryHierarchyStats.hpp"
#include <iostream>
#include <chrono>
#include <vector>

int main() {
    // Create memory profiler instance
    nlm::MemoryProfiler& profiler = nlm::MemoryProfiler::getInstance();
    
    // Enable memory profiling
    profiler.enable(true);
    
    // Configure profiling options
    profiler.setProfileType(nlm::ProfileType::AllocationProfile);
    profiler.setProfileInterval(1.0);
    profiler.setPressureThreshold(0.8);
    
    std::cout << "=== NLM Memory Profiling Example ===" << std::endl;
    std::cout << "Memory profiling enabled" << std::endl;
    std::cout << "Profile type: " << profiler.getProfileType() << std::endl;
    std::cout << "Profile interval: " << profiler.getProfileInterval() << " seconds" << std::endl;
    std::cout << "Pressure threshold: " << profiler.getPressureThreshold() << std::endl;
    std::cout << std::endl;
    
    // Initialize memory hierarchy stats
    std::vector<nlm::MemoryHierarchyStats> hierarchyStats;
    
    nlm::MemoryHierarchyStats l1Cache;
    l1Cache.level = nlm::MemoryLevel::L1_CACHE;
    l1Cache.totalSize = 32 * 1024; // 32KB
    l1Cache.usedSize = 8 * 1024; // 8KB
    l1Cache.availableSize = l1Cache.totalSize - l1Cache.usedSize;
    l1Cache.hitCount = 500;
    l1Cache.missCount = 50;
    l1Cache.averageLatency = 0.5; // 0.5 ns
    hierarchyStats.push_back(l1Cache);
    
    nlm::MemoryHierarchyStats l2Cache;
    l2Cache.level = nlm::MemoryLevel::L2_CACHE;
    l2Cache.totalSize = 256 * 1024; // 256KB
    l2Cache.usedSize = 64 * 1024; // 64KB
    l2Cache.availableSize = l2Cache.totalSize - l2Cache.usedSize;
    l2Cache.hitCount = 2000;
    l2Cache.missCount = 300;
    l2Cache.averageLatency = 5.0; // 5 ns
    hierarchyStats.push_back(l2Cache);
    
    nlm::MemoryHierarchyStats l3Cache;
    l3Cache.level = nlm::MemoryLevel::L3_CACHE;
    l3Cache.totalSize = 8 * 1024 * 1024; // 8MB
    l3Cache.usedSize = 2 * 1024 * 1024; // 2MB
    l3Cache.availableSize = l3Cache.totalSize - l3Cache.usedSize;
    l3Cache.hitCount = 8000;
    l3Cache.missCount = 2000;
    l3Cache.averageLatency = 20.0; // 20 ns
    hierarchyStats.push_back(l3Cache);
    
    nlm::MemoryHierarchyStats ram;
    ram.level = nlm::MemoryLevel::RAM;
    ram.totalSize = 16 * 1024 * 1024 * 1024; // 16GB
    ram.usedSize = 4 * 1024 * 1024 * 1024; // 4GB
    ram.availableSize = ram.totalSize - ram.usedSize;
    ram.hitCount = 0;
    ram.missCount = 8000;
    ram.averageLatency = 100.0; // 100 ns
    hierarchyStats.push_back(ram);
    
    // Update memory hierarchy
    profiler.updateMemoryHierarchy(hierarchyStats);
    
    // Initialize memory pressure
    nlm::MemoryPressure pressure;
    pressure.currentPressure = 0.6; // 60% pressure
    pressure.pressureThreshold = 0.8; // 80% threshold
    pressure.availableMemory = 6 * 1024 * 1024 * 1024; // 6GB available
    pressure.totalMemory = 16 * 1024 * 1024 * 1024; // 16GB total
    pressure.pressureIncreaseRate = 0.01; // 1% per second
    pressure.isCritical = false;
    
    // Update memory pressure
    profiler.updateMemoryPressure(pressure);
    
    // Start profiling session
    std::cout << "Starting profiling session..." << std::endl;
    profiler.startProfile("Memory Profiling Session");
    
    // Simulate simulation steps
    std::cout << "Simulating simulation steps..." << std::endl;
    
    for (size_t step = 0; step < 10; ++step) {
        profiler.startStep(step, step * 0.1);
        
        // Simulate memory allocations
        size_t allocation1 = 1024;
        size_t allocation2 = 2048;
        size_t allocation3 = 4096;
        
        profiler.recordAllocation(reinterpret_cast<size_t>(&allocation1), allocation1,
                                 "example3_memory_profiling.cpp:80", "Stack trace 1", 0);
        profiler.recordAllocation(reinterpret_cast<size_t>(&allocation2), allocation2,
                                 "example3_memory_profiling.cpp:81", "Stack trace 2", 0);
        profiler.recordAllocation(reinterpret_cast<size_t>(&allocation3), allocation3,
                                 "example3_memory_profiling.cpp:82", "Stack trace 3", 0);
        
        // Simulate memory accesses
        profiler.recordAccess(reinterpret_cast<size_t>(&allocation1), nlm::MemoryLevel::L1_CACHE, 0, true, false, 0.5, allocation1, "example3_memory_profiling.cpp:85");
        profiler.recordAccess(reinterpret_cast<size_t>(&allocation2), nlm::MemoryLevel::L2_CACHE, 0, true, false, 5.0, allocation2, "example3_memory_profiling.cpp:86");
        profiler.recordAccess(reinterpret_cast<size_t>(&allocation3), nlm::MemoryLevel::RAM, 0, true, false, 100.0, allocation3, "example3_memory_profiling.cpp:87");
        
        // Simulate pool usage
        profiler.recordPoolUsage("NeuronPool", step * 1024, 10000 - step * 1024);
        profiler.recordPoolAllocation("NeuronPool", allocation1);
        
        // End simulation step
        profiler.endStep(step, (step + 1) * 0.1);
        
        // Update memory pressure
        pressure.currentPressure = static_cast<double>(step + 1) / 100.0; // Increasing pressure
        profiler.updateMemoryPressure(pressure);
    }
    
    // End profiling session
    std::cout << "Ending profiling session..." << std::endl;
    profiler.endProfile("Memory Profiling Session");
    
    // Get current profile
    auto currentProfile = profiler.getCurrentProfile();
    std::cout << "=== Current Profile ===" << std::endl;
    std::cout << "Session: " << currentProfile.sessionName << std::endl;
    std::cout << "Start time: " << currentProfile.startTime << std::endl;
    std::cout << "End time: " << currentProfile.endTime << std::endl;
    std::cout << "Steps: " << currentProfile.stepCount << std::endl;
    std::cout << "Memory accesses: " << currentProfile.accesses.size() << std::endl;
    std::cout << "Allocations: " << currentProfile.allocations.size() << std::endl;
    std::cout << "Deallocations: " << currentProfile.deallocations.size() << std::endl;
    std::cout << "Memory pressure: " << currentProfile.pressure.currentPressure << std::endl;
    std::cout << std::endl;
    
    // Get profile history
    auto profileHistory = profiler.getProfileHistory();
    std::cout << "Profile history: " << profileHistory.size() << " sessions" << std::endl;
    std::cout << std::endl;
    
    // Get optimization recommendations
    std::cout << "Getting optimization recommendations..." << std::endl;
    auto recommendations = profiler.getOptimizationRecommendations();
    if (recommendations.empty()) {
        std::cout << "No optimization recommendations." << std::endl;
    } else {
        std::cout << "Optimization recommendations:" << std::endl;
        for (size_t i = 0; i < recommendations.size(); ++i) {
            const auto& rec = recommendations[i];
            std::cout << "  " << i + 1 << ". " << rec.optimizationType << std::endl;
            std::cout << "     Description: " << rec.description << std::endl;
            std::cout << "     Potential gain: " << rec.potentialGain << "%" << std::endl;
            std::cout << "     Auto-fixable: " << (rec.isAutoFixable ? "Yes" : "No") << std::endl;
            std::cout << "     Implementation cost: " << rec.implementationCost << "/10" << std::endl;
            std::cout << "     Implementation plan: " << rec.implementationPlan << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Generate profile report
    std::cout << "Generating profile report..." << std::endl;
    std::string profileReport = profiler.generateProfileReport();
    std::cout << profileReport << std::endl;
    
    profiler.saveProfileReport("memory_profiling_report.txt");
    
    // Generate optimization report
    std::cout << "Generating optimization report..." << std::endl;
    profiler.saveOptimizationReport("memory_optimization_report.txt");
    
    // Get performance alerts
    std::cout << "Getting performance alerts..." << std::endl;
    auto alerts = profiler.getPerformanceAlerts();
    if (alerts.empty()) {
        std::cout << "No performance alerts." << std::endl;
    } else {
        std::cout << "Performance alerts:" << std::endl;
        for (size_t i = 0; i < alerts.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << alerts[i] << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Cleanup
    profiler.flush();
    std::cout << "Memory profiling completed successfully." << std::endl;
    
    return 0;
}
```

## Example 4: Integration Example

### File: example4_integration.cpp

```cpp
#include "../src/memory/MemoryTracker.hpp"
#include "../src/memory/MemoryDebug.hpp"
#include "../src/memory/MemoryProfiler.hpp"
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== NLM Memory Management Integration Example ===" << std::endl;
    
    // Create memory management instances
    nlm::MemoryTracker& tracker = nlm::MemoryTracker::getInstance();
    nlm::MemoryDebug& debugger = nlm::MemoryDebug::getInstance();
    nlm::MemoryProfiler& profiler = nlm::MemoryProfiler::getInstance();
    
    // Enable all memory management systems
    tracker.enable(true);
    debugger.enable(true);
    profiler.enable(true);
    
    // Configure systems
    tracker.setMaxHistorySize(50000);
    profiler.setProfileInterval(0.5);
    profiler.setPressureThreshold(0.7);
    
    // Integrate systems
    profiler.integrateWithMemoryTracker(&tracker);
    profiler.integrateWithMemoryDebug(&debugger);
    
    std::cout << "Memory management systems integrated successfully" << std::endl;
    std::cout << std::endl;
    
    // Start simulation
    std::cout << "Starting simulation..." << std::endl;
    size_t step = 0;
    double time = 0.0;
    
    for (size_t i = 0; i < 20; ++i) {
        // Start step
        tracker.startStep(step, time);
        debugger.startStep(step, time);
        profiler.startStep(step, time);
        
        // Simulate simulation with memory management
        std::cout << "Step " << step << ": Simulation running..." << std::endl;
        
        // Simulate memory allocations
        size_t alloc1 = 512;
        size_t alloc2 = 1024;
        size_t alloc3 = 2048;
        
        // Track with tracker
        tracker.trackAllocation(reinterpret_cast<size_t>(&alloc1), alloc1, "integration.cpp:60", nlm::AllocationType::Standard);
        tracker.trackAllocation(reinterpret_cast<size_t>(&alloc2), alloc2, "integration.cpp:61", nlm::AllocationType::Pool);
        tracker.trackAllocation(reinterpret_cast<size_t>(&alloc3), alloc3, "integration.cpp:62", nlm::AllocationType::Simulation);
        
        // Debug with debugger
        debugger.trackAllocation(reinterpret_cast<size_t>(&alloc1), alloc1, "integration.cpp:65", 0);
        debugger.debugBlock(reinterpret_cast<size_t>(&alloc1), alloc1, "integration.cpp:66");
        
        // Profile with profiler
        profiler.recordAccess(reinterpret_cast<size_t>(&alloc1), nlm::MemoryLevel::L1_CACHE, 0, true, false, 0.5, alloc1, "integration.cpp:69");
        profiler.recordAllocation(reinterpret_cast<size_t>(&alloc1), alloc1, "integration.cpp:70", "Integration stack trace", 0);
        profiler.recordPoolUsage("IntegrationPool", i * 100, 10000 - i * 100);
        
        // Update memory pressure
        nlm::MemoryPressure pressure;
        pressure.currentPressure = static_cast<double>(i + 1) / 200.0; // Varying pressure
        pressure.pressureThreshold = 0.7;
        pressure.availableMemory = (20 - i) * 1024 * 1024; // Remaining memory
        pressure.totalMemory = 20 * 1024 * 1024; // Total memory
        profiler.updateMemoryPressure(pressure);
        
        // Simulate time passing
        time += 0.1;
        step++;
        
        // End step
        tracker.endStep(step, time);
        debugger.endStep(step, time);
        profiler.endStep(step, time);
        
        // Show progress
        if ((i + 1) % 5 == 0) {
            std::cout << "  Progress: " << (i + 1) * 5 << "% completed" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // End simulation
    std::cout << "Simulation completed." << std::endl;
    std::cout << std::endl;
    
    // Generate comprehensive report
    std::cout << "Generating comprehensive report..." << std::endl;
    
    std::string trackerReport = tracker.generateReport();
    std::string debugReport = debugger.generateReport();
    std::string profilerReport = profiler.generateProfileReport();
    
    // Save reports
    tracker.saveReport("integration_tracker_report.txt");
    debugger.saveReport("integration_debugger_report.txt");
    profiler.saveProfileReport("integration_profiler_report.txt");
    
    // Get optimization recommendations from all systems
    auto trackerRecommendations = tracker.getOptimizationRecommendations();
    auto debugRecommendations = debugger.getDebugRecommendations();
    auto profilerRecommendations = profiler.getOptimizationRecommendations();
    
    // Combine recommendations
    std::vector<std::string> allRecommendations;
    allRecommendations.insert(allRecommendations.end(), trackerRecommendations.begin(), trackerRecommendations.end());
    allRecommendations.insert(allRecommendations.end(), debugRecommendations.begin(), debugRecommendations.end());
    allRecommendations.insert(allRecommendations.end(), profilerRecommendations.begin(), profilerRecommendations.end());
    
    std::cout << "=== Comprehensive Integration Report ===" << std::endl;
    std::cout << "Integration Summary:" << std::endl;
    std::cout << "  Memory Tracker: " << tracker.getCurrentMemoryUsage() << " bytes" << std::endl;
    std::cout << "  Memory Debug: " << (debugger.findUnfreedBlocks().size() > 0 ? "Issues detected" : "No issues") << std::endl;
    std::cout << "  Memory Profiler: " << profiler.getPerformanceAlerts().size() << " alerts" << std::endl;
    std::cout << "  Total optimization recommendations: " << allRecommendations.size() << std::endl;
    std::cout << std::endl;
    
    if (!allRecommendations.empty()) {
        std::cout << "Top recommendations:" << std::endl;
        size_t count = 0;
        for (const auto& rec : allRecommendations) {
            if (count >= 5) break;
            std::cout << "  - " << rec << std::endl;
            count++;
        }
    }
    
    std::cout << std::endl;
    
    // Cleanup
    tracker.flush();
    debugger.flush();
    profiler.flush();
    
    std::cout << "Integration example completed successfully." << std::endl;
    
    return 0;
}
```

## Building and Running Examples

### Prerequisites
- NLM framework with C++11 or higher compiler
- CMake 3.10 or higher

### Build Instructions
1. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

2. Configure with CMake:
   ```bash
   cmake ..
   ```

3. Build examples:
   ```bash
   make example1_basic_memory_tracking
   make example2_memory_debugging
   make example3_memory_profiling
   make example4_integration
   ```

4. Run examples:
   ```bash
   ./example1_basic_memory_tracking
   ./example2_memory_debugging
   ./example3_memory_profiling
   ./example4_integration
   ```

### Expected Output

#### Example 1: Basic Memory Tracking
- Shows memory tracking enabled with configuration
- Demonstrates allocation/deallocation tracking
- Shows efficiency metrics (allocation efficiency, fragmentation ratio)
- Detects memory leaks
- Shows optimization recommendations

#### Example 2: Memory Debugging
- Shows memory debugging with configuration
- Demonstrates memory block debugging
- Shows corruption detection
- Shows memory leak detection
- Shows debug recommendations

#### Example 3: Memory Profiling
- Shows memory profiling with configuration
- Demonstrates memory hierarchy analysis
- Shows memory pressure monitoring
- Shows optimization recommendations
- Generates comprehensive reports

#### Example 4: Integration
- Shows integration of all memory management systems
- Demonstrates comprehensive memory management
- Shows combined reports and recommendations
- Shows system coordination

## Files Included

1. **src/memory/** - Memory management source files
   - MemoryTracker.hpp, MemoryTracker.cpp
   - MemoryDebug.hpp, MemoryDebug.cpp
   - MemoryProfiler.hpp, MemoryProfiler.cpp

2. **python/** - Python bindings
   - bindings.cpp (extended with memory management)
   - __init__.py (framework for Python module)

3. **docs/** - Documentation
   - MEMORY_MANAGEMENT.md (comprehensive documentation)

4. **examples/memory/** - Example programs
   - example1_basic_memory_tracking.cpp
   - example2_memory_debugging.cpp
   - example3_memory_profiling.cpp
   - example4_integration.cpp

## Usage in Real Applications

### For Research
- Use Example 1 for basic memory tracking in research simulations
- Use Example 2 for debugging memory issues in complex simulations
- Use Example 3 for performance analysis and optimization
- Use Example 4 for comprehensive memory management integration

### For Development
- Use examples as starting points for custom memory management
- Extend examples for specific use cases
- Integrate with existing NLM components

### For Production
- Configure memory management systems for production use
- Use zero-overhead operation when disabled
- Set appropriate thresholds and sampling rates
- Monitor and optimize memory usage continuously

## License

This documentation is part of the NLM (Neural Learning Machine) framework.
See LICENSE file for details.

## Author

NLM Framework Team

## Version

1.0.0

Generated: 2026-09-25

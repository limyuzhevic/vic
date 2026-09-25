#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

// NLM headers
#include "src/agent/MemoryManagementAPI.hpp"
#include "src/agent/AdvancedConfig.hpp"

using namespace nlm;

// Memory management example
class MemoryManagementExample {
public:
    MemoryManagementExample() {
        std::cout << "=== NLM Memory Management Example ===" << std::endl;
    }
    
    void runExample() {
        std::cout << "Running memory management example..." << std::endl;
        
        // Initialize memory management system
        initializeMemoryManagementSystem();
        
        // Step 1: Create custom memory pools
        createMemoryPools();
        
        // Step 2: Allocate memory from pools
        allocateMemoryFromPools();
        
        // Step 3: Track memory allocations
        trackMemoryAllocations();
        
        // Step 4: Monitor memory usage
        monitorMemoryUsage();
        
        // Step 5: Detect memory leaks
        detectMemoryLeaks();
        
        // Step 6: Optimize memory usage
        optimizeMemoryUsage();
        
        // Step 7: Run memory monitoring
        runMemoryMonitoring();
        
        // Step 8: Integrate with NLM
        integrateWithNLM();
        
        // Step 9: Clean up
        cleanupMemoryManagement();
        
        std::cout << "Memory management example completed successfully!" << std::endl;
    }
    
private:
    void initializeMemoryManagementSystem() {
        std::cout << "\n1. Initializing memory management system..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Set memory management configuration
        nlm::Config memoryConfig;
        memoryConfig.setValue("memory.management_enabled", true);
        memoryConfig.setValue("memory.leak_detection_enabled", true);
        memoryConfig.setValue("memory.optimization_enabled", true);
        memoryConfig.setValue("memory.monitoring_enabled", true);
        memoryConfig.setValue("memory.threshold.alert_high", 1024 * 1024 * 1024);  // 1GB
        memoryConfig.setValue("memory.threshold.alert_low", 64 * 1024 * 1024);    // 64MB
        memoryConfig.setValue("memory.max_allocations", 1000);
        memoryConfig.setValue("memory.max_fragmentation", 20);
        
        memoryManager.setMemoryManagementConfig(memoryConfig);
        std::cout << "  Memory management configuration set successfully!" << std::endl;
        
        // Start memory monitoring
        if (memoryManager.startMemoryMonitoring()) {
            std::cout << "  Memory monitoring started successfully!" << std::endl;
        } else {
            std::cout << "  Failed to start memory monitoring!" << std::endl;
        }
        
        // Get system memory statistics
        MemoryManagementAPI::SystemMemoryStats stats = memoryManager.getSystemMemoryStats();
        std::cout << "  Initial memory statistics: " << stats.totalAllocatedBytes << " bytes, "
                  << stats.totalAllocationCount << " allocations" << std::endl;
    }
    
    void createMemoryPools() {
        std::cout << "\n2. Creating memory pools..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Create standard memory pool
        CustomMemoryPoolConfig standardConfig;
        standardConfig.poolName = "standard_pool";
        standardConfig.poolType = MemoryPoolType::Standard;
        standardConfig.initialSize = 1024 * 1024;  // 1MB
        standardConfig.maxSize = 10 * 1024 * 1024;  // 10MB
        standardConfig.chunkSize = 64;
        standardConfig.alignment = 8;
        standardConfig.allocatorType = "std::allocator";
        standardConfig.enableThreadLocal = false;
        standardConfig.enableStatsTracking = true;
        standardConfig.enableLeakDetection = true;
        
        if (memoryManager.createMemoryPool(standardConfig)) {
            std::cout << "  Standard memory pool created successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create standard memory pool!" << std::endl;
        }
        
        // Create custom memory pool
        CustomMemoryPoolConfig customConfig;
        customConfig.poolName = "neuron_pool";
        customConfig.poolType = MemoryPoolType::Custom;
        customConfig.initialSize = 2 * 1024 * 1024;  // 2MB
        customConfig.maxSize = 20 * 1024 * 1024;   // 20MB
        customConfig.chunkSize = 128;
        customConfig.alignment = 16;
        customConfig.allocatorType = "custom";
        customConfig.customParameters = {{"allocation_strategy", "first_fit"}, {"growth_strategy", "exponential"}};
        customConfig.enableThreadLocal = true;
        customConfig.enableStatsTracking = true;
        customConfig.enableLeakDetection = true;
        customConfig.growthFactor = 1.5;
        customConfig.shrinkFactor = 0.8;
        
        if (memoryManager.createMemoryPool(customConfig)) {
            std::cout << "  Custom neuron memory pool created successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create custom neuron memory pool!" << std::endl;
        }
        
        // Create thread-local memory pool
        CustomMemoryPoolConfig threadLocalConfig;
        threadLocalConfig.poolName = "threadlocal_pool";
        threadLocalConfig.poolType = MemoryPoolType::ThreadLocal;
        threadLocalConfig.initialSize = 512 * 1024;  // 512KB
        threadLocalConfig.maxSize = 5 * 1024 * 1024;  // 5MB
        threadLocalConfig.chunkSize = 32;
        threadLocalConfig.alignment = 4;
        threadLocalConfig.allocatorType = "custom";
        threadLocalConfig.customParameters = {{"thread_local_strategy", "per_thread"}};
        threadLocalConfig.enableThreadLocal = true;
        threadLocalConfig.enableStatsTracking = true;
        threadLocalConfig.enableLeakDetection = true;
        
        if (memoryManager.createMemoryPool(threadLocalConfig)) {
            std::cout << "  Thread-local memory pool created successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create thread-local memory pool!" << std::endl;
        }
        
        // Create shared memory pool
        CustomMemoryPoolConfig sharedConfig;
        sharedConfig.poolName = "shared_pool";
        sharedConfig.poolType = MemoryPoolType::Shared;
        sharedConfig.initialSize = 4 * 1024 * 1024;  // 4MB
        sharedConfig.maxSize = 50 * 1024 * 1024;    // 50MB
        sharedConfig.chunkSize = 256;
        sharedConfig.alignment = 32;
        sharedConfig.allocatorType = "shared";
        sharedConfig.customParameters = {{"sharing_strategy", "mmap"}, {"shared_memory_file", "/dev/shm/nlm_pool"}};
        sharedConfig.enableThreadLocal = false;
        sharedConfig.enableStatsTracking = true;
        sharedConfig.enableLeakDetection = true;
        
        if (memoryManager.createMemoryPool(sharedConfig)) {
            std::cout << "  Shared memory pool created successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create shared memory pool!" << std::endl;
        }
        
        // Create hierarchical memory pool
        CustomMemoryPoolConfig hierarchicalConfig;
        hierarchicalConfig.poolName = "hierarchical_pool";
        hierarchicalConfig.poolType = MemoryPoolType::Hierarchical;
        hierarchicalConfig.initialSize = 1024 * 1024;  // 1MB
        hierarchicalConfig.maxSize = 15 * 1024 * 1024;  // 15MB
        hierarchicalConfig.chunkSize = 64;
        hierarchicalConfig.alignment = 8;
        hierarchicalConfig.allocatorType = "hierarchical";
        hierarchicalConfig.customParameters = {
            {"level1_size", "512KB"},
            {"level2_size", "1MB"},
            {"level3_size", "2MB"},
            {"allocation_strategy", "best_fit"}
        };
        hierarchicalConfig.enableThreadLocal = true;
        hierarchicalConfig.enableStatsTracking = true;
        hierarchicalConfig.enableLeakDetection = true;
        
        if (memoryManager.createMemoryPool(hierarchicalConfig)) {
            std::cout << "  Hierarchical memory pool created successfully!" << std::endl;
        } else {
            std::cout << "  Failed to create hierarchical memory pool!" << std::endl;
        }
    }
    
    void allocateMemoryFromPools() {
        std::cout << "\n3. Allocating memory from pools..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Allocate memory from standard pool
        void* standardPtr = nullptr;
        if (memoryManager.allocateFromPool("standard_pool", 1024, standardPtr, "standard")) {
            std::cout << "  Memory allocated from standard pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to allocate memory from standard pool!" << std::endl;
        }
        
        // Allocate memory from neuron pool
        void* neuronPtr = nullptr;
        if (memoryManager.allocateFromPool("neuron_pool", 1024, neuronPtr, "neuron")) {
            std::cout << "  Memory allocated from neuron pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to allocate memory from neuron pool!" << std::endl;
        }
        
        // Allocate memory from thread-local pool
        void* threadLocalPtr = nullptr;
        if (memoryManager.allocateFromPool("threadlocal_pool", 512, threadLocalPtr, "threadlocal")) {
            std::cout << "  Memory allocated from thread-local pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to allocate memory from thread-local pool!" << std::endl;
        }
        
        // Allocate memory from shared pool
        void* sharedPtr = nullptr;
        if (memoryManager.allocateFromPool("shared_pool", 2048, sharedPtr, "shared")) {
            std::cout << "  Memory allocated from shared pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to allocate memory from shared pool!" << std::endl;
        }
        
        // Allocate memory from hierarchical pool
        void* hierarchicalPtr = nullptr;
        if (memoryManager.allocateFromPool("hierarchical_pool", 256, hierarchicalPtr, "hierarchical")) {
            std::cout << "  Memory allocated from hierarchical pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to allocate memory from hierarchical pool!" << std::endl;
        }
        
        // Track memory allocations
        void* trackedPtr1 = memoryManager.trackAllocation(
            1024,           // size
            "neuron",       // type
            __FILE__,       // file
            __LINE__,       // line
            "neural_network_simulation"  // context
        );
        
        void* trackedPtr2 = memoryManager.trackAllocation(
            512,            // size
            "synapse",      // type
            __FILE__,       // file
            __LINE__,       // line
            "neural_network_simulation"  // context
        );
        
        void* trackedPtr3 = memoryManager.trackAllocation(
            256,            // size
            "population",  // type
            __FILE__,      // file
            __LINE__,      // line
            "neural_network_simulation"  // context
        );
        
        std::cout << "  Memory allocations tracked successfully!" << std::endl;
        
        // Deallocate memory from pools
        if (trackedPtr1) {
            if (memoryManager.deallocateFromPool("neuron_pool", trackedPtr1, 1024)) {
                std::cout << "  Memory deallocated from neuron pool successfully!" << std::endl;
            } else {
                std::cout << "  Failed to deallocate memory from neuron pool!" << std::endl;
            }
        }
        
        if (trackedPtr2) {
            if (memoryManager.deallocateFromPool("standard_pool", trackedPtr2, 512)) {
                std::cout << "  Memory deallocated from standard pool successfully!" << std::endl;
            } else {
                std::cout << "  Failed to deallocate memory from standard pool!" << std::endl;
            }
        }
        
        if (trackedPtr3) {
            if (memoryManager.deallocateFromPool("threadlocal_pool", trackedPtr3, 256)) {
                std::cout << "  Memory deallocated from thread-local pool successfully!" << std::endl;
            } else {
                std::cout << "  Failed to deallocate memory from thread-local pool!" << std::endl;
            }
        }
    }
    
    void trackMemoryAllocations() {
        std::cout << "\n4. Tracking memory allocations..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Track additional memory allocations
        void* allocation1 = memoryManager.trackAllocation(
            2048,           // size
            "memory",       // type
            __FILE__,       // file
            __LINE__,       // line
            "memory_simulation"  // context
        );
        
        void* allocation2 = memoryManager.trackAllocation(
            1024,           // size
            "brain",        // type
            __FILE__,       // file
            __LINE__,       // line
            "brain_simulation"  // context
        );
        
        void* allocation3 = memoryManager.trackAllocation(
            512,            // size
            "network",      // type
            __FILE__,       // file
            __LINE__,       // line
            "network_simulation"  // context
        );
        
        std::cout << "  Additional memory allocations tracked successfully!" << std::endl;
        
        // Get active allocations
        std::vector<MemoryAllocationInfo> allocations = memoryManager.getActiveAllocations();
        std::cout << "  Active allocations: " << allocations.size() << std::endl;
        
        // Get allocations by type
        std::vector<MemoryAllocationInfo> neuronAllocations = memoryManager.getAllocationsByType("neuron");
        std::cout << "  Neuron allocations: " << neuronAllocations.size() << std::endl;
        
        // Get allocations by file
        std::vector<MemoryAllocationInfo> fileAllocations = memoryManager.getAllocationsByFile(__FILE__);
        std::cout << "  File allocations: " << fileAllocations.size() << std::endl;
        
        // Analyze memory allocations by type
        std::map<std::string, size_t> leaksByType = memoryManager.analyzeLeaksByType();
        std::cout << "  Leaks by type:" << std::endl;
        for (const auto& pair : leaksByType) {
            std::cout << "    " << pair.first << ": " << pair.second << std::endl;
        }
        
        // Analyze memory allocations by file
        std::map<std::string, size_t> leaksByFile = memoryManager.analyzeLeaksByFile();
        std::cout << "  Leaks by file:" << std::endl;
        for (const auto& pair : leaksByFile) {
            std::cout << "    " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    void monitorMemoryUsage() {
        std::cout << "\n5. Monitoring memory usage..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Get system memory statistics
        MemoryManagementAPI::SystemMemoryStats stats = memoryManager.getSystemMemoryStats();
        
        std::cout << "  System memory statistics:" << std::endl;
        std::cout << "    Total allocated bytes: " << stats.totalAllocatedBytes << std::endl;
        std::cout << "    Peak allocated bytes: " << stats.peakAllocatedBytes << std::endl;
        std::cout << "    Total allocation count: " << stats.totalAllocationCount << std::endl;
        std::cout << "    Total deallocation count: " << stats.totalDeallocationCount << std::endl;
        std::cout << "    Memory utilization ratio: " << (stats.memoryUtilizationRatio * 100) << "%" << std::endl;
        std::cout << "    Fragmentation level: " << stats.fragmentationLevel << std::endl;
        
        // Get pool statistics
        std::cout << "  Pool statistics:" << std::endl;
        for (const auto& poolStats : stats.poolStats) {
            std::cout << "    " << poolStats.first << ": " << poolStats.second.totalAllocatedBytes << " bytes, "
                      << poolStats.second.currentAllocations << " allocations, "
                      << poolStats.second.fragmentationRatio << "% fragmentation" << std::endl;
        }
        
        // Check memory thresholds
        if (stats.totalAllocatedBytes > 1024 * 1024 * 1024) {  // 1GB
            std::cout << "  Warning: Memory usage exceeded high threshold!" << std::endl;
        } else if (stats.totalAllocatedBytes < 64 * 1024 * 1024) {  // 64MB
            std::cout << "  Warning: Memory usage below low threshold!" << std::endl;
        }
        
        // Check memory fragmentation
        if (stats.fragmentationLevel > 20) {  // 20%
            std::cout << "  Warning: Memory fragmentation too high!" << std::endl;
        }
        
        std::cout << "  Memory usage monitoring completed!" << std::endl;
    }
    
    void detectMemoryLeaks() {
        std::cout << "\n6. Detecting memory leaks..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Detect memory leaks
        MemoryLeakReport leakReport = memoryManager.detectMemoryLeaks();
        
        if (leakReport.hasLeaks()) {
            std::cout << "  Memory leaks detected!" << std::endl;
            std::cout << "    Total leaked bytes: " << leakReport.totalLeakedBytes << std::endl;
            std::cout << "    Overall severity: " << leakReport.getSeverityString() << std::endl;
            
            // Get leaks by type
            std::cout << "    Leaks by type:" << std::endl;
            for (const auto& pair : leakReport.leaksByType) {
                std::cout << "      " << pair.first << ": " << pair.second << std::endl;
            }
            
            // Get leaks by file
            std::cout << "    Leaks by file:" << std::endl;
            for (const auto& pair : leakReport.leaksByFile) {
                std::cout << "      " << pair.first << ": " << pair.second << std::endl;
            }
            
            // Get affected contexts
            std::cout << "    Affected contexts:" << std::endl;
            for (const auto& context : leakReport.affectedContexts) {
                std::cout << "      " << context << std::endl;
            }
        } else {
            std::cout << "  No memory leaks detected!" << std::endl;
        }
    }
    
    void optimizeMemoryUsage() {
        std::cout << "\n7. Optimizing memory usage..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Analyze memory optimizations
        std::vector<MemoryOptimization> optimizations = memoryManager.analyzeMemoryOptimizations();
        
        std::cout << "  Memory optimization analysis:" << std::endl;
        std::cout << "    Total optimizations identified: " << optimizations.size() << std::endl;
        
        for (size_t i = 0; i < optimizations.size(); ++i) {
            const auto& optimization = optimizations[i];
            std::cout << "    " << (i + 1) << ". " << optimization.description << std::endl;
            std::cout << "       Type: " << memoryOptimizationTypeToString(optimization.type) << ", "
                      << "Savings: " << optimization.estimatedMemorySavings << " bytes, "
                      << "Priority: " << optimization.getPriorityLabel() << std::endl;
        }
        
        // Apply memory optimizations
        if (memoryManager.applyAllMemoryOptimizations()) {
            std::cout << "  All memory optimizations applied successfully!" << std::endl;
        } else {
            std::cout << "  Failed to apply all memory optimizations!" << std::endl;
        }
        
        std::cout << "  Memory optimization completed!" << std::endl;
    }
    
    void runMemoryMonitoring() {
        std::cout << "\n8. Running memory monitoring..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Run additional simulation to monitor memory usage
        std::cout << "  Running simulation with memory monitoring..." << std::endl;
        
        for (int step = 0; step < 5; ++step) {
            // Simulate memory allocation
            void* ptr = memoryManager.trackAllocation(
                1024,           // size
                "simulation",  // type
                __FILE__,       // file
                __LINE__,       // line
                "simulation_step"  // context
            );
            
            // Simulate memory usage
            if (ptr) {
                // Simulate memory work
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                // Deallocate memory
                memoryManager.trackDeallocation(ptr, "simulation", __FILE__, __LINE__, "simulation_step");
            }
            
            if (step % 2 == 0) {
                std::cout << "  Completed step " << step << std::endl;
            }
        }
        
        // Get updated memory statistics
        MemoryManagementAPI::SystemMemoryStats stats = memoryManager.getSystemMemoryStats();
        std::cout << "  Final memory statistics:" << std::endl;
        std::cout << "    Total allocated bytes: " << stats.totalAllocatedBytes << std::endl;
        std::cout << "    Total allocation count: " << stats.totalAllocationCount << std::endl;
        std::cout << "    Total deallocation count: " << stats.totalDeallocationCount << std::endl;
        
        std::cout << "  Memory monitoring completed!" << std::endl;
    }
    
    void integrateWithNLM() {
        std::cout << "\n9. Integrating with NLM..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Integrate with existing memory pools
        if (memoryManager.integrateWithExistingMemoryPool("neuron_pool")) {
            std::cout << "  Integrated with neuron pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to integrate with neuron pool!" << std::endl;
        }
        
        if (memoryManager.integrateWithExistingMemoryPool("synapse_pool")) {
            std::cout << "  Integrated with synapse pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to integrate with synapse pool!" << std::endl;
        }
        
        // Optimize existing memory pools
        if (memoryManager.optimizeExistingMemoryPool("neuron_pool")) {
            std::cout << "  Optimized neuron pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to optimize neuron pool!" << std::endl;
        }
        
        if (memoryManager.optimizeExistingMemoryPool("synapse_pool")) {
            std::cout << "  Optimized synapse pool successfully!" << std::endl;
        } else {
            std::cout << "  Failed to optimize synapse pool!" << std::endl;
        }
        
        // Get final system memory statistics
        MemoryManagementAPI::SystemMemoryStats stats = memoryManager.getSystemMemoryStats();
        std::cout << "  Integrated system memory statistics:" << std::endl;
        std::cout << "    Total allocated bytes: " << stats.totalAllocatedBytes << std::endl;
        std::cout << "    Peak allocated bytes: " << stats.peakAllocatedBytes << std::endl;
        std::cout << "    Memory utilization ratio: " << (stats.memoryUtilizationRatio * 100) << "%" << std::endl;
        
        std::cout << "  Integration with NLM completed!" << std::endl;
    }
    
    void cleanupMemoryManagement() {
        std::cout << "\n10. Cleaning up memory management..." << std::endl;
        
        MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
        
        // Stop memory monitoring
        memoryManager.stopMemoryMonitoring();
        std::cout << "  Memory monitoring stopped successfully!" << std::endl;
        
        // Note: In real implementation, this would clean up all memory pools
        // For demonstration, we'll just log the action
        std::cout << "  Memory management cleanup completed!" << std::endl;
    }
    
    std::string memoryOptimizationTypeToString(MemoryOptimizationType type) const {
        switch (type) {
            case MemoryOptimizationType::PoolSizing: return "PoolSizing";
            case MemoryOptimizationType::Alignment: return "Alignment";
            case MemoryOptimizationType::FragmentationReduction: return "FragmentationReduction";
            case MemoryOptimizationType::AllocationBatching: return "AllocationBatching";
            case MemoryOptimizationType::Reuse: return "Reuse";
            case MemoryOptimizationType::Compression: return "Compression";
            default: return "Unknown";
        }
    }
};

int main() {
    std::cout << "=== NLM Memory Management Example ===" << std::endl;
    
    // Create example instance
    MemoryManagementExample example;
    
    // Run example
    example.runExample();
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    return 0;
}

// NLM Performance Optimizations Example
// This file demonstrates the use of performance optimizations in the NLM framework

#include "PerformanceProfiler.hpp"
#include "MemoryPool.hpp"
#include "AdvancedMemoryManager.hpp"
#include "LargeScaleOptimizations.hpp"
#include "ResourceManagement.hpp"
#include "Brain.hpp"
#include <iostream>
#include <chrono>

void demonstrateMemoryManagement() {
    std::cout << "=== Memory Management Optimizations ===" << std::endl;
    
    // Create memory pools
    nlm::EnhancedMemoryPool<float> floatPool(1024, 4);
    nlm::EnhancedMemoryPool<int> intPool(512, 2);
    
    // Allocate and deallocate resources
    float* data1 = floatPool.allocate(__FILE__, __LINE__, 1);
    int* data2 = intPool.allocate(__FILE__, __LINE__, 2);
    
    if (data1) *data1 = 3.14f;
    if (data2) *data2 = 42;
    
    // Deallocate resources
    floatPool.deallocate(data1, __FILE__, __LINE__, 1);
    intPool.deallocate(data2, __FILE__, __LINE__, 2);
    
    // Report memory usage
    std::cout << "Float pool stats:" << std::endl;
    std::cout << "  - Allocations: " << floatPool.allocatedCount() << std::endl;
    std::cout << "  - Available: " << floatPool.availableCount() << std::endl;
    std::cout << "  - Memory usage: " << floatPool.memoryUsage() << " bytes" << std::endl;
    
    std::cout << "Int pool stats:" << std::endl;
    std::cout << "  - Allocations: " << intPool.allocatedCount() << std::endl;
    std::cout << "  - Available: " << intPool.availableCount() << std::endl;
    std::cout << "  - Memory usage: " << intPool.memoryUsage() << " bytes" << std::endl;
    
    std::cout << "Memory leak detection report:" << std::endl;
    floatPool.reportLeaks();
    intPool.reportLeaks();
}

void demonstratePerformanceMonitoring() {
    std::cout << "=== Performance Monitoring ===" << std::endl;
    
    // Start performance profiling
    nlm::PerformanceProfiler::startFrame();
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // End frame and report
    nlm::PerformanceProfiler::endFrame();
    
    // Record specific operations
    nlm::PerformanceProfiler::recordSpikeProcessing(1000);
    nlm::PerformanceProfiler::recordNeuronStepping(5000);
    nlm::PerformanceProfiler::recordMemoryAllocation(500);
    nlm::PerformanceProfiler::recordPlasticityUpdate(2000);
    
    // Print detailed report
    nlm::PerformanceProfiler::printDetailedReport();
}

void demonstrateAdvancedMemoryManagement() {
    std::cout << "=== Advanced Memory Management ===" << std::endl;
    
    nlm::AdvancedMemoryManager memoryManager;
    
    // Track some operations
    memoryManager.trackAllocation();
    memoryManager.trackAllocation();
    memoryManager.trackDeallocation();
    
    // Track cache operations
    memoryManager.trackCacheHit();
    memoryManager.trackCacheHit();
    memoryManager.trackCacheMiss();
    
    // Report statistics
    memoryManager.reportStats();
    
    // Simulate cleanup if needed
    if (memoryManager.needsCleanup()) {
        memoryManager.performCleanup();
    }
}

void demonstrateLargeScaleOptimizations() {
    std::cout << "=== Large-Scale Optimizations ===" << std::endl;
    
    // Create sparse connectivity
    nlm::SparseConnectivity connectivity(1000000); // 1M neurons
    
    // Add some connections (simulated)
    for (size_t i = 0; i < 1000; ++i) {
        for (size_t j = 0; j < 10; ++j) {
            size_t target = (i + j + 1) % 1000000;
            connectivity.addConnection(i, target, 0.5f, 1);
        }
    }
    
    // Calculate memory usage
    size_t memoryUsage = connectivity.getMemoryUsage();
    std::cout << "Memory usage for sparse connectivity: " << memoryUsage << " bytes" << std::endl;
    
    // Create parallel processor
    nlm::ParallelProcessor processor(8);
    
    // Create multi-core optimizer
    nlm::MultiCoreOptimizer optimizer(8);
    optimizer.optimize(processor, connectivity);
    
    // Create memory-mapped file
    nlm::MemoryMappedFile mmapFile("brain_checkpoint.dat");
}

void demonstrateResourceManagement() {
    std::cout << "=== Resource Management ===" << std::endl;
    
    // Create resource pool
    nlm::ResourcePool resourcePool(1024, 4096);
    
    // Allocate resources
    void* res1 = resourcePool.allocate();
    void* res2 = resourcePool.allocate();
    void* res3 = resourcePool.allocate();
    
    // Use resources (simulate)
    if (res1) std::cout << "Resource 1 allocated" << std::endl;
    if (res2) std::cout << "Resource 2 allocated" << std::endl;
    if (res3) std::cout << "Resource 3 allocated" << std::endl;
    
    // Deallocate resources
    resourcePool.deallocate(res1);
    resourcePool.deallocate(res2);
    resourcePool.deallocate(res3);
    
    // Report pool statistics
    resourcePool.reportStats();
    
    // Create resource monitor
    nlm::ResourceMonitor resourceMonitor;
    resourceMonitor.start(1000); // 1 second interval
    
    // Update monitor stats
    resourceMonitor.updateStats();
    resourceMonitor.reportStats();
    
    resourceMonitor.stop();
}

void demonstrateGarbageCollection() {
    std::cout << "=== Garbage Collection ===" << std::endl;
    
    // Create garbage collector
    nlm::GarbageCollectionOptimizer gc(nlm::GarbageCollectionOptimizer::CollectionStrategy::GENERATIONAL);
    
    // Check if collection is needed
    if (gc.needsCollection()) {
        size_t collected = gc.collect();
        std::cout << "Collected " << collected << " objects" << std::endl;
    }
    
    // Report statistics
    gc.reportStats();
}

void demonstrateResourceCleanup() {
    std::cout << "=== Resource Cleanup ===" << std::endl;
    
    // Create resource cleanup manager
    nlm::ResourceCleanupManager cleanup(nlm::ResourceCleanupManager::CleanupMode::NORMAL);
    
    // Check if cleanup is needed
    if (cleanup.needsCleanup()) {
        cleanup.cleanup();
    }
    
    // Report statistics
    cleanup.reportStats();
}

int main() {
    std::cout << "NLM Performance Optimizations Demo" << std::endl;
    std::cout << "=================================" << std::endl;
    
    // Demonstrate all optimizations
    demonstrateMemoryManagement();
    std::cout << std::endl;
    
    demonstratePerformanceMonitoring();
    std::cout << std::endl;
    
    demonstrateAdvancedMemoryManagement();
    std::cout << std::endl;
    
    demonstrateLargeScaleOptimizations();
    std::cout << std::endl;
    
    demonstrateResourceManagement();
    std::cout << std::endl;
    
    demonstrateGarbageCollection();
    std::cout << std::endl;
    
    demonstrateResourceCleanup();
    
    std::cout << std::endl;
    std::cout << "All demonstrations completed successfully!" << std::endl;
    
    return 0;
}

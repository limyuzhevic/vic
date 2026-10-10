#include "test_common.hpp"
#include <cassert>
#include <cmath>
#include <algorithm>

namespace test_performance {

void testMemoryPoolCreation() {
    nlm::MemoryPool memoryPool;
    assert(memoryPool.getTotalCapacity() == 1024 * 1024); // 1MB default
    assert(memoryPool.getUsedMemory() == 0);
    assert(memoryPool.isOptimized() == false);
    std::cout << "    testMemoryPoolCreation passed" << std::endl;
}

void testEventQueueCreation() {
    nlm::EventQueue eventQueue;
    assert(eventQueue.getQueueSize() == 1000);
    assert(eventQueue.getEventCount() == 0);
    assert(eventQueue.isProcessing() == false);
    std::cout << "    testEventQueueCreation passed" << std::endl;
}

void testParallelProcessingCreation() {
    nlm::ParallelProcessing parallelProc;
    assert(parallelProc.getThreadCount() == 4); // Default
    assert(parallelProc.isProcessing() == false);
    std::cout << "    testParallelProcessingCreation passed" << std::endl;
}

void testMemoryPoolAllocation() {
    nlm::MemoryPool memoryPool;
    
    // Allocate memory
    void* ptr1 = memoryPool.allocate(1024);
    void* ptr2 = memoryPool.allocate(2048);
    
    assert(ptr1 != nullptr);
    assert(ptr2 != nullptr);
    assert(memoryPool.getUsedMemory() == (1024 + 2048));
    
    // Free memory
    memoryPool.free(ptr1);
    memoryPool.free(ptr2);
    assert(memoryPool.getUsedMemory() == 0);
    std::cout << "    testMemoryPoolAllocation passed" << std::endl;
}

void testEventQueue() {
    nlm::EventQueue eventQueue;
    
    // Add events
    eventQueue.enqueue("event1", 1.0f);
    eventQueue.enqueue("event2", 2.0f);
    
    assert(eventQueue.getEventCount() == 2);
    
    // Process events
    eventQueue.processEvents();
    assert(eventQueue.getEventCount() == 0);
    std::cout << "    testEventQueue passed" << std::endl;
}

void testParallelProcessing() {
    nlm::ParallelProcessing parallelProc;
    
    // Set thread count
    parallelProc.setThreadCount(8);
    assert(parallelProc.getThreadCount() == 8);
    
    // Start processing
    parallelProc.startProcessing();
    assert(parallelProc.isProcessing() == true);
    
    // Stop processing
    parallelProc.stopProcessing();
    assert(parallelProc.isProcessing() == false);
    std::cout << "    testParallelProcessing passed" << std::endl;
}

void testPerformanceMonitoring() {
    nlm::MemoryPool memoryPool;
    nlm::EventQueue eventQueue;
    nlm::ParallelProcessing parallelProc;
    
    // Monitor performance
    float memoryUsage = memoryPool.getMemoryUsage();
    float queueUsage = eventQueue.getQueueUsage();
    float cpuUsage = parallelProc.getCPUUsage();
    
    assert(memoryUsage >= 0.0f);
    assert(queueUsage >= 0.0f && queueUsage <= 1.0f);
    assert(cpuUsage >= 0.0f && cpuUsage <= 100.0f);
    std::cout << "    testPerformanceMonitoring passed" << std::endl;
}

void runAll() {
    std::cout << "Running Performance tests..." << std::endl;
    testMemoryPoolCreation();
    testEventQueueCreation();
    testParallelProcessingCreation();
    testMemoryPoolAllocation();
    testEventQueue();
    testParallelProcessing();
    testPerformanceMonitoring();
}

} // namespace test_performance
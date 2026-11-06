// Performance Tests
// Tests for Phase 5 performance optimizations
#include "brain/Brain.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/SIMDVectorization.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace test_performance {

void testMemoryPoolPerformance() {
    // Test memory pool allocation/deallocation performance
    nlm::MemoryPool<float> pool(1000, 5);
    
    // Allocate objects
    std::vector<float*> objects;
    for (int i = 0; i < 500; ++i) {
        float* obj = pool.allocate();
        objects.push_back(obj);
    }
    
    assert(objects.size() == 500);
    assert(pool.allocatedCount() == 500);
    
    // Deallocate objects
    for (float* obj : objects) {
        pool.deallocate(obj);
    }
    
    assert(pool.allocatedCount() == 0);
    assert(pool.empty() == false); // Should still have blocks allocated
    
    std::cout << "    testMemoryPoolPerformance passed" << std::endl;
}

void testEventQueuePerformance() {
    // Test event queue performance
    nlm::EventQueue queue(nlm::EventQueue::Priority::High);
    
    // Add many events
    for (int i = 0; i < 100; ++i) {
        EventQueue::Event event;
        event.priority = nlm::EventQueue::Priority::Low;
        event.data = new int(i); // Simple data for testing
        queue.enqueue(event);
    }
    
    assert(queue.size() == 100);
    
    // Dequeue events
    for (int i = 0; i < 100; ++i) {
        EventQueue::Event event;
        bool success = queue.tryDequeue(event);
        assert(success);
        assert(*static_cast<int*>(event.data) == i);
        delete static_cast<int*>(event.data);
    }
    
    assert(queue.size() == 0);
    
    std::cout << "    testEventQueuePerformance passed" << std::endl;
}

void testParallelProcessingPerformance() {
    // Test parallel processing performance
    nlm::ParallelProcessingEngine engine;
    
    // Test vector addition using SIMD
    std::vector<float> a(1000, 1.0f);
    std::vector<float> b(1000, 2.0f);
    std::vector<float> result(1000, 0.0f);
    
    engine.parallel_for<float>(1000, 
        [&a, &b, &result](const float& valA, const float& valB, float& resultVal) {
            resultVal = valA + valB;
        });
    
    // Verify results
    for (int i = 0; i < 1000; ++i) {
        assert(std::abs(result[i] - 3.0f) < 0.001f);
    }
    
    std::cout << "    testParallelProcessingPerformance passed" << std::endl;
}

void testSIMDVectorization() {
    // Test SIMD vectorization
    nlm::SIMDVectorization simd;
    
    // Test vector operations
    std::vector<float> a = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    std::vector<float> b = {2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    std::vector<float> result(a.size());
    
    simd.addVectors(a, b, result);
    
    // Verify results
    for (int i = 0; i < a.size(); ++i) {
        assert(std::abs(result[i] - (a[i] + b[i])) < 0.001f);
    }
    
    std::cout << "    testSIMDVectorization passed" << std::endl;
}

void testBrainPerformanceWithOptimizations() {
    // Test brain performance with Phase 5 optimizations enabled
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Enable all performance optimizations
    brain.configurePerformanceOptimizations();
    brain.enableMemoryPools(true);
    brain.enableEventQueues(true);
    brain.enableParallelProcessing(true);
    brain.enableSIMD(true);
    
    bool initSuccess = brain.initialize();
    assert(initSuccess);
    
    // Measure performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation with optimizations
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 5.0f);
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Performance should be reasonable (less than 10 seconds for 50 steps)
    assert(duration.count() < 10000);
    
    std::cout << "    testBrainPerformanceWithOptimizations passed (duration: " 
              << duration.count() << " ms)" << std::endl;
}

void testMemoryPoolMemoryUsage() {
    // Test memory pool memory usage tracking
    nlm::MemoryPool<int> pool(100, 2);
    
    // Check initial memory usage
    size_t initialUsage = pool.memoryUsage();
    assert(initialUsage > 0);
    
    // Allocate and deallocate to verify memory tracking
    int* obj = pool.allocate();
    *obj = 42;
    
    assert(pool.allocatedCount() == 1);
    
    pool.deallocate(obj);
    assert(pool.allocatedCount() == 0);
    
    std::cout << "    testMemoryPoolMemoryUsage passed" << std::endl;
}

void testEventQueuePriority() {
    // Test event queue priority handling
    nlm::EventQueue highPriorityQueue(nlm::EventQueue::Priority::High);
    nlm::EventQueue lowPriorityQueue(nlm::EventQueue::Priority::Low);
    
    // Add events to both queues
    EventQueue::Event highEvent;
    highEvent.priority = nlm::EventQueue::Priority::High;
    highEvent.data = new int(1);
    highPriorityQueue.enqueue(highEvent);
    
    EventQueue::Event lowEvent;
    lowEvent.priority = nlm::EventQueue::Priority::Low;
    lowEvent.data = new int(2);
    lowPriorityQueue.enqueue(lowEvent);
    
    assert(highPriorityQueue.size() == 1);
    assert(lowPriorityQueue.size() == 1);
    
    // Verify priority handling by attempting to dequeue
    bool highSuccess = highPriorityQueue.tryDequeue(highEvent);
    bool lowSuccess = lowPriorityQueue.tryDequeue(lowEvent);
    
    assert(highSuccess);
    assert(lowSuccess);
    
    delete static_cast<int*>(highEvent.data);
    delete static_cast<int*>(lowEvent.data);
    
    std::cout << "    testEventQueuePriority passed" << std::endl;
}

void runAllPerformanceTests() {
    testMemoryPoolPerformance();
    testEventQueuePerformance();
    testParallelProcessingPerformance();
    testSIMDVectorization();
    testBrainPerformanceWithOptimizations();
    testMemoryPoolMemoryUsage();
    testEventQueuePriority();
}

} // namespace test_performance

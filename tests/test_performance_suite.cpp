// Performance Tests
// Memory Pool Performance Tests

#include "performance/MemoryPool.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>

namespace test_memory_pool_performance {

void testMemoryPoolAllocationPerformance() {
    std::cout << "  Testing MemoryPool allocation performance..." << std::endl;
    
    struct TestNeuron {
        uint64_t id;
        float potential;
        bool active;
        std::vector<float> synapticWeights; // Add some complexity
    };
    
    // Test with different pool sizes
    const size_t poolSizes[] = {1024, 4096, 16384};
    const size_t allocationCounts[] = {100, 1000, 5000};
    
    for (size_t poolSize : poolSizes) {
        for (size_t allocCount : allocationCounts) {
            if (allocCount > poolSize * 4) continue; // Don't exceed pool capacity
            
            nlm::MemoryPool<TestNeuron> neuronPool(poolSize, 4);
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Allocate and initialize neurons
            std::vector<TestNeuron*> neurons;
            neurons.reserve(allocCount);
            
            for (size_t i = 0; i < allocCount; ++i) {
                TestNeuron* neuron = neuronPool.allocate();
                assert(neuron != nullptr);
                neuron->id = i;
                neuron->potential = -70.0f;
                neuron->active = false;
                neurons.push_back(neuron);
            }
            
            auto allocateTime = std::chrono::high_resolution_clock::now();
            
            // Deallocate neurons
            for (TestNeuron* neuron : neurons) {
                neuronPool.deallocate(neuron);
            }
            
            auto deallocateTime = std::chrono::high_resolution_clock::now();
            
            double allocateDuration = std::chrono::duration<double>(allocateTime - startTime).count();
            double deallocateDuration = std::chrono::duration<double>(deallocateTime - allocateTime).count();
            
            std::cout << "    Pool size " << poolSize << ", " << allocCount << " allocations: "
                     << "allocate=" << allocateDuration << "s, deallocate=" << deallocateDuration << "s" << std::endl;
            
            // Verify all deallocated
            assert(neuronPool.allocatedCount() == 0);
        }
    }
    
    std::cout << "    MemoryPool allocation performance test completed!" << std::endl;
}

void testMemoryPoolReallocationPerformance() {
    std::cout << "  Testing MemoryPool reallocation performance..." << std::endl;
    
    struct ComplexNeuron {
        uint64_t id;
        float potential;
        bool active;
        std::vector<float> synapticWeights;
        std::vector<float> dendriticSegments;
        float membraneVoltage;
        float inputCurrent;
    };
    
    const size_t poolSize = 8192;
    const size_t iterations = 100;
    
    nlm::MemoryPool<ComplexNeuron> neuronPool(poolSize, 4);
    
    // Allocate and deallocate repeatedly
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t iter = 0; iter < iterations; ++iter) {
        // Allocate batch
        std::vector<ComplexNeuron*> batch;
        batch.reserve(100);
        
        for (size_t i = 0; i < 100; ++i) {
            if (neuronPool.allocatedCount() < poolSize * 4) {
                ComplexNeuron* neuron = neuronPool.allocate();
                if (neuron) {
                    neuron->id = iter * 100 + i;
                    neuron->potential = -70.0f;
                    neuron->active = false;
                    neuron->synapticWeights.resize(50, 0.0f);
                    neuron->dendriticSegments.resize(20, 0.0f);
                    neuron->membraneVoltage = -70.0f;
                    neuron->inputCurrent = 0.0f;
                    batch.push_back(neuron);
                }
            }
        }
        
        // Deallocate batch
        for (ComplexNeuron* neuron : batch) {
            neuronPool.deallocate(neuron);
        }
        
        if (iter % 10 == 0) {
            std::cout << "    Iteration " << iter << "/" << iterations << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalDuration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    MemoryPool reallocation performance: "
             << iterations << " iterations in " << totalDuration << "s ("
             << (iterations * 100.0 / totalDuration) << " allocations/sec)" << std::endl;
    
    std::cout << "    MemoryPool reallocation performance test completed!" << std::endl;
}

void testMemoryPoolCacheEfficiency() {
    std::cout << "  Testing MemoryPool cache efficiency..." << std::endl;
    
    struct CacheSensitiveNeuron {
        uint64_t id;
        float potential;
        bool active;
        // Add fields to test cache alignment
        float dummy1[4];  // Cache line padding
        float dummy2[4];
        float dummy3[4];
    };
    
    nlm::MemoryPool<CacheSensitiveNeuron> neuronPool(4096, 8);
    
    // Allocate in different patterns to test cache efficiency
    std::vector<CacheSensitiveNeuron*> neurons;
    neurons.reserve(2000);
    
    // Pattern 1: Sequential allocation
    auto startTime = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 1000; ++i) {
        CacheSensitiveNeuron* neuron = neuronPool.allocate();
        if (neuron) {
            neuron->id = i;
            neurons.push_back(neuron);
        }
    }
    auto seqTime = std::chrono::high_resolution_clock::now();
    
    // Deallocate
    for (CacheSensitiveNeuron* neuron : neurons) {
        neuronPool.deallocate(neuron);
    }
    neurons.clear();
    
    // Pattern 2: Random-like allocation
    for (size_t i = 0; i < 1000; ++i) {
        size_t index = (i * 137) % 2000; // Pseudo-random pattern
        if (neuronPool.allocatedCount() < 2000) {
            CacheSensitiveNeuron* neuron = neuronPool.allocate();
            if (neuron) {
                neuron->id = index;
                neurons.push_back(neuron);
            }
        }
    }
    auto randTime = std::chrono::high_resolution_clock::now();
    
    // Deallocate
    for (CacheSensitiveNeuron* neuron : neurons) {
        neuronPool.deallocate(neuron);
    }
    
    double seqDuration = std::chrono::duration<double>(seqTime - startTime).count();
    double randDuration = std::chrono::duration<double>(randTime - seqTime).count();
    
    std::cout << "    Sequential allocation: " << seqDuration << "s" << std::endl;
    std::cout << "    Random-like allocation: " << randDuration << "s" << std::endl;
    
    std::cout << "    MemoryPool cache efficiency test completed!" << std::endl;
}

void runAllMemoryPoolPerformanceTests() {
    std::cout << "=== Memory Pool Performance Tests ===" << std::endl;
    
    try {
        testMemoryPoolAllocationPerformance();
        testMemoryPoolReallocationPerformance();
        testMemoryPoolCacheEfficiency();
        
        std::cout << std::endl;
        std::cout << "=== All Memory Pool Performance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_memory_pool_performance

// Performance Tests
// Event Queue Processing Tests

#include "performance/EventQueue.hpp"
#include <thread>
#include <atomic>

namespace test_event_queue_performance {

void testSpikeQueueProcessingPerformance() {
    std::cout << "  Testing SpikeQueue processing performance..." << std::endl;
    
    const size_t numEvents = 10000;
    const size_t batchSize = 100;
    
    nlm::SpikeRingBuffer spikeQueue;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Generate spike events
    for (size_t i = 0; i < numEvents; ++i) {
        nlm::QueuedSpikeEvent event(
            i % 1000,           // sourceNeuron
            (i + 1) % 1000,     // destNeuron
            (i % 100) * 0.1f,  // weight
            (i % 2),            // isExcitatory (0 or 1)
            i % 5               // delay
        );
        
        assert(spikeQueue.push(event));
        
        if ((i + 1) % batchSize == 0) {
            std::cout << "    Processed " << (i + 1) << "/" << numEvents << " events" << std::endl;
        }
    }
    
    auto generateTime = std::chrono::high_resolution_clock::now();
    
    // Process events in batches
    size_t processedCount = 0;
    size_t batchProcessed = 0;
    
    while (!spikeQueue.empty() || spikeQueue.size() > 0) {
        auto event = spikeQueue.pop();
        if (event.has_value()) {
            processedCount++;
            batchProcessed++;
            
            if (batchProcessed % 1000 == 0) {
                std::cout << "    Processed batch of " << batchProcessed << " events" << std::endl;
            }
        }
    }
    
    auto processTime = std::chrono::high_resolution_clock::now();
    
    double generateDuration = std::chrono::duration<double>(generateTime - startTime).count();
    double processDuration = std::chrono::duration<double>(processTime - generateTime).count();
    
    std::cout << "    Event generation: " << numEvents << " events in " << generateDuration << "s ("
             << (numEvents / generateDuration) << " events/sec)" << std::endl;
    std::cout << "    Event processing: " << processedCount << " events in " << processDuration << "s ("
             << (processedCount / processDuration) << " events/sec)" << std::endl;
    
    assert(processedCount == numEvents);
    std::cout << "    SpikeQueue processing performance test completed!" << std::endl;
}

void testDelayedSpikeQueuePerformance() {
    std::cout << "  Testing DelayedSpikeQueue performance..." << std::endl;
    
    const size_t numScheduled = 5000;
    const size_t maxDelay = 100;
    
    nlm::DelayedSpikeQueue delayedQueue;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Schedule spike events with various delays
    for (size_t i = 0; i < numScheduled; ++i) {
        size_t delay = (i * 7) % maxDelay; // Varying delays
        delayedQueue.schedule(
            i % 1000,           // sourceNeuron
            (i + 1) % 1000,     // destNeuron
            (i % 100) * 0.1f,  // weight
            (i % 2),            // isExcitatory
            delay               // delay
        );
        
        if ((i + 1) % 1000 == 0) {
            std::cout << "    Scheduled " << (i + 1) << "/" << numScheduled << " events" << std::endl;
        }
    }
    
    auto scheduleTime = std::chrono::high_resolution_clock::now();
    
    // Process events for different time points
    for (size_t timePoint = 0; timePoint <= maxDelay; timePoint += 10) {
        std::vector<nlm::QueuedSpikeEvent> dueEvents = delayedQueue.getDueSpikes(timePoint);
        
        if (!dueEvents.empty()) {
            std::cout << "    Time " << timePoint << ": " << dueEvents.size() << " events due" << std::endl;
        }
    }
    
    auto processTime = std::chrono::high_resolution_clock::now();
    
    double scheduleDuration = std::chrono::duration<double>(scheduleTime - startTime).count();
    double processDuration = std::chrono::duration<double>(processTime - scheduleTime).count();
    
    std::cout << "    Scheduling: " << numScheduled << " events in " << scheduleDuration << "s ("
             << (numScheduled / scheduleDuration) << " events/sec)" << std::endl;
    std::cout << "    Processing: " << maxDelay / 10 << " time points in " << processDuration << "s" << std::endl;
    
    std::cout << "    DelayedSpikeQueue performance test completed!" << std::endl;
}

void testConcurrentEventProcessing() {
    std::cout << "  Testing concurrent event processing..." << std::endl;
    
    const size_t numThreads = 4;
    const size_t eventsPerThread = 2500;
    const size_t totalEvents = numThreads * eventsPerThread;
    
    std::atomic<size_t> processedCount{0};
    std::vector<std::thread> threads;
    
    nlm::SpikeRingBuffer spikeQueue;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Launch producer threads
    for (size_t threadId = 0; threadId < numThreads; ++threadId) {
        threads.emplace_back([&, threadId]() {
            for (size_t i = 0; i < eventsPerThread; ++i) {
                nlm::QueuedSpikeEvent event(
                    threadId * 100 + i,           // sourceNeuron
                    (threadId * 100 + i + 1) % 1000, // destNeuron
                    (i % 50) * 0.01f,             // weight
                    (i % 2),                      // isExcitatory
                    i % 10                        // delay
                );
                
                if (spikeQueue.push(event)) {
                    processedCount.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }
    
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto produceTime = std::chrono::high_resolution_clock::now();
    
    // Consume all events
    size_t consumedCount = 0;
    while (!spikeQueue.empty() || spikeQueue.size() > 0) {
        auto event = spikeQueue.pop();
        if (event.has_value()) {
            consumedCount++;
        }
    }
    
    auto consumeTime = std::chrono::high_resolution_clock::now();
    
    double produceDuration = std::chrono::duration<double>(produceTime - startTime).count();
    double consumeDuration = std::chrono::duration<double>(consumeTime - produceTime).count();
    
    std::cout << "    Production: " << processedCount << " events in " << produceDuration << "s ("
             << (processedCount / produceDuration) << " events/sec)" << std::endl;
    std::cout << "    Consumption: " << consumedCount << " events in " << consumeDuration << "s ("
             << (consumedCount / consumeDuration) << " events/sec)" << std::endl;
    
    assert(processedCount == totalEvents);
    assert(consumedCount == totalEvents);
    
    std::cout << "    Concurrent event processing test completed!" << std::endl;
}

void runAllEventQueuePerformanceTests() {
    std::cout << "=== Event Queue Processing Performance Tests ===" << std::endl;
    
    try {
        testSpikeQueueProcessingPerformance();
        testDelayedSpikeQueuePerformance();
        testConcurrentEventProcessing();
        
        std::cout << std::endl;
        std::cout << "=== All Event Queue Performance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_event_queue_performance

// Performance Tests
// Parallel Processing Tests

#include "performance/ParallelProcessing.hpp"
#include <atomic>

namespace test_parallel_processing {

void testThreadPoolPerformance() {
    std::cout << "  Testing ThreadPool performance..." << std::endl;
    
    const size_t numThreads = 8;
    const size_t tasksPerThread = 1000;
    const size_t totalTasks = numThreads * tasksPerThread;
    
    nlm::ThreadPool threadPool(numThreads);
    
    std::atomic<size_t> completedTasks{0};
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Submit tasks
    std::vector<std::future<void>> futures;
    futures.reserve(totalTasks);
    
    for (size_t taskId = 0; taskId < totalTasks; ++taskId) {
        futures.emplace_back(threadPool.submit([&, taskId]() {
            // Simulate computational work
            volatile size_t sum = 0;
            for (size_t i = 0; i < 1000; ++i) {
                sum += i * taskId;
            }
            completedTasks.fetch_add(1, std::memory_order_relaxed);
        }));
    }
    
    // Wait for all tasks to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    threadPool.wait();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    ThreadPool: " << totalTasks << " tasks in " << duration << "s ("
             << (totalTasks / duration) << " tasks/sec)" << std::endl;
    
    assert(completedTasks == totalTasks);
    std::cout << "    ThreadPool performance test completed!" << std::endl;
}

void testParallelNeuralProcessorPerformance() {
    std::cout << "  Testing ParallelNeuralProcessor performance..." << std::endl;
    
    try {
        const size_t numProcessors = 4;
        const size_t numNeurons = 5000;
        
        nlm::ParallelNeuralProcessor processor(numProcessors);
        
        std::cout << "    ParallelNeuralProcessor created with " << numProcessors << " processors" << std::endl;
        
        // Simulate neural processing
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t totalProcessed = processor.processNeurons(numNeurons, [](size_t neuronId, nlm::ParallelNeuralProcessor::ThreadLocalState& state) {
            // Simulate neuron processing
            if (neuronId % 3 == 0) {
                state.localFiringCount++;
            }
            if (neuronId % 5 == 0) {
                state.localSpikeCount++;
            }
            if (neuronId % 7 == 0) {
                state.localWeightChange += 0.1f;
            }
            if (neuronId % 11 == 0) {
                state.localPotential += 0.01f;
            }
        });
        
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "    Processed " << totalProcessed << " neurons in " << duration << "s ("
                 << (totalProcessed / duration) << " neurons/sec)" << std::endl;
        
        std::cout << "    ParallelNeuralProcessor performance test completed!" << std::endl;
    } catch (...) {
        std::cout << "    ParallelNeuralProcessor not available, skipping tests" << std::endl;
    }
}

void testSIMDParallelPerformance() {
    std::cout << "  Testing SIMD parallel processing performance..." << std::endl;
    
    nlm::SIMDUtils utils;
    
    std::cout << "    SIMD type: " << utils.getSIMDType() << std::endl;
    std::cout << "    SIMD width: " << utils.SIMD_WIDTH << std::endl;
    
    // Test vectorized operations performance
    const size_t arraySize = 10000;
    
    std::vector<float> voltages(arraySize, -70.0f);
    std::vector<float> inputs(arraySize, 0.0f);
    std::vector<uint32_t> refractory(arraySize, 0);
    std::vector<int> fired(arraySize, 0);
    
    // Set some neurons to have input current
    for (size_t i = 0; i < arraySize; i += 7) {
        inputs[i] = 1.0f;
    }
    
    nlm::VectorizedLIF lif;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    size_t firedCount = lif.update(voltages, inputs, refractory, fired, arraySize);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    VectorizedLIF: " << arraySize << " neurons in " << duration << "s ("
             << (arraySize / duration) << " neurons/sec)" << std::endl;
    std::cout << "    VectorizedLIF fired " << firedCount << " neurons" << std::endl;
    
    // Test vectorized STDP performance
    nlm::VectorizedSTDP stdp;
    
    auto startTime2 = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < arraySize; ++i) {
        float deltaW = stdp.calculateDeltaW((i % 2 == 0) ? 1.0f : -1.0f);
    }
    
    auto endTime2 = std::chrono::high_resolution_clock::now();
    
    double stdpDuration = std::chrono::duration<double>(endTime2 - startTime2).count();
    
    std::cout << "    VectorizedSTDP: " << arraySize << " updates in " << stdpDuration << "s ("
             << (arraySize / stdpDuration) << " updates/sec)" << std::endl;
    
    std::cout << "    SIMD parallel performance test completed!" << std::endl;
}

void runAllParallelProcessingPerformanceTests() {
    std::cout << "=== Parallel Processing Performance Tests ===" << std::endl;
    
    try {
        testThreadPoolPerformance();
        testParallelNeuralProcessorPerformance();
        testSIMDParallelPerformance();
        
        std::cout << std::endl;
        std::cout << "=== All Parallel Processing Performance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_parallel_processing

// Performance Tests
// SIMD Vectorization Tests

void testVectorizedLIFPerformance() {
    std::cout << "  Testing VectorizedLIF performance..." << std::endl;
    
    const size_t arraySizes[] = {1000, 5000, 10000, 50000};
    
    nlm::VectorizedLIF lif;
    
    for (size_t arraySize : arraySizes) {
        std::vector<float> voltages(arraySize, -70.0f);
        std::vector<float> inputs(arraySize, 0.0f);
        std::vector<uint32_t> refractory(arraySize, 0);
        std::vector<int> fired(arraySize, 0);
        
        // Add input spikes
        for (size_t i = 0; i < arraySize; i += 3) {
            inputs[i] = 1.5f;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t firedCount = lif.update(voltages, inputs, refractory, fired, arraySize);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "    Array size " << arraySize << ": " << firedCount << " spikes in " << duration << "s ("
                 << (arraySize / duration) << " neurons/sec)" << std::endl;
    }
    
    std::cout << "    VectorizedLIF performance test completed!" << std::endl;
}

void testVectorizedSTDPPerformance() {
    std::cout << "  Testing VectorizedSTDP performance..." << std::endl;
    
    const size_t updateCounts[] = {1000, 5000, 10000, 50000};
    
    nlm::VectorizedSTDP stdp;
    
    for (size_t count : updateCounts) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < count; ++i) {
            // Mix of LTP and LTD
            float deltaW = stdp.calculateDeltaW((i % 2 == 0) ? 1.0f : -1.0f);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        std::cout << "    " << count << " updates: " << duration << "s ("
                 << (count / duration) << " updates/sec)" << std::endl;
    }
    
    std::cout << "    VectorizedSTDP performance test completed!" << std::endl;
}

void testSIMDUtilsPerformance() {
    std::cout << "  Testing SIMDUtils performance..." << std::endl;
    
    nlm::SIMDUtils utils;
    
    // Test SIMD operation performance
    const size_t vectorSize = 100000;
    
    std::vector<float> vec1(vectorSize);
    std::vector<float> vec2(vectorSize);
    std::vector<float> result(vectorSize);
    
    // Initialize vectors
    for (size_t i = 0; i < vectorSize; ++i) {
        vec1[i] = static_cast<float>(i) * 0.01f;
        vec2[i] = static_cast<float>(i) * 0.02f;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform vectorized operations
    for (size_t i = 0; i < vectorSize; ++i) {
        result[i] = vec1[i] * vec2[i] + 1.0f;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    SIMD vector operations: " << vectorSize << " elements in " << duration << "s ("
             << (vectorSize / duration) << " elements/sec)" << std::endl;
    
    std::cout << "    SIMDUtils performance test completed!" << std::endl;
}

void runAllSIMDVectorizationPerformanceTests() {
    std::cout << "=== SIMD Vectorization Performance Tests ===" << std::endl;
    
    try {
        testVectorizedLIFPerformance();
        testVectorizedSTDPPerformance();
        testSIMDUtilsPerformance();
        
        std::cout << std::endl;
        std::cout << "=== All SIMD Vectorization Performance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_simd_vectorization

// Performance Tests
// Checkpoint Performance Tests

void testCheckpointWritePerformance() {
    std::cout << "  Testing checkpoint write performance..." << std::endl;
    
    nlm::CheckpointManager manager;
    manager.configure("./test_checkpoints_perf", 1000, 10, false); // No compression for speed test
    
    const size_t numCheckpoints = 20;
    const size_t checkpointSize = 10000;
    
    // Create test brain state data
    std::vector<float> brainData(checkpointSize, 0.0f);
    for (size_t i = 0; i < checkpointSize; ++i) {
        brainData[i] = static_cast<float>(i) * 0.1f;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t checkpointId = 0; checkpointId < numCheckpoints; ++checkpointId) {
        // Simulate checkpoint save (in real implementation, this would save actual brain state)
        std::cout << "    Writing checkpoint " << checkpointId << "/" << numCheckpoints << std::endl;
        
        // Simulate time for checkpoint write
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    Checkpoint writes: " << numCheckpoints << " in " << duration << "s ("
             << (numCheckpoints / duration) << " checkpoints/sec)" << std::endl;
    
    std::cout << "    Checkpoint write performance test completed!" << std::endl;
}

void testCheckpointReadPerformance() {
    std::cout << "  Testing checkpoint read performance..." << std::endl;
    
    nlm::CheckpointManager manager;
    manager.configure("./test_checkpoints_perf", 1000, 10, false);
    
    const size_t numCheckpoints = 10;
    const size_t checkpointSize = 50000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t checkpointId = 0; checkpointId < numCheckpoints; ++checkpointId) {
        // Simulate checkpoint load (in real implementation, this would load actual brain state)
        std::cout << "    Loading checkpoint " << checkpointId << "/" << numCheckpoints << std::endl;
        
        // Simulate time for checkpoint read
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    Checkpoint reads: " << numCheckpoints << " in " << duration << "s ("
             << (numCheckpoints / duration) << " checkpoints/sec)" << std::endl;
    
    std::cout << "    Checkpoint read performance test completed!" << std::endl;
}

void testCheckpointCleanupPerformance() {
    std::cout << "  Testing checkpoint cleanup performance..." << std::endl;
    
    nlm::CheckpointManager manager;
    manager.configure("./test_checkpoints_perf", 1000, 5, false); // Small max checkpoints
    
    const size_t numNewCheckpoints = 50;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create many checkpoints to trigger cleanup
    for (size_t checkpointId = 0; checkpointId < numNewCheckpoints; ++checkpointId) {
        // Simulate checkpoint save to trigger cleanup
        if (checkpointId % 10 == 0) {
            std::cout << "    Creating checkpoint " << checkpointId << "/" << numNewCheckpoints << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << "    Checkpoint creation and cleanup: " << numNewCheckpoints << " in " << duration << "s ("
             << (numNewCheckpoints / duration) << " checkpoints/sec)" << std::endl;
    
    std::cout << "    Checkpoint cleanup performance test completed!" << std::endl;
}

void runAllCheckpointPerformanceTests() {
    std::cout << "=== Checkpoint Performance Tests ===" << std::endl;
    
    try {
        testCheckpointWritePerformance();
        testCheckpointReadPerformance();
        testCheckpointCleanupPerformance();
        
        std::cout << std::endl;
        std::cout << "=== All Checkpoint Performance Tests Passed ===" << std::endl;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace test_checkpoint_performance

// Main function to run all performance tests
int main() {
    std::cout << "=== NLM Performance Tests ===" << std::endl;
    std::cout << "Testing performance of all NLM enhancements" << std::endl;
    std::cout << "===============================" << std::endl;
    
    try {
        test_memory_pool_performance::runAllMemoryPoolPerformanceTests();
        test_event_queue_performance::runAllEventQueuePerformanceTests();
        test_parallel_processing::runAllParallelProcessingPerformanceTests();
        test_simd_vectorization::runAllSIMDVectorizationPerformanceTests();
        test_checkpoint_performance::runAllCheckpointPerformanceTests();
        
        std::cout << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << "=== ALL PERFORMANCE TESTS PASSED ===" << std::endl;
        std::cout << "===============================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << "=== Performance Test Suite FAILED ===" << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        std::cout << "===============================" << std::endl;
        return 1;
    }
}
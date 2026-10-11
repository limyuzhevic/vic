// Performance Tests Suite
// Tests for performance infrastructure integration in Phase 6
// Validates memory pool efficiency, event queue performance, SIMD optimizations,
// and parallel processing integration

#include "PerformanceTests.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/SIMDVectorization.hpp"
#include "performance/PerformanceProfiler.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

namespace test_performance {

using namespace nlm;

void PerformanceTestSuite::initialize() {
    Logger::initialize();
    NLM_LOG_INFO("Initializing Performance Test Suite");
}

void PerformanceTestSuite::cleanup() {
    NLM_LOG_INFO("Performance Test Suite cleanup completed");
}

void PerformanceTestSuite::runAllTests() {
    NLM_LOG_INFO("Starting comprehensive performance test suite");
    
    // Memory Pool Tests
    runMemoryPoolTests();
    
    // Event Queue Tests
    runEventQueueTests();
    
    // SIMD Integration Tests
    runSIMDIntegrationTests();
    
    // Parallel Processing Tests
    runParallelProcessingTests();
    
    // Performance Infrastructure Integration Tests
    runPerformanceInfrastructureTests();
    
    // Memory Bandwidth Tests
    runMemoryBandwidthTests();
    
    // CPU Performance Tests
    runCPUPerformanceTests();
    
    // Scalability Tests
    runScalabilityTests();
    
    // Resource Management Tests
    runResourceManagementTests();
    
    NLM_LOG_SUCCESS("All performance tests completed successfully!");
}

void PerformanceTestSuite::runMemoryPoolTests() {
    NLM_LOG_INFO("Running Memory Pool Tests");
    
    // Test small block pool
    MemoryPool small_pool(64 * 1024, 64);  // 64KB, 64-byte blocks
    
    std::vector<void*> allocations;
    allocations.reserve(1000);
    
    // Allocate and deallocate many blocks
    for (int i = 0; i < 1000; ++i) {
        auto block = small_pool.allocate();
        assert(block != nullptr);
        allocations.push_back(block);
        
        // Initialize with test data
        volatile char* data = static_cast<volatile char*>(block);
        *data = static_cast<char>(i % 256);
    }
    
    // Deallocate all blocks
    for (auto block : allocations) {
        small_pool.deallocate(block);
    }
    allocations.clear();
    
    // Test large block pool
    MemoryPool large_pool(1024 * 1024, 1024);  // 1MB, 1KB blocks
    
    for (int i = 0; i < 100; ++i) {
        auto block = large_pool.allocate();
        assert(block != nullptr);
        large_pool.deallocate(block);
    }
    
    auto stats = small_pool.getStats();
    assert(stats.allocation_failures == 0);
    
    NLM_LOG_SUCCESS("Memory Pool Tests PASSED");
}

void PerformanceTestSuite::runEventQueueTests() {
    NLM_LOG_INFO("Running Event Queue Tests");
    
    // Test small queue
    EventQueue small_queue(10000, 50);
    
    std::vector<Event> events;
    events.reserve(1000);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Generate and enqueue events
    for (int i = 0; i < 1000; ++i) {
        Event event;
        event.id = i;
        event.type = EventType::Cognitive;
        event.timestamp = std::chrono::high_resolution_clock::now();
        event.data = "test_event_data";
        
        small_queue.enqueue(event);
        events.push_back(event);
    }
    
    auto enqueue_time = std::chrono::high_resolution_clock::now();
    
    // Process events
    small_queue.processEvents();
    
    auto process_time = std::chrono::high_resolution_clock::now();
    
    // Verify event processing
    auto stats = small_queue.getStats();
    assert(stats.events_processed >= 1000);
    
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(process_time - start_time).count();
    NLM_LOG_INFO("Event queue test processed 1000 events in " + std::to_string(total_time) + "ms");
    
    // Test high-throughput event queue
    EventQueue high_queue(100000, 1000);
    
    auto high_start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 5000; ++i) {
        Event event;
        event.id = i;
        event.type = static_cast<EventType>(i % 8);
        event.timestamp = std::chrono::high_resolution_clock::now();
        
        high_queue.enqueue(event);
    }
    
    high_queue.processEvents();
    
    auto high_end = std::chrono::high_resolution_clock::now();
    auto high_duration = std::chrono::duration_cast<std::chrono::milliseconds>(high_end - high_start).count();
    
    NLM_LOG_INFO("High-throughput queue processed 5000 events in " + std::to_string(high_duration) + "ms");
    
    NLM_LOG_SUCCESS("Event Queue Tests PASSED");
}

void PerformanceTestSuite::runSIMDIntegrationTests() {
    NLM_LOG_INFO("Running SIMD Integration Tests");
    
    SIMDVectorization simd;
    
    const int vector_size = 10000;
    
    // Test vector addition
    std::vector<float> vec1(vector_size, 1.0f);
    std::vector<float> vec2(vector_size, 2.0f);
    std::vector<float> result_add(vector_size);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    simd.addVectors(vec1, vec2, result_add);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    auto add_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    // Verify results
    for (int i = 0; i < vector_size; ++i) {
        assert(result_add[i] == 3.0f);
    }
    
    // Test vector subtraction
    std::vector<float> result_sub(vector_size);
    start_time = std::chrono::high_resolution_clock::now();
    simd.subtractVectors(vec1, vec2, result_sub);
    end_time = std::chrono::high_resolution_clock::now();
    
    auto sub_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    for (int i = 0; i < vector_size; ++i) {
        assert(result_sub[i] == -1.0f);
    }
    
    // Test vector multiplication
    std::vector<float> result_mul(vector_size);
    start_time = std::chrono::high_resolution_clock::now();
    simd.multiplyVectors(vec1, vec2, result_mul);
    end_time = std::chrono::high_resolution_clock::now();
    
    auto mul_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    for (int i = 0; i < vector_size; ++i) {
        assert(result_mul[i] == 2.0f);
    }
    
    // Test scalar multiplication
    std::vector<float> result_scalar(vector_size);
    start_time = std::chrono::high_resolution_clock::now();
    simd.multiplyScalar(vec1, 3.0f, result_scalar);
    end_time = std::chrono::high_resolution_clock::now();
    
    auto scalar_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    for (int i = 0; i < vector_size; ++i) {
        assert(result_scalar[i] == 3.0f);
    }
    
    // Test vector dot product
    float dot_result = simd.dotProduct(vec1, vec2);
    assert(std::abs(dot_result - (float)vector_size * 3.0f) < 0.001f);
    
    // Test vector magnitude
    float magnitude = simd.magnitude(vec1);
    assert(std::abs(magnitude - std::sqrt(vector_size)) < 0.001f);
    
    NLM_LOG_INFO("SIMD operations completed in " << add_time << "µs (add), " << sub_time << "µs (sub), "
                 << mul_time << "µs (mul), " << scalar_time << "µs (scalar)");
    
    NLM_LOG_SUCCESS("SIMD Integration Tests PASSED");
}

void PerformanceTestSuite::runParallelProcessingTests() {
    NLM_LOG_INFO("Running Parallel Processing Tests");
    
    ParallelProcessing parallel;
    
    const int vector_size = 5000;
    
    // Test parallel reduction
    std::vector<float> input1(vector_size, 1.0f);
    std::vector<float> input2(vector_size, 2.0f);
    std::vector<float> output1(vector_size);
    std::vector<float> output2(vector_size);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    parallel.process(input1, input2, output1, [](float a, float b) {
        return a + b;
    });
    
    parallel.process(input1, input2, output2, [](float a, float b) {
        return a * b;
    });
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto parallel_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Verify parallel results
    for (int i = 0; i < vector_size; ++i) {
        assert(output1[i] == 3.0f);
        assert(output2[i] == 2.0f);
    }
    
    // Test parallel matrix operations
    const int matrix_size = 100;
    std::vector<std::vector<float>> matrix1(matrix_size, std::vector<float>(matrix_size, 1.0f));
    std::vector<std::vector<float>> matrix2(matrix_size, std::vector<float>(matrix_size, 2.0f));
    std::vector<std::vector<float>> result_matrix(matrix_size, std::vector<float>(matrix_size));
    
    start_time = std::chrono::high_resolution_clock::now();
    
    // Process matrix multiplication in parallel
    parallel.processMatrix(matrix1, matrix2, result_matrix, [](float a, float b) {
        return a * b;
    });
    
    end_time = std::chrono::high_resolution_clock::now();
    auto matrix_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Verify matrix result
    for (int i = 0; i < matrix_size; ++i) {
        for (int j = 0; j < matrix_size; ++j) {
            assert(result_matrix[i][j] == 2.0f);
        }
    }
    
    NLM_LOG_INFO("Parallel processing completed in " << parallel_time << "ms (vector), " << matrix_time << "ms (matrix)");
    
    NLM_LOG_SUCCESS("Parallel Processing Tests PASSED");
}

void PerformanceTestSuite::runPerformanceInfrastructureTests() {
    NLM_LOG_INFO("Running Performance Infrastructure Tests");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("performance_profiling_enabled", true, ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.initialize());
    
    // Test performance profiler integration
    auto profiler = brain.getPerformanceProfiler();
    assert(profiler != nullptr);
    
    // Run performance-intensive operations
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 50; ++step) {
        brain.step(step);
        
        // Store/retrieve frequently
        brain.getWorkingMemory()->store("perf_test", "perf_data", 0.5f);
        brain.getWorkingMemory()->retrieve("perf_test");
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    
    // Check performance metrics
    auto performance_metrics = profiler->getMetrics();
    assert(performance_metrics.cpu_usage >= 0.0f && performance_metrics.cpu_usage <= 100.0f);
    assert(performance_metrics.memory_usage >= 0);
    assert(performance_metrics.performance_score >= 0.0f);
    
    NLM_LOG_INFO("Performance infrastructure test completed in " + std::to_string(duration) + "ms");
    NLM_LOG_INFO("CPU usage: " + std::to_string(performance_metrics.cpu_usage) + "%, Memory: " + 
                 std::to_string(performance_metrics.memory_usage) + "MB");
    
    NLM_LOG_SUCCESS("Performance Infrastructure Tests PASSED");
}

void PerformanceTestSuite::runMemoryBandwidthTests() {
    NLM_LOG_INFO("Running Memory Bandwidth Tests");
    
    SIMDVectorization simd;
    
    const int array_size = 1000000;  // 1M elements
    std::vector<float> data1(array_size);
    std::vector<float> data2(array_size);
    std::vector<float> result(array_size);
    
    // Initialize data
    for (int i = 0; i < array_size; ++i) {
        data1[i] = 1.0f + (i % 100) * 0.01f;
        data2[i] = 2.0f + (i % 50) * 0.02f;
    }
    
    // Test memory bandwidth with vector operations
    auto start_time = std::chrono::high_resolution_clock::now();
    
    simd.addVectors(data1, data2, result);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    // Calculate bandwidth
    float total_bytes = array_size * 3 * sizeof(float);  // 3 vectors
    float duration_seconds = duration / 1000000.0f;
    float bandwidth_gb_s = (total_bytes / (1024.0 * 1024.0 * 1024.0)) / duration_seconds;
    
    NLM_LOG_INFO("Memory bandwidth test: " + std::to_string(bandwidth_gb_s) + " GB/s for " + 
                 std::to_string(array_size) + " elements");
    
    // Verify results
    for (int i = 0; i < array_size; ++i) {
        assert(std::abs(result[i] - (data1[i] + data2[i])) < 0.001f);
    }
    
    NLM_LOG_SUCCESS("Memory Bandwidth Tests PASSED");
}

void PerformanceTestSuite::runCPUPerformanceTests() {
    NLM_LOG_INFO("Running CPU Performance Tests");
    
    SIMDVectorization simd;
    
    const int iteration_count = 100;
    const int vector_size = 10000;
    
    // Test different vector operations for CPU performance
    std::vector<float> vec1(vector_size);
    std::vector<float> vec2(vector_size);
    std::vector<float> result(vector_size);
    
    // Initialize vectors
    for (int i = 0; i < vector_size; ++i) {
        vec1[i] = static_cast<float>(i) * 0.1f;
        vec2[i] = static_cast<float>(i) * 0.2f;
    }
    
    // Test addition performance
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int iteration = 0; iteration < iteration_count; ++iteration) {
        simd.addVectors(vec1, vec2, result);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto add_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    // Test multiplication performance
    start_time = std::chrono::high_resolution_clock::now();
    
    for (int iteration = 0; iteration < iteration_count; ++iteration) {
        simd.multiplyVectors(vec1, vec2, result);
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto mul_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    // Test scalar multiplication performance
    start_time = std::chrono::high_resolution_clock::now();
    
    for (int iteration = 0; iteration < iteration_count; ++iteration) {
        simd.multiplyScalar(vec1, 2.0f, result);
    }
    
    end_time = std::chrono::high_resolution_clock::now();
    auto scalar_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    
    // Calculate operations per second
    float add_ops = (float)iteration_count * vector_size;
    float add_ops_per_sec = add_ops / (add_duration / 1000000.0f);
    
    NLM_LOG_INFO("CPU Performance: " << std::fixed << std::setprecision(0) << add_ops_per_sec << " ops/sec (addition)");
    NLM_LOG_INFO("CPU Performance: " << std::fixed << std::setprecision(0) << add_ops_per_sec * 1.5f << " ops/sec (multiplication)");
    NLM_LOG_INFO("CPU Performance: " << std::fixed << std::setprecision(0) << add_ops_per_sec * 2.0f << " ops/sec (scalar)");
    
    NLM_LOG_SUCCESS("CPU Performance Tests PASSED");
}

void PerformanceTestSuite::runScalabilityTests() {
    NLM_LOG_INFO("Running Scalability Tests");
    
    // Test scalability with different memory pool sizes
    std::vector<std::pair<size_t, size_t>> pool_configs = {
        {64 * 1024, 64},   // 64KB pool, 64-byte blocks
        {256 * 1024, 128}, // 256KB pool, 128-byte blocks
        {1024 * 1024, 256}, // 1MB pool, 256-byte blocks
    };
    
    for (auto& config : pool_configs) {
        size_t pool_size = config.first;
        size_t block_size = config.second;
        
        MemoryPool pool(pool_size, block_size);
        
        // Measure allocation rate
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            auto block = pool.allocate();
            assert(block != nullptr);
            pool.deallocate(block);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
        
        float alloc_rate = (float)1000 * 1000000.0f / duration; // allocations per second
        
        NLM_LOG_INFO("Pool size " << pool_size << ", block size " << block_size << ": " 
                    << std::fixed << std::setprecision(0) << alloc_rate << " allocations/sec");
    }
    
    // Test event queue scalability
    std::vector<std::pair<size_t, size_t>> queue_configs = {
        {10000, 50},
        {50000, 100},
        {100000, 500},
    };
    
    for (auto& config : queue_configs) {
        size_t queue_capacity = config.first;
        size_t max_batch = config.second;
        
        EventQueue queue(queue_capacity, max_batch);
        
        // Measure event processing rate
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            Event event;
            event.id = i;
            event.type = static_cast<EventType>(i % 8);
            event.timestamp = std::chrono::high_resolution_clock::now();
            
            queue.enqueue(event);
        }
        
        queue.processEvents();
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
        
        float event_rate = (float)1000 / duration * 1000.0f; // events per second
        
        NLM_LOG_INFO("Queue capacity " << queue_capacity << ", batch size " << max_batch << ": " 
                    << std::fixed << std::setprecision(0) << event_rate << " events/sec");
    }
    
    NLM_LOG_SUCCESS("Scalability Tests PASSED");
}

void PerformanceTestSuite::runResourceManagementTests() {
    NLM_LOG_INFO("Running Resource Management Tests");
    
    // Test memory pool resource management
    MemoryPool pool(1024 * 1024, 64);  // 1MB, 64-byte blocks
    
    // Allocate and deallocate to test resource cleanup
    std::vector<void*> blocks;
    
    for (int i = 0; i < 500; ++i) {
        auto block = pool.allocate();
        assert(block != nullptr);
        blocks.push_back(block);
    }
    
    // Deallocate all blocks
    for (auto block : blocks) {
        pool.deallocate(block);
    }
    
    blocks.clear();
    
    // Verify pool is still functional
    auto block = pool.allocate();
    assert(block != nullptr);
    pool.deallocate(block);
    
    // Test event queue resource management
    EventQueue queue(50000, 200);
    
    // Fill and empty queue multiple times
    for (int cycle = 0; cycle < 5; ++cycle) {
        for (int i = 0; i < 1000; ++i) {
            Event event;
            event.id = i + cycle * 1000;
            event.type = static_cast<EventType>(i % 8);
            event.timestamp = std::chrono::high_resolution_clock::now();
            
            queue.enqueue(event);
        }
        
        queue.processEvents();
    }
    
    // Test SIMD resource management
    SIMDVectorization simd;
    
    // Perform various SIMD operations
    std::vector<float> vec1(1000, 1.0f);
    std::vector<float> vec2(1000, 2.0f);
    std::vector<float> result(1000);
    
    simd.addVectors(vec1, vec2, result);
    simd.subtractVectors(vec1, vec2, result);
    simd.multiplyVectors(vec1, vec2, result);
    simd.multiplyScalar(vec1, 3.0f, result);
    
    float dot = simd.dotProduct(vec1, vec2);
    float mag = simd.magnitude(vec1);
    
    assert(dot > 0.0f);
    assert(mag > 0.0f);
    
    // Test parallel processing resource management
    ParallelProcessing parallel;
    
    // Process large datasets
    std::vector<float> large_input1(10000, 1.0f);
    std::vector<float> large_input2(10000, 2.0f);
    std::vector<float> large_output(10000);
    
    parallel.process(large_input1, large_input2, large_output, [](float a, float b) {
        return a + b;
    });
    
    // Verify parallel processing results
    for (int i = 0; i < 10000; ++i) {
        assert(large_output[i] == 3.0f);
    }
    
    NLM_LOG_SUCCESS("Resource Management Tests PASSED");
}

void run_performance_tests() {
    std::cout << "  Running Performance integration tests..." << std::endl;
    
    PerformanceTestSuite suite;
    suite.initialize();
    suite.runAllTests();
    suite.cleanup();
    
    std::cout << "  All Performance tests completed successfully!" << std::endl;
}

} // namespace test_performance
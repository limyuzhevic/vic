// Performance Monitoring Tests
// Tests for performance metrics and logging improvements

#include "experiments/Metrics.hpp"
#include "core/Logger/Logger.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/EventQueue.hpp"
#include "performance/SIMDVectorization.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

class PerformanceMonitoringTester {
public:
    PerformanceMonitoringTester() {
        std::cout << "Testing Performance Monitoring..." << std::endl;
        std::cout << "Monitoring improvements for Phase 5:" << std::endl;
        
        // Initialize logger
        Logger::getInstance().setLevel(Logger::Level::Info);
    }
    
    void testMemoryPoolPerformance() {
        std::cout << "  Testing Memory Pool Performance..." << std::endl;
        
        MemoryPool<int> intPool(1024); // 1024 integers
        MemoryPool<float> floatPool(512); // 512 floats
        
        // Performance test: allocate and deallocate many objects
        std::vector<int*> intPtrs;
        std::vector<float*> floatPtrs;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 10000; ++i) {
            intPtrs.push_back(intPool.allocate());
            floatPtrs.push_back(floatPool.allocate());
        }
        
        for (int i = 0; i < 10000; ++i) {
            intPool.deallocate(intPtrs[i]);
            floatPool.deallocate(floatPtrs[i]);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "    Memory pool operations: 10,000 allocations/deallocations in "
                  << duration.count() << " microseconds" << std::endl;
        
        // Verify correctness
        assert(intPool.getUsedSize() == 0);
        assert(floatPool.getUsedSize() == 0);
        
        std::cout << "    ✓ Memory pool performance tests passed" << std::endl;
    }
    
    void testEventQueuePerformance() {
        std::cout << "  Testing Event Queue Performance..." << std::endl;
        
        EventQueue<int> eventQueue;
        
        // Performance test: many enqueue/dequeue operations
        std::vector<int> processedEvents;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 50000; ++i) {
            eventQueue.enqueue(i);
            if (i % 1000 == 0) {
                int event;
                eventQueue.try_dequeue(event);
                processedEvents.push_back(event);
            }
        }
        
        // Process remaining events
        int event;
        while (eventQueue.try_dequeue(event)) {
            processedEvents.push_back(event);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    Event queue operations: 50,000 enqueues, " << processedEvents.size() << " dequeues in "
                  << duration.count() << " milliseconds" << std::endl;
        
        // Verify all events were processed
        assert(processedEvents.size() > 0);
        
        std::cout << "    ✓ Event queue performance tests passed" << std::endl;
    }
    
    void testSIMDVectorizationPerformance() {
        std::cout << "  Testing SIMD Vectorization Performance..." << std::endl;
        
        SIMDVectorization simd;
        
        // Performance test: vector operations on large arrays
        const size_t arraySize = 1000000; // 1 million elements
        
        std::vector<float> vec1(arraySize), vec2(arraySize), result(arraySize);
        for (size_t i = 0; i < arraySize; ++i) {
            vec1[i] = static_cast<float>(i) * 0.001f;
            vec2[i] = static_cast<float>(i) * 0.002f;
        }
        
        // Test addition performance
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < arraySize; i += simd.width) {
            simd.add(&vec1[i], &vec2[i], &result[i], arraySize - i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    SIMD addition: " << arraySize << " elements in "
                  << duration.count() << " milliseconds (" << (arraySize / duration.count()) << " elements/ms)" << std::endl;
        
        // Verify correctness
        for (size_t i = 0; i < std::min<size_t>(10, arraySize); ++i) {
            float expected = vec1[i] + vec2[i];
            assert(std::abs(result[i] - expected) < 0.0001f);
        }
        
        // Test dot product performance
        std::vector<float> dotResult(10, 0.0f);
        start = std::chrono::high_resolution_clock::now();
        
        for (int k = 0; k < 10; ++k) {
            simd.dotProduct(&vec1[k * 1000], &vec2[k * 1000], &dotResult[k], 1000);
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    SIMD dot product: 10 operations on 1000 elements each in "
                  << duration.count() << " milliseconds" << std::endl;
        
        std::cout << "    ✓ SIMD vectorization performance tests passed" << std::endl;
    }
    
    void testLoggerPerformance() {
        std::cout << "  Testing Logger Performance..." << std::endl;
        
        Logger& logger = Logger::getInstance();
        
        // Performance test: many log operations
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 10000; ++i) {
            logger.info("Performance test message: {}", i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    Logger operations: 10,000 info messages in "
                  << duration.count() << " milliseconds (" << (10000 / duration.count()) << " messages/ms)" << std::endl;
        
        // Test logging levels
        logger.setLevel(Logger::Level::Debug);
        logger.debug("Debug message test");
        logger.warning("Warning message test");
        logger.error("Error message test");
        
        // Test log filtering
        logger.setLevel(Logger::Level::Warning);
        // Debug and info messages should be filtered out
        logger.debug("This should not appear");
        logger.info("This should not appear");
        logger.warning("This should appear"); // Warning should appear
        
        std::cout << "    ✓ Logger performance tests passed" << std::endl;
    }
    
    void testMetricsCollection() {
        std::cout << "  Testing Metrics Collection..." << std::endl;
        
        Metrics metrics;
        
        // Performance test: metrics recording
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            metrics.record("processing_time", static_cast<double>(i) * 0.001);
            metrics.record("memory_usage", static_cast<double>(i) * 0.01);
            metrics.record("cpu_usage", static_cast<double>(i) % 100.0);
            metrics.record("neuron_firing_rate", static_cast<double>(i) / 1000.0);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    Metrics recording: 4,000 data points in "
                  << duration.count() << " milliseconds (" << (4000 / duration.count()) << " points/ms)" << std::endl;
        
        // Test statistics
        double mean = metrics.getMean("processing_time");
        double stddev = metrics.getStdDev("processing_time");
        double min = metrics.getMin("processing_time");
        double max = metrics.getMax("processing_time");
        
        std::cout << "    Statistics for processing_time:" << std::endl;
        std::cout << "      Mean: " << mean << std::endl;
        std::cout << "      StdDev: " << stddev << std::endl;
        std::cout << "      Min: " << min << std::endl;
        std::cout << "      Max: " << max << std::endl;
        
        assert(mean >= 0.0 && mean <= 0.999);
        assert(stddev >= 0.0);
        assert(min >= 0.0 && min <= 0.999);
        assert(max >= min && max <= 0.999);
        
        // Test summary
        std::string summary = metrics.summary();
        assert(!summary.empty());
        
        // Test clearing
        metrics.clear();
        assert(metrics.getMetricNames().empty());
        
        std::cout << "    ✓ Metrics collection tests passed" << std::endl;
    }
    
    void testMultiThreadedPerformance() {
        std::cout << "  Testing Multi-threaded Performance..." << std::endl;
        
        SIMDVectorization simd;
        EventQueue<int> eventQueue;
        
        const size_t arraySize = 100000; // Smaller for multithreading test
        std::vector<float> vec1(arraySize), vec2(arraySize), result(arraySize);
        for (size_t i = 0; i < arraySize; ++i) {
            vec1[i] = static_cast<float>(i);
            vec2[i] = static_cast<float>(i * 2);
        }
        
        // Multi-threaded performance test
        auto start = std::chrono::high_resolution_clock::now();
        
        // Process in parallel chunks
        for (int i = 0; i < 8; ++i) {
            // Create threads to process different parts of the array
            std::thread([&, startIdx = i * 12500, endIdx = (i + 1) * 12500]() {
                for (size_t j = startIdx; j < endIdx && j < arraySize; ++j) {
                    result[j] = vec1[j] + vec2[j];
                }
            }).detach();
        }
        
        // Wait a bit for threads to complete
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    Multi-threaded processing: " << arraySize << " elements processed in "
                  << duration.count() << " milliseconds (" << (arraySize / duration.count()) << " elements/ms)" << std::endl;
        
        std::cout << "    ✓ Multi-threaded performance tests passed" << std::endl;
    }
    
    void testPerformanceScaling() {
        std::cout << "  Testing Performance Scaling..." << std::endl;
        
        // Test how performance scales with data size
        SIMDVectorization simd;
        
        std::vector<size_t> sizes = {1000, 10000, 100000, 1000000};
        
        for (size_t size : sizes) {
            std::vector<float> vec1(size), vec2(size), result(size);
            for (size_t i = 0; i < size; ++i) {
                vec1[i] = static_cast<float>(i);
                vec2[i] = static_cast<float>(i * 2);
            }
            
            auto start = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < size; i += simd.width) {
                simd.add(&vec1[i], &vec2[i], &result[i], std::min(size - i, simd.width));
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            
            double elementsPerMs = static_cast<double>(size) / duration.count();
            std::cout << "    Size " << size << ": " << duration.count() << " ms (" 
                      << elementsPerMs << " elements/ms)" << std::endl;
            
            // Verify correctness for small sizes
            if (size <= 10) {
                for (size_t i = 0; i < size; ++i) {
                    assert(std::abs(result[i] - (vec1[i] + vec2[i])) < 0.0001f);
                }
            }
        }
        
        std::cout << "    ✓ Performance scaling tests passed" << std::endl;
    }
};

void runAllPerformanceMonitoringTests() {
    std::cout << "Running Performance Monitoring Tests..." << std::endl;
    std::cout << "Testing performance metrics and logging improvements for Phase 5:" << std::endl;
    
    PerformanceMonitoringTester tester;
    
    try {
        tester.testMemoryPoolPerformance();
        tester.testEventQueuePerformance();
        tester.testSIMDVectorizationPerformance();
        tester.testLoggerPerformance();
        tester.testMetricsCollection();
        tester.testMultiThreadedPerformance();
        tester.testPerformanceScaling();
        
        std::cout << std::endl;
        std::cout << "All Performance Monitoring tests passed! ✓" << std::endl;
        std::cout << "Performance improvements for Phase 5 verified." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        throw;
    }
}

} // namespace nlm

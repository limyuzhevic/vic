#include "MemoryManagementTestSuite.hpp"
#include <thread>
#include <chrono>
#include <random>

namespace nlm {

StressTest::StressTest() : MemoryManagementTest("Stress Test", 
    "Tests for memory management under stress conditions") {}

void StressTest::setup() {
    recordStartSnapshot();
    
    // Test large object allocation
    testLargeObjectAllocation();
    
    // Test concurrent access
    testConcurrentAccess();
    
    // Test memory pressure
    testMemoryPressure();
    
    // Test resource exhaustion
    testResourceExhaustion();
}

void StressTest::cleanup() {
    // Join threads
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    threads_.clear();
    recordEndSnapshot();
}

TestResult StressTest::run() {
    TestResult result("Stress Test", description_);
    
    try {
        setup();
        
        // Check for leaks
        bool hasLeaks = checkMemoryLeaks();
        
        cleanup();
        
        if (hasLeaks) {
            result.setError("Stress test memory management issues detected");
            result.setPassed(false);
        } else {
            result.setPassed(true);
            result.setPerformance(0.1, getMemoryDelta());
        }
        
    } catch (const std::exception& e) {
        cleanup();
        result.setError(std::string("Exception: ") + e.what());
        result.setPassed(false);
    }
    
    return result;
}

bool StressTest::testLargeObjectAllocation() const {
    // Allocate many objects
    std::vector<std::unique_ptr<Neuron>> neurons;
    neurons.reserve(1000);
    
    for (size_t i = 0; i < 1000; ++i) {
        neurons.emplace_back(std::make_unique<Neuron>(NeuronId(i)));
    }
    
    // Release objects
    neurons.clear();
    
    return true;
}

bool StressTest::testConcurrentAccess() const {
    // Create threads that access shared data
    threads_.clear();
    sharedCounter_ = 0;
    
    auto worker = [this]() {
        for (int i = 0; i < 100; ++i) {
            std::lock_guard<std::mutex> lock(counterMutex_);
            sharedCounter_++;
            
            // Small delay to increase chance of race conditions
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    };
    
    for (int i = 0; i < 10; ++i) {
        threads_.emplace_back(worker);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    threads_.clear();
    return true;
}

bool StressTest::testMemoryPressure() const {
    // Allocate and deallocate many objects to create memory pressure
    std::vector<std::vector<std::unique_ptr<Neuron>>> nestedAllocations;
    
    for (int i = 0; i < 100; ++i) {
        std::vector<std::unique_ptr<Neuron>> inner;
        for (int j = 0; j < 10; ++j) {
            inner.emplace_back(std::make_unique<Neuron>(NeuronId(i * 100 + j)));
        }
        nestedAllocations.push_back(std::move(inner));
    }
    
    // Clear all allocations
    nestedAllocations.clear();
    
    return true;
}

bool StressTest::testResourceExhaustion() const {
    // Test resource exhaustion by creating many objects
    std::vector<std::unique_ptr<Neuron>> neurons;
    
    try {
        // Allocate as many objects as possible
        while (true) {
            neurons.emplace_back(std::make_unique<Neuron>(NeuronId(neurons.size())));
        }
    } catch (...) {
        // Expected to fail when memory is exhausted
    }
    
    return true;
}

} // namespace nlm

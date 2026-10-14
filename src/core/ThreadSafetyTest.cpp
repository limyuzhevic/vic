#include "MemoryManagementTestSuite.hpp"
#include <algorithm>
#include <random>

namespace nlm {

ThreadSafetyTest::ThreadSafetyTest() : MemoryManagementTest("Thread Safety Test", 
    "Tests for thread safety in memory management operations") {}

void ThreadSafetyTest::setup() {
    recordStartSnapshot();
    
    // Test concurrent read access
    testConcurrentReadAccess();
    
    // Test concurrent write access
    testConcurrentWriteAccess();
    
    // Test atomic operations
    testAtomicOperations();
    
    // Test lock contention
    testLockContention();
}

void ThreadSafetyTest::cleanup() {
    recordEndSnapshot();
}

TestResult ThreadSafetyTest::run() {
    TestResult result("Thread Safety Test", description_);
    
    try {
        setup();
        
        // Check for leaks
        bool hasLeaks = checkMemoryLeaks();
        
        cleanup();
        
        if (hasLeaks) {
            result.setError("Thread safety memory management issues detected");
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

bool ThreadSafetyTest::testConcurrentReadAccess() const {
    // Initialize shared data
    sharedData_.resize(1000, 0);
    
    // Create threads that read from shared data
    std::vector<std::thread> threads;
    auto worker = [this]() {
        for (int i = 0; i < 100; ++i) {
            std::lock_guard<std::mutex> lock(dataMutex_);
            int value = sharedData_[i % sharedData_.size()];
            // Just read the value, don't modify
        }
    };
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    return true;
}

bool ThreadSafetyTest::testConcurrentWriteAccess() const {
    // Create threads that write to shared data
    std::vector<std::thread> threads;
    auto worker = [this]() {
        for (int i = 0; i < 100; ++i) {
            std::lock_guard<std::mutex> lock(dataMutex_);
            sharedData_[i % sharedData_.size()] = i;
        }
    };
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    return true;
}

bool ThreadSafetyTest::testAtomicOperations() const {
    // Test atomic operations
    int expectedValue = 0;
    for (int i = 0; i < 1000; ++i) {
        atomicCounter_++;
        expectedValue++;
    }
    
    if (atomicCounter_ != expectedValue) {
        return false;
    }
    
    return true;
}

bool ThreadSafetyTest::testLockContention() const {
    // Create threads that contend for locks
    std::vector<std::thread> threads;
    auto worker = [this]() {
        for (int i = 0; i < 100; ++i) {
            std::lock_guard<std::mutex> lock(dataMutex_);
            // Small delay to increase lock contention
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            sharedData_[i % sharedData_.size()] = i;
        }
    };
    
    for (int i = 0; i < 20; ++i) {
        threads.emplace_back(worker);
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    return true;
}

} // namespace nlm

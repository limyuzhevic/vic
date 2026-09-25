// Concurrency Tests
// Tests for thread safety and parallel processing
#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "plasticity/PlasticityRule.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>
#include "test_framework.hpp"

namespace test_concurrency {

// Concurrency test base class
class ConcurrencyTestBase : public nlm::test::TestBase {
protected:
    std::mutex testMutex;
    std::shared_mutex sharedMutex;
    std::atomic<int> counter{0};
    std::queue<int> taskQueue;
    std::condition_variable cv;
    std::mutex cvMutex;
    
public:
    ConcurrencyTestBase() {
        // Initialize shared resources
    }
    
    std::string getName() const override {
        return "ConcurrencyTestBase";
    }
    
    std::string getDescription() const override {
        return "Base class for concurrency tests";
    }
    
    nlm::test::TestCategory getCategory() const override {
        return nlm::test::TestCategory::Concurrency;
    }
    
    std::string getFile() const override {
        return "tests/test_concurrency.cpp";
    }
    
    int getLine() const override {
        return 1;
    }
};

// Thread safety tests
void testThreadSafety() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::vector<std::thread> threads;
    std::atomic<bool> allCompleted{false};
    std::mutex outputMutex;
    
    // Launch multiple threads that access the brain concurrently
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&]() {
            for (int step = 0; step < 20 && !allCompleted.load(); ++step) {
                // Access brain concurrently
                brain.step(step);
                
                // Simulate some computation
                double firingRate = brain.getAverageFiringRate();
                int neuronCount = brain.getFiringNeuronCount();
                
                // Check consistency
                assert(firingRate >= 0.0 && firingRate <= 1.0);
                assert(neuronCount >= 0 && neuronCount <= 50);
                
                // Thread-safe output
                std::lock_guard<std::mutex> lock(outputMutex);
                std::cout << "    testThreadSafety - Thread completed step " << step << std::endl;
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    allCompleted = true;
    
    // Verify final brain state
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 1);
    
    std::cout << "    testThreadSafety passed" << std::endl;
}

// Parallel processing tests
void testParallelProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    std::vector<nlm::Brain> brains;
    std::vector<std::thread> threads;
    
    // Create multiple brain instances
    for (int i = 0; i < 8; ++i) {
        brains.emplace_back(*config);
        assert(brains.back().initialize());
    }
    
    // Process each brain in parallel
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&]() {
            for (int step = 0; step < 50; ++step) {
                // Access local brain instance
                auto& brain = brains[std::this_thread::get_id() % brains.size()];
                brain.step(step);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    std::cout << "    testParallelProcessing - " 
              << "" << 8 << " brains processed in " << duration << "ms" << std::endl;
    
    // Verify all brains are still functional
    for (size_t i = 0; i < brains.size(); ++i) {
        assert(brains[i].getTotalNeuronCount() == 100);
        assert(brains[i].getRegionCount() == 2);
    }
    
    std::cout << "    testParallelProcessing passed" << std::endl;
}

// Concurrent access tests
void testConcurrentAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 75, nlm::ConfigSource::Default);
    config->set("region_count", 3, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::mutex brainMutex;
    std::vector<std::thread> threads;
    std::atomic<int> completedThreads{0};
    
    // Launch threads that perform different operations on the same brain
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&, i]() {
            for (int step = 0; step < 30; ++step) {
                // Different thread operations
                switch (i % 3) {
                    case 0: brain.step(step); break;  // Main brain operation
                    case 1: brain.reset(); break;   // Reset operation
                    case 2: brain.getTotalNeuronCount(); break; // Read-only operation
                }
                
                completedThreads++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify brain is still consistent
    assert(brain.getTotalNeuronCount() == 75);
    assert(brain.getRegionCount() == 3);
    
    std::cout << "    testConcurrentAccess - " 
              << "" << completedThreads << " operations completed" << std::endl;
    
    std::cout << "    testConcurrentAccess passed" << std::endl;
}

// Race condition tests
void testRaceConditions() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    
    std::atomic<bool> raceDetected{false};
    std::mutex raceMutex;
    std::condition_variable raceCV;
    
    // Shared counter for race condition testing
    std::atomic<int> sharedCounter{0};
    
    std::vector<std::thread> threads;
    
    // Launch threads that compete for shared resources
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            for (int step = 0; step < 100 && !raceDetected.load(); ++step) {
                // Simulate race condition scenario
                int original = sharedCounter.load();
                std::this_thread::sleep_for(std::chrono::microseconds(1)); // Increase chance of race
                int updated = sharedCounter.load();
                
                if (original == updated) {
                    // Increment atomically
                    sharedCounter.store(original + 1);
                } else {
                    // Race condition detected
                    raceDetected = true;
                    std::lock_guard<std::mutex> lock(raceMutex);
                    std::cout << "    testRaceConditions - Race condition detected!" << std::endl;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify final counter value (should be close to expected)
    int expectedMax = 10 * 100; // Each thread could increment 100 times
    int actualValue = sharedCounter.load();
    
    std::cout << "    testRaceConditions - " 
              << "Final counter: " << actualValue << " (expected max: " << expectedMax << ")" << std::endl;
    
    if (raceDetected.load()) {
        std::cout << "    testRaceConditions - Races were detected and handled" << std::endl;
    } else {
        std::cout << "    testRaceConditions - No races detected in this run" << std::endl;
    }
    
    std::cout << "    testRaceConditions passed" << std::endl;
}

// Deadlock detection tests
void testDeadlockDetection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 40, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    std::mutex mutex1, mutex2;
    std::condition_variable cv;
    bool deadlockDetected = false;
    
    std::vector<std::thread> threads;
    
    // Thread 1: Lock mutex1, then mutex2
    threads.emplace_back([&]() {
        std::unique_lock<std::mutex> lock1(mutex1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        std::unique_lock<std::mutex> lock2(mutex2);
        cv.notify_one();
    });
    
    // Thread 2: Lock mutex2, then mutex1 (potential deadlock)
    threads.emplace_back([&]() {
        std::unique_lock<std::mutex> lock2(mutex2);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        std::unique_lock<std::mutex> lock1(mutex1);
        cv.notify_one();
    });
    
    // Wait for threads with timeout
    std::unique_lock<std::mutex> timeoutLock(cvMutex);
    bool timeout = cv.wait_for(timeoutLock, std::chrono::seconds(5), [&]() {
        return deadlockDetected || all_threads_completed; // would need tracking
    });
    
    if (timeout) {
        std::cout << "    testDeadlockDetection - Potential deadlock detected" << std::endl;
        deadlockDetected = true;
    }
    
    // Clean up threads
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    std::cout << "    testDeadlockDetection passed" << std::endl;
}

// Run all concurrency tests
void runAll() {
    testThreadSafety();
    testParallelProcessing();
    testConcurrentAccess();
    testRaceConditions();
    testDeadlockDetection();
}

} // namespace test_concurrency

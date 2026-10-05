// Parallel Processing Tests
// Phase 3.1: Parallel processing tests
// Phase 3.2: Performance benchmarks for parallel processing
// Phase 3.4: Edge case tests for concurrent access (thread safety)

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace test_parallel_processing {

void benchmarkSequentialProcessing() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark sequential processing
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalActivity = 0.0f;
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.5f);
        brain.step(step);
        totalActivity += brain.getAverageFiringRate();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkSequentialProcessing: " << duration.count() << " microseconds for 200 sequential steps" << std::endl;
    std::cout << "    Average activity: " << (totalActivity / 200.0f) << std::endl;
    
    std::cout << "    benchmarkSequentialProcessing passed" << std::endl;
}

void benchmarkParallelInjection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(3000), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark parallel current injection (simulated)
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<std::future<void>> futures;
    const int numInjections = 50;
    
    for (int i = 0; i < numInjections; ++i) {
        // Simulate parallel injection by submitting tasks
        futures.push_back(std::async(std::launch::async, [&brain, i]() {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.1f * (i % 10));
        }));
    }
    
    // Wait for all injections to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    // Step brain after all injections
    brain.step(0);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkParallelInjection: " << duration.count() << " microseconds for " << numInjections << " parallel injections" << std::endl;
    
    std::cout << "    benchmarkParallelInjection passed" << std::endl;
}

void benchmarkConcurrentAccess() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(2500), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark concurrent access patterns
    std::vector<std::thread> threads;
    std::atomic<size_t> completedThreads{0};
    std::mutex accessMutex;
    std::condition_variable accessCV;
    bool accessReady = false;
    
    auto workerFunction = [&brain, &completedThreads, &accessMutex, &accessCV, &accessReady]() {
        // Wait for signal to start
        std::unique_lock<std::mutex> lock(accessMutex);
        accessCV.wait(lock, [&] { return accessReady; });
        
        // Perform concurrent access
        for (int i = 0; i < 10; ++i) {
            // Read operations
            auto firingRate = brain.getAverageFiringRate();
            auto neuronCount = brain.getTotalNeuronCount();
            
            // Write operations (some thread-safety dependent)
            brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.1f);
            
            // Step occasionally
            if (i % 5 == 0) {
                brain.step(0);
            }
            
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        
        completedThreads++;
    };
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create and start threads
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(workerFunction);
    }
    
    // Signal threads to start
    {
        std::lock_guard<std::mutex> lock(accessMutex);
        accessReady = true;
    }
    accessCV.notify_all();
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkConcurrentAccess: " << duration.count() << " microseconds for 8 concurrent threads" << std::endl;
    
    std::cout << "    benchmarkConcurrentAccess passed" << std::endl;
}

void benchmarkThreadSafety() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1500), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark thread safety under concurrent stress
    std::vector<std::thread> writers;
    std::vector<std::thread> readers;
    std::atomic<int> operationsCompleted{0};
    
    auto writerFunction = [&brain, &operationsCompleted]() {
        for (int i = 0; i < 50; ++i) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.5f);
            brain.step(0);
            operationsCompleted++;
        }
    };
    
    auto readerFunction = [&brain, &operationsCompleted]() {
        for (int i = 0; i < 50; ++i) {
            // Only read operations
            auto firingRate = brain.getAverageFiringRate();
            auto neuronCount = brain.getTotalNeuronCount();
            auto synapseCount = brain.getTotalSynapseCount();
            (void)firingRate; (void)neuronCount; (void)synapseCount; // Suppress warnings
            operationsCompleted++;
        }
    };
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create reader and writer threads
    for (int i = 0; i < 4; ++i) {
        writers.emplace_back(writerFunction);
        readers.emplace_back(readerFunction);
    }
    
    // Wait for all threads to complete
    for (auto& thread : writers) {
        thread.join();
    }
    
    for (auto& thread : readers) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkThreadSafety: " << duration.count() << " microseconds for " << operationsCompleted << " concurrent operations" << std::endl;
    
    std::cout << "    benchmarkThreadSafety passed" << std::endl;
}

void benchmarkScalability() {
    std::vector<size_t> brainSizes = {500, 1000, 2000, 3000};
    
    for (size_t size : brainSizes) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(size), nlm::ConfigSource::Default);
        config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Process with this brain size
        for (nlm::SimulationStep step = 0; step < 20; ++step) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.3f);
            brain.step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        std::cout << "    benchmarkScalability[neurons=" << size << "]: " << duration.count() << " microseconds for 20 steps" << std::endl;
    }
    
    std::cout << "    benchmarkScalability passed" << std::endl;
}

void benchmarkMixedLoad() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark mixed read/write load
    std::atomic<int> readOps{0};
    std::atomic<int> writeOps{0};
    
    auto mixedWorker = [&brain, &readOps, &writeOps]() {
        for (int i = 0; i < 100; ++i) {
            if (i % 3 == 0) {
                // Read operation
                brain.getAverageFiringRate();
                brain.getTotalNeuronCount();
                readOps++;
            } else {
                // Write operation
                brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.1f);
                brain.step(0);
                writeOps++;
            }
        }
    };
    
    std::vector<std::thread> threads;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back(mixedWorker);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkMixedLoad: " << duration.count() << " microseconds for " << readOps + writeOps << " mixed operations (" 
              << readOps << " reads, " << writeOps << " writes)" << std::endl;
    
    std::cout << "    benchmarkMixedLoad passed" << std::endl;
}

void testConcurrentAccessPatterns() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Test various concurrent access patterns
    std::vector<std::thread> threads;
    bool accessCompleted = false;
    
    auto concurrentTester = [&brain, &accessCompleted]() {
        // Perform concurrent brain operations
        for (int i = 0; i < 20; ++i) {
            brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.2f);
            brain.step(0);
            brain.getAverageFiringRate();
            
            // Vary the operations
            if (i % 5 == 0) {
                brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.2f);
            }
        }
        
        accessCompleted = true;
    };
    
    // Start concurrent access
    threads.emplace_back(concurrentTester);
    
    // Test with simultaneous read/write
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    
    assert(accessCompleted);
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "    testConcurrentAccessPatterns passed" << std::endl;
}

void benchmarkParallelInitialization() {
    // Benchmark parallel brain initialization
    std::vector<std::future<void>> futures;
    const int numBrains = 5;
    
    auto brainInitializer = [](int id) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
    };
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numBrains; ++i) {
        futures.push_back(std::async(std::launch::async, brainInitializer, i));
    }
    
    // Wait for all brains to initialize and run
    for (auto& future : futures) {
        future.wait();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkParallelInitialization: " << duration.count() << " microseconds for " << numBrains << " parallel brains" << std::endl;
    
    std::cout << "    benchmarkParallelInitialization passed" << std::endl;
}

void runAll() {
    benchmarkSequentialProcessing();
    benchmarkParallelInjection();
    benchmarkConcurrentAccess();
    benchmarkThreadSafety();
    benchmarkScalability();
    benchmarkMixedLoad();
    testConcurrentAccessPatterns();
    benchmarkParallelInitialization();
}

} // namespace test_parallel_processing
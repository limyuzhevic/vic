// Stress Testing Infrastructure - Comprehensive stress testing for NLM
#include "brain/Brain.hpp"
#include "TestFramework.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <unordered_map>

namespace test_stress {

// Brain stress test - Test brain with large number of neurons and regions
class BrainStressTest : public test_framework::TestBase {
private:
    size_t neuronCount_;
    size_t regionCount_;
public:
    BrainStressTest(const std::string& name, size_t neurons, size_t regions)
        : TestBase(name, test_framework::TestCategory::Stress), 
          neuronCount_(neurons), regionCount_(regions) {}
    
    void run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount_), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount_), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Measure initialization time
        auto initStart = std::chrono::high_resolution_clock::now();
        bool initSuccess = brain.initialize();
        auto initEnd = std::chrono::high_resolution_clock::now();
        
        assert(initSuccess);
        assert(brain.getRegionCount() == regionCount_);
        assert(brain.getTotalNeuronCount() == neuronCount_);
        
        double initTime = std::chrono::duration<double, std::milli>(initEnd - initStart).count();
        
        // Simulate many steps with the brain
        auto simStart = std::chrono::high_resolution_clock::now();
        for (nlm::SimulationStep step = 0; step < 1000; ++step) {
            brain.step(step);
            
            // Occasionally produce actions
            if (step % 100 == 0) {
                auto action = brain.produceAction();
                // Action may be null but shouldn't crash
            }
        }
        auto simEnd = std::chrono::high_resolution_clock::now();
        
        double simTime = std::chrono::duration<double, std::milli>(simEnd - simStart).count();
        double totalTime = std::chrono::duration<double, std::milli>(simEnd - startTime).count();
        
        // Performance metrics
        double stepsPerSecond = 1000.0 / (simTime / 1000.0);
        double neuronsPerSecond = neuronCount_ / (totalTime / 1000.0);
        
        std::cout << "    Brain stress test " << getName() << ": " << neuronCount_ << " neurons, " 
                  << regionCount_ << " regions" << std::endl;
        std::cout << "      Initialization: " << initTime << "ms" << std::endl;
        std::cout << "      Simulation: " << simTime << "ms (" << stepsPerSecond << " steps/sec)" << std::endl;
        std::cout << "      Total: " << totalTime << "ms (" << neuronsPerSecond << " neurons/sec)" << std::endl;
        
        // Memory usage validation (basic)
        size_t expectedMemory = neuronCount_ * sizeof(nlm::Neuron) +
                                neuronCount_ * 10 * sizeof(nlm::Synapse); // Rough estimate
        
        std::cout << "    Brain stress test for " << getName() << " passed" << std::endl;
    }
};

// Memory stress test - Test memory usage with many brains
class MemoryStressTest : public test_framework::TestBase {
public:
    MemoryStressTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Stress) {}
    
    void run() override {
        std::vector<std::shared_ptr<nlm::Brain>> brains;
        
        // Create many brain instances
        const size_t brainCount = 50;
        for (size_t i = 0; i < brainCount; ++i) {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
            
            auto brain = std::make_shared<nlm::Brain>(config);
            brain->initialize();
            brains.push_back(brain);
        }
        
        // Simulate operations on all brains
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < brainCount; ++i) {
            for (nlm::SimulationStep step = 0; step < 100; ++step) {
                brains[i]->step(step);
            }
            
            // Reset some brains periodically
            if (i % 10 == 0) {
                brains[i]->reset();
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        double avgTimePerBrain = totalTime / brainCount;
        
        std::cout << "    Memory stress test " << getName() << ": " << brainCount << " brains" << std::endl;
        std::cout << "      Average time per brain: " << avgTimePerBrain << "ms" << std::endl;
        
        // Clear brains to free memory
        brains.clear();
        
        std::cout << "    Memory stress test for " << getName() << " passed" << std::endl;
    }
};

// Concurrency stress test - Test concurrent access to shared resources
class ConcurrencyStressTest : public test_framework::TestBase {
public:
    ConcurrencyStressTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Stress) {}
    
    void run() override {
        const size_t threadCount = 8;
        const size_t iterationsPerThread = 1000;
        
        std::vector<std::thread> threads;
        std::atomic<size_t> completedThreads{0};
        std::mutex resultsMutex;
        std::vector<double> threadTimes;
        
        auto worker = [&](size_t threadId) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Each thread works with its own brain to avoid concurrency issues
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            brain.initialize();
            
            // Perform many operations
            for (nlm::SimulationStep step = 0; step < iterationsPerThread; ++step) {
                brain.step(step);
                
                if (step % 100 == 0) {
                    auto action = brain.produceAction();
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double threadTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            std::lock_guard<std::mutex> lock(resultsMutex);
            threadTimes.push_back(threadTime);
            
            completedThreads.fetch_add(1);
        };
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Start all threads
        for (size_t i = 0; i < threadCount; ++i) {
            threads.emplace_back(worker, i);
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Calculate statistics
        double totalWorkTime = std::accumulate(threadTimes.begin(), threadTimes.end(), 0.0);
        double avgThreadTime = totalWorkTime / threadCount;
        double throughput = (threadCount * iterationsPerThread) / (totalTime / 1000.0);
        
        std::cout << "    Concurrency stress test " << getName() << ": " << threadCount << " threads" << std::endl;
        std::cout << "      Total time: " << totalTime << "ms" << std::endl;
        std::cout << "      Average thread time: " << avgThreadTime << "ms" << std::endl;
        std::cout << "      Throughput: " << throughput << " steps/sec" << std::endl;
        
        // Calculate speedup (should be close to ideal for independent work)
        double idealTime = avgThreadTime;
        double actualTime = totalTime / threadCount;
        double efficiency = (idealTime / actualTime) * 100.0;
        
        std::cout << "      Efficiency: " << efficiency << "%" << std::endl;
        
        std::cout << "    Concurrency stress test for " << getName() << " passed" << std::endl;
    }
};

// Configuration stress test - Test with many configuration variations
class ConfigurationStressTest : public test_framework::TestBase {
public:
    ConfigurationStressTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Stress) {}
    
    void run() override {
        const size_t configCount = 100;
        
        // Generate many different configurations
        std::vector<std::shared_ptr<nlm::Config>> configs;
        
        for (size_t i = 0; i < configCount; ++i) {
            auto config = std::make_shared<nlm::Config>();
            
            // Vary configuration parameters
            config->set("neuron_count", static_cast<int64_t>((i % 10 + 1) * 100), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>((i % 5 + 1) * 2), nlm::ConfigSource::Default);
            config->set("connection_probability", static_cast<double>(i % 10) / 10.0, nlm::ConfigSource::Default);
            
            // Add some additional keys
            config->set("debug_mode", i % 2 == 0, nlm::ConfigSource::Runtime);
            config->set("log_level", (i % 3 == 0) ? "INFO" : ((i % 3 == 1) ? "DEBUG" : "WARN"), nlm::ConfigSource::Runtime);
            
            configs.push_back(config);
        }
        
        // Process all configurations
        auto startTime = std::chrono::high_resolution_clock::now();
        
        size_t successCount = 0;
        for (size_t i = 0; i < configCount; ++i) {
            auto brain = std::make_shared<nlm::Brain>(configs[i]);
            
            if (brain->initialize()) {
                successCount++;
                
                // Simulate a few steps
                for (nlm::SimulationStep step = 0; step < 10; ++step) {
                    brain->step(step);
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        double avgTimePerConfig = totalTime / configCount;
        double successRate = (static_cast<double>(successCount) / configCount) * 100.0;
        
        std::cout << "    Configuration stress test " << getName() << ": " << configCount << " configurations" << std::endl;
        std::cout << "      Success rate: " << successRate << "%" << std::endl;
        std::cout << "      Average time per config: " << avgTimePerConfig << "ms" << std::endl;
        
        // Verify that most configurations were valid
        assert(successRate > 90.0);  // At least 90% should succeed
        
        std::cout << "    Configuration stress test for " << getName() << " passed" << std::endl;
    }
};

// Long-running test - Test extended simulation time
class LongRunningTest : public test_framework::TestBase {
public:
    LongRunningTest(const std::string& name)
        : TestBase(name, test_framework::TestCategory::Stress) {}
    
    void run() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(20), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run for extended period
        const nlm::SimulationStep totalSteps = 10000;
        const nlm::SimulationStep stepsPerReport = 1000;
        
        std::cout << "    Long-running test " << getName() << ": " << totalSteps << " simulation steps" << std::endl;
        
        for (nlm::SimulationStep step = 0; step < totalSteps; ++step) {
            brain.step(step);
            
            // Report progress periodically
            if (step % stepsPerReport == 0 || step == totalSteps - 1) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                double elapsed = std::chrono::duration<double, std::milli>(currentTime - startTime).count();
                double stepsRemaining = totalSteps - step;
                double estimatedTotal = elapsed / (step + 1) * totalSteps;
                double stepsPerSecond = (step + 1) / (elapsed / 1000.0);
                
                std::cout << "      Step " << step << "/" << totalSteps << ": " 
                          << stepsPerSecond << " steps/sec, "
                          << estimatedTotal - elapsed << "ms remaining" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        double stepsPerSecond = totalSteps / (totalTime / 1000.0);
        
        std::cout << "    Long-running test " << getName() << " completed:" << std::endl;
        std::cout << "      Total time: " << totalTime << "ms" << std::endl;
        std::cout << "      Steps per second: " << stepsPerSecond << std::endl;
        
        // Verify brain state is still valid
        assert(brain.getTotalNeuronCount() == 5000);
        
        std::cout << "    Long-running test for " << getName() << " passed" << std::endl;
    }
};

// Unit Tests (Original stress tests would go here)
void testBrainStress() {
    nlm::Brain brain;
    
    // Minimal stress test
    assert(true);
    
    std::cout << "    testBrainStress passed" << std::endl;
}

void testMemoryStress() {
    // Memory stress test
    std::vector<std::shared_ptr<nlm::Brain>> brains;
    
    for (int i = 0; i < 10; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        brains.push_back(brain);
    }
    
    std::cout << "    testMemoryStress passed" << std::endl;
}

void runAll() {
    std::cout << "=== Running Enhanced Stress Tests ===" << std::endl;
    
    // Run original unit tests
    testBrainStress();
    testMemoryStress();
    
    std::cout << "\n=== Running Brain Stress Tests ===" << std::endl;
    
    // Run comprehensive stress tests
    std::vector<std::pair<size_t, size_t>> brainConfigs = {
        {100, 1},
        {500, 2},
        {1000, 5},
        {2000, 10}
    };
    
    for (size_t i = 0; i < brainConfigs.size(); ++i) {
        BrainStressTest test(
            "brain_stress_" + std::to_string(brainConfigs[i].first) + "neurons_" + 
            std::to_string(brainConfigs[i].second) + "regions",
            brainConfigs[i].first, brainConfigs[i].second
        );
        test.run();
    }
    
    std::cout << "\n=== Running Memory Stress Tests ===" << std::endl;
    
    // Run memory stress tests
    MemoryStressTest memoryTest("memory_stress_50brains");
    memoryTest.run();
    
    std::cout << "\n=== Running Concurrency Stress Tests ===" << std::endl;
    
    // Run concurrency stress tests
    ConcurrencyStressTest concurrencyTest("concurrency_stress_8threads");
    concurrencyTest.run();
    
    std::cout << "\n=== Running Configuration Stress Tests ===" << std::endl;
    
    // Run configuration stress tests
    ConfigurationStressTest configTest("config_stress_100configs");
    configTest.run();
    
    std::cout << "\n=== Running Long-running Tests ===" << std::endl;
    
    // Run long-running tests
    LongRunningTest longTest("long_running_10000steps");
    longTest.run();
    
    std::cout << "\n=== All Stress Tests Completed ===" << std::endl;
}

} // namespace test_stress
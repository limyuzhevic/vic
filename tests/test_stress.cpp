// Stress Tests
// Tests for large-scale neural network scenarios
#pragma once

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <algorithm>
#include <cmath>
#include <map>
#include "test_framework.hpp"

namespace test_stress {

// Stress test base class
class StressTestBase : public nlm::test::TestBase {
protected:
    nlm::test::TestTimer::Timer timer;
    
public:
    StressTestBase() {
        timer.start();
    }
    
    std::string getName() const override {
        return "StressTestBase";
    }
    
    std::string getDescription() const override {
        return "Base class for stress tests";
    }
    
    nlm::test::TestCategory getCategory() const override {
        return nlm::test::TestCategory::Stress;
    }
    
    std::string getFile() const override {
        return "tests/test_stress.cpp";
    }
    
    int getLine() const override {
        return 1;
    }
};

// Large brain networks stress test
void testLargeBrainNetworks() {
    std::vector<std::pair<int, int>> networkSizes = {
        {500, 2},    // 500 neurons, 2 regions
        {1000, 4},   // 1000 neurons, 4 regions  
        {2000, 8},   // 2000 neurons, 8 regions
        {5000, 16}   // 5000 neurons, 16 regions
    };
    
    for (const auto& size : networkSizes) {
        int neuronCount = size.first;
        int regionCount = size.second;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.05, nlm::ConfigSource::Default); // Lower probability for large networks
        
        nlm::Brain brain(config);
        auto startTime = std::chrono::high_resolution_clock::now();
        
        bool success = brain.initialize();
        assert(success);
        
        // Stress test with many steps
        for (int step = 0; step < 200; ++step) {
            brain.step(step);
            if (step % 50 == 0) {
                std::cout << "    testLargeBrainNetworks - " 
                          << "" << neuronCount << " neurons: step " << step << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        std::cout << "    testLargeBrainNetworks - " 
                  << "" << neuronCount << " neurons: " << duration << "ms total, "
                  << "" << (200 * neuronCount) / (duration / 1000.0) << " neurons/sec" << std::endl;
    }
    
    std::cout << "    testLargeBrainNetworks passed" << std::endl;
}

// High plasticity rates stress test
void testHighPlasticityRates() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    config->set("plasticity_rate", 0.5, nlm::ConfigSource::Default); // High plasticity rate
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Perform many plasticity operations (STDP)
    nlm::STDP stdp(config);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 1000; ++step) {
        // Simulate high-frequency spikes
        for (int neuronId = 0; neuronId < 100; neuronId += 2) {
            // This would normally involve actual spike events
            // For stress test, we'll just trigger plasticity operations
            stdp.update(brain.step(step), step);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    std::cout << "    testHighPlasticityRates - " 
              << "" << 1000 << " steps completed in " << duration << "ms" << std::endl;
    
    std::cout << "    testHighPlasticityRates passed" << std::endl;
}

// Complex learning scenarios stress test
void testComplexLearningScenarios() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200, nlm::ConfigSource::Default);
    config->set("region_count", 3, nlm::ConfigSource::Default);
    config->set("learning_rate", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Complex learning scenario with multiple inputs and outputs
    std::vector<std::vector<double>> inputPatterns = {
        {0.1, 0.2, 0.3, 0.4, 0.5},
        {0.9, 0.8, 0.7, 0.6, 0.5},
        {0.2, 0.4, 0.6, 0.8, 1.0},
        {1.0, 0.9, 0.8, 0.7, 0.6},
        {0.5, 0.5, 0.5, 0.5, 0.5}
    };
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int episode = 0; episode < 500; ++episode) {
        // Select random input pattern
        auto pattern = inputPatterns[episode % inputPatterns.size()];
        
        // Run brain for 50 steps with this pattern
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
            
            // Simulate external input
            auto action = brain.produceAction();
            // Process action (would normally involve reward learning)
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    std::cout << "    testComplexLearningScenarios - " 
              << "" << 500 << " episodes completed in " << duration << "ms" << std::endl;
    
    std::cout << "    testComplexLearningScenarios passed" << std::endl;
}

// Extended simulation times stress test
void testExtendedSimulationTimes() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Extended simulation (would normally be hours in real scenarios)
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 10000; ++step) {
        brain.step(step);
        
        if (step % 1000 == 0) {
            std::cout << "    testExtendedSimulationTimes - " 
                      << "" << step << " steps completed" << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    double stepsPerSecond = (10000.0 / duration) * 1000.0; // Convert to steps per second
    
    std::cout << "    testExtendedSimulationTimes - " 
              << "" << 10000 << " steps in " << duration << "ms ("
              << stepsPerSecond << " steps/sec)" << std::endl;
    
    // Verify brain is still stable
    assert(brain.getTotalNeuronCount() == 50);
    assert(brain.getRegionCount() == 2);
    
    std::cout << "    testExtendedSimulationTimes passed" << std::endl;
}

// Concurrent stress test
void testConcurrentStress() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 2, nlm::ConfigSource::Default);
    
    std::vector<std::thread> threads;
    std::vector<nlm::Brain> brains;
    
    // Create multiple brain instances for concurrent stress testing
    for (int i = 0; i < 8; ++i) {
        brains.emplace_back(*config);
        assert(brains.back().initialize());
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run concurrent stress tests
    for (int i = 0; i < 8; ++i) {
        threads.emplace_back([&, i]() {
            for (int step = 0; step < 200; ++step) {
                brains[i].step(step);
                
                if (step % 50 == 0) {
                    std::cout << "    testConcurrentStress - " 
                              << "" << i << ": step " << step << std::endl;
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    std::cout << "    testConcurrentStress - " 
              << "" << 8 << " brains completed in " << duration << "ms" << std::endl;
    
    // Verify all brains are still functional
    for (size_t i = 0; i < brains.size(); ++i) {
        assert(brains[i].getTotalNeuronCount() == 100);
        assert(brains[i].getRegionCount() == 2);
    }
    
    std::cout << "    testConcurrentStress passed" << std::endl;
}

// Run all stress tests
void runAll() {
    testLargeBrainNetworks();
    testHighPlasticityRates();
    testComplexLearningScenarios();
    testExtendedSimulationTimes();
    testConcurrentStress();
}

} // namespace test_stress

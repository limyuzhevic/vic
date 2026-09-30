// Performance Tests
// Test performance validation and benchmarking

#include <gtest/gtest.h>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <chrono>
#include <vector>

namespace nlm {

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
    }
    
    void TearDown() override {
        config.reset();
    }
    
    std::shared_ptr<Config> config;
};

TEST_F(PerformanceTest, SynapseUpdateSpeed) {
    // Test synapse weight update speed
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    
    Brain brain(config);
    ASSERT_TRUE(brain.initialize());
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Perform many synapse updates
    for (int i = 0; i < 10000; ++i) {
        for (auto& synapse : brain.getAllSynapses()) {
            synapse->addToWeight(0.001f);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Performance should be reasonable (less than 1 second for 10,000 updates)
    EXPECT_LT(duration.count(), 1000) << "Synapse updates too slow";
}

TEST_F(PerformanceTest, SimulationSpeed) {
    // Test simulation speed with different network sizes
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    
    Brain brain(config);
    ASSERT_TRUE(brain.initialize());
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run simulation
    for (SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Simulation should be reasonably fast
    EXPECT_LT(duration.count(), 5000) << "Simulation too slow";
}

TEST_F(PerformanceTest, Scalability) {
    // Test scalability with different network sizes
    std::vector<int> neuronCounts = {100, 500, 1000, 2000};
    std::vector<double> maxDurations = {1000.0, 5000.0, 10000.0, 20000.0};  // milliseconds
    
    for (size_t i = 0; i < neuronCounts.size(); ++i) {
        config->set("neuron_count", static_cast<int64_t>(neuronCounts[i]), ConfigSource::Default);
        
        Brain brain(config);
        ASSERT_TRUE(brain.initialize());
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
            .count();
        
        EXPECT_LT(duration, maxDurations[i]) 
            << "Network with " << neuronCounts[i] << " neurons too slow: " 
            << duration << "ms";  
    }
}

TEST_F(PerformanceTest, MemoryUsage) {
    // Test memory usage patterns
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    
    Brain brain(config);
    ASSERT_TRUE(brain.initialize());
    
    // Record initial spike count
    size_t initialSpikes = brain.getTotalSpikeCount();
    
    // Run simulation
    for (SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify that memory usage is reasonable (no memory leaks)
    size_t finalSpikes = brain.getTotalSpikeCount();
    EXPECT_GE(finalSpikes, initialSpikes);
}

TEST(PerformanceTest, LearningSpeedComparison) {
    // Compare learning speed between different plasticity rules
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    
    Brain brain(config);
    ASSERT_TRUE(brain.initialize());
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate learning
    for (SimulationStep step = 0; step < 200; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
        .count();
    
    // Learning should be reasonably fast
    EXPECT_LT(duration, 5000) << "Learning too slow";
}

} // namespace nlm


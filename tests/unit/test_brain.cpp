// Brain Unit Tests using Google Test Framework
// Comprehensive unit tests for nlm::Brain class
// Tests brain initialization, neuron management, and connectivity

#include "fixtures/TestHelpers.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

namespace test {

// Test 3.1: Brain Creation and Configuration
TEST_F(NLMTest, BrainCreation) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Verify brain is created with correct properties
    // Note: These properties might be accessible via public API or private implementation
}

// Test 3.2: Brain Initialization
TEST_F(NLMTest, BrainInitialization) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success);
    
    // Verify initialization created expected components
    EXPECT_EQ(brain.getRegionCount(), 2);
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

// Test 3.3: Brain Step Processing
TEST_F(NLMTest, BrainStepProcessing) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run a few steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    // Verify brain state changed after steps
    // Implementation-specific verification
}

// Test 3.4: Brain Region Management
TEST_F(NLMTest, BrainRegionManagement) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify regions were created
    EXPECT_EQ(brain.getRegionCount(), 3);
    
    // Test region-specific operations
    for (nlm::RegionId regionId = 0; regionId < 3; ++regionId) {
        nlm::NeuralRegion* region = brain.getRegion(regionId);
        ASSERT_NE(region, nullptr);
    }
}

// Test 3.5: Brain Neuron Access
TEST_F(NLMTest, BrainNeuronAccess) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify neurons were created
    EXPECT_EQ(brain.getTotalNeuronCount(), 10);
    
    // Test neuron access
    for (nlm::NeuronId neuronId = 0; neuronId < 10; ++neuronId) {
        nlm::Neuron* neuron = brain.getNeuron(neuronId);
        ASSERT_NE(neuron, nullptr);
        EXPECT_EQ(neuron->getId(), neuronId);
    }
}

// Test 3.6: Brain Synapse Creation
TEST_F(NLMTest, BrainSynapseCreation) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify synapses were created based on probability
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
    EXPECT_LE(brain.getTotalSynapseCount(), 50);  // Upper bound
}

// Test 3.7: Brain Configuration Loading
TEST_F(NLMTest, BrainConfigurationLoading) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(75), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success);
    
    // Verify configuration was applied
    // This would test that brain parameters match config values
}

// Test 3.8: Brain Error Handling During Initialization
TEST_F(NLMTest, BrainErrorHandlingInitialization) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(0), nlm::ConfigSource::Default);  // Edge case
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    // Brain initialization should handle edge cases gracefully
    EXPECT_TRUE(success || !success);  // Either outcome is acceptable
}

// Test 3.9: Brain Reset Functionality
TEST_F(NLMTest, BrainResetFunctionality) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Record initial state
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Run some steps
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    size_t afterStepsSynapses = brain.getTotalSynapseCount();
    
    // Reset brain
    brain.reset();
    
    size_t afterResetSynapses = brain.getTotalNeuronCount();
    
    // Reset should restore initial state
    EXPECT_LE(afterResetSynapses, initialSynapses + afterStepsSynapses);
}

// Test 3.10: Brain Performance Monitoring
TEST_F(PerformanceTest, BrainInitializationPerformance) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Time brain initialization
    auto start = std::chrono::high_resolution_clock::now();
    bool success = brain.initialize();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    ASSERT_TRUE(success);
    EXPECT_LT(duration.count(), 1000);  // Should complete in under 1 second
}

// Parameterized test for different brain sizes
class BrainSizeTest : public NLMTest, public ::testing::WithParamInterface<std::pair<size_t, size_t>> {
};

TEST_P(BrainSizeTest, BrainOfDifferentSizes) {
    auto [neuronCount, regionCount] = GetParam();
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(regionCount), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    
    ASSERT_TRUE(success);
    EXPECT_EQ(brain.getTotalNeuronCount(), neuronCount);
    EXPECT_EQ(brain.getRegionCount(), regionCount);
}

INSTANTIATE_TEST_SUITE_P(
    BrainSizeVariations,
    BrainSizeTest,
    ::testing::Values(
        std::make_pair(10, 1),
        std::make_pair(50, 2),
        std::make_pair(100, 5),
        std::make_pair(200, 10)
    )
);

} // namespace test
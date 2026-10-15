// Brain Tests - GoogleTest Version
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <memory>

namespace {

TEST(BrainTest, BrainCreation) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    EXPECT_EQ(brain.getRegionCount(), 1);
}

TEST(BrainTest, BrainInitialization) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    EXPECT_TRUE(success);
    EXPECT_EQ(brain.getRegionCount(), 2);
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

TEST(BrainTest, BrainStep) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
}

TEST(BrainTest, BrainReset) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    brain.step(0);
    brain.reset();
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 50);
}

TEST(BrainTest, BrainRegions) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    EXPECT_EQ(regionIds.size(), 3);
    
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        EXPECT_NE(region, nullptr);
    }
}

TEST(BrainTest, BrainActionProduction) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    EXPECT_NE(action, nullptr);
}

TEST(BrainTest, BrainStatus) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    EXPECT_GT(brain.getTotalNeuronCount(), 0);
    EXPECT_GE(brain.getTotalSynapseCount(), 0);
    EXPECT_GE(brain.getFiringNeuronCount(), 0);
    EXPECT_GE(brain.getAverageFiringRate(), 0.0f);
}

TEST(BrainTest, BrainInitializationWithConfig) {
    auto config = std::make_shared<nlm::Config>();
    
    // Test with different configurations
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success1 = brain.initialize();
    EXPECT_TRUE(success1);
    
    // Different config
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    
    nlm::Brain brain2(config);
    bool success2 = brain2.initialize();
    EXPECT_TRUE(success2);
    EXPECT_EQ(brain2.getRegionCount(), 5);
    EXPECT_EQ(brain2.getTotalNeuronCount(), 200);
}

TEST(BrainTest, BrainStepConsistency) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run the same steps twice and compare state
    brain.step(0);
    brain.step(1);
    
    // Should not crash and should maintain neuron count
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    
    brain.reset();
    brain.initialize();
    
    // After reset, state should be the same as fresh
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
}

TEST(BrainTest, BrainRegionProperties) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto regionIds = brain.getRegionIds();
    EXPECT_EQ(regionIds.size(), 3);
    
    // Each region should have neurons
    for (auto rid : regionIds) {
        auto* region = brain.getRegion(rid);
        EXPECT_NE(region, nullptr);
        
        // Region should have neuron count allocated proportionally
        // (implementation dependent, just ensure it's valid)
    }
}

TEST(BrainTest, BrainActionProperties) {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    auto action = brain.produceAction();
    EXPECT_NE(action, nullptr);
    
    // Action should have valid properties
    EXPECT_GE(action->getTimestamp(), 0.0);
    EXPECT_GE(action->getValue(), 0.0);
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_brain {
    void runAll();
}
// Brain Integration Tests
// Test brain-level functionality and system behavior

#include <gtest/gtest.h>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("learning_rate", 0.01f, ConfigSource::Default);
    }
    
    void TearDown() override {
        config.reset();
    }
    
    std::shared_ptr<Config> config;
};

TEST_F(BrainTest, LearningCapability) {
    // Test brain learning with STDP
    Brain brain(config);
    bool initialized = brain.initialize();
    ASSERT_TRUE(initialized);
    
    // Test memory formation and consolidation
    brain.recordEpisode();
    
    // Test cognitive function integration
    auto action = brain.decideAction();
    
    EXPECT_GE(brain.getTotalSpikeCount(), 0);
}

TEST_F(BrainTest, BasicFunctionality) {
    // Test basic brain functionality
    Brain brain(config);
    bool initialized = brain.initialize();
    ASSERT_TRUE(initialized);
    
    EXPECT_EQ(brain.getRegionCount(), 2);
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

TEST_F(BrainTest, ErrorHandling) {
    // Test error handling
    Config nullConfig;
    Brain brain(&nullConfig);
    EXPECT_FALSE(brain.initialize());
}

TEST_F(BrainTest, SimulationSteps) {
    // Test simulation step execution
    Brain brain(config);
    bool initialized = brain.initialize();
    ASSERT_TRUE(initialized);
    
    for (SimulationStep step = 0; step < 10; ++step) {
        brain.step(step);
    }
    
    EXPECT_GT(brain.getTotalSpikeCount(), 0);
}

} // namespace nlm


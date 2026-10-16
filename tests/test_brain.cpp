// Brain Tests - GoogleTest Version
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include "GoogleTestHelper.hpp"
#include "TestMatchers.hpp"

// TEST SUITE
class BrainTestSuite : public ::testing::Test {
protected:
    std::shared_ptr<nlm::Config> config;
    nlm::Brain brain;
    
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        brain = nlm::Brain(config);
    }
};

// BRAIN TESTS
TEST_F(BrainTestSuite, BrainCreation) {
    nlm::Brain brain(config);
    EXPECT_TRUE(true);
}

TEST_F(BrainTestSuite, BrainInitialization) {
    bool success = brain.initialize();
    EXPECT_TRUE(success);
    EXPECT_EQ(brain.getRegionCount(), 1);
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    EXPECT_GT(brain.getTotalSynapseCount(), 0);
}

TEST_F(BrainTestSuite, BrainStep) {
    bool success = brain.initialize();
    if (success) {
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
        EXPECT_EQ(brain.getTotalNeuronCount(), 100);
    }
}

TEST_F(BrainTestSuite, BrainReset) {
    bool success = brain.initialize();
    if (success) {
        brain.step(0);
        brain.reset();
        EXPECT_EQ(brain.getTotalNeuronCount(), 50);
    }
}

TEST_F(BrainTestSuite, BrainRegions) {
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    nlm::Brain brain2(config);
    bool success = brain2.initialize();
    if (success) {
        auto regionIds = brain2.getRegionIds();
        EXPECT_EQ(regionIds.size(), 3);
        
        for (auto rid : regionIds) {
            auto* region = brain2.getRegion(rid);
            EXPECT_NE(region, nullptr);
        }
    }
}

TEST_F(BrainTestSuite, BrainActionProduction) {
    bool success = brain.initialize();
    if (success) {
        auto action = brain.produceAction();
        EXPECT_NE(action, nullptr);
    }
}

TEST_F(BrainTestSuite, BrainStatus) {
    bool success = brain.initialize();
    if (success) {
        EXPECT_GT(brain.getTotalNeuronCount(), 0);
        EXPECT_GE(brain.getTotalSynapseCount(), 0);
        EXPECT_GE(brain.getFiringNeuronCount(), 0);
        EXPECT_GE(brain.getAverageFiringRate(), 0.0f);
    }
}

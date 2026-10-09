#include <gtest/gtest.h>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <vector>

namespace test_brain {

class BrainTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.5, nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        
        brain = std::make_unique<nlm::Brain>(config);
        brain->initialize();
    }

    void TearDown() override {
        brain.reset();
        config.reset();
    }

    std::shared_ptr<nlm::Config> config;
    std::unique_ptr<nlm::Brain> brain;
};

TEST_F(BrainTestFixture, BrainCreationWithConfig) {
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_EQ(brain->getRegionCount(), 1);
    EXPECT_GT(brain->getTotalSynapseCount(), 0);
}

TEST_F(BrainTestFixture, BrainInitializationSuccess) {
    EXPECT_TRUE(brain->initialize());
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
}

TEST_F(BrainTestFixture, BrainStepExecution) {
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->step(step);
    }
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
}

TEST_F(BrainTestFixture, BrainResetFunctionality) {
    brain->step(5);
    brain->reset();
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
}

TEST_F(BrainTestFixture, BrainRegionManagement) {
    auto regionIds = brain->getRegionIds();
    EXPECT_EQ(regionIds.size(), 1);
    
    for (auto rid : regionIds) {
        EXPECT_NE(brain->getRegion(rid), nullptr);
    }
}

TEST_F(BrainTestFixture, BrainActionProduction) {
    auto action = brain->produceAction();
    EXPECT_NE(action, nullptr);
}

TEST_F(BrainTestFixture, BrainStatusInformation) {
    EXPECT_GT(brain->getTotalNeuronCount(), 0);
    EXPECT_GE(brain->getTotalSynapseCount(), 0);
    EXPECT_GE(brain->getFiringNeuronCount(), 0);
    EXPECT_GE(brain->getAverageFiringRate(), 0.0f);
}

TEST_F(BrainTestFixture, BrainSimulationStepSequence) {
    nlm::SimulationStep initialStep = 0;
    brain->step(initialStep);
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    
    for (nlm::SimulationStep step = 1; step < 5; ++step) {
        brain->step(step);
    }
}

TEST_F(BrainTestFixture, MultipleBrainInstances) {
    auto config2 = std::make_shared<nlm::Config>();
    config2->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    auto brain2 = std::make_unique<nlm::Brain>(config2);
    brain2->initialize();
    
    EXPECT_EQ(brain->getTotalNeuronCount(), 100);
    EXPECT_EQ(brain2->getTotalNeuronCount(), 50);
    EXPECT_EQ(brain2->getRegionCount(), 2);
}

TEST_F(BrainTestFixture, BrainConfigurationValidation) {
    EXPECT_TRUE(config->getOr<int64_t>("neuron_count", 0) > 0);
    EXPECT_TRUE(config->getOr<int64_t>("region_count", 0) > 0);
    EXPECT_GT(config->getOr<double>("connection_probability", 0.0), 0.0);
}

} // namespace test_brain
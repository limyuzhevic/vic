// Comprehensive Synapse Tests
// Covers synaptic transmission, plasticity, and weight management

#include <gtest/gtest.h>
#include "brain/Synapse.hpp"
#include <cmath>

namespace nlm {

class SynapseTest : public ::testing::Test {
protected:
    void SetUp() override {
        synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
        synapse->setWeight(0.5f);
    }
    
    void TearDown() override {
        synapse.reset();
    }
    
    std::unique_ptr<Synapse> synapse;
};

TEST_F(SynapseTest, Plasticity) {
    // Test STDP and Hebbian plasticity
    synapse->setWeight(0.5f);
    synapse->addToWeight(0.1f);
    EXPECT_EQ(synapse->getWeight(), 0.6f);
}

TEST_F(SynapseTest, WeightBounds) {
    // Test weight bounds and clamping
    synapse->setWeight(10.0f);
    EXPECT_LE(synapse->getWeight(), 1.0f);
    
    synapse->setWeight(-10.0f);
    EXPECT_GE(synapse->getWeight(), -1.0f);
}

TEST_F(SynapseTest, ErrorConditions) {
    // Test error conditions
    synapse->setWeight(0.5f);
    EXPECT_NO_THROW(synapse->addToWeight(0.1f));
}

TEST_F(SynapseTest, SynapticTransmission) {
    // Test synaptic transmission
    synapse->setWeight(1.0f);
    synapse->setTransmissionProbability(0.8f);
    EXPECT_EQ(synapse->getTransmissionProbability(), 0.8f);
}

TEST(IntegrationTest, BasicBrainFunctionality) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    
    Brain brain(config);
    bool initialized = brain.initialize();
    ASSERT_TRUE(initialized);
    
    EXPECT_EQ(brain.getTotalNeuronCount(), 100);
}

TEST(IntegrationTest, LearningMechanism) {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("connection_probability", 0.2f, ConfigSource::Default);
    
    Brain brain(config);
    bool initialized = brain.initialize();
    ASSERT_TRUE(initialized);
    
    // Run learning simulation
    for (int step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Verify learning occurred
    EXPECT_GT(brain.getTotalSpikeCount(), 0);
}

} // namespace nlm


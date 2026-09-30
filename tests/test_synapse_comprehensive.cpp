// Comprehensive Synapse Unit Tests
// File: test_synapse_comprehensive.cpp
// Coverage: Synapse.cpp, Synapse.hpp

#include <gtest/gtest.h>
#include "brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

class SynapseComprehensiveTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a synapse between two neurons
        synapse = std::make_unique<Synapse>(SynapseId(1), NeuronId(1), NeuronId(2));
        synapse->setWeight(0.5f);
        synapse->setType(SynapticType::Excitatory);
    }
    
    void TearDown() override {
        synapse.reset();
    }
    
    std::unique_ptr<Synapse> synapse;
};

TEST_F(SynapseComprehensiveTest, BasicCreation) {
    // Test synapse creation and basic properties
    EXPECT_EQ(synapse->getId(), SynapseId(1));
    EXPECT_EQ(synapse->getSourceNeuron(), NeuronId(1));
    EXPECT_EQ(synapse->getDestinationNeuron(), NeuronId(2));
    EXPECT_EQ(synapse->getWeight(), 0.5f);
    EXPECT_EQ(synapse->getType(), SynapticType::Excitatory);
}

TEST_F(SynapseComprehensiveTest, WeightManagement) {
    // Test weight management
    EXPECT_EQ(synapse->getWeight(), 0.5f);
    
    // Test setting weight
    synapse->setWeight(0.8f);
    EXPECT_EQ(synapse->getWeight(), 0.8f);
    
    // Test adding to weight
    synapse->addToWeight(0.1f);
    EXPECT_EQ(synapse->getWeight(), 0.9f);
    
    synapse->addToWeight(-0.3f);
    EXPECT_EQ(synapse->getWeight(), 0.6f);
}

TEST_F(SynapseComprehensiveTest, WeightBoundsClamping) {
    // Test weight bounds clamping
    // Test upper bound
    synapse->setWeight(10.0f);
    EXPECT_LE(synapse->getWeight(), 1.0f);
    
    // Test lower bound
    synapse->setWeight(-10.0f);
    EXPECT_GE(synapse->getWeight(), -1.0f);
    
    // Test valid weight
    synapse->setWeight(0.0f);
    EXPECT_EQ(synapse->getWeight(), 0.0f);
}

TEST_F(SynapseComprehensiveTest, SynapticEfficacy) {
    // Test synaptic efficacy management
    EXPECT_EQ(synapse->getEfficacy(), 1.0f);
    
    synapse->setEfficacy(0.7f);
    EXPECT_EQ(synapse->getEfficacy(), 0.7f);
    
    synapse->setEfficacy(1.5f);
    EXPECT_LE(synapse->getEfficacy(), 1.0f);
}

TEST_F(SynapseComprehensiveTest, DelayManagement) {
    // Test synaptic delay management
    EXPECT_EQ(synapse->getDelay(), 1.0f);  // Default delay
    
    synapse->setDelay(5.0f);
    EXPECT_EQ(synapse->getDelay(), 5.0f);
    
    synapse->setDelay(0.0f);
    EXPECT_EQ(synapse->getDelay(), 0.0f);
}

TEST_F(SynapseComprehensiveTest, TypeManagement) {
    // Test synaptic type management
    EXPECT_EQ(synapse->getType(), SynapticType::Excitatory);
    
    synapse->setType(SynapticType::Inhibitory);
    EXPECT_EQ(synapse->getType(), SynapticType::Inhibitory);
    
    synapse->setType(SynapticType::Modulatory);
    EXPECT_EQ(synapse->getType(), SynapticType::Modulatory);
}

TEST_F(SynapseComprehensiveTest, PlasticityState) {
    // Test synaptic plasticity state
    EXPECT_FALSE(synapse->isPlastic());
    
    synapse->setPlastic(true);
    EXPECT_TRUE(synapse->isPlastic());
    
    synapse->setPlastic(false);
    EXPECT_FALSE(synapse->isPlastic());
}

TEST_F(SynapseComprehensiveTest, StrengthSaturation) {
    // Test strength saturation
    EXPECT_EQ(synapse->getStrengthSaturation(), 1.0f);
    
    synapse->setStrengthSaturation(0.8f);
    EXPECT_EQ(synapse->getStrengthSaturation(), 0.8f);
}

TEST_F(SynapseComprehensiveTest, NormalizedWeight) {
    // Test normalized weight (0-1 range)
    EXPECT_GE(synapse->getNormalizedWeight(), 0.0f);
    EXPECT_LE(synapse->getNormalizedWeight(), 1.0f);
    
    synapse->setWeight(-0.5f);
    EXPECT_EQ(synapse->getNormalizedWeight(), 0.0f);
    
    synapse->setWeight(0.5f);
    EXPECT_EQ(synapse->getNormalizedWeight(), 0.5f);
    
    synapse->setWeight(2.0f);
    EXPECT_EQ(synapse->getNormalizedWeight(), 1.0f);
}

TEST_F(SynapseComprehensiveTest, EffectiveConductance) {
    // Test effective conductance calculation
    float expectedConductance = synapse->getWeight() * synapse->getEfficacy();
    EXPECT_NEAR(synapse->getEffectiveConductance(), expectedConductance, 0.001f);
}

TEST_F(SynapseComprehensiveTest, TransmissionProbability) {
    // Test transmission probability
    EXPECT_GT(synapse->getTransmissionProbability(), 0.0f);
    EXPECT_LE(synapse->getTransmissionProbability(), 1.0f);
    
    synapse->setTransmissionProbability(0.3f);
    EXPECT_EQ(synapse->getTransmissionProbability(), 0.3f);
}

TEST_F(SynapseComprehensiveTest, ReleaseProbability) {
    // Test neurotransmitter release probability
    EXPECT_GT(synapse->getReleaseProbability(), 0.0f);
    EXPECT_LE(synapse->getReleaseProbability(), 1.0f);
    
    synapse->setReleaseProbability(0.8f);
    EXPECT_EQ(synapse->getReleaseProbability(), 0.8f);
}

TEST_F(SynapseComprehensiveTest, BackpropagationWeightChange) {
    // Test backpropagation weight change
    float initialWeight = synapse->getWeight();
    synapse->applyBackpropagationWeightChange(0.1f);
    EXPECT_EQ(synapse->getWeight(), initialWeight + 0.1f);
}

TEST_F(SynapseComprehensiveTest, EligibilityTrace) {
    // Test synaptic eligibility trace
    EXPECT_EQ(synapse->getEligibilityTrace(), 0.0f);
    
    synapse->updateEligibilityTrace(0.5f, 0.1f);
    EXPECT_GT(synapse->getEligibilityTrace(), 0.0f);
    
    synapse->updateEligibilityTrace(0.0f, 0.1f);
    // Trace should decay or change based on update
}

TEST_F(SynapseComprehensiveTest, TransmissionDelay) {
    // Test transmission delay effects
    float delay = synapse->getDelay();
    synapse->addTransmissionDelay(2.0f);
    EXPECT_EQ(synapse->getDelay(), delay + 2.0f);
}

TEST_F(SynapseComprehensiveTest, SynapticDynamics) {
    // Test synaptic dynamics parameters
    EXPECT_GT(synapse->getTimeConstant(), 0.0f);
    EXPECT_GT(synapse->getRiseTime(), 0.0f);
    EXPECT_GT(synapse->getDecayTime(), 0.0f);
}

TEST_F(SynapseComprehensiveTest, InputOutputState) {
    // Test input/output state management
    bool wasActive = synapse->getLastInputActive();
    synapse->setLastInputActive(true);
    EXPECT_TRUE(synapse->getLastInputActive());
    
    bool wasOutput = synapse->getLastOutputActive();
    synapse->setLastOutputActive(true);
    EXPECT_TRUE(synapse->getLastOutputActive());
}

TEST_F(SynapseComprehensiveTest, ActivityHistory) {
    // Test activity history tracking
    EXPECT_GE(synapse->getActivityHistory().size(), 0);
    
    // Record some activity
    synapse->recordActivity(100.0f, true);
    EXPECT_GT(synapse->getActivityCount(), 0);
}

TEST_F(SynapseComprehensiveTest, PlasticityRuleAssignment) {
    // Test plasticity rule assignment
    EXPECT_NE(synapse->getPlasticityRule(), nullptr);
    
    // Note: This depends on the specific implementation
    // For now, we just verify the synapse is functional
    synapse->setPlasticityRule(std::make_shared<STDP>());
    EXPECT_NE(synapse->getPlasticityRule(), nullptr);
}

TEST_F(SynapseComprehensiveTest, StatePersistence) {
    // Test state persistence across operations
    SynapseId id = synapse->getId();
    NeuronId source = synapse->getSourceNeuron();
    NeuronId dest = synapse->getDestinationNeuron();
    float weight = synapse->getWeight();
    
    // Apply various operations
    synapse->addToWeight(0.1f);
    synapse->setEfficacy(0.9f);
    synapse->setDelay(2.0f);
    
    // Check that ID, source, and destination are preserved
    EXPECT_EQ(synapse->getId(), id);
    EXPECT_EQ(synapse->getSourceNeuron(), source);
    EXPECT_EQ(synapse->getDestinationNeuron(), dest);
    EXPECT_NEAR(synapse->getWeight(), weight + 0.1f, 0.001f);
    EXPECT_EQ(synapse->getEfficacy(), 0.9f);
    EXPECT_EQ(synapse->getDelay(), 2.0f);
}

} // namespace nlm


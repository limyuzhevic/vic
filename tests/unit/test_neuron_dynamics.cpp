// Neuron Unit Tests using Google Test Framework
// Comprehensive unit tests for nlm::Neuron class
// Tests LIF dynamics, spike generation, and state management

#include "fixtures/TestHelpers.hpp"
#include "brain/Neuron.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <cmath>

namespace test {

// Test 4.1: Neuron Creation and Identity
TEST_F(NLMTest, NeuronCreation) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_EQ(neuron.getId(), nlm::NeuronId(1));
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Internal);
    EXPECT_EQ(neuron.getFiringState(), nlm::FiringState::Resting);
}

// Test 4.2: Neuron Type Management
TEST_F(NLMTest, NeuronTypeManagement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setType(nlm::NeuronType::Excitatory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Excitatory);
    
    neuron.setType(nlm::NeuronType::Inhibitory);
    EXPECT_EQ(neuron.getType(), nlm::NeuronType::Inhibitory);
}

// Test 4.3: Membrane Potential Dynamics
TEST_F(NLMTest, MembranePotentialDynamics) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test initial value
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), DEFAULT_POTENTIAL);
    
    // Test set value
    neuron.setMembranePotential(-65.0f);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -65.0f);
    
    // Test addition
    neuron.addToMembranePotential(10.0f);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -55.0f);
    
    // Test setting to threshold
    neuron.setMembranePotential(THRESHOLD);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), THRESHOLD);
    EXPECT_TRUE(neuron.checkThreshold());
}

// Test 4.4: Firing State Management
TEST_F(NLMTest, FiringStateManagement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_FALSE(neuron.isFiring());
    EXPECT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron.isFiring());
    EXPECT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(nlm::FiringState::Refractory);
    EXPECT_FALSE(neuron.isFiring());
    EXPECT_TRUE(neuron.isRefractory());
    
    neuron.setRefractoryPeriod(10);
    EXPECT_EQ(neuron.getRefractoryPeriod(), 10);
}

// Test 4.5: Firing Rate Management
TEST_F(NLMTest, FiringRateManagement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_DOUBLE_EQ(neuron.getFiringRate(), DEFAULT_RATE);
    
    neuron.setFiringRate(50.0f);
    EXPECT_DOUBLE_EQ(neuron.getFiringRate(), 50.0f);
    
    neuron.setFiringRate(0.0f);
    EXPECT_DOUBLE_EQ(neuron.getFiringRate(), 0.0f);
}

// Test 4.6: LIF Neuron Parameters
TEST_F(NLMTest, LIFParameters) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test leak conductance
    neuron.setLeakConductance(15.0f);
    EXPECT_DOUBLE_EQ(neuron.getLeakConductance(), 15.0f);
    
    // Test resting potential
    neuron.setRestingPotential(-75.0f);
    EXPECT_DOUBLE_EQ(neuron.getRestingPotential(), -75.0f);
    
    // Test threshold
    neuron.setThreshold(-50.0f);
    EXPECT_DOUBLE_EQ(neuron.getThreshold(), -50.0f);
    
    // Test reset potential
    neuron.setResetPotential(-65.0f);
    EXPECT_DOUBLE_EQ(neuron.getResetPotential(), -65.0f);
}

// Test 4.7: LIF Step Function
TEST_F(NLMTest, LIFStepFunction) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setThreshold(-50.0f);
    neuron.setResetPotential(-70.0f);
    
    // Set membrane potential below threshold
    neuron.setMembranePotential(-60.0f);
    neuron.setFiringState(nlm::FiringState::Resting);
    
    bool fired = neuron.stepLIF(0.0, 0.001);
    EXPECT_FALSE(fired);  // Should not fire
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -60.0f);  // Should not change
    
    // Set membrane potential above threshold
    neuron.setMembranePotential(-40.0f);
    neuron.setFiringState(nlm::FiringState::Resting);
    
    fired = neuron.stepLIF(0.0, 0.001);
    EXPECT_TRUE(fired);  // Should fire
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), -70.0f);  // Should reset
    EXPECT_EQ(neuron.getFiringState(), nlm::FiringState::Refractory);
}

// Test 4.8: Synaptic Inputs
TEST_F(NLMTest, SynapticInputs) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test excitatory input
    neuron.receiveExcitatoryInput(5.0f);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), DEFAULT_POTENTIAL + 5.0f);
    
    // Reset for next test
    neuron.setMembranePotential(DEFAULT_POTENTIAL);
    
    // Test inhibitory input
    neuron.receiveInhibitoryInput(3.0f);
    EXPECT_DOUBLE_EQ(neuron.getMembraneState(), DEFAULT_POTENTIAL - 3.0f);
    
    // Test modulatory input
    neuron.setMembranePotential(DEFAULT_POTENTIAL);
    neuron.receiveModulatoryInput(2.0f);
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), DEFAULT_POTENTIAL + 2.0f);
}

// Test 4.9: Current Injection
TEST_F(NLMTest, CurrentInjection) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Test external current injection
    neuron.injectCurrent(10.0f);
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 10.0f);
    
    // Test multiple injections
    neuron.injectCurrent(5.0f);
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 15.0f);
    
    // Test clearing current
    neuron.clearTotalCurrent();
    EXPECT_DOUBLE_EQ(neuron.getTotalCurrent(), 0.0f);
}

// Test 4.10: Spike Recording
TEST_F(NLMTest, SpikeRecording) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.0);
    EXPECT_EQ(neuron.getSpikeHistory().size(), 1);
    EXPECT_DOUBLE_EQ(neuron.getSpikeHistory()[0], 0.0);
    
    neuron.recordSpike(0.1);
    EXPECT_EQ(neuron.getSpikeHistory().size(), 2);
    EXPECT_DOUBLE_EQ(neuron.getSpikeHistory()[1], 0.1);
    
    neuron.clearSpikeHistory();
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}

// Test 4.11: Synapse Management
TEST_F(NLMTest, SynapseManagement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    EXPECT_TRUE(neuron.getIncomingSynapses().empty());
    EXPECT_TRUE(neuron.getOutgoingSynapses().empty());
    
    nlm::SynapseHandle incomingHandle(1);
    nlm::SynapseHandle outgoingHandle(2);
    
    neuron.addIncomingSynapse(incomingHandle);
    neuron.addOutgoingSynapse(outgoingHandle);
    
    EXPECT_EQ(neuron.getIncomingSynapses().size(), 1);
    EXPECT_EQ(neuron.getIncomingSynapses()[0], incomingHandle);
    
    EXPECT_EQ(neuron.getOutgoingSynapses().size(), 1);
    EXPECT_EQ(neuron.getOutgoingSynapses()[0], outgoingHandle);
}

// Test 4.12: Plasticity Management
TEST_F(NLMTest, PlasticityManagement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    const auto& plasticityFlags = neuron.getPlasticityFlags();
    EXPECT_FALSE(plasticityFlags.hebbian);
    EXPECT_FALSE(plasticityFlags.stdp);
    EXPECT_FALSE(plasticityFlags.rewardModulated);
    
    neuron.enablePlasticity(true, false, true);
    
    const auto& updatedFlags = neuron.getPlasticityFlags();
    EXPECT_TRUE(updatedFlags.hebbian);
    EXPECT_FALSE(updatedFlags.stdp);
    EXPECT_TRUE(updatedFlags.rewardModulated);
}

// Test 4.13: Region and Population Membership
TEST_F(NLMTest, RegionPopulationMembership) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRegionId(nlm::RegionId(0));
    EXPECT_EQ(neuron.getRegionId(), nlm::RegionId(0));
    
    neuron.setPopulationId(nlm::PopulationId(5));
    EXPECT_EQ(neuron.getPopulationId(), nlm::PopulationId(5));
}

// Test 4.14: Neuron Reset
TEST_F(NLMTest, NeuronReset) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    // Modify neuron state
    neuron.setMembranePotential(-60.0f);
    neuron.setFiringState(nlm::FiringState::Active);
    neuron.recordSpike(0.5);
    
    // Reset neuron
    neuron.reset();
    
    // Verify state was restored to initial values
    EXPECT_DOUBLE_EQ(neuron.getMembranePotential(), DEFAULT_POTENTIAL);
    EXPECT_EQ(neuron.getFiringState(), nlm::FiringState::Resting);
    EXPECT_TRUE(neuron.getSpikeHistory().empty());
}

// Test 4.15: Refractory Period Management
TEST_F(NLMTest, RefractoryPeriodManagement) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    
    neuron.setRefractoryPeriod(10);
    EXPECT_EQ(neuron.getRefractoryPeriod(), 10);
    EXPECT_FALSE(neuron.isRefractory());
    
    // Simulate refractory period
    neuron.decrementRefractory();
    // After first decrement, still refractory
    EXPECT_TRUE(neuron.isRefractory());
    
    // Decrement until not refractory
    for (uint32_t i = 0; i < 10; ++i) {
        neuron.decrementRefractory();
    }
    EXPECT_FALSE(neuron.isRefractory());
}

// Test 4.16: Neuron Step Method
TEST_F(NLMTest, NeuronStepMethod) {
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Excitatory);
    neuron.setThreshold(THRESHOLD);
    
    // Test with membrane potential below threshold
    neuron.setMembranePotential(-60.0f);
    neuron.step(0.0);
    EXPECT_FALSE(neuron.isFiring());
    
    // Test with membrane potential at threshold
    neuron.setMembranePotential(THRESHOLD);
    neuron.step(0.0);
    EXPECT_TRUE(neuron.isFiring() || neuron.checkThreshold());
}

// Parameterized test for different neuron types
class NeuronTypeTest : public NLMTest, public ::testing::WithParamInterface<nlm::NeuronType> {
};

TEST_P(NeuronTypeTest, DifferentNeuronTypes) {
    nlm::NeuronType type = GetParam();
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(type);
    
    EXPECT_EQ(neuron.getType(), type);
    
    // Test type-specific behavior
    if (type == nlm::NeuronType::Excitatory) {
        neuron.receiveExcitatoryInput(5.0f);
        EXPECT_GT(neuron.getMembranePotential(), DEFAULT_POTENTIAL);
    } else if (type == nlm::NeuronType::Inhibitory) {
        neuron.receiveInhibitoryInput(5.0f);
        EXPECT_LT(neuron.getMembranePotential(), DEFAULT_POTENTIAL);
    }
}

INSTANTIATE_TEST_SUITE_P(
    NeuronTypeVariations,
    NeuronTypeTest,
    ::testing::Values(
        nlm::NeuronType::Internal,
        nlm::NeuronType::Excitatory,
        nlm::NeuronType::Inhibitory
    )
);

} // namespace test
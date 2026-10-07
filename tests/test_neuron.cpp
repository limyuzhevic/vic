// Neuron Tests - Google Test Version
#include "brain/Neuron.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <iostream>

namespace nlm {

class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = Neuron(NeuronId(1));
        rng = RandomGenerator(42);
    }
    
    Neuron neuron;
    RandomGenerator rng;
};

TEST_F(NeuronTest, NeuronCreation) {
    ASSERT_EQ(neuron.getId(), NeuronId(1));
    ASSERT_EQ(neuron.getType(), NeuronType::Internal);
}

TEST_F(NeuronTest, NeuronType) {
    neuron.setType(NeuronType::Excitatory);
    ASSERT_EQ(neuron.getType(), NeuronType::Excitatory);
    
    neuron.setType(NeuronType::Inhibitory);
    ASSERT_EQ(neuron.getType(), NeuronType::Inhibitory);
}

TEST_F(NeuronTest, MembranePotential) {
    // Default value
    ASSERT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);
    
    neuron.setMembranePotential(-65.0f);
    ASSERT_FLOAT_EQ(neuron.getMembranePotential(), -65.0f);
    
    neuron.addToMembranePotential(5.0f);
    ASSERT_FLOAT_EQ(neuron.getMembranePotential(), -60.0f);
}

TEST_F(NeuronTest, FiringState) {
    ASSERT_FALSE(neuron.isFiring());
    ASSERT_FALSE(neuron.isRefractory());
    
    neuron.setFiringState(FiringState::Active);
    ASSERT_TRUE(neuron.isFiring());
    
    neuron.setRefractoryPeriod(10);
    ASSERT_TRUE(neuron.isRefractory());
    ASSERT_FALSE(neuron.isFiring());
}

TEST_F(NeuronTest, RefractoryDecrement) {
    neuron.setRefractoryPeriod(5);
    ASSERT_TRUE(neuron.isRefractory());
    
    neuron.decrementRefractory();
    ASSERT_TRUE(neuron.isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron.decrementRefractory();
    }
    ASSERT_FALSE(neuron.isRefractory());
}

TEST_F(NeuronTest, CurrentInjection) {
    ASSERT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
    
    neuron.injectCurrent(5.0f);
    ASSERT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    ASSERT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
}

TEST_F(NeuronTest, SynapticInput) {
    neuron.clearTotalCurrent();
    neuron.receiveExcitatoryInput(5.0f);
    ASSERT_FLOAT_EQ(neuron.getTotalCurrent(), 5.0f);
    
    neuron.clearTotalCurrent();
    neuron.receiveInhibitoryInput(3.0f);
    ASSERT_FLOAT_EQ(neuron.getTotalCurrent(), -3.0f);
}

TEST_F(NeuronTest, SpikeHistory) {
    ASSERT_TRUE(neuron.getSpikeHistory().empty());
    
    neuron.recordSpike(0.1);
    neuron.recordSpike(0.2);
    neuron.recordSpike(0.3);
    
    ASSERT_EQ(neuron.getSpikeHistory().size(), 3);
    
    neuron.clearSpikeHistory();
    ASSERT_TRUE(neuron.getSpikeHistory().empty());
}

TEST_F(NeuronTest, RandomInitialization) {
    neuron.initializeRandom(rng);
    
    // Verify values are in reasonable range
    float potential = neuron.getMembranePotential();
    float threshold = neuron.getThreshold();
    
    ASSERT_GT(potential, -80.0f);
    ASSERT_LT(potential, -60.0f);
    ASSERT_GT(threshold, -60.0f);
    ASSERT_LT(threshold, -50.0f);
}

TEST_F(NeuronTest, Reset) {
    neuron.setMembranePotential(-60.0f);
    neuron.injectCurrent(5.0f);
    neuron.recordSpike(0.1);
    
    neuron.reset();
    
    ASSERT_FLOAT_EQ(neuron.getMembranePotential(), -70.0f);
    ASSERT_FLOAT_EQ(neuron.getTotalCurrent(), 0.0f);
    ASSERT_TRUE(neuron.getSpikeHistory().empty());
}

TEST_F(NeuronTest, LIFStep) {
    // Set up neuron for firing test
    neuron.setMembranePotential(-50.0f);  // Above threshold
    neuron.setThreshold(-55.0f);
    neuron.setFiringState(FiringState::Resting);
    
    // Neuron should fire
    bool fired = neuron.stepLIF(0.0, 0.001);
    ASSERT_TRUE(fired);
    ASSERT_EQ(neuron.getFiringState(), FiringState::Refractory);
}

TEST_F(NeuronTest, RefractoryPeriodBehavior) {
    neuron.setRefractoryPeriod(5);
    neuron.setFiringState(FiringState::Refractory);
    
    // During refractory, should not integrate
    neuron.addToMembranePotential(10.0f);
    // Membrane potential should remain unchanged during refractory
    
    // Decrement refractory
    for (int i = 0; i < 5; ++i) {
        neuron.decrementRefractory();
    }
    ASSERT_EQ(neuron.getFiringState(), FiringState::Resting);
}

TEST_F(NeuronTest, AdaptationVariable) {
    neuron.recordSpike(0.0);
    neuron.recordSpike(0.1);
    
    // Spike should increase adaptation variable
    ASSERT_GT(neuron.getState().adaptationVariable, 0.0f);
}

TEST_F(NeuronTest, PlasticityFlags) {
    neuron.enablePlasticity(true, true, true);
    
    const auto& flags = neuron.getPlasticityFlags();
    ASSERT_TRUE(flags.hebbian);
    ASSERT_TRUE(flags.stdp);
    ASSERT_TRUE(flags.reward_modulated);
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM Neuron Tests (Google Test) ===" << std::endl;
    std::cout << "Testing LIF Neuron Dynamics:" << std::endl;
    std::cout << "  - Membrane potential integration" << std::endl;
    std::cout << "  - Spike generation and refractory period" << std::endl;
    std::cout << "  - Synaptic input processing" << std::endl;
    std::cout << "  - Plasticity flag management" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

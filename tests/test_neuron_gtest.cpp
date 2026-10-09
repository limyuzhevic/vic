#include <gtest/gtest.h>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Random/Random.hpp"
#include <memory>
#include <vector>

namespace test_neuron {

class NeuronTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
        neuron2 = std::make_unique<nlm::Neuron>(nlm::NeuronId(2));
    }

    void TearDown() override {
        neuron.reset();
        neuron2.reset();
    }

    std::unique_ptr<nlm::Neuron> neuron;
    std::unique_ptr<nlm::Neuron> neuron2;
};

TEST_F(NeuronTestFixture, NeuronCreation) {
    EXPECT_EQ(neuron->getId(), nlm::NeuronId(1));
    EXPECT_EQ(neuron->getType(), nlm::NeuronType::Internal);
}

TEST_F(NeuronTestFixture, NeuronTypeSetting) {
    neuron->setType(nlm::NeuronType::Excitatory);
    EXPECT_EQ(neuron->getType(), nlm::NeuronType::Excitatory);
    
    neuron->setType(nlm::NeuronType::Inhibitory);
    EXPECT_EQ(neuron->getType(), nlm::NeuronType::Inhibitory);
}

TEST_F(NeuronTestFixture, MembranePotential) {
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -70.0f);
    
    neuron->setMembranePotential(-65.0f);
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -65.0f);
    
    neuron->addToMembranePotential(5.0f);
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -60.0f);
}

TEST_F(NeuronTestFixture, FiringState) {
    EXPECT_FALSE(neuron->isFiring());
    EXPECT_FALSE(neuron->isRefractory());
    
    neuron->setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron->isFiring());
    
    neuron->setRefractoryPeriod(10);
    EXPECT_TRUE(neuron->isRefractory());
    EXPECT_FALSE(neuron->isFiring());
}

TEST_F(NeuronTestFixture, RefractoryDecrement) {
    neuron->setRefractoryPeriod(5);
    EXPECT_TRUE(neuron->isRefractory());
    
    neuron->decrementRefractory();
    EXPECT_TRUE(neuron->isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron->decrementRefractory();
    }
    EXPECT_FALSE(neuron->isRefractory());
}

TEST_F(NeuronTestFixture, CurrentInjection) {
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 0.0f);
    
    neuron->injectCurrent(5.0f);
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 5.0f);
    
    neuron->clearTotalCurrent();
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 0.0f);
}

TEST_F(NeuronTestFixture, SynapticInput) {
    neuron->receiveExcitatoryInput(5.0f);
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 5.0f);
    
    neuron->clearTotalCurrent();
    neuron->receiveInhibitoryInput(3.0f);
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), -3.0f);
}

TEST_F(NeuronTestFixture, SpikeHistory) {
    EXPECT_TRUE(neuron->getSpikeHistory().empty());
    
    neuron->recordSpike(0.1);
    neuron->recordSpike(0.2);
    neuron->recordSpike(0.3);
    
    EXPECT_EQ(neuron->getSpikeHistory().size(), 3);
    EXPECT_FLOAT_EQ(neuron->getSpikeHistory()[0], 0.1f);
    
    neuron->clearSpikeHistory();
    EXPECT_TRUE(neuron->getSpikeHistory().empty());
}

TEST_F(NeuronTestFixture, RandomInitialization) {
    nlm::RandomGenerator rng(42);
    neuron->initializeRandom(rng);
    
    EXPECT_LT(neuron->getMembranePotential(), -60.0f);
    EXPECT_GT(neuron->getMembranePotential(), -80.0f);
    EXPECT_LT(neuron->getThreshold(), -50.0f);
    EXPECT_GT(neuron->getThreshold(), -60.0f);
}

TEST_F(NeuronTestFixture, ResetFunctionality) {
    neuron->setMembranePotential(-60.0f);
    neuron->injectCurrent(5.0f);
    neuron->recordSpike(0.1);
    
    neuron->reset();
    
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -70.0f);
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 0.0f);
    EXPECT_TRUE(neuron->getSpikeHistory().empty());
}

TEST_F(NeuronTestFixture, NeuronComparison) {
    EXPECT_NE(neuron->getId(), neuron2->getId());
    EXPECT_TRUE(neuron->getId() < neuron2->getId() || neuron->getId() > neuron2->getId());
}

TEST_F(NeuronTestFixture, NeuronThreshold) {
    float originalThreshold = neuron->getThreshold();
    neuron->setThreshold(-45.0f);
    EXPECT_FLOAT_EQ(neuron->getThreshold(), -45.0f);
    
    neuron->setThreshold(originalThreshold);
    EXPECT_FLOAT_EQ(neuron->getThreshold(), originalThreshold);
}

TEST_F(NeuronTestFixture, NeuronTimeConstants) {
    EXPECT_GT(neuron->getTauMem(), 0.0f);
    EXPECT_GT(neuron->getTauSyn(), 0.0f);
    EXPECT_GT(neuron->getTauRef(), 0.0f);
    EXPECT_GT(neuron->getTauMax(), 0.0f);
}

TEST_F(NeuronTestFixture, MembranePotentialClamp) {
    neuron->setMembranePotential(-90.0f);
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -90.0f);
    
    neuron->setMembranePotential(-50.0f);
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -50.0f);
}

} // namespace test_neuron
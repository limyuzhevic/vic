"""
C++ Unit Tests

Tests for C++ components using Google Test framework
"""
#include <gtest/gtest.h>
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "memory/Memory.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "performance/MemoryPool.hpp"
#include <memory>

// Forward declarations for test classes
class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
    }
    
    std::unique_ptr<nlm::Neuron> neuron;
};

class SynapseTest : public ::testing::Test {
protected:
    void SetUp() override {
        synapse = std::make_unique<nlm::Synapse>(nlm::SynapseId(1));
        pre = nlm::NeuronId(1);
        post = nlm::NeuronId(2);
    }
    
    std::unique_ptr<nlm::Synapse> synapse;
    nlm::NeuronId pre;
    nlm::NeuronId post;
};

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_unique<nlm::Config>();
    }
    
    std::unique_ptr<nlm::Config> config;
};

TEST_F(NeuronTest, Creation) {
    ASSERT_TRUE(neuron->getId().value == 1);
    ASSERT_TRUE(neuron->getType() == nlm::NeuronType::Internal);
    ASSERT_TRUE(neuron->getMembranePotential() == -70.0f);
}

TEST_F(NeuronTest, TypeChanges) {
    neuron->setType(nlm::NeuronType::Excitatory);
    ASSERT_TRUE(neuron->getType() == nlm::NeuronType::Excitatory);
    
    neuron->setType(nlm::NeuronType::Inhibitory);
    ASSERT_TRUE(neuron->getType() == nlm::NeuronType::Inhibitory);
}

TEST_F(NeuronTest, MembranePotential) {
    neuron->setMembranePotential(-65.0f);
    ASSERT_TRUE(neuron->getMembranePotential() == -65.0f);
    
    neuron->addToMembranePotential(5.0f);
    ASSERT_TRUE(neuron->getMembranePotential() == -60.0f);
}

TEST_F(NeuronTest, FiringState) {
    ASSERT_FALSE(neuron->isFiring());
    ASSERT_FALSE(neuron->isRefractory());
    
    neuron->setFiringState(nlm::FiringState::Active);
    ASSERT_TRUE(neuron->isFiring());
    
    neuron->setRefractoryPeriod(10);
    ASSERT_TRUE(neuron->isRefractory());
    ASSERT_FALSE(neuron->isFiring());
}

TEST_F(NeuronTest, RefractoryDecrement) {
    neuron->setRefractoryPeriod(5);
    ASSERT_TRUE(neuron->isRefractory());
    
    neuron->decrementRefractory();
    ASSERT_TRUE(neuron->isRefractory());
    
    for (int i = 0; i < 4; ++i) {
        neuron->decrementRefractory();
    }
    ASSERT_FALSE(neuron->isRefractory());
}

TEST_F(NeuronTest, CurrentInjection) {
    ASSERT_TRUE(neuron->getTotalCurrent() == 0.0f);
    
    neuron->injectCurrent(5.0f);
    ASSERT_TRUE(neuron->getTotalCurrent() == 5.0f);
    
    neuron->clearTotalCurrent();
    ASSERT_TRUE(neuron->getTotalCurrent() == 0.0f);
}

TEST_F(NeuronTest, SynapticInput) {
    neuron->receiveExcitatoryInput(5.0f);
    ASSERT_TRUE(neuron->getTotalCurrent() == 5.0f);
    
    neuron->clearTotalCurrent();
    neuron->receiveInhibitoryInput(3.0f);
    ASSERT_TRUE(neuron->getTotalCurrent() == -3.0f);
}

TEST_F(NeuronTest, SpikeHistory) {
    ASSERT_TRUE(neuron->getSpikeHistory().empty());
    
    neuron->recordSpike(0.1);
    neuron->recordSpike(0.2);
    neuron->recordSpike(0.3);
    
    ASSERT_TRUE(neuron->getSpikeHistory().size() == 3);
    
    neuron->clearSpikeHistory();
    ASSERT_TRUE(neuron->getSpikeHistory().empty());
}

TEST_F(NeuronTest, RandomInitialization) {
    nlm::RandomGenerator rng(42);
    nlm::Neuron testNeuron(nlm::NeuronId(1));
    
    testNeuron.initializeRandom(rng);
    
    ASSERT_TRUE(testNeuron.getMembranePotential() < -60.0f && testNeuron.getMembranePotential() > -80.0f);
    ASSERT_TRUE(testNeuron.getThreshold() < -50.0f && testNeuron.getThreshold() > -60.0f);
}

TEST_F(NeuronTest, Reset) {
    neuron->setMembranePotential(-60.0f);
    neuron->injectCurrent(5.0f);
    neuron->recordSpike(0.1);
    
    neuron->reset();
    
    ASSERT_TRUE(neuron->getMembranePotential() == -70.0f);
    ASSERT_TRUE(neuron->getTotalCurrent() == 0.0f);
    ASSERT_TRUE(neuron->getSpikeHistory().empty());
}

TEST_F(SynapseTest, Creation) {
    ASSERT_TRUE(synapse->getId().value == 1);
    ASSERT_TRUE(synapse->getType() == nlm::SynapseType::Excitatory);
    ASSERT_TRUE(synapse->getStrength() == 1.0f);
}

TEST_F(SynapseTest, Connection) {
    synapse->setPreSynaptic(neuron->getId());
    synapse->setPostSynaptic(post);
    
    ASSERT_TRUE(synapse->getPreSynaptic() == neuron->getId());
    ASSERT_TRUE(synapse->getPostSynaptic() == post);
}

TEST_F(SynapseTest, STDP) {
    nlm::STDP stdp;
    stdp.update(1.0f, 1.0f, true, nlm::FiringState::Active);
    
    ASSERT_TRUE(stdp.getLTP() > 0.0f);
    ASSERT_TRUE(stdp.getLTD() == 0.0f);
}

TEST_F(ConfigTest, Creation) {
    ASSERT_TRUE(config != nullptr);
}

TEST_F(ConfigTest, KeyValueOperations) {
    config->set("test_key", "test_value");
    ASSERT_TRUE(config->has("test_key"));
    
    std::string value = config->get<std::string>("test_key");
    ASSERT_TRUE(value == "test_value");
    
    config->remove("test_key");
    ASSERT_FALSE(config->has("test_key"));
}

TEST(MemoryPoolTest, AllocationDeallocation) {
    nlm::MemoryPool pool(1024, 64);
    
    void* ptr1 = pool.allocate();
    ASSERT_TRUE(ptr1 != nullptr);
    
    void* ptr2 = pool.allocate();
    ASSERT_TRUE(ptr2 != nullptr);
    
    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
}

TEST(NeuromodulatorTest, Creation) {
    nlm::Dopamine dopamine;
    ASSERT_TRUE(dopamine.getType() == nlm::ModulatorType::Dopamine);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
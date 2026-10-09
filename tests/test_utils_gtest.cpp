#include <gtest/gtest.h>
#include "core/Config/Config.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include <cmath>

namespace test_utils {

class ConfigTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
    }

    void TearDown() override {
        config.reset();
    }

    std::shared_ptr<nlm::Config> config;
};

TEST_F(ConfigTestFixture, ConfigCreation) {
    EXPECT_NE(config, nullptr);
}

TEST_F(ConfigTestFixture, ConfigValueSetting) {
    config->set("test_key", 42, nlm::ConfigSource::Runtime);
    
    auto value = config->get<int64_t>("test_key");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, 42);
}

TEST_F(ConfigTestFixture, ConfigDoubleValue) {
    config->set("test_double", 3.14, nlm::ConfigSource::Runtime);
    
    auto value = config->get<double>("test_double");
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(*value, 3.14);
}

TEST_F(ConfigTestFixture, ConfigStringValue) {
    config->set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
    
    auto value = config->get<std::string>("test_string");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "hello");
}

TEST_F(ConfigTestFixture, ConfigVectorValue) {
    std::vector<int> testVector = {1, 2, 3, 4, 5};
    config->set("test_vector", testVector, nlm::ConfigSource::Runtime);
    
    auto value = config->get<std::vector<int>>("test_vector");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value->size(), 5);
}

TEST_F(ConfigTestFixture, ConfigDefaultValue) {
    auto value = config->getOr<int64_t>("non_existent", 99);
    EXPECT_EQ(value, 99);
}

TEST_F(ConfigTestFixture, ConfigFileOperations) {
    config->set("test_value", 123, nlm::ConfigSource::Runtime);
    
    bool saved = config->saveToFile("test_config.json");
    EXPECT_TRUE(saved);
    
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_config.json");
    EXPECT_TRUE(loaded);
    
    auto value = config2.get<int64_t>("test_value");
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, 123);
    
    std::remove("test_config.json");
}

TEST_F(ConfigTestFixture, ConfigJSONOperations) {
    std::string jsonString = config->saveToJsonString();
    EXPECT_FALSE(jsonString.empty());
    
    nlm::Config config2;
    bool loaded = config2.loadFromJsonString(jsonString);
    EXPECT_TRUE(loaded);
}

TEST_F(ConfigTestFixture, ConfigClear) {
    config->set("to_clear", 123, nlm::ConfigSource::Runtime);
    config->clear();
    
    auto value = config->get<int64_t>("to_clear");
    EXPECT_FALSE(value.has_value());
}

TEST_F(ConfigTestFixture, ConfigValidation) {
    config->set("valid_int", 42, nlm::ConfigSource::Runtime);
    config->set("valid_double", 3.14, nlm::ConfigSource::Runtime);
    config->set("valid_string", "test", nlm::ConfigSource::Runtime);
    
    auto intVal = config->get<int64_t>("valid_int");
    auto doubleVal = config->get<double>("valid_double");
    auto stringVal = config->get<std::string>("valid_string");
    
    ASSERT_TRUE(intVal.has_value());
    ASSERT_TRUE(doubleVal.has_value());
    ASSERT_TRUE(stringVal.has_value());
}

TEST_F(ConfigTestFixture, ConfigTypeConversions) {
    config->set("int", 42, nlm::ConfigSource::Runtime);
    config->set("double", 3.14, nlm::ConfigSource::Runtime);
    config->set("string", "hello", nlm::ConfigSource::Runtime);
    
    auto intVal = config->getOr<int64_t>("int", 0);
    auto doubleVal = config->getOr<double>("double", 0.0);
    auto stringVal = config->getOr<std::string>("string", "");
    
    EXPECT_EQ(intVal, 42);
    EXPECT_DOUBLE_EQ(doubleVal, 3.14);
    EXPECT_EQ(stringVal, "hello");
}

TEST_F(ConfigTestFixture, ConfigSourceTracking) {
    config->set("default_key", 1, nlm::ConfigSource::Default);
    config->set("runtime_key", 2, nlm::ConfigSource::Runtime);
    
    // All keys should be accessible regardless of source
    auto defaultVal = config->get<int64_t>("default_key");
    auto runtimeVal = config->get<int64_t>("runtime_key");
    
    ASSERT_TRUE(defaultVal.has_value());
    ASSERT_TRUE(runtimeVal.has_value());
}

TEST_F(ConfigTestFixture, ConfigStressTest) {
    // Test config under heavy load
    for (int i = 0; i < 1000; ++i) {
        config->set(std::to_string(i), static_cast<int64_t>(i), nlm::ConfigSource::Runtime);
        auto value = config->getOr<int64_t>(std::to_string(i), -1);
        EXPECT_EQ(value, i);
    }
}

class NeuronUtilsTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
    }

    void TearDown() override {
        neuron.reset();
    }

    std::unique_ptr<nlm::Neuron> neuron;
};

TEST_F(NeuronUtilsTestFixture, NeuronBasicProperties) {
    EXPECT_EQ(neuron->getId(), nlm::NeuronId(1));
    EXPECT_EQ(neuron->getType(), nlm::NeuronType::Internal);
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -70.0f);
    EXPECT_FLOAT_EQ(neuron->getThreshold(), -55.0f);
}

TEST_F(NeuronUtilsTestFixture, NeuronStateChanges) {
    neuron->setMembranePotential(-60.0f);
    neuron->setThreshold(-50.0f);
    neuron->setType(nlm::NeuronType::Excitatory);
    
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -60.0f);
    EXPECT_FLOAT_EQ(neuron->getThreshold(), -50.0f);
    EXPECT_EQ(neuron->getType(), nlm::NeuronType::Excitatory);
}

TEST_F(NeuronUtilsTestFixture, NeuronInputOutput) {
    neuron->injectCurrent(10.0f);
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 10.0f);
    
    neuron->clearTotalCurrent();
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 0.0f);
    
    neuron->recordSpike(0.1f);
    EXPECT_EQ(neuron->getSpikeHistory().size(), 1);
    
    neuron->clearSpikeHistory();
    EXPECT_TRUE(neuron->getSpikeHistory().empty());
}

TEST_F(NeuronUtilsTestFixture, NeuronPhysics) {
    EXPECT_GT(neuron->getTauMem(), 0.0f);
    EXPECT_GT(neuron->getTauSyn(), 0.0f);
    EXPECT_GT(neuron->getTauRef(), 0.0f);
    EXPECT_GT(neuron->getTauMax(), 0.0f);
}

TEST_F(NeuronUtilsTestFixture, NeuronFiring) {
    neuron->setFiringState(nlm::FiringState::Active);
    EXPECT_TRUE(neuron->isFiring());
    
    neuron->setRefractoryPeriod(10);
    EXPECT_TRUE(neuron->isRefractory());
    EXPECT_FALSE(neuron->isFiring());
}

TEST_F(NeuronUtilsTestFixture, NeuronReset) {
    neuron->setMembranePotential(-60.0f);
    neuron->injectCurrent(5.0f);
    neuron->recordSpike(0.1f);
    
    neuron->reset();
    
    EXPECT_FLOAT_EQ(neuron->getMembranePotential(), -70.0f);
    EXPECT_FLOAT_EQ(neuron->getTotalCurrent(), 0.0f);
    EXPECT_TRUE(neuron->getSpikeHistory().empty());
}

TEST_F(NeuronUtilsTestFixture, NeuronRandom) {
    nlm::RandomGenerator rng(42);
    neuron->initializeRandom(rng);
    
    EXPECT_LT(neuron->getMembranePotential(), -60.0f);
    EXPECT_GT(neuron->getMembranePotential(), -80.0f);
}

TEST_F(NeuronUtilsTestFixture, NeuronComparison) {
    nlm::Neuron neuron2(nlm::NeuronId(2));
    
    EXPECT_NE(neuron->getId(), neuron2.getId());
    EXPECT_TRUE(neuron->getId() < neuron2.getId() || neuron->getId() > neuron2.getId());
}

TEST_F(NeuronUtilsTestFixture, SynapseBasicProperties) {
    auto preNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
    auto postNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(2));
    
    preNeuron->setType(nlm::NeuronType::Excitatory);
    postNeuron->setType(nlm::NeuronType::Excitatory);
    
    auto synapse = std::make_unique<nlm::Synapse>(preNeuron.get(), postNeuron.get(), 0.5f);
    
    EXPECT_FLOAT_EQ(synapse->getWeight(), 0.5f);
    EXPECT_EQ(synapse->getPreNeuronId(), nlm::NeuronId(1));
    EXPECT_EQ(synapse->getPostNeuronId(), nlm::NeuronId(2));
}

TEST_F(NeuronUtilsTestFixture, SynapseState) {
    auto preNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
    auto postNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(2));
    
    auto synapse = std::make_unique<nlm::Synapse>(preNeuron.get(), postNeuron.get(), 0.5f);
    
    synapse->setWeight(0.8f);
    EXPECT_FLOAT_EQ(synapse->getWeight(), 0.8f);
    
    synapse->setEnabled(false);
    EXPECT_FALSE(synapse->isEnabled());
}

TEST_F(NeuronUtilsTestFixture, SynapseIntegration) {
    auto preNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
    auto postNeuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(2));nlm::Synapse synapse(preNeuron.get(), postNeuron.get(), 0.5f);
    
    EXPECT_TRUE(std::isfinite(synapse.getWeight()));
}

TEST_F(NeuronUtilsTestFixture, UtilsPerformance) {
    // Performance test for utilities
    auto neuron = std::make_unique<nlm::Neuron>(nlm::NeuronId(1));
    
    for (int i = 0; i < 1000; ++i) {
        neuron->setMembranePotential(static_cast<float>(i));
        neuron->getMembranePotential();
    }
}

} // namespace test_utils
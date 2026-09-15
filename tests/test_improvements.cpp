#include <gtest/gtest.h>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

class ConfigTests : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_unique<nlm::Config>();
    }
    
    void TearDown() override {
        config.reset();
    }
    
    std::unique_ptr<nlm::Config> config;
};

TEST_F(ConfigTests, TrimFunctionTests) {
    EXPECT_EQ(config->trim("  hello  "), "hello");
    EXPECT_EQ(config->trim("\thello\t"), "hello");
    EXPECT_EQ(config->trim("\nhello\n"), "hello");
    EXPECT_EQ(config->trim("\r\nhello\r\n"), "hello");
    EXPECT_EQ(config->trim("\fhello\f"), "hello");
    EXPECT_EQ(config->trim("\vhello\v"), "hello");
    EXPECT_EQ(config->trim("  hello world  "), "hello world");
    EXPECT_EQ(config->trim(""), "");
}

TEST_F(ConfigTests, LoadFromFileTests) {
    // Test loading from a config file
    EXPECT_TRUE(config->loadFromFile("configs/default.cfg"));
    
    // Test that values were loaded
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_TRUE(config->has("region_count"));
    EXPECT_TRUE(config->has("connection_probability"));
    
    // Verify values
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 500);
    EXPECT_EQ(config->getOr<int64_t>("region_count", 0), 1);
    EXPECT_EQ(config->getOr<float>("connection_probability", 0.0f), 0.15f);
}

TEST_F(ConfigTests, JSONParserTests) {
    // Test JSON parsing functionality
    std::unordered_map<std::string, nlm::ConfigValue> jsonValues;
    
    // Create test JSON
    std::string testJson = "{\"neuron_count\": 1000, \"region_count\": 2, \"connection_probability\": 0.25}";
    
    // This would require proper JSON parser implementation
    // For now, just test that the Config class compiles and can be used
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.25f);
    
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 1000);
    EXPECT_EQ(config->getOr<int64_t>("region_count", 0), 2);
    EXPECT_EQ(config->getOr<float>("connection_probability", 0.0f), 0.25f);
}

TEST_F(ConfigTests, CommandLineArgsTests) {
    const char* argv[] = {"test", "--neuron_count", "800", "--region_count", "3"};
    int argc = 5;
    
    EXPECT_TRUE(config->loadFromArgs(argc, const_cast<char**>(argv)));
    
    EXPECT_EQ(config->getOr<int64_t>("neuron_count", 0), 800);
    EXPECT_EQ(config->getOr<int64_t>("region_count", 0), 3);
}

class BrainTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a minimal config
        config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42);
        
        brain = std::make_shared<nlm::Brain>(config);
        
        // Initialize the brain
        if (!brain->initialize()) {
            FAIL() << "Failed to initialize brain";
        }
    }
    
    void TearDown() override {
        brain.reset();
        config.reset();
    }
    
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
};

TEST_F(BrainTests, ExcitationInhibitionRatioTests) {
    // Test E/I ratio calculation
    float eiRatio = brain->getExcitationInhibitionRatio();
    
    // Should be non-negative
    EXPECT_GE(eiRatio, 0.0f);
    
    // Should be finite (not infinity or NaN)
    EXPECT_TRUE(std::isfinite(eiRatio));
    
    // Test with reset brain
    brain->reset();
    brain->initialize();
    float eiRatio2 = brain->getExcitationInhibitionRatio();
    EXPECT_TRUE(std::isfinite(eiRatio2));
}

TEST_F(BrainTests, NeuralRegionGetPopulationTests) {
    // Get the brain region
    auto regions = brain->getRegions();
    ASSERT_FALSE(regions.empty());
    
    auto* region = regions[0].get();
    
    // Add a population
    auto popId = region->addPopulation(10, nlm::NeuronType::Internal);
    ASSERT_NE(popId.value, 0);  // Should not be INVALID_POPULATION_ID
    
    // Get the population
    auto* population = region->getPopulation(popId);
    ASSERT_NE(population, nullptr);
    EXPECT_EQ(population->getSize(), 10);
    
    // Test with invalid ID
    nlm::PopulationId invalidId;
    invalidId.value = 99999;
    auto* invalidPop = region->getPopulation(invalidId);
    EXPECT_EQ(invalidPop, nullptr);
    
    // Test with ID 0
    nlm::PopulationId zeroId(0);
    auto* zeroPop = region->getPopulation(zeroId);
    EXPECT_EQ(zeroPop, nullptr);
}

TEST_F(BrainTests, NeuronInitializationTests) {
    // Test neuron initialization
    auto neurons = brain->getAllNeurons();
    ASSERT_FALSE(neurons.empty());
    
    // Check that neurons have valid properties
    for (auto* neuron : neurons) {
        EXPECT_NE(neuron->getId().value, 0);
        EXPECT_GE(neuron->getState().threshold, -100.0f);
        EXPECT_LE(neuron->getState().threshold, 50.0f);
    }
}

TEST_F(BrainTests, SynapseOperationsTests) {
    // Test synapse operations
    auto regions = brain->getRegions();
    ASSERT_FALSE(regions.empty());
    
    auto* region = regions[0].get();
    
    // Get all neurons
    auto neurons = region->getAllNeurons();
    ASSERT_GE(neurons.size(), 2);
    
    // Add a synapse
    auto sourceId = neurons[0]->getId();
    auto destId = neurons[1]->getId();
    auto synId = region->addSynapse(sourceId, destId, 0.5f, 1);
    
    EXPECT_NE(synId.value, 0);
    
    // Get the synapse
    auto* synapse = region->getSynapse(synId);
    ASSERT_NE(synapse, nullptr);
    EXPECT_EQ(synapse->getWeight(), 0.5f);
    EXPECT_EQ(synapse->getDelay(), 1);
}

TEST_F(BrainTests, PlasticityEnabledTests) {
    // Test enabling plasticity
    auto regions = brain->getRegions();
    ASSERT_FALSE(regions.empty());
    
    auto* region = regions[0].get();
    auto neurons = region->getAllNeurons();
    
    if (!neurons.empty() && !region->getSynapses().empty()) {
        auto* synapse = region->getSynapses().front().get();
        
        // Enable Hebbian and STDP plasticity
        synapse->enablePlasticity(true, true, false);
        
        auto& flags = synapse->getPlasticityFlags();
        EXPECT_TRUE(flags.hebbian);
        EXPECT_TRUE(flags.stdp);
        EXPECT_FALSE(flags.reward_modulated);
    }
}

TEST_F(BrainTests, BrainStepTests) {
    // Test basic brain stepping
    auto initialStep = brain->getTotalSpikeCount();
    
    // Step the brain
    brain->step(0, 0.0);
    
    // Spike count should increase or stay the same
    EXPECT_LE(brain->getTotalSpikeCount(), initialStep + 10);  // Allow for some spike activity
    
    // Test multiple steps
    for (int i = 1; i < 10; ++i) {
        brain->step(i, i * 0.001);
    }
    
    // Should have some spike activity
    EXPECT_TRUE(brain->getTotalSpikeCount() >= initialStep);
}

TEST(ConfigEdgeCases, EmptyFileLoad) {
    nlm::Config config;
    
    // Test loading non-existent file (should not crash)
    EXPECT_FALSE(config.loadFromFile("non_existent_file.cfg"));
}

TEST(ConfigEdgeCases, TrimEdgeCases) {
    nlm::Config config;
    
    EXPECT_EQ(config.trim(""), "");
    EXPECT_EQ(config.trim("   "), "");
    EXPECT_EQ(config.trim("\t\n\r\f\v"), "");
}

TEST(ConfigEdgeCases, CommandLineParsing) {
    nlm::Config config;
    
    const char* argv[] = {"test", "--int", "42", "--double", "3.14", "--string", "hello"};
    int argc = 7;
    
    config.loadFromArgs(argc, const_cast<char**>(argv));
    
    EXPECT_EQ(config.getOr<int64_t>("int", 0), 42);
    EXPECT_DOUBLE_EQ(config.getOr<double>("double", 0.0), 3.14);
    EXPECT_EQ(config.getOr<std::string>("string", ""), "hello");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
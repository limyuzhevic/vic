// Brain Tests - Google Test Version
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <iostream>

namespace nlm {

class BrainTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        
        brain = std::make_shared<Brain>(config);
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

TEST_F(BrainTest, BrainCreation) {
    // Test that brain can be created with config
    auto testConfig = std::make_shared<Config>();
    testConfig->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    
    Brain testBrain(testConfig);
    
    // Just test construction - no assertions since we can't easily verify internal state
    ASSERT_NE(testBrain.getRegionCount(), 0);  // Should have regions
}

TEST_F(BrainTest, BrainInitialization) {
    bool success = brain->initialize();
    ASSERT_TRUE(success);
    
    // Verify brain initialized with expected properties
    ASSERT_EQ(brain->getRegionCount(), 2);
    ASSERT_EQ(brain->getTotalNeuronCount(), 100);
    ASSERT_GT(brain->getTotalSynapseCount(), 0);
    ASSERT_GE(brain->getActiveNeuronCount(), 0);
    ASSERT_GE(brain->getFiringNeuronCount(), 0);
    ASSERT_GE(brain->getAverageFiringRate(), 0.0f);
    ASSERT_GE(brain->getExcitationInhibitionRatio(), 0.0f);
    ASSERT_GE(brain->getTotalSpikeCount(), 0);
    ASSERT_EQ(brain->getDevelopmentalStage(), DevelopmentalStage::Adult);
}

TEST_F(BrainTest, BrainStep) {
    brain->initialize();
    
    // Run a few steps
    for (SimulationStep step = 0; step < 10; ++step) {
        brain->step(step);
    }
    
    // Verify basic properties remain consistent
    ASSERT_EQ(brain->getTotalNeuronCount(), 100);
}

TEST_F(BrainTest, BrainReset) {
    brain->initialize();
    
    brain->step(0);
    brain->reset();
    
    // Verify reset worked
    ASSERT_EQ(brain->getTotalNeuronCount(), 50);
    ASSERT_EQ(brain->getTotalSynapseCount(), 0);  // After reset, synapses should be cleared
}

TEST_F(BrainTest, BrainRegions) {
    brain->initialize();
    
    auto regionIds = brain->getRegionIds();
    ASSERT_EQ(regionIds.size(), 3);  // 2 from config + 1 default
    
    for (auto rid : regionIds) {
        auto* region = brain->getRegion(rid);
        ASSERT_NE(region, nullptr);
    }
}

TEST_F(BrainTest, BrainAddRegion) {
    brain->initialize();
    
    int initialCount = brain->getRegionCount();
    brain->addRegion("TestRegion");
    
    ASSERT_EQ(brain->getRegionCount(), initialCount + 1);
    
    // Get the new region
    auto regionIds = brain->getRegionIds();
    ASSERT_FALSE(regionIds.empty());
}

TEST_F(BrainTest, BrainActionProduction) {
    brain->initialize();
    
    auto action = brain->produceAction();
    // Note: action might be nullptr if no neurons are firing
    // Just test that the method doesn't crash
    // If action is not null, it should have valid properties
    if (action != nullptr) {
        ASSERT_NE(action->getName(), "");
    }
}

TEST_F(BrainTest, BrainSensoryInput) {
    brain->initialize();
    
    // Create a simple sensory input
    SensoryInput visionInput;
    visionInput.setType(SensoryInputType::Vision);
    
    // Test that brain can receive sensory input
    brain->receiveSensoryInput(visionInput);
    
    // Should not crash
    ASSERT_TRUE(true);
}

TEST_F(BrainTest, BrainStatus) {
    brain->initialize();
    
    // Verify statistics are accessible and reasonable
    ASSERT_GT(brain->getTotalNeuronCount(), 0);
    ASSERT_GE(brain->getTotalSynapseCount(), 0);
    ASSERT_GE(brain->getActiveNeuronCount(), 0);
    ASSERT_GE(brain->getFiringNeuronCount(), 0);
    ASSERT_GE(brain->getAverageFiringRate(), 0.0f);
    ASSERT_GE(brain->getExcitationInhibitionRatio(), 0.0f);
    ASSERT_GE(brain->getTotalSpikeCount(), 0);
}

TEST_F(BrainTest, BrainConfigIntegration) {
    // Test that config is properly integrated into brain
    auto testConfig = std::make_shared<Config>();
    testConfig->set("custom_param", 999, ConfigSource::Runtime);
    
    Brain testBrain(testConfig);
    testBrain.initialize();
    
    // Config should be accessible
    auto config = testBrain.getConfig();
    ASSERT_TRUE(config->has("custom_param"));
}

TEST_F(BrainTest, BrainLogStatus) {
    brain->initialize();
    
    // Test that logging works (should not crash)
    brain->logStatus();
    
    ASSERT_TRUE(true);  // If no exception, test passes
}

TEST_F(BrainTest, BrainSaveLoad) {
    brain->initialize();
    
    // Test save functionality
    std::string testFile = "/tmp/test_brain_save.json";
    brain->save(testFile);
    
    // Verify file was created
    ASSERT_TRUE(std::ifstream(testFile).good());
    
    // Test load functionality
    auto newConfig = std::make_shared<Config>();
    Brain newBrain(newConfig);
    newBrain.load(testFile);
    
    // Clean up
    std::remove(testFile.c_str());
}

TEST_F(BrainTest, BrainDevelopmentalStages) {
    brain->initialize();
    
    // Test different developmental stages
    brain->setDevelopmentalStage(DevelopmentalStage::Initial);
    ASSERT_EQ(brain->getDevelopmentalStage(), DevelopmentalStage::Initial);
    
    brain->setDevelopmentalStage(DevelopmentalStage::Adult);
    ASSERT_EQ(brain->getDevelopmentalStage(), DevelopmentalStage::Adult);
}

TEST_F(BrainTest, BrainMultipleStepsConsistency) {
    brain->initialize();
    
    // Take multiple steps and verify consistency
    int step1 = brain->getTotalNeuronCount();
    brain->step(0);
    int step2 = brain->getTotalNeuronCount();
    
    // Neuron count should remain constant across steps
    ASSERT_EQ(step1, step2);
}

TEST_F(BrainTest, BrainWithDifferentConfigs) {
    // Test brain creation with different configurations
    auto config1 = std::make_shared<Config>();
    config1->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto config2 = std::make_shared<Config>();
    config2->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
    
    Brain brain1(config1);
    Brain brain2(config2);
    
    ASSERT_NE(brain1.getTotalNeuronCount(), brain2.getTotalNeuronCount());
    ASSERT_LT(brain1.getTotalNeuronCount(), brain2.getTotalNeuronCount());
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM Brain Tests (Google Test) ===" << std::endl;
    std::cout << "Testing Core Brain Simulation and Integration:" << std::endl;
    std::cout << "  - Brain construction and configuration" << std::endl;
    std::cout << "  - Brain initialization and setup" << std::endl;
    std::cout << "  - Simulation stepping" << std::endl;
    std::cout << "  - Reset and state management" << std::endl;
    std::cout << "  - Region management" << std::endl;
    std::cout << "  - Action production" << std::endl;
    std::cout << "  - Sensory input processing" << std::endl;
    std::cout << "  - Status and statistics" << std::endl;
    std::cout << "  - Configuration integration" << std::endl;
    std::cout << "  - Save/load functionality" << std::endl;
    std::cout << "  - Developmental stage management" << std::endl;
    std::cout << "  - Consistency across steps" << std::endl;
    std::cout << "  - Different configuration scenarios" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

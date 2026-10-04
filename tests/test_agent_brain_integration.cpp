#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

// Include NLM headers
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"

class AgentBrainTest : public ::testing::Test {
protected:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    
    void SetUp() override {
        config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        
        brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        agent = std::make_shared<AgentBrain>(brain);
    }
};

TEST_F(AgentBrainTest, Constructor) {
    ASSERT_NE(agent, nullptr);
    ASSERT_EQ(agent->getBrain(), brain.get());
}

TEST_F(AgentBrainTest, SensoryInputSize) {
    // Test default sensory input size
    size_t inputSize = agent->getSensoryInputSize();
    // Should be vision (16x16=256) + touch (8) + internal (4) + proprioception (6) = 274
    // Note: audio is 0 by default
    EXPECT_EQ(inputSize, 274);
}

TEST_F(AgentBrainTest, MotorOutputSize) {
    size_t outputSize = agent->getMotorOutputSize();
    // Should be 6 motor commands
    EXPECT_EQ(outputSize, 6);
}

TEST_F(AgentBrainTest, Initialize) {
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Should not crash and should log initialization
}

TEST_F(AgentBrainTest, ProcessSensoryInput) {
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Create a perceptual input with typical sizes
    SensoryPercept percept;
    percept.setVision(std::vector<float>(256, 0.5f));  // Vision (16x16)
    percept.setTouch(std::vector<float>(8, 0.0f));     // Touch (8)
    percept.setInternal(std::vector<float>(4, 0.0f));   // Internal (4)
    percept.setProprioception(std::vector<float>(6, 0.0f)); // Proprioception (6)
    
    // Should process without crashing
    agent->processSensoryInput(percept);
    
    // Check that agent has processed the input
    EXPECT_EQ(agent->getCuriosityLevel(), 0.0f); // No novelty expected with uniform input
}

TEST_F(AgentBrainTest, DecodeMotorCommand) {
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Without any neural activity, should default to Wait
    MotorCommand cmd = agent->decodeMotorCommand();
    EXPECT_EQ(cmd, MotorCommand::Wait);
}

TEST_F(AgentBrainTest, ApplyRewardModulation) {
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Apply reward modulation
    agent->applyRewardModulation(1.0f, 0.5f);  // Positive reward
    
    // Should have computed prediction error and updated state
    EXPECT_GT(agent->getNeuromodulationLevel(), 0.0f); // Positive prediction error
}

TEST_F(AgentBrainTest, UpdateDevelopment) {
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Update development
    agent->updateDevelopment(1.0);
    
    // Should have updated developmental age
    EXPECT_EQ(agent->getDevelopmentalStage(), DevelopmentalStage::Initial);
    
    // Update more to reach critical period
    agent->updateDevelopment(100.0);
    EXPECT_EQ(agent->getDevelopmentalStage(), DevelopmentalStage::CriticalPeriod);
}

TEST_F(AgentBrainTest, GetAllStats) {
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Test all getter methods
    EXPECT_EQ(agent->getNeuromodulationLevel(), 0.0f); // Initial state
    EXPECT_EQ(agent->getCuriosityLevel(), 0.0f);
    EXPECT_EQ(agent->getNoveltyLevel(), 0.0f);
    EXPECT_EQ(agent->getPredictionError(), 0.0f);
    EXPECT_EQ(agent->getDevelopmentalStage(), DevelopmentalStage::Initial);
}

TEST_F(AgentBrainTest, ConfigurationOptions) {
    // Test that agent can be configured
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Enable/disable features
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    
    EXPECT_TRUE(agent->isRewardModulationEnabled());
    EXPECT_TRUE(agent->isStructuralPlasticityEnabled());
    EXPECT_TRUE(agent->isDevelopmentEnabled());
    EXPECT_TRUE(agent->isCuriosityEnabled());
}

TEST_F(AgentBrainTest, Reset) {
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Modify state
    agent->applyRewardModulation(0.5f, 0.2f);
    agent->updateDevelopment(10.0);
    
    // Reset agent
    agent->reset();
    
    // State should be reset
    EXPECT_EQ(agent->getNeuromodulationLevel(), 0.0f);
    EXPECT_EQ(agent->getCuriosityLevel(), 0.0f);
    EXPECT_EQ(agent->getPredictionError(), 0.0f);
    EXPECT_EQ(agent->getDevelopmentalStage(), DevelopmentalStage::Initial);
}

TEST_F(AgentBrainTest, SensoryPerceptIntegration) {
    // Test integration with configurable SensoryPercept
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    agent->initialize(world);
    
    // Test with configured sensory percept (non-default sizes)
    SensoryPercept percept(32, 32, 16, 8, 12, 4); // Larger dimensions
    
    // Fill with test data
    std::vector<float> vision(32 * 32, 0.3f);
    std::vector<float> touch(16, 0.1f);
    std::vector<float> internal(8, 0.5f);
    std::vector<float> proprioception(12, 0.2f);
    std::vector<float> audio(4, 0.8f);
    
    percept.setVision(vision);
    percept.setTouch(touch);
    percept.setInternal(internal);
    percept.setProprioception(proprioception);
    percept.setAudio(audio);
    
    // Process should work with custom dimensions
    agent->processSensoryInput(percept);
    
    // Validate the percept
    EXPECT_TRUE(percept.validate());
    EXPECT_EQ(percept.getVisionWidth(), 32);
    EXPECT_EQ(percept.getVisionHeight(), 32);
    EXPECT_EQ(percept.getTouchSize(), 16);
    EXPECT_EQ(percept.getInternalSize(), 8);
    EXPECT_EQ(percept.getProprioceptionSize(), 12);
    EXPECT_EQ(percept.getAudioSize(), 4);
    
    // Total input size should match
    size_t totalInput = percept.getTotalSensoryInputSize();
    EXPECT_EQ(totalInput, 32*32 + 16 + 8 + 12 + 4);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Initialize NLM logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    return RUN_ALL_TESTS();
}

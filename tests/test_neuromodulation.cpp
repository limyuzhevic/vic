// Neuromodulator Tests
// Tests for neuromodulation systems (Curiosity, Reward, Novelty, PredictionError)

#include <gtest/gtest.h>
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/brain/Brain.hpp"

class NeuromodulationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a mock brain for testing
        config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        
        brain = std::make_shared<Brain>(config);
        
        // Initialize systems
        reward = std::make_unique<Reward>();
        novelty = std::make_unique<Novelty>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        
        // Set brain references
        curiosity->initialize(brain.get());
        predictionError->initialize(brain.get());
        
        NLM_LOG_INFO("NeuromodulationTest setup complete");
    }
    
    void TearDown() override {
        NLM_LOG_INFO("NeuromodulationTest cleanup complete");
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::unique_ptr<Reward> reward;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
};

TEST_F(NeuromodulationTest, RewardSystemBasic) {
    // Test basic reward functionality
    EXPECT_FLOAT_EQ(reward->getValue(), 0.0f);
    
    reward->add(5.0f);
    EXPECT_FLOAT_EQ(reward->getValue(), 5.0f);
    EXPECT_FLOAT_EQ(reward->getAccumulated(), 5.0f);
    
    reward->setValue(10.0f);
    EXPECT_FLOAT_EQ(reward->getValue(), 10.0f);
    
    reward->reset();
    EXPECT_FLOAT_EQ(reward->getValue(), 0.0f);
    EXPECT_FLOAT_EQ(reward->getAccumulated(), 0.0f);
}

TEST_F(NeuromodulationTest, RewardAccumulation) {
    reward->add(1.0f);
    reward->add(2.0f);
    reward->add(3.0f);
    
    EXPECT_FLOAT_EQ(reward->getValue(), 6.0f);
    EXPECT_FLOAT_EQ(reward->getAccumulated(), 6.0f);
}

TEST_F(NeuromodulationTest, NoveltyDetection) {
    // Test novelty calculation
    std::vector<float> oldVision = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> newVision = {0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
    
    float novelty = novelty->compute(oldVision, newVision);
    EXPECT_GT(novelty, 0.0f);
    
    // Similar vision should have low novelty
    float similarity = novelty->compute(newVision, newVision);
    EXPECT_LT(similarity, 0.1f);
}

TEST_F(NeuromodulationTest, CuriosityIntegration) {
    // Test curiosity update with novelty and prediction error
    curiosity->update(0.8f, 0.5f, 0.1f);
    
    EXPECT_GT(curiosity->getLevel(), 0.0f);
    EXPECT_LE(curiosity->getLevel(), 1.0f);
    
    // Higher novelty and prediction error should increase curiosity
    curiosity->reset();
    curiosity->update(0.0f, 0.0f, 0.1f);
    float lowLevel = curiosity->getLevel();
    
    curiosity->reset();
    curiosity->update(1.0f, 1.0f, 0.1f);
    float highLevel = curiosity->getLevel();
    
    EXPECT_GT(highLevel, lowLevel);
}

TEST_F(NeuromodulationTest, PredictionErrorComputation) {
    // Test prediction error calculation
    float predicted = 5.0f;
    float actual = 8.0f;
    
    predictionError->compute(predicted, actual);
    
    EXPECT_FLOAT_EQ(predictionError->getError(), 3.0f);
    EXPECT_FLOAT_EQ(predictionError->getMagnitude(), 3.0f);
    
    // Negative error
    predictionError->compute(10.0f, 5.0f);
    EXPECT_FLOAT_EQ(predictionError->getError(), -5.0f);
    EXPECT_FLOAT_EQ(predictionError->getMagnitude(), 5.0f);
}

TEST_F(NeuromodulationTest, PredictionErrorHistory) {
    predictionError->compute(1.0f, 2.0f);
    predictionError->compute(2.0f, 4.0f);
    predictionError->compute(3.0f, 6.0f);
    
    const auto& history = predictionError->getHistory();
    EXPECT_EQ(history.size(), 3);
    EXPECT_FLOAT_EQ(history[0], 1.0f);  // 2 - 1 = 1
    EXPECT_FLOAT_EQ(history[1], 2.0f);  // 4 - 2 = 2
    EXPECT_FLOAT_EQ(history[2], 3.0f);  // 6 - 3 = 3
}

TEST_F(NeuromodulationTest, NeuromodulationIntegration) {
    // Test integration between neuromodulation systems
    reward->add(10.0f);
    novelty->compute(std::vector<float>(10, 0.0f), std::vector<float>(10, 1.0f));
    curiosity->update(novelty->getLevel(), predictionError->getError(), 0.1f);
    
    // Verify systems are working together
    EXPECT_GT(curiosity->getLevel(), 0.0f);
}

TEST_F(NeuromodulationTest, NeuromodulatorInitialization) {
    // Test that all neuromodulators initialize properly
    EXPECT_NE(curiosity.get(), nullptr);
    EXPECT_NE(reward.get(), nullptr);
    EXPECT_NE(novelty.get(), nullptr);
    EXPECT_NE(predictionError.get(), nullptr);
    
    // Verify they have correct brain references
    EXPECT_EQ(curiosity->getBrain(), brain.get());
    EXPECT_EQ(predictionError->getBrain(), brain.get());
}

TEST_F(NeuromodulationTest, CuriosityDecay) {
    // Test that curiosity decays over time
    curiosity->update(1.0f, 0.0f, 0.1f); // Set high curiosity
    float initialLevel = curiosity->getLevel();
    
    curiosity->update(0.0f, 0.0f, 0.1f); // No new input
    float afterOneStep = curiosity->getLevel();
    
    // Curiosity should decay over time
    curiosity->update(0.0f, 0.0f, 0.1f); // Another step
    float afterTwoSteps = curiosity->getLevel();
    
    EXPECT_LE(afterTwoSteps, afterOneStep);
}

int runAll() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

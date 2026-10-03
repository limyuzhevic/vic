#include <gtest/gtest.h>
#include "../neuromodulation/Neuromodulator.hpp"

class NeuromodulatorTest : public ::testing::Test {
protected:
    nlm::Dopamine dopamine;
};

TEST_F(NeuromodulatorTest, InitialState) {
    EXPECT_STREQ(dopamine.getName(), "DA");
    EXPECT_FLOAT_EQ(dopamine.getLevel(), 0.0f);
    EXPECT_FLOAT_EQ(dopamine.getPlasticityFactor(), 0.5f); // Default placeholder
}

TEST_F(NeuromodulatorTest, SetLevel) {
    dopamine.setLevel(0.5f);
    EXPECT_FLOAT_EQ(dopamine.getLevel(), 0.5f);
    
    dopamine.setLevel(1.5f);
    EXPECT_FLOAT_EQ(dopamine.getLevel(), 1.0f); // Clamped to max
    
    dopamine.setLevel(-0.5f);
    EXPECT_FLOAT_EQ(dopamine.getLevel(), 0.0f); // Clamped to min
}

TEST_F(NeuromodulatorTest, PlasticityFactor) {
    EXPECT_FLOAT_EQ(dopamine.getPlasticityFactor(), 0.5f);
    
    dopamine.setLevel(1.0f);
    EXPECT_FLOAT_EQ(dopamine.getPlasticityFactor(), 1.0f);
}

TEST_F(NeuromodulatorTest, RewardSignaling) {
    float initialLevel = dopamine.getLevel();
    dopamine.signalReward(1.0f);
    EXPECT_GE(dopamine.getLevel(), initialLevel);
}

TEST_F(NeuromodulatorTest, PredictionErrorSignaling) {
    float initialLevel = dopamine.getLevel();
    dopamine.signalRewardPredictionError(0.5f);
    EXPECT_GE(dopamine.getLevel(), initialLevel);
}

TEST_F(NeuromodulatorTest, Update) {
    // Test that update doesn't crash
    dopamine.setLevel(1.0f);
    dopamine.update(0.001f); // Small timestep
    EXPECT_LE(dopamine.getLevel(), 1.0f);
}
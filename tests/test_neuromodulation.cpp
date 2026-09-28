// tests/test_neuromodulation.cpp
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"
#include <gtest/gtest.h>

class DopamineTest : public ::testing::Test {
protected:
    void SetUp() override {
        dopamine = std::make_unique<Dopamine>();
    }
    
    std::unique_ptr<Dopamine> dopamine;
};

TEST_F(DopamineTest, InitialState) {
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 0.0f);
    EXPECT_FLOAT_EQ(dopamine->getPlasticityFactor(), 0.5f);
}

TEST_F(DopamineTest, SetLevel) {
    dopamine->setLevel(0.5f);
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 0.5f);
}

TEST_F(DopamineTest, LevelClamping) {
    dopamine->setLevel(-0.5f);
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 0.0f); // Clamped to minimum
    
    dopamine->setLevel(1.5f);
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 1.0f); // Clamped to maximum
}

TEST_F(DopamineTest, PlasticityFactor) {
    dopamine->setLevel(0.0f);
    EXPECT_FLOAT_EQ(dopamine->getPlasticityFactor(), 0.5f);
    
    dopamine->setLevel(1.0f);
    EXPECT_FLOAT_EQ(dopamine->getPlasticityFactor(), 1.5f);
}

TEST_F(DopamineTest, UpdateDecay) {
    dopamine->setLevel(1.0f);
    dopamine->update(1.0f);
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 0.9f); // Decays with rate 0.1
}

TEST_F(DopamineTest, SignalReward) {
    dopamine->setLevel(0.0f);
    dopamine->signalReward(1.0f);
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 1.0f);
}

TEST_F(DopamineTest, SignalPredictionError) {
    dopamine->setLevel(0.5f);
    dopamine->signalRewardPredictionError(0.3f);
    EXPECT_FLOAT_EQ(dopamine->getLevel(), 0.8f);
}

class CuriosityTest : public ::testing::Test {
protected:
    void SetUp() override {
        curiosity = std::make_unique<Curiosity>();
    }
    
    std::unique_ptr<Curiosity> curiosity;
};

TEST_F(CuriosityTest, InitialState) {
    EXPECT_FLOAT_EQ(curiosity->getLevel(), 0.0f);
}

TEST_F(CuriosityTest, Update) {
    curiosity->setLevel(0.5f);
    curiosity->update(1.0f);
    EXPECT_FLOAT_EQ(curiosity->getLevel(), 0.45f); // Decays with rate 0.1
}

class NoveltyTest : public ::testing::Test {
protected:
    void SetUp() override {
        novelty = std::make_unique<Novelty>();
    }
    
    std::unique_ptr<Novelty> novelty;
};

TEST_F(NoveltyTest, InitialState) {
    EXPECT_FLOAT_EQ(novelty->getLevel(), 0.0f);
}

TEST_F(NoveltyTest, Update) {
    novelty->setLevel(1.0f);
    novelty->update(1.0f);
    EXPECT_FLOAT_EQ(novelty->getLevel(), 0.9f); // Decays with rate 0.1
}

class PredictionErrorTest : public ::testing::Test {
protected:
    void SetUp() override {
        predError = std::make_unique<PredictionError>();
    }
    
    std::unique_ptr<PredictionError> predError;
};

TEST_F(PredictionErrorTest, InitialState) {
    EXPECT_FLOAT_EQ(predError->getLevel(), 0.0f);
}

TEST_F(PredictionErrorTest, Update) {
    predError->setLevel(0.5f);
    predError->update(1.0f);
    EXPECT_FLOAT_EQ(predError->getLevel(), 0.45f); // Decays with rate 0.1
}

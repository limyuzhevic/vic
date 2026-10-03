#include <gtest/gtest.h>
#include "../development/Synaptogenesis.hpp"
#include "../development/Maturation.hpp"
#include "../development/Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"

class SynaptogenesisTest : public ::testing::Test {
protected:
    nlm::Synaptogenesis synaptogenesis;
    nlm::Brain* brain;
    nlm::RandomGenerator rng{42};
};

TEST_F(SynaptogenesisTest, InitialProperties) {
    EXPECT_GT(synaptogenesis.getFormationRate(), 0.0f);
    EXPECT_GT(synaptogenesis.getTargetDensity(), 0.0f);
}

TEST_F(SynaptogenesisTest, SetFormationRate) {
    float testRate = 0.01f;
    synaptogenesis.setFormationRate(testRate);
    EXPECT_FLOAT_EQ(synaptogenesis.getFormationRate(), testRate);
}

TEST_F(SynaptogenesisTest, Update) {
    // Test that update doesn't crash
    synaptogenesis.update(brain, rng);
}

class MaturationTest : public ::testing::Test {
protected:
    nlm::Maturation maturation;
    nlm::Brain* brain;
};

TEST_F(MaturationTest, InitialProperties) {
    EXPECT_FLOAT_EQ(maturation.getProgress(), 0.0f);
    EXPECT_LT(maturation.getMatureThreshold(), 0.0f); // Negative threshold
    EXPECT_LT(maturation.getMatureRestingPotential(), 0.0f); // Negative potential
}

TEST_F(MaturationTest, SetProgress) {
    maturation.setProgress(0.5f);
    EXPECT_FLOAT_EQ(maturation.getProgress(), 0.5f);
    
    maturation.setProgress(1.5f);
    EXPECT_FLOAT_EQ(maturation.getProgress(), 1.0f); // Clamped
}

TEST_F(MaturationTest, Update) {
    maturation.update(brain, 1000);
    EXPECT_GE(maturation.getProgress(), 0.0f);
}

class PruningTest : public ::testing::Test {
protected:
    nlm::Pruning pruning;
    nlm::Brain* brain;
    nlm::RandomGenerator rng{42};
};

TEST_F(PruningTest, InitialProperties) {
    EXPECT_GT(pruning.getThreshold(), 0.0f);
    EXPECT_GT(pruning.getPruningRate(), 0.0f);
}

TEST_F(PruningTest, SetThreshold) {
    float testThreshold = 0.05f;
    pruning.setThreshold(testThreshold);
    EXPECT_FLOAT_EQ(pruning.getThreshold(), testThreshold);
}

TEST_F(PruningTest, SetPruningRate) {
    float testRate = 0.01f;
    pruning.setPruningRate(testRate);
    EXPECT_FLOAT_EQ(pruning.getPruningRate(), testRate);
}

TEST_F(PruningTest, Update) {
    // Test that update doesn't crash
    pruning.update(brain, rng);
}

TEST_F(PruningTest, PruneSynapse) {
    nlm::Synapse* dummySynapse = nullptr; // We can't easily create a valid synapse for testing
    
    // This should not crash, even with nullptr
    bool result = pruning.pruneSynapse(dummySynapse);
    EXPECT_FALSE(result);
}
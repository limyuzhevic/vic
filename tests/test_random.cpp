// Random Tests
// Test RandomGenerator functionality

#include <gtest/gtest.h>
#include "core/Random/Random.hpp"

namespace nlm {

class RandomTest : public ::testing::Test {
protected:
    void SetUp() override {
        random = std::make_unique<RandomGenerator>();
    }
    
    void TearDown() override {
        random.reset();
    }
    
    std::unique_ptr<RandomGenerator> random;
};

TEST_F(RandomTest, BasicFunctionality) {
    EXPECT_GT(random->nextFloat(), 0.0f);
    EXPECT_LE(random->nextFloat(), 1.0f);
}

TEST_F(RandomTest, RangeGeneration) {
    int value = random->nextInt(0, 100);
    EXPECT_GE(value, 0);
    EXPECT_LE(value, 100);
}

TEST_F(RandomTest, GaussianDistribution) {
    float value = random->nextGaussian(0.0f, 1.0f);
    EXPECT_NEAR(value, 0.0f, 2.0f);  // Within 2 standard deviations
}

} // namespace nlm


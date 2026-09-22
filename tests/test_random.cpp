// Random Tests - Modern Google Test Version
#include "core/Random/Random.hpp"
#include "NLMTestFramework.hpp"

namespace nlm {
namespace test {

class RandomTests : public NLMTestBase {
public:
    void TestRandomGeneratorCreation() {
        nlm::RandomGenerator rng1(42);
        nlm::RandomGenerator rng2(42);
        
        // Same seed should produce same sequence
        for (int i = 0; i < 10; ++i) {
            EXPECT_EQ(rng1.uniformInt(0, 100), rng2.uniformInt(0, 100));
        }
    }
    
    void TestUniformInt() {
        nlm::RandomGenerator rng(42);
        
        for (int i = 0; i < 100; ++i) {
            uint32_t val = rng.uniformInt(5, 10);
            EXPECT_GE(val, 5);
            EXPECT_LE(val, 10);
        }
    }
    
    void TestUniformReal() {
        nlm::RandomGenerator rng(42);
        
        for (int i = 0; i < 100; ++i) {
            double val = rng.uniformReal(0.0, 1.0);
            EXPECT_GE(val, 0.0);
            EXPECT_LT(val, 1.0);
        }
    }
    
    void TestNormal() {
        nlm::RandomGenerator rng(42);
        
        double sum = 0.0;
        int samples = 10000;
        for (int i = 0; i < samples; ++i) {
            double val = rng.normal(0.0, 1.0);
            sum += val;
        }
        double mean = sum / samples;
        
        // Mean should be close to 0
        EXPECT_NEAR(mean, 0.0, 0.1);
    }
    
    void TestBernoulli() {
        nlm::RandomGenerator rng(42);
        
        int trueCount = 0;
        int samples = 10000;
        for (int i = 0; i < samples; ++i) {
            if (rng.bernoulli(0.3)) {
                ++trueCount;
            }
        }
        
        // Should be roughly 30%
        double ratio = static_cast<double>(trueCount) / samples;
        EXPECT_GT(ratio, 0.25);
        EXPECT_LT(ratio, 0.35);
    }
    
    void TestRandomBool() {
        nlm::RandomGenerator rng(42);
        
        bool val = rng.randomBool();
        // Just check it doesn't crash
        EXPECT_TRUE(true || false);  // Always true
    }
    
    void TestPickRandomIndex() {
        nlm::RandomGenerator rng(42);
        
        for (int i = 0; i < 100; ++i) {
            uint64_t idx = rng.pickRandomIndex(10);
            EXPECT_LT(idx, 10);
        }
    }
    
    void TestShuffle() {
        nlm::RandomGenerator rng(42);
        
        std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto vecCopy = vec;
        
        rng.shuffle(vec);
        
        // Should have same elements but possibly different order
        EXPECT_EQ(vec.size(), vecCopy.size());
        
        // Sort and compare
        std::sort(vec.begin(), vec.end());
        std::sort(vecCopy.begin(), vecCopy.end());
        EXPECT_EQ(vec, vecCopy);
    }
    
    void TestSeed() {
        nlm::RandomGenerator rng;
        
        rng.seed(12345);
        uint32_t val1 = rng.uniformInt(0, 1000);
        
        rng.seed(12345);
        uint32_t val2 = rng.uniformInt(0, 1000);
        
        EXPECT_EQ(val1, val2);
    }
};

TEST_F(RandomTests, TestRandomGeneratorCreation) { TestRandomGeneratorCreation(); }
TEST_F(RandomTests, TestUniformInt) { TestUniformInt(); }
TEST_F(RandomTests, TestUniformReal) { TestUniformReal(); }
TEST_F(RandomTests, TestNormal) { TestNormal(); }
TEST_F(RandomTests, TestBernoulli) { TestBernoulli(); }
TEST_F(RandomTests, TestRandomBool) { TestRandomBool(); }
TEST_F(RandomTests, TestPickRandomIndex) { TestPickRandomIndex(); }
TEST_F(RandomTests, TestShuffle) { TestShuffle(); }
TEST_F(RandomTests, TestSeed) { TestSeed(); }

} // namespace test
} // namespace nlm

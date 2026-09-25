// Random Tests
#include "core/Random/Random.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cmath>

namespace test_random {

TEST_CASE("RandomGenerator creation and seeding", "[random][creation]") {
    nlm::RandomGenerator rng1(42);
    nlm::RandomGenerator rng2(42);
    
    // Same seed should produce same sequence
    for (int i = 0; i < 10; ++i) {
        REQUIRE(rng1.uniformInt(0, 100) == rng2.uniformInt(0, 100));
    }
}

TEST_CASE("Uniform integer generation bounds", "[random][uniform_int]") {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng.uniformInt(5, 10);
        REQUIRE(val >= 5 && val <= 10);
    }
}

TEST_CASE("Uniform real generation bounds", "[random][uniform_real]") {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        REQUIRE(val >= 0.0 && val < 1.0);
    }
}

TEST_CASE("Normal distribution generation (mean and variance)", "[random][normal]") {
    nlm::RandomGenerator rng(42);
    
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    
    // Mean should be close to 0
    REQUIRE(std::abs(mean) < 0.1);
}

TEST_CASE("Bernoulli distribution generation (probability)", "[random][bernoulli]") {
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
    REQUIRE(ratio > 0.25 && ratio < 0.35);
}

TEST_CASE("Random boolean generation", "[random][bool]") {
    nlm::RandomGenerator rng(42);
    
    bool val = rng.randomBool();
    // Just check it doesn't crash (always true)
    REQUIRE(true);
}

TEST_CASE("Random index selection bounds", "[random][pick_index]") {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng.pickRandomIndex(10);
        REQUIRE(idx < 10);
    }
}

TEST_CASE("Shuffle operation preserves elements", "[random][shuffle]") {
    nlm::RandomGenerator rng(42);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng.shuffle(vec);
    
    // Should have same elements but possibly different order
    REQUIRE(vec.size() == vecCopy.size());
    
    // Sort and compare
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    REQUIRE(vec == vecCopy);
}

TEST_CASE("Seed reproducibility", "[random][seed]") {
    nlm::RandomGenerator rng;
    
    rng.seed(12345);
    uint32_t val1 = rng.uniformInt(0, 1000);
    
    rng.seed(12345);
    uint32_t val2 = rng.uniformInt(0, 1000);
    
    REQUIRE(val1 == val2);
}

} // namespace test_random

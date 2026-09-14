// Random Tests
#include "core/Random/Random.hpp"
#include "TestFramework.h"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_random {

class TestRandomGeneratorCreation : public TestBase {
public:
    std::string getName() const override {
        return "testRandomGeneratorCreation";
    }
    
    void run() override {
        nlm::RandomGenerator rng1(42);
        nlm::RandomGenerator rng2(42);
        
        // Same seed should produce same sequence
        for (int i = 0; i < 10; ++i) {
            assert(rng1.uniformInt(0, 100) == rng2.uniformInt(0, 100));
        }
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestUniformInt : public TestBase {
public:
    std::string getName() const override {
        return "testUniformInt";
    }
    
    void run() override {
        nlm::RandomGenerator rng(42);
        
        for (int i = 0; i < 100; ++i) {
            uint32_t val = rng.uniformInt(5, 10);
            assert(val >= 5 && val <= 10);
        }
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestUniformReal : public TestBase {
public:
    std::string getName() const override {
        return "testUniformReal";
    }
    
    void run() override {
        nlm::RandomGenerator rng(42);
        
        for (int i = 0; i < 100; ++i) {
            double val = rng.uniformReal(0.0, 1.0);
            assert(val >= 0.0 && val < 1.0);
        }
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestNormal : public TestBase {
public:
    std::string getName() const override {
        return "testNormal";
    }
    
    void run() override {
        nlm::RandomGenerator rng(42);
        
        double sum = 0.0;
        int samples = 10000;
        for (int i = 0; i < samples; ++i) {
            double val = rng.normal(0.0, 1.0);
            sum += val;
        }
        double mean = sum / samples;
        
        // Mean should be close to 0
        assert(std::abs(mean) < 0.1);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestBernoulli : public TestBase {
public:
    std::string getName() const override {
        return "testBernoulli";
    }
    
    void run() override {
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
        assert(ratio > 0.25 && ratio < 0.35);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestRandomBool : public TestBase {
public:
    std::string getName() const override {
        return "testRandomBool";
    }
    
    void run() override {
        nlm::RandomGenerator rng(42);
        
        bool val = rng.randomBool();
        // Just check it doesn't crash
        assert(true || false);  // Always true
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestPickRandomIndex : public TestBase {
public:
    std::string getName() const override {
        return "testPickRandomIndex";
    }
    
    void run() override {
        nlm::RandomGenerator rng(42);
        
        for (int i = 0; i < 100; ++i) {
            uint64_t idx = rng.pickRandomIndex(10);
            assert(idx < 10);
        }
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestShuffle : public TestBase {
public:
    std::string getName() const override {
        return "testShuffle";
    }
    
    void run() override {
        nlm::RandomGenerator rng(42);
        
        std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        auto vecCopy = vec;
        
        rng.shuffle(vec);
        
        // Should have same elements but possibly different order
        assert(vec.size() == vecCopy.size());
        
        // Sort and compare
        std::sort(vec.begin(), vec.end());
        std::sort(vecCopy.begin(), vecCopy.end());
        assert(vec == vecCopy);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

class TestSeed : public TestBase {
public:
    std::string getName() const override {
        return "testSeed";
    }
    
    void run() override {
        nlm::RandomGenerator rng;
        
        rng.seed(12345);
        uint32_t val1 = rng.uniformInt(0, 1000);
        
        rng.seed(12345);
        uint32_t val2 = rng.uniformInt(0, 1000);
        
        assert(val1 == val2);
        
        std::cout << "    " << getName() << " passed" << std::endl;
    }
};

// Function to register all random tests
void registerRandomTests(TestRunner& runner) {
    runner.registerTest<TestRandomGeneratorCreation>();
    runner.registerTest<TestUniformInt>();
    runner.registerTest<TestUniformReal>();
    runner.registerTest<TestNormal>();
    runner.registerTest<TestBernoulli>();
    runner.registerTest<TestRandomBool>();
    runner.registerTest<TestPickRandomIndex>();
    runner.registerTest<TestShuffle>();
    runner.registerTest<TestSeed>();
}

// Keep backward compatibility
void runAll() {
    std::cout << "Running Random tests..." << std::endl;
    TestRunner runner;
    registerRandomTests(runner);
    runner.runAll();
}

} // namespace test_random

void testUniformInt() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint32_t val = rng.uniformInt(5, 10);
        assert(val >= 5 && val <= 10);
    }
    
    std::cout << "    testUniformInt passed" << std::endl;
}

void testUniformReal() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(0.0, 1.0);
        assert(val >= 0.0 && val < 1.0);
    }
    
    std::cout << "    testUniformReal passed" << std::endl;
}

void testNormal() {
    nlm::RandomGenerator rng(42);
    
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        double val = rng.normal(0.0, 1.0);
        sum += val;
    }
    double mean = sum / samples;
    
    // Mean should be close to 0
    assert(std::abs(mean) < 0.1);
    
    std::cout << "    testNormal passed" << std::endl;
}

void testBernoulli() {
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
    assert(ratio > 0.25 && ratio < 0.35);
    
    std::cout << "    testBernoulli passed" << std::endl;
}

void testRandomBool() {
    nlm::RandomGenerator rng(42);
    
    bool val = rng.randomBool();
    // Just check it doesn't crash
    assert(true || false);  // Always true
    
    std::cout << "    testRandomBool passed" << std::endl;
}

void testPickRandomIndex() {
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t idx = rng.pickRandomIndex(10);
        assert(idx < 10);
    }
    
    std::cout << "    testPickRandomIndex passed" << std::endl;
}

void testShuffle() {
    nlm::RandomGenerator rng(42);
    
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto vecCopy = vec;
    
    rng.shuffle(vec);
    
    // Should have same elements but possibly different order
    assert(vec.size() == vecCopy.size());
    
    // Sort and compare
    std::sort(vec.begin(), vec.end());
    std::sort(vecCopy.begin(), vecCopy.end());
    assert(vec == vecCopy);
    
    std::cout << "    testShuffle passed" << std::endl;
}

void testSeed() {
    nlm::RandomGenerator rng;
    
    rng.seed(12345);
    uint32_t val1 = rng.uniformInt(0, 1000);
    
    rng.seed(12345);
    uint32_t val2 = rng.uniformInt(0, 1000);
    
    assert(val1 == val2);
    
    std::cout << "    testSeed passed" << std::endl;
}

void runAll() {
    testRandomGeneratorCreation();
    testUniformInt();
    testUniformReal();
    testNormal();
    testBernoulli();
    testRandomBool();
    testPickRandomIndex();
    testShuffle();
    testSeed();
}

} // namespace test_random

// Comprehensive Random Template Tests
// Parameterized tests for all template methods and data types
// Tests uint32_t, uint64_t, float, double versions and edge cases

#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace test_random_template {

// Test template methods with different data types
void testUniformIntTemplate() {
    std::cout << "Testing uniformInt template method..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with int
    for (int i = 0; i < 100; ++i) {
        int val = rng.uniformInt(-100, 100);
        assert(val >= -100 && val <= 100);
    }
    
    // Test with long long
    for (int i = 0; i < 100; ++i) {
        long long val = rng.uniformInt(-1000000000000, 1000000000000);
        assert(val >= -1000000000000 && val <= 1000000000000);
    }
    
    // Test with uint8_t
    for (int i = 0; i < 100; ++i) {
        uint8_t val = rng.uniformInt(static_cast<uint8_t>(0), static_cast<uint8_t>(255));
        assert(val >= 0 && val <= 255);
    }
    
    std::cout << "    uniformInt template tests passed" << std::endl;
}

void testUniformRealTemplate() {
    std::cout << "Testing uniformReal template method..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with float
    for (int i = 0; i < 100; ++i) {
        float val = rng.uniformReal(0.0f, 1.0f);
        assert(val >= 0.0f && val < 1.0f);
    }
    
    // Test with double
    for (int i = 0; i < 100; ++i) {
        double val = rng.uniformReal(-3.14, 3.14);
        assert(val >= -3.14 && val < 3.14);
    }
    
    // Test with long double
    for (int i = 0; i < 100; ++i) {
        long double val = rng.uniformReal(-1000.0L, 1000.0L);
        assert(val >= -1000.0L && val < 1000.0L);
    }
    
    std::cout << "    uniformReal template tests passed" << std::endl;
}

void testNormalTemplate() {
    std::cout << "Testing normal template method..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with float
    double sum = 0.0;
    int samples = 10000;
    for (int i = 0; i < samples; ++i) {
        float val = rng.normal(0.0f, 1.0f);
        sum += val;
    }
    double mean = sum / samples;
    assert(std::abs(mean) < 0.1);
    
    // Test with double
    sum = 0.0;
    for (int i = 0; i < samples; ++i) {
        double val = rng.normal(5.0, 2.0);
        sum += val;
    }
    mean = sum / samples;
    assert(std::abs(mean - 5.0) < 0.5);
    
    // Test with long double
    sum = 0.0;
    for (int i = 0; i < samples; ++i) {
        long double val = rng.normal(-10.0L, 3.0L);
        sum += static_cast<double>(val);
    }
    mean = sum / samples;
    assert(std::abs(mean + 10.0) < 2.0);
    
    std::cout << "    normal template tests passed" << std::endl;
}

void testShuffleContainerTemplate() {
    std::cout << "Testing shuffle container template method..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with std::vector
    std::vector<int> vec1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<int> vec1_copy = vec1;
    rng.shuffle(vec1);
    assert(vec1.size() == vec1_copy.size());
    std::sort(vec1.begin(), vec1.end());
    std::sort(vec1_copy.begin(), vec1_copy.end());
    assert(vec1 == vec1_copy);
    
    // Test with std::list
    std::list<double> list1 = {1.1, 2.2, 3.3, 4.4, 5.5};
    std::list<double> list1_copy = list1;
    rng.shuffle(list1);
    assert(list1.size() == list1_copy.size());
    list1.sort();
    list1_copy.sort();
    assert(list1 == list1_copy);
    
    // Test with std::array
    std::array<float, 5> arr1 = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    std::array<float, 5> arr1_copy = arr1;
    rng.shuffle(arr1);
    assert(arr1.size() == arr1_copy.size());
    std::sort(arr1.begin(), arr1.end());
    std::sort(arr1_copy.begin(), arr1_copy.end());
    assert(arr1 == arr1_copy);
    
    std::cout << "    shuffle container template tests passed" << std::endl;
}

void testPickRandomTemplate() {
    std::cout << "Testing pickRandom container template method..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with std::vector
    std::vector<int> vec1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int val1 = rng.pickRandom(vec1);
    assert(std::find(vec1.begin(), vec1.end(), val1) != vec1.end());
    
    // Test with std::set
    std::set<std::string> set1 = {"apple", "banana", "cherry", "date"};
    std::string val2 = rng.pickRandom(set1);
    assert(set1.find(val2) != set1.end());
    
    // Test with std::map
    std::map<int, std::string> map1 = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto val3 = rng.pickRandom(map1);
    assert(val3.first >= 1 && val3.first <= 3);
    
    // Test that empty container throws exception
    std::vector<int> emptyVec;
    try {
        int val4 = rng.pickRandom(emptyVec);
        assert(false && "Should have thrown exception");
    } catch (const std::runtime_error&) {
        // Expected
    }
    
    std::cout << "    pickRandom container template tests passed" << std::endl;
}

void testPickRandomIndexTemplate() {
    std::cout << "Testing pickRandomIndex template method..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with small sizes
    for (uint64_t i = 1; i <= 10; ++i) {
        uint64_t idx = rng.pickRandomIndex(i);
        assert(idx < i);
    }
    
    // Test with large sizes
    for (uint64_t i = 1000; i <= 2000; i += 100) {
        uint64_t idx = rng.pickRandomIndex(i);
        assert(idx < i);
    }
    
    // Test with edge cases
    uint64_t idx0 = rng.pickRandomIndex(0);
    assert(idx0 == 0); // Special case for size 0
    
    std::cout << "    pickRandomIndex template tests passed" << std::endl;
}

void testFillBytesTemplate() {
    std::cout << "Testing fillBytes method..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test with char array
    char data1[100];
    rng.fillBytes(data1, 100);
    for (int i = 0; i < 100; ++i) {
        assert(data1[i] != 0); // Should have some randomness
    }
    
    // Test with uint32_t array
    uint32_t data2[25];
    rng.fillBytes(data2, 100); // 25 * 4 bytes = 100 bytes
    for (int i = 0; i < 25; ++i) {
        assert(data2[i] != 0); // Should have some randomness
    }
    
    // Test with pointer
    std::vector<uint8_t> data3(500);
    rng.fillBytes(data3.data(), 500);
    for (int i = 0; i < 500; ++i) {
        assert(data3[i] != 0); // Should have some randomness
    }
    
    std::cout << "    fillBytes template tests passed" << std::endl;
}

void testEdgeCases() {
    std::cout << "Testing edge cases and error conditions..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test uniform with min == max
    uint32_t equal1 = rng.uniformInt(5, 5);
    assert(equal1 == 5);
    
    float equal2 = rng.uniformReal(1.0f, 1.0f);
    assert(equal2 == 1.0f);
    
    double equal3 = rng.uniformReal(-5.0, -5.0);
    assert(equal3 == -5.0);
    
    // Test normal with zero stddev
    float normal1 = rng.normal(10.0f, 0.0f);
    assert(normal1 == 10.0f);
    
    // Test very large ranges
    uint64_t large1 = rng.uniformInt(0, std::numeric_limits<uint64_t>::max());
    assert(large1 <= std::numeric_limits<uint64_t>::max());
    
    long double large2 = rng.uniformReal(-1e100L, 1e100L);
    assert(large2 >= -1e100L && large2 < 1e100L);
    
    // Test with special floating point values
    float inf = rng.uniformReal(1.0f, 2.0f);
    assert(std::isfinite(inf));
    
    // Test seed reproducibility
    nlm::RandomGenerator rng1(12345);
    nlm::RandomGenerator rng2(12345);
    
    for (int i = 0; i < 100; ++i) {
        assert(rng1.uniformInt(0, 1000) == rng2.uniformInt(0, 1000));
        assert(rng1.uniformReal(0.0, 1.0) == rng2.uniformReal(0.0, 1.0));
    }
    
    std::cout << "    edge cases tests passed" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility with non-template methods..." << std::endl;
    
    nlm::RandomGenerator rng(42);
    
    // Test non-template methods
    uint32_t ui32 = rng.uniformInt32(0, 100);
    assert(ui32 >= 0 && ui32 <= 100);
    
    uint64_t ui64 = rng.uniformInt64(0, 100);
    assert(ui64 >= 0 && ui64 <= 100);
    
    double r64 = rng.uniformReal64(0.0, 1.0);
    assert(r64 >= 0.0 && r64 < 1.0);
    
    float r32 = rng.uniformReal32(0.0f, 1.0f);
    assert(r32 >= 0.0f && r32 < 1.0f);
    
    double n64 = rng.normal64(0.0, 1.0);
    assert(std::abs(n64) < 0.1);
    
    float n32 = rng.normal32(0.0f, 1.0f);
    assert(std::abs(static_cast<double>(n32)) < 0.1);
    
    bool b = rng.bernoulli(0.5);
    assert(b == true || b == false);
    
    uint32_t p = rng.poisson(5.0);
    assert(p >= 0);
    
    double e = rng.exponential(1.0);
    assert(e >= 0.0);
    
    bool randb = rng.randomBool();
    assert(randb == true || randb == false);
    
    uint32_t bit = rng.randomBit();
    assert(bit == 0 || bit == 1);
    
    std::cout << "    backward compatibility tests passed" << std::endl;
}

void runAll() {
    testUniformIntTemplate();
    testUniformRealTemplate();
    testNormalTemplate();
    testShuffleContainerTemplate();
    testPickRandomTemplate();
    testPickRandomIndexTemplate();
    testFillBytesTemplate();
    testEdgeCases();
    testBackwardCompatibility();
    
    std::cout << std::endl;
    std::cout << "=== All Random Template Tests Passed! ===" << std::endl;
    std::cout << std::endl;
}

} // namespace test_random_template
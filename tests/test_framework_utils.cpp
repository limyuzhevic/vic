#include "test_framework_utils.h"
#include <iostream>

namespace test_utils {

void runAll() {
    std::cout << "Running Framework Utils tests..." << std::endl;
    
    // Test validation utilities
    std::cout << "  Testing ValidationUtils..." << std::endl;
    
    // Test withinRange
    assert(ValidationUtils::withinRange(5, 0, 10));
    assert(!ValidationUtils::withinRange(15, 0, 10));
    
    // Test hasExpectedSize
    std::vector<float> testVec = {1, 2, 3, 4, 5};
    assert(ValidationUtils::hasExpectedSize(testVec, 5));
    assert(!ValidationUtils::hasExpectedSize(testVec, 6));
    
    // Test allPositive
    std::vector<float> posVec = {1.0f, 2.0f, 3.0f};
    assert(ValidationUtils::allPositive(posVec));
    std::vector<float> negVec = {-1.0f, 0.0f, 1.0f};
    assert(!ValidationUtils::allPositive(negVec));
    
    // Test allNonNegative
    std::vector<float> nonNegVec = {0.0f, 1.0f, 5.0f};
    assert(ValidationUtils::allNonNegative(nonNegVec));
    
    // Test normalized
    std::vector<float> normVec = {0.0f, 0.5f, 1.0f};
    assert(ValidationUtils::normalized(normVec));
    
    // Test computeMean
    assert(ValidationUtils::computeMean(testVec) == 3.0f);
    
    // Test computeStdDev
    std::vector<float> data = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    double stddev = ValidationUtils::computeStdDev(data);
    assert(std::abs(stddev - 1.581) < 0.01f);
    
    // Test isMonotonicIncreasing
    std::vector<float> incVec = {1.0f, 2.0f, 3.0f, 4.0f};
    assert(ValidationUtils::isMonotonicIncreasing(incVec));
    
    std::vector<float> decVec = {5.0f, 4.0f, 3.0f, 2.0f};
    assert(ValidationUtils::isMonotonicDecreasing(decVec));
    
    // Test random data generation
    auto randomData = ValidationUtils::generateRandomData(10);
    assert(randomData.size() == 10);
    assert(ValidationUtils::allNonNegative(randomData));
    
    auto spikeTimes = ValidationUtils::generateSpikeTimes(5, 0.0f, 50.0f);
    assert(spikeTimes.size() == 5);
    assert(ValidationUtils::isMonotonicIncreasing(spikeTimes));
    
    auto weights = ValidationUtils::generateWeightValues(5, -1.0f, 1.0f);
    assert(weights.size() == 5);
    
    auto states = ValidationUtils::generateStateVectors(3, 5);
    assert(states.size() == 15);
    
    std::cout << "  PASS" << std::endl;
    
    std::cout << "Framework Utils tests completed successfully!" << std::endl;
}

} // namespace test_utils

// TestUtilities Tests
// Tests for the new TestUtilities functionality

#include <iostream>
#include <cassert>
#include <vector>
#include <cmath>
#include "../src/main/TestUtilities.hpp"

namespace test_test_utilities {

void testCalculateMean() {
    std::vector<float> values = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float mean = nlm::TestUtils::calculateMean(values);
    assert(std::abs(mean - 3.0f) < 0.001f);
}

void testCalculateMeanEmpty() {
    std::vector<float> values = {};
    float mean = nlm::TestUtils::calculateMean(values);
    assert(mean == 0.0f);
}

void testCalculateVariance() {
    std::vector<float> values = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float mean = nlm::TestUtils::calculateMean(values);
    float variance = nlm::TestUtils::calculateVariance(values, mean);
    float expectedVariance = 2.0f; // (1-3)² + (2-3)² + (3-3)² + (4-3)² + (5-3)² = 10/5 = 2.0
    assert(std::abs(variance - expectedVariance) < 0.001f);
}

void testCountSignificantChanges() {
    std::vector<float> before = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
    std::vector<float> after = {0.11f, 0.21f, 0.31f, 0.4f, 0.5f}; // Only first two changed by > 0.01
    size_t count = nlm::TestUtils::countSignificantChanges(before, after, 0.01f);
    assert(count == 2);
}

void testLogUtilities() {
    // Test logging doesn't crash
    nlm::TestUtils::logSection("Test Section");
    nlm::TestUtils::logSubsection("Test Subsection");
    nlm::TestUtils::logResult("Test Label", "Test Value");
    nlm::TestUtils::logMetric("Test Metric", 42.0, " units");
    nlm::TestUtils::logProgress(5, 10, "Test Progress");
    nlm::TestUtils::logError("Test Error Message");
}

void runAll() {
    std::cout << "  Testing TestUtilities..." << std::endl;
    
    testCalculateMean();
    std::cout << "    testCalculateMean PASSED" << std::endl;
    
    testCalculateMeanEmpty();
    std::cout << "    testCalculateMeanEmpty PASSED" << std::endl;
    
    testCalculateVariance();
    std::cout << "    testCalculateVariance PASSED" << std::endl;
    
    testCountSignificantChanges();
    std::cout << "    testCountSignificantChanges PASSED" << std::endl;
    
    testLogUtilities();
    std::cout << "    testLogUtilities PASSED" << std::endl;
}

} // namespace test_test_utilities

void runAll() {
    std::cout << "Running TestUtilities tests..." << std::endl;
    test_test_utilities::runAll();
    std::cout << "  TestUtilities tests PASSED" << std::endl;
}
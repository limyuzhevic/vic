// Quick syntax test for NLM Scientific Validation
// This file tests basic compilation without running full tests

#include "tests/test_scientific_validation.h"
#include <iostream>

int main() {
    std::cout << "Testing NLM Scientific Validation syntax..." << std::endl;
    
    // Create test suite instance
    test_scientific_validation::ScientificValidationTestSuite testSuite;
    
    // Test basic functionality
    testSuite.setRandomSeed(42);
    
    std::cout << "Random seed set to: " << testSuite.getRandomSeed() << std::endl;
    
    float testFloat = testSuite.getRandomFloat(0.0f, 1.0f);
    std::cout << "Random float test: " << testFloat << std::endl;
    
    float mean = testSuite.calculateMean(std::vector<float>{1.0f, 2.0f, 3.0f, 4.0f, 5.0f});
    std::cout << "Mean calculation test: " << mean << std::endl;
    
    std::cout << "✓ All syntax tests passed!" << std::endl;
    
    return 0;
}

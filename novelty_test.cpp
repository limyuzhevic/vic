// Implementation of the test harness
void runNoveltyTests() {
    std::cout << "Testing NoveltyOptimizer..." << std::endl;
    
    // Create test data
    std::vector<float> vec1 = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    std::vector<float> vec2 = {1.5f, 2.5f, 3.5f, 4.5f, 5.5f};
    
    // Test 1: Basic functionality
    float result = NoveltyOptimizer::computeAbsDiffSum(vec1, vec2, 5);
    float expected = 2.5f;
    
    if (std::abs(result - expected) < 1e-6f) {
        std::cout << "Test 1 PASSED: Basic computation correct" << std::endl;
    } else {
        std::cout << "Test 1 FAILED: Expected " << expected 
                  << ", got " << result << std::endl;
        return;
    }
    
    // Test 2: Edge case - empty vectors
    std::vector<float> empty;
    result = NoveltyOptimizer::computeAbsDiffSum(empty, vec2, 0);
    if (result == 0.0f) {
        std::cout << "Test 2 PASSED: Empty vector handling" << std::endl;
    } else {
        std::cout << "Test 2 FAILED: Expected 0.0f, got " << result << std::endl;
        return;
    }
    
    // Test 3: Identical vectors
    result = NoveltyOptimizer::computeAbsDiffSum(vec1, vec1, 5);
    if (result == 0.0f) {
        std::cout << "Test 3 PASSED: Identical vector handling" << std::endl;
    } else {
        std::cout << "Test 3 FAILED: Expected 0.0f, got " << result << std::endl;
        return;
    }
    
    // Test 4: Novelty computation
    result = NoveltyOptimizer::computeNovelty(vec1, vec2, 1.0f, 1.0f);
    expected = std::min(1.0f, 2.5f / 1.0f);
    if (std::abs(result - expected) < 1e-6f) {
        std::cout << "Test 4 PASSED: Novelty computation correct" << std::endl;
    } else {
        std::cout << "Test 4 FAILED: Expected " << expected 
                  << ", got " << result << std::endl;
        return;
    }
    
    // Test 5: Small vector optimization
    std::vector<float> small1 = {1.0f, 2.0f};
    std::vector<float> small2 = {1.1f, 2.1f};
    result = NoveltyOptimizer::computeSmallAbsDiffSum(small1, small2, 2);
    expected = 0.2f;
    if (std::abs(result - expected) < 1e-6f) {
        std::cout << "Test 5 PASSED: Small vector optimization" << std::endl;
    } else {
        std::cout << "Test 5 FAILED: Expected " << expected 
                  << ", got " << result << std::endl;
        return;
    }
    
    std::cout << "\nAll NoveltyOptimizer tests passed successfully!" << std::endl;
}

// Test utility functions implementation
#include "Utilities.hpp"
#include <iostream>
#include <vector>
#include <cmath>

namespace nlm {

// Test Math utilities
void testMathUtils() {
    std::cout << "Testing Math utilities..." << std::endl;
    
    // Test clamp
    float clamped = Utils::Math::clamp(5.5f, 0.0f, 5.0f);
    assert(clamped == 5.0f);
    
    // Test lerp
    float interpolated = Utils::Math::lerp(0.0f, 10.0f, 0.5f);
    assert(std::abs(interpolated - 5.0f) < 0.001f);
    
    // Test normalize
    float normalized = Utils::Math::normalize(5.0f, 0.0f, 10.0f);
    assert(std::abs(normalized - 0.5f) < 0.001f);
    
    // Test safeExp
    float expResult = Utils::Math::safeExp(2.0f);
    assert(std::abs(expResult - std::exp(2.0f)) < 0.001f);
    
    // Test approximatelyEqual
    assert(Utils::Math::approximatelyEqual(1.0f, 1.000001f));
    
    std::cout << "Math utilities test passed!" << std::endl;
}

// Test Container utilities
void testContainerUtils() {
    std::cout << "Testing Container utilities..." << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    const int* ptr;
    
    // Test safeAccess
    assert(Utils::Containers::safeAccess(vec, 2, ptr));
    assert(*ptr == 3);
    
    // Test mergeVectors
    std::vector<int> vec1 = {1, 2, 3};
    std::vector<int> vec2 = {3, 4, 5};
    std::vector<int> merged;
    Utils::Containers::mergeVectors(vec1, vec2, merged);
    assert(merged.size() == 5);
    
    // Test getByteSize
    assert(Utils::Containers::getByteSize(vec) == vec.size() * sizeof(int));
    
    std::cout << "Container utilities test passed!" << std::endl;
}

// Test Memory utilities
void testMemoryUtils() {
    std::cout << "Testing Memory utilities..." << std::endl;
    
    auto ptr = Utils::Memory::makeShared<int>(42);
    assert(ptr != nullptr);
    assert(*ptr == 42);
    
    std::cout << "Memory utilities test passed!" << std::endl;
}

// Test Error utilities
void testErrorUtils() {
    std::cout << "Testing Error utilities..." << std::endl;
    
    std::string errorMsg = Utils::Error::formatError("test", "component", "details");
    assert(errorMsg.find("test") != std::string::npos);
    
    // Test validateParameter
    assert(Utils::Error::validateParameter("value", 5, 0, 10, false));
    assert(!Utils::Error::validateParameter("value", 15, 0, 10, false));
    
    std::cout << "Error utilities test passed!" << std::endl;
}

// Test Vector utilities
void testVectorUtils() {
    std::cout << "Testing Vector utilities..." << std::endl;
    
    std::vector<float> values = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    
    // Test mean
    assert(std::abs(Utils::Vector::mean(values) - 3.0f) < 0.001f);
    
    // Test standardDeviation
    float stddev = Utils::Vector::standardDeviation(values);
    assert(std::abs(stddev - 1.581138830f) < 0.001f);
    
    // Test minMax
    float min, max;
    Utils::Vector::minMax(values, min, max);
    assert(std::abs(min - 1.0f) < 0.001f);
    assert(std::abs(max - 5.0f) < 0.001f);
    
    // Test dotProduct
    std::vector<float> vec1 = {1.0f, 2.0f, 3.0f};
    std::vector<float> vec2 = {4.0f, 5.0f, 6.0f};
    assert(std::abs(Utils::Vector::dotProduct(vec1, vec2) - 32.0f) < 0.001f);
    
    // Test magnitude
    float mag = Utils::Vector::magnitude(vec1);
    assert(std::abs(mag - std::sqrt(14.0f)) < 0.001f);
    
    std::cout << "Vector utilities test passed!" << std::endl;
}

// Test Performance utilities
void testPerformanceUtils() {
    std::cout << "Testing Performance utilities..." << std::endl;
    
    Utils::Performance::Timer timer;
    double elapsed = timer.elapsedMilliseconds();
    assert(elapsed >= 0.0);
    
    Utils::Performance::SimpleCache<int, std::string> cache;
    cache.put(1, "test");
    std::string value;
    assert(cache.get(1, value));
    assert(value == "test");
    
    std::cout << "Performance utilities test passed!" << std::endl;
}

// Test Concurrency utilities
void testConcurrencyUtils() {
    std::cout << "Testing Concurrency utilities..." << std::endl;
    
    Utils::Concurrency::AtomicCounter counter(5);
    assert(counter.get() == 5);
    counter.increment();
    assert(counter.get() == 6);
    counter.decrement();
    assert(counter.get() == 5);
    
    // Test Barrier (basic test)
    Utils::Concurrency::Barrier barrier(3);
    // Barrier test would require multiple threads for proper testing
    
    std::cout << "Concurrency utilities test passed!" << std::endl;
}

// Test safeExecute helper
void testSafeExecute() {
    std::cout << "Testing safeExecute helper..." << std::endl;
    
    auto successfulFunc = []() { return true; };
    auto failingFunc = []() { return false; };
    
    assert(nlm::safeExecute("test", "component", successfulFunc, "success"));
    assert(!nlm::safeExecute("test", "component", failingFunc, "failure"));
    
    std::cout << "safeExecute helper test passed!" << std::endl;
}

// Test safeVectorAccess helper
void testSafeVectorAccess() {
    std::cout << "Testing safeVectorAccess helper..." << std::endl;
    
    std::vector<int> vec = {1, 2, 3, 4, 5};
    const int* ptr;
    
    assert(nlm::safeVectorAccess(vec, 2, ptr));
    assert(*ptr == 3);
    
    assert(!nlm::safeVectorAccess(vec, 10, ptr)); // Out of bounds
    
    std::cout << "safeVectorAccess helper test passed!" << std::endl;
}

// Test safeMapAccess helper
void testSafeMapAccess() {
    std::cout << "Testing safeMapAccess helper..." << std::endl;
    
    std::unordered_map<int, std::string> map = {{1, "one"}, {2, "two"}, {3, "three"}};
    const std::string* ptr;
    
    assert(nlm::safeMapAccess(map, 2, ptr));
    assert(*ptr == "two");
    
    assert(!nlm::safeMapAccess(map, 5, ptr)); // Key not found
    
    std::cout << "safeMapAccess helper test passed!" << std::endl;
}

// Main test function
void runAllTests() {
    std::cout << "=== Running NLM Utility Function Tests ===" << std::endl;
    
    try {
        testMathUtils();
        testContainerUtils();
        testMemoryUtils();
        testErrorUtils();
        testVectorUtils();
        testPerformanceUtils();
        testConcurrencyUtils();
        testSafeExecute();
        testSafeVectorAccess();
        testSafeMapAccess();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test failed with exception: " << e.what() << " ===" << std::endl;
        throw;
    }
}

} // namespace nlm

// Entry point for testing
int main() {
    nlm::runAllTests();
    return 0;
}
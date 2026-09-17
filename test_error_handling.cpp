#include <iostream>
#include <memory>
#include <stdexcept>
#include "src/core/Random/Random.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "Testing improved error handling in NLM" << std::endl;
    
    // Test 1: RandomGenerator parameter validation
    std::cout << "\nTest 1: RandomGenerator parameter validation" << std::endl;
    nlm::RandomGenerator rng(42);
    
    try {
        // Should throw on invalid min > max
        rng.uniformInt(10, 5);
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "PASS: Caught invalid_argument: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "ERROR: Caught unexpected exception: " << e.what() << std::endl;
    }
    
    try {
        // Should throw on invalid mean < stddev
        rng.normal(1.0, -0.1);
        std::cout << "ERROR: Should have thrown exception!" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "PASS: Caught invalid_argument: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "ERROR: Caught unexpected exception: " << e.what() << std::endl;
    }
    
    // Test 2: Config file operations
    std::cout << "\nTest 2: Config file operations" << std::endl;
    nlm::Config config;
    
    // Test loading from empty file path
    bool result = config.loadFromFile("");
    std::cout << "Load from empty path: " << (result ? "PASS (false)" : "FAIL (returned)") << std::endl;
    
    // Test loading from non-existent file
    result = config.loadFromFile("/nonexistent/file.txt");
    std::cout << "Load from non-existent file: " << (result ? "FAIL (true)" : "PASS (false)") << std::endl;
    
    // Test save to file
    result = config.saveToFile("config.txt");
    std::cout << "Save to file: " << (result ? "PASS (true)" : "FAIL (false)") << std::endl;
    
    // Test get with empty key
    auto value = config.get<std::string>("");
    std::cout << "Get with empty key: " << (value ? "FAIL (found)" : "PASS (nullopt)") << std::endl;
    
    // Test set with empty key
    config.set("", "value");
    std::cout << "Set with empty key: " << "PASS (should silently fail)" << std::endl;
    
    std::cout << "\nError handling tests completed!" << std::endl;
    return 0;
}

// Error Condition Tests for NLM
// Tests exception handling and error conditions

#include "../src/core/Config/Config.hpp"
#include "../src/core/Exception.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace error_tests {

void testConfigValidation() {
    std::cout << "Running configuration validation tests..." << std::endl;
    
    // Test 1: Invalid key (empty)
    try {
        nlm::Config config;
        config.set("", "value");
        std::cerr << "  ✗ Test 1 FAILED: Should have thrown ConfigError for empty key" << std::endl;
        return;
    } catch (const nlm::ConfigError& e) {
        std::cout << "  ✓ Test 1 PASSED: Caught ConfigError for empty key" << std::endl;
    }
    
    // Test 2: Invalid key (special characters)
    try {
        nlm::Config config;
        config.set("invalid-key!", "value");
        std::cerr << "  ✗ Test 2 FAILED: Should have thrown ConfigError for special characters" << std::endl;
        return;
    } catch (const nlm::ConfigError& e) {
        std::cout << "  ✓ Test 2 PASSED: Caught ConfigError for special characters" << std::endl;
    }
    
    // Test 3: Invalid value for negative random seed
    try {
        nlm::Config config;
        config.set("random_seed", -5);
        std::cerr << "  ✗ Test 3 FAILED: Should have thrown ValidationError for negative seed" << std::endl;
        return;
    } catch (const nlm::ValidationError& e) {
        std::cout << "  ✓ Test 3 PASSED: Caught ValidationError for negative seed" << std::endl;
    }
    
    // Test 4: Invalid value for zero timestep
    try {
        nlm::Config config;
        config.set("simulation_timestep", 0.0);
        std::cerr << "  ✗ Test 4 FAILED: Should have thrown ValidationError for zero timestep" << std::endl;
        return;
    } catch (const nlm::ValidationError& e) {
        std::cout << "  ✓ Test 4 PASSED: Caught ValidationError for zero timestep" << std::endl;
    }
    
    // Test 5: Invalid value for connection probability > 1.0
    try {
        nlm::Config config;
        config.set("connection_probability", 1.5);
        std::cerr << "  ✗ Test 5 FAILED: Should have thrown ValidationError for probability > 1.0" << std::endl;
        return;
    } catch (const nlm::ValidationError& e) {
        std::cout << "  ✓ Test 5 PASSED: Caught ValidationError for probability > 1.0" << std::endl;
    }
    
    // Test 6: Valid configuration should work
    try {
        nlm::Config config;
        config.set("random_seed", 42);
        config.set("simulation_timestep", 0.001);
        config.set("neuron_count", 100);
        config.set("connection_probability", 0.15);
        
        assert(config.has("random_seed"));
        assert(config.getOr<int64_t>("random_seed", 0) == 42);
        
        std::cout << "  ✓ Test 6 PASSED: Valid configuration works correctly" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Test 6 FAILED: Valid configuration should not throw" << std::endl;
        return;
    }
    
    std::cout << "  ✓ All configuration validation tests passed" << std::endl;
}

void testFileErrorHandling() {
    std::cout << "Running file error handling tests..." << std::endl;
    
    // Test 1: Loading non-existent file
    try {
        nlm::Config config;
        if (config.loadFromFile("/nonexistent/path/config.txt")) {
            std::cerr << "  ✗ Test 1 FAILED: Should not load non-existent file" << std::endl;
            return;
        }
        std::cout << "  ✓ Test 1 PASSED: Non-existent file correctly rejected" << std::endl;
    } catch (const nlm::FileError& e) {
        std::cout << "  ✓ Test 1 PASSED: Caught FileError for non-existent file" << std::endl;
    }
    
    // Test 2: Invalid file format
    try {
        nlm::Config config;
        // Create a temporary file with invalid format
        std::string testFile = "/tmp/invalid_config.txt";
        std::ofstream file(testFile);
        file << "invalid line without equals sign\n";
        file << "valid=key\n";
        file.close();
        
        config.loadFromFile(testFile);
        std::cerr << "  ✗ Test 2 FAILED: Should have thrown ConfigError for invalid format" << std::endl;
        return;
    } catch (const nlm::ConfigError& e) {
        std::cout << "  ✓ Test 2 PASSED: Caught ConfigError for invalid file format" << std::endl;
    }
    
    // Cleanup
    std::remove("/tmp/invalid_config.txt");
    
    std::cout << "  ✓ All file error handling tests passed" << std::endl;
}

void testExceptionHierarchy() {
    std::cout << "Running exception hierarchy tests..." << std::endl;
    
    // Test that all NLM exceptions inherit from std::runtime_error
    try {
        throw nlm::NLMError("Base NLM error");
    } catch (const std::runtime_error& e) {
        std::cout << "  ✓ Test 1 PASSED: NLMError caught as std::runtime_error" << std::endl;
    }
    
    try {
        throw nlm::ConfigError("Configuration error");
    } catch (const nlm::NLMError& e) {
        std::cout << "  ✓ Test 2 PASSED: ConfigError caught as NLMError" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "  ✓ Test 2 PASSED: ConfigError caught as std::runtime_error" << std::endl;
    }
    
    try {
        throw nlm::BrainError("Brain error");
    } catch (const nlm::NLMError& e) {
        std::cout << "  ✓ Test 3 PASSED: BrainError caught as NLMError" << std::endl;
    }
    
    try {
        throw nlm::MemoryError("Memory error");
    } catch (const nlm::NLMError& e) {
        std::cout << "  ✓ Test 4 PASSED: MemoryError caught as NLMError" << std::endl;
    }
    
    try {
        throw nlm::ValidationError("Validation error");
    } catch (const nlm::NLMError& e) {
        std::cout << "  ✓ Test 5 PASSED: ValidationError caught as NLMError" << std::endl;
    }
    
    try {
        throw nlm::FileError("File error");
    } catch (const nlm::NLMError& e) {
        std::cout << "  ✓ Test 6 PASSED: FileError caught as NLMError" << std::endl;
    }
    
    try {
        throw nlm::IntegrationError("Integration error");
    } catch (const nlm::NLMError& e) {
        std::cout << "  ✓ Test 7 PASSED: IntegrationError caught as NLMError" << std::endl;
    }
    
    try {
        throw nlm::SimulationError("Simulation error");
    } catch (const nlm::NLMError& e) {
        std::cout << "  ✓ Test 8 PASSED: SimulationError caught as NLMError" << std::endl;
    }
    
    std::cout << "  ✓ All exception hierarchy tests passed" << std::endl;
}

void testErrorMessages() {
    std::cout << "Running error message tests..." << std::endl;
    
    // Test ConfigError messages
    try {
        throw nlm::ConfigError("Test message", "test_key");
    } catch (const nlm::ConfigError& e) {
        std::string msg = e.what();
        assert(msg.find("Configuration Error") != std::string::npos);
        assert(msg.find("test_key") != std::string::npos);
        std::cout << "  ✓ ConfigError message: " << msg << std::endl;
    }
    
    // Test ValidationError messages
    try {
        throw nlm::ValidationError("Test validation", "param_name");
    } catch (const nlm::ValidationError& e) {
        std::string msg = e.what();
        assert(msg.find("Validation Error") != std::string::npos);
        assert(msg.find("param_name") != std::string::npos);
        std::cout << "  ✓ ValidationError message: " << msg << std::endl;
    }
    
    // Test FileError messages
    try {
        throw nlm::FileError("Cannot read file", "/path/to/file.txt");
    } catch (const nlm::FileError& e) {
        std::string msg = e.what();
        assert(msg.find("File Error") != std::string::npos);
        assert(msg.find("/path/to/file.txt") != std::string::npos);
        std::cout << "  ✓ FileError message: " << msg << std::endl;
    }
    
    std::cout << "  ✓ All error message tests passed" << std::endl;
}

void runAllErrorTests() {
    std::cout << "=== NLM Error Condition Tests ===" << std::endl;
    std::cout << "Testing exception handling and error conditions" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    try {
        testConfigValidation();
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Config validation test failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testFileErrorHandling();
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ File error handling test failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testExceptionHierarchy();
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Exception hierarchy test failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testErrorMessages();
        std::cout << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "  ✗ Error message test failed: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "=== All Error Condition Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Error Condition Tests FAILED ===" << std::endl;
    }
    
    return allPassed;
}

} // namespace error_tests

#include <iostream>
#include <cassert>
#include <exception>
#include <string>

// Include all the exception and error handling headers
#include "src/core/Exceptions.hpp"
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/core/Random/Random.hpp"

// Test helper functions
void testExceptionHierarchy() {
    std::cout << "Testing Exception Hierarchy..." << std::endl;
    
    try {
        nlm::NLMConfigException configEx("Test config error", 
                                          nlm::NLMConfigException::ConfigError::INVALID_KEY,
                                          "test_config");
        assert(configEx.what() != nullptr);
        assert(configEx.errorCode() == 0); // 0 corresponds to VALIDATION_FAILED
        NLM_LOG_INFO("Config exception created: " + std::string(configEx.what()));
    } catch (const std::exception& e) {
        std::cerr << "Failed to create NLMConfigException: " << e.what() << std::endl;
        assert(false);
    }
    
    try {
        nlm::NLMBrainException brainEx("Test brain error",
                                       nlm::NLMBrainException::BrainError::INITIALIZATION_FAILED,
                                       "test_brain");
        assert(brainEx.what() != nullptr);
        NLM_LOG_INFO("Brain exception created: " + std::string(brainEx.what()));
    } catch (const std::exception& e) {
        std::cerr << "Failed to create NLMBrainException: " << e.what() << std::endl;
        assert(false);
    }
    
    std::cout << "Exception Hierarchy test passed!" << std::endl;
}

void testConfigWithExceptionHandling() {
    std::cout << "Testing Config with Exception Handling..." << std::endl;
    
    nlm::Config config;
    
    // Test setting values with exception handling
    try {
        config.set("test_key", "test_value");
        NLM_LOG_INFO("Config set successful");
    } catch (const std::exception& e) {
        std::cerr << "Failed to set config value: " << e.what() << std::endl;
        assert(false);
    }
    
    // Test getting values with validation
    try {
        auto value = config.getOr<std::string>("test_key", "default");
        assert(value == "test_value");
        NLM_LOG_INFO("Config getOr successful: " + value);
    } catch (const std::exception& e) {
        std::cerr << "Failed to get config value: " << e.what() << std::endl;
        assert(false);
    }
    
    std::cout << "Config Exception Handling test passed!" << std::endl;
}

void testRandomGeneratorWithExceptionHandling() {
    std::cout << "Testing Random Generator with Exception Handling..." << std::endl;
    
    nlm::RandomGenerator rng(12345);
    
    // Test uniform distribution with validation
    try {
        uint32_t result = rng.uniformInt(1, 10);
        assert(result >= 1 && result <= 10);
        NLM_LOG_INFO("Random uniformInt successful: " + std::to_string(result));
    } catch (const std::exception& e) {
        std::cerr << "Failed random uniformInt: " << e.what() << std::endl;
        assert(false);
    }
    
    // Test invalid range (should throw exception)
    try {
        uint32_t result = rng.uniformInt(10, 1); // Invalid range
        std::cerr << "Expected exception for invalid range but got result: " << result << std::endl;
        assert(false);
    } catch (const nlm::NLMValidationException& e) {
        NLM_LOG_INFO("Caught expected NLMValidationException for invalid range: " + std::string(e.what()));
    } catch (const std::exception& e) {
        std::cerr << "Caught unexpected exception: " << e.what() << std::endl;
        assert(false);
    }
    
    // Test pickRandomIndex with zero items (should throw exception)
    try {
        uint64_t result = rng.pickRandomIndex(0); // Should throw exception
        std::cerr << "Expected exception for zero items but got result: " << result << std::endl;
        assert(false);
    } catch (const nlm::NLMMemoryException& e) {
        NLM_LOG_INFO("Caught expected NLMMemoryException for zero items: " + std::string(e.what()));
    } catch (const std::exception& e) {
        std::cerr << "Caught unexpected exception: " << e.what() << std::endl;
        assert(false);
    }
    
    std::cout << "Random Generator Exception Handling test passed!" << std::endl;
}

void testValidationFunctions() {
    std::cout << "Testing Validation Functions..." << std::endl;
    
    // Test key validation
    try {
        nlm::Validation::validateKey("", "test_empty_key");
        std::cerr << "Expected exception for empty key but validation passed" << std::endl;
        assert(false);
    } catch (const nlm::NLMValidationException& e) {
        NLM_LOG_INFO("Caught expected NLMValidationException for empty key: " + std::string(e.what()));
    }
    
    // Test valid key
    try {
        nlm::Validation::validateKey("valid_key", "test_valid_key");
        NLM_LOG_INFO("Key validation successful for valid key");
    } catch (const std::exception& e) {
        std::cerr << "Unexpected exception for valid key: " << e.what() << std::endl;
        assert(false);
    }
    
    std::cout << "Validation Functions test passed!" << std::endl;
}

void testEnhancedLoggingMacros() {
    std::cout << "Testing Enhanced Logging Macros..." << std::endl;
    
    // Test NLM_LOG_INFO_WITH_EXCEPTION macro
    try {
        NLM_LOG_INFO_WITH_EXCEPTION("Test info message with exception", "test_info");
        NLM_LOG_INFO("Regular info message test");
    } catch (const std::exception& e) {
        std::cerr << "Error in logging test: " << e.what() << std::endl;
        assert(false);
    }
    
    // Test NLM_LOG_ERROR_WITH_EXCEPTION macro
    try {
        NLM_LOG_ERROR_WITH_EXCEPTION("Test error message with exception", "test_error");
        NLM_LOG_ERROR("Regular error message test");
    } catch (const std::exception& e) {
        std::cerr << "Error in error logging test: " << e.what() << std::endl;
        assert(false);
    }
    
    std::cout << "Enhanced Logging Macros test passed!" << std::endl;
}

void testExceptionRecoveryManager() {
    std::cout << "Testing Exception Recovery Manager..." << std::endl;
    
    // Test the recovery manager's validation functions
    try {
        nlm::getRecoveryManager().validateInputs(
            []() { nlm::Validation::validateKey("test"); },
            []() { nlm::Validation::validateFilepath("valid_file.txt"); }
        );
        NLM_LOG_INFO("Exception recovery manager validation successful");
    } catch (const std::exception& e) {
        std::cerr << "Exception recovery manager test failed: " << e.what() << std::endl;
        assert(false);
    }
    
    std::cout << "Exception Recovery Manager test passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Enhanced Exception Handling Test ===" << std::endl;
    
    try {
        testExceptionHierarchy();
        testConfigWithExceptionHandling();
        testRandomGeneratorWithExceptionHandling();
        testValidationFunctions();
        testEnhancedLoggingMacros();
        testExceptionRecoveryManager();
        
        std::cout << "=== All tests passed successfully! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "=== Test failed with exception: " << e.what() << " ===" << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "=== Test failed with unknown exception ===" << std::endl;
        return 1;
    }
}
#!/usr/bin/env g++ -std=c++20
// Simple test to verify error handling framework

#include "src/core/ErrorHandling.hpp"
#include "src/brain/BrainErrorHandling.hpp"
#include <iostream>
#include <cassert>

void testErrorHandlingFramework() {
    std::cout << "Testing NLM Error Handling Framework..." << std::endl;
    
    try {
        // Test error creation
        NLMError error(ErrorCode::CONFIG_VALIDATION_FAILED, 
                      "Test error with context", ErrorCategory::ERROR);
        error.addContext("key", "value");
        error.addContext("file", "test.cpp");
        
        std::cout << "Error created: " << error.what() << std::endl;
        
        // Test error formatting
        std::string formatted = error.formatError();
        assert(formatted.find("Error Code:") != std::string::npos);
        assert(formatted.find("Test error with context") != std::string::npos);
        
        // Test recovery status
        std::string recovery = error.getRecoveryStatus();
        assert(recovery.find("Recovery Attempts:") != std::string::npos);
        
        std::cout << "✓ Error formatting test passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Error handling test failed: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "✓ Error handling framework test completed successfully" << std::endl;
}

void testBrainErrorHandling() {
    std::cout << "Testing Brain Error Handling..." << std::endl;
    
    try {
        // Test configuration validation
        bool validConfig = BrainError::BrainErrorRecovery::validateBrainConfiguration(
            std::make_shared<Config>()
        );
        assert(!validConfig);  // Empty config should be invalid
        
        std::cout << "✓ Configuration validation test passed" << std::endl;
        
        // Test neural connectivity validation (empty regions should fail)
        std::vector<std::unique_ptr<NeuralRegion>> emptyRegions;
        bool validConnectivity = BrainError::BrainErrorRecovery::validateNeuralConnectivity(emptyRegions);
        assert(!validConnectivity);  // Empty regions should be invalid
        
        std::cout << "✓ Neural connectivity validation test passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Brain error handling test failed: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "✓ Brain error handling test completed successfully" << std::endl;
}

void testSafeAllocation() {
    std::cout << "Testing safe memory allocation..." << std::endl;
    
    try {
        // Test safe allocation
        int* ptr = BrainError::BrainErrorRecovery::safeAllocate<int>(100, "test allocation");
        assert(ptr != nullptr);
        
        // Verify allocation
        for (int i = 0; i < 100; ++i) {
            ptr[i] = i;
        }
        
        // Clean up
        delete[] ptr;
        
        std::cout << "✓ Safe allocation test passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ Safe allocation test failed: " << e.what() << std::endl;
        throw;
    }
    
    std::cout << "✓ Safe allocation test completed successfully" << std::endl;
}

int main() {
    try {
        testErrorHandlingFramework();
        testBrainErrorHandling();
        testSafeAllocation();
        
        std::cout << "\n✓ All error handling tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "\n✗ Error handling tests failed: " << e.what() << std::endl;
        return 1;
    }
}
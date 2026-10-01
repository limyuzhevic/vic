#include <iostream>
#include <string>
#include <functional>

// Include test modules
#include "test_brain_advanced.cpp"
#include "test_python_bindings_advanced.cpp"

// Simple test execution function
void runTests(const std::string& testName, std::function<void()> testRunner) {
    std::cout << "\n=== " << testName << " ===" << std::endl;
    try {
        testRunner();
        std::cout << "✓ " << testName << " PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "✗ " << testName << " FAILED: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    std::cout << "NLM Advanced Features Test Suite" << std::endl;
    std::cout << "===============================" << std::endl;
    
    try {
        // Test Brain Advanced API
        runTests("Brain Advanced API Tests", []() {
            test_brain_advanced::runAllBrainAdvancedTests();
        });
        
        // Test Python Bindings Advanced Tests
        runTests("Python Bindings Advanced Tests", []() {
            test_python_bindings_advanced::runAllPythonBindingsTests();
        });
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
        std::cout << "=" << std::string(50, '=') << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "TESTS FAILED: " << e.what() << std::endl;
        std::cout << "=" << std::string(50, '=') << std::endl;
        return 1;
    }
}
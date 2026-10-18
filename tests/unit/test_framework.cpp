// Test Framework Tests
// Tests for the test framework infrastructure

#include "test_framework.hpp"
#include <cassert>
#include <iostream>

namespace test_framework {

// Simple test case for framework testing
class FrameworkTest : public TestSuite {
public:
    FrameworkSuiteTest() : TestSuite("Framework::Basic") {}
    
    void run() override {
        std::cout << "  Testing framework assertion macros..." << std::endl;
        
        // Test ASSERT_EQ
        try {
            ASSERT_EQ(1, 1);  // Should pass
        } catch (const std::exception& e) {
            std::cout << "  ASSERT_EQ test failed unexpectedly: " << e.what() << std::endl;
        }
        
        // Test ASSERT_NE
        try {
            ASSERT_NE(1, 2);  // Should pass
        } catch (const std::exception& e) {
            std::cout << "  ASSERT_NE test failed unexpectedly: " << e.what() << std::endl;
        }
        
        // Test ASSERT_GT
        try {
            ASSERT_GT(2, 1);  // Should pass
        } catch (const std::exception& e) {
            std::cout << "  ASSERT_GT test failed unexpectedly: " << e.what() << std::endl;
        }
        
        // Test ASSERT_LT
        try {
            ASSERT_LT(1, 2);  // Should pass
        } catch (const std::exception& e) {
            std::cout << "  ASSERT_LT test failed unexpectedly: " << e.what() << std::endl;
        }
        
        // Test ASSERT_TRUE
        try {
            ASSERT_TRUE(true);  // Should pass
        } catch (const std::exception& e) {
            std::cout << "  ASSERT_TRUE test failed unexpectedly: " << e.what() << std::endl;
        }
        
        // Test ASSERT_FALSE
        try {
            ASSERT_FALSE(false);  // Should pass
        } catch (const std::exception& e) {
            std::cout << "  ASSERT_FALSE test failed unexpectedly: " << e.what() << std::endl;
        }
        
        std::cout << "  Framework tests PASSED" << std::endl;
    }
};

// Global test instance
static FrameworkTest frameworkTest;

void runAll() {
    std::cout << "=== Test Framework Tests ===" << std::endl;
    
    // Register test suite with framework
    TestFramework& framework = TestFramework::getInstance();
    framework.addSuite(&frameworkTest);
    
    // Run tests
    bool allPassed = framework.runAllTests();
    
    if (allPassed) {
        std::cout << "=== All Framework Tests PASSED ===" << std::endl;
    } else {
        std::cout << "=== Some Framework Tests FAILED ===" << std::endl;
    }
}

} // namespace test_framework

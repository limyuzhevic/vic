// Test Framework Test
// Tests for the test framework infrastructure

#include "test_framework.hpp"
#include <cassert>
#include <iostream>

namespace test_framework {

// Simple test case for framework testing
class FrameworkTestSuite : public TestSuite {
public:
    FrameworkTestSuite() : TestSuite("Framework::Test") {}
    
    void run() override {
        std::cout << "  Testing framework assertion macros..." << std::endl;
        
        // Test ASSERT_EQ
        try {
            ASSERT_EQ(1, 1);  // Should pass
            std::cout << "    ASSERT_EQ passed" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    ASSERT_EQ failed: " << e.what() << std::endl;
        }
        
        // Test ASSERT_NE
        try {
            ASSERT_NE(1, 2);  // Should pass
            std::cout << "    ASSERT_NE passed" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    ASSERT_NE failed: " << e.what() << std::endl;
        }
        
        // Test ASSERT_GT
        try {
            ASSERT_GT(2, 1);  // Should pass
            std::cout << "    ASSERT_GT passed" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    ASSERT_GT failed: " << e.what() << std::endl;
        }
        
        // Test ASSERT_LT
        try {
            ASSERT_LT(1, 2);  // Should pass
            std::cout << "    ASSERT_LT passed" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    ASSERT_LT failed: " << e.what() << std::endl;
        }
        
        // Test ASSERT_TRUE
        try {
            ASSERT_TRUE(true);  // Should pass
            std::cout << "    ASSERT_TRUE passed" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    ASSERT_TRUE failed: " << e.what() << std::endl;
        }
        
        // Test ASSERT_FALSE
        try {
            ASSERT_FALSE(false);  // Should pass
            std::cout << "    ASSERT_FALSE passed" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    ASSERT_FALSE failed: " << e.what() << std::endl;
        }
        
        std::cout << "  Framework tests PASSED" << std::endl;
    }
};

// Global test instance
static FrameworkTestSuite frameworkTest;

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

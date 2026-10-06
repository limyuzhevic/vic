#include "framework/TestFramework.hpp"
#include <iostream>
#include <memory>

// Simple demonstration of the enhanced NLM Test Framework
void demonstrateEnhancedTestFramework() {
    std::cout << "=== NLM Enhanced Test Framework Demonstration ===" << std::endl;
    
    // Initialize the test framework
    nlm::test::initializeTestFramework();
    
    // Configure test framework
    auto& config = nlm::test::getConfiguration();
    config.verboseOutput = true;
    config.showProgress = true;
    config.enableMemoryTracker = true;
    config.enablePerformanceBenchmarking = true;
    
    // Create a simple test case
    class SimpleTest : public nlm::test::TestBase {
    public:
        SimpleTest() {
            setName("SimpleTest");
            setDescription("A simple test demonstrating NLM Test Framework");
            setCategory(nlm::test::TestSeverity::UNIT);
        }
        
        void runTest() override {
            std::cout << "  Running: Checking if 2 + 2 equals 4" << std::endl;
            
            // Use modern assertion macros
            NLM_ASSERT_EQ(2 + 2, 4);
            NLM_ASSERT_TRUE(true);
            NLM_ASSERT_FALSE(false);
            
            std::cout << "  SimpleTest completed successfully!" << std::endl;
        }
    };
    
    // Register the test
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<SimpleTest>()
    );
    
    // Create another test to demonstrate different features
    class EnhancedTest : public nlm::test::TestBase {
    public:
        EnhancedTest() {
            setName("EnhancedTest");
            setDescription("A test demonstrating enhanced features");
            setCategory(nlm::test::TestSeverity::INTEGRATION);
        }
        
        void setup() override {
            std::cout << "  Setting up EnhancedTest" << std::endl;
        }
        
        void teardown() override {
            std::cout << "  Tearing down EnhancedTest" << std::endl;
        }
        
        void runTest() override {
            std::cout << "  Running: Testing string operations" << std::endl;
            
            // Test string operations with enhanced assertions
            std::string str1 = "Hello";
            std::string str2 = "Hello";
            
            NLM_ASSERT_EQ(str1, str2);
            NLM_ASSERT_TRUE(str1.length() == 5);
            NLM_ASSERT_TRUE(str2.length() == 5);
            
            std::cout << "  EnhancedTest completed successfully!" << std::endl;
        }
    };
    
    // Register the enhanced test
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<EnhancedTest>()
    );
    
    // Create a performance test
    class PerformanceTest : public nlm::test::TestBase {
    public:
        PerformanceTest() {
            setName("PerformanceTest");
            setDescription("A performance benchmark");
            setCategory(nlm::test::TestSeverity::PERFORMANCE);
        }
        
        void setup() override {
            std::cout << "  Setting up PerformanceTest" << std::endl;
        }
        
        void teardown() override {
            std::cout << "  Tearing down PerformanceTest" << std::endl;
        }
        
        void runTest() override {
            std::cout << "  Running: Performance benchmark (100 iterations)" << std::endl;
            
            nlm::test::TestTimer timer;
            
            // Perform some operations
            nlm::RandomGenerator rng(42);
            for (int i = 0; i < 100; ++i) {
                uint32_t val = rng.uniformInt(0, 1000);
                double d = rng.uniformReal(0.0, 1.0);
                bool b = rng.bernoulli(0.5);
            }
            
            double elapsed = timer.elapsed();
            std::cout << "  Performance test completed in " << elapsed << " seconds" << std::endl;
            
            // Check performance (should be reasonable)
            NLM_ASSERT_TRUE(elapsed < 1.0);  // Should complete in less than 1 second
            
            std::cout << "  PerformanceTest completed successfully!" << std::endl;
        }
    };
    
    // Register the performance test
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<PerformanceTest>()
    );
    
    std::cout << "\nRunning enhanced tests..." << std::endl;
    
    // Run all tests
    nlm::test::TestRegistry::getInstance().runAllTests();
    
    // Get test statistics
    nlm::test::TestStats stats = nlm::test::TestRegistry::getInstance().getStats();
    
    // Print enhanced report
    std::cout << "\n=== Enhanced Test Results ===" << std::endl;
    std::cout << "Total Tests: " << stats.total << std::endl;
    std::cout << "Passed: " << stats.passed << std::endl;
    std::cout << "Failed: " << stats.failed << std::endl;
    std::cout << "Skipped: " << stats.skipped << std::endl;
    std::cout << "Errors: " << stats.errors << std::endl;
    std::cout << "Total Time: " << stats.totalTime << " seconds" << std::endl;
    
    std::cout << "\n=== Demonstration Complete ===" << std::endl;
    
    // Cleanup
    nlm::test::cleanupTestFramework();
}

int main() {
    // Demonstrate the enhanced NLM Test Framework
    demonstrateEnhancedTestFramework();
    
    return 0;
}

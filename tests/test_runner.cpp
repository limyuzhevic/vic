// Main test runner for NLM testing framework

// Main test runner for NLM testing framework

#include "tests/test_framework.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// Forward declarations for test modules
namespace test_neuron {
    void runAll();
}

namespace test_brain {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_main {
    void runAll();
}

namespace test_types {
    void runAll();
}

// Test suite manager
class TestSuite {
private:
    std::vector<std::function<void()>> testSuites;
    std::vector<std::string> suiteNames;
    
public:
    // Add a test suite
    void addTestSuite(const std::string& name, std::function<void()> testFunc) {
        testSuites.push_back(testFunc);
        suiteNames.push_back(name);
    }
    
    // Run all test suites
    void runAllTestSuites() {
        std::cout << "=== Running Test Suites ===" << std::endl;
        
        for (size_t i = 0; i < testSuites.size(); ++i) {
            std::cout << std::endl;
            std::cout << "--- Running " << suiteNames[i] << " ---" << std::endl;
            try {
                testSuites[i]();
                std::cout << "✓ " << suiteNames[i] << " completed" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "✗ " << suiteNames[i] << " failed with exception: " << e.what() << std::endl;
            } catch (...) {
                std::cout << "✗ " << suiteNames[i] << " failed with unknown exception" << std::endl;
            }
        }
        
        std::cout << std::endl << "=== All test suites completed ===" << std::endl;
    }
    
    // Get number of test suites
    size_t getSuiteCount() const { return testSuites.size(); }
};

// Custom test runner with enhanced reporting
class EnhancedTestRunner {
private:
    TestFramework& framework;
    bool verbose;
    
public:
    EnhancedTestRunner(TestFramework& testFramework, bool verboseOutput = false)
        : framework(testFramework), verbose(verboseOutput) {}
    
    // Run specific test suite
    void runTestSuite(const std::string& suiteName, std::function<void()> testSuiteFunc) {
        std::cout << std::endl << "=== " << suiteName << " ===" << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        try {
            testSuiteFunc();
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(end - start).count();
            
            std::cout << "Suite completed in " << std::fixed << std::setprecision(2)
                     << duration << " ms" << std::endl;
            
            if (verbose) {
                std::cout << std::endl << "Detailed results:" << std::endl;
                const auto& results = framework.getResults();
                for (const auto& result : results) {
                    std::cout << "  " << result.category << "." << result.name << ": "
                             << (result.passed ? "PASS" : "FAIL") << 
                             " (" << std::fixed << std::setprecision(2) << result.durationMs
                             << " ms)";
                    if (!result.message.empty()) {
                        std::cout << " - " << result.message;
                    }
                    std::cout << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Suite failed with exception: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Suite failed with unknown exception" << std::endl;
        }
    }
};

// Main function to run all tests
int main(int argc, char* argv[]) {
    std::cout << "=== NLM Test Suite ===" << std::endl;
    std::cout << "Starting comprehensive test suite with enhanced reporting..." << std::endl;
    
    // Determine verbosity from command line arguments
    bool verbose = false;
    bool help = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-h" || arg == "--help") {
            help = true;
        } else if (arg == "--quiet") {
            verbose = false;
        }
    }
    
    if (help) {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -v, --verbose    Enable verbose output" << std::endl;
        std::cout << "  -h, --help       Show this help message" << std::endl;
        std::cout << "  --quiet          Disable verbose output" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << argv[0] << "           Run all tests with default settings" << std::endl;
        std::cout << "  " << argv[0] << " --verbose   Run all tests with verbose output" << std::endl;
        return 0;
    }
    
    // Initialize test framework
    TestFramework testFramework(verbose);
    EnhancedTestRunner testRunner(testFramework, verbose);
    
    // Build test suite
    TestSuite testSuite;
    
    // Add all test suites
    testSuite.addTestSuite("Neuron Tests", []() { test_neuron::runAll(); });
    testSuite.addTestSuite("Brain Tests", []() { test_brain::runAll(); });
    testSuite.addTestSuite("Synapse Tests", []() { test_synapse::runAll(); });
    testSuite.addTestSuite("Config Tests", []() { test_config::runAll(); });
    testSuite.addTestSuite("Clock Tests", []() { test_clock::runAll(); });
    testSuite.addTestSuite("STDP Tests", []() { test_stdp::runAll(); });
    testSuite.addTestSuite("Random Tests", []() { test_random::runAll(); });
    testSuite.addTestSuite("Main Tests", []() { test_main::runAll(); });
    testSuite.addTestSuite("Types Tests", []() { test_types::runAll(); });
    
    // Run all test suites
    testSuite.runAllTestSuites();
    
    // Get final results from framework
    testFramework.printResults();
    
    // Final summary
    std::cout << std::endl << "=== Final Test Summary ===" << std::endl;
    std::cout << "Total test suites: " << testSuite.getSuiteCount() << std::endl;
    std::cout << "Total tests: " << testFramework.getTotalTests() << std::endl;
    std::cout << "Passed: " << testFramework.getPassedTests() << std::endl;
    std::cout << "Failed: " << testFramework.getFailedTests() << std::endl;
    
    if (testFramework.allPassed()) {
        std::cout << std::endl << "✓ All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << std::endl << "✗ Some tests failed!" << std::endl;
        return 1;
    }
}
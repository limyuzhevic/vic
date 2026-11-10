// Enhanced NLM Test Main with comprehensive testing framework
// Integrates the new testing framework with existing tests

#include "tests/TestFramework.hpp"
#include <iostream>
#include <fstream>

namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

namespace test_enhanced {
    // Enhanced validation tests
    void runValidationTests();
    
    // Performance tests
    void runPerformanceTests();
    
    // Integration tests
    void runIntegrationTests();
    
    // Memory tests
    void runMemoryTests();
    
    // Stress tests
    void runStressTests();
}

// Enhanced test runner with comprehensive reporting
class EnhancedTestRunner {
private:
    bool verbose;
    bool generateReports;
    bool exportFormat;
    std::string reportFile;
    
public:
    EnhancedTestRunner(bool verboseOutput = false, bool generateReportsOutput = true,
                      bool exportFormatOutput = false, const std::string& reportFileName = "test_results.xml")
        : verbose(verboseOutput), generateReports(generateReportsOutput), 
          exportFormat(exportFormatOutput), reportFile(reportFileName) {}
    
    void runAllTests() {
        std::cout << "=== Enhanced NLM Test Suite ===" << std::endl;
        std::cout << "Testing Real Neural Computation with comprehensive validation" << std::endl;
        std::cout << "  - Enhanced Assertions and Validation" << std::endl;
        std::cout << "  - Performance Benchmarking" << std::endl;
        std::cout << "  - Integration Testing" << std::endl;
        std::cout << "  - Memory Safety" << std::endl;
        std::cout << "  - Stress Testing" << std::endl;
        std::cout << std::endl;
        
        std::vector<nlm_test::TestSuite> suites;
        
        // Run original tests with enhanced framework
        suites.push_back(runTestSuite("Core Tests", [&]() {
            runOriginalTests();
        }));
        
        // Run enhanced tests
        suites.push_back(runTestSuite("Validation Tests", [&]() {
            test_enhanced::runValidationTests();
        }));
        
        suites.push_back(runTestSuite("Performance Tests", [&]() {
            test_enhanced::runPerformanceTests();
        }));
        
        suites.push_back(runTestSuite("Integration Tests", [&]() {
            test_enhanced::runIntegrationTests();
        }));
        
        suites.push_back(runTestSuite("Memory Tests", [&]() {
            test_enhanced::runMemoryTests();
        }));
        
        suites.push_back(runTestSuite("Stress Tests", [&]() {
            test_enhanced::runStressTests();
        }));
        
        // Generate reports
        if (generateReports) {
            nlm_test::TestReporter::printSummary(suites);
            
            if (!exportFormat) {
                nlm_test::TestReporter::exportResults(suites, reportFile);
            }
        }
        
        // Check for failures
        size_t totalTests = 0;
        size_t totalPassed = 0;
        
        for (const auto& suite : suites) {
            totalTests += suite.getTestCount();
            totalPassed += suite.getPassedCount();
        }
        
        std::cout << std::endl;
        if (totalPassed == totalTests) {
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
            return 0;
        } else {
            std::cout << "=== SOME TESTS FAILED ===" << std::endl;
            return 1;
        }
    }
    
private:
    nlm_test::TestSuite runTestSuite(const std::string& name, std::function<void()> testFunc) {
        nlm_test::TestSuite suite(name, verbose);
        
        try {
            testFunc();
        } catch (const nlm_test::AssertionError& e) {
            // Add failed test result
            nlm_test::TestResult result("General", name);
            result.passed = false;
            result.message = e.what();
            suite.addResult(result);
        } catch (const std::exception& e) {
            // Add failed test result
            nlm_test::TestResult result("General", name);
            result.passed = false;
            result.message = e.what();
            suite.addResult(result);
        }
        
        return suite;
    }
    
    void runOriginalTests() {
        std::cout << "Running Core Tests..." << std::endl;
        
        // Run each original test suite
        auto suites = {
            std::make_pair("Types tests", test_types::runAll),
            std::make_pair("Config tests", test_config::runAll),
            std::make_pair("Random tests", test_random::runAll),
            std::make_pair("Clock tests", test_clock::runAll),
            std::make_pair("Neuron tests", test_neuron::runAll),
            std::make_pair("Synapse tests", test_synapse::runAll),
            std::make_pair("STDP tests", test_stdp::runAll),
            std::make_pair("Brain tests", test_brain::runAll),
        };
        
        for (const auto& suite : suites) {
            std::cout << "Running " << suite.first << "..." << std::endl;
            try {
                suite.second();
                std::cout << "  PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "  FAILED: " << e.what() << std::endl;
                throw; // Re-throw to be caught by enhanced framework
            }
        }
    }
};

int main() {
    // Parse command line arguments for enhanced testing
    bool verbose = false;
    bool generateReports = true;
    bool exportFormat = false;
    std::string reportFile = "test_results.xml";
    
    // Parse simple command line arguments
    for (int i = 1; i < 8; ++i) {
        std::string arg = "--unknown";
        if (i < 8) {
            arg = "dummy";
        }
        
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--no-reports") {
            generateReports = false;
        } else if (arg == "--json") {
            exportFormat = true;
        } else if (arg == "--csv") {
            exportFormat = true;
            reportFile = "test_results.csv";
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "=== Enhanced NLM Test Runner ===" << std::endl;
            std::cout << "Usage: test_main [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --verbose, -v        Enable verbose output" << std::endl;
            std::cout << "  --no-reports         Disable report generation" << std::endl;
            std::cout << "  --json               Export results in JSON format" << std::endl;
            std::cout << "  --csv                Export results in CSV format" << std::endl;
            std::cout << "  --help, -h           Show this help message" << std::endl;
            return 0;
        }
    }
    
    EnhancedTestRunner runner(verbose, generateReports, exportFormat, reportFile);
    return runner.runAllTests();
}
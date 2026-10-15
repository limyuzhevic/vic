// NLM Test Main
// Phase 6: Modular Brain Tests
// Comprehensive test framework for modular architecture

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

// Test framework classes and utilities
namespace test_utils {
    
    class TestResult {
    public:
        std::string name;
        bool passed;
        std::string message;
        
        TestResult(const std::string& testName, bool testPassed, const std::string& testMessage)
            : name(testName), passed(testPassed), message(testMessage) {}
    };
    
    class TestSuite {
    public:
        std::string name;
        std::vector<TestResult> results;
        
        TestSuite(const std::string& suiteName) : name(suiteName) {}
        
        void addResult(const TestResult& result) {
            results.push_back(result);
        }
        
        bool allPassed() const {
            return std::all_of(results.begin(), results.end(),
                              [](const TestResult& r) { return r.passed; });
        }
        
        size_t passedCount() const {
            return std::count_if(results.begin(), results.end(),
                                [](const TestResult& r) { return r.passed; });
        }
        
        size_t totalCount() const {
            return results.size();
        }
    };
    
    void printTestSummary(const TestSuite& suite) {
        std::cout << "Test Suite: " << suite.name << std::endl;
        std::cout << "  Passed: " << suite.passedCount() << "/" << suite.totalCount() << std::endl;
        
        if (!suite.allPassed()) {
            std::cout << "  FAILED Tests:" << std::endl;
            for (const auto& result : suite.results) {
                if (!result.passed) {
                    std::cout << "    - " << result.name << ": " << result.message << std::endl;
                }
            }
        }
    }
}

// Forward declarations of modular component test functions
namespace brain_core_tests {
    void runAll();
    void runBasicNeuralComputationTest();
    void runSynapticConnectivityTest();
    void runPlasticityTest();
}

namespace brain_integration_tests {
    void runAll();
    void runMemoryIntegrationTest();
    void runPredictionIntegrationTest();
    void runCognitionIntegrationTest();
    void runNeuromodulationIntegrationTest();
}

namespace brain_io_tests {
    void runAll();
    void runSensoryInputTest();
    void runMotorOutputTest();
    void runEnvironmentInteractionTest();
}

namespace brain_state_tests {
    void runAll();
    void runStatePersistenceTest();
    void runDevelopmentalStageTest();
    void runCheckpointSystemTest();
}

namespace performance_tests {
    void runAll();
    void runBenchmarkTests();
    void runMemoryUsageTest();
    void runPerformanceProfilingTest();
}

// Test selection function
void runSelectedTests(const std::vector<std::string>& testNames) {
    bool runAllTests = (testNames.empty() || 
                       std::find(testNames.begin(), testNames.end(), "all") != testNames.end());
    
    if (runAllTests) {
        std::cout << "Running all tests..." << std::endl;
        
        // Create test suites
        test_utils::TestSuite coreSuite("BrainCore Tests");
        test_utils::TestSuite integrationSuite("BrainIntegration Tests");
        test_utils::TestSuite ioSuite("BrainIO Tests");
        test_utils::TestSuite stateSuite("BrainState Tests");
        test_utils::TestSuite performanceSuite("Performance Tests");
        
        // Run BrainCore tests
        try {
            brain_core_tests::runAll();
            std::cout << "  PASSED" << std::endl;
            coreSuite.addResult(test_utils::TestResult("Core Tests", true, "All BrainCore tests passed"));
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            coreSuite.addResult(test_utils::TestResult("Core Tests", false, e.what()));
        }
        
        // Run BrainIntegration tests
        try {
            brain_integration_tests::runAll();
            std::cout << "  PASSED" << std::endl;
            integrationSuite.addResult(test_utils::TestResult("Integration Tests", true, "All BrainIntegration tests passed"));
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            integrationSuite.addResult(test_utils::TestResult("Integration Tests", false, e.what()));
        }
        
        // Run BrainIO tests
        try {
            brain_io_tests::runAll();
            std::cout << "  PASSED" << std::endl;
            ioSuite.addResult(test_utils::TestResult("IO Tests", true, "All BrainIO tests passed"));
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            ioSuite.addResult(test_utils::TestResult("IO Tests", false, e.what()));
        }
        
        // Run BrainState tests
        try {
            brain_state_tests::runAll();
            std::cout << "  PASSED" << std::endl;
            stateSuite.addResult(test_utils::TestResult("State Tests", true, "All BrainState tests passed"));
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            stateSuite.addResult(test_utils::TestResult("State Tests", false, e.what()));
        }
        
        // Run Performance tests
        try {
            performance_tests::runAll();
            std::cout << "  PASSED" << std::endl;
            performanceSuite.addResult(test_utils::TestResult("Performance Tests", true, "All Performance tests passed"));
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << e.what() << std::endl;
            performanceSuite.addResult(test_utils::TestResult("Performance Tests", false, e.what()));
        }
        
        // Print summary
        std::cout << std::endl;
        test_utils::printTestSummary(coreSuite);
        test_utils::printTestSummary(integrationSuite);
        test_utils::printTestSummary(ioSuite);
        test_utils::printTestSummary(stateSuite);
        test_utils::printTestSummary(performanceSuite);
        
        // Overall result
        bool allPassed = coreSuite.allPassed() && integrationSuite.allPassed() && 
                        ioSuite.allPassed() && stateSuite.allPassed() && 
                        performanceSuite.allPassed();
        
        if (allPassed) {
            std::cout << std::endl << "=== All Tests PASSED ===" << std::endl;
            return 0;
        } else {
            std::cout << std::endl << "=== Some Tests FAILED ===" << std::endl;
            return 1;
        }
    }
    
    // Run selected individual tests
    std::cout << "Running selected tests: " << std::endl;
    for (const std::string& testName : testNames) {
        std::cout << "  - " << testName << std::endl;
        
        if (testName == "brain_core_all" || testName == "all") {
            try {
                brain_core_tests::runAll();
                std::cout << "    PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "    FAILED: " << e.what() << std::endl;
                return 1;
            }
        } else if (testName == "brain_integration_all" || testName == "all") {
            try {
                brain_integration_tests::runAll();
                std::cout << "    PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "    FAILED: " << e.what() << std::endl;
                return 1;
            }
        } else if (testName == "brain_io_all" || testName == "all") {
            try {
                brain_io_tests::runAll();
                std::cout << "    PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "    FAILED: " << e.what() << std::endl;
                return 1;
            }
        } else if (testName == "brain_state_all" || testName == "all") {
            try {
                brain_state_tests::runAll();
                std::cout << "    PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "    FAILED: " << e.what() << std::endl;
                return 1;
            }
        } else if (testName == "performance_all" || testName == "all") {
            try {
                performance_tests::runAll();
                std::cout << "    PASSED" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "    FAILED: " << e.what() << std::endl;
                return 1;
            }
        } else {
            std::cout << "    UNKNOWN TEST: " << testName << std::endl;
            return 1;
        }
    }
    
    std::cout << std::endl << "=== All Selected Tests PASSED ===" << std::endl;
    return 0;
}

int main(int argc, char** argv) {
    std::cout << "=== NLM Phase 6: Modular Brain Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation with Modular Architecture:" << std::endl;
    std::cout << "  - BrainCore: Core neural computation (neurons, synapses, plasticity)" << std::endl;
    std::cout << "  - BrainIntegration: Integrated memory, prediction, cognition, neuromodulation" << std::endl;
    std::cout << "  - BrainIO: Input/output and environment interaction" << std::endl;
    std::cout << "  - BrainState: State management and persistence" << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments for test selection
    std::vector<std::string> testNames;
    
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Support for --help flag
            if (arg == "--help" || arg == "-h") {
                std::cout << "Available test commands:" << std::endl;
                std::cout << "  --help, -h              Show this help message" << std::endl;
                std::cout << "  --test <name>           Run specific test (e.g., --test brain_core_all)" << std::endl;
                std::cout << "  --test <name1> <name2>   Run multiple specific tests" << std::endl;
                std::cout << "  (No arguments)            Run all tests" << std::endl;
                std::cout << std::endl;
                std::cout << "Available test names:" << std::endl;
                std::cout << "  brain_core_all           Run all BrainCore tests" << std::endl;
                std::cout << "  brain_integration_all    Run all BrainIntegration tests" << std::endl;
                std::cout << "  brain_io_all             Run all BrainIO tests" << std::endl;
                std::cout << "  brain_state_all          Run all BrainState tests" << std::endl;
                std::cout << "  performance_all          Run all Performance tests" << std::endl;
                std::cout << "  all                      Run all tests" << std::endl;
                std::cout << std::endl;
                std::cout << "Example usage:" << std::endl;
                std::cout << "  ./nlm_test --test brain_core_all" << std::endl;
                std::cout << "  ./nlm_test --test brain_core_all brain_io_all" << std::endl;
                std::cout << "  ./nlm_test --test all" << std::endl;
                return 0;
            }
            
            // Support for --test flag
            if (arg.substr(0, 7) == "--test=") {
                testNames.push_back(arg.substr(7));
            } else if (arg.substr(0, 7) == "--test") {
                if (i + 1 < argc) {
                    testNames.push_back(argv[++i]);
                }
            }
        }
    }
    
    // Run tests
    return runSelectedTests(testNames);
}
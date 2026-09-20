// Comprehensive Integration Test Runner
// Main integration test file that runs all integration tests
// Uses the IntegrationTestFramework to test multiple systems simultaneously

#include "tests/integration/IntegrationTestFramework.hpp"
#include "tests/integration/TestSystemIntegration.hpp"
#include "tests/integration/TestMemoryIntegration.hpp"
#include "tests/integration/TestCognitionIntegration.hpp"
#include "tests/integration/TestNeuromodulationIntegration.hpp"
#include "tests/integration/TestDevelopmentIntegration.hpp"
#include "tests/integration/TestCrossSystemIntegration.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace nlm {

class IntegrationTestRunner {
public:
    IntegrationTestRunner() {
        config = std::make_shared<Config>();
        
        // Configure for integration testing
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("max_synaptic_weight", 1.0, ConfigSource::Default);
        config->set("min_synaptic_weight", -1.0, ConfigSource::Default);
    }
    
    void runAllIntegrationTests() {
        std::cout << "=== NLM Comprehensive Integration Tests ===" << std::endl;
        std::cout << "Testing all major neural system integrations:" << std::endl;
        std::cout << std::endl;
        
        // Create result collection
        IntegrationTestResult results;
        
        // Run each integration test suite
        runSystemIntegrationTests(config, results);
        runMemoryIntegrationTests(config, results);
        runCognitionIntegrationTests(config, results);
        runNeuromodulationIntegrationTests(config, results);
        runDevelopmentIntegrationTests(config, results);
        runCrossSystemIntegrationTests(config, results);
        
        // Print summary
        results.printResults();
        
        // Save detailed results
        std::string outputFile = "/tmp/integration_test_results_" + 
                                 IntegrationTestResult::getTimestamp() + ".txt";
        results.saveResults(outputFile);
        std::cout << std::endl;
        std::cout << "Detailed results saved to: " << outputFile << std::endl;
        
        // Return summary
        int totalTests = results.getTestCount();
        int passedTests = results.getPassedCount();
        int failedTests = results.getFailedCount();
        
        std::cout << std::endl;
        std::cout << "=== Integration Test Summary ===" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        
        if (failedTests == 0) {
            std::cout << "=== ALL INTEGRATION TESTS PASSED ===" << std::endl;
            return 0;
        } else {
            std::cout << "=== SOME INTEGRATION TESTS FAILED ===" << std::endl;
            return 1;
        }
    }
    
private:
    std::shared_ptr<Config> config;
    
    void runSystemIntegrationTests(std::shared_ptr<Config> cfg, IntegrationTestResult& results) {
        std::cout << "Running System Integration Tests..." << std::endl;
        TestSystemIntegration tester(cfg, results);
        tester.runAllTests();
        std::cout << "  System Integration Tests Complete" << std::endl;
    }
    
    void runMemoryIntegrationTests(std::shared_ptr<Config> cfg, IntegrationTestResult& results) {
        std::cout << "Running Memory Integration Tests..." << std::endl;
        TestMemoryIntegration tester(cfg, results);
        tester.runAllTests();
        std::cout << "  Memory Integration Tests Complete" << std::endl;
    }
    
    void runCognitionIntegrationTests(std::shared_ptr<Config> cfg, IntegrationTestResult& results) {
        std::cout << "Running Cognition Integration Tests..." << std::endl;
        TestCognitionIntegration tester(cfg, results);
        tester.runAllTests();
        std::cout << "  Cognition Integration Tests Complete" << std::endl;
    }
    
    void runNeuromodulationIntegrationTests(std::shared_ptr<Config> cfg, IntegrationTestResult& results) {
        std::cout << "Running Neuromodulation Integration Tests..." << std::endl;
        TestNeuromodulationIntegration tester(cfg, results);
        tester.runAllTests();
        std::cout << "  Neuromodulation Integration Tests Complete" << std::endl;
    }
    
    void runDevelopmentIntegrationTests(std::shared_ptr<Config> cfg, IntegrationTestResult& results) {
        std::cout << "Running Development Integration Tests..." << std::endl;
        TestDevelopmentIntegration tester(cfg, results);
        tester.runAllTests();
        std::cout << "  Development Integration Tests Complete" << std::endl;
    }
    
    void runCrossSystemIntegrationTests(std::shared_ptr<Config> cfg, IntegrationTestResult& results) {
        std::cout << "Running Cross-System Integration Tests..." << std::endl;
        TestCrossSystemIntegration tester(cfg, results);
        tester.runAllTests();
        std::cout << "  Cross-System Integration Tests Complete" << std::endl;
    }
};

} // namespace nlm

int main() {
    try {
        nlm::IntegrationTestRunner runner;
        return runner.runAllIntegrationTests();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Integration test runner failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown error in integration test runner" << std::endl;
        return 1;
    }
}
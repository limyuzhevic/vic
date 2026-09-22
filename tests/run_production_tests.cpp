// Test Runner Integration
// Entry point for running NLM production unit tests

#include "NLMTestFramework.hpp"
#include <memory>

// Forward declarations for test categories
namespace test_config_production {
    void runAllProductionTests();
}

namespace test_neuron_production {
    void runAllProductionTests();
}

namespace test_synapse_production {
    void runAllProductionTests();
}

namespace test_brain_production {
    void runAllProductionTests();
}

// Global test runner instance
static std::unique_ptr<nlm::test::TestRunner> testRunner;

// Initialize all production test suites
void initializeProductionTests() {
    testRunner = std::make_unique<nlm::test::TestRunner>();
    
    // Initialize all production test categories
    test_config_production::runAllProductionTests();
    test_neuron_production::runAllProductionTests();
    test_synapse_production::runAllProductionTests();
    test_brain_production::runAllProductionTests();
    
    // TODO: Add other production test modules when implemented
}

// Main test runner function
int runProductionTests() {
    std::cout << "========================================" << std::endl;
    std::cout << "NLM Production Unit Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (!testRunner) {
        initializeProductionTests();
    }
    
    testRunner->runAllTests();
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Test Suite Summary:" << std::endl;
    std::cout << "  Total Tests: " << testRunner->getTotalCount() << std::endl;
    std::cout << "  Passed:      " << testRunner->getPassedCount() << std::endl;
    std::cout << "  Failed:      " << testRunner->getFailedCount() << std::endl;
    double successRate = testRunner->getTotalCount() > 0 ? 
        (testRunner->getPassedCount() * 100.0 / testRunner->getTotalCount()) : 0.0;
    std::cout << "  Success Rate: " << std::fixed << std::setprecision(1) << successRate << "%" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Generate detailed report
    std::cout << "Detailed Report:" << std::endl;
    std::cout << testRunner->generateReport() << std::endl;
    
    // Save JSON report for automated testing
    std::string jsonReport = testRunner->generateJSONReport();
    std::ofstream jsonFile("test_results.json");
    if (jsonFile.is_open()) {
        jsonFile << jsonReport;
        jsonFile.close();
        std::cout << "JSON report saved to: test_results.json" << std::endl;
    }
    
    // Return non-zero exit code if any tests failed
    return testRunner->getFailedCount() > 0 ? 1 : 0;
}

// Category-specific test runner
int runCategoryTests(const std::string& category) {
    std::cout << "========================================" << std::endl;
    std::cout << "NLM " << category << " Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    if (!testRunner) {
        initializeProductionTests();
    }
    
    testRunner->runCategory(category);
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Test Category Summary:" << std::endl;
    std::cout << "  Category: " << category << std::endl;
    std::cout << "  Passed:   " << testRunner->getPassedCount() << std::endl;
    std::cout << "  Failed:   " << testRunner->getFailedCount() << std::endl;
    std::cout << "========================================" << std::endl;
    
    return testRunner->getFailedCount() > 0 ? 1 : 0;
}

// Entry point for production testing
int main(int argc, char** argv) {
    std::string categoryFilter;
    
    // Parse command line arguments
    if (argc >= 2) {
        std::string arg(argv[1]);
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h        Show this help message" << std::endl;
            std::cout << "  --category NAME   Run tests for specific category only" << std::endl;
            std::cout << "  --json-only       Output only JSON report" << std::endl;
            std::cout << std::endl;
            std::cout << "Available categories:" << std::endl;
            // TODO: List available categories dynamically
            std::cout << "  - production" << std::endl;
            std::cout << "  (more categories to be implemented)" << std::endl;
            return 0;
        } else if (arg == "--category" && argc >= 3) {
            categoryFilter = argv[2];
        }
    }
    
    int exitCode;
    if (!categoryFilter.empty()) {
        exitCode = runCategoryTests(categoryFilter);
    } else {
        exitCode = runProductionTests();
    }
    
    return exitCode;
}

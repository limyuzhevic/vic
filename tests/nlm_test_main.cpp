// Enhanced NLM Test Main
// Uses modern testing framework for comprehensive test coverage

#include "nlm_testing_framework.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

// Test registration for different components
namespace test_registration {
    void registerCoreTests() {
        std::cout << "Registering Core component tests..." << std::endl;
        // Core components will be registered automatically via TEST_CASE macro
    }
    
    void registerBrainTests() {
        std::cout << "Registering Brain component tests..." << std::endl;
        // Brain components will be registered automatically via TEST_CASE macro
    }
    
    void registerIntegrationTests() {
        std::cout << "Registering Integration tests..." << std::endl;
        // Integration tests will be registered automatically
    }
}

// Enhanced test main with category filtering
int main(int argc, char* argv[]) {
    TestRegistry& registry = TestRegistry::getInstance();
    
    std::cout << "=== Enhanced NLM Test Suite ===" << std::endl;
    std::cout << "Phase 3: Modern Neural Computation Testing" << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments
    TestCategory filterCategory = TestCategory::UNIT;
    std::string filterArg;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--category") {
            if (i + 1 < argc) {
                filterArg = argv[++i];
                if (filterArg == "unit") filterCategory = TestCategory::UNIT;
                else if (filterArg == "integration") filterCategory = TestCategory::INTEGRATION;
                else if (filterArg == "performance") filterCategory = TestCategory::PERFORMANCE;
                else if (filterArg == "configuration") filterCategory = TestCategory::CONFIGURATION;
                else if (filterArg == "error") filterCategory = TestCategory::ERROR_HANDLING;
                else if (filterArg == "cli") filterCategory = TestCategory::CLI;
                else if (filterArg == "system") filterCategory = TestCategory::SYSTEM_INTERACTION;
            }
        } else if (arg == "--help") {
            std::cout << "Usage: nlm_test [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --category TYPE    Filter by test category (unit, integration, performance, configuration, error, cli, system)" << std::endl;
            std::cout << "  --help             Show this help message" << std::endl;
            std::cout << std::endl;
            std::cout << "Categories:" << std::endl;
            std::cout << "  unit: Core component tests (brain, plasticity, memory, etc.)" << std::endl;
            std::cout << "  integration: System interaction and cross-component tests" << std::endl;
            std::cout << "  performance: Performance and benchmarking tests" << std::endl;
            std::cout << "  configuration: Configuration loading and validation tests" << std::endl;
            std::cout << "  error: Error handling and exception safety tests" << std::endl;
            std::cout << "  cli: Command-line interface tests" << std::endl;
            std::cout << "  system: System interaction and integration tests" << std::endl;
            return 0;
        }
    }
    
    // Run tests based on category filter
    if (filterArg.empty()) {
        std::cout << "Running all tests..." << std::endl;
        registry.runAll();
    } else {
        std::cout << "Running tests for category: " << filterArg << std::endl;
        registry.runCategory(filterCategory);
    }
    
    // Print test summary
    std::cout << "\n=== Test Run Complete ===" << std::endl;
    return 0;
}
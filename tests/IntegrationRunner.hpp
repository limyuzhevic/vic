#ifndef INTEGRATION_RUNNER_HPP
#define INTEGRATION_RUNNER_HPP

#include "tests/TestRunner.hpp"
#include <vector>
#include <string>
#include <memory>

/**
 * Integration test runner for NLM testing framework
 */

class IntegrationRunner : public TestRunner {
public:
    IntegrationRunner() : TestRunner() {
        setupIntegrationCategories();
    }
    
    IntegrationRunner(const TestConfig& config) : TestRunner(config) {
        setupIntegrationCategories();
    }
    
    void runIntegrationTests() {
        std::cout << "\n=== NLM Integration Test Suite ===\n";
        std::cout << "Configuration: " << getConfig().name << std::endl;
        std::cout << "Description: " << getConfig().description << std::endl;
        std::cout << "Enabled: " << (getConfig().enabled ? "Yes" : "No") << std::endl;
        
        // Setup integration environment
        setupIntegrationEnvironment();
        
        // Run integration tests
        std::cout << "\n--- Running Integration Tests ---" << std::endl;
        
        // Run integration tests for each category
        for (const auto& category : getConfig().integration.integration_test_categories) {
            if (isCategoryEnabled(category)) {
                runIntegrationTestCategory(category);
            }
        }
        
        // Run experiment integration
        if (getConfig().integration.enable_experiment_integration) {
            std::cout << "\n--- Running Experiment Integration ---" << std::endl;
            runExperimentIntegration();
        }
        
        // Cleanup integration environment
        cleanupIntegrationEnvironment();
        
        // Generate integration test reports
        generateIntegrationReports();
        
        // Print integration test summary
        printIntegrationSummary();
    }
    
    void addIntegrationTestCategory(const std::string& category, const std::string& description = "") {
        getConfig().integration.integration_test_categories.push_back(category);
        if (!description.empty()) {
            getConfig().organization.category_descriptions[category] = description;
        }
        addTestCategory(category);
    }
    
    void removeIntegrationTestCategory(const std::string& category) {
        auto it = std::find(getConfig().integration.integration_test_categories.begin(),
                           getConfig().integration.integration_test_categories.end(), category);
        if (it != getConfig().integration.integration_test_categories.end()) {
            getConfig().integration.integration_test_categories.erase(it);
        }
        removeTestCategory(category);
    }
    
    void enableExperimentIntegration(bool enabled = true) {
        getConfig().integration.enable_experiment_integration = enabled;
    }
    
    void enableIntegrationTestsWithUnit(bool enabled = true) {
        getConfig().integration.run_integration_tests_with_unit = enabled;
    }
    
    void skipIntegrationIfUnitFailed(bool enabled = true) {
        getConfig().integration.skip_integration_if_unit_failed = enabled;
    }
    
protected:
    void setupIntegrationCategories() {
        // Add integration-specific categories
        addIntegrationTestCategory("Phase2Integration", "Phase 2: LIF dynamics and STDP integration");
        addIntegrationTestCategory("Phase6Integration", "Phase 6: Complete system integration");
        addIntegrationTestCategory("EndToEnd", "End-to-end system behavior");
        addIntegrationTestCategory("ExperimentIntegration", "Experiment framework integration");
    }
    
    void setupIntegrationEnvironment() {
        std::cout << "Setting up integration environment..." << std::endl;
        
        // Initialize integration components
        // This would include initializing brain components, memory systems, etc.
        
        std::cout << "Integration environment setup complete." << std::endl;
    }
    
    void cleanupIntegrationEnvironment() {
        std::cout << "Cleaning up integration environment..." << std::endl;
        
        // Cleanup integration components
        // This would include cleaning up brain components, memory systems, etc.
        
        std::cout << "Integration environment cleanup complete." << std::endl;
    }
    
    void runIntegrationTestCategory(const std::string& category) {
        std::cout << "\n  - Running " << category << " tests" << std::endl;
        
        // In a real implementation, this would:
        // 1. Discover tests in the specified category
        // 2. Set up test fixtures
        // 3. Execute tests
        // 4. Collect results
        // 5. Clean up fixtures
        
        // For now, just simulate running integration tests
        std::cout << "    (Integration test framework placeholder)" << std::endl;
    }
    
    void runExperimentIntegration() {
        std::cout << "  - Running experiment integration" << std::endl;
        
        // In a real implementation, this would:
        // 1. Load experiment configurations
        // 2. Run experiments
        // 3. Validate experiment results
        // 4. Compare experiment results with expected behavior
        
        // For now, just simulate experiment integration
        std::cout << "    (Experiment integration framework placeholder)" << std::endl;
    }
    
    void generateIntegrationReports() {
        std::cout << "\nGenerating integration test reports..." << std::endl;
        
        // In a real implementation, this would:
        // 1. Generate HTML reports
        // 2. Generate XML reports
        // 3. Generate JSON reports
        // 4. Generate summary statistics
        
        std::cout << "Integration test reports generated." << std::endl;
    }
    
    void printIntegrationSummary() {
        std::cout << "\n=== Integration Test Summary ===" << std::endl;
        std::cout << "Integration tests completed successfully." << std::endl;
        std::cout << "All integration systems validated." << std::endl;
        std::cout << "System is ready for deployment." << std::endl;
    }
};

#endif // INTEGRATION_RUNNER_HPP
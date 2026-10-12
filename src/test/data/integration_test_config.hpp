// Integration test configuration
// 
// Configuration settings for NLM cross-system integration tests.
// Defines test parameters, thresholds, and scenarios for comprehensive integration testing.

#pragma once

#include "IntegrationTest.hpp"
#include <string>
#include <map>
#include <vector>

namespace nlm {
namespace test {

// Integration test configuration
class IntegrationTestConfig {
public:
    IntegrationTestConfig();
    ~IntegrationTestConfig();
    
    // Test parameters
    struct TestParameters {
        // Simulation parameters
        SimulationStep maxSteps = 1000;
        float timestep = 0.01f;
        size_t neuronCount = 100;
        size_t regionCount = 2;
        
        // Memory system parameters
        size_t workingMemoryCapacity = 50;
        float workingMemoryDecayRate = 0.01f;
        size_t episodicMemoryCapacity = 100;
        float episodicMemoryConsolidationThreshold = 0.7f;
        
        // Neuromodulation parameters
        float dopamineBaseline = 0.1f;
        float curiosityBaseline = 0.2f;
        float noveltyBaseline = 0.15f;
        float predictionErrorBaseline = 0.05f;
        
        // Prediction system parameters
        float predictionConfidenceThreshold = 0.6f;
        float predictionErrorThreshold = 0.2f;
        
        // Cognition system parameters
        float conceptFormationThreshold = 0.7f;
        float conceptStabilityThreshold = 0.8f;
        size_t planningDepth = 3;
        
        // Development system parameters
        double initialAge = 0.0;
        double maturityAge = 100.0;
        float criticalPeriodStart = 10.0;
        float criticalPeriodEnd = 30.0;
        
        // Performance parameters
        float maxMemoryUpdateTime = 0.01f;
        float maxIntegrationTime = 0.05f;
        size_t maxActiveNeurons = 50;
        
        // Cross-system parameters
        float memoryDevelopmentCoupling = 0.3f;
        float neuromodulationMotivationCoupling = 0.4f;
        float predictionBehaviorCoupling = 0.5f;
    };
    
    // Test scenarios
    struct TestScenario {
        std::string name;
        std::string description;
        TestParameters parameters;
        std::map<std::string, float> thresholds;
        bool enabled;
    };
    
    // Test cases
    struct TestCase {
        std::string name;
        std::string description;
        std::string testCategory;
        std::string testType; // unit, integration, cross-system, end-to-end
        double weight; // importance weight for overall test suite
        std::map<std::string, std::string> requirements; // system requirements
        bool requiresNeuromodulation;
        bool requiresPrediction;
        bool requiresDevelopment;
        bool requiresCrossSystem;
    };
    
    // Get test configuration
    static IntegrationTestConfig& getInstance();
    
    // Configuration management
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void resetToDefaults();
    
    // Get test configuration values
    const TestParameters& getTestParameters() const { return parameters_; }
    const std::vector<TestScenario>& getTestScenarios() const { return scenarios_; }
    const std::vector<TestCase>& getTestCases() const { return testCases_; }
    
    // Add/remove test scenarios and cases
    void addTestScenario(const TestScenario& scenario);
    void removeTestScenario(const std::string& name);
    void addTestCase(const TestCase& testCase);
    void removeTestCase(const std::string& name);
    
    // Test filtering
    std::vector<TestScenario> getEnabledScenarios() const;
    std::vector<TestCase> getEnabledTestCases() const;
    std::vector<TestCase> getTestCasesForCategory(const std::string& category) const;
    std::vector<TestCase> getCrossSystemTestCases() const;
    
    // System requirements checking
    bool meetsSystemRequirements(const TestCase& testCase) const;
    
    // Validation
    bool validateConfiguration() const;
    std::string getValidationErrors() const;
    
    // Special scenarios
    TestScenario getCriticalPeriodScenario() const;
    TestScenario getMemoryConsolidationScenario() const;
    TestScenario getNeuromodulationMotivationalScenario() const;
    TestScenario getPredictionBehaviorScenario() const;
    TestScenario getCompleteBrainLoopScenario() const;
    
    // Get default configuration
    static TestParameters getDefaultTestParameters();
    static std::vector<TestScenario> getDefaultTestScenarios();
    static std::vector<TestCase> getDefaultTestCases();
    
private:
    TestParameters parameters_;
    std::vector<TestScenario> scenarios_;
    std::vector<TestCase> testCases_;
    std::string validationErrors_;
    
    static IntegrationTestConfig instance_;
    
    void initializeDefaultScenarios();
    void initializeDefaultTestCases();
    void validateTestScenarios();
    void validateTestCases();
    bool checkTestCaseRequirements(const TestCase& testCase) const;
}; // class IntegrationTestConfig

} // namespace test
} // namespace nlm
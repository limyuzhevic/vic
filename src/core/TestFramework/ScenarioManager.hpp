#pragma once

#include "TestBase.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include < chrono>

namespace nlm {

class ScenarioManager {
public:
    struct TestScenario {
        std::string name;
        std::string description;
        std::string category;
        std::vector<std::string> testSequence; // Names of tests to run
        std::map<std::string, std::string> parameters; // Configuration overrides
        bool repeat; // Repeat the scenario
        int repeatCount; // Number of repetitions
        double probability; // Probability of running this scenario (0.0 to 1.0)
        std::vector<std::string> dependencies; // Other scenarios that must run first
        std::chrono::milliseconds timeout; // Maximum duration for scenario
        bool enabled; // Whether scenario is enabled
        
        // Constructor
        TestScenario(const std::string& name, const std::string& description = "")
            : name(name), description(description), repeat(false), 
              repeatCount(1), probability(1.0), enabled(true), timeout(std::chrono::milliseconds(3600)) {}
    };
    
    struct ScenarioResult {
        std::string scenarioName;
        bool passed;
        bool skipped;
        std::string errorMessage;
        std::chrono::milliseconds duration;
        std::vector<std::string> executedTests;
        std::map<std::string, std::string> metrics;
    };
    
    // Scenario management
    void loadScenarios(const std::string& configFile);
    void loadScenariosFromJSON(const std::string& jsonString);
    void loadScenariosFromYAML(const std::string& yamlString);
    
    // Scenario operations
    void addScenario(const TestScenario& scenario);
    bool removeScenario(const std::string& name);
    TestScenario* getScenario(const std::string& name);
    const TestScenario* getScenario(const std::string& name) const;
    
    // Scenario discovery
    std::vector<TestScenario> getAllScenarios() const;
    std::vector<TestScenario> getEnabledScenarios() const;
    std::vector<TestScenario> getScenariosByCategory(const std::string& category) const;
    std::vector<std::string> getCategories() const;
    
    // Scenario execution
    ScenarioResult runScenario(const TestScenario& scenario, 
                              const TestConfig& config,
                              std::shared_ptr<Brain> brain = nullptr);
    std::vector<ScenarioResult> runScenarios(
        const std::vector<std::string>& scenarioNames,
        const TestConfig& config,
        std::shared_ptr<Brain> brain = nullptr);
    std::vector<ScenarioResult> runAllScenarios(
        const TestConfig& config,
        std::shared_ptr<Brain> brain = nullptr);
    
    // Scenario filtering and selection
    std::vector<std::string> selectScenarios(const TestConfig& config) const;
    bool shouldRunScenario(const TestScenario& scenario, const TestConfig& config) const;
    
    // Scenario configuration
    void setScenarioParameter(const std::string& scenarioName, 
                             const std::string& parameter, 
                             const std::string& value);
    void enableScenario(const std::string& scenarioName);
    void disableScenario(const std::string& scenarioName);
    void setScenarioEnabled(const std::string& scenarioName, bool enabled);
    
    // Scenario lifecycle management
    void validateScenarios() const;
    void resolveDependencies();
    bool isDependencyResolved(const TestScenario& scenario) const;
    
    // Scenario metadata
    bool hasScenario(const std::string& name) const;
    size_t getScenarioCount() const;
    std::string getScenarioDescription(const std::string& name) const;
    
    // Scenario export/import
    bool exportScenarios(const std::string& filepath) const;
    bool importScenarios(const std::string& filepath);
    std::string exportScenariosToJSON() const;
    bool importScenariosFromJSON(const std::string& jsonString);
    
    // Scenario statistics
    size_t getScenarioCount(bool enabledOnly = true) const;
    std::map<std::string, size_t> getScenarioCountByCategory() const;
    
private:
    std::vector<TestScenario> scenarios_;
    std::unordered_map<std::string, TestScenario> scenarioMap_;
    mutable std::mutex mutex_;
    
    // Scenario validation and resolution
    void validateScenarioDependencies() const;
    void resolveScenarioOrder(std::vector<TestScenario*>& sortedScenarios) const;
    
    // Scenario execution helpers
    ScenarioResult executeScenarioTests(
        const TestScenario& scenario,
        const TestConfig& config,
        std::shared_ptr<Brain> brain);
    
    // Scenario filtering
    bool scenarioMatchesFilters(const TestScenario& scenario,
                               const TestConfig& config) const;
    
    // Scenario probability
    bool shouldRunByProbability(double probability) const;
};

// Scenario manager factory
class ScenarioManagerFactory {
public:
    static std::unique_ptr<ScenarioManager> createScenarioManager();
    static std::vector<std::string> getAvailableScenarioFormats();
};

// Scenario configuration loader
class ScenarioConfigLoader {
public:
    static bool loadScenarios(const std::string& filepath, ScenarioManager& manager);
    static bool loadScenariosFromJSON(const std::string& jsonString, ScenarioManager& manager);
    static bool loadScenariosFromYAML(const std::string& yamlString, ScenarioManager& manager);
    static bool saveScenarios(const std::string& filepath, const ScenarioManager& manager);
};

} // namespace nlm

#include "ScenarioManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace nlm {

void ScenarioManager::loadScenarios(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open scenario file: " + configFile);
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        loadScenariosFromJSON(jsonData.dump());
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse scenario file " + configFile + ": " + e.what());
    }
}

void ScenarioManager::loadScenariosFromJSON(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        if (jsonData.contains("scenarios")) {
            auto scenarios = jsonData["scenarios"];
            
            for (const auto& scenarioJson : scenarios) {
                TestScenario scenario;
                
                if (scenarioJson.contains("name")) {
                    scenario.name = scenarioJson["name"];
                }
                
                if (scenarioJson.contains("description")) {
                    scenario.description = scenarioJson["description"];
                }
                
                if (scenarioJson.contains("category")) {
                    scenario.category = scenarioJson["category"];
                }
                
                if (scenarioJson.contains("enabled")) {
                    scenario.enabled = scenarioJson["enabled"];
                }
                
                if (scenarioJson.contains("test_sequence")) {
                    scenario.testSequence.clear();
                    for (const auto& testName : scenarioJson["test_sequence"]) {
                        scenario.testSequence.push_back(testName);
                    }
                }
                
                if (scenarioJson.contains("parameters")) {
                    scenario.parameters.clear();
                    for (auto it = scenarioJson["parameters"].begin(); it != scenarioJson["parameters"].end(); ++it) {
                        scenario.parameters[it.key()] = it.value();
                    }
                }
                
                if (scenarioJson.contains("repeat")) {
                    scenario.repeat = scenarioJson["repeat"];
                }
                
                if (scenarioJson.contains("repeat_count")) {
                    scenario.repeatCount = scenarioJson["repeat_count"];
                }
                
                if (scenarioJson.contains("probability")) {
                    scenario.probability = scenarioJson["probability"];
                }
                
                if (scenarioJson.contains("timeout_ms")) {
                    scenario.timeout = std::chrono::milliseconds(scenarioJson["timeout_ms"]);
                }
                
                if (scenarioJson.contains("dependencies")) {
                    scenario.dependencies.clear();
                    for (const auto& dep : scenarioJson["dependencies"]) {
                        scenario.dependencies.push_back(dep);
                    }
                }
                
                addScenario(scenario);
            }
        }
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse JSON scenarios: " + std::string(e.what()));
    }
}

void ScenarioManager::loadScenariosFromYAML(const std::string& yamlString) {
    // TODO: Implement YAML parsing
    throw std::runtime_error("YAML scenario parsing not yet implemented");
}

void ScenarioManager::addScenario(const TestScenario& scenario) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (scenario.name.empty()) {
        throw std::invalid_argument("Scenario name cannot be empty");
    }
    
    // Check if scenario already exists
    if (hasScenario(scenario.name)) {
        removeScenario(scenario.name);
    }
    
    scenarios_.push_back(scenario);
    scenarioMap_[scenario.name] = scenario;
}

bool ScenarioManager::removeScenario(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarioMap_.find(name);
    if (it == scenarioMap_.end()) {
        return false;
    }
    
    // Remove from scenarios_ vector
    scenarios_.erase(std::remove_if(scenarios_.begin(), scenarios_.end(),
        [&name](const TestScenario& s) { return s.name == name; }),
        scenarios_.end());
    
    scenarioMap_.erase(it);
    return true;
}

TestScenario* ScenarioManager::getScenario(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarioMap_.find(name);
    return it != scenarioMap_.end() ? &it->second : nullptr;
}

const TestScenario* ScenarioManager::getScenario(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarioMap_.find(name);
    return it != scenarioMap_.end() ? &it->second : nullptr;
}

std::vector<TestScenario> ScenarioManager::getAllScenarios() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return scenarios_;
}

std::vector<TestScenario> ScenarioManager::getEnabledScenarios() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<TestScenario> enabledScenarios;
    
    for (const auto& scenario : scenarios_) {
        if (scenario.enabled) {
            enabledScenarios.push_back(scenario);
        }
    }
    
    return enabledScenarios;
}

std::vector<TestScenario> ScenarioManager::getScenariosByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<TestScenario> categoryScenarios;
    
    for (const auto& scenario : scenarios_) {
        if (scenario.category == category && scenario.enabled) {
            categoryScenarios.push_back(scenario);
        }
    }
    
    return categoryScenarios;
}

std::vector<std::string> ScenarioManager::getCategories() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> categories;
    std::unordered_set<std::string> uniqueCategories;
    
    for (const auto& scenario : scenarios_) {
        if (!scenario.category.empty() && uniqueCategories.find(scenario.category) == uniqueCategories.end()) {
            categories.push_back(scenario.category);
            uniqueCategories.insert(scenario.category);
        }
    }
    
    return categories;
}

ScenarioManager::ScenarioResult ScenarioManager::runScenario(const TestScenario& scenario,
                                                            const TestConfig& config,
                                                            std::shared_ptr<Brain> brain) {
    ScenarioResult result;
    result.scenarioName = scenario.name;
    result.passed = false;
    result.skipped = false;
    result.errorMessage = "";
    result.duration = std::chrono::milliseconds(0);
    
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        // Validate scenario
        if (!scenario.enabled) {
            result.skipped = true;
            result.errorMessage = "Scenario is disabled";
            return result;
        }
        
        // Check dependencies
        if (!isDependencyResolved(scenario)) {
            result.skipped = true;
            result.errorMessage = "Dependencies not resolved";
            return result;
        }
        
        // Check probability
        if (shouldRunByProbability(scenario.probability)) {
            result.skipped = true;
            result.errorMessage = "Probability check failed";
            return result;
        }
        
        // Run scenario tests
        std::vector<TestResult> testResults;
        
        // Get tests from test registry
        TestRegistry& registry = TestRegistry::getInstance();
        std::vector<std::unique_ptr<TestBase>> tests;
        
        // Find tests by name
        for (const std::string& testName : scenario.testSequence) {
            TestBase* test = registry.getTest(testName);
            if (test) {
                tests.push_back(std::unique_ptr<TestBase>(test));
            } else {
                result.errorMessage = "Test not found: " + testName;
                result.skipped = true;
                return result;
            }
        }
        
        if (tests.empty()) {
            result.errorMessage = "No tests found in scenario";
            result.skipped = true;
            return result;
        }
        
        // Run tests
        TestRunner::runTests(tests, config, brain);
        
        // Determine if scenario passed
        result.passed = true;
        for (const auto& testResult : testResults) {
            if (!testResult.passed && !testResult.skipped) {
                result.passed = false;
                if (!result.errorMessage.empty()) {
                    result.errorMessage += "; ";
                }
                result.errorMessage += "Test " + testResult.testName + " failed";
            }
        }
        
        // Add executed tests to result
        for (const auto& testResult : testResults) {
            result.executedTests.push_back(testResult.testName);
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = std::string("Exception: ") + e.what();
    }
    
    result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime);
    
    return result;
}

std::vector<ScenarioManager::ScenarioResult> ScenarioManager::runScenarios(
    const std::vector<std::string>& scenarioNames,
    const TestConfig& config,
    std::shared_ptr<Brain> brain) {
    
    std::vector<ScenarioResult> results;
    
    for (const auto& name : scenarioNames) {
        const TestScenario* scenario = getScenario(name);
        if (scenario) {
            results.push_back(runScenario(*scenario, config, brain));
        } else {
            ScenarioResult result;
            result.scenarioName = name;
            result.passed = false;
            result.skipped = true;
            result.errorMessage = "Scenario not found";
            results.push_back(result);
        }
    }
    
    return results;
}

std::vector<ScenarioManager::ScenarioResult> ScenarioManager::runAllScenarios(
    const TestConfig& config,
    std::shared_ptr<Brain> brain) {
    
    std::vector<ScenarioResult> results;
    std::vector<TestScenario> scenarios = getEnabledScenarios();
    
    for (const auto& scenario : scenarios) {
        results.push_back(runScenario(scenario, config, brain));
        
        // Repeat if requested
        if (scenario.repeat) {
            for (int i = 1; i < scenario.repeatCount; ++i) {
                results.push_back(runScenario(scenario, config, brain));
            }
        }
    }
    
    return results;
}

std::vector<std::string> ScenarioManager::selectScenarios(const TestConfig& config) const {
    std::vector<std::string> selectedScenarios;
    
    // Get scenarios from config
    std::vector<std::string> configScenarios = config.getStringList("scenarios");
    
    if (!configScenarios.empty()) {
        // Run only specified scenarios
        for (const auto& name : configScenarios) {
            const TestScenario* scenario = getScenario(name);
            if (scenario && scenario->enabled) {
                selectedScenarios.push_back(name);
            }
        }
    } else {
        // Run all enabled scenarios
        for (const auto& scenario : scenarios_) {
            if (scenario.enabled && shouldRunScenario(scenario, config)) {
                selectedScenarios.push_back(scenario.name);
            }
        }
    }
    
    return selectedScenarios;
}

bool ScenarioManager::shouldRunScenario(const TestScenario& scenario, const TestConfig& config) const {
    // Check if scenario matches filters
    if (!scenarioMatchesFilters(scenario, config)) {
        return false;
    }
    
    // Check probability
    return shouldRunByProbability(scenario.probability);
}

void ScenarioManager::setScenarioParameter(const std::string& scenarioName,
                                           const std::string& parameter,
                                           const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarioMap_.find(scenarioName);
    if (it != scenarioMap_.end()) {
        it->second.parameters[parameter] = value;
    }
}

void ScenarioManager::enableScenario(const std::string& scenarioName) {
    setScenarioEnabled(scenarioName, true);
}

void ScenarioManager::disableScenario(const std::string& scenarioName) {
    setScenarioEnabled(scenarioName, false);
}

void ScenarioManager::setScenarioEnabled(const std::string& scenarioName, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarioMap_.find(scenarioName);
    if (it != scenarioMap_.end()) {
        it->second.enabled = enabled;
    }
}

void ScenarioManager::validateScenarios() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& scenario : scenarios_) {
        // Validate scenario name
        if (scenario.name.empty()) {
            throw std::invalid_argument("Scenario name cannot be empty");
        }
        
        // Validate test names
        for (const auto& testName : scenario.testSequence) {
            if (testName.empty()) {
                throw std::invalid_argument("Test name cannot be empty in scenario " + scenario.name);
            }
        }
        
        // Validate dependencies
        for (const auto& dependency : scenario.dependencies) {
            if (!hasScenario(dependency)) {
                throw std::invalid_argument("Scenario " + scenario.name + " depends on unknown scenario " + dependency);
            }
        }
        
        // Validate probability
        if (scenario.probability < 0.0 || scenario.probability > 1.0) {
            throw std::invalid_argument("Scenario probability must be between 0.0 and 1.0 for " + scenario.name);
        }
        
        // Validate timeout
        if (scenario.timeout.count() <= 0) {
            throw std::invalid_argument("Scenario timeout must be positive for " + scenario.name);
        }
    }
}

void ScenarioManager::resolveDependencies() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Sort scenarios by dependencies
    std::vector<TestScenario*> sortedScenarios;
    std::vector<bool> visited(scenarios_.size(), false);
    
    for (size_t i = 0; i < scenarios_.size(); ++i) {
        if (!visited[i]) {
            resolveDependenciesHelper(i, visited, sortedScenarios);
        }
    }
    
    // Clear original scenarios and add sorted ones
    scenarios_.clear();
    scenarioMap_.clear();
    
    for (TestScenario* scenario : sortedScenarios) {
        scenarios_.push_back(*scenario);
        scenarioMap_[scenario->name] = *scenario;
    }
}

bool ScenarioManager::isDependencyResolved(const TestScenario& scenario) const {
    for (const auto& dependency : scenario.dependencies) {
        const TestScenario* depScenario = getScenario(dependency);
        if (!depScenario || !depScenario->enabled) {
            return false;
        }
    }
    return true;
}

bool ScenarioManager::scenarioMatchesFilters(const TestScenario& scenario, const TestConfig& config) const {
    // Check category filter
    std::vector<std::string> categories = config.getStringList("test_categories");
    if (!categories.empty() && !scenario.category.empty()) {
        if (std::find(categories.begin(), categories.end(), scenario.category) == categories.end()) {
            return false;
        }
    }
    
    // Check probability
    if (!shouldRunByProbability(scenario.probability)) {
        return false;
    }
    
    return true;
}

bool ScenarioManager::shouldRunByProbability(double probability) const {
    // Simple probability check
    if (probability >= 1.0) {
        return true;
    }
    
    // Generate random number (simplified - in real implementation would use proper RNG)
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        seeded = true;
    }
    
    int randomValue = rand() % 100;
    int threshold = static_cast<int>(probability * 100.0f);
    
    return randomValue < threshold;
}

void ScenarioManager::resolveDependenciesHelper(size_t index, std::vector<bool>& visited,
                                                  std::vector<TestScenario*>& sortedScenarios) {
    visited[index] = true;
    
    const TestScenario& scenario = scenarios_[index];
    
    // Resolve dependencies first
    for (const auto& dependency : scenario.dependencies) {
        const TestScenario* depScenario = getScenario(dependency);
        if (depScenario) {
            auto it = std::find_if(scenarios_.begin(), scenarios_.end(),
                                 [&depScenario](const TestScenario& s) { return &s == depScenario; });
            if (it != scenarios_.end()) {
                size_t depIndex = std::distance(scenarios_.begin(), it);
                if (!visited[depIndex]) {
                    resolveDependenciesHelper(depIndex, visited, sortedScenarios);
                }
            }
        }
    }
    
    sortedScenarios.push_back(const_cast<TestScenario*>(&scenario));
}

bool ScenarioManager::hasScenario(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return scenarioMap_.find(name) != scenarioMap_.end();
}

size_t ScenarioManager::getScenarioCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return scenarios_.size();
}

std::string ScenarioManager::getScenarioDescription(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarioMap_.find(name);
    if (it != scenarioMap_.end()) {
        return it->second.description;
    }
    
    return "";
}

bool ScenarioManager::exportScenarios(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Export as JSON
        nlohmann::json jsonData;
        jsonData["scenarios"] = nlohmann::json::array();
        
        for (const auto& scenario : scenarios_) {
            nlohmann::json scenarioJson;
            
            scenarioJson["name"] = scenario.name;
            scenarioJson["description"] = scenario.description;
            scenarioJson["category"] = scenario.category;
            scenarioJson["enabled"] = scenario.enabled;
            scenarioJson["repeat"] = scenario.repeat;
            scenarioJson["repeat_count"] = scenario.repeatCount;
            scenarioJson["probability"] = scenario.probability;
            scenarioJson["timeout_ms"] = scenario.timeout.count();
            
            scenarioJson["test_sequence"] = scenario.testSequence;
            
            scenarioJson["parameters"] = scenario.parameters;
            
            scenarioJson["dependencies"] = scenario.dependencies;
            
            jsonData["scenarios"].push_back(scenarioJson);
        }
        
        file << jsonData.dump(2);
        file.close();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error exporting scenarios: " << e.what() << std::endl;
        return false;
    }
}

bool ScenarioManager::importScenarios(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing scenarios
        clear();
        
        // Import scenarios from JSON
        if (jsonData.contains("scenarios")) {
            auto scenarios = jsonData["scenarios"];
            
            for (const auto& scenarioJson : scenarios) {
                TestScenario scenario;
                
                if (scenarioJson.contains("name")) {
                    scenario.name = scenarioJson["name"];
                }
                
                if (scenarioJson.contains("description")) {
                    scenario.description = scenarioJson["description"];
                }
                
                if (scenarioJson.contains("category")) {
                    scenario.category = scenarioJson["category"];
                }
                
                if (scenarioJson.contains("enabled")) {
                    scenario.enabled = scenarioJson["enabled"];
                }
                
                if (scenarioJson.contains("repeat")) {
                    scenario.repeat = scenarioJson["repeat"];
                }
                
                if (scenarioJson.contains("repeat_count")) {
                    scenario.repeatCount = scenarioJson["repeat_count"];
                }
                
                if (scenarioJson.contains("probability")) {
                    scenario.probability = scenarioJson["probability"];
                }
                
                if (scenarioJson.contains("timeout_ms")) {
                    scenario.timeout = std::chrono::milliseconds(scenarioJson["timeout_ms"]);
                }
                
                if (scenarioJson.contains("test_sequence")) {
                    scenario.testSequence.clear();
                    for (const auto& testName : scenarioJson["test_sequence"]) {
                        scenario.testSequence.push_back(testName);
                    }
                }
                
                if (scenarioJson.contains("parameters")) {
                    scenario.parameters.clear();
                    for (auto it = scenarioJson["parameters"].begin(); it != scenarioJson["parameters"].end(); ++it) {
                        scenario.parameters[it.key()] = it.value();
                    }
                }
                
                if (scenarioJson.contains("dependencies")) {
                    scenario.dependencies.clear();
                    for (const auto& dep : scenarioJson["dependencies"]) {
                        scenario.dependencies.push_back(dep);
                    }
                }
                
                addScenario(scenario);
            }
        }
        
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error importing scenarios: " << e.what() << std::endl;
        return false;
    }
}

std::string ScenarioManager::exportScenariosToJSON() const {
    nlohmann::json jsonData;
    jsonData["scenarios"] = nlohmann::json::array();
    
    for (const auto& scenario : scenarios_) {
        nlohmann::json scenarioJson;
        
        scenarioJson["name"] = scenario.name;
        scenarioJson["description"] = scenario.description;
        scenarioJson["category"] = scenario.category;
        scenarioJson["enabled"] = scenario.enabled;
        scenarioJson["repeat"] = scenario.repeat;
        scenarioJson["repeat_count"] = scenario.repeatCount;
        scenarioJson["probability"] = scenario.probability;
        scenarioJson["timeout_ms"] = scenario.timeout.count();
        
        scenarioJson["test_sequence"] = scenario.testSequence;
        
        scenarioJson["parameters"] = scenario.parameters;
        
        scenarioJson["dependencies"] = scenario.dependencies;
        
        jsonData["scenarios"].push_back(scenarioJson);
    }
    
    return jsonData.dump(2);
}

bool ScenarioManager::importScenariosFromJSON(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        // Clear existing scenarios
        clear();
        
        if (jsonData.contains("scenarios")) {
            auto scenarios = jsonData["scenarios"];
            
            for (const auto& scenarioJson : scenarios) {
                TestScenario scenario;
                
                if (scenarioJson.contains("name")) {
                    scenario.name = scenarioJson["name"];
                }
                
                if (scenarioJson.contains("description")) {
                    scenario.description = scenarioJson["description"];
                }
                
                if (scenarioJson.contains("category")) {
                    scenario.category = scenarioJson["category"];
                }
                
                if (scenarioJson.contains("enabled")) {
                    scenario.enabled = scenarioJson["enabled"];
                }
                
                if (scenarioJson.contains("repeat")) {
                    scenario.repeat = scenarioJson["repeat"];
                }
                
                if (scenarioJson.contains("repeat_count")) {
                    scenario.repeatCount = scenarioJson["repeat_count"];
                }
                
                if (scenarioJson.contains("probability")) {
                    scenario.probability = scenarioJson["probability"];
                }
                
                if (scenarioJson.contains("timeout_ms")) {
                    scenario.timeout = std::chrono::milliseconds(scenarioJson["timeout_ms"]);
                }
                
                if (scenarioJson.contains("test_sequence")) {
                    scenario.testSequence.clear();
                    for (const auto& testName : scenarioJson["test_sequence"]) {
                        scenario.testSequence.push_back(testName);
                    }
                }
                
                if (scenarioJson.contains("parameters")) {
                    scenario.parameters.clear();
                    for (auto it = scenarioJson["parameters"].begin(); it != scenarioJson["parameters"].end(); ++it) {
                        scenario.parameters[it.key()] = it.value();
                    }
                }
                
                if (scenarioJson.contains("dependencies")) {
                    scenario.dependencies.clear();
                    for (const auto& dep : scenarioJson["dependencies"]) {
                        scenario.dependencies.push_back(dep);
                    }
                }
                
                addScenario(scenario);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error importing scenarios from JSON: " << e.what() << std::endl;
        return false;
    }
}

void ScenarioManager::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    scenarios_.clear();
    scenarioMap_.clear();
}

} // namespace nlm

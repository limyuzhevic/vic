// TestConfiguration.cpp
#include "TestConfiguration.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace nlm {
namespace test {

bool TestConfiguration::loadFromFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path)) {
        return false;
    }

    std::string extension = path.extension().string();
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();

    if (extension == ".json" || extension == ".JSON") {
        return loadFromJSON(content);
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
        return loadFromYAML(content);
    }

    return false;
}

bool TestConfiguration::loadFromJSON(const std::string& jsonString) {
    // Simplified JSON parsing - in production, use a proper JSON library
    // This is a simplified implementation for demonstration
    try {
        // Parse JSON and populate suiteConfigs_, parameterConfigs_, fixtureConfigs_
        // This is a placeholder implementation
        return parseJSON(jsonString);
    } catch (const std::exception& e) {
        return false;
    }
}

bool TestConfiguration::loadFromYAML(const std::string& yamlString) {
    // Simplified YAML parsing - in production, use a proper YAML library
    // This is a placeholder implementation
    try {
        // Parse YAML and populate configs
        return parseYAML(yamlString);
    } catch (const std::exception& e) {
        return false;
    }
}

TestSuiteConfig* TestConfiguration::getSuiteConfig(const std::string& name) {
    auto it = std::find_if(suiteConfigs_.begin(), suiteConfigs_.end(),
        [&name](const TestSuiteConfig& config) { return config.name == name; });
    if (it != suiteConfigs_.end()) {
        return &(*it);
    }
    return nullptr;
}

TestParameterConfig* TestConfiguration::getParameterConfig(const std::string& name) {
    auto it = std::find_if(parameterConfigs_.begin(), parameterConfigs_.end(),
        [&name](const TestParameterConfig& config) { return config.name == name; });
    if (it != parameterConfigs_.end()) {
        return &(*it);
    }
    return nullptr;
}

TestFixtureConfig* TestConfiguration::getFixtureConfig(const std::string& name) {
    auto it = std::find_if(fixtureConfigs_.begin(), fixtureConfigs_.end(),
        [&name](const TestFixtureConfig& config) { return config.name == name; });
    if (it != fixtureConfigs_.end()) {
        return &(*it);
    }
    return nullptr;
}

bool TestConfiguration::validate() const {
    auto suiteErrors = validateSuiteConfigs();
    auto parameterErrors = validateParameterConfigs();
    auto fixtureErrors = validateFixtureConfigs();

    return suiteErrors.empty() && parameterErrors.empty() && fixtureErrors.empty();
}

std::string TestConfiguration::getValidationErrors() const {
    std::vector<std::string> allErrors;
    auto suiteErrors = validateSuiteConfigs();
    auto parameterErrors = validateParameterConfigs();
    auto fixtureErrors = validateFixtureConfigs();

    allErrors.insert(allErrors.end(), suiteErrors.begin(), suiteErrors.end());
    allErrors.insert(allErrors.end(), parameterErrors.begin(), parameterErrors.end());
    allErrors.insert(allErrors.end(), fixtureErrors.begin(), fixtureErrors.end());

    std::stringstream ss;
    for (size_t i = 0; i < allErrors.size(); ++i) {
        if (i > 0) ss << "\n";
        ss << allErrors[i];
    }

    return ss.str();
}

void TestConfiguration::applyToTest(TestBase& test) const {
    // Apply configuration to a specific test instance
    // This would typically involve setting parameters, fixtures, etc.
}

void TestConfiguration::applyToSuite(TestSuite& suite) const {
    // Apply configuration to a test suite
    // This would typically involve adding tests, setting up filtering, etc.
}

TestSuiteConfig TestConfiguration::getEnvironmentSuiteConfig(const std::string& environment) const {
    TestSuiteConfig config;
    // Find and return suite config for specific environment
    auto it = std::find_if(suiteConfigs_.begin(), suiteConfigs_.end(),
        [&environment](const TestSuiteConfig& c) { return c.environment == environment; });
    if (it != suiteConfigs_.end()) {
        return *it;
    }
    return config;
}

TestParameterConfig TestConfiguration::getEnvironmentParameterConfig(const std::string& environment) const {
    TestParameterConfig config;
    // Find and return parameter config for specific environment
    auto it = std::find_if(parameterConfigs_.begin(), parameterConfigs_.end(),
        [&environment](const TestParameterConfig& c) { return c.name.find(environment) != std::string::npos; });
    if (it != parameterConfigs_.end()) {
        return *it;
    }
    return config;
}

TestFixtureConfig TestConfiguration::getEnvironmentFixtureConfig(const std::string& environment) const {
    TestFixtureConfig config;
    // Find and return fixture config for specific environment
    auto it = std::find_if(fixtureConfigs_.begin(), fixtureConfigs_.end(),
        [&environment](const TestFixtureConfig& c) { return c.name.find(environment) != std::string::npos; });
    if (it != fixtureConfigs_.end()) {
        return *it;
    }
    return config;
}

void TestConfiguration::addSuiteConfig(const TestSuiteConfig& config) {
    suiteConfigs_.push_back(config);
}

void TestConfiguration::addParameterConfig(const TestParameterConfig& config) {
    parameterConfigs_.push_back(config);
}

void TestConfiguration::addFixtureConfig(const TestFixtureConfig& config) {
    fixtureConfigs_.push_back(config);
}

void TestConfiguration::merge(const TestConfiguration& other) {
    suiteConfigs_.insert(suiteConfigs_.end(), other.suiteConfigs_.begin(), other.suiteConfigs_.end());
    parameterConfigs_.insert(parameterConfigs_.end(), other.parameterConfigs_.begin(), other.parameterConfigs_.end());
    fixtureConfigs_.insert(fixtureConfigs_.end(), other.fixtureConfigs_.begin(), other.fixtureConfigs_.end());
    metadata_.insert(other.metadata_.begin(), other.metadata_.end());
}

bool TestConfiguration::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << saveToJSON();
    file.close();
    return true;
}

bool TestConfiguration::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << saveToYAML();
    file.close();
    return true;
}

std::string TestConfiguration::saveToJSON() const {
    // Generate JSON representation of configuration
    // This is a simplified implementation
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"suites\": [],\n";
    ss << "  \"parameters\": [],\n";
    ss << "  \"fixtures\": []\n";
    ss << "}\n";
    return ss.str();
}

std::string TestConfiguration::saveToYAML() const {
    // Generate YAML representation of configuration
    // This is a simplified implementation
    std::stringstream ss;
    ss << "suites:\n";
    ss << "  - []\n";
    ss << "parameters:\n";
    ss << "  - []\n";
    ss << "fixtures:\n";
    ss << "  - []\n";
    return ss.str();
}

std::vector<TestSuiteConfig> TestConfiguration::filterSuites(const std::function<bool(const TestSuiteConfig&)>& filter) const {
    std::vector<TestSuiteConfig> filtered;
    for (const auto& config : suiteConfigs_) {
        if (filter(config)) {
            filtered.push_back(config);
        }
    }
    return filtered;
}

std::vector<TestParameterConfig> TestConfiguration::filterParameters(const std::function<bool(const TestParameterConfig&)>& filter) const {
    std::vector<TestParameterConfig> filtered;
    for (const auto& config : parameterConfigs_) {
        if (filter(config)) {
            filtered.push_back(config);
        }
    }
    return filtered;
}

std::vector<TestFixtureConfig> TestConfiguration::filterFixtures(const std::function<bool(const TestFixtureConfig&)>& filter) const {
    std::vector<TestFixtureConfig> filtered;
    for (const auto& config : fixtureConfigs_) {
        if (filter(config)) {
            filtered.push_back(config);
        }
    }
    return filtered;
}

std::string TestConfiguration::generateDocumentation() const {
    std::stringstream ss;
    ss << "Test Configuration Documentation\n";
    ss << "==============================\n\n";

    ss << "Suites:\n";
    for (const auto& suite : suiteConfigs_) {
        ss << "  - " << suite.name << ": " << suite.description << "\n";
    }

    ss << "\nParameters:\n";
    for (const auto& param : parameterConfigs_) {
        ss << "  - " << param.name << " (" << param.type << ")\n";
    }

    ss << "\nFixtures:\n";
    for (const auto& fixture : fixtureConfigs_) {
        ss << "  - " << fixture.name << ": " << fixture.description << "\n";
    }

    return ss.str();
}

bool TestConfiguration::parseJSON(const std::string& jsonString) {
    // Placeholder implementation
    return true;
}

bool TestConfiguration::parseYAML(const std::string& yamlString) {
    // Placeholder implementation
    return true;
}

std::vector<std::string> TestConfiguration::validateSuiteConfigs() const {
    std::vector<std::string> errors;
    // Validate suite configurations
    return errors;
}

std::vector<std::string> TestConfiguration::validateParameterConfigs() const {
    std::vector<std::string> errors;
    // Validate parameter configurations
    return errors;
}

std::vector<std::string> TestConfiguration::validateFixtureConfigs() const {
    std::vector<std::string> errors;
    // Validate fixture configurations
    return errors;
}

} // namespace test
} // namespace nlm
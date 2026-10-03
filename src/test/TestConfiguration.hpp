// TestConfiguration.hpp
#pragma once

#include "TestFramework.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <filesystem>

namespace nlm {
namespace test {

// Test suite configuration from JSON/YAML
struct TestSuiteConfig {
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> tags;
    bool enabled{true};
    std::vector<std::string> testFiles;
    std::vector<std::string> testNames;
    std::unordered_map<std::string, std::string> parameters;
    std::chrono::nanoseconds timeout{std::chrono::seconds(30)};
    bool parallel{false};
    size_t parallelThreads{std::thread::hardware_concurrency()};
    std::string environment;
    std::string outputFormat;
    bool verbose{false};
    std::string skipPattern;
    std::string includePattern;

    TestSuiteConfig() = default;
    TestSuiteConfig(const std::string& name) : name(name) {}
};

// Test parameter configuration
struct TestParameterConfig {
    std::string name;
    std::string type;
    std::any defaultValue;
    std::vector<std::any> possibleValues;
    std::string description;
    bool required{false};
    std::string validationPattern;
    std::any minValue;
    std::any maxValue;
};

// Test fixture configuration
struct TestFixtureConfig {
    std::string name;
    std::string type; // "setup", "teardown", "environment"
    std::string description;
    std::string code;
    std::unordered_map<std::string, std::string> parameters;
    std::chrono::nanoseconds timeout{std::chrono::seconds(10)};
    bool shouldFail{false};
    std::string failureMessage;
};

// Test configuration parser and loader
class TestConfiguration {
public:
    TestConfiguration() = default;
    ~TestConfiguration() = default;

    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);

    // Get configuration data
    const std::vector<TestSuiteConfig>& getSuiteConfigs() const { return suiteConfigs_; }
    const std::vector<TestParameterConfig>& getParameterConfigs() const { return parameterConfigs_; }
    const std::vector<TestFixtureConfig>& getFixtureConfigs() const { return fixtureConfigs_; }

    // Get specific configuration
    TestSuiteConfig* getSuiteConfig(const std::string& name);
    TestParameterConfig* getParameterConfig(const std::string& name);
    TestFixtureConfig* getFixtureConfig(const std::string& name);

    // Configuration validation
    bool validate() const;
    std::string getValidationErrors() const;

    // Apply configuration to test instances
    void applyToTest(TestBase& test) const;
    void applyToSuite(TestSuite& suite) const;

    // Get configuration by environment
    TestSuiteConfig getEnvironmentSuiteConfig(const std::string& environment) const;
    TestParameterConfig getEnvironmentParameterConfig(const std::string& environment) const;
    TestFixtureConfig getEnvironmentFixtureConfig(const std::string& environment) const;

    // Configuration management
    void addSuiteConfig(const TestSuiteConfig& config);
    void addParameterConfig(const TestParameterConfig& config);
    void addFixtureConfig(const TestFixtureConfig& config);

    // Merge with another configuration
    void merge(const TestConfiguration& other);

    // Save configuration
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    std::string saveToJSON() const;
    std::string saveToYAML() const;

    // Configuration filtering and selection
    std::vector<TestSuiteConfig> filterSuites(const std::function<bool(const TestSuiteConfig&)>& filter) const;
    std::vector<TestParameterConfig> filterParameters(const std::function<bool(const TestParameterConfig&)>& filter) const;
    std::vector<TestFixtureConfig> filterFixtures(const std::function<bool(const TestFixtureConfig&)>& filter) const;

    // Configuration export
    std::string generateDocumentation() const;

private:
    std::vector<TestSuiteConfig> suiteConfigs_;
    std::vector<TestParameterConfig> parameterConfigs_;
    std::vector<TestFixtureConfig> fixtureConfigs_;
    std::unordered_map<std::string, std::string> metadata_;

    // JSON/YAML parsing helpers
    bool parseJSON(const std::string& jsonString);
    bool parseYAML(const std::string& yamlString);

    // Configuration validation helpers
    std::vector<std::string> validateSuiteConfigs() const;
    std::vector<std::string> validateParameterConfigs() const;
    std::vector<std::string> validateFixtureConfigs() const;
};

} // namespace test
} // namespace nlm
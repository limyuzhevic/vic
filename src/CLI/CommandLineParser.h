// CommandLineParser.h
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

struct CommandLineOption {
    std::string name;
    char shortOpt;
    std::string description;
    bool required;
    std::string defaultValue;
    // Action is optional - if null, option is just stored without side effects
    std::function<void(const std::string&)> action;
};

struct ParsedCommand {
    std::string command;
    std::map<std::string, std::string> options;
    std::vector<std::string> args;
    bool helpRequested;
    bool versionRequested;
};

class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser() = default;
    
    // Add a command with options
    void addCommand(const std::string& name, const std::string& description, 
                   const std::vector<CommandLineOption>& options = {});
    
    // Add a global option
    void addOption(const CommandLineOption& option);
    
    // Parse command line arguments
    ParsedCommand parse(int argc, char** argv);
    
    // Show help
    void showHelp() const;
    
    // Show version
    void showVersion() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// TestRunner.h
#pragma once

#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <functional>

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double executionTime;
    std::map<std::string, std::string> metadata;
};

class TestRunner {
public:
    TestRunner();
    ~TestRunner() = default;
    
    // Register a test function
    void registerTest(const std::string& name, 
                     std::function<void(std::shared_ptr<nlm::Config>)> testFunc,
                     const std::string& description = "");
    
    // Run a specific test
    TestResult runTest(const std::string& name, std::shared_ptr<nlm::Config> config);
    
    // Run all tests
    std::vector<TestResult> runAllTests(std::shared_ptr<nlm::Config> config);
    
    // List available tests
    void listTests() const;
    
    // Set verbose output
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool verbose_;
};

// ConfigurationManager.h
#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>

struct DefaultConfigValue {
    std::string key;
    std::string value;
    std::string description;
    nlm::ConfigSource source;
};

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager() = default;
    
    // Add default configuration values
    void addDefault(const DefaultConfigValue& defaultValue);
    
    // Validate configuration
    bool validateConfig(std::shared_ptr<nlm::Config> config) const;
    
    // Merge with defaults
    void applyDefaults(std::shared_ptr<nlm::Config> config) const;
    
    // Get all default keys
    std::vector<std::string> getDefaultKeys() const;
    
    // Show configuration summary
    void showConfigSummary(std::shared_ptr<nlm::Config> config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

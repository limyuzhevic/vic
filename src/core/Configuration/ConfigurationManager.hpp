#pragma once

#include <string>
#include <unordered_map>
#include "core/Config/Config.hpp"
#include "Validation.hpp"

namespace nlm {

// Forward declarations for brain types
class Brain;
class Neuron;

// Configuration Manager - Handles configuration loading, validation, and management
class ConfigurationManager {
public:
    ConfigurationManager() = default;
    ~ConfigurationManager() = default;
    
    // Load configuration from multiple sources (file, args, defaults)
    bool loadConfiguration(int argc, char** argv);
    
    // Validate configuration values
    std::vector<std::string> validateConfiguration() const;
    
    // Get configuration value with type safety
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue = T()) const;
    
    // Set configuration value
    void setValue(const std::string& key, const std::string& value);
    
    // Get configuration summary for logging
    std::string getConfigurationSummary() const;
    
    // Check if configuration is complete
    bool isComplete() const;
    
    // Get configuration file path
    std::string getConfigFilePath() const;
    
private:
    std::shared_ptr<Config> config_;
    std::string configFilePath_ = "configs/default.cfg";
    ConfigurationValidator validator_;
    
    // Default configuration values
    void loadDefaultConfiguration();
};

// Test Runner - Manages test execution and coordination
class TestRunner {
public:
    TestRunner(std::shared_ptr<ConfigurationManager> config);
    ~TestRunner() = default;
    
    // Run all configured tests
    int runAllTests(std::shared_ptr<Brain> brain);
    
    // Individual test methods
    bool runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    bool runPlasticityExperiment(std::shared_ptr<Brain> brain);
    bool runStdpVerification(std::shared_ptr<Brain> brain);
    
    // Get test statistics
    std::string getTestStatistics() const;
    
    // Enable/disable specific tests
    void setTestEnabled(const std::string& testName, bool enabled);
    
    // Get enabled tests
    std::vector<std::string> getEnabledTests() const;
    
private:
    std::shared_ptr<ConfigurationManager> config_;
    std::unordered_map<std::string, bool> testEnabled_;
    int testsRun_ = 0;
    int testsPassed_ = 0;
    int testsFailed_ = 0;
    
    // Test helper methods
    bool waitForSpikes(std::shared_ptr<Brain> brain, size_t expectedSpikes, size_t timeoutSteps = 1000);
    bool checkConvergence(std::vector<float>& weights, float tolerance = 0.01f);
};

// Brain Initializer - Handles brain setup, reset, and state management
class BrainInitializer {
public:
    BrainInitializer() = default;
    ~BrainInitializer() = default;
    
    // Create and initialize brain with given configuration
    std::shared_ptr<Brain> createBrain(std::shared_ptr<ConfigurationManager> config);
    
    // Reset brain to initial state
    bool resetBrain(std::shared_ptr<Brain> brain);
    
    // Log brain status and statistics
    void logBrainStatus(std::shared_ptr<Brain> brain);
    
    // Validate brain state
    bool validateBrainState(std::shared_ptr<Brain> brain) const;
    
    // Get initialization statistics
    std::string getInitializationStats() const;
    
private:
    int initializationCount_ = 0;
    int totalBrainSize_ = 0;
    
    // Brain setup helpers
    void setupNetworkTopology(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void setupSynapticConnections(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void enablePlasticity(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
};

// Command Line Parser - Enhanced command line argument parsing with validation
class CommandLineParser {
public:
    CommandLineParser(int argc, char** argv);
    ~CommandLineParser() = default;
    
    // Parse command line arguments
    bool parse();
    
    // Get parsed values
    std::string getConfigFilePath() const;
    std::string getLogLevel() const;
    std::string getLogFile() const;
    bool isHelpRequested() const;
    bool isVersionRequested() const;
    bool isBatchMode() const;
    bool isVerboseMode() const;
    
    // Get unrecognized arguments for extensibility
    std::vector<std::string> getUnrecognizedArgs() const;
    
    // Display help information
    void displayHelp() const;
    void displayVersion() const;
    
    // Validate parsed arguments
    std::vector<std::string> validateArguments() const;
    
private:
    int argc_;
    char** argv_;
    
    // Parsed values
    std::string configFilePath_;
    std::string logLevel_ = "info";
    std::string logFile_ = "";
    bool helpRequested_ = false;
    bool versionRequested_ = false;
    bool batchMode_ = false;
    bool verboseMode_ = false;
    std::vector<std::string> unrecognizedArgs_;
    
    // Argument parsing helpers
    bool parseArgument(const std::string& arg, size_t& currentArg);
    bool parseConfigArgument(const std::string& arg);
    bool parseLogArgument(const std::string& arg);
};
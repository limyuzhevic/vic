#pragma once

#include "../core/Config/Config.hpp"
#include "CLIParser.hpp"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace nlm {

class TestConfig {
public:
    struct TestParameters {
        // Test execution parameters
        std::string executionMode; // "sequential", "parallel", "pipeline"
        double timestep;
        int stepsPerTest;
        int maxSteps;
        bool resetBrainBetweenTests;
        bool saveBrainState;
        std::string brainSavePath;
        bool restoreBrainState;
        std::string brainLoadPath;
        
        // Output parameters
        std::string outputDirectory;
        std::string outputFormat; // "json", "xml", "csv", "plaintext"
        std::string reportFormat;
        bool generateReports;
        std::vector<std::string> reportFormats;
        std::string benchmarkMode;
        
        // Logging parameters
        std::string logLevel; // "debug", "info", "warning", "error", "critical"
        std::string logFile;
        bool enableProgress;
        int progressInterval;
        bool enableConsoleOutput;
        
        // Plugin parameters
        std::vector<std::string> enabledPlugins;
        std::vector<std::string> disabledPlugins;
        std::map<std::string, std::string> pluginConfig;
        
        // Scenario parameters
        std::string scenarioFile;
        std::string scenarioName;
        bool runScenarios;
        
        // Performance parameters
        int maxParallelTests;
        int timeoutSeconds;
        int memoryLimitMB;
        bool enableBenchmarking;
        
        // Validation parameters
        bool validateConfig;
        bool skipValidationErrors;
        
        // Default constructor
        TestParameters() :
            timestep(0.001),
            stepsPerTest(100),
            maxSteps(1000),
            resetBrainBetweenTests(true),
            saveBrainState(false),
            restoreBrainState(false),
            outputFormat("plaintext"),
            reportFormat("plaintext"),
            generateReports(true),
            enableProgress(true),
            progressInterval(100),
            enableConsoleOutput(true),
            runScenarios(false),
            maxParallelTests(1),
            timeoutSeconds(0),
            memoryLimitMB(0),
            enableBenchmarking(false),
            validateConfig(true),
            skipValidationErrors(false) {}
    };
    
    static TestConfig fromCLI(const CLIParser::TestOptions& options);
    static TestConfig fromFile(const std::string& filepath);
    static TestConfig fromDefaults();
    
    // Configuration management
    void mergeWithDefaults();
    bool validate(std::vector<std::string>& errors) const;
    
    // Get configuration values
    const TestParameters& getParameters() const { return parameters_; }
    const Config& getConfig() const { return config_; }
    
    // Configuration access
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getStringList(const std::string& key) const;
    
    // Update configuration
    void setParameter(const std::string& key, const std::string& value);
    void setParameter(const std::string& key, int value);
    void setParameter(const std::string& key, double value);
    void setParameter(const std::string& key, bool value);
    
private:
    Config config_;
    TestParameters parameters_;
    
    // Configuration file parsing
    bool loadFromJSON(const std::string& filepath);
    bool loadFromTOML(const std::string& filepath);
    bool loadFromINI(const std::string& filepath);
    
    // Parameter mapping
    static std::map<std::string, std::function<void(TestConfig&, const std::string&)>> paramParsers;
};

} // namespace nlm

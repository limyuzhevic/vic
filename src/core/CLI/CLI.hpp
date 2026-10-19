#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <chrono>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Command line option types
enum class OptionType {
    FLAG,
    STRING,
    INT,
    FLOAT,
    DOUBLE,
    VECTOR_STRING
};

// Command line option definition
struct CLIOption {
    std::string name;
    std::string description;
    OptionType type;
    std::string defaultValue;
    std::string shortOption;
    bool required;
    std::string category;
};

// Command line arguments parser
class CLIParser {
public:
    CLIParser(int argc, char** argv);
    ~CLIParser() = default;
    
    // Add option
    void addOption(const std::string& name, const std::string& description, 
                   OptionType type, const std::string& defaultValue = "", 
                   const std::string& shortOption = "", bool required = false,
                   const std::string& category = "General");
    
    // Parse arguments
    bool parse();
    
    // Get option value
    std::string getString(const std::string& name) const;
    int getInt(const std::string& name) const;
    float getFloat(const std::string& name) const;
    double getDouble(const std::string& name) const;
    bool getBool(const std::string& name) const;
    std::vector<std::string> getStringList(const std::string& name) const;
    
    // Check if option was provided
    bool hasOption(const std::string& name) const;
    
    // Get help text
    std::string getHelp() const;
    
    // Get unknown arguments
    const std::vector<std::string>& getUnknownArgs() const { return unknownArgs_; }
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs_; }
    
    // Get remaining args after --
    const std::vector<std::string>& getExtraArgs() const { return extraArgs_; }
    
private:
    int argc_;
    char** argv_;
    std::map<std::string, CLIOption> options_;
    std::map<std::string, std::string> parsedValues_;
    std::map<std::string, bool> flagValues_;
    std::vector<std::string> unknownArgs_;
    std::vector<std::string> positionalArgs_;
    std::vector<std::string> extraArgs_;
    
    void parseArgs();
    bool parseOption(const std::string& arg);
    void showHelp() const;
    std::string trim(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    std::string toLower(const std::string& str) const;
};

// Enhanced configuration manager
class EnhancedConfigManager {
public:
    EnhancedConfigManager();
    ~EnhancedConfigManager() = default;
    
    // Load configuration from various sources
    bool loadConfig(const std::string& configFile = "");
    bool loadFromCLI(const CLIParser& cli);
    bool loadFromDefaults();
    
    // Save configuration
    bool saveConfig(const std::string& configFile) const;
    bool exportConfig(const std::string& outputFile, const std::string& format = "json") const;
    
    // Get configuration value
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value
    void setValue(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate configuration
    bool validateConfig() const;
    std::string getValidationErrors() const;
    
    // Get configuration summary
    std::string getSummary() const;
    
    // Get config object
    std::shared_ptr<const Config> getConfig() const { return config_; }
    std::shared_ptr<Config> getConfig() { return config_; }
    
private:
    std::shared_ptr<Config> config_;
    std::vector<std::string> validationErrors_;
    
    void initializeDefaults();
    bool validateNeuronConfig() const;
    bool validatePlasticityConfig() const;
    bool validateNeuromodulationConfig() const;
    bool validateDevelopmentConfig() const;
};

// Enhanced logger with session tracking
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger() = default;
    
    // Initialize logger with session information
    void initialize(const std::string& sessionId = "", bool enableProgressTracking = false);
    
    // Log session start/end
    void logSessionStart(const std::string& sessionId);
    void logSessionEnd(const std::string& sessionId, double duration);
    
    // Log progress
    void logProgress(const std::string& stage, double progress, const std::string& message = "");
    
    // Log experiment run
    void logExperiment(const std::string& name, double duration, bool success, const std::string& details = "");
    
    // Get session statistics
    std::string getSessionStats() const;
    
    // Get performance metrics
    std::string getPerformanceMetrics() const;
    
    // Enable/disable features
    void enableProgressTracking(bool enable) { progressTrackingEnabled_ = enable; }
    void enableSessionLogging(bool enable) { sessionLoggingEnabled_ = enable; }
    void enablePerformanceMetrics(bool enable) { performanceMetricsEnabled_ = enable; }
    
private:
    double sessionStartTime_;
    bool progressTrackingEnabled_;
    bool sessionLoggingEnabled_;
    bool performanceMetricsEnabled_;
    std::map<std::string, double> stageTimes_;
    std::vector<std::string> experimentLogs_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Simulation runner with enhanced features
class SimulationRunner {
public:
    SimulationRunner(std::shared_ptr<Config> config, EnhancedLogger& logger);
    ~SimulationRunner() = default;
    
    // Run a specific test/experiment
    bool runTest(const std::string& testName, const std::map<std::string, std::string>& params = {});
    
    // Run all enabled tests
    bool runAllTests(const std::map<std::string, std::string>& globalParams = {});
    
    // Get available tests
    static std::vector<std::string> getAvailableTests();
    
    // Get test parameters
    static std::map<std::string, std::vector<std::string>> getTestParameters(const std::string& testName);
    
    // Get test description
    static std::string getTestDescription(const std::string& testName);
    
    // Save results
    bool saveResults(const std::string& filepath) const;
    bool exportResults(const std::string& filepath, const std::string& format = "json") const;
    
private:
    std::shared_ptr<Config> config_;
    EnhancedLogger& logger_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    
    void initializeBrain();
    bool runBasicConnectivityTest();
    bool runPlasticityExperiment();
    bool runStdpVerificationTest();
    bool runDevelopmentTest();
    bool runNeuromodulationTest();
    bool runMemoryTest();
    
    double measureExecutionTime(std::function<bool()> func);
};

// Interactive mode for advanced users
class InteractiveMode {
public:
    InteractiveMode(std::shared_ptr<Config> config, EnhancedLogger& logger);
    ~InteractiveMode() = default;
    
    // Start interactive session
    bool startSession();
    
    // Display main menu
    void displayMainMenu() const;
    
    // Handle user commands
    bool handleCommand(const std::string& command);
    
    // Display help
    void displayHelp() const;
    
    // Display brain status
    void displayBrainStatus() const;
    
    // Display configuration
    void displayConfiguration() const;
    
    // Modify configuration
    bool modifyConfiguration(const std::string& key, const std::string& value);
    
    // Run experiments
    bool runExperiment(const std::string& name, const std::map<std::string, std::string>& params = {});
    
    // Export data
    bool exportBrainState(const std::string& filepath) const;
    bool exportLogs(const std::string& filepath) const;
    
private:
    std::shared_ptr<Config> config_;
    EnhancedLogger& logger_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimulationClock> clock_;
    bool sessionActive_;
    
    void initializeBrain();
    std::string getCurrentPrompt() const;
    bool executeCommand(const std::string& commandLine);
};

// Batch mode for automated runs
class BatchMode {
public:
    BatchMode(std::shared_ptr<Config> config, EnhancedLogger& logger);
    ~BatchMode() = default;
    
    // Run batch of experiments
    bool runBatch(const std::vector<std::string>& experimentNames, 
                  const std::vector<std::map<std::string, std::string>>& params = {});
    
    // Run parameter sweep
    bool runParameterSweep(const std::string& experimentName, 
                          const std::string& parameterName, 
                          const std::vector<std::string>& parameterValues);
    
    // Get batch results
    std::string getBatchResults() const;
    
    // Save batch results
    bool saveBatchResults(const std::string& filepath) const;
    
private:
    std::shared_ptr<Config> config_;
    EnhancedLogger& logger_;
    std::vector<std::string> batchResults_;
};

} // namespace nlm
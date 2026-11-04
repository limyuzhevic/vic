#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

/**
 * Advanced command-line argument parser
 * Supports complex argument parsing with validation and help generation
 */
class CommandLineParser {
public:
    // Argument definition
    struct ArgumentDef {
        std::string name;
        char shortOpt;
        std::string longOpt;
        std::string description;
        std::string defaultValue;
        bool required;
        bool hasValue;
        std::vector<std::string> allowedValues;
        std::function<bool(const std::string&)> validator;
        std::string validationError;
    };
    
    CommandLineParser();
    ~CommandLineParser();
    
    // Disable copying
    CommandLineParser(const CommandLineParser&) = delete;
    CommandLineParser& operator=(const CommandLineParser&) = delete;
    
    // Add argument
    void addArgument(const std::string& name, char shortOpt = '\0',
                    const std::string& longOpt = "",
                    const std::string& description = "",
                    const std::string& defaultValue = "",
                    bool required = false,
                    bool hasValue = false,
                    const std::vector<std::string>& allowedValues = {});
    
    // Add argument with validator
    void addArgument(const std::string& name,
                    std::function<bool(const std::string&)> validator,
                    const std::string& validationError = "",
                    char shortOpt = '\0', const std::string& longOpt = "",
                    const std::string& description = "",
                    const std::string& defaultValue = "",
                    bool required = false, bool hasValue = false,
                    const std::vector<std::string>& allowedValues = {});
    
    // Parse command line arguments
    bool parse(int argc, char** argv, std::string& errorMessage);
    
    // Get parsed values
    std::string getValue(const std::string& name) const;
    int getInt(const std::string& name) const;
    double getDouble(const std::string& name) const;
    bool getBool(const std::string& name) const;
    std::vector<std::string> getStringList(const std::string& name) const;
    
    // Check if argument was provided
    bool hasValue(const std::string& name) const;
    bool isSet(const std::string& name) const;
    
    // Argument information
    bool hasArgument(const std::string& name) const;
    std::string getArgumentDescription(const std::string& name) const;
    std::string getArgumentDefault(const std::string& name) const;
    bool isRequired(const std::string& name) const;
    
    // Help and usage
    std::string generateHelp() const;
    std::string generateUsage(const std::string& programName) const;
    bool validateArguments(std::string& errorMessage) const;
    
    // Configuration
    void setCaseInsensitive(bool insensitive) { caseInsensitive = insensitive; }
    void setAllowUnknown(bool allow) { allowUnknown = allow; }
    void setEqualSignSeparator(bool use) { equalSignSeparator = use; }
    void setHelpOnError(bool help) { helpOnError = help; }
    
    // Statistics
    size_t getArgumentCount() const { return argumentDefs.size(); }
    size_t getProvidedArgumentCount() const { return providedArguments.size(); }
    size_t getRequiredArgumentCount() const;
    
    // Reset
    void clear();
    void resetToDefaults();
    
private:
    // Internal data structures
    std::vector<ArgumentDef> argumentDefs;
    std::unordered_map<std::string, std::string> providedArguments;
    std::unordered_map<std::string, bool> argumentSet;
    
    // Configuration
    bool caseInsensitive;
    bool allowUnknown;
    bool equalSignSeparator;
    bool helpOnError;
    
    // Parsing state
    std::vector<std::string> unknownArgs;
    bool parseError;
    std::string parseErrorMessage;
    
    // Helper methods
    std::string normalizeName(const std::string& name) const;
    bool findArgument(const std::string& name, ArgumentDef*& argDef) const;
    bool parseShortOption(std::string& arg, std::string& remaining);
    bool parseLongOption(std::string& arg, std::string& remaining);
    bool parseEqualSignOption(std::string& arg, std::string& remaining);
    bool validateArgumentValue(const ArgumentDef& arg, const std::string& value) const;
    std::string getTypeString(const ArgumentDef& arg) const;
    void setError(const std::string& message) { parseError = true; parseErrorMessage = message; }
    
    // String utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringStartsWith(const std::string& str, const std::string& prefix);
    static size_t findCaseInsensitive(const std::string& str, const std::string& target);
    
    // Friend classes
    friend class CommandLineInterface;
    friend class NLMCommandLineInterface;
};

/**
 * Command-line interface base class
 * Provides common functionality for command-line interfaces
 */
class CommandLineInterface {
public:
    CommandLineInterface();
    virtual ~CommandLineInterface();
    
    // Disable copying
    CommandLineInterface(const CommandLineInterface&) = delete;
    CommandLineInterface& operator=(const CommandLineInterface&) = delete;
    
    // Initialize interface
    virtual bool initialize(int argc, char** argv);
    
    // Run interface
    virtual int run() = 0;
    
    // Cleanup
    virtual void cleanup();
    
    // Interface management
    void setParser(std::shared_ptr<CommandLineParser> parser) { commandParser = parser; }
    std::shared_ptr<CommandLineParser> getParser() const { return commandParser; }
    
    // Configuration
    void setProgramName(const std::string& name) { programName = name; }
    void setVersion(const std::string& version) { programVersion = version; }
    void setDescription(const std::string& desc) { programDescription = desc; }
    
    // Access
    std::string getProgramName() const { return programName; }
    std::string getVersion() const { return programVersion; }
    std::string getDescription() const { return programDescription; }
    
    // Configuration
    void enableColorOutput(bool enable) { colorOutput = enable; }
    void setOutputFile(const std::string& filepath) { outputFile = filepath; }
    void enableVerbose(bool enable) { verbose = enable; }
    
    // Statistics
    size_t getArgumentCount() const;
    size_t getProvidedArgumentCount() const;
    
    // Report generation
    std::string generateHelp() const;
    std::string generateVersionInfo() const;
    std::string generateUsage() const;
    
protected:
    // Helper methods
    virtual void setupArguments() = 0;
    virtual bool validateArguments() = 0;
    virtual int executeCommand() = 0;
    virtual void showHelp() const;
    virtual void showError(const std::string& message) const;
    virtual void showUsage() const;
    
    // Utility methods
    std::string formatOutput(const std::string& text, const std::string& color = "") const;
    bool isColorEnabled() const;
    std::ofstream getOutputStream() const;
    
    // Configuration
    std::shared_ptr<CommandLineParser> commandParser;
    std::string programName;
    std::string programVersion;
    std::string programDescription;
    bool colorOutput;
    std::string outputFile;
    bool verbose;
    
private:
    // File output
    mutable std::ofstream outputStream;
};

/**
 * NLM-specific command-line interface
 * Tailored for Neural Learning Machine (NLM) configuration and execution
 */
class NLMCommandLineInterface : public CommandLineInterface {
public:
    NLMCommandLineInterface();
    ~NLMCommandLineInterface() override;
    
    // Disable copying
    NLMCommandLineInterface(const NLMCommandLineInterface&) = delete;
    NLMCommandLineInterface& operator=(const NLMCommandLineInterface&) = delete;
    
    // Initialize with NLM-specific arguments
    bool initialize(int argc, char** argv) override;
    
    // Run NLM command line interface
    int run() override;
    
    // Cleanup
    void cleanup() override;
    
    // NLM-specific setup
    void setupArguments() override;
    
    // NLM-specific validation
    bool validateArguments() override;
    
    // NLM-specific execution
    int executeCommand() override;
    
    // NLM-specific help and usage
    void showHelp() const override;
    void showVersionInfo() const;
    void showSimulationHelp() const;
    void showCheckpointHelp() const;
    void showPerformanceHelp() const;
    void showDevelopmentHelp() const;
    void showNeuromodulationHelp() const;
    
    // NLM-specific functionality
    void createCheckpoint() const;
    void listCheckpoints() const;
    void restoreCheckpoint() const;
    void showPerformanceMetrics() const;
    void showDevelopmentStatus() const;
    void showNeuromodulationStatus() const;
    void exportConfiguration() const;
    void importConfiguration() const;
    
    // Configuration management
    void setConfigurationFile(const std::string& filepath) { configFile = filepath; }
    void setDefaultConfig() const;
    void setQuickTestConfig() const;
    void setResearchConfig() const;
    
    // Execution modes
    void runSimulation() const;
    void runBenchmark() const;
    void runExperiment() const;
    void runPhaseDemo(int phase) const;
    
    // Configuration access
    std::string getConfigValue(const std::string& key) const;
    void setConfigValue(const std::string& key, const std::string& value);
    
private:
    // NLM-specific configuration
    std::string configFile;
    
    // Execution state
    bool showHelpOnly;
    bool showVersionOnly;
    bool showUsageOnly;
    bool createCheckpointOnly;
    bool listCheckpointsOnly;
    bool restoreCheckpointOnly;
    bool showPerformanceMetricsOnly;
    bool showDevelopmentStatusOnly;
    bool showNeuromodulationStatusOnly;
    bool exportConfigOnly;
    bool importConfigOnly;
    bool setDefaultConfigOnly;
    bool setQuickTestConfigOnly;
    bool setResearchConfigOnly;
    bool runSimulationOnly;
    bool runBenchmarkOnly;
    bool runExperimentOnly;
    
    // Helper methods
    void parseExecutionMode(int& phase, std::string& experimentName) const;
    void executePhaseDemo(int phase) const;
    void executeExperiment(const std::string& experimentName) const;
    bool loadConfiguration(const std::string& filepath) const;
    bool saveConfiguration(const std::string& filepath) const;
    
    // Configuration validation
    bool validateConfigValue(const std::string& key, const std::string& value) const;
    bool validatePhase(int phase) const;
    bool validateExperimentName(const std::string& name) const;
    
    // Help generation
    void addConfigurationHelp(std::stringstream& ss) const;
    void addExecutionModeHelp(std::stringstream& ss) const;
    void addExperimentHelp(std::stringstream& ss) const;
    void addPhaseHelp(std::stringstream& ss) const;
    void addCheckpointHelp(std::stringstream& ss) const;
    void addPerformanceHelp(std::stringstream& ss) const;
    void addDevelopmentHelp(std::stringstream& ss) const;
    void addNeuromodulationHelp(std::stringstream& ss) const;
    
    // Statistics
    void updateStatistics();
    void resetStatistics();
};

} // namespace nlm

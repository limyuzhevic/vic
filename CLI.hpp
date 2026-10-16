// NLM CLI Infrastructure
// Comprehensive command-line interface for Neural Learning Machine
// Provides modern argument parsing, configuration management, session handling, and output formatting

#ifndef CLI_HPP
#define CLI_HPP

#include "CLI/CLIParser.hpp"
#include "CLI/ConfigManager.hpp"
#include "CLI/SessionManager.hpp"
#include "CLI/OutputFormatter.hpp"

namespace nlm {

// Main CLI class that orchestrates all components
class CLI {
private:
    std::unique_ptr<ArgumentParser> parser;
    std::unique_ptr<ConfigManager> configManager;
    std::unique_ptr<SessionManager> sessionManager;
    std::unique_ptr<OutputFormatter> outputFormatter;
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    bool interactiveMode;
    bool scriptMode;
    bool autoSave;
    bool validationMode;
    
public:
    CLI();
    ~CLI() = default;
    
    // Core functionality
    bool initialize(int argc, char** argv);
    bool run();
    bool runScript(const std::string& scriptPath);
    bool runInteractive();
    
    // Configuration management
    bool loadConfig(const std::string& configPath);
    bool saveConfig(const std::string& configPath);
    bool validateConfig();
    bool validateArguments();
    
    // Session management
    bool saveSession(const std::string& sessionPath);
    bool loadSession(const std::string& sessionPath);
    bool compareSessions(const std::string& sessionPath1, const std::string& sessionPath2);
    bool backupSession(const std::string& sessionPath);
    bool restoreSession(const std::string& sessionPath);
    
    // Output formatting
    void setOutputFormat(const std::string& format);
    void setOutputFilter(const std::string& filter);
    void setLogLevel(const std::string& level);
    void setProgress(const std::string& title, double progress, const std::string& message = "");
    void finishProgress(const std::string& title);
    
    // Configuration options
    void addBrainOptions();
    void addMemoryOptions();
    void addPlasticityOptions();
    void addDevelopmentOptions();
    void addNeuromodulationOptions();
    void addPerformanceOptions();
    void addOutputOptions();
    void addDebugOptions();
    void addLoggingOptions();
    void addExperimentOptions();
    
    // Help and documentation
    void printHelp() const;
    void printVersion() const;
    void printConfiguration() const;
    void printSystemInfo() const;
    
    // Error handling
    bool handleError(const std::string& error) const;
    void setupErrorHandling();
    
    // Configuration validation
    bool isConfigValid() const { return validationMode && config && configManager && configManager->validateConfig().valid; }
    
    // State management
    bool isReady() const { return parser && config && brain; }
    const std::shared_ptr<Config>& getConfig() const { return config; }
    const std::shared_ptr<Brain>& getBrain() const { return brain; }
    
    // CLI commands
    bool parseCommand(const std::string& commandLine);
    bool executeCommand(const std::string& command, const std::map<std::string, std::string>& args);
    bool processCLIArgs(int argc, char** argv);
    
    // Session commands
    bool sessionSaveCommand(const std::map<std::string, std::string>& args);
    bool sessionLoadCommand(const std::map<std::string, std::string>& args);
    bool sessionListCommand(const std::map<std::string, std::string>& args);
    bool sessionCompareCommand(const std::map<std::string, std::string>& args);
    bool sessionDeleteCommand(const std::map<std::string, std::string>& args);
    bool sessionBackupCommand(const std::map<std::string, std::string>& args);
    bool sessionRestoreCommand(const std::map<std::string, std::string>& args);
    
    // Configuration commands
    bool configSaveCommand(const std::map<std::string, std::string>& args);
    bool configLoadCommand(const std::map<std::string, std::string>& args);
    bool configValidateCommand(const std::map<std::string, std::string>& args);
    bool configListCommand(const std::map<std::string, std::string>& args);
    bool configMergeCommand(const std::map<std::string, std::string>& args);
    bool configDiffCommand(const std::map<std::string, std::string>& args);
    
    // Output commands
    bool outputFormatCommand(const std::map<std::string, std::string>& args);
    bool outputFilterCommand(const std::map<std::string, std::string>& args);
    bool outputProgressCommand(const std::map<std::string, std::string>& args);
    bool outputStatisticsCommand(const std::map<std::string, std::string>& args);
    bool outputSummaryCommand(const std::map<std::string, std::string>& args);
    
    // Utility commands
    bool helpCommand(const std::map<std::string, std::string>& args);
    bool versionCommand(const std::map<std::string, std::string>& args);
    bool exitCommand(const std::map<std::string, std::string>& args);
    bool runCommand(const std::map<std::string, std::string>& args);
    bool scriptCommand(const std::map<std::string, std::string>& args);
    
    // Configuration group helpers
    void setupBrainConfiguration(const std::string& section = "brain");
    void setupMemoryConfiguration(const std::string& section = "memory");
    void setupPlasticityConfiguration(const std::string& section = "plasticity");
    void setupDevelopmentConfiguration(const std::string& section = "development");
    void setupNeuromodulationConfiguration(const std::string& section = "neuromodulation");
    void setupPerformanceConfiguration(const std::string& section = "performance");
    void setupOutputConfiguration(const std::string& section = "output");
    void setupDebugConfiguration(const std::string& section = "debug");
    void setupLoggingConfiguration(const std::string& section = "logging");
    void setupExperimentConfiguration(const std::string& section = "experiment");
    
private:
    void setupAllComponents();
    void loadDefaultConfiguration();
    void applyConfigurationOverrides();
    void initializeBrain();
    void runExperiments();
    void outputResults();
    void saveAutoSession();
    std::string getSessionFilename(const std::string& prefix) const;
};

} // namespace nlm

#endif // CLI_HPP

// Advanced Command Line Interface
// Provides enhanced options for users, including configuration management,
// output formatting, and advanced debugging features

#ifndef ADVANCED_CLI_HPP
#define ADVANCED_CLI_HPP

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <memory>

namespace nlm {
namespace cli {

class AdvancedCLI {
public:
    AdvancedCLI();
    ~AdvancedCLI();
    
    // Parse command line arguments
    bool parseArgs(int argc, char** argv);
    
    // Execute the requested command
    int execute();
    
    // Print help information
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Print configuration details
    void printConfig() const;
    
    // Export configuration to file
    bool exportConfig(const std::string& filename) const;
    
    // Import configuration from file
    bool importConfig(const std::string& filename);
    
    // Validate configuration
    bool validateConfig() const;
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    // Check if running in quiet mode
    bool isQuiet() const { return quiet_; }
    
    // Check if running in verbose mode
    bool isVerbose() const { return verbose_; }
    
    // Check if running in debug mode
    bool isDebug() const { return debug_; }
    
    // Get output format
    const std::string& getOutputFormat() const { return outputFormat_; }
    
    // Get log level
    const std::string& getLogLevel() const { return logLevel_; }
    
    // Get configuration file
    const std::string& getConfigFile() const { return configFile_; }
    
    // Get workspace directory
    const std::string& getWorkspaceDir() const { return workspaceDir_; }
    
private:
    // Command line arguments
    int argc_;
    char** argv_;
    
    // Configuration
    std::shared_ptr<Config> config_;
    
    // CLI options
    bool help_;
    bool version_;
    bool quiet_;
    bool verbose_;
    bool debug_;
    std::string outputFormat_;
    std::string logLevel_;
    std::string configFile_;
    std::string workspaceDir_;
    
    // Commands
    std::string command_;
    std::vector<std::string> commandArgs_;
    
    // Command handlers
    bool handleHelpCommand() const;
    bool handleVersionCommand() const;
    bool handleRunCommand() const;
    bool handleConfigCommand() const;
    bool handleExportCommand() const;
    bool handleImportCommand() const;
    bool handleValidateCommand() const;
    
    // Configuration handling
    bool loadConfiguration();
    bool saveConfiguration() const;
    bool applyConfigurationFromArgs();
    
    // Argument parsing
    void parseCommandLine();
    void parseCommand(const std::string& cmd);
    void parseArgs(const std::string& key, const std::string& value);
    
    // Validation
    bool validateArguments() const;
    bool validateConfiguration() const;
    
    // Output formatting
    void printOutput(const std::string& message, bool useFormat = false) const;
    void printError(const std::string& error) const;
    void printWarning(const std::string& warning) const;
    
    // File operations
    bool readFile(const std::string& filename, std::string& content) const;
    bool writeFile(const std::string& filename, const std::string& content) const;
    
    // Configuration value conversion
    template<typename T>
    T convertStringToType(const std::string& value) const;
    
    template<typename T>
    std::string convertTypeToString(const T& value) const;
    
    // Command completion
    void completeCommand(std::string& cmd) const;
};

// Command line option definitions
struct CommandOption {
    std::string flag;
    std::string description;
    std::string defaultValue;
    bool requiresArgument;
    bool isAdvanced;
};

// Command definitions
namespace commands {
    const std::string RUN = "run";
    const std::string CONFIG = "config";
    const std::string EXPORT = "export";
    const std::string IMPORT = "import";
    const std::string VALIDATE = "validate";
}

// Output format options
namespace formats {
    const std::string TEXT = "text";
    const std::string JSON = "json";
    const std::string YAML = "yaml";
    const std::string CSV = "csv";
}

// Log level options
namespace logLevels {
    const std::string DEBUG = "debug";
    const std::string INFO = "info";
    const std::string WARNING = "warning";
    const std::string ERROR = "error";
    const std::string CRITICAL = "critical";
}

} // namespace cli
} // namespace nlm

#endif // ADVANCED_CLI_HPP
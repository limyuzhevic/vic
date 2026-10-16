// NLM Command Line Interface - Header
// Phase 6: Final Integration - Advanced User Interface

#pragma once

#include <string>
#include <map>
#include <memory>
#include "core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class CLICommand;
class CLIConfigManager;
class CLISimulationManager;
class CLIOutputFormatter;

class NLMCLI;
class HelpCommand;
class VersionCommand;
class ConfigCommand;
class PresetCommand;
class RunCommand;
class BatchCommand;
class InteractiveCommand;
class TestCommand;
class BenchmarkCommand;
class ExportCommand;
class ImportCommand;

/**
 * Main CLI application class
 */
class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI() = default;
    
    // Main entry point
    int run(int argc, char** argv);
    
    // Display help and usage information
    void showHelp() const;
    void showVersion() const;
    void showConfigurationPresets() const;
    
    // Configuration management
    bool loadConfiguration(const std::string& filename);
    bool saveConfiguration(const std::string& filename) const;
    void applyConfigurationPresets(const std::string& preset_name);
    
    // Command execution
    int executeCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Interactive mode
    int runInteractiveMode();
    
    // Batch mode
    int runBatchMode(const std::string& script_file);
    
    // Simulation execution
    int runSimulation(const std::map<std::string, std::string>& params);
    int runPhase6Demo(const std::map<std::string, std::string>& params);
    int runBenchmark(const std::map<std::string, std::string>& params);
    int runTest(const std::string& test_name, const std::map<std::string, std::string>& params);
    
    // Accessors
    const std::map<std::string, CLICommand*>& getCommandRegistry() const { return command_registry_; }
    
private:
    // Command registry
    std::map<std::string, CLICommand*> command_registry_;
    
    // Configuration manager
    CLIConfigManager* config_manager_;
    
    // Simulation manager
    CLISimulationManager* simulation_manager_;
    
    // Output formatter
    CLIOutputFormatter* output_formatter_;
    
    // Global configuration
    std::shared_ptr<Config> global_config_;
    
    // Command line arguments
    int argc_;
    char** argv_;
    
    // Current working directory
    std::string working_directory_;
    
    // Register commands
    void registerCommands();
};

/**
 * Command base class
 */
class CLICommand {
public:
    CLICommand(const std::string& name, const std::string& description);
    virtual ~CLICommand() = default;
    
    virtual int execute(const std::vector<std::string>& args) = 0;
    virtual void showHelp() const;
    
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
protected:
    std::string name_;
    std::string description_;
};

/**
 * Configuration manager for CLI
 */
class CLIConfigManager {
public:
    CLIConfigManager(std::shared_ptr<Config> config);
    ~CLIConfigManager() = default;
    
    // Configuration presets
    void loadPreset(const std::string& preset_name);
    void savePreset(const std::string& preset_name) const;
    bool presetExists(const std::string& preset_name) const;
    
    // Configuration manipulation
    void setParameter(const std::string& key, const std::string& value);
    std::string getParameter(const std::string& key) const;
    bool hasParameter(const std::string& key) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration serialization
    std::string toJSON() const;
    bool fromJSON(const std::string& json_string);
    
    // Default configurations
    static std::map<std::string, std::shared_ptr<Config>> getDefaultPresets();
    
private:
    std::shared_ptr<Config> config_;
    std::map<std::string, std::shared_ptr<Config>> presets_;
};

/**
 * Simulation manager for CLI
 */
class CLISimulationManager {
public:
    CLISimulationManager(std::shared_ptr<Config> config);
    ~CLISimulationManager() = default;
    
    // Simulation execution
    bool runPhase2Demo(const std::map<std::string, std::string>& params);
    bool runPhase6Demo(const std::map<std::string, std::string>& params);
    bool runBenchmark(const std::map<std::string, std::string>& params);
    bool runTest(const std::string& test_name, const std::map<std::string, std::string>& params);
    
    // Configuration management
    void updateConfig(const std::map<std::string, std::string>& params);
    
    // Status and statistics
    std::string getStatus() const;
    std::string getPerformanceMetrics() const;
    
private:
    std::shared_ptr<Config> config_;
    std::shared_ptr<class Brain> brain_;
};

/**
 * Output formatter for CLI
 */
class CLIOutputFormatter {
public:
    enum class Format {
        TEXT,
        JSON,
        CSV,
        HTML,
        MARKDOWN
    };
    
    CLIOutputFormatter(Format format = Format::TEXT);
    ~CLIOutputFormatter() = default;
    
    std::string formatStatus(const std::string& status) const;
    std::string formatConfiguration(const Config& config) const;
    std::string formatStatistics(const std::map<std::string, std::string>& stats) const;
    std::string formatError(const std::string& error) const;
    std::string formatHelp(const std::string& help_text) const;
    
    Format getFormat() const { return format_; }
    void setFormat(Format format) { format_ = format; }
    
private:
    Format format_;
};

// Command classes (forward declarations)
class HelpCommand;
class VersionCommand;
class ConfigCommand;
class PresetCommand;
class RunCommand;
class BatchCommand;
class InteractiveCommand;
class TestCommand;
class BenchmarkCommand;
class ExportCommand;
class ImportCommand;

} // namespace nlm

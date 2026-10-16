#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Global CLI configuration
struct CLIConfig {
    std::string configFile;
    std::string outputFormat;
    std::string logLevel;
    bool verbose;
    bool quiet;
    bool help;
    bool version;
    std::map<std::string, std::string> envOverrides;
};

// Command interface for plugin architecture
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool execute(const std::vector<std::string>& args, const CLIConfig& config) = 0;
    virtual bool validateArguments(const std::vector<std::string>& args) = 0;
    virtual void printHelp() const = 0;
};

// Command result structure
struct CommandResult {
    bool success;
    std::string message;
    std::string output;
    int exitCode;
    double executionTime;
    
    CommandResult(bool s = true, const std::string& msg = "", const std::string& out = "", 
                  int code = 0, double time = 0.0) 
        : success(s), message(msg), output(out), exitCode(code), executionTime(time) {}
};

// Command runner for managing and executing commands
class CommandRunner {
public:
    CommandRunner();
    ~CommandRunner();
    
    // Register a command
    void registerCommand(std::unique_ptr<ICommand> command);
    
    // Execute a command by name
    CommandResult executeCommand(const std::string& commandName, 
                                const std::vector<std::string>& args, 
                                const CLIConfig& config);
    
    // List all registered commands
    std::vector<std::string> getCommandNames() const;
    
    // Get command help
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Check if command exists
    bool hasCommand(const std::string& commandName) const;
    
    // Get all registered command descriptions
    std::map<std::string, std::string> getCommandDescriptions() const;
    
    // Execute interactive mode
    void runInteractive(const CLIConfig& config);
    
    // Generate tab completion suggestions
    std::vector<std::string> generateCompletions(const std::string& partialCommand) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global function to get the command runner instance
CommandRunner& getCommandRunner();

// CLI argument parser
class CLIArgumentParser {
public:
    static CLIConfig parseGlobalArguments(int argc, char** argv);
    static std::vector<std::string> parseCommandArguments(int argc, char** argv, std::string& commandName);
    static void printHelp(const CLIConfig& config, const CommandRunner& runner);
    static void printVersion();
    static bool isHelpArgument(const std::string& arg);
    static bool isVersionArgument(const std::string& arg);
};

// Output formatter for different formats
class OutputFormatter {
public:
    static std::string format(const std::string& content, const std::string& format);
    static std::string formatConfig(const std::shared_ptr<Config>& config);
    static std::string formatResults(const std::vector<CommandResult>& results);
    static std::string formatProgress(const std::string& operation, double progress, const std::string& details = "");
    static std::string formatTable(const std::vector<std::vector<std::string>>& rows, const std::vector<std::string>& headers);
    static std::string formatError(const std::string& error, const std::string& context = "");
    static std::string formatSuccess(const std::string& message, const std::string& details = "");
};

// Progress reporter for long-running operations
class ProgressReporter {
public:
    ProgressReporter(const std::string& operation, int totalSteps);
    ~ProgressReporter();
    
    void update(int currentStep, const std::string& message = "");
    void complete(const std::string& finalMessage = "");
    void error(const std::string& errorMessage);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration manager for CLI-specific configuration
class CLIManager {
public:
    static bool loadConfigFile(const std::string& filepath, std::shared_ptr<Config>& config);
    static bool saveConfigFile(const std::string& filepath, const std::shared_ptr<Config>& config);
    static std::shared_ptr<Config> mergeConfigurations(const std::shared_ptr<Config>& base, 
                                                      const std::map<std::string, std::string>& cliArgs,
                                                      const std::map<std::string, std::string>& envVars);
    static bool validateConfiguration(const std::shared_ptr<Config>& config);
    static std::string configToString(const std::shared_ptr<Config>& config);
    static bool stringToConfig(const std::string& configStr, std::shared_ptr<Config>& config);
};

} // namespace nlm
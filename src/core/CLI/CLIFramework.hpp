// NLM CLI Framework - Advanced command-line interface for NLM
// Provides comprehensive argument parsing, command execution, and help system

#ifndef NLM_CLI_FRAMEWORK_HPP
#define NLM_CLI_FRAMEWORK_HPP

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Command types for different operation modes
enum class CommandType {
    Run,           // Run simulation/experiment
    Configure,     // Load/save configuration
    Debug,         // Debug mode
    Benchmark,     // Performance benchmarking
    Analyze,       // Analyze results
    Export,        // Export data
    Visual,        // Visualization
    Help,          // Show help
    Version,       // Show version info
    List,          // List available options
    Custom         // Custom command
};

// Command execution result structure
struct CommandResult {
    bool success;
    std::string output;
    std::string error;
    std::chrono::milliseconds executionTime;
    std::map<std::string, std::string> metadata;
    
    CommandResult() : success(false), executionTime(0) {}
};

// Command argument definition
struct CommandArg {
    std::string name;
    std::string shortName;
    std::string description;
    std::string type; // string, int, float, bool, file, dir
    std::string defaultValue;
    bool required = false;
    std::vector<std::string> choices;
    std::function<bool(const std::string&)> validator;
    
    CommandArg(const std::string& name, const std::string& desc, 
               const std::string& type = "string", bool req = false)
        : name(name), shortName(""), description(desc), type(type), 
          required(req), validator(nullptr) {}
};

// Command definition
struct Command {
    std::string name;
    std::string description;
    std::string category;
    CommandType type;
    std::vector<CommandArg> arguments;
    std::function<CommandResult(const std::map<std::string, std::string>&)> handler;
    std::vector<std::string> aliases;
    
    Command(const std::string& name, const std::string& desc, 
            const std::string& category = "General",
            CommandType type = CommandType::Custom)
        : name(name), description(desc), category(category), type(type) {}
};

// Advanced argument parser with completion support
class ArgumentParser {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ArgumentParser();
    ~ArgumentParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv, std::string& command, 
               std::map<std::string, std::string>& args,
               std::vector<std::string>& unknownArgs);
    
    // Add command to parser
    void addCommand(const Command& cmd);
    
    // Get command by name
    const Command* getCommand(const std::string& name) const;
    
    // Get all commands
    std::vector<Command> getAllCommands() const;
    
    // Get command suggestions based on partial input
    std::vector<std::string> getCommandSuggestions(const std::string& partial) const;
    
    // Get argument suggestions based on partial input
    std::vector<std::string> getArgumentSuggestions(const std::string& partial) const;
    
    // Validate arguments against command definition
    bool validateArguments(const std::string& command, 
                         const std::map<std::string, std::string>& args) const;
    
    // Convert value to target type
    template<typename T>
    static T convertValue(const std::string& value);
    
    // Print help for specific command
    void printCommandHelp(const std::string& command) const;
    
    // Print general help
    void printHelp() const;
    
    // Print completion suggestions
    void printCompletion(const std::string& partial) const;
};

// Command execution manager
class CommandExecutor {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    CommandExecutor();
    ~CommandExecutor();
    
    // Execute a command with given arguments
    CommandResult execute(const std::string& command, 
                         const std::map<std::string, std::string>& args,
                         const std::shared_ptr<Config>& config = nullptr);
    
    // Register a command handler
    void registerCommand(const Command& cmd);
    
    // Execute command from string
    CommandResult executeFromString(const std::string& commandLine,
                                   const std::shared_ptr<Config>& config = nullptr);
    
    // Get command history
    std::vector<std::string> getCommandHistory() const;
    
    // Clear command history
    void clearCommandHistory();
    
    // Set working directory
    void setWorkingDirectory(const std::string& path);
    
    // Get working directory
    std::string getWorkingDirectory() const;
};

// CLI context manager for maintaining state across commands
class CLIContext {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    CLIContext();
    ~CLIContext();
    
    // Initialize context with configuration
    void initialize(const std::shared_ptr<Config>& config);
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const;
    
    // Set configuration
    void setConfig(const std::shared_ptr<Config>& config);
    
    // Get logger
    std::shared_ptr<Logger> getLogger() const;
    
    // Set logger
    void setLogger(const std::shared_ptr<Logger>& logger);
    
    // Get command executor
    CommandExecutor& getCommandExecutor();
    
    // Get argument parser
    ArgumentParser& getArgumentParser();
    
    // Save and restore context
    void saveContext(const std::string& name);
    bool restoreContext(const std::string& name);
    
    // List saved contexts
    std::vector<std::string> listContexts() const;
    
    // Clean up old contexts
    void cleanupContexts(double maxAgeSeconds = 3600.0);
};

// Command utilities and helper functions
class CommandUtils {
public:
    // Format duration for display
    static std::string formatDuration(const std::chrono::milliseconds& duration);
    
    // Format memory usage for display
    static std::string formatMemory(size_t bytes);
    
    // Parse file size specification (e.g., "1MB", "500KB")
    static size_t parseFileSize(const std::string& spec);
    
    // Validate file exists and is readable
    static bool validateFile(const std::string& path);
    
    // Validate directory exists and is readable
    static bool validateDirectory(const std::string& path);
    
    // Create directory if it doesn't exist
    static bool ensureDirectoryExists(const std::string& path);
    
    // Generate timestamp string
    static std::string getTimestamp(bool includeMilliseconds = false);
    
    // Progress reporting for long-running operations
    class ProgressReporter {
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
    public:
        ProgressReporter(const std::string& description, 
                        size_t totalSteps,
                        size_t updateInterval = 100);
        ~ProgressReporter();
        
        // Update progress
        void update(size_t currentStep, const std::string& status = "");
        
        // Set progress details
        void setDetails(const std::string& details);
        
        // Finish progress reporting
        void finish();
        
        // Check if progress reporter is active
        bool isActive() const;
    };
    
    // Configuration file utilities
    static bool loadConfigFromFile(const std::string& filepath, 
                                  std::shared_ptr<Config>& config);
    static bool saveConfigToFile(const std::string& filepath,
                                const std::shared_ptr<Config>& config);
    static std::string getDefaultConfigPath(const std::string& command = "");
    
    // Result and data export utilities
    static bool exportResults(const std::string& filepath,
                             const CommandResult& result,
                             const std::string& format = "json");
    static bool exportResults(const std::string& filepath,
                             const std::vector<std::map<std::string, std::string>>& data,
                             const std::string& format = "csv");
    
    // Command history management
    static void saveCommandHistory(const std::vector<std::string>& history,
                                  const std::string& filepath);
    static bool loadCommandHistory(std::vector<std::string>& history,
                                  const std::string& filepath);
    
    // Error and message formatting
    static std::string formatError(const std::string& error, 
                                  const std::string& command = "");
    static std::string formatSuccess(const std::string& message,
                                   const std::chrono::milliseconds& duration = std::chrono::milliseconds(0));
    
    // Environment and system utilities
    static std::string getEnvironmentVariable(const std::string& name,
                                             const std::string& defaultValue = "");
    static bool setEnvironmentVariable(const std::string& name,
                                      const std::string& value);
    static std::string getPlatformInfo();
    
    // Interactive utilities (for future enhancement)
    class InteractiveUtils {
    public:
        // Read user input with timeout
        static std::string readInput(const std::string& prompt,
                                    int timeoutSeconds = 30,
                                    const std::vector<std::string>& suggestions = {});
        
        // Display menu and get selection
        static int showMenu(const std::string& title,
                           const std::vector<std::string>& options,
                           int timeoutSeconds = 300);
        
        // Confirm action with user
        static bool confirm(const std::string& question,
                           bool defaultAnswer = false);
    };
};

// Global CLI context instance
extern CLIContext globalCLIContext;

// Convenience functions for common CLI operations
namespace CLI {
    // Initialize CLI with configuration
    void initialize(const std::string& configFile = "");
    
    // Execute command from command line string
    CommandResult execute(const std::string& commandLine);
    
    // Execute command with configuration
    CommandResult execute(const std::string& commandLine,
                         const std::shared_ptr<Config>& config);
    
    // Show help for command
    void help(const std::string& command = "");
    
    // Show version information
    void version();
    
    // Check if CLI is initialized
    bool isInitialized();
    
    // Cleanup
    void cleanup();
}

} // namespace nlm

#endif // NLM_CLI_FRAMEWORK_HPP
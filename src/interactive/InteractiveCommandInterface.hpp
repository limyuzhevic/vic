#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>
#include <optional>
#include <algorithm>
#include <set>
#include <sstream>
#include <iomanip>

namespace nlm {

// Interactive command interface version
constexpr int INTERACTIVE_INTERFACE_VERSION = 1;

// Command categories
enum class CommandCategory {
    System,        // System control commands
    Simulation,    // Simulation control commands
    Visualization, // Visualization control commands
    Data,          // Data management commands
    Configuration,  // Configuration management
    Plugin,        // Plugin management
    Debug,         // Debugging commands
    Export,        // Export commands
    Help,          // Help commands
    General        // General commands
};

// Command priority levels
enum class CommandPriority {
    Low,
    Normal,
    High,
    Critical
};

// Command execution result
struct CommandResult {
    bool success;
    std::string message;
    std::string error;
    std::chrono::milliseconds executionTime;
    std::map<std::string, std::string> metadata;
    
    CommandResult() : success(false), executionTime(0) {}
    CommandResult(bool success, const std::string& message) 
        : success(success), message(message), executionTime(0) {}
    CommandResult(bool success, const std::string& message, const std::string& error)
        : success(success), message(message), error(error), executionTime(0) {}
};

// Command context
struct CommandContext {
    std::string commandId;
    std::string commandName;
    std::vector<std::string> arguments;
    std::string source; // "console", "script", "api", "event"
    std::chrono::system_clock::time_point timestamp;
    std::string user;
    std::map<std::string, std::any> metadata;
    
    CommandContext(const std::string& name) : commandName(name), timestamp(std::chrono::system_clock::now()) {}
};

// Command argument definition
struct CommandArgument {
    std::string name;
    std::string type; // "string", "int", "double", "bool", "file", "directory", "enum"
    std::string description;
    std::string defaultValue;
    std::vector<std::string> enumValues;
    bool required = false;
    bool optional = false;
    std::string helpText;
    
    CommandArgument() : required(false), optional(false) {}
    CommandArgument(const std::string& name, const std::string& type, const std::string& description)
        : name(name), type(type), description(description), required(false) {}
};

// Command definition
struct Command {
    std::string name;
    std::string description;
    std::string usage;
    CommandCategory category;
    CommandPriority priority;
    std::vector<CommandArgument> arguments;
    std::vector<std::string> aliases;
    std::function<bool(const CommandContext&)> validator;
    std::function<CommandResult(const CommandContext&)> executor;
    std::function<void(const CommandResult&)> callback;
    
    // Auto-completion
    std::function<std::vector<std::string>(const std::string&)> completer;
    
    // Help text
    std::string helpText;
    std::string longDescription;
    std::vector<std::string> examples;
    
    Command() : category(CommandCategory::General), priority(CommandPriority::Normal) {}
};

// Command history entry
struct CommandHistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::string command;
    CommandResult result;
    
    CommandHistoryEntry(const std::string& cmd, const CommandResult& result)
        : command(cmd), result(result), timestamp(std::chrono::system_clock::now()) {}
};

// Auto-suggestion entry
struct AutoSuggestion {
    std::string text;
    double score;
    std::string description;
    CommandCategory category;
    
    AutoSuggestion(const std::string& text, double score, const std::string& description,
                   CommandCategory category = CommandCategory::General)
        : text(text), score(score), description(description), category(category) {}
    
    bool operator<(const AutoSuggestion& other) const {
        return score > other.score; // Higher scores first
    }
};

// Interactive command interface
class InteractiveCommandInterface {
private:
    std::map<std::string, Command> commands_;
    std::map<std::string, std::function<CommandResult(const CommandContext&)>> commandExecutors_;
    
    std::mutex mutex_;
    std::condition_variable cv_;
    
    // Command processing
    std::queue<CommandContext> commandQueue_;
    std::vector<CommandHistoryEntry> commandHistory_;
    
    // Session management
    std::string currentSessionId_;
    std::chrono::system_clock::time_point sessionStartTime_;
    std::chrono::system_clock::time_point lastCommandTime_;
    
    // State
    bool isRunning_;
    bool isPaused_;
    std::atomic<bool> stopRequested_;
    
    // Auto-completion
    std::priority_queue<AutoSuggestion> suggestions_;
    std::string currentInput_;
    size_t cursorPosition_;
    
    // Configuration
    std::string prompt_;
    std::string helpHeader_;
    std::string errorPrefix_;
    bool showCommandTimestamps_;
    bool showCommandCounts_;
    size_t maxHistorySize_;
    
    // Callbacks
    std::function<void(const CommandHistoryEntry&)> historyCallback_;
    std::function<void(const std::string&)> commandCallback_;
    std::function<void(const std::string&)> errorCallback_;
    std::function<void(const AutoSuggestion&)> suggestionCallback_;
    
    // Performance monitoring
    std::chrono::nanoseconds totalCommandTime_;
    size_t commandCount_;
    double averageCommandTime_;
    
public:
    InteractiveCommandInterface() : isRunning_(false), isPaused_(false), stopRequested_(false),
                                 cursorPosition_(0), commandCount_(0), totalCommandTime_(0), averageCommandTime_(0.0),
                                 maxHistorySize_(1000) {
        sessionStartTime_ = std::chrono::system_clock::now();
        lastCommandTime_ = sessionStartTime_;
        
        // Default configuration
        prompt_ = ">>> ";
        helpHeader_ = "Available Commands:";
        errorPrefix_ = "Error:";
        showCommandTimestamps_ = true;
        showCommandCounts_ = true;
        
        initializeCommands();
    }
    
    ~InteractiveCommandInterface() {
        stop();
    }
    
    // Command management
    void registerCommand(const Command& command) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        commands_[command.name] = command;
        commandExecutors_[command.name] = command.executor;
        
        for (const auto& alias : command.aliases) {
            commands_[alias] = command;
            commandExecutors_[alias] = command.executor;
        }
    }
    
    bool executeCommand(const std::string& input) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        CommandContext context = parseCommand(input);
        if (!context.commandName.empty()) {
            return processCommand(context);
        }
        
        return false;
    }
    
    std::vector<std::string> completeCommand(const std::string& input) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<std::string> suggestions;
        
        // Find matching commands
        std::string prefix = extractCommandPrefix(input);
        if (prefix.empty()) {
            // Return all command names
            for (const auto& pair : commands_) {
                suggestions.push_back(pair.first);
            }
        } else {
            // Return matching commands
            for (const auto& pair : commands_) {
                if (pair.first.find(prefix) == 0) {
                    suggestions.push_back(pair.first);
                }
            }
        }
        
        // Sort suggestions
        std::sort(suggestions.begin(), suggestions.end());
        
        return suggestions;
    }
    
    std::vector<AutoSuggestion> getAutoSuggestions(const std::string& input) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        currentInput_ = input;
        suggestions_ = std::priority_queue<AutoSuggestion>();
        
        std::string prefix = extractCommandPrefix(input);
        
        // Generate suggestions based on input
        if (!prefix.empty()) {
            generateSuggestions(prefix);
        }
        
        std::vector<AutoSuggestion> suggestionList;
        while (!suggestions_.empty()) {
            suggestionList.push_back(suggestions_.top());
            suggestions_.pop();
        }
        
        return suggestionList;
    }
    
    // Command history
    std::vector<CommandHistoryEntry> getCommandHistory() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return commandHistory_;
    }
    
    std::vector<CommandHistoryEntry> getRecentHistory(size_t count) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        size_t start = std::max(0, static_cast<long>(commandHistory_.size() - count));
        return std::vector<CommandHistoryEntry>(commandHistory_.begin() + start, commandHistory_.end());
    }
    
    void clearHistory() {
        std::lock_guard<std::mutex> lock(mutex_);
        commandHistory_.clear();
    }
    
    // Session management
    void startSession(const std::string& sessionId = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (!sessionId.empty()) {
            currentSessionId_ = sessionId;
        } else {
            currentSessionId_ = "session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        }
        
        sessionStartTime_ = std::chrono::system_clock::now();
        lastCommandTime_ = sessionStartTime_;
        isRunning_ = true;
        isPaused_ = false;
        commandCount_ = 0;
        totalCommandTime_ = std::chrono::nanoseconds(0);
    }
    
    void stopSession() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (isRunning_) {
            stopRequested_ = true;
            isRunning_ = false;
        }
    }
    
    std::string getSessionId() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentSessionId_;
    }
    
    double getSessionUptime() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStartTime_);
        return static_cast<double>(duration.count());
    }
    
    // Performance monitoring
    double getAverageCommandTime() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return commandCount_ > 0 ? averageCommandTime_ : 0.0;
    }
    
    size_t getCommandCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return commandCount_;
    }
    
    double getCommandCountPerMinute() const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - sessionStartTime_);
        
        if (duration.count() == 0) {
            return static_cast<double>(commandCount_);
        }
        
        return static_cast<double>(commandCount_) / duration.count();
    }
    
    // Callbacks
    void setHistoryCallback(std::function<void(const CommandHistoryEntry&)> callback) {
        historyCallback_ = callback;
    }
    
    void setCommandCallback(std::function<void(const std::string&)> callback) {
        commandCallback_ = callback;
    }
    
    void setErrorCallback(std::function<void(const std::string&)> callback) {
        errorCallback_ = callback;
    }
    
    void setSuggestionCallback(std::function<void(const AutoSuggestion&)> callback) {
        suggestionCallback_ = callback;
    }
    
    // Configuration
    void setPrompt(const std::string& prompt) {
        prompt_ = prompt;
    }
    
    void setHelpHeader(const std::string& header) {
        helpHeader_ = header;
    }
    
    void setErrorPrefix(const std::string& prefix) {
        errorPrefix_ = prefix;
    }
    
    void setShowTimestamps(bool show) {
        showCommandTimestamps_ = show;
    }
    
    void setShowCommandCounts(bool show) {
        showCommandCounts_ = show;
    }
    
    void setMaxHistorySize(size_t size) {
        maxHistorySize_ = size;
    }
    
    // Built-in commands
    void registerSystemCommands() {
        // Help command
        Command helpCommand;
        helpCommand.name = "help";
        helpCommand.description = "Show help information for commands";
        helpCommand.usage = "help [command]";
        helpCommand.category = CommandCategory::Help;
        helpCommand.priority = CommandPriority::Low;
        helpCommand.arguments = {};
        helpCommand.aliases = {"?"."h"};
        helpCommand.validator = [](const CommandContext& context) { return true; };
        helpCommand.executor = [this](const CommandContext& context) {
            return executeHelpCommand(context);
        };
        registerCommand(helpCommand);
        
        // Exit command
        Command exitCommand;
        exitCommand.name = "exit";
        exitCommand.description = "Exit the interactive session";
        exitCommand.usage = "exit";
        exitCommand.category = CommandCategory::System;
        exitCommand.priority = CommandPriority::Critical;
        exitCommand.arguments = {};
        exitCommand.aliases = {"quit", "q"};
        exitCommand.validator = [](const CommandContext& context) { return true; };
        exitCommand.executor = [this](const CommandContext& context) {
            return executeExitCommand(context);
        };
        registerCommand(exitCommand);
        
        // Clear command
        Command clearCommand;
        clearCommand.name = "clear";
        clearCommand.description = "Clear the screen";
        clearCommand.usage = "clear";
        clearCommand.category = CommandCategory::System;
        clearCommand.priority = CommandPriority::Low;
        clearCommand.arguments = {};
        clearCommand.aliases = {};
        clearCommand.validator = [](const CommandContext& context) { return true; };
        clearCommand.executor = [this](const CommandContext& context) {
            return executeClearCommand(context);
        };
        registerCommand(clearCommand);
        
        // History command
        Command historyCommand;
        historyCommand.name = "history";
        historyCommand.description = "Show command history";
        historyCommand.usage = "history [count]";
        historyCommand.category = CommandCategory::System;
        historyCommand.priority = CommandPriority::Low;
        historyCommand.arguments = {
            CommandArgument("count", "int", "Number of commands to show", "10")
        };
        historyCommand.aliases = {};
        historyCommand.validator = [this](const CommandContext& context) { return validateHistoryCommand(context); };
        historyCommand.executor = [this](const CommandContext& context) {
            return executeHistoryCommand(context);
        };
        registerCommand(historyCommand);
        
        // Status command
        Command statusCommand;
        statusCommand.name = "status";
        statusCommand.description = "Show system status";
        statusCommand.usage = "status";
        statusCommand.category = CommandCategory::System;
        statusCommand.priority = CommandPriority::Normal;
        statusCommand.arguments = {};
        statusCommand.aliases = {"stat", "sysinfo"};
        statusCommand.validator = [](const CommandContext& context) { return true; };
        statusCommand.executor = [this](const CommandContext& context) {
            return executeStatusCommand(context);
        };
        registerCommand(statusCommand);
        
        // List commands
        Command listCommand;
        listCommand.name = "list";
        listCommand.description = "List all available commands";
        listCommand.usage = "list [category]";
        listCommand.category = CommandCategory::Help;
        listCommand.priority = CommandPriority::Low;
        listCommand.arguments = {
            CommandArgument("category", "string", "Command category (System, Simulation, Visualization, Data, Configuration, Plugin, Debug, Export, Help, General)", "")
        };
        listCommand.aliases = {"ls", "commands"};
        listCommand.validator = [this](const CommandContext& context) { return validateListCommand(context); };
        listCommand.executor = [this](const CommandContext& context) {
            return executeListCommand(context);
        };
        registerCommand(listCommand);
        
        // Config command
        Command configCommand;
        configCommand.name = "config";
        configCommand.description = "Manage configuration";
        configCommand.usage = "config <key> [value]";
        configCommand.category = CommandCategory::Configuration;
        configCommand.priority = CommandPriority::Normal;
        configCommand.arguments = {
            CommandArgument("key", "string", "Configuration key"),
            CommandArgument("value", "string", "Configuration value (optional)")
        };
        configCommand.aliases = {"set", "cfg"};
        configCommand.validator = [this](const CommandContext& context) { return validateConfigCommand(context); };
        configCommand.executor = [this](const CommandContext& context) {
            return executeConfigCommand(context);
        };
        registerCommand(configCommand);
        
        // Export command
        Command exportCommand;
        exportCommand.name = "export";
        exportCommand.description = "Export data to file";
        exportCommand.usage = "export <format> <output>";
        exportCommand.category = CommandCategory::Export;
        exportCommand.priority = CommandPriority::Normal;
        exportCommand.arguments = {
            CommandArgument("format", "string", "Export format (json, csv, xml)"),
            CommandArgument("output", "string", "Output file path")
        };
        exportCommand.aliases = {"save", "dump"};
        exportCommand.validator = [this](const CommandContext& context) { return validateExportCommand(context); };
        exportCommand.executor = [this](const CommandContext& context) {
            return executeExportCommand(context);
        };
        registerCommand(exportCommand);
        
        // Import command
        Command importCommand;
        importCommand.name = "import";
        importCommand.description = "Import data from file";
        importCommand.usage = "import <format> <input>";
        importCommand.category = CommandCategory::Data;
        importCommand.priority = CommandPriority::Normal;
        importCommand.arguments = {
            CommandArgument("format", "string", "Import format (json, csv, xml)"),
            CommandArgument("input", "string", "Input file path")
        };
        importCommand.aliases = {"load", "read"};
        importCommand.validator = [this](const CommandContext& context) { return validateImportCommand(context); };
        importCommand.executor = [this](const CommandContext& context) {
            return executeImportCommand(context);
        };
        registerCommand(importCommand);
    }
    
protected:
    void initializeCommands() {
        registerSystemCommands();
        registerSimulationCommands();
        registerVisualizationCommands();
        registerDataCommands();
        registerPluginCommands();
        registerDebugCommands();
    }
    
    // Command processing
    CommandContext parseCommand(const std::string& input) {
        CommandContext context("unknown");
        
        if (input.empty()) {
            return context;
        }
        
        std::stringstream ss(input);
        std::string token;
        
        // Extract command name
        if (std::getline(ss, token, ' ')) {
            context.commandName = token;
        }
        
        // Extract arguments
        while (std::getline(ss, token, ' ')) {
            if (!token.empty()) {
                context.arguments.push_back(token);
            }
        }
        
        return context;
    }
    
    std::string extractCommandPrefix(const std::string& input) {
        std::string prefix;
        std::istringstream ss(input);
        std::string token;
        
        if (std::getline(ss, token, ' ')) {
            prefix = token;
        }
        
        return prefix;
    }
    
    bool processCommand(const CommandContext& context) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = commands_.find(context.commandName);
        if (it == commands_.end()) {
            if (errorCallback_) {
                errorCallback_("Unknown command: " + context.commandName);
            }
            return false;
        }
        
        // Validate command
        if (it->second.validator && !it->second.validator(context)) {
            if (errorCallback_) {
                errorCallback_("Invalid arguments for command: " + context.commandName);
            }
            return false;
        }
        
        // Execute command
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            CommandResult result = it->second.executor(context);
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            result.executionTime = duration;
            
            // Update statistics
            commandCount_++;
            totalCommandTime_ += duration;
            averageCommandTime_ = static_cast<double>(totalCommandTime_.count()) / commandCount_;
            
            // Add to history
            CommandHistoryEntry historyEntry(context.command, result);
            commandHistory_.push_back(historyEntry);
            
            if (historyCallback_) {
                historyCallback_(historyEntry);
            }
            
            // Handle callbacks
            if (commandCallback_) {
                commandCallback_(context.commandName);
            }
            
            // Print result
            printCommandResult(result, context.commandName);
            
            return result.success;
            
        } catch (const std::exception& e) {
            CommandResult errorResult;
            errorResult.success = false;
            errorResult.message = "Command execution failed";
            errorResult.error = std::string(e.what());
            
            auto endTime = std::chrono::high_resolution_clock::now();
            errorResult.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            if (errorCallback_) {
                errorCallback_("Command error: " + std::string(e.what()));
            }
            
            return false;
        }
    }
    
    void printCommandResult(const CommandResult& result, const std::string& commandName) {
        if (result.success) {
            if (showCommandCounts_) {
                std::cout << "[" << commandCount_ << "] ";
            }
            if (showCommandTimestamps_) {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStartTime_);
                std::cout << "[" << duration.count() << "s] ";
            }
            std::cout << "Command: " << commandName << std::endl;
            if (!result.message.empty()) {
                std::cout << "  Result: " << result.message << std::endl;
            }
            if (!result.error.empty()) {
                std::cout << "  Note: " << result.error << std::endl;
            }
        } else {
            if (showCommandCounts_) {
                std::cout << "[" << commandCount_ << "] ";
            }
            if (showCommandTimestamps_) {
                auto now = std::chrono::system_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStartTime_);
                std::cout << "[" << duration.count() << "s] ";
            }
            std::cout << errorPrefix_ << " Command: " << commandName << std::endl;
            if (!result.error.empty()) {
                std::cout << "  Error: " << result.error << std::endl;
            }
        }
    }
    
    // Auto-completion
    void generateSuggestions(const std::string& prefix) {
        // Generate suggestions based on prefix
        for (const auto& pair : commands_) {
            if (pair.first.find(prefix) == 0) {
                AutoSuggestion suggestion(pair.first, calculateSuggestionScore(pair.first, prefix), pair.second.description, pair.second.category);
                suggestions_.push(suggestion);
                
                if (suggestionCallback_) {
                    suggestionCallback_(suggestion);
                }
            }
        }
    }
    
    double calculateSuggestionScore(const std::string& command, const std::string& prefix) const {
        // Simple scoring based on prefix match length
        if (command.find(prefix) != 0) {
            return 0.0;
        }
        
        return static_cast<double>(prefix.length()) / command.length();
    }
    
    // Built-in command execution
    CommandResult executeHelpCommand(const CommandContext& context) {
        if (context.arguments.empty()) {
            printHelp();
            return CommandResult(true, "Help displayed");
        }
        
        std::string commandName = context.arguments[0];
        auto it = commands_.find(commandName);
        if (it == commands_.end()) {
            return CommandResult(false, "Command not found: " + commandName);
        }
        
        printCommandHelp(it->second);
        return CommandResult(true, "Help for command displayed");
    }
    
    CommandResult executeExitCommand(const CommandContext& context) {
        stopSession();
        return CommandResult(true, "Session ended");
    }
    
    CommandResult executeClearCommand(const CommandContext& context) {
        // Clear screen (platform-dependent)
        std::cout << "\033[2J\033[H"; // Clear screen and reset cursor
        return CommandResult(true, "Screen cleared");
    }
    
    CommandResult executeHistoryCommand(const CommandContext& context) {
        size_t count = 10;
        if (!context.arguments.empty()) {
            try {
                count = std::stoul(context.arguments[0]);
            } catch (const std::exception&) {
                return CommandResult(false, "Invalid count number: " + context.arguments[0]);
            }
        }
        
        std::vector<CommandHistoryEntry> history = getRecentHistory(count);
        printHistory(history);
        return CommandResult(true, "History displayed");
    }
    
    CommandResult executeStatusCommand(const CommandContext& context) {
        printStatus();
        return CommandResult(true, "Status displayed");
    }
    
    CommandResult executeListCommand(const CommandContext& context) {
        if (context.arguments.empty()) {
            printAllCommands();
            return CommandResult(true, "All commands listed");
        }
        
        std::string categoryStr = context.arguments[0];
        CommandCategory category = stringToCategory(categoryStr);
        
        printCommandsByCategory(category);
        return CommandResult(true, "Commands listed by category");
    }
    
    CommandResult executeConfigCommand(const CommandContext& context) {
        if (context.arguments.empty()) {
            printHelpForCommand("config");
            return CommandResult(true, "Configuration command help displayed");
        }
        
        std::string key = context.arguments[0];
        
        if (context.arguments.size() > 1) {
            // Set configuration
            std::string value = context.arguments[1];
            return setConfiguration(key, value);
        } else {
            // Get configuration
            return getConfiguration(key);
        }
    }
    
    CommandResult executeExportCommand(const CommandContext& context) {
        if (context.arguments.size() < 2) {
            return CommandResult(false, "Insufficient arguments for export command");
        }
        
        std::string format = context.arguments[0];
        std::string output = context.arguments[1];
        
        return exportData(format, output);
    }
    
    CommandResult executeImportCommand(const CommandContext& context) {
        if (context.arguments.size() < 2) {
            return CommandResult(false, "Insufficient arguments for import command");
        }
        
        std::string format = context.arguments[0];
        std::string input = context.arguments[1];
        
        return importData(format, input);
    }
    
    // Command validators
    bool validateHistoryCommand(const CommandContext& context) {
        return true;
    }
    
    bool validateListCommand(const CommandContext& context) {
        return true;
    }
    
    bool validateConfigCommand(const CommandContext& context) {
        return context.arguments.size() >= 1 && context.arguments.size() <= 2;
    }
    
    bool validateExportCommand(const CommandContext& context) {
        return context.arguments.size() == 2;
    }
    
    bool validateImportCommand(const CommandContext& context) {
        return context.arguments.size() == 2;
    }
    
    // Command execution helpers
    CommandResult getConfiguration(const std::string& key) {
        // This would get configuration from the plugin manager
        // For now, return a placeholder
        return CommandResult(true, "Configuration: " + key + " = not implemented");
    }
    
    CommandResult setConfiguration(const std::string& key, const std::string& value) {
        // This would set configuration in the plugin manager
        // For now, return a placeholder
        return CommandResult(true, "Configuration set: " + key + " = " + value);
    }
    
    CommandResult exportData(const std::string& format, const std::string& output) {
        // This would export data from the visualization manager
        // For now, return a placeholder
        return CommandResult(true, "Export: " + format + " to " + output);
    }
    
    CommandResult importData(const std::string& format, const std::string& input) {
        // This would import data from the visualization manager
        // For now, return a placeholder
        return CommandResult(true, "Import: " + format + " from " + input);
    }
    
    // Helper methods for built-in commands
    void printHelp() {
        std::cout << "=== NLM Interactive Command Interface ===" << std::endl;
        std::cout << "Version: " << INTERACTIVE_INTERFACE_VERSION << std::endl;
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::General);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::System);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Simulation);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Visualization);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Data);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Configuration);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Plugin);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Debug);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Export);
        std::cout << std::endl;
        printHelpForCategory(CommandCategory::Help);
    }
    
    void printHelpForCommand(const std::string& commandName) {
        auto it = commands_.find(commandName);
        if (it == commands_.end()) {
            std::cout << "Command not found: " << commandName << std::endl;
            return;
        }
        
        const Command& cmd = it->second;
        std::cout << "=== " << cmd.name << " ===" << std::endl;
        std::cout << cmd.description << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << cmd.usage << std::endl;
        std::cout << std::endl;
        if (!cmd.longDescription.empty()) {
            std::cout << cmd.longDescription << std::endl;
        }
        std::cout << std::endl;
        if (!cmd.arguments.empty()) {
            std::cout << "Arguments:" << std::endl;
            for (const auto& arg : cmd.arguments) {
                std::cout << "  " << arg.name << " (" << arg.type << "): " << arg.description << std::endl;
                if (!arg.defaultValue.empty()) {
                    std::cout << "    Default: " << arg.defaultValue << std::endl;
                }
            }
        }
        std::cout << std::endl;
        if (!cmd.examples.empty()) {
            std::cout << "Examples:" << std::endl;
            for (const auto& example : cmd.examples) {
                std::cout << "  " << example << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
    void printCommandHelp(const Command& cmd) {
        std::cout << "=== " << cmd.name << " ===" << std::endl;
        std::cout << cmd.description << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << cmd.usage << std::endl;
        std::cout << std::endl;
    }
    
    void printHistory(const std::vector<CommandHistoryEntry>& history) {
        std::cout << "Command History (most recent first):" << std::endl;
        for (size_t i = 0; i < history.size(); ++i) {
            const CommandHistoryEntry& entry = history[i];
            std::cout << "[" << (i + 1) << "] ";
            if (showCommandTimestamps_) {
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(entry.timestamp - sessionStartTime_);
                std::cout << "[" << duration.count() << "s] ";
            }
            std::cout << entry.command;
            if (!entry.result.message.empty()) {
                std::cout << " -> " << entry.result.message;
            }
            std::cout << std::endl;
        }
    }
    
    void printStatus() {
        std::cout << "=== NLM System Status ===" << std::endl;
        std::cout << "Session: " << currentSessionId_ << std::endl;
        std::cout << "Uptime: " << getSessionUptime() << " seconds" << std::endl;
        std::cout << "Commands executed: " << commandCount_ << std::endl;
        std::cout << "Average command time: " << getAverageCommandTime() * 1000 << " ms" << std::endl;
        std::cout << "Command count per minute: " << getCommandCountPerMinute() << std::endl;
        std::cout << "Visualization count: " << getActiveVisualizationCount() << std::endl;
        std::cout << "Memory usage: " << getSystemMemoryUsage() / 1024 / 1024 << " MB" << std::endl;
        std::cout << std::endl;
        std::cout << "Loaded plugins:" << std::endl;
        for (const auto& name : getLoadedPlugins()) {
            std::cout << "  - " << name << std::endl;
        }
        std::cout << std::endl;
    }
    
    void printAllCommands() {
        std::cout << helpHeader_ << std::endl;
        std::cout << std::endl;
        
        for (const auto& category : {
            CommandCategory::General, CommandCategory::System, CommandCategory::Simulation,
            CommandCategory::Visualization, CommandCategory::Data, CommandCategory::Configuration,
            CommandCategory::Plugin, CommandCategory::Debug, CommandCategory::Export, CommandCategory::Help
        }) {
            printCommandsByCategory(category);
            std::cout << std::endl;
        }
    }
    
    void printCommandsByCategory(CommandCategory category) {
        std::cout << categoryToString(category) << ":" << std::endl;
        
        for (const auto& pair : commands_) {
            if (pair.second.category == category) {
                std::cout << "  " << pair.second.name;
                if (!pair.second.aliases.empty()) {
                    std::cout << " (also: ";
                    for (size_t i = 0; i < pair.second.aliases.size(); ++i) {
                        if (i > 0) std::cout << ", ";
                        std::cout << pair.second.aliases[i];
                    }
                    std::cout << ")";
                }
                std::cout << " - " << pair.second.description << std::endl;
            }
        }
    }
    
    std::string categoryToString(CommandCategory category) {
        switch (category) {
            case CommandCategory::System: return "System Commands";
            case CommandCategory::Simulation: return "Simulation Commands";
            case CommandCategory::Visualization: return "Visualization Commands";
            case CommandCategory::Data: return "Data Commands";
            case CommandCategory::Configuration: return "Configuration Commands";
            case CommandCategory::Plugin: return "Plugin Commands";
            case CommandCategory::Debug: return "Debug Commands";
            case CommandCategory::Export: return "Export Commands";
            case CommandCategory::Help: return "Help Commands";
            case CommandCategory::General: return "General Commands";
            default: return "Unknown Category";
        }
    }
    
    CommandCategory stringToCategory(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower.find("system") != std::string::npos) return CommandCategory::System;
        if (lower.find("simulation") != std::string::npos) return CommandCategory::Simulation;
        if (lower.find("visualization") != std::string::npos) return CommandCategory::Visualization;
        if (lower.find("data") != std::string::npos) return CommandCategory::Data;
        if (lower.find("config") != std::string::npos || lower.find("configuration") != std::string::npos) return CommandCategory::Configuration;
        if (lower.find("plugin") != std::string::npos) return CommandCategory::Plugin;
        if (lower.find("debug") != std::string::npos) return CommandCategory::Debug;
        if (lower.find("export") != std::string::npos) return CommandCategory::Export;
        if (lower.find("help") != std::string::npos) return CommandCategory::Help;
        
        return CommandCategory::General;
    }
    
    // Virtual methods for additional command registration
    virtual void registerSimulationCommands() {}
    virtual void registerVisualizationCommands() {}
    virtual void registerDataCommands() {}
    virtual void registerPluginCommands() {}
    virtual void registerDebugCommands() {}
    
    // Helper methods for getting system information
    size_t getActiveVisualizationCount() const {
        // This would get visualization count from the visualization manager
        // For now, return 0
        return 0;
    }
    
    std::vector<std::string> getLoadedPlugins() const {
        // This would get plugin list from the plugin manager
        // For now, return empty list
        return {};
    }
    
    size_t getSystemMemoryUsage() const {
        // This would get system memory usage from the system
        // For now, return 0
        return 0;
    }
}; // namespace nlm

// Global interactive command interface instance
InteractiveCommandInterface& getGlobalInteractiveCommandInterface() {
    static InteractiveCommandInterface interface;
    return interface;
}

// Convenience functions for command execution
inline bool executeCommand(const std::string& input) {
    return getGlobalInteractiveCommandInterface().executeCommand(input);
}

inline std::vector<std::string> completeCommand(const std::string& input) {
    return getGlobalInteractiveCommandInterface().completeCommand(input);
}

inline void startInteractiveSession(const std::string& sessionId) {
    getGlobalInteractiveCommandInterface().startSession(sessionId);
}

inline void stopInteractiveSession() {
    getGlobalInteractiveCommandInterface().stopSession();
}

inline std::string getCurrentSessionId() {
    return getGlobalInteractiveCommandInterface().getSessionId();
}
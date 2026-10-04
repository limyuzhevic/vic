#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>
#include <variant>
#include <any>

namespace nlm {

// Basic CLI types for neural simulation interface
using String = std::string;
using StringList = std::vector<std::string>;
using StringMap = std::map<std::string, std::string>;
using StringPair = std::pair<std::string, std::string>;
using StringVector = std::vector<std::string>;
using OptionArgs = std::vector<std::string>;
using ParameterArgs = std::vector<std::string>;

// Progress tracking
struct ProgressInfo {
    std::string message;
    double percentage;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point currentTime;
    bool active;
    
    ProgressInfo(const std::string& message = "", double percentage = 0.0)
        : message(message), percentage(percentage), active(false) {}
    
    double getElapsedTime() const {
        if (!active) return 0.0;
        auto duration = std::chrono::duration<double>(currentTime - startTime).count();
        return duration;
    }
    
    void update() { currentTime = std::chrono::system_clock::now(); }
};

// Colored output for terminals
enum class Color {
    Reset = 0,
    Red = 1,
    Green = 2,
    Yellow = 3,
    Blue = 4,
    Magenta = 5,
    Cyan = 6,
    White = 7,
    BrightRed = 8,
    BrightGreen = 9,
    BrightYellow = 10,
    BrightBlue = 11,
    BrightMagenta = 12,
    BrightCyan = 13,
    BrightWhite = 14
};

// Output formatters
class CLIFormatters {
public:
    static std::string formatValue(const std::any& value);
    static std::string formatDuration(double seconds);
    static std::string formatFileSize(size_t bytes);
    static std::string formatPercentage(double value);
    static std::string formatMemory(size_t bytes);
    static std::string formatNumber(int64_t number);
    static std::string formatList(const StringVector& items);
    static std::string formatKeyValue(const StringMap& kv);
    
    static void printColored(const std::string& text, Color color = Color::Reset);
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printDebug(const std::string& message);
    static void printProgress(const std::string& message, double progress = -1.0);
    static void printSpinner(const std::string& message, std::chrono::milliseconds delay = std::chrono::milliseconds(100));
    
    // Progress bar
    static void printProgressBar(double progress, const std::string& message = "", 
                                 size_t width = 50, const std::string& fillChar = "=",
                                 const std::string& emptyChar = " ",
                                 const std::string& leftChar = "[",
                                 const std::string& rightChar = "]");
    
    // Status indicators
    static std::string getStatusSymbol(bool success, bool warning = false);
    static std::string getStatusColor(bool success, bool warning = false);
};

// Command execution context
struct CommandContext {
    std::string commandName;
    StringList args;
    std::map<std::string, std::any> options;
    std::map<std::string, std::any> parameters;
    std::shared_ptr<ProgressInfo> progress;
    bool verbose;
    bool quiet;
    std::string outputFormat;
    
    CommandContext(const std::string& commandName, const StringList& args)
        : commandName(commandName), args(args), verbose(false), quiet(false),
          outputFormat("text"), progress(nullptr) {}
    
    // Helper methods for accessing context
    template<typename T>
    T getOption(const std::string& key, const T& defaultValue = T()) const {
        auto it = options.find(key);
        if (it != options.end()) {
            return std::any_cast<T>(it->second);
        }
        return defaultValue;
    }
    
    template<typename T>
    void setOption(const std::string& key, const T& value) {
        options[key] = value;
    }
    
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return std::any_cast<T>(it->second);
        }
        return defaultValue;
    }
    
    template<typename T>
    void setParameter(const std::string& key, const T& value) {
        parameters[key] = value;
    }
    
    std::string getStringOption(const std::string& key, const std::string& defaultValue = "") const {
        return getOption<std::string>(key, defaultValue);
    }
    
    int getIntOption(const std::string& key, int defaultValue = 0) const {
        return getOption<int>(key, defaultValue);
    }
    
    double getDoubleOption(const std::string& key, double defaultValue = 0.0) const {
        return getOption<double>(key, defaultValue);
    }
    
    bool getBoolOption(const std::string& key, bool defaultValue = false) const {
        return getOption<bool>(key, defaultValue);
    }
};

// Command result types
enum class CommandResult {
    Success,
    Warning,
    Error,
    Cancelled,
    Skipped
};

struct CommandOutput {
    CommandResult result;
    std::string message;
    std::map<std::string, std::any> data;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    CommandOutput(CommandResult result, const std::string& message = "")
        : result(result), message(message) {}
    
    void addWarning(const std::string& warning) { warnings.push_back(warning); }
    void addError(const std::string& error) { errors.push_back(error); }
    
    bool hasWarnings() const { return !warnings.empty(); }
    bool hasErrors() const { return !errors.empty(); }
    bool isSuccess() const { return result == CommandResult::Success; }
};

// Progress reporter for long-running operations
class ProgressReporter {
public:
    ProgressReporter();
    ~ProgressReporter();
    
    // Start a progress operation
    void startProgress(const std::string& message, double maxValue = 100.0);
    
    // Update progress
    void updateProgress(double value, const std::string& message = "");
    
    // Complete progress
    void completeProgress(const std::string& message = "");
    
    // Check if progress is active
    bool isActive() const { return progressInfo && progressInfo->active; }
    
    // Get current progress
    double getProgress() const { return progressInfo ? progressInfo->percentage : 0.0; }
    std::string getMessage() const { return progressInfo ? progressInfo->message : ""; }
    
    // Disable output (for quiet mode)
    void setQuiet(bool quiet) { this->quiet = quiet; }
    
private:
    std::unique_ptr<ProgressInfo> progressInfo;
    bool quiet;
    std::mutex mutex;
    
    // Output methods
    void printProgress() const;
    void printProgressBar() const;
};

// Error handling and recovery
class CLIErrorHandler {
public:
    CLIErrorHandler();
    ~CLIErrorHandler();
    
    // Add error
    void addError(const std::string& error, bool fatal = false);
    void addWarning(const std::string& warning);
    void addInfo(const std::string& info);
    
    // Get errors and warnings
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    const std::vector<std::string>& getInfos() const { return infos; }
    
    // Check status
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    bool isSuccess() const { return !hasErrors(); }
    bool hasFatalErrors() const { return fatalErrors; }
    
    // Clear all messages
    void clear();
    
    // Get formatted error report
    std::string getErrorReport() const;
    std::string getWarningReport() const;
    std::string getInfoReport() const;
    
    // Exit with error code
    int exitWithError() const;
    
    // Set error handling behavior
    void setExitOnError(bool exit) { exitOnError = exit; }
    void setVerboseError(bool verbose) { verboseError = verbose; }
    
private:
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> infos;
    bool fatalErrors;
    bool exitOnError;
    bool verboseError;
    
    // Output methods
    void printError(const std::string& error, bool fatal = false) const;
    void printWarning(const std::string& warning) const;
    void printInfo(const std::string& info) const;
};

// Command history for CLI recall
class CommandHistory {
public:
    CommandHistory();
    ~CommandHistory();
    
    // Add command to history
    void addCommand(const std::string& command, const std::string& output = "");
    
    // Get command from history
    std::string getCommand(size_t index) const;
    
    // Search history
    std::vector<size_t> search(const std::string& query) const;
    
    // Clear history
    void clear() { history.clear(); outputHistory.clear(); }
    
    // Get all commands
    const std::vector<std::string>& getCommands() const { return history; }
    
    // Get history size
    size_t size() const { return history.size(); }
    
    // Auto-complete for history
    std::vector<std::string> autoComplete(const std::string& partial) const;
    
private:
    std::vector<std::string> history;
    std::map<size_t, std::string> outputHistory;
    mutable std::mutex mutex;
    
    // Add command with timestamp
    void addCommandInternal(const std::string& command, const std::string& output);
};

// Shell completion support
class ShellCompletion {
public:
    ShellCompletion();
    ~ShellCompletion();
    
    // Generate completion script for shell
    static std::string generateBashCompletion(const std::string& programName);
    static std::string generateZshCompletion(const std::string& programName);
    static std::string generateFishCompletion(const std::string& programName);
    
    // Get available commands for completion
    static std::vector<std::string> getCommandsForCompletion(const std::string& partial);
    
    // Get options for a command
    static std::vector<std::string> getOptionsForCommand(const std::string& command, const std::string& partial);
    
    // Get parameters for a command
    static std::vector<std::string> getParametersForCommand(const std::string& command, const std::string& partial);
    
    // Complete a command line
    static std::string completeCommandLine(const std::string& commandLine);
    
private:
    static std::vector<std::string> splitCommandLine(const std::string& commandLine);
    static std::string escapeForShell(const std::string& text);
};

} // namespace nlm

// CLI - Command Line Interface Utilities

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace cli {

// Command line argument parser
class CLIParser {
public:
    struct Argument {
        std::string name;
        char shortOpt;
        std::string longOpt;
        std::string description;
        bool required;
        std::string defaultValue;
        std::function<void(const std::string&)> callback;
        
        Argument(const std::string& name, char shortOpt, const std::string& longOpt,
                 const std::string& description, bool required = false,
                 const std::string& defaultValue = "")
            : name(name), shortOpt(shortOpt), longOpt(longOpt),
              description(description), required(required),
              defaultValue(defaultValue) {}
    };
    
    CLIParser(const std::string& programName, const std::string& description)
        : programName(programName), description(description) {}
    
    void addArgument(const Argument& arg);
    bool parse(int argc, char* argv[]);
    
    // Get argument values
    std::string getValue(const std::string& name) const;
    bool hasValue(const std::string& name) const;
    
    // Help and usage
    void printHelp() const;
    void printVersion() const;
    
    // Configuration file handling
    bool loadConfigFromFile(const std::string& filename);
    void saveConfigToFile(const std::string& filename) const;
    
    // Configuration management
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    const std::map<std::string, std::string>& getConfig() const { return config; }
    
private:
    std::string programName;
    std::string description;
    std::vector<Argument> arguments;
    std::map<std::string, std::string> config;
    std::map<std::string, std::string> parsedValues;
    
    void parseArgument(const std::string& arg);
    bool validateRequiredArgs() const;
};

// Unified logging system
class UnifiedLogger {
public:
    enum class Level {
        OFF = 0,
        FATAL = 1,
        ERROR = 2,
        WARN = 3,
        INFO = 4,
        DEBUG = 5,
        TRACE = 6
    };
    
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        Level level;
        std::string module;
        std::string message;
        std::string file;
        int line;
        std::map<std::string, std::string> metadata;
    };
    
    UnifiedLogger();
    ~UnifiedLogger();
    
    static UnifiedLogger& getInstance();
    
    void setLevel(Level level);
    Level getLevel() const { return currentLevel; }
    
    void log(Level level, const std::string& module, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::map<std::string, std::string>& metadata = {});
    
    // Convenience methods
    void fatal(const std::string& module, const std::string& message);
    void error(const std::string& module, const std::string& message);
    void warn(const std::string& module, const std::string& message);
    void info(const std::string& module, const std::string& message);
    void debug(const std::string& module, const std::string& message);
    void trace(const std::string& module, const std::string& message);
    
    // Log formatters
    std::string formatLogEntry(const LogEntry& entry) const;
    std::string formatColored(const LogEntry& entry) const;
    
    // Log sinks
    void addConsoleSink();
    void addFileSink(const std::string& filename);
    void addJsonSink(const std::string& filename);
    
    // Log queries
    std::vector<LogEntry> getRecentLogs(size_t limit = 100) const;
    std::vector<LogEntry> searchLogs(const std::string& pattern, Level level = Level::OFF) const;
    
    // Log rotation
    void setLogRotation(size_t maxSize, size_t maxFiles);
    
    // Performance logging
    void logPerformance(const std::string& operation, std::chrono::duration<double> duration,
                       const std::map<std::string, std::string>& metadata = {});
    
private:
    Level currentLevel;
    std::vector<LogEntry> logBuffer;
    std::vector<void*> sinks; // Will be replaced with proper sink classes
    size_t maxLogSize;
    size_t maxLogFiles;
    
    void rotateLogs();
};

// Configuration manager
class ConfigManager {
public:
    struct ConfigValue {
        std::string key;
        std::string value;
        std::string type; // "string", "int", "float", "bool"
        std::string description;
        bool required;
        std::string defaultValue;
        std::vector<std::string> allowedValues;
    };
    
    ConfigManager();
    ~ConfigManager();
    
    // Register configuration options
    void registerOption(const ConfigValue& option);
    void registerOption(const std::string& key, const std::string& description,
                       const std::string& type = "string",
                       bool required = false, const std::string& defaultValue = "");
    
    // Get configuration values
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Set configuration values
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setFloat(const std::string& key, float value);
    void setBool(const std::string& key, bool value);
    
    // Configuration file I/O
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    bool loadFromCommandLine(int argc, char* argv[]);
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration export/import
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    std::string exportToINI() const;
    
    bool importFromJSON(const std::string& json);
    bool importFromYAML(const std::string& yaml);
    bool importFromINI(const std::string& ini);
    
    // Get all options
    std::vector<ConfigValue> getAllOptions() const;
    ConfigValue getOption(const std::string& key) const;
    
    // Configuration templates
    void loadTemplate(const std::string& name);
    void saveTemplate(const std::string& name);
    std::vector<std::string> getAvailableTemplates() const;
    
private:
    std::map<std::string, ConfigValue> options;
    std::map<std::string, std::string> values;
    std::vector<std::string> validationErrors;
    
    void validateOption(const ConfigValue& option, const std::string& value) const;
};

// Command execution history
class CommandHistory {
public:
    struct CommandEntry {
        std::chrono::system_clock::time_point timestamp;
        std::string command;
        std::string output;
        bool success;
        double executionTime;
        std::map<std::string, std::string> metadata;
    };
    
    CommandHistory();
    ~CommandHistory();
    
    void addCommand(const std::string& command, bool success, double executionTime,
                   const std::string& output = "", const std::map<std::string, std::string>& metadata = {});
    
    std::vector<CommandEntry> getRecentCommands(size_t limit = 100) const;
    std::vector<CommandEntry> searchCommands(const std::string& pattern) const;
    
    void saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    
    void clear();
    size_t size() const { return commands.size(); }
    
private:
    std::vector<CommandEntry> commands;
    size_t maxHistorySize;
};

// Utility functions
namespace utils {
    std::string trim(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    
    // String formatting
    std::string format(const std::string& format, ...);
    
    // Time utilities
    std::string getCurrentTimestamp();
    std::chrono::duration<double> getElapsedTime(const std::chrono::system_clock::time_point& start);
    
    // File utilities
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    std::string getFileExtension(const std::string& filename);
    std::string getBasename(const std::string& path);
    std::string getDirectory(const std::string& path);
    
    // Process utilities
    void setProcessTitle(const std::string& title);
    void setProcessPriority(const std::string& priority); // low, normal, high, realtime
    
    // System utilities
    std::string getSystemInfo();
    std::string getCompilerInfo();
    std::string getBuildInfo();
}

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_H

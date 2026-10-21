// NLM CLI Framework
// Common CLI utilities for NLM executables

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <functional>
#include <sstream>

namespace nlm {

// Custom exception for CLI errors
class CLIError : public std::runtime_error {
public:
    CLIError(const std::string& message) : std::runtime_error(message) {}
};

// Command line argument parsing
class ArgumentParser {
public:
    struct ArgumentInfo {
        std::string name;
        char shortOpt;
        std::string longOpt;
        std::string description;
        std::string defaultValue;
        bool hasValue;
        std::string value;
        bool isSet;
        std::function<void(const std::string&)> validator;
    };

    ArgumentParser(const std::string& programName, const std::string& version,
                   const std::string& description);
    ~ArgumentParser() = default;

    // Parse arguments
    void parse(int argc, char* argv[]);

    // Add argument definitions
    void addArgument(char shortOpt, const std::string& longOpt,
                     const std::string& description,
                     const std::string& defaultValue = "",
                     bool hasValue = false,
                     const std::string& validatorName = "");

    // Get value
    std::string getValue(const std::string& longOpt) const;
    int getInt(const std::string& longOpt, int default = 0) const;
    double getDouble(const std::string& longOpt, double default = 0.0) const;
    bool getBool(const std::string& longOpt, bool default = false) const;

    // Check if argument was provided
    bool hasArgument(const std::string& longOpt) const;

    // Help and version
    void printHelp() const;
    void printVersion() const;

    // Get program metadata
    const std::string& getProgramName() const { return programName_; }
    const std::string& getVersion() const { return version_; }
    const std::string& getDescription() const { return description_; }

private:
    std::string programName_;
    std::string version_;
    std::string description_;
    std::vector<ArgumentInfo> arguments_;
    std::unordered_map<std::string, ArgumentInfo*> argumentMap_;
    std::string unknownArgValue;

    void setupBuiltInArguments();
    void processArguments(int argc, char* argv[]);
    void validateArguments() const;
    static std::string trim(const std::string& str);
    static bool isFlag(const std::string& arg);
};

// Logger controller for CLI
class LoggerController {
public:
    LoggerController();
    ~LoggerController() = default;

    // Configure logging based on CLI arguments
    void configure(const std::string& logLevel, const std::string& logFile = "",
                   bool enableColors = true);

    // Get current log level
    std::string getLogLevel() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration helper
class CLIConfigHelper {
public:
    CLIConfigHelper();
    ~CLIConfigHelper() = default;

    // Load configuration with CLI precedence over file config
    bool loadConfig(std::shared_ptr<Config> config, int argc, char* argv[]);

    // Validate critical configuration
    bool validateConfig(const Config& config) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Common CLI operations
namespace CLI {
    // Common help text templates
    const std::string BASIC_USAGE = "usage: {} [OPTIONS] [CONFIG_FILE]";
    const std::string HELP_HEADER = "Neural Learning Machine (NLM)";
    const std::string VERSION = "0.1.0";
    const std::string DESCRIPTION = "Experimental artificial developmental brain simulation.";

    // Common argument names
    const std::string ARG_HELP = "help";
    const std::string ARG_VERSION = "version";
    const std::string ARG_LOG_LEVEL = "log-level";
    const std::string ARG_LOG_FILE = "log-file";
    const std::string ARG_CONFIG = "config";

    // Log level constants
    const std::string LOG_LEVEL_DEBUG = "debug";
    const std::string LOG_LEVEL_INFO = "info";
    const std::string LOG_LEVEL_WARNING = "warning";
    const std::string LOG_LEVEL_ERROR = "error";
    const std::string LOG_LEVEL_CRITICAL = "critical";

    // Validation functions
    void validatePositiveInt(const std::string& value);
    void validatePositiveDouble(const std::string& value);
    void validateFilePath(const std::string& value);
    void validateLogLevel(const std::string& value);
}

} // namespace nlm

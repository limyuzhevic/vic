// CLI namespace - Unified command line interface for NLM demos
#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>

namespace nlm {

class CLI {
public:
    CLI(const std::string& version = "0.1.0");
    ~CLI() = default;

    // Parse command line arguments
    bool parse(int argc, char** argv, std::shared_ptr<Config> config);

    // Print help information
    void printHelp() const;

    // Print version information
    void printVersion() const;

    // Set custom callback for unknown arguments
    void setUnknownArgHandler(std::function<bool(const std::string&)> handler);

    // Set custom callback for unknown options
    void setUnknownOptionHandler(std::function<bool(const std::string&)> handler);

    // Get last error message
    const std::string& getError() const { return errorMsg; }

    // Check if parsing failed
    bool hasError() const { return !errorMsg.empty(); }

    // Progress reporting
    void setProgressCallback(std::function<void(double, const std::string&)> callback);
    void reportProgress(double progress, const std::string& message = "") const;

    // Standard option handlers
    static bool helpHandler(const std::string& option);
    static bool versionHandler(const std::string& option, const std::string& version);
    static bool configHandler(const std::string& option, std::shared_ptr<Config> config);
    static bool errorHandler(const std::string& message);

private:
    // Option handlers registry
    std::unordered_map<std::string, std::function<bool(const std::string&)>> optionHandlers;

    std::string version;
    std::string errorMsg;
    std::function<void(double, const std::string&)> progressCallback;

    // Standard short options
    static const std::unordered_map<char, std::string> standardShortOptions;

    // Standard long options
    static const std::unordered_map<std::string, std::string> standardLongOptions;

    // Parse individual argument
    bool parseArg(const std::string& arg, std::shared_ptr<Config> config);

    // Parse --key=value format
    bool parseEqualsFormat(const std::string& arg, std::shared_ptr<Config> config);

    // Parse -key value format
    bool parseShortFormat(const std::string& arg, const std::string& value, std::shared_ptr<Config> config);

    // Validate and apply option
    bool applyOption(const std::string& option, const std::string& value, std::shared_ptr<Config> config);

    // Common option handlers
    static bool handleHelp(const std::string& option);
    static bool handleVersion(const std::string& option, const std::string& version);
    static bool handleConfig(const std::string& option, std::shared_ptr<Config> config);
    static bool handleError(const std::string& message);

    // Progress reporting defaults
    void defaultProgressCallback(double progress, const std::string& message) const;

    // Error handling
    void setError(const std::string& message);
};

// CLI builder for creating custom demo interfaces
class DemoCLI {
public:
    DemoCLI(const std::string& programName, const std::string& version);
    ~DemoCLI() = default;

    // Add a parameter to the CLI
    DemoCLI& addParameter(const std::string& name, 
                         const ConfigValue& defaultValue,
                         const std::string& description,
                         ConfigSource source = ConfigSource::Default);

    // Add a parameter with type and constraints
    template<typename T>
    DemoCLI& addParameter(const std::string& name,
                         T defaultValue,
                         const std::string& description,
                         std::function<bool(const T&)> validator = nullptr,
                         ConfigSource source = ConfigSource::Default);

    // Set progress reporting interval
    DemoCLI& setProgressInterval(double interval);

    // Set minimum progress for status updates
    DemoCLI& setStatusProgress(double progress);

    // Parse arguments and configure demo
    bool parse(int argc, char** argv, std::shared_ptr<Config> config);

    // Print help for this demo
    void printDemoHelp() const;

    // Get the program name
    const std::string& getProgramName() const { return programName; }

    // Get version
    const std::string& getVersion() const { return version; }

private:
    std::string programName;
    std::string version;
    std::vector<ConfigEntry> parameters;
    double progressInterval;
    double statusProgress;

    // Internal parsing logic
    bool parseArgs(int argc, char** argv, std::shared_ptr<Config> config);
    bool handleHelp(const std::string& option);
    bool handleVersion(const std::string& option);
    bool handleConfig(const std::string& option, std::shared_ptr<Config> config);

    // Print formatted help
    void printFormattedHelp() const;

    // Print parameter details
    void printParameter(const ConfigEntry& param) const;
};

// Error handling utilities
class CLIErrors {
public:
    static void logError(const std::string& context, const std::string& message);
    static void logWarning(const std::string& context, const std::string& message);
    static void logInfo(const std::string& context, const std::string& message);
    static std::string formatError(const std::string& context, const std::string& message);

    // Common error types
    static const std::string INVALID_ARGUMENT;
    static const std::string MISSING_VALUE;
    static const std::string INVALID_VALUE;
    static const std::string FILE_NOT_FOUND;
    static const std::string CONFIG_ERROR;
};

// Progress reporting utilities
class ProgressReporter {
public:
    ProgressReporter();
    ~ProgressReporter() = default;

    // Set progress callback
    void setCallback(std::function<void(double, const std::string&)> callback);

    // Report progress with optional message
    void report(double progress, const std::string& message = "");

    // Create a scoped progress reporter
    class ScopedReporter {
    public:
        ScopedReporter(ProgressReporter& reporter, const std::string& id);
        ~ScopedReporter();
        void update(double progress, const std::string& message);
    } createScopedReporter(const std::string& id);

    // Reset reporter
    void reset();

    // Get current progress
    double getProgress() const { return currentProgress; }

    // Check if progress reporting is enabled
    bool isEnabled() const { return callback; }

private:
    std::function<void(double, const std::string&)> callback;
    double currentProgress;
    std::unordered_map<std::string, double> scopedProgress;
};

// Configuration utilities
class CLIConfig {
public:
    // Merge config from file and CLI with proper priority
    static void mergeConfig(std::shared_ptr<Config> config,
                           const std::string& configFile,
                           int argc, char** argv);

    // Load config from file if it exists
    static bool loadConfigFile(std::shared_ptr<Config> config, const std::string& filepath);

    // Save config to file
    static bool saveConfig(std::shared_ptr<Config> config, const std::string& filepath);

    // Validate config values
    static bool validateConfig(const Config& config);

    // Get default config path
    static std::string getDefaultConfigPath();

    // Expand user home directory in path
    static std::string expandPath(const std::string& path);
};

} // namespace nlm

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Command line option definition
struct CommandLineOption {
    std::string longName;
    std::string shortName;
    std::string description;
    std::string type;
    std::string defaultValue;
    std::string envVar;
    std::function<bool(const std::string&)> validator;
    bool required = false;
    bool hasValue = false;
    std::string category; // Group options by category
};

// Parsed command line arguments
struct ParsedArgs {
    std::map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    std::string configFile;
    std::string outputFile;
    std::string logFile;
    std::string logLevel;
    bool help = false;
    bool version = false;
    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    bool profile = false;
    bool benchmark = false;
    bool validation = false;
    bool list = false;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool showTiming = false;
    bool jsonOutput = false;
    bool csvOutput = false;
    bool xmlOutput = false;
    bool textOutput = true;
    std::string outputFormat;
    std::string metricsFile;
    bool enableMetrics = false;
};

// Command line argument parser with comprehensive options
class CommandLineParser {
public:
    CommandLineParser();
    ~CommandLineParser() = default;
    
    // Add a command line option
    void addOption(const std::string& longName, const std::string& shortName,
                  const std::string& description, const std::string& type = "string",
                  const std::string& defaultValue = "", 
                  const std::string& envVar = "",
                  const std::function<bool(const std::string&)>& validator = nullptr,
                  bool required = false, const std::string& category = "general");
    
    // Get all options
    const std::vector<CommandLineOption>& getOptions() const { return options_; }
    
    // Parse command line arguments
    bool parse(int argc, char** argv, ParsedArgs& args);
    
    // Generate command line help
    std::string generateHelp() const;
    
    // Generate help for a specific category
    std::string generateCategoryHelp(const std::string& category) const;
    
    // Check if help is requested
    static bool isHelpRequested(int argc, char** argv);
    
    // Check if version is requested
    static bool isVersionRequested(int argc, char** argv);
    
    // Get all option categories
    std::vector<std::string> getCategories() const;
    
    // Load options from configuration file
    bool loadOptionsFromConfig(const std::string& configFile, ParsedArgs& args);
    
    // Merge multiple configuration files
    bool mergeConfigs(const std::vector<std::string>& configFiles, ParsedArgs& args);
    
    // Validate parsed arguments
    bool validate(const ParsedArgs& args, std::vector<std::string>& errors) const;
    
    // Generate environment variable documentation
    std::string generateEnvHelp() const;
    
    // Complete command line (for shell completion)
    std::string generateCompletion() const;

private:
    std::vector<CommandLineOption> options_;
    std::map<std::string, std::vector<CommandLineOption>> categories_;
    
    // Helper functions
    static bool parseOption(const std::string& arg, std::string& currentKey,
                           std::map<std::string, std::string>& options,
                           const std::vector<CommandLineOption>& validOptions);
    
    static bool resolveOptionValue(const std::string& arg, std::string& currentKey,
                                  const std::vector<CommandLineOption>& validOptions);
    
    static bool isOption(const std::string& arg);
    
    static bool isShortOption(const std::string& arg);
    
    static bool isLongOption(const std::string& arg);
    
    static std::string trim(const std::string& str);
    
    static std::string toLower(const std::string& str);
    
    static std::string toUpper(const std::string& str);
};

// Comprehensive brain configuration options
class BrainConfigOptions {
public:
    static void addBrainOptions(CommandLineParser& parser);
    
    static void addNeuralNetworkOptions(CommandLineParser& parser);
    
    static void addSimulationOptions(CommandLineParser& parser);
    
    static void addMemoryOptions(CommandLineParser& parser);
    
    static void addOutputOptions(CommandLineParser& parser);
    
    static void addDebugOptions(CommandLineParser& parser);
    
    static void addProfilingOptions(CommandLineParser& parser);
};

// Configuration file utilities
class ConfigFileUtils {
public:
    // Merge multiple configuration files
    static bool mergeFiles(const std::vector<std::string>& files,
                          std::map<std::string, std::string>& mergedConfig);
    
    // Validate configuration file format
    static bool validateFileFormat(const std::string& filepath);
    
    // Get configuration file backup path
    static std::string getBackupPath(const std::string& originalPath);
    
    // Create backup of configuration file
    static bool createBackup(const std::string& originalPath);
    
    // Restore from backup
    static bool restoreFromBackup(const std::string& originalPath);
    
    // List backup files
    static std::vector<std::string> listBackups(const std::string& directory);
};

// Command line completion support
class CommandLineCompletion {
public:
    CommandLineCompletion() = default;
    
    // Generate bash completion script
    static std::string generateBashCompletion(const CommandLineParser& parser);
    
    // Generate zsh completion script
    static std::string generateZshCompletion(const CommandLineParser& parser);
    
    // Generate fish completion script
    static std::string generateFishCompletion(const CommandLineParser& parser);
    
    // Check if completion is supported
    static bool isCompletionSupported();
};

// Environment variable helper
class EnvVarHelper {
public:
    EnvVarHelper() = default;
    
    // Get environment variable value
    static std::string getEnvVar(const std::string& name, 
                               const std::string& defaultValue = "");
    
    // Set environment variable
    static bool setEnvVar(const std::string& name, const std::string& value);
    
    // Get all environment variables with NLM prefix
    static std::map<std::string, std::string> getNLMEnvVars();
    
    // Parse environment variable to appropriate type
    template<typename T>
    static T parseEnvVar(const std::string& value, const std::string& key = "");
    
    // Check if environment variable exists
    static bool hasEnvVar(const std::string& name);
};

// Progress reporting for long-running operations
class ProgressReporter {
public:
    ProgressReporter() = default;
    
    // Start progress reporting
    void start(const std::string& description, int totalSteps);
    
    // Update progress
    void update(int currentStep, const std::string& status = "");
    
    // Complete progress reporting
    void complete(const std::string& finalStatus = "");
    
    // Set progress output format
    void setOutputFormat(const std::string& format);
    
    // Enable/disable progress reporting
    void enable(bool enable);
    
    // Check if progress reporting is enabled
    bool isEnabled() const;

private:
    bool enabled_ = true;
    std::string format_ = "[{percentage}%] {status} {elapsed}s/{total}s";
    std::chrono::system_clock::time_point startTime_;
    int totalSteps_ = 0;
    int currentStep_ = 0;
};

// Performance profiling and timing
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    
    // Start profiling a section
    void startProfile(const std::string& section);
    
    // End profiling a section
    double endProfile(const std::string& section);
    
    // Get total elapsed time
    double getTotalTime() const;
    
    // Print profile results
    void printProfileResults() const;
    
    // Save profile results to file
    bool saveProfileResults(const std::string& filepath) const;
    
    // Reset all profiles
    void reset();

private:
    std::map<std::string, std::chrono::system_clock::time_point> startTimes_;
    std::map<std::string, double> elapsedTimes_;
    std::chrono::system_clock::time_point sessionStart_;
};

// Metrics collection and reporting
class MetricsCollector {
public:
    MetricsCollector() = default;
    
    // Record a metric
    void recordMetric(const std::string& name, double value, 
                     const std::map<std::string, std::string>& tags = {});
    
    // Get metric value
    std::optional<double> getMetric(const std::string& name) const;
    
    // Get all metrics
    std::map<std::string, double> getAllMetrics() const;
    
    // Save metrics to file
    bool saveMetrics(const std::string& filepath, const std::string& format = "json") const;
    
    // Generate metrics report
    std::string generateReport() const;
    
    // Clear all metrics
    void clear();

private:
    std::map<std::string, double> metrics_;
    std::map<std::string, std::map<std::string, std::string>> metricTags_;
};

} // namespace nlm
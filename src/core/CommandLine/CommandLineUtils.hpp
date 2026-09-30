// Command-line utility functions for NLM
// Helper functions for argument processing, conversion, and configuration integration

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <cctype>
#include <sstream>
#include "core/CommandLine/CommandLineParser.hpp"
#include "core/CommandLine/CommandLineOptions.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// String utility functions
class CommandLineUtils {
public:
    // Convert string to lower case
    static std::string toLower(const std::string& str);
    
    // Convert string to upper case
    static std::string toUpper(const std::string& str);
    
    // Trim whitespace from both ends
    static std::string trim(const std::string& str);
    
    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // Join vector of strings with delimiter
    static std::string join(const std::vector<std::string>& parts, char delimiter);
    
    // Check if string starts with prefix
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // Check if string ends with suffix
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // Replace all occurrences of substring
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // Parse key-value pair from string
    static std::pair<std::string, std::string> parseKeyValue(const std::string& str);
    
    // Parse range specification (e.g., "1,10" or "1-10")
    static std::pair<double, double> parseRange(const std::string& rangeStr);
    
    // Format value with type-specific formatting
    static std::string formatValue(const std::string& value, const std::string& type);
    
    // Validate value against type
    static bool validateValueType(const std::string& value, const std::string& type);
    
    // Convert boolean string to bool
    static bool parseBool(const std::string& value);
    
    // Convert string to int64
    static int64_t parseInt(const std::string& value);
    
    // Convert string to double
    static double parseDouble(const std::string& value);
    
    // Get config type from string type
    static ConfigValue::Type getConfigTypeFromString(const std::string& typeStr);
    
    // Get string type from config type
    static std::string getTypeFromConfigType(ConfigValue::Type type);
    
    // Parse advanced options with complex formats
    static std::map<std::string, std::string> parseAdvancedOptions(const std::vector<std::string>& args);
    
    // Validate option combinations
    static std::vector<std::string> validateOptionCombinations(const std::vector<CommandLineArg>& args);
    
    // Check if option requires another option
    static bool optionRequires(const std::string& option, const std::string& required, const std::vector<CommandLineArg>& args);
    
    // Get all options that conflict with a given option
    static std::vector<std::string> getConflictingOptions(const std::string& option, const std::vector<CommandLineArg>& args);
    
    // Format duration in seconds to human-readable format
    static std::string formatDuration(double seconds);
    
    // Format bytes to human-readable format
    static std::string formatBytes(size_t bytes);
    
    // Generate progress bar
    static std::string generateProgressBar(double progress, int width = 50);
    
    // Escape special characters for help text
    static std::string escapeHelpText(const std::string& text);
    
    // Unescape help text
    static std::string unescapeHelpText(const std::string& text);
    
    // Generate command line from argument list
    static std::string generateCommandLine(const std::vector<CommandLineArg>& args);
    
    // Parse command line string to arguments
    static std::vector<CommandLineArg> parseCommandLine(const std::string& commandLine);
};

// Configuration integration utilities
class ConfigIntegrationUtils {
public:
    // Apply command line arguments to configuration
    static void applyCommandLineToConfig(const std::vector<CommandLineArg>& args, Config& config);
    
    // Merge configuration sources with precedence: CommandLine > File > Default
    static void mergeConfigSources(Config& config);
    
    // Validate configuration against command line
    static std::vector<std::string> validateConfigAgainstCommandLine(const Config& config, const std::vector<CommandLineArg>& args);
    
    // Get configuration value with type conversion
    template<typename T>
    static T getConfigValue(const Config& config, const std::string& key, const T& defaultValue);
    
    // Set configuration value from command line
    static void setConfigValueFromCommandLine(Config& config, const std::string& key, const std::string& value);
    
    // Check if configuration is complete (all required values set)
    static bool isConfigurationComplete(const Config& config);
    
    // Get missing required configuration keys
    static std::vector<std::string> getMissingRequiredKeys(const Config& config);
    
    // Validate configuration schema
    static bool validateConfigurationSchema(const Config& config);
    
    // Resolve environment variables in configuration values
    static std::string resolveEnvVars(const std::string& value);
    
    // Expand file paths in configuration values
    static std::string expandPath(const std::string& path);
};

// Performance monitoring utilities
class PerformanceUtils {
public:
    // Measure execution time
    static double measureExecutionTime(std::function<void()> func);
    
    // Get memory usage in bytes
    static size_t getMemoryUsage();
    
    // Get CPU core count
    static int getCoreCount();
    
    // Check if SIMD is available
    static bool isSIMDSupported();
    
    // Get CPU features
    static std::string getCPUFeatures();
    
    // Optimize configuration for current hardware
    static void optimizeConfigForHardware(Config& config);
    
    // Get performance recommendation
    static std::string getPerformanceRecommendation(const Config& config);
};

// File and path utilities
class FileUtils {
public:
    // Check if file exists
    static bool fileExists(const std::string& path);
    
    // Get file size
    static size_t getFileSize(const std::string& path);
    
    // List files matching pattern
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern = "*");
    
    // Create directory recursively
    static bool createDirectory(const std::string& path);
    
    // Get file extension
    static std::string getFileExtension(const std::string& filename);
    
    // Get base filename without extension
    static std::string getBaseFilename(const std::string& filename);
    
    // Get parent directory
    static std::string getParentDirectory(const std::string& path);
    
    // Check if path is absolute
    static bool isAbsolutePath(const std::string& path);
    
    // Normalize path
    static std::string normalizePath(const std::string& path);
    
    // Expand user home directory (~)
    static std::string expandHomeDirectory(const std::string& path);
};
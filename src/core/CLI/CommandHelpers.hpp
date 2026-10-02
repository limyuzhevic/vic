// CommandHelpers.hpp - Helper functions for common CLI operations
// Provides utilities for argument parsing, validation, and common CLI tasks

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <algorithm>

namespace nlm {
namespace CLI {

// Helper functions for common CLI operations
class CommandHelpers {
public:
    // Parse --key=value style arguments (backward compatibility)
    static bool parseKeyValueArgs(int argc, char** argv, 
                                  std::unordered_map<std::string, std::string>& args);
    
    // Format help text with examples
    static std::string formatHelp(const std::string& title, const std::string& description,
                                 const std::vector<std::string>& examples);
    
    // Format configuration for output
    static std::string formatConfig(const Config& config, const std::string& format);
    
    // Convert string to lowercase
    static std::string toLowercase(const std::string& str);
    
    // Convert string to uppercase
    static std::string toUppercase(const std::string& str);
    
    // Check if file exists
    static bool fileExists(const std::string& path);
    
    // Get current timestamp
    static std::string getTimestamp();
    
    // Progress reporting
    static void showProgress(int current, int total, const std::string& message = "");
    
    // Format duration
    static std::string formatDuration(std::chrono::milliseconds duration);
    
    // Validate output format
    static bool validateOutputFormat(const std::string& format);
    
    // Validate log level
    static bool validateLogLevel(const std::string& level);
    
    // Parse comma-separated list
    static std::vector<std::string> parseCommaSeparated(const std::string& str);
    
    // Join vector to string with separator
    static std::string joinVector(const std::vector<std::string>& vec, const std::string& separator);
    
    // Truncate string
    static std::string truncate(const std::string& str, size_t maxLength);
    
    // Wrap text to specified width
    static std::string wrapText(const std::string& text, size_t width);
};

} // namespace CLI
} // namespace nlm
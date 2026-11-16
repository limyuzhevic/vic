// Common tool utilities and classes for NLM advanced tools

#pragma once

#include <string>
#include <vector>
#include <map>
#include "core/Logger/Logger.hpp"

namespace nlm {

// Tool configuration structure
struct ToolConfig {
    std::string logLevel;
    std::string logFile;
    bool quiet;
    bool verbose;
    std::string outputFormat;
    std::map<std::string, std::string> options;
};

// Common tool utilities
class ToolUtils {
public:
    // Argument parsing utilities
    static std::vector<std::pair<std::string, std::string>> parseArgs(int argc, char** argv);
    
    // Help and usage utilities
    static void printHelp(const std::string& toolName, const std::string& description);
    
    // File and directory utilities
    static bool validateFile(const std::string& filepath, bool requireRead = false);
    static bool ensureDirectory(const std::string& dirpath);
    
    // String utilities
    static std::string toLowercase(const std::string& str);
    static std::string trim(const std::string& str);
};

// Global tool initialization function
bool initializeTool(const ToolConfig& config);

// Global tool cleanup function
void cleanupTool();

} // namespace nlm
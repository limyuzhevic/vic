#pragma once

#include <string>
#include <vector>
#include <utility>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {

/**
 * Command-line tools for NLM neural simulation framework
 * 
 * This module provides advanced command-line utilities for:
 * - Brain configuration management
 * - Batch simulation execution
 * - Simulation result analysis
 * - Text-based brain activity visualization
 * - Checkpoint management
 * - Performance profiling
 * 
 * All tools use the existing NLM framework APIs and follow the project's
 * coding conventions and patterns.
 */

enum class ToolExitCode {
    Success = 0,
    UsageError = 1,
    FileError = 2,
    ConfigError = 3,
    SimulationError = 4,
    AnalysisError = 5,
    VisualizationError = 6,
    CheckpointError = 7,
    PerformanceError = 8
};

/**
 * Common tool utilities and helper functions
 */
class ToolUtils {
public:
    /**
     * Parse command-line arguments into key-value pairs
     */
    static std::vector<std::pair<std::string, std::string>> parseArgs(int argc, char** argv);
    
    /**
     * Print help message for a tool
     */
    static void printHelp(const std::string& toolName, const std::string& description);
    
    /**
     * Validate file path exists and is readable
     */
    static bool validateFile(const std::string& filepath, bool requireRead = true);
    
    /**
     * Ensure directory exists
     */
    static bool ensureDirectory(const std::string& dirpath);
    
    /**
     * Convert string to lowercase
     */
    static std::string toLowercase(const std::string& str);
    
    /**
     * Trim whitespace from string
     */
    static std::string trim(const std::string& str);
};

/**
 * Common configuration for tools
 */
struct ToolConfig {
    std::string logLevel = "info";
    std::string logFile = "";
    bool verbose = false;
    bool quiet = false;
};

/**
 * Initialize tool environment
 */
bool initializeTool(const ToolConfig& config);

/**
 * Cleanup tool environment
 */
void cleanupTool();

} // namespace nlm

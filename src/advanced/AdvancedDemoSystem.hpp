// NLM Advanced Demo System
//
// Advanced demo modes for power users and researchers
// Provides custom parameter demonstrations, interactive sessions,
// debugging tools, and experimental capabilities
// Maintains backward compatibility with enhanced features

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <random>
#include <functional>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Advanced demo modes
enum class AdvancedDemoMode {
    Standard,      // Standard demonstration (backward compatibility)
    Custom,        // Custom parameter demonstration
    Interactive,   // Interactive demonstration with user input
    Debugging,     // Debugging and analysis mode
    Experimental,  // Experimental mode with parameter sweeps
    Visualization, // Visualization mode
    Benchmarking,  // Performance benchmarking mode
    Research       // Research mode with data collection
};

// Custom parameter demonstration
struct CustomParameters {
    std::string name;
    std::map<std::string, std::string> parameters;
    std::vector<std::pair<std::string, std::function<bool(const Config&)>> validationRules;
    std::vector<std::string> dependencies;
    bool enabled;
    double executionTime;
    bool success;
    std::string description;
    std::map<std::string, std::string> metadata;
    
    CustomParameters(const std::string& n, const std::string& desc = "")
        : name(n), enabled(true), executionTime(0.0), success(false), description(desc) {}
};

// Debugging tools
struct DebugInfo {
    std::string sessionId;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string eventType;
    std::string description;
    std::map<std::string, std::string> data;
    size_t threadId;
    std::string sourceFile;
    int sourceLine;
    std::string functionName;
    std::map<std::string, std::string> stackTrace;
    bool critical;
    std::string severity;
    
    DebugInfo(const std::string& event, const std::string& desc, bool crit = false, const std::string& sev = "INFO")
        : eventType(event), description(desc), critical(crit), severity(sev) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Advanced demo system
class AdvancedDemoSystem {
public:
    AdvancedDemoSystem();
    ~AdvancedDemoSystem();
    
    // Demo mode management
    void setDemoMode(AdvancedDemoMode mode);
    AdvancedDemoMode getDemoMode() const;
    bool isDemoMode(AdvancedDemoMode mode) const;
    
    // Custom parameter management
    void addCustomParameter(const CustomParameters& param);
    void removeCustomParameter(const std::string& name);
    bool hasCustomParameter(const std::string& name) const;
    CustomParameters& getCustomParameter(const std::string& name);
    std::vector<CustomParameters> getAllCustomParameters() const;
    
    // Debugging tools
    void logDebugEvent(const DebugInfo& debugInfo);
    std::vector<DebugInfo> getDebugLogs(const std::string& sessionId = "") const;
    void clearDebugLogs(const std::string& sessionId = "");
    void setDebugLevel(const std::string& level);
    std::string getDebugLevel() const;
    
    // Advanced demo execution
    bool runAdvancedDemo(const std::shared_ptr<Config>& config);
    bool runCustomParameterDemo(const std::string& paramName, const std::shared_ptr<Config>& config);
    bool runDebugDemo(const std::shared_ptr<Config>& config);
    
    // Advanced output and reporting
    std::string generateAdvancedReport(const std::shared_ptr<Brain>& brain) const;
    std::string generateCustomParameterReport(const std::string& paramName) const;
    std::string generateDebugReport() const;
    
    // Configuration and state management
    void updateConfiguration(const std::shared_ptr<Config>& config);
    std::shared_ptr<Config> getCurrentConfiguration() const;
    void saveConfiguration(const std::string& filename) const;
    bool loadConfiguration(const std::string& filename);
    
    // Validation and error handling
    std::vector<std::string> validateParameters(const std::shared_ptr<Config>& config) const;
    bool isValidConfiguration(const std::shared_ptr<Config>& config) const;
    std::vector<std::string> getValidationErrors(const std::shared_ptr<Config>& config) const;
    
    // Utility functions
    static std::string generateSessionId();
    static std::string getCurrentTimestamp();
    static void logMessage(const std::string& level, const std::string& message);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Command-line advanced demo tool
class AdvancedDemoTool {
public:
    struct AdvancedDemoArgs {
        std::string mode;
        std::string configFile;
        bool verbose;
        bool quiet;
        bool autoMode;
        bool saveState;
        std::string outputFile;
        std::map<std::string, std::string> customOptions;
    };
    
    static int run(int argc, char** argv);
    static int runAdvancedDemo(const AdvancedDemoArgs& args);
    static int runCustomParameterDemo(const std::string& paramName, const AdvancedDemoArgs& args);
    static int runDebugDemo(const AdvancedDemoArgs& args);
    static void printHelp();
    static void printVersion();
    static AdvancedDemoArgs parseArgs(int argc, char** argv);
    
private:
    static void setupLogging(const AdvancedDemoArgs& args);
    static std::shared_ptr<Config> loadConfiguration(const std::string& configFile);
    static AdvancedDemoMode parseDemoMode(const std::string& modeStr);
    static std::string generateDemoReport(const std::string& mode, const std::shared_ptr<Config>& config);
    static bool exportDemoResults(const std::string& filename, const std::string& mode,
                                 const std::shared_ptr<Config>& config);
};

} // namespace nlm
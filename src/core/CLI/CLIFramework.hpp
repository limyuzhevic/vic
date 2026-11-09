#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

/**
 * Command-line interface options and configuration
 */
struct CLIConfig {
    std::string demoName;
    std::string configFile;
    std::string logFile;
    std::string checkpointPath;
    LogLevel logLevel;
    bool enableColors;
    bool enableVerbose;
    bool enablePerformanceMonitoring;
    bool autoSaveCheckpoint;
    float checkpointInterval;
    size_t maxMemoryUsageMB;
    std::vector<std::string> customArgs;
    
    CLIConfig() : 
        logLevel(LogLevel::Info),
        enableColors(true),
        enableVerbose(false),
        enablePerformanceMonitoring(false),
        autoSaveCheckpoint(false),
        checkpointInterval(60.0f),  // 60 seconds
        maxMemoryUsageMB(512) {}
};

/**
 * Performance monitoring data
 */
struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsageMB;
    double wallClockTime;
    size_t totalSpikesGenerated;
    size_t neuronsActive;
    size_t synapsesActive;
    double avgFiringRate;
    double simulationSpeed;
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsageMB(0.0), 
                           wallClockTime(0.0), totalSpikesGenerated(0),
                           neuronsActive(0), synapsesActive(0),
                           avgFiringRate(0.0), simulationSpeed(1.0) {}
};

/**
 * Unified command-line interface framework for NLM demos
 */
class CLIFramework {
public:
    CLIFramework();
    ~CLIFramework();
    
    // Initialize framework
    bool initialize(const std::string& demoName, int argc, char** argv);
    
    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Setup logging based on configuration
    bool setupLogging();
    
    // Load configuration
    bool loadConfiguration();
    
    // Get configuration
    const CLIConfig& getConfig() const { return config; }
    const std::shared_ptr<Config>& getNLMConfig() const { return nlmConfig; }
    
    // Get performance metrics
    const PerformanceMetrics& getPerformanceMetrics() const { return performanceMetrics; }
    
    // Start performance monitoring
    void startPerformanceMonitoring();
    
    // Stop performance monitoring
    void stopPerformanceMonitoring();
    
    // Update performance metrics
    void updatePerformanceMetrics(const std::shared_ptr<Brain>& brain);
    
    // Display help information
    void showHelp() const;
    
    // Display version information
    void showVersion() const;
    
    // Display configuration summary
    void showConfiguration() const;
    
    // Display performance metrics
    void showPerformanceMetrics() const;
    
    // Save checkpoint
    bool saveCheckpoint(const std::shared_ptr<Brain>& brain) const;
    
    // Load checkpoint
    bool loadCheckpoint(std::shared_ptr<Brain>& brain) const;
    
    // Execute demo with framework
    int executeDemo(std::function<int(const CLIConfig&, const std::shared_ptr<Config>&)> demoFunction);
    
    // Register custom argument
    void registerArgument(const std::string& name, const std::string& description, 
                         bool required = false, const std::string& defaultValue = "");
    
    // Register custom option
    void registerOption(const std::string& name, const std::string& description,
                       bool* flag = nullptr, const std::string& defaultValue = "");
    
    // Get argument value
    std::string getArgument(const std::string& name) const;
    
    // Get option value
    bool getOption(const std::string& name) const;
    int getOptionInt(const std::string& name) const;
    float getOptionFloat(const std::string& name) const;
    std::string getOptionString(const std::string& name) const;
    
private:
    // Parse predefined arguments
    bool parsePredefinedArguments(int argc, char** argv);
    
    // Parse custom arguments
    bool parseCustomArguments(int argc, char** argv);
    
    // Process long arguments (--key=value)
    bool processLongArgument(const std::string& arg);
    
    // Process short arguments (-k value)
    bool processShortArgument(const std::string& arg, const std::string& value);
    
    // Show error message
    void showError(const std::string& message) const;
    
    // Show warning message
    void showWarning(const std::string& message) const;
    
    // Parse boolean value
    bool parseBool(const std::string& str) const;
    
    // Parse numeric value
    template<typename T>
    T parseNumber(const std::string& str) const;
    
    // Trim whitespace from string
    std::string trim(const std::string& str) const;
    
    // Convert to lowercase
    std::string toLower(const std::string& str) const;
    
    // Get current memory usage in MB
    double getCurrentMemoryUsage() const;
    
    // Get current CPU usage
    double getCurrentCPUUsage() const;
    
    // Internal state
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    CLIConfig config;
    std::shared_ptr<Config> nlmConfig;
    PerformanceMetrics performanceMetrics;
    std::chrono::high_resolution_clock::time_point startTime;
    std::map<std::string, std::string> arguments;
    std::map<std::string, std::string> options;
    std::map<std::string, std::string> argumentDescriptions;
    std::map<std::string, std::string> optionDescriptions;
    std::map<std::string, bool*> optionFlags;
};

} // namespace nlm
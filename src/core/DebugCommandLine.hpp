// NLM Advanced Debugging and Profiling Tools - Debug Command Line Interface

#pragma once

#include "core/CommandLineParser.hpp"
#include "debug/DebugManager.hpp"
#include "profiling/PerformanceProfiler.hpp"
#include <iostream>
#include <string>
#include <memory>

namespace nlm {

// Debug command line options
struct DebugCommandLineOptions {
    bool enableDebug = false;
    bool enableProfiling = false;
    bool enableMonitoring = false;
    bool enableOptimization = false;
    bool enableVisualization = false;
    std::string debugConfigFile = "debug_config.json";
    std::string profileConfigFile = "profile_config.json";
    std::string outputFormat = "text";
    std::string outputFile = "";
    double profilingInterval = 0.5;
    std::string logLevel = "info";
    bool nonIntrusiveMode = true;
    bool exportAllData = false;
    bool validateOnly = false;
};

// Debug command line interface
class DebugCommandLine {
public:
    DebugCommandLine() = default;
    ~DebugLineInterface() = default;
    
    // Parse command line arguments
    bool parseArguments(int argc, char* argv[]);
    void printUsage() const;
    void printHelp() const;
    
    // Execute debugging and profiling
    bool execute(DebugCommandLineOptions& options);
    bool initializeDebugManager(const DebugCommandLineOptions& options);
    bool initializePerformanceProfiler(const DebugCommandLineOptions& options);
    
    // Configuration management
    bool loadDebugConfig(const std::string& filename, DebugCommandLineOptions& options);
    bool loadProfileConfig(const std::string& filename, DebugCommandLineOptions& options);
    
    // Output generation
    void generateOutput(const std::string& format, const std::string& filename = "");
    void exportDebugData(const std::string& format, const std::string& filename = "");
    void exportProfileData(const std::string& format, const std::string& filename = "");
    
    // Status and monitoring
    void printStatus() const;
    void printPerformanceReport() const;
    void printDebugReport() const;
    void printSystemStatus() const;
    
    // Convenience methods
    void runDebugSession();
    void runProfileSession();
    void runMonitorSession();
    void runOptimizationSession();
    
private:
    std::unique_ptr<CommandLineParser> parser_;
    std::unique_ptr<DebugManager> debugManager_;
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    
    DebugCommandLineOptions options_;
    
    // Internal methods
    void setupDebugTools();
    void setupPerformanceTools();
    void setupMonitoringTools();
    void setupVisualizationTools();
    void setupExportTools();
    void validateConfiguration() const;
    std::string getTimestampString() const;
};

} // namespace nlm

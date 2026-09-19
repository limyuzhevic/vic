// NLM Unified Command Line Interface Enhancement
// Unified CLI for all NLM executables with advanced features

#ifndef NLM_CLI_H
#define NLM_CLI_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

class NLMCLI {
public:
    struct CommandLineOptions {
        // Basic options
        std::string configFile = "configs/default.cfg";
        std::string outputFile;
        std::string logLevel = "info";
        bool verbose = false;
        bool quiet = false;
        bool helpRequested = false;
        
        // Simulation options
        int numSteps = 1000;
        double timestep = 0.001;
        int randomSeed = 42;
        bool enableCheckpointing = false;
        bool enableReplay = false;
        bool enableDevelopment = false;
        
        // Performance options
        int maxMemoryMB = 512;
        int threads = 1;
        bool benchmark = false;
        std::string benchmarkMode = "quick";
        
        // Experiment options
        int experimentType = 0; // 0: Basic, 1: Phase3, 2: Phase4, 3: Phase6
        bool runAllPhases = false;
        std::vector<std::string> experimentArgs;
        
        // Debug/Validation options
        bool validateConfig = false;
        bool validateConnections = false;
        bool profilePerformance = false;
        std::string profileOutput;
        
        // Visualization options
        bool enableVisualization = false;
        std::string visualizationOutput;
        int visualizationFrequency = 100;
        
        // Export options
        bool exportNeuralData = false;
        bool exportMemoryData = false;
        bool exportConfiguration = false;
        std::string exportFormat = "json";
    };
    
    struct PerformanceMetrics {
        double totalWallClockTime = 0.0;
        double cpuTime = 0.0;
        size_t peakMemoryUsage = 0;
        size_t totalMemoryAllocated = 0;
        int stepsPerSecond = 0;
        std::vector<double> stepTimes;
    };
    
    NLMCLI();
    ~NLMCLI();
    
    // Parse command line arguments
    bool parseArguments(int argc, char* argv[]);
    
    // Display help and usage information
    void displayHelp() const;
    void displayVersion() const;
    void displayConfiguration() const;
    void displayAdvancedHelp() const;
    
    // Process and validate options
    bool validateOptions() const;
    bool processOptions();
    
    // Get parsed options
    const CommandLineOptions& getOptions() const { return options; }
    
    // Configuration loading with validation
    bool loadConfiguration();
    bool validateConfiguration() const;
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    void printPerformanceReport() const;
    
    // Export and reporting utilities
    bool exportResults() const;
    bool exportNeuralState() const;
    bool exportMemoryState() const;
    bool exportConfigurationState() const;
    
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLowerCase(const std::string& str);
    static bool fileExists(const std::string& path);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
private:
    CommandLineOptions options;
    PerformanceMetrics metrics;
    std::chrono::high_resolution_clock::time_point startTime;
    std::shared_ptr<Config> currentConfig;
    
    // Configuration validation schema
    struct ConfigValidationRule {
        std::string key;
        std::string type; // "int", "float", "bool", "string"
        std::string validation; // "range:min:max", "enum:value1:value2", etc.
        bool required = false;
        std::string defaultValue;
    };
    
    std::vector<ConfigValidationRule> validationRules;
    
    // Option parsing helpers
    bool parseArgument(const std::string& arg, size_t& index, int argc, char* argv[]);
    bool parseKeyValue(const std::string& arg, std::string& key, std::string& value);
    bool parseNumericValue(const std::string& value, double& result);
    
    // Configuration validation helpers
    bool applyValidationRule(const ConfigValidationRule& rule) const;
    bool validateAllRules() const;
    
    // Performance monitoring helpers
    void recordStepTime(double stepTime);
    void printProgress() const;
    
    // Export helpers
    bool exportToJSON(const std::string& filename) const;
    bool exportToText(const std::string& filename) const;
    bool exportToBinary(const std::string& filename) const;
};

} // namespace nlm

#endif // NLM_CLI_H
// Command-line interface for NLM (Neural Learning Machine) advanced features
// Provides advanced tools for configuration validation, network analysis, and experimental control

#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Command types for advanced CLI
enum class CLICommand {
    Help,
    Version,
    ConfigValidate,
    ConfigGenerate,
    NetworkAnalyze,
    NetworkStats,
    BrainRun,
    BrainExport,
    BrainImport,
    BrainBenchmark,
    MemoryProfile,
    PerformanceProfile,
    ExperimentRun,
    ExperimentList,
    DebugInfo,
    Diagnostics,
    Clear,
    Exit
};

// CLI tool configuration
struct CLIToolConfig {
    std::string name;
    std::string version;
    std::string description;
    std::map<std::string, std::string> options;
    bool verbose;
    bool quiet;
    std::string outputFormat;
};

// Command-line argument parser
class CLIParser {
public:
    CLIParser();
    ~CLIParser();
    
    // Parse command line arguments
    bool parse(int argc, char* argv[]);
    
    // Get parsed command
    CLICommand getCommand() const;
    
    // Get command arguments
    const std::vector<std::string>& getArgs() const;
    
    // Get option value
    std::string getOption(const std::string& option) const;
    
    // Check if option exists
    bool hasOption(const std::string& option) const;
    
    // Get help text
    std::string getHelpText() const;
    
    // Print help
    void printHelp() const;
    
private:
    CLICommand command_;
    std::vector<std::string> args_;
    std::map<std::string, std::string> options_;
    std::string commandStr_;
};

// Advanced configuration validation tool
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validate configuration file
    bool validateFile(const std::string& filepath);
    
    // Validate configuration string
    bool validateString(const std::string& configString);
    
    // Generate validation report
    std::string generateReport(const std::string& filepath) const;
    
    // Check for common configuration errors
    std::vector<std::string> checkCommonErrors(const std::string& filepath) const;
    
    // Validate configuration against schema
    bool validateSchema(const std::string& filepath, int schemaVersion = 1);
    
    // Get validation statistics
    struct ValidationStats {
        int totalSections;
        int validSections;
        int invalidSections;
        std::vector<std::string> errorMessages;
        double validationScore;
    };
    
    ValidationStats getStats() const;
    
    // Suggest configuration fixes
    std::vector<std::string> suggestFixes(const std::string& filepath) const;
    
private:
    ValidationStats stats_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

// Network analysis tool
class NetworkAnalyzer {
public:
    NetworkAnalyzer();
    ~NetworkAnalyzer();
    
    // Load brain from file for analysis
    bool loadBrain(const std::string& filepath);
    
    // Analyze network connectivity
    struct NetworkMetrics {
        int neuronCount;
        int synapseCount;
        int regionCount;
        double averageDegree;
        double clusteringCoefficient;
        double pathLength;
        double smallWorldness;
        double efficiency;
        double modularity;
        std::vector<double> degreeDistribution;
        std::vector<double> weightDistribution;
    };
    
    NetworkMetrics analyzeConnectivity() const;
    
    // Analyze community structure
    struct CommunityAnalysis {
        int communityCount;
        std::vector<int> communitySizes;
        std::vector<std::vector<int>> communities;
        double modularityQuality;
        std::vector<double> communityStability;
    };
    
    CommunityAnalysis analyzeCommunities() const;
    
    // Analyze network robustness
    struct RobustnessAnalysis {
        double attackTolerance;
        double failureRecovery;
        double redundancy;
        std::vector<double> removalResilience;
    };
    
    RobustnessAnalysis analyzeRobustness() const;
    
    // Generate network visualization data
    std::string generateNetworkGraph(const std::string& format = "graphml") const;
    
    // Analyze activity patterns
    struct ActivityAnalysis {
        double averageFiringRate;
        double firingVariance;
        double burstFrequency;
        double synchrony;
        std::vector<double> neuronFiringRates;
        std::vector<double> activityCorrelations;
    };
    
    ActivityAnalysis analyzeActivityPatterns() const;
    
    // Generate network statistics report
    std::string generateNetworkReport() const;
    
private:
    std::string brainFilepath_;
    mutable bool brainLoaded_;
};

// Performance benchmarking tool
class PerformanceBenchmark {
public:
    PerformanceBenchmark();
    ~PerformanceBenchmark();
    
    // Run benchmark on brain simulation
    struct BenchmarkResult {
        double realTime;           // Wall clock time
        double simulationTime;     // Simulated time
        double neuronsPerSecond;   // Processing speed
        double synapsesPerSecond;  // Synapse operations speed
        double memoryUsage;        // Memory consumption
        double cpuUsage;           // CPU utilization
        std::vector<double> metrics; // Additional metrics
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
    };
    
    BenchmarkResult runBenchmark(const std::string& configFile, int duration = 60);
    
    // Compare performance across different configurations
    struct PerformanceComparison {
        std::string configurationName;
        BenchmarkResult result;
        double improvementFactor;
        std::string recommendation;
    };
    
    std::vector<PerformanceComparison> compareConfigurations(
        const std::vector<std::string>& configFiles);
    
    // Profile memory usage
    struct MemoryProfile {
        size_t totalMemory;
        size_t neuronMemory;
        size_t synapseMemory;
        size_t plasticityMemory;
        size_t memoryPoolUsage;
        double memoryEfficiency;
    };
    
    MemoryProfile profileMemoryUsage() const;
    
    // Generate performance report
    std::string generatePerformanceReport(const BenchmarkResult& result) const;
    
    // Check for performance bottlenecks
    std::vector<std::string> detectBottlenecks() const;
    
private:
    BenchmarkResult runSingleBenchmark(const std::string& configFile, int duration);
};

// Configuration management tool
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    
    // List available configurations
    std::vector<std::string> listConfigurations(const std::string& directory = ".");
    
    // Create new configuration
    bool createConfiguration(const std::string& name, const std::string& content);
    
    // Backup configurations
    bool backupConfigurations(const std::string& outputDir);
    
    // Restore configurations
    bool restoreConfigurations(const std::string& inputDir);
    
    // Validate configuration suite
    struct ConfigSuiteResult {
        int totalConfigs;
        int validConfigs;
        int invalidConfigs;
        std::map<std::string, std::vector<std::string>> configErrors;
        double overallScore;
    };
    
    ConfigSuiteResult validateConfigSuite(const std::string& directory);
    
    // Generate configuration summary
    std::string generateConfigSummary(const std::string& directory) const;
    
private:
    std::vector<std::string> findConfigFiles(const std::string& directory) const;
    bool isConfigFile(const std::string& filepath) const;
};

// Diagnostic tool
class DiagnosticTool {
public:
    DiagnosticTool();
    ~DiagnosticTool();
    
    // Run comprehensive diagnostics
    struct DiagnosticReport {
        bool systemOK;
        std::vector<std::string> systemInfo;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        std::vector<std::string> suggestions;
        double healthScore;
        std::chrono::system_clock::time_point timestamp;
    };
    
    DiagnosticReport runDiagnostics();
    
    // Check specific system components
    struct ComponentStatus {
        std::string componentName;
        bool healthy;
        std::string statusMessage;
        double performanceMetric;
        std::chrono::system_clock::time_point lastCheck;
    };
    
    std::vector<ComponentStatus> checkComponents() const;
    
    // Get system information
    struct SystemInfo {
        std::string osInfo;
        std::string compilerInfo;
        std::string buildInfo;
        std::string nlmVersion;
        size_t availableMemory;
        size_t usedMemory;
        std::string architecture;
    };
    
    SystemInfo getSystemInfo() const;
    
    // Get NLM-specific information
    struct NLMInfo {
        std::string version;
        std::string buildDate;
        std::string gitCommit;
        std::vector<std::string> features;
        std::vector<std::string> experimentalFeatures;
        int maxNeurons;
        int maxSynapses;
    };
    
    NLMInfo getNLMInfo() const;
    
    // Generate diagnostic log
    std::string generateDiagnosticLog(const DiagnosticReport& report) const;
    
private:
    std::string collectSystemInfo() const;
    std::string collectNLMInfo() const;
    std::string checkEnvironment() const;
};

// Main CLI application class
class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Run CLI application
    int run(int argc, char* argv[]);
    
    // Register tools
    void registerTool(const std::string& name, std::function<void(const std::vector<std::string>&)> toolFunc);
    
    // Get tool list
    std::vector<std::string> getToolList() const;
    
    // Print version information
    void printVersion() const;
    
    // Print help
    void printHelp() const;
    
private:
    CLIParser parser_;
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> tools_;
    CLIToolConfig config_;
    DiagnosticTool diagnostics_;
};

} // namespace nlm

// Global CLI application instance
extern nlm::NLMCLI gCLI;
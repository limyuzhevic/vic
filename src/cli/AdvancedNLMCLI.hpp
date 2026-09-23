# Advanced NLM Command Line Interface
# ================================
# Provides advanced commands for running, configuring, and managing NLM experiments
# Supports batch processing, scripting, and advanced configuration management

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>

namespace nlm {

class AdvancedNLMCLI {
public:
    AdvancedNLMCLI();
    ~AdvancedNLMCLI();
    
    // Main command line processing
    bool processCommandLine(int argc, char* argv[]);
    
    // Command handlers
    bool handleCommand(const std::string& cmd, const std::vector<std::string>& args);
    
    // Advanced commands
    bool cmdRun(const std::vector<std::string>& args);
    bool cmdBatch(const std::vector<std::string>& args);
    bool cmdScript(const std::vector<std::string>& args);
    bool cmdConfig(const std::vector<std::string>& args);
    bool cmdExport(const std::vector<std::string>& args);
    bool cmdImport(const std::vector<std::string>& args);
    bool cmdStats(const std::vector<std::string>& args);
    bool cmdMonitor(const std::vector<std::string>& args);
    bool cmdBenchmark(const std::vector<std::string>& args);
    bool cmdProfile(const std::vector<std::string>& args);
    
    // Configuration management
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath) const;
    bool validateConfiguration() const;
    
    // Batch processing
    struct BatchJob {
        std::string name;
        std::vector<std::string> commandArgs;
        double priority;
        std::chrono::system_clock::time_point scheduledTime;
        bool enabled;
    };
    
    bool addBatchJob(const BatchJob& job);
    bool removeBatchJob(const std::string& name);
    bool listBatchJobs() const;
    bool executeBatch(const std::string& batchName);
    bool executeAllBatches();
    
    // Script execution
    bool executeScript(const std::string& scriptPath);
    std::vector<std::string> parseScript(const std::string& scriptContent);
    
    // Monitoring and profiling
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double simulationTime;
        size_t neuronCount;
        size_t synapseCount;
        float avgFiringRate;
        float totalReward;
    };
    
    PerformanceMetrics getCurrentMetrics() const;
    bool startMonitoring(const std::string& outputFile);
    bool stopMonitoring();
    
    // Help and documentation
    void printHelp(const std::string& command = "") const;
    void printAdvancedHelp() const;
    void printExamples() const;
    
    // Configuration templates
    static std::string getDefaultConfigTemplate(const std::string& type = "phase6");
    static std::string getQuickStartTemplate();
    static std::string getResearchTemplate();
    
private:
    // Command line parsing
    struct ParsedCommand {
        std::string command;
        std::vector<std::string> arguments;
        std::map<std::string, std::string> options;
    };
    
    ParsedCommand parseCommandLine(int argc, char* argv[]);
    bool validateCommand(const ParsedCommand& cmd) const;
    
    // Command help text
    std::map<std::string, std::string> getCommandHelp() const;
    
    // Internal state
    std::string currentConfigFile;
    std::vector<BatchJob> batchJobs_;
    PerformanceMetrics currentMetrics_;
    bool monitoringEnabled_;
    std::string monitoringOutputFile;
    
    // Helper functions
    static bool stringToBool(const std::string& str);
    static std::string boolToString(bool value);
    static std::string timestampToString(const std::chrono::system_clock::time_point& tp);
    
    // Script processing
    std::vector<std::string> readScriptFile(const std::string& filepath);
    bool writeScriptFile(const std::string& filepath, const std::vector<std::string>& commands);
};

} // namespace nlm

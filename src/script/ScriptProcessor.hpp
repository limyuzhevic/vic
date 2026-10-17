// NLM Script Processing System
// Provides script parsing, validation, and execution for batch processing

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include "core/Config/Config.hpp"

namespace nlm {

// Script configuration structure for loading and validation
struct ScriptConfig {
    std::string name;
    std::string description;
    std::string format;  // json, yaml, simple
    std::string filename;
    std::map<std::string, std::string> parameters;
    bool validate = true;
    
    ScriptConfig() {}
    ScriptConfig(const std::string& n, const std::string& desc) : name(n), description(desc) {}
};

// Script execution options
struct ScriptOptions {
    std::string scriptFile;
    bool dryRun = false;
    bool continueFromCheckpoint = false;
    std::string checkpointPath;
    bool verbose = false;
    std::string outputFormat;  // json, csv, text
    std::string outputFile;
    bool parallel = false;
    size_t maxThreads = std::thread::hardware_concurrency();
    double timeoutSeconds = 0.0;  // 0 = no timeout
    std::string logLevel = "info";
    
    // Parameter sweeps
    std::vector<std::pair<std::string, std::vector<std::string>>> sweepParameters;
    std::vector<std::pair<std::string, std::vector<std::string>>> gridSearch;
    
    // Execution modes
    bool gridSearchMode = false;
    bool sweepMode = false;
    bool sequentialMode = false;
    std::vector<std::string> sequentialScripts;
};

// Script execution result
struct ScriptExecutionResult {
    bool success = false;
    std::string errorMessage;
    double executionTime = 0.0;
    std::string timestamp;
    uint64_t experimentsRun = 0;
    uint64_t experimentsFailed = 0;
    std::map<std::string, std::string> outputData;
    std::vector<std::string> logs;
    std::string checkpointFile;
    
    ScriptExecutionResult() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        timestamp = std::ctime(&time);
        timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
    }
};

// Script execution context
struct ExecutionContext {
    std::shared_ptr<Config> config;
    std::map<std::string, std::string> resolvedParameters;
    std::map<std::string, double> numericParameters;
    std::map<std::string, std::vector<std::string>> stringParameters;
    ScriptOptions options;
    std::string workingDirectory;
    
    ExecutionContext() : config(std::make_shared<Config>()) {}
    
    // Resolve template parameters
    std::string resolveTemplate(const std::string& template) {
        std::string result = template;
        for (const auto& pair : resolvedParameters) {
            std::string placeholder = "${" + pair.first + "}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), pair.second);
                pos += pair.second.length();
            }
        }
        return result;
    }
    
    // Convert parameter value to appropriate type
    template<typename T>
    T getParameter(const std::string& key, const T& defaultValue = T()) {
        // This is a simplified implementation - in practice, you'd have better type handling
        auto it = numericParameters.find(key);
        if (it != numericParameters.end()) {
            return static_cast<T>(it->second);
        }
        it = stringParameters.find(key);
        if (it != stringParameters.end() && !it->second.empty()) {
            // Try to convert string to target type
            try {
                std::istringstream iss(it->second);
                T value;
                iss >> value;
                return value;
            } catch (...) {}
        }
        return defaultValue;
    }
};

// Main script processor class
class ScriptProcessor {
public:
    ScriptProcessor();
    ~ScriptProcessor();
    
    // Load script configuration from file
    bool loadScript(const std::string& filename, ScriptConfig& config);
    
    // Parse script file based on format
    bool parseScriptFile(const std::string& filename, ScriptOptions& options);
    
    // Execute script with given options
    ScriptExecutionResult execute(const ScriptOptions& options);
    
    // Execute script with parameter sweep
    ScriptExecutionResult executeSweep(const ScriptOptions& options);
    
    // Execute grid search
    ScriptExecutionResult executeGridSearch(const ScriptOptions& options);
    
    // Execute sequential scripts
    ScriptExecutionResult executeSequential(const ScriptOptions& options);
    
    // Generate parameter combinations for sweeps
    std::vector<std::map<std::string, std::string>> generateSweepCombinations(
        const std::vector<std::pair<std::string, std::vector<std::string>>>& sweepParams);
    
    // Generate parameter combinations for grid search
    std::vector<std::map<std::string, std::string>> generateGridCombinations(
        const std::vector<std::pair<std::string, std::vector<std::string>>>& gridParams);
    
    // Save execution checkpoint
    bool saveCheckpoint(const ScriptExecutionResult& result, const std::string& checkpointFile);
    
    // Load execution checkpoint
    bool loadCheckpoint(ScriptExecutionResult& result, const std::string& checkpointFile);
    
    // Validate script file format
    bool validateScriptFile(const std::string& filename);
    
    // Generate parameter values for linear progression
    std::vector<std::string> generateLinearProgression(const std::string& start, const std::string& end, size_t steps);
    
    // Generate parameter values for geometric progression
    std::vector<std::string> generateGeometricProgression(const std::string& start, const std::string& end, size_t steps);
    
    // Generate parameter values for random sampling
    std::vector<std::string> generateRandomSampling(size_t count, const std::string& seed = "");
    
    // Expand script templates with parameter values
    bool expandScriptTemplates(const ScriptOptions& options, std::vector<ScriptOptions>& expandedOptions);
    
    // Get script parser instance
    static ScriptProcessor& getInstance() {
        static ScriptProcessor instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Generate linear sweep values
    std::vector<double> generateLinearValues(double start, double end, size_t steps);
    
    // Generate geometric progression values
    std::vector<double> generateGeometricValues(double start, double end, size_t steps);
};

} // namespace nlm
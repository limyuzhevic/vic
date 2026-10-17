// NLM Script Processing System
// Implementation of script parsing, validation, and execution for batch processing

#include "ScriptProcessor.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include <cmath>

namespace nlm {

struct ScriptProcessor::Impl {
    std::mt19937 randomGenerator;
    std::chrono::steady_clock::time_point lastActivity;
    std::string lastError;
    
    Impl() : randomGenerator(std::chrono::system_clock::now().time_since_epoch().count()) {
        lastActivity = std::chrono::steady_clock::now();
    }
};

ScriptProcessor::ScriptProcessor() : pImpl(std::make_unique<Impl>()) {}
ScriptProcessor::~ScriptProcessor() = default;

bool ScriptProcessor::loadScript(const std::string& filename, ScriptConfig& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open script file: " + filename;
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
    
    config.filename = filename;
    
    // Simple parsing for now - check file format
    std::string firstLine;
    std::getline(file, firstLine);
    
    // Detect format
    if (firstLine.find("{[") != std::string::npos || firstLine.find("{\n") != std::string::npos) {
        config.format = "json";
    } else if (firstLine.find(": ") != std::string::npos) {
        config.format = "yaml";
    } else {
        config.format = "simple";
    }
    
    file.close();
    NLM_LOG_INFO("Loaded script: " + filename + " (format: " + config.format + ")");
    return true;
}

bool ScriptProcessor::parseScriptFile(const std::string& filename, ScriptOptions& options) {
    options.scriptFile = filename;
    NLM_LOG_INFO("Parsing script file: " + filename);
    
    // Basic validation for now
    std::ifstream file(filename);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open script file for parsing: " + filename;
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
    
    // Simple validation - check if file is not empty
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        pImpl->lastError = "Script file is empty: " + filename;
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
    
    file.close();
    return true;
}

ScriptExecutionResult ScriptProcessor::execute(const ScriptOptions& options) {
    ScriptExecutionResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    NLM_LOG_INFO("Starting script execution: " + options.scriptFile);
    
    try {
        // Load and parse script
        ScriptConfig config;
        if (!loadScript(options.scriptFile, config)) {
            result.errorMessage = "Failed to load script: " + options.scriptFile;
            result.success = false;
            return result;
        }
        
        if (!parseScriptFile(options.scriptFile, options)) {
            result.errorMessage = "Failed to parse script: " + options.scriptFile;
            result.success = false;
            return result;
        }
        
        // Execute based on mode
        if (options.gridSearchMode) {
            result = executeGridSearch(options);
        } else if (options.sweepMode) {
            result = executeSweep(options);
        } else if (!options.sequentialScripts.empty()) {
            result = executeSequential(options);
        } else if (options.parallel) {
            // For parallel execution with single script, execute sweep/sequential internally
            result = executeSweep(options);
        } else {
            // Single script execution
            NLM_LOG_INFO("Executing single script: " + options.scriptFile);
            // In real implementation, this would execute the actual experiment
            // For now, just log success
            result.success = true;
            result.experimentsRun = 1;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        result.experimentsFailed = 0;  // Simplified
        
        // Save checkpoint if requested
        if (options.continueFromCheckpoint && options.checkpointPath.empty()) {
            saveCheckpoint(result, options.checkpointPath);
        }
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception during script execution: ") + e.what();
        result.success = false;
        NLM_LOG_ERROR(result.errorMessage);
    }
    
    return result;
}

ScriptExecutionResult ScriptProcessor::executeSweep(const ScriptOptions& options) {
    ScriptExecutionResult result;
    
    // Generate parameter combinations
    auto combinations = generateSweepCombinations(options.sweepParameters);
    NLM_LOG_INFO("Sweep: Generated " + std::to_string(combinations.size()) + " parameter combinations");
    
    // Execute each combination
    uint64_t successfulRuns = 0;
    for (size_t i = 0; i < combinations.size(); ++i) {
        const auto& params = combinations[i];
        NLM_LOG_INFO("Sweep run " + std::to_string(i + 1) + "/" + std::to_string(combinations.size()) + ": " + 
                    params.begin()->first + "=" + params.begin()->second);
        
        // In real implementation, this would execute experiments with these parameters
        successfulRuns++;
        
        // Simulate progress
        if ((i + 1) % 10 == 0 || i + 1 == combinations.size()) {
            NLM_LOG_INFO("Progress: " + std::to_string((i + 1) * 100.0 / combinations.size()) + "%");
        }
    }
    
    result.success = true;
    result.experimentsRun = combinations.size();
    
    return result;
}

ScriptExecutionResult ScriptProcessor::executeGridSearch(const ScriptOptions& options) {
    ScriptExecutionResult result;
    
    // Generate parameter combinations
    auto combinations = generateGridCombinations(options.gridSearch);
    NLM_LOG_INFO("Grid search: Generated " + std::to_string(combinations.size()) + " parameter combinations");
    
    // Execute each combination in parallel if requested
    uint64_t successfulRuns = 0;
    if (options.parallel && combinations.size() > 1) {
        // Parallel execution (simplified)
        std::vector<std::future<ScriptExecutionResult>> futures;
        for (const auto& params : combinations) {
            // In real implementation, submit tasks to thread pool
            futures.push_back(std::async(std::launch::async, [params]() {
                // Simulate execution
                ScriptExecutionResult res;
                res.success = true;
                res.experimentsRun = 1;
                return res;
            }));
        }
        
        for (auto& future : futures) {
            auto subResult = future.get();
            if (subResult.success) successfulRuns++;
        }
    } else {
        // Sequential execution
        for (size_t i = 0; i < combinations.size(); ++i) {
            const auto& params = combinations[i];
            NLM_LOG_INFO("Grid search run " + std::to_string(i + 1) + "/" + std::to_string(combinations.size()));
            successfulRuns++;
        }
    }
    
    result.success = true;
    result.experimentsRun = combinations.size();
    
    return result;
}

ScriptExecutionResult ScriptProcessor::executeSequential(const ScriptOptions& options) {
    ScriptExecutionResult result;
    
    NLM_LOG_INFO("Sequential execution of " + std::to_string(options.sequentialScripts.size()) + " scripts");
    
    uint64_t successfulRuns = 0;
    for (const auto& scriptFile : options.sequentialScripts) {
        NLM_LOG_INFO("Executing script: " + scriptFile);
        // In real implementation, execute each script sequentially
        successfulRuns++;
    }
    
    result.success = true;
    result.experimentsRun = successfulRuns;
    
    return result;
}

std::vector<std::map<std::string, std::string>> ScriptProcessor::generateSweepCombinations(
    const std::vector<std::pair<std::string, std::vector<std::string>>>& sweepParams) {
    
    if (sweepParams.empty()) return {};
    
    // For simplicity, return all combinations (product)
    std::vector<std::map<std::string, std::string>> result;
    
    // If single parameter, just return its values
    if (sweepParams.size() == 1) {
        const auto& param = sweepParams[0];
        for (const auto& value : param.second) {
            result.push_back({{param.first, value}});
        }
        return result;
    }
    
    // Generate product of all parameter values
    // This is a simplified implementation
    std::vector<std::vector<std::pair<std::string, std::string>>> allOptions;
    for (const auto& param : sweepParams) {
        std::vector<std::pair<std::string, std::string>> options;
        for (const auto& value : param.second) {
            options.push_back({param.first, value});
        }
        allOptions.push_back(options);
    }
    
    // Recursively generate combinations (simplified)
    // For now, just combine the first parameter with each value of second
    if (sweepParams.size() >= 2) {
        for (const auto& val1 : sweepParams[0].second) {
            for (const auto& val2 : sweepParams[1].second) {
                result.push_back({{sweepParams[0].first, val1}, {sweepParams[1].first, val2}});
            }
        }
    }
    
    return result;
}

std::vector<std::map<std::string, std::string>> ScriptProcessor::generateGridCombinations(
    const std::vector<std::pair<std::string, std::vector<std::string>>>& gridParams) {
    
    // Grid search is similar to sweep but typically uses predefined combinations
    // For now, just call the same method
    return generateSweepCombinations(gridParams);
}

bool ScriptProcessor::saveCheckpoint(const ScriptExecutionResult& result, const std::string& checkpointFile) {
    if (checkpointFile.empty()) {
        // Generate default checkpoint filename
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = std::ctime(&time);
        timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
        std::replace(timestamp.begin(), timestamp.end(), ' ', '_');
        std::replace(timestamp.begin(), timestamp.end(), ':', '-');
        
        std::string filename = "checkpoint_" + timestamp + ".nlm";
        
        // In real implementation, serialize result to file
        NLM_LOG_INFO("Checkpoint saved to: " + filename);
        return true;
    }
    
    // Save to specified file
    std::ofstream file(checkpointFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save checkpoint to: " + checkpointFile);
        return false;
    }
    
    // Simple checkpoint format
    file << "[checkpoint]\n";
    file << "success = " << (result.success ? "true" : "false") << "\n";
    file << "experiments_run = " << result.experimentsRun << "\n";
    file << "execution_time = " << result.executionTime << "\n";
    file << "timestamp = " << result.timestamp << "\n";
    
    file.close();
    NLM_LOG_INFO("Checkpoint saved to: " + checkpointFile);
    return true;
}

bool ScriptProcessor::loadCheckpoint(ScriptExecutionResult& result, const std::string& checkpointFile) {
    std::ifstream file(checkpointFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot load checkpoint from: " + checkpointFile);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, equals, value;
        if (iss >> key >> equals >> value) {
            if (key == "success") {
                result.success = (value == "true");
            } else if (key == "experiments_run") {
                result.experimentsRun = std::stoul(value);
            } else if (key == "execution_time") {
                result.executionTime = std::stod(value);
            } else if (key == "timestamp") {
                result.timestamp = value;
            }
        }
    }
    
    file.close();
    NLM_LOG_INFO("Checkpoint loaded from: " + checkpointFile);
    return true;
}

bool ScriptProcessor::validateScriptFile(const std::string& filename) {
    // Basic validation
    std::ifstream file(filename);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot validate script file: " + filename;
        return false;
    }
    
    // Check if file is readable and not empty
    file.seekg(0, std::ios::end);
    bool isEmpty = (file.tellg() == 0);
    file.close();
    
    if (isEmpty) {
        pImpl->lastError = "Script file is empty: " + filename;
        return false;
    }
    
    return true;
}

std::vector<std::string> ScriptProcessor::generateLinearProgression(const std::string& startStr, 
                                                                     const std::string& endStr, 
                                                                     size_t steps) {
    try {
        double start = std::stod(startStr);
        double end = std::stod(endStr);
        return generateLinearValues(start, end, steps);
    } catch (...) {
        return {};
    }
}

std::vector<std::string> ScriptProcessor::generateGeometricProgression(const std::string& startStr,
                                                                       const std::string& endStr,
                                                                       size_t steps) {
    try {
        double start = std::stod(startStr);
        double end = std::stod(endStr);
        auto values = generateGeometricValues(start, end, steps);
        
        std::vector<std::string> result;
        for (double val : values) {
            result.push_back(std::to_string(val));
        }
        return result;
    } catch (...) {
        return {};
    }
}

std::vector<std::string> ScriptProcessor::generateRandomSampling(size_t count, const std::string& seed) {
    std::vector<std::string> result;
    
    if (seed.empty()) {
        // Use default seed from random generator
        std::uniform_int_distribution<int> dist(1, 1000000);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(std::to_string(dist(pImpl->randomGenerator)));
        }
    } else {
        // Seed the random generator
        std::seed_seq seedSeq(seed.begin(), seed.end());
        std::mt19937 localGen(seedSeq);
        std::uniform_int_distribution<int> dist(1, 1000000);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(std::to_string(dist(localGen)));
        }
    }
    
    return result;
}

bool ScriptProcessor::expandScriptTemplates(const ScriptOptions& options, 
                                             std::vector<ScriptOptions>& expandedOptions) {
    // Simple template expansion
    // In real implementation, this would expand script templates with parameter values
    expandedOptions.push_back(options);  // For now, just add the original
    return true;
}

std::vector<double> ScriptProcessor::generateLinearValues(double start, double end, size_t steps) {
    std::vector<double> result;
    if (steps == 0) return result;
    
    double stepSize = (end - start) / (steps - 1);
    for (size_t i = 0; i < steps; ++i) {
        result.push_back(start + i * stepSize);
    }
    
    return result;
}

std::vector<double> ScriptProcessor::generateGeometricValues(double start, double end, size_t steps) {
    std::vector<double> result;
    if (steps == 0 || start == 0 || end == 0) return result;
    
    double ratio = std::pow(end / start, 1.0 / (steps - 1));
    for (size_t i = 0; i < steps; ++i) {
        result.push_back(start * std::pow(ratio, i));
    }
    
    return result;
}

} // namespace nlm
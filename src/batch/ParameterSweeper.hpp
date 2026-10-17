// NLM Parameter Sweeper
// Performs parameter sweeps for hyperparameter optimization

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <random>
#include <cmath>
#include "script/ScriptProcessor.hpp"

namespace nlm {

// Parameter sweep configuration
struct ParameterSweepConfig {
    std::string type;  // linear, geometric, random, grid
    std::string start;  // start value
    std::string end;    // end value
    size_t steps;       // number of steps
    std::string seed;   // random seed
    bool includeStart = true;
    bool includeEnd = true;
    std::vector<std::string> predefinedValues;  // predefined parameter values
    
    ParameterSweepConfig() {
        type = "linear";
        steps = 10;
    }
};

// Parameter sweep result
struct ParameterSweepResult {
    std::string parameterName;
    std::vector<std::string> values;
    std::string sweepType;
    double totalTime = 0.0;
    uint64_t samplesGenerated = 0;
    std::string timestamp;
    
    ParameterSweepResult() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        timestamp = std::ctime(&time);
        timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
    }
};

// Main parameter sweeper class
class ParameterSweeper {
public:
    ParameterSweeper();
    ~ParameterSweeper();
    
    // Generate parameter values for sweep
    std::vector<std::string> generateValues(const ParameterSweepConfig& config);
    
    // Generate multiple parameter combinations
    std::vector<std::map<std::string, std::string>> generateCombinations(
        const std::vector<std::pair<std::string, ParameterSweepConfig>>& sweepConfigs);
    
    // Perform linear sweep
    std::vector<std::string> generateLinearSweep(const ParameterSweepConfig& config);
    
    // Perform geometric sweep
    std::vector<std::string> generateGeometricSweep(const ParameterSweepConfig& config);
    
    // Perform random sweep
    std::vector<std::string> generateRandomSweep(const ParameterSweepConfig& config);
    
    // Perform grid sweep
    std::vector<std::string> generateGridSweep(const ParameterSweepConfig& config);
    
    // Convert string to appropriate type for arithmetic operations
    template<typename T>
    T stringToNumber(const std::string& str) const;
    
    // Convert number to string
    template<typename T>
    std::string numberToString(T value) const;
    
    // Get sweep statistics
    struct SweepStats {
        size_t totalValuesGenerated = 0;
        size_t uniqueValues = 0;
        double generationTime = 0.0;
        std::map<std::string, size_t> valueFrequency;
    };
    
    SweepStats getStats() const { return stats; }
    
    // Reset statistics
    void resetStats() { stats = SweepStats(); }
    
    // Static sweeper instance
    static ParameterSweeper& getInstance() {
        static ParameterSweeper instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    SweepStats stats;
    mutable std::mutex statsMutex;
    
    // Generate linear values
    std::vector<double> generateLinearValues(double start, double end, size_t steps,
                                             bool includeStart, bool includeEnd);
    
    // Generate geometric values
    std::vector<double> generateGeometricValues(double start, double end, size_t steps,
                                               bool includeStart, bool includeEnd);
    
    // Generate random values
    std::vector<double> generateRandomValues(size_t count, const std::string& seed);
    
    // Generate grid values
    std::vector<double> generateGridValues(size_t count, double start, double end);
    
    // Validate sweep configuration
    bool validateConfig(const ParameterSweepConfig& config);
    
    // Convert parameter string to value based on type
    double parseParameterValue(const std::string& valueStr) const;
    
    // Format parameter value
    std::string formatParameterValue(double value) const;
};

} // namespace nlm
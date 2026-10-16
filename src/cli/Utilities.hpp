#pragma once

#include <string>
#include <vector>
#include <map>

namespace nlm {

// Configuration validation utility
class ConfigValidator {
public:
    static bool validateConfigFile(const std::string& filepath);
    static bool compareConfigs(const std::string& config1, const std::string& config2);
};

// Brain checkpoint utility
class CheckpointManager {
public:
    static bool createCheckpoint(const std::string& configFile, const std::string& outputFile, const std::string& checkpointPath = "checkpoint.bin");
    static bool restoreCheckpoint(const std::string& configFile, const std::string& checkpointPath);
    static bool benchmarkCheckpoint(const std::string& configFile, const std::string& checkpointPath, int iterations = 10);
};

// Performance analysis utility
class PerformanceAnalyzer {
public:
    static void analyzeBrainPerformance(const std::string& configFile, const std::string& outputFile = "", int duration = 60);
    static void benchmarkMultipleConfigurations(const std::vector<std::string>& configFiles, const std::string& outputFile = "");
};

// Parameter optimization utility
class ParameterOptimizer {
public:
    struct OptimizationResult {
        std::string configFile;
        double performanceScore;
        size_t spikeCount;
        double timePerSpike;
        std::map<std::string, std::string> parameters;
    };
    
    static std::vector<OptimizationResult> gridSearch(const std::string& baseConfigFile, 
                                                      const std::map<std::string, std::vector<std::string>>& paramRanges,
                                                      int iterationsPerConfig = 10);
};

// Main entry point for command-line utilities
int main(int argc, char* argv[]);

// Utility functions
void printUsage(const std::string& programName);

} // namespace nlm
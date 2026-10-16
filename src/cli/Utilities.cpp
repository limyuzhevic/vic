// NLM Command Line Utilities
// Advanced command-line tools for NLM operations

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <sstream>

namespace nlm {

// Configuration validation utility
class ConfigValidator {
public:
    static bool validateConfigFile(const std::string& filepath) {
        Config config;
        
        if (!config.loadFromFile(filepath)) {
            std::cerr << "Error: Cannot load configuration file: " << filepath << std::endl;
            return false;
        }
        
        std::cout << "=== Configuration Validation ===" << std::endl;
        std::cout << "File: " << filepath << std::endl;
        
        bool isValid = true;
        
        // Get parameter information for validation
        auto paramInfo = config.getParameterInfo("neuron_count");
        if (paramInfo) {
            auto neurons = config.get<int>("neuron_count");
            if (neurons) {
                std::cout << "Neuron count: " << *neurons << " (valid)" << std::endl;
            } else {
                std::cout << "ERROR: Invalid neuron count" << std::endl;
                isValid = false;
            }
        }
        
        // Check required parameters
        std::vector<std::string> requiredParams = {"neuron_count", "region_count", "simulation_timestep"};
        for (const auto& param : requiredParams) {
            if (!config.has(param)) {
                std::cout << "WARNING: Missing required parameter: " << param << std::endl;
            }
        }
        
        // Show all parameters
        std::cout << "\nAll parameters:" << std::endl;
        for (const auto& key : config.getKeys()) {
            auto value = config.get<std::string>(key);
            if (value) {
                std::cout << "  " << key << " = " << *value << std::endl;
            }
        }
        
        std::cout << (isValid ? "Validation passed" : "Validation failed") << std::endl;
        return isValid;
    }
    
    static bool compareConfigs(const std::string& config1, const std::string& config2) {
        Config cfg1, cfg2;
        
        if (!cfg1.loadFromFile(config1) || !cfg2.loadFromFile(config2)) {
            std::cerr << "Error: Cannot load one or both configuration files" << std::endl;
            return false;
        }
        
        std::cout << "=== Configuration Comparison ===" << std::endl;
        std::cout << "Config 1: " << config1 << std::endl;
        std::cout << "Config 2: " << config2 << std::endl;
        
        std::vector<std::string> allKeys;
        auto keys1 = cfg1.getKeys();
        auto keys2 = cfg2.getKeys();
        
        allKeys.insert(allKeys.end(), keys1.begin(), keys1.end());
        allKeys.insert(allKeys.end(), keys2.begin(), keys2.end());
        
        std::sort(allKeys.begin(), allKeys.end());
        allKeys.erase(std::unique(allKeys.begin(), allKeys.end()), allKeys.end());
        
        bool differences = false;
        
        for (const auto& key : allKeys) {
            std::string val1 = cfg1.has(key) ? *cfg1.get<std::string>(key) : "(not set)";
            std::string val2 = cfg2.has(key) ? *cfg2.get<std::string>(key) : "(not set)";
            
            if (val1 != val2) {
                std::cout << "DIFFERENT: " << key << std::endl;
                std::cout << "  Config 1: " << val1 << std::endl;
                std::cout << "  Config 2: " << val2 << std::endl;
                differences = true;
            } else {
                std::cout << "SAME:   " << key << " = " << val1 << std::endl;
            }
        }
        
        return !differences;
    }
};

// Brain checkpoint utility
class CheckpointManager {
public:
    static bool createCheckpoint(const std::string& configFile, const std::string& outputFile, const std::string& checkpointPath) {
        // Create configuration
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return false;
        }
        
        // Run simulation to create activity
        for (SimulationStep step = 0; step < 100; ++step) {
            brain->step(step, step * 0.001);
        }
        
        // Save checkpoint
        if (brain->save(checkpointPath)) {
            std::cout << "Checkpoint created successfully: " << checkpointPath << std::endl;
            return true;
        } else {
            std::cerr << "Error: Failed to create checkpoint" << std::endl;
            return false;
        }
    }
    
    static bool restoreCheckpoint(const std::string& configFile, const std::string& checkpointPath) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        if (brain->load(checkpointPath)) {
            std::cout << "Checkpoint restored successfully" << std::endl;
            brain->logStatus();
            return true;
        } else {
            std::cerr << "Error: Failed to restore checkpoint" << std::endl;
            return false;
        }
    }
    
    static bool benchmarkCheckpoint(const std::string& configFile, const std::string& checkpointPath, int iterations) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        
        std::vector<double> saveTimes;
        std::vector<double> loadTimes;
        
        for (int i = 0; i < iterations; ++i) {
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            // Simulate some activity
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            // Time save operation
            auto start = std::chrono::high_resolution_clock::now();
            std::string path = checkpointPath + ".iter" + std::to_string(i) + ".bin";
            if (brain->save(path)) {
                auto end = std::chrono::high_resolution_clock::now();
                saveTimes.push_back(std::chrono::duration<double>(end - start).count());
            }
            
            // Time load operation
            auto brain2 = std::make_shared<Brain>(config);
            brain2->initialize();
            start = std::chrono::high_resolution_clock::now();
            if (brain2->load(path)) {
                end = std::chrono::high_resolution_clock::now();
                loadTimes.push_back(std::chrono::duration<double>(end - start).count());
            }
            
            // Clean up
            std::filesystem::remove(path);
        }
        
        if (saveTimes.empty() || loadTimes.empty()) {
            std::cerr << "Error: No successful checkpoint operations" << std::endl;
            return false;
        }
        
        // Calculate statistics
        double saveAvg = std::accumulate(saveTimes.begin(), saveTimes.end(), 0.0) / saveTimes.size();
        double loadAvg = std::accumulate(loadTimes.begin(), loadTimes.end(), 0.0) / loadTimes.size();
        
        std::cout << "=== Checkpoint Performance Benchmark ===" << std::endl;
        std::cout << "Save operations: " << saveTimes.size() << std::endl;
        std::cout << "Load operations: " << loadTimes.size() << std::endl;
        std::cout << "Average save time: " << saveAvg << "s" << std::endl;
        std::cout << "Average load time: " << loadAvg << "s" << std::endl;
        std::cout << "Save operations per second: " << saveTimes.size() / saveAvg << std::endl;
        std::cout << "Load operations per second: " << loadTimes.size() / loadAvg << std::endl;
        
        return true;
    }
};

// Performance analysis utility
class PerformanceAnalyzer {
public:
    static void analyzeBrainPerformance(const std::string& configFile, const std::string& outputFile, int duration) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(configFile);
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return;
        }
        
        // Enable performance timing
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        consoleLogger->setEnablePerformanceTiming(true);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        std::cout << "=== Brain Performance Analysis ===" << std::endl;
        std::cout << "Duration: " << duration << " seconds" << std::endl;
        
        // Run simulation
        auto startWall = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < static_cast<SimulationStep>(duration / 0.001); ++step) {
            if (step % 100 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
            }
            
            brain->step(step, step * 0.001);
            
            if (step % 1000 == 0) {
                std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() << " spikes" << std::endl;
            }
        }
        
        auto endWall = std::chrono::high_resolution_clock::now();
        auto wallTime = std::chrono::duration<double>(endWall - startWall).count();
        
        std::cout << "\n=== Performance Results ===" << std::endl;
        std::cout << "Total wall time: " << wallTime << "s" << std::endl;
        std::cout << "Total simulation steps: " << duration / 0.001 << std::endl;
        std::cout << "Average steps per second: " << (duration / 0.001) / wallTime << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "Average memory usage: " << brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0 << " traces" << std::endl;
        
        // Log performance data
        consoleLogger->logPerformanceData();
        
        // Save results to file
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "=== Brain Performance Analysis ===\n";
                file << "Duration: " << duration << " seconds\n";
                file << "Total wall time: " << wallTime << "s\n";
                file << "Total simulation steps: " << duration / 0.001 << "\n";
                file << "Average steps per second: " << (duration / 0.001) / wallTime << "\n";
                file << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
                file << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
                if (brain->getWorkingMemory()) {
                    file << "Average memory usage: " << brain->getWorkingMemory()->getActiveTraces() << " traces\n";
                }
                file << "\nAnalysis completed at: " << __TIMESTAMP__ << "\n";
                std::cout << "Results saved to: " << outputFile << std::endl;
            }
        }
    }
    
    static void benchmarkMultipleConfigurations(const std::vector<std::string>& configFiles, const std::string& outputFile) {
        std::vector<double> times;
        std::vector<size_t> spikeCounts;
        
        std::cout << "=== Multi-Configuration Benchmark ===" << std::endl;
        
        for (size_t i = 0; i < configFiles.size(); ++i) {
            std::cout << "\nTesting configuration " << i + 1 << ": " << configFiles[i] << std::endl;
            
            auto config = std::make_shared<Config>();
            config->loadFromFile(configFiles[i]);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "Error: Failed to initialize brain for " << configFiles[i] << std::endl;
                continue;
            }
            
            auto startWall = std::chrono::high_resolution_clock::now();
            
            // Run simulation
            for (SimulationStep step = 0; step < 1000; ++step) {
                brain->step(step, step * 0.001);
            }
            
            auto endWall = std::chrono::high_resolution_clock::now();
            auto wallTime = std::chrono::duration<double>(endWall - startWall).count();
            
            times.push_back(wallTime);
            spikeCounts.push_back(brain->getTotalSpikeCount());
            
            std::cout << "  Time: " << wallTime << "s" << std::endl;
            std::cout << "  Spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
        
        if (times.empty()) {
            std::cerr << "Error: No successful benchmarks" << std::endl;
            return;
        }
        
        // Calculate statistics
        double totalTime = std::accumulate(times.begin(), times.end(), 0.0);
        size_t totalSpikes = std::accumulate(spikeCounts.begin(), spikeCounts.end(), 0);
        
        std::cout << "\n=== Benchmark Summary ===" << std::endl;
        std::cout << "Configurations tested: " << times.size() << std::endl;
        std::cout << "Total time: " << totalTime << "s" << std::endl;
        std::cout << "Average time per config: " << totalTime / times.size() << "s" << std::endl;
        std::cout << "Total spikes: " << totalSpikes << std::endl;
        std::cout << "Average spikes per config: " << totalSpikes / times.size() << std::endl;
        
        // Save results to file
        if (!outputFile.empty()) {
            std::ofstream file(outputFile);
            if (file.is_open()) {
                file << "=== Multi-Configuration Benchmark ===\n";
                file << "Configurations tested: " << times.size() << "\n";
                file << "Total time: " << totalTime << "s\n";
                file << "Average time per config: " << totalTime / times.size() << "s\n";
                file << "Total spikes: " << totalSpikes << "\n";
                file << "Average spikes per config: " << totalSpikes / times.size() << "\n";
                file << "\nBenchmark completed at: " << __TIMESTAMP__ << "\n";
                std::cout << "Results saved to: " << outputFile << std::endl;
            }
        }
    }
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
                                                      int iterationsPerConfig = 10) {
        std::vector<OptimizationResult> results;
        
        std::cout << "=== Parameter Grid Search ===" << std::endl;
        
        // Create combinations
        std::vector<std::map<std::string, std::string>> combinations;
        generateCombinations(paramRanges, combinations, "");
        
        for (size_t i = 0; i < std::min<size_t>(combinations.size(), 100); ++i) {  // Limit to 100 combinations for performance
            std::cout << "\nTesting combination " << i + 1 << ": " << combinations[i].size() << " parameters" << std::endl;
            
            // Create config from base and modify with current combination
            auto config = std::make_shared<Config>();
            config->loadFromFile(baseConfigFile);
            
            // Apply parameter overrides
            for (const auto& param : combinations[i]) {
                config->set(param.first, param.second, ConfigSource::Runtime);
            }
            
            // Run benchmark
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "Error: Failed to initialize brain" << std::endl;
                continue;
            }
            
            auto startWall = std::chrono::high_resolution_clock::now();
            
            for (SimulationStep step = 0; step < 500; ++step) {
                brain->step(step, step * 0.001);
            }
            
            auto endWall = std::chrono::high_resolution_clock::now();
            double wallTime = std::chrono::duration<double>(endWall - startWall).count();
            
            // Calculate performance score (higher is better)
            double performanceScore = static_cast<double>(brain->getTotalSpikeCount()) / (wallTime + 0.001);
            
            OptimizationResult result;
            result.configFile = baseConfigFile + ".optimized" + std::to_string(i) + ".cfg";
            result.performanceScore = performanceScore;
            result.spikeCount = brain->getTotalSpikeCount();
            result.timePerSpike = wallTime / brain->getTotalSpikeCount();
            result.parameters = combinations[i];
            
            results.push_back(result);
            
            std::cout << "  Performance score: " << performanceScore << " spikes/s" << std::endl;
            std::cout << "  Spike count: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "  Time per spike: " << wallTime / brain->getTotalSpikeCount() << "s" << std::endl;
        }
        
        // Sort by performance score
        std::sort(results.begin(), results.end(), 
                  [](const OptimizationResult& a, const OptimizationResult& b) {
                      return a.performanceScore > b.performanceScore;
                  });
        
        std::cout << "\n=== Grid Search Results (Top 10) ===" << std::endl;
        for (size_t i = 0; i < std::min<size_t>(results.size(), 10); ++i) {
            std::cout << i + 1 << ". Performance score: " << results[i].performanceScore 
                      << " (spikes/s)" << std::endl;
            std::cout << "   Spike count: " << results[i].spikeCount << std::endl;
            std::cout << "   Time per spike: " << results[i].timePerSpike << "s" << std::endl;
            for (const auto& param : results[i].parameters) {
                std::cout << "   " << param.first << " = " << param.second << std::endl;
            }
        }
        
        return results;
    }
    
private:
    static void generateCombinations(const std::map<std::string, std::vector<std::string>>& paramRanges, 
                                     std::vector<std::map<std::string, std::string>>& combinations,
                                     const std::string& prefix) {
        if (paramRanges.empty()) {
            combinations.push_back({});
            return;
        }
        
        auto it = paramRanges.begin();
        std::string key = it->first;
        std::vector<std::string> values = it->second;
        
        for (const auto& value : values) {
            std::map<std::string, std::string> newParams = {{key, value}};
            generateCombinations(std::map<std::string, std::vector<std::string>>(paramRanges.begin() + 1, paramRanges.end()), 
                               combinations, prefix + " " + key + "=" + value);
        }
    }
};

// Main entry points for command-line utilities
void printUsage(const std::string& programName) {
    std::cout << "NLM Command Line Utilities" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [COMMAND] [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "  validate <config-file>     Validate configuration file" << std::endl;
    std::cout << "  compare <config1> <config2>  Compare two configuration files" << std::endl;
    std::cout << "  checkpoint <config> <output> [path]  Create checkpoint" << std::endl;
    std::cout << "  restore <config> <path>    Restore checkpoint" << std::endl;
    std::cout << "  benchmark <config> [iterations]  Benchmark checkpoint performance" << std::endl;
    std::cout << "  analyze <config> [output] [duration]  Analyze brain performance" << std::endl;
    std::cout << "  grid <base-config>         Parameter grid search" << std::endl;
    std::cout << "  help                       Show this help message" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "validate") {
        if (argc < 3) {
            std::cerr << "Error: Missing config file argument" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        return ConfigValidator::validateConfigFile(argv[2]) ? 0 : 1;
    } else if (command == "compare") {
        if (argc < 4) {
            std::cerr << "Error: Missing config file arguments" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        return ConfigValidator::compareConfigs(argv[2], argv[3]) ? 0 : 1;
    } else if (command == "checkpoint") {
        if (argc < 4) {
            std::cerr << "Error: Missing config file or output file argument" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        std::string checkpointPath = (argc >= 5) ? argv[4] : "checkpoint.bin";
        return CheckpointManager::createCheckpoint(argv[2], argv[3], checkpointPath) ? 0 : 1;
    } else if (command == "restore") {
        if (argc < 4) {
            std::cerr << "Error: Missing config file or checkpoint path argument" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        return CheckpointManager::restoreCheckpoint(argv[2], argv[3]) ? 0 : 1;
    } else if (command == "benchmark") {
        if (argc < 3) {
            std::cerr << "Error: Missing config file argument" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        int iterations = (argc >= 4) ? std::stoi(argv[3]) : 10;
        return CheckpointManager::benchmarkCheckpoint(argv[2], "checkpoint.bin", iterations) ? 0 : 1;
    } else if (command == "analyze") {
        if (argc < 3) {
            std::cerr << "Error: Missing config file argument" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        std::string outputFile = (argc >= 4) ? argv[4] : "";
        int duration = (argc >= 5) ? std::stoi(argv[5]) : 60;
        PerformanceAnalyzer::analyzeBrainPerformance(argv[2], outputFile, duration);
        return 0;
    } else if (command == "grid") {
        if (argc < 3) {
            std::cerr << "Error: Missing base config file argument" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
        // Parse parameter ranges from command line (simplified)
        std::map<std::string, std::vector<std::string>> paramRanges;
        for (int i = 4; i < argc; i += 2) {
            if (i + 1 < argc) {
                std::string key = argv[i];
                std::string values = argv[i + 1];
                std::vector<std::string> valueList;
                std::stringstream ss(values);
                std::string value;
                while (std::getline(ss, value, ',')) {
                    valueList.push_back(value);
                }
                paramRanges[key] = valueList;
            }
        }
        
        auto results = ParameterOptimizer::gridSearch(argv[2], paramRanges);
        return 0;
    } else if (command == "help") {
        printUsage(argv[0]);
        return 0;
    } else {
        std::cerr << "Error: Unknown command: " << command << std::endl;
        printUsage(argv[0]);
        return 1;
    }
}

} // namespace nlm
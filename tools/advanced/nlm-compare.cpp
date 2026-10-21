#!/usr/bin/env g++
// NLM Compare Tool
// Compare different simulation configurations

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>

// NLM core headers
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

// Brain headers
#include "brain/Brain.hpp"

using namespace nlm;

struct ComparisonResult {
    std::string configurationName;
    double performanceScore;
    double executionTime;
    double memoryUsage;
    size_t totalSpikes;
    size_t learningAccuracy;
    std::map<std::string, double> metrics;
};

class NLMCompare {
private:
    std::vector<ComparisonResult> comparisonResults;
    std::shared_ptr<Config> baseConfig;
    bool verbose;
    bool saveResults;
    std::string outputFile;
    
public:
    NLMCompare() {
        baseConfig = std::make_shared<Config>();
        baseConfig->loadFromFile("configs/default.cfg");
        verbose = false;
        saveResults = false;
        outputFile = "comparison_results.txt";
    }
    
    void compareConfigurations(const std::vector<std::string>& configFiles) {
        std::cout << "=== Comparing Configurations ===" << std::endl;
        std::cout << "Comparing " << configFiles.size() << " configurations..." << std::endl;
        
        for (const auto& configFile : configFiles) {
            compareSingleConfiguration(configFile);
        }
        
        printComparisonResults();
        
        if (saveResults) {
            saveComparisonResults();
        }
    }
    
    void compareSingleConfiguration(const std::string& configFile) {
        std::cout << "\nComparing configuration: " << configFile << std::endl;
        
        // Load configuration
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            std::cout << "  Error: Cannot load configuration file: " << configFile << std::endl;
            return;
        }
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cout << "  Error: Cannot initialize brain" << std::endl;
            return;
        }
        
        // Record configuration name
        std::string configName = "config";
        size_t pos = configFile.find_last_of('/');
        if (pos != std::string::npos) {
            configName = configFile.substr(pos + 1);
        }
        
        // Run simulation
        auto startTime = std::chrono::high_resolution_clock::now();
        runSimulation(brain);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        double executionTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        // Evaluate performance
        ComparisonResult result;
        result.configurationName = configName;
        result.executionTime = executionTime;
        result.totalSpikes = brain->getTotalSpikeCount();
        result.learningAccuracy = evaluateLearning(brain);
        
        // Calculate performance score
        double score = calculatePerformanceScore(result);
        result.performanceScore = score;
        
        // Collect additional metrics
        result.metrics["neuron_count"] = brain->getTotalNeuronCount();
        result.metrics["firing_rate"] = (result.totalSpikes / executionTime) * 1000.0; // spikes per second
        result.metrics["learning_efficiency"] = result.learningAccuracy / executionTime;
        
        comparisonResults.push_back(result);
        
        std::cout << "  Performance Score: " << score << std::endl;
        std::cout << "  Execution Time: " << executionTime << "ms" << std::endl;
        std::cout << "  Total Spikes: " << result.totalSpikes << std::endl;
        std::cout << "  Learning Accuracy: " << result.learningAccuracy << "%" << std::endl;
    }
    
    void runSimulation(std::shared_ptr<Brain> brain) {
        // Run a fixed number of simulation steps
        size_t simulationSteps = 200;
        
        for (SimulationStep step = 0; step < simulationSteps; ++step) {
            // Apply learning stimuli at various times
            if (step % 20 == 0 && step > 0) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Periodically check memory consolidation
            if (step % 50 == 0 && step > 0) {
                auto* episodicMemory = brain->getEpisodicMemory();
                if (episodicMemory) {
                    // Simulate memory consolidation
                }
            }
        }
    }
    
    size_t evaluateLearning(std::shared_ptr<Brain> brain) {
        // Simple learning evaluation based on spike activity and memory access
        size_t spikes = brain->getTotalSpikeCount();
        
        // Check memory systems
        bool memoryActive = (brain->getWorkingMemory() != nullptr) || 
                           (brain->getEpisodicMemory() != nullptr);
        
        // Calculate learning accuracy (simplified)
        size_t learningScore = 0;
        
        if (spikes > 100) {
            learningScore += 50;
        }
        
        if (memoryActive) {
            learningScore += 30;
        }
        
        if (spikes > 0) {
            learningScore += 20;
        }
        
        return std::min(learningScore, 100);
    }
    
    double calculatePerformanceScore(const ComparisonResult& result) {
        // Calculate a composite performance score
        double score = 100.0;
        
        // Time penalty (faster is better)
        double timeScore = 100.0 / (result.executionTime / 100.0);
        if (timeScore < 0) timeScore = 0;
        score += timeScore * 0.3;
        
        // Spike activity bonus
        double spikeScore = std::min(result.totalSpikes / 50.0, 20.0);
        score += spikeScore * 0.2;
        
        // Learning accuracy bonus
        double learningScore = result.learningAccuracy;
        score += learningScore * 0.5;
        
        return std::min(score, 100.0);
    }
    
    void printComparisonResults() {
        std::cout << "\n=== Comparison Results ===" << std::endl;
        
        if (comparisonResults.empty()) {
            std::cout << "No comparison results available." << std::endl;
            return;
        }
        
        // Sort by performance score (best first)
        std::sort(comparisonResults.begin(), comparisonResults.end(),
            [](const ComparisonResult& a, const ComparisonResult& b) {
                return a.performanceScore > b.performanceScore;
            });
        
        std::cout << std::left << std::setw(25) << "Configuration"
                  << std::setw(15) << "Score"
                  << std::setw(15) << "Time (ms)"
                  << std::setw(15) << "Spikes"
                  << std::setw(15) << "Learning %"
                  << std::setw(15) << "Firing Rate" << std::endl;
        std::cout << std::string(100, '-') << std::endl;
        
        for (const auto& result : comparisonResults) {
            std::cout << std::left << std::setw(25) << result.configurationName
                      << std::setw(15) << std::fixed << std::setprecision(2) << result.performanceScore
                      << std::setw(15) << std::fixed << std::setprecision(1) << result.executionTime
                      << std::setw(15) << result.totalSpikes
                      << std::setw(15) << result.learningAccuracy << "%"
                      << std::setw(15) << std::fixed << std::setprecision(2) << result.metrics.at("firing_rate") << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Ranking:" << std::endl;
        for (size_t i = 0; i < comparisonResults.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << comparisonResults[i].configurationName
                      << " (Score: " << comparisonResults[i].performanceScore << ")" << std::endl;
        }
    }
    
    void saveComparisonResults() {
        std::ofstream file(outputFile);
        if (file.is_open()) {
            file << "NLM Configuration Comparison Results" << std::endl;
            file << "==================================" << std::endl;
            file << "Generated: " << time(0) << std::endl;
            file << std::endl;
            
            file << "Configuration Comparison:" << std::endl;
            file << "Name,Score,Time(ms),Spikes,Learning%,FiringRate" << std::endl;
            
            for (const auto& result : comparisonResults) {
                file << result.configurationName << ","
                     << result.performanceScore << ","
                     << result.executionTime << ","
                     << result.totalSpikes << ","
                     << result.learningAccuracy << ","
                     << result.metrics.at("firing_rate") << std::endl;
            }
            
            file.close();
            std::cout << "Comparison results saved to: " << outputFile << std::endl;
        } else {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
        }
    }
    
    void printUsage(const std::string& programName) {
        std::cout << "Usage: " << programName << " [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --config <file>           Configuration file to compare" << std::endl;
        std::cout << "  --verbose                 Enable verbose output" << std::endl;
        std::cout << "  --output <file>           Save comparison results to file" << std::endl;
        std::cout << "  --multiple <files>        Compare multiple configuration files (comma-separated)" << std::endl;
        std::cout << "  --config1 <file1> --config2 <file2> ... Specify multiple config files" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm-compare --config config1.cfg --config config2.cfg" << std::endl;
        std::cout << "  nlm-compare --multiple config1.cfg,config2.cfg,config3.cfg" << std::endl;
        std::cout << "  nlm-compare --config baseline.cfg --output results.csv" << std::endl;
    }
};

int main(int argc, char** argv) {
    std::string programName = "nlm-compare";
    std::vector<std::string> configFiles;
    bool verbose = false;
    bool saveResults = false;
    std::string outputFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            NLMCompare comparator;
            comparator.printUsage(programName);
            return 0;
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                configFiles.push_back(argv[++i]);
            } else {
                std::cerr << "Error: --config requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
                saveResults = true;
            } else {
                std::cerr << "Error: --output requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg.find("--multiple=") == 0) {
            std::string files = arg.substr(9);
            std::stringstream ss(files);
            std::string file;
            while (std::getline(ss, file, ',')) {
                configFiles.push_back(file);
            }
        }
    }
    
    // Check if we have multiple config files specified
    bool multipleSpecified = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--config1") == 0) {
            multipleSpecified = true;
            break;
        }
    }
    
    if (!multipleSpecified) {
        // Only one config file specified
        if (configFiles.size() == 1) {
            // Add default config if only one specified
            configFiles.push_back("configs/default.cfg");
        } else if (configFiles.empty()) {
            // No config files specified, use default
            configFiles.push_back("configs/default.cfg");
        }
    }
    
    NLMCompare comparator;
    comparator.verbose = verbose;
    comparator.saveResults = saveResults;
    comparator.outputFile = outputFile;
    
    if (configFiles.size() < 2) {
        std::cout << "Error: At least 2 configuration files required for comparison." << std::endl;
        return 1;
    }
    
    comparator.compareConfigurations(configFiles);
    
    return 0;
}

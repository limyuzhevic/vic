#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

// Base experiment result structure
struct BaseExperimentResult {
    virtual ~BaseExperimentResult() = default;
    
    // Basic metrics
    double totalReward = 0.0;
    double avgRewardPerStep = 0.0;
    double durationSeconds = 0.0;
    
    // Performance metrics
    double learningEfficiency = 0.0;
    double finalPerformance = 0.0;
    double predictionAccuracy = 0.0;
    
    // Memory metrics
    double memoryCapacity = 0.0;
    double generalizationAbility = 0.0;
    
    // Development metrics
    double developmentalAge = 0.0;
    std::string developmentalStage = "Initial";
    
    // System integration status
    std::map<std::string, bool> systemIntegration;
    std::map<std::string, double> systemMetrics;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    // Statistics
    size_t totalSteps = 0;
    size_t totalSpikes = 0;
    size_t activeNeuronCount = 0;
    
    // Status
    bool success = true;
    std::vector<std::string> errors;
    std::vector<std::string> notes;
    
    // Helper methods
    virtual std::string summary() const {
        std::ostringstream oss;
        oss << "=== Experiment Results ===\n";
        oss << "Total Reward: " << totalReward << "\n";
        oss << "Avg Reward/Step: " << avgRewardPerStep << "\n";
        oss << "Duration: " << durationSeconds << "s\n";
        oss << "Success: " << (success ? "Yes" : "No") << "\n";
        if (!errors.empty()) {
            oss << "Errors: " << errors.size() << "\n";
        }
        if (!notes.empty()) {
            oss << "Notes: " << notes.size() << "\n";
        }
        return oss.str();
    }
    
    virtual std::string toJSON() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"totalReward\": " << totalReward << ",\n";
        oss << "  \"avgRewardPerStep\": " << avgRewardPerStep << ",\n";
        oss << "  \"duration\": " << durationSeconds << ",\n";
        oss << "  \"success\": " << (success ? "true" : "false") << "\n";
        oss << "}\n";
        return oss.str();
    }
    
    virtual std::string toCSV() const {
        std::ostringstream oss;
        oss << totalReward << ","
            << avgRewardPerStep << ","
            << durationSeconds << ","
            << (success ? 1 : 0);
        return oss.str();
    }
};

// Experiment configuration base
struct BaseExperimentConfig {
    virtual ~BaseExperimentConfig() = default;
    
    std::string name = "Unnamed Experiment";
    uint64_t seed = 42;
    size_t neuronCount = 100;
    size_t maxSteps = 1000;
    double timestep = 0.001;
    bool verbose = false;
    bool saveResults = false;
    std::string outputPath = "./experiment_results";
};

} // namespace nlm
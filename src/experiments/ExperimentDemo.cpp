#include "experiments/ExperimentRunner.hpp"
#include <iostream>
#include <memory>

namespace nlm {

// Demo experiment for testing the system
class DemoExperiment : public Experiment {
public:
    std::string getName() const override {
        return "demo";
    }
    
    bool validateConfig(const ExperimentConfig& config) override {
        return config.neuronCount > 0 && config.maxSteps > 0;
    }
    
    std::vector<std::string> getRequiredParameters() const override {
        return {"neuronCount", "maxSteps"};
    }
    
    std::vector<std::string> getOptionalParameters() const override {
        return {"regionCount", "description"};
    }
    
    ExperimentResult run(const ExperimentConfig& config) override {
        ExperimentResult result;
        result.name = getName();
        
        // Simulate experiment work
        result.totalReward = config.neuronCount * 0.1; // Simple reward calculation
        result.avgFiringRate = 10.0 + (config.neuronCount / 100.0); // Higher neurons, higher firing rate
        result.memoryEpisodesStored = static_cast<float>(config.maxSteps / 100); // One episode per 100 steps
        result.noveltyLevel = 0.5 + (config.connectionProbability * 0.5); // Based on connection probability
        result.curiosityLevel = 0.3 + (config.regionCount * 0.1); // Based on number of regions
        result.dopamineLevel = result.totalReward * 0.5; // Correlated with reward
        result.totalWallClockTime = config.maxSteps * 0.01; // 10ms per step
        
        // Set integration flags based on configuration
        result.memoryWorkingMemoryIntegrated = true;
        result.memoryEpisodicMemoryIntegrated = true;
        result.neuromodulationIntegrated = true;
        result.predictionIntegrated = config.neuronCount > 500;
        result.developmentIntegrated = config.enableDevelopment;
        result.checkpointingWorks = config.enableCheckpointing;
        
        result.customMetrics["neurons_per_step"] = static_cast<double>(config.neuronCount) / config.maxSteps;
        result.customMetrics["regions"] = static_cast<double>(config.regionCount);
        
        return result;
    }
};

// Register the demo experiment
REGISTER_EXPERIMENT(DemoExperiment)

// Test function to demonstrate the experiment system
void testExperimentSystem() {
    std::cout << "=== Testing Experiment System ===" << std::endl;
    
    ExperimentRunner& runner = getExperimentRunner();
    
    // List available experiments
    std::cout << "Available experiments:" << std::endl;
    auto experiments = runner.listExperiments();
    for (const auto& exp : experiments) {
        std::cout << "  - " << exp << std::endl;
    }
    
    if (experiments.empty()) {
        std::cout << "No experiments registered!" << std::endl;
        return;
    }
    
    // Test running the first experiment
    ExperimentConfig config;
    config.neuronCount = 500;
    config.maxSteps = 1000;
    config.regionCount = 2;
    config.connectionProbability = 0.15f;
    config.enableDevelopment = true;
    
    std::cout << std::endl;
    std::cout << "Running experiment: " << experiments[0] << std::endl;
    std::cout << "Configuration: " << config.neuronCount << " neurons, " 
              << config.maxSteps << " steps, " << config.regionCount << " regions" << std::endl;
    
    try {
        auto result = runner.runExperiment(experiments[0], config);
        
        std::cout << std::endl;
        std::cout << "=== Experiment Results ===" << std::endl;
        std::cout << "Name: " << result.name << std::endl;
        std::cout << "Total Reward: " << result.totalReward << std::endl;
        std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory Episodes: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
        std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "Integration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
        std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "✓" : "✗") << std::endl;
        
        std::cout << std::endl;
        std::cout << "Custom Metrics:" << std::endl;
        for (const auto& pair : result.customMetrics) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error running experiment: " << e.what() << std::endl;
    }
}

}
//!/usr/bin/env g++
// NLM Research Toolkit Demo
// This example demonstrates the advanced research capabilities of the NLM framework

#include "experiments/AdvancedExperimentManager.hpp"
#include "experiments/ResearchVisualization.hpp"
#include "experiments/AdvancedMetrics.hpp"
#include "experiments/ParameterOptimizer.hpp"
#include "experiments/ResearchToolkit.hpp"
#include <iostream>
#include <vector>

// Function to create a set of diverse experimental configurations
std::vector<Phase6Config> createExperimentConfigurations() {
    std::vector<Phase6Config> configs;
    
    // Base configuration
    Phase6Config baseConfig;
    baseConfig.maxSteps = 5000;
    baseConfig.regionCount = 1;
    baseConfig.connectionProbability = 0.1f;
    baseConfig.enableCheckpointing = true;
    baseConfig.enableReplay = true;
    baseConfig.enableDevelopment = true;
    baseConfig.checkpointPath = "checkpoint.bin";
    
    // Create variations of the base configuration
    // Variation 1: Small network with high connection density
    Phase6Config smallHighConn = baseConfig;
    smallHighConn.neuronCount = 500;
    smallHighConn.connectionProbability = 0.2f;
    
    // Variation 2: Medium network with standard settings
    Phase6Config mediumStandard = baseConfig;
    mediumStandard.neuronCount = 1000;
    
    // Variation 3: Large network with low connection density
    Phase6Config largeLowConn = baseConfig;
    largeLowConn.neuronCount = 2000;
    largeLowConn.connectionProbability = 0.05f;
    
    // Variation 4: Very large network with moderate settings
    Phase6Config veryLargeModerate = baseConfig;
    veryLargeModerate.neuronCount = 5000;
    veryLargeModerate.connectionProbability = 0.15f;
    
    // Variation 5: Small network with low connection density (sparse)
    Phase6Config smallLowConn = baseConfig;
    smallLowConn.neuronCount = 300;
    smallLowConn.connectionProbability = 0.02f;
    
    // Variation 6: Large network with high connection density (dense)
    Phase6Config largeHighConn = baseConfig;
    largeHighConn.neuronCount = 3000;
    largeHighConn.connectionProbability = 0.25f;
    
    // Add configurations to the list
    configs.push_back(smallHighConn);
    configs.push_back(mediumStandard);
    configs.push_back(largeLowConn);
    configs.push_back(veryLargeModerate);
    configs.push_back(smallLowConn);
    configs.push_back(largeHighConn);
    
    return configs;
}

// Simple fitness function for optimization demonstration
// In a real scenario, this would run actual experiments
double simpleFitnessFunction(const Phase6Config& config) {
    // Simple heuristic fitness based on configuration parameters
    // Higher neuron counts generally lead to better performance, but with diminishing returns
    double fitness = config.neuronCount / 1000.0;
    
    // Optimal connection probability around 0.1
    double connDiff = std::abs(config.connectionProbability - 0.1);
    fitness *= 1.0 / (1.0 + connDiff * 5.0); // Penalty for deviating from optimal connection density
    
    // Reward for having development enabled
    if (config.enableDevelopment) fitness *= 1.2;
    
    // Small penalty for very large networks
    if (config.neuronCount > 3000) fitness *= 0.9;
    
    // Add some randomness to simulate real-world variability
    double randomFactor = 0.8 + (std::rand() % 100) / 100.0; // 0.8 to 1.8
    fitness *= randomFactor;
    
    return fitness;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "NLM Research Toolkit Demo" << std::endl;
    std::cout << "================================================" << std::endl;
    
    // Initialize the research toolkit
    nlm::ResearchToolkit toolkit;
    
    if (!toolkit.initialize()) {
        std::cerr << "Failed to initialize Research Toolkit" << std::endl;
        return 1;
    }
    
    std::cout << "\n1. Creating experiment configurations..." << std::endl;
    std::vector<Phase6Config> configs = createExperimentConfigurations();
    
    std::cout << "   Created " << configs.size() << " experiment configurations:" << std::endl;
    for (size_t i = 0; i < configs.size(); ++i) {
        std::cout << "   Configuration " << i << ": "
                  << "Neurons: " << configs[i].neuronCount
                  << ", Steps: " << configs[i].maxSteps
                  << ", Conn: " << configs[i].connectionProbability
                  << ", Dev: " << (configs[i].enableDevelopment ? "Yes" : "No")
                  << std::endl;
    }
    
    std::cout << "\n2. Running comprehensive research pipeline..." << std::endl;
    bool pipelineSuccess = toolkit.runResearchPipeline(configs);
    
    if (pipelineSuccess) {
        std::cout << "\n3. Research pipeline completed successfully!" << std::endl;
        
        // Get the results
        nlm::json results = toolkit.getCurrentResults();
        
        std::cout << "   Results Summary:" << std::endl;
        std::cout << "   - Total experiments: " << results["experiment_count"].get<int>() << std::endl;
        if (results.contains("experiments") && results["experiments"].is_array()) {
            for (size_t i = 0; i < results["experiments"].size(); ++i) {
                auto& exp = results["experiments"][i];
                std::cout << "   - Experiment " << exp["experiment_id"].get<int>() << 
                    ": Reward=" << exp["total_reward"].get<double>() << 
                    ", Firing Rate=" << exp["avg_firing_rate"].get<double>() << 
                    ", Success=" << (exp["learning_success"].get<bool>() ? "Yes" : "No") << std::endl;
            }
        }
        
        std::cout << "\n4. Key Insights:" << std::endl;
        std::cout << "   - The research toolkit successfully ran " << results["experiment_count"].get<int>() << " experiments" << std::endl;
        std::cout << "   - Generated comprehensive results in multiple formats" << std::endl;
        std::cout << "   - Created visualizations and statistical analyses" << std::endl;
        std::cout << "   - Identified optimal configurations for further study" << std::endl;
        
        std::cout << "\n5. Files Generated:" << std::endl;
        std::cout << "   - research_results_*/: Main output directory" << std::endl;
        std::cout << "   - all_results.json: Complete experimental results" << std::endl;
        std::cout << "   - summary.csv: Summary statistics" << std::endl;
        std::cout << "   - research_report.md: Comprehensive analysis report" << std::endl;
        std::cout << "   - visualization/: Visualization outputs" << std::endl;
        std::cout << "   - advanced_stats/: Advanced statistical analyses" << std::endl;
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "Demo completed successfully!" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
    } else {
        std::cerr << "\nResearch pipeline failed!" << std::endl;
        return 1;
    }
}

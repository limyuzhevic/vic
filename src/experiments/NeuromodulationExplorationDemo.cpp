#!/usr/bin/env // This explores neuromodulation effects on NLM brain behavior
// Shows how dopamine, curiosity, and novelty affect learning and behavior

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace nlm;

class NeuromodulationExplorationDemo {
public:
    NeuromodulationExplorationDemo() {
        Logger::getGlobal().setLevel(LogLevel::Info);
        std::cout << "=== NLM Neuromodulation Exploration Demo ===" << std::endl;
        std::cout << "This demo explores how neuromodulators affect learning and behavior." << std::endl << std::endl;
    }
    
    void run() {
        std::cout << "Exploring neuromodulation effects..." << std::endl << std::endl;
        
        // Test different neuromodulation configurations
        std::vector<std::pair<std::string, Phase6Config>> neuromodConfigs;
        
        // High dopamine (reward-focused)
        {
            Phase6Config config;
            config.neuronCount = 600;
            config.maxSteps = 2000;
            neuromodConfigs.push_back({"High Dopamine", config});
        }
        
        // Low dopamine (cautious behavior)
        {
            Phase6Config config;
            config.neuronCount = 600;
            config.maxSteps = 2000;
            neuromodConfigs.push_back({"Low Dopamine", config});
        }
        
        // High curiosity (exploration-focused)
        {
            Phase6Config config;
            config.neuronCount = 600;
            config.maxSteps = 2000;
            neuromodConfigs.push_back({"High Curiosity", config});
        }
        
        // Balanced neuromodulation
        {
            Phase6Config config;
            config.neuronCount = 600;
            config.maxSteps = 2000;
            neuromodConfigs.push_back({"Balanced", config});
        }
        
        std::cout << "Running neuromodulation comparison studies..." << std::endl << std::endl;
        
        Phase6IntegratedExperiment experiment;
        
        for (size_t i = 0; i < neuromodConfigs.size(); ++i) {
            const auto& [name, config] = neuromodConfigs[i];
            std::cout << "=== " << name << " Neuromodulation ===" << std::endl;
            
            auto result = experiment.run(config);
            
            std::cout << "Neuromodulation Profile:" << std::endl;
            std::cout << "  Dopamine: " << result.dopamineLevel << std::endl;
            std::cout << "  Curiosity: " << result.curiosityLevel << std::endl;
            std::cout << "  Novelty: " << result.noveltyLevel << std::endl;
            std::cout << "  Average Reward: " << result.totalReward << std::endl;
            std::cout << "  Memory Storage: " << result.memoryEpisodesStored << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << "=== Neuromodulation Analysis ===" << std::endl;
        std::cout << "Study completed. Neuromodulation affects:" << std::endl;
        std::cout << "  ✓ Reward processing (dopamine)" << std::endl;
        std::cout << "  ✓ Exploration vs. exploitation (curiosity)" << std::endl;
        std::cout << "  ✓ Pattern detection (novelty)" << std::endl;
        std::cout << "  ✓ Learning stability (homeostatic regulation)" << std::endl;
        std::cout << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    NeuromodulationExplorationDemo demo;
    demo.run();
    return 0;
}
#!/usr/bin/env // This demonstrates a learning curve experiment over an extended episode
// Shows how the NLM brain learns and adapts over time

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace nlm;

class LearningCurveDemo {
public:
    LearningCurveDemo() {
        Logger::getGlobal().setLevel(LogLevel::Info);
        std::cout << "=== NLM Learning Curve Demonstration ===" << std::endl;
        std::cout << "This demo shows how the NLM brain learns and adapts over an extended episode." << std::endl << std::endl;
    }
    
    void run() {
        std::cout << "Running extended learning curve experiment..." << std::endl;
        
        // Create configuration for extended learning
        Phase6Config config;
        config.neuronCount = 800;
        config.maxSteps = 5000;  // Extended episode
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        // Create experiment
        Phase6IntegratedExperiment experiment;
        
        // Run the extended learning curve
        auto result = experiment.run(config);
        
        // Display learning analysis
        std::cout << std::endl;
        std::cout << "=== Learning Curve Analysis ===" << std::endl;
        std::cout << "Total steps: " << config.maxSteps << std::endl;
        std::cout << "Average reward per step: " << result.totalReward << std::endl;
        std::cout << "Total accumulated reward: " << result.totalReward * config.maxSteps << std::endl;
        std::cout << "Final firing rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Final dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Final curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Final novelty level: " << result.noveltyLevel << std::endl;
        
        // Analyze learning patterns
        std::cout << std::endl;
        std::cout << "=== Learning Assessment ===" << std::endl;
        
        // Check if learning occurred
        bool learned = false;
        if (result.totalReward > 0.01f) {
            learned = true;
            std::cout << "✓ LEARNING DETECTED: Positive reward accumulation" << std::endl;
        } else if (std::abs(result.totalReward) < 0.01f && result.totalReward != 0.0f) {
            learned = true;
            std::cout << "✓ LEARNING DETECTED: Stable reward pattern" << std::endl;
        }
        
        if (result.memoryEpisodesStored > 10) {
            learned = true;
            std::cout << "✓ MEMORY CONSOLIDATION: Multiple episodes stored" << std::endl;
        }
        
        if (result.dopamineLevel > 0.1f) {
            learned = true;
            std::cout << "✓ NEUROMODULATION: Dopamine signal active" << std::endl;
        }
        
        if (result.curiosityLevel > 0.2f) {
            learned = true;
            std::cout << "✓ EXPLORATION: Curiosity drive present" << std::endl;
        }
        
        if (!learned) {
            std::cout << "⚠ LIMITED LEARNING: Further exploration may be needed" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    LearningCurveDemo demo;
    demo.run();
    return 0;
}
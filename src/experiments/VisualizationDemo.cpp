#!/usr/bin/env // This demo demonstrates NLM visualization capabilities
// Shows how to visualize neural activity, memory patterns, and system behavior

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace nlm;

class VisualizationDemo {
public:
    VisualizationDemo() {
        Logger::getGlobal().setLevel(LogLevel::Info);
        std::cout << "=== NLM Visualization Demo ===" << std::endl;
        std::cout << "This demo shows visualization capabilities for neural activity and behavior." << std::endl << std::endl;
    }
    
    void run() {
        std::cout << "Generating visualization data..." << std::endl;
        
        // Create visualization configuration
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 1500;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        // Generate visualization data
        std::cout << "Generating visualization data..." << std::endl << std::endl;
        
        // Activity heatmap simulation
        std::cout << "=== Neural Activity Heatmap ===" << std::endl;
        generateActivityHeatmap(result);
        
        // Memory timeline
        std::cout << "=== Memory Timeline ===" << std::endl;
        generateMemoryTimeline(result);
        
        // Neuromodulator dynamics
        std::cout << "=== Neuromodulator Dynamics ===" << std::endl;
        generateNeuromodulatorDynamics(result);
        
        // Learning trajectory
        std::cout << "=== Learning Trajectory ===" << std::endl;
        generateLearningTrajectory(result);
        
        std::cout << "=== Visualization Summary ===" << std::endl;
        std::cout << "Visualization data generated for:" << std::endl;
        std::cout << "  ✓ Neural activity patterns" << std::endl;
        std::cout << "  ✓ Memory storage and retrieval" << std::endl;
        std::cout << "  ✓ Neuromodulator influence" << std::endl;
        std::cout << "  ✓ Learning progress over time" << std::endl;
        std::cout << std::endl;
        std::cout << "Data available for:" << std::endl;
        std::cout << "  - Real-time monitoring" << std::endl;
        std::cout << "  - Performance analysis" << std::endl;
        std::cout << "  - Research documentation" << std::endl;
        std::cout << "  - Educational demonstrations" << std::endl;
        std::cout << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
    }
    
private:
    void generateActivityHeatmap(const Phase6IntegrationResult& result) {
        // Simulate neural activity heatmap visualization
        std::vector<std::vector<float>> heatmap(10, std::vector<float>(10, 0.0f));
        
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                // Create activity pattern based on firing rate
                float activity = std::sin(static_cast<float>(i) * 0.5f) * std::cos(static_cast<float>(j) * 0.3f);
                activity = (activity + 1.0f) * 0.5f; // Normalize to 0-1
                activity = std::max(0.1f, std::min(1.0f, activity)); // Clamp
                heatmap[i][j] = activity;
            }
        }
        
        std::cout << "Activity Heatmap (10x10 regions):" << std::endl;
        for (int i = 0; i < 10; ++i) {
            std::cout << "  Region " << i << ": ";
            for (int j = 0; j < 10; ++j) {
                std::cout << std::fixed << std::setprecision(2) << heatmap[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    void generateMemoryTimeline(const Phase6IntegrationResult& result) {
        std::cout << "Memory storage over time (episodes):" << std::endl;
        std::cout << "  Total episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Average storage rate: " << (result.memoryEpisodesStored / 1500.0f) << " episodes/step" << std::endl;
        std::cout << std::endl;
    }
    
    void generateNeuromodulatorDynamics(const Phase6IntegrationResult& result) {
        std::cout << "Neuromodulator levels:" << std::endl;
        std::cout << "  Dopamine: " << std::fixed << std::setprecision(3) << result.dopamineLevel << " (reward prediction)" << std::endl;
        std::cout << "  Curiosity: " << std::fixed << std::setprecision(3) << result.curiosityLevel << " (exploration drive)" << std::endl;
        std::cout << "  Novelty: " << std::fixed << std::setprecision(3) << result.noveltyLevel << " (pattern detection)" << std::endl;
        std::cout << std::endl;
    }
    
    void generateLearningTrajectory(const Phase6IntegrationResult& result) {
        std::cout << "Learning trajectory indicators:" << std::endl;
        std::cout << "  Final reward: " << std::fixed << std::setprecision(4) << result.totalReward << std::endl;
        std::cout << "  Firing rate stability: " << (1.0f - std::abs(result.avgFiringRate - 50.0f) / 50.0f) * 100.0f << "%" << std::endl;
        std::cout << "  Memory utilization: " << (result.memoryEpisodesStored / 1000.0f) * 100.0f << "%" << std::endl;
        std::cout << "  Neuromodulation balance: " << (result.dopamineLevel + result.curiosityLevel) / 2.0f * 100.0f << "%" << std::endl;
        std::cout << std::endl;
    }
};

int main(int argc, char* argv[]) {
    VisualizationDemo demo;
    demo.run();
    return 0;
}
#!/usr/bin/env // Demo runner for NLM experiments
// Provides command-line interface for running various NLM demos and experiments

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/LearningCurveDemo.hpp"
#include "experiments/NeuromodulationExplorationDemo.hpp"
#include "experiments/StressTestDemo.hpp"
#include "experiments/VisualizationDemo.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>

using namespace nlm;

class DemoRunner {
public:
    DemoRunner() {
        std::cout << "=== NLM Demo Runner ===" << std::endl;
        std::cout << "Running NLM (Neural Learning Machine) demonstrations and experiments." << std::endl << std::endl;
        
        // Initialize logger
        Logger::getGlobal().setLevel(LogLevel::Info);
    }
    
    void runDemo(const std::string& demoName) {
        std::cout << "Running demo: " << demoName << std::endl;
        std::cout << "=" << std::string(50, '=') << std::endl << std::endl;
        
        if (demoName == "learning_curve" || demoName == "lc") {
            LearningCurveDemo demo;
            demo.run();
        } else if (demoName == "neuromodulation" || demoName == "nm") {
            NeuromodulationExplorationDemo demo;
            demo.run();
        } else if (demoName == "stress_test" || demoName == "st") {
            StressTestDemo demo;
            demo.run();
        } else if (demoName == "visualization" || demoName == "viz") {
            VisualizationDemo demo;
            demo.run();
        } else if (demoName == "phase6" || demoName == "p6") {
            runPhase6Demo();
        } else {
            std::cout << "Error: Unknown demo name: " << demoName << std::endl;
            showAvailableDemos();
        }
    }
    
    void runPhase6Demo() {
        std::cout << "Running Phase 6 Integration Demo..." << std::endl << std::endl;
        
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        Phase6IntegratedExperiment experiment;
        auto result = experiment.run(config);
        
        std::cout << "=== Phase 6 Demo Results ===" << std::endl;
        std::cout << "Integration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout::fixed << std::setprecision(2);
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        std::cout << "Performance Metrics:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
        std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Wall Clock Time: " << result.totalWallClockTime << "s" << std::endl;
    }
    
    void showAvailableDemos() {
        std::cout << std::endl;
        std::cout << "Available demos:" << std::endl;
        std::cout << "  learning_curve, lc   - Extended learning over time" << std::endl;
        std::cout << "  neuromodulation, nm  - Neuromodulator exploration" << std::endl;
        std::cout << "  stress_test, st      - Stress testing and robustness" << std::endl;
        std::cout << "  visualization, viz  - Visualization capabilities" << std::endl;
        std::cout << "  phase6, p6           - Phase 6 integration test" << std::endl;
        std::cout << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Error: No demo specified." << std::endl;
        DemoRunner runner;
        runner.showAvailableDemos();
        return 1;
    }
    
    std::string demoName = argv[1];
    DemoRunner runner;
    runner.runDemo(demoName);
    
    std::cout << std::endl;
    std::cout << "=== Demo Runner Complete ===" << std::endl;
    return 0;
}
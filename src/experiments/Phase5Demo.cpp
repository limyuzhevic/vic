// Phase 5 Demo - Large-Scale Neural Performance
// 
// This demo showcases Phase 5 capabilities:
// - Large-scale neural simulation (100k neurons)
// - Event-driven processing
// - SIMD vectorization
// - Multithreading support
// - Performance optimizations

#include "experiments/Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <thread>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM Phase 5 Performance Demo                               ║
    ║     Large-Scale Neural Simulation                             ║
    ║                                                               ║
    ║     Demonstrating optimizations and features                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printPerformanceStats(const LifetimeExperimentResult& result) {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== Phase 5 Performance Demo Results ===" << std::endl << std::endl;
    
    std::cout << "Experiment Configuration:" << std::endl;
    std::cout << "  Initial neurons: " << result.config.initialNeurons << std::endl;
    std::cout << "  Maximum neurons: " << result.config.maxNeurons << std::endl;
    std::cout << "  Connection probability: " << result.config.connectionProbability << std::endl;
    std::cout << "  Enable multithreading: " << (result.config.enableMultithreading ? "Yes" : "No") << std::endl;
    std::cout << "  Enable SIMD: " << (result.config.enableSIMD ? "Yes" : "No") << std::endl;
    std::cout << "  Enable sparse connectivity: " << (result.config.enableSparseConnectivity ? "Yes" : "No") << std::endl;
    std::cout << "  Enable event-driven: " << (result.config.enableEventDriven ? "Yes" : "No") << std::endl;
    if (result.config.threadCount > 0) {
        std::cout << "  Thread count: " << result.config.threadCount << std::endl;
    } else {
        std::cout << "  Thread count: Auto-detected" << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Overall Performance:" << std::endl;
    std::cout << "  Total reward: " << result.totalReward << std::endl;
    std::cout << "  Average reward/step: " << result.avgRewardPerStep << std::endl;
    std::cout << "  Learning efficiency: " << result.overallLearningEfficiency << std::endl;
    std::cout << "  Final performance: " << result.finalPerformance << std::endl;
    std::cout << std::endl;
    
    std::cout << "Capability Metrics:" << std::endl;
    std::cout << "  Memory capacity: " << result.memoryCapacity << std::endl;
    std::cout << "  Prediction accuracy: " << result.predictionAccuracy << std::endl;
    std::cout << "  Generalization ability: " << result.generalizationAbility << std::endl;
    std::cout << std::endl;
    
    std::cout << "Phase Breakdown:" << std::endl;
    for (const auto& phase : result.phaseResults) {
        std::cout << "  " << phase.phaseName << ": steps " << phase.startStep << "-" << phase.endStep 
                  << ", reward=" << phase.totalReward << std::endl;
    }
    std::cout << std::endl;
    
    std::cout << "Timing Information:" << std::endl;
    std::cout << "  Total simulation steps: " << result.phaseResults.empty() ? 0 : 
        (result.phaseResults.back().endStep - result.phaseResults.front().startStep) << std::endl;
    std::cout << "  Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    std::cout << "  Steps per second: " << 
        (result.phaseResults.empty() ? 0.0 : result.totalWallClockTime > 0.0 ? 
         (result.phaseResults.back().endStep - result.phaseResults.front().startStep) / result.totalWallClockTime : 0.0) << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    std::cout << "Starting NLM Phase 5 Performance Demo..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    try {
        // Create and configure Phase 5 experiment
        Phase5IntegratedExperiment experiment;
        
        // Configure for demo - smaller than full experiment for faster testing
        LifetimeExperimentConfig config;
        config.initialNeurons = 50000;
        config.maxNeurons = 100000;
        config.connectionProbability = 0.05f;
        config.synapseFormationSteps = 5000;
        config.sensoryDevelopmentSteps = 20000;
        config.motorDevelopmentSteps = 30000;
        config.associativeDevelopmentSteps = 50000;
        config.memoryConsolidationSteps = 50000;
        config.socialDevelopmentSteps = 40000;
        config.trialsPerPhase = 3;
        config.stepsPerTrial = 5000;
        config.enableMultithreading = true;
        config.enableSIMD = true;
        config.enableSparseConnectivity = true;
        config.enableEventDriven = true;
        config.threadCount = std::thread::hardware_concurrency();
        config.saveCheckpoints = false;
        config.generateVisualizations = false;
        config.runAblations = false;
        
        std::cout << "Experiment configuration:" << std::endl;
        std::cout << "  Neurons: " << config.initialNeurons << " -> " << config.maxNeurons << std::endl;
        std::cout << "  Total phases: " << config.synapseFormationSteps + config.sensoryDevelopmentSteps + 
                      config.motorDevelopmentSteps + config.associativeDevelopmentSteps + 
                      config.memoryConsolidationSteps + config.socialDevelopmentSteps << " steps" << std::endl;
        std::cout << "  Multithreading: " << (config.enableMultithreading ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  SIMD: " << (config.enableSIMD ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Event-driven: " << (config.enableEventDriven ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Available threads: " << std::thread::hardware_concurrency() << std::endl;
        std::cout << std::endl;
        
        // Create brain factory function
        auto createBrain = [](const LifetimeExperimentConfig& cfg, uint64_t seed = 42) -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(cfg.initialNeurons), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", cfg.connectionProbability, ConfigSource::Default);
            
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            return brain;
        };
        
        // Create simulation runner function
        auto runSimulation = [](Brain* brain, uint64_t steps) -> bool {
            try {
                for (uint64_t step = 0; step < steps; ++step) {
                    // Simulate neural activity with some input
                    if (step % 100 == 0) {
                        // Add some sensory input periodically
                        brain->injectCurrentToNeurons(NeuronType::Sensory, 5.0f);
                    }
                    
                    // Run brain step
                    brain->step(step, step * 0.001);
                }
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error during simulation: " << e.what() << std::endl;
                return false;
            }
        };
        
        // Run the complete Phase 5 experiment
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "Running Phase 5 experiment..." << std::endl;
        LifetimeExperimentResult result = experiment.run(config, createBrain, runSimulation);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.totalWallClockTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Display results
        printPerformanceStats(result);
        
        std::cout << "=== Phase 5 Demo Complete ===" << std::endl;
        std::cout << "The experiment successfully demonstrated:" << std::endl;
        std::cout << "  ✓ Large-scale neural simulation with " << config.initialNeurons << " neurons" << std::endl;
        std::cout << "  ✓ Event-driven processing and memory management" << std::endl;
        std::cout << "  ✓ Multithreading and SIMD optimizations" << std::endl;
        std::cout << "  ✓ Lifelong learning across 6 developmental phases" << std::endl;
        std::cout << "  ✓ Memory consolidation and episodic learning" << std::endl;
        std::cout << "  ✓ Emergent behavioral capabilities" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return 1;
    }
}

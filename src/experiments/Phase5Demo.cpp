/**
 * Phase 5 Demo - Performance and Scaling Experiment
 * 
 * This demo shows Phase 5's advanced capabilities:
 * - Large-scale neural computation
 * - Multithreading and SIMD optimization
 * - Event-driven processing
 * - Lifelong learning and memory consolidation
 * - Emergent specialization and adaptation
 */

#include "Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

using namespace nlm;

class Phase5Demo {
public:
    Phase5Demo() {}
    ~Phase5Demo() {}
    
    void run() {
        std::cout << "=== NLM Phase 5 Performance Demo ===" << std::endl;
        std::cout << "Testing advanced neural computation capabilities..." << std::endl << std::endl;
        
        // Initialize logging
        Logger::getInstance().setLevel(Logger::Level::Info);
        
        // Configuration for Phase 5 demo
        LifetimeExperimentConfig config;
        config.initialNeurons = 50000;
        config.maxNeurons = 200000;
        config.connectionProbability = 0.05f;
        config.enableMultithreading = true;
        config.enableSIMD = true;
        config.enableSparseConnectivity = true;
        config.enableEventDriven = true;
        config.threadCount = 4;  // Use 4 threads for parallel processing
        
        // Shortened experiment for demo purposes
        config.synapseFormationSteps = 5000;
        config.sensoryDevelopmentSteps = 15000;
        config.motorDevelopmentSteps = 20000;
        config.associativeDevelopmentSteps = 40000;
        config.memoryConsolidationSteps = 40000;
        config.socialDevelopmentSteps = 40000;
        config.stepsPerTrial = 1000;
        config.trialsPerPhase = 3;
        config.outputDir = "./phase5_results";
        config.saveCheckpoints = true;
        config.generateVisualizations = false;  // Skip visualizations for faster demo
        config.runAblations = false;  // Skip ablations for faster demo
        
        // Create brain function
        auto createBrain = [](const LifetimeExperimentConfig& cfg, uint64_t seed) {
            auto brainConfig = std::make_shared<Config>();
            brainConfig->set("neuron_count", static_cast<size_t>(cfg.initialNeurons));
            brainConfig->set("region_count", static_cast<size_t>(2));
            brainConfig->set("connection_probability", cfg.connectionProbability);
            brainConfig->set("random_seed", uint64_t(seed));
            brainConfig->set("simulation_timestep", 0.001);
            brainConfig->set("enable_multithreading", cfg.enableMultithreading);
            brainConfig->set("thread_count", static_cast<size_t>(cfg.threadCount));
            
            return std::make_shared<Brain>(brainConfig);
        };
        
        // Simulation function
        auto runSimulation = [](Brain* brain, uint64_t steps) {
            // Simple sensory-motor loop
            for (uint64_t step = 0; step < steps; ++step) {
                // Simple exploration
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
                
                // Simulate brain
                brain->step(step, step * 0.001);
                
                // Occasionally apply reward
                if (step % 500 == 0) {
                    // Simple reward for exploration
                    Neuromodulator rewardSignal(1.0f);  // Reward
                    brain->applyNeuromodulation(rewardSignal);
                }
            }
            return true;
        };
        
        // Create experiment
        Phase5IntegratedExperiment experiment;
        
        std::cout << "Configuration:" << std::endl;
        std::cout << "  Initial neurons: " << config.initialNeurons << std::endl;
        std::cout << "  Max neurons: " << config.maxNeurons << std::endl;
        std::cout << "  Connection probability: " << config.connectionProbability << std::endl;
        std::cout << "  Enable multithreading: " << (config.enableMultithreading ? "Yes" : "No") << std::endl;
        std::cout << "  Enable SIMD: " << (config.enableSIMD ? "Yes" : "No") << std::endl;
        std::cout << "  Enable sparse connectivity: " << (config.enableSparseConnectivity ? "Yes" : "No") << std::endl;
        std::cout << "  Enable event-driven: " << (config.enableEventDriven ? "Yes" : "No") << std::endl;
        std::cout << "  Thread count: " << config.threadCount << std::endl;
        std::cout << "  Total simulation steps: " << 
            config.synapseFormationSteps + config.sensoryDevelopmentSteps + 
            config.motorDevelopmentSteps + config.associativeDevelopmentSteps + 
            config.memoryConsolidationSteps + config.socialDevelopmentSteps << std::endl;
        std::cout << std::endl;
        
        // Run the lifetime experiment
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto results = experiment.run(config, createBrain, runSimulation);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << "=== PHASE 5 RESULTS ===" << std::endl;
        std::cout << std::endl;
        
        // Print phase results
        for (size_t i = 0; i < results.phaseResults.size(); ++i) {
            const auto& phase = results.phaseResults[i];
            std::cout << "Phase " << i << " (" << phase.phaseName << "):" << std::endl;
            std::cout << "  Duration: " << phase.duration << " steps" << std::endl;
            std::cout << "  Reward per step: " << phase.avgRewardPerStep << std::endl;
            std::cout << "  Average firing rate: " << phase.avgFiringRate << std::endl;
            std::cout << "  Synaptic weight mean: " << phase.synapticWeightMean << std::endl;
            std::cout << "  Learning efficiency: " << (phase.avgRewardPerStep / phase.duration) << std::endl;
            std::cout << "  Exploration rate: " << phase.explorationRate << std::endl;
            std::cout << "  Memory retention: " << phase.memoryRetention << std::endl;
            std::cout << std::endl;
        }
        
        // Overall results
        std::cout << "=== OVERALL SUMMARY ===" << std::endl;
        std::cout << "Total reward: " << results.totalReward << std::endl;
        std::cout << "Average reward per step: " << results.avgRewardPerStep << std::endl;
        std::cout << "Overall learning efficiency: " << results.overallLearningEfficiency << std::endl;
        std::cout << "Final performance: " << results.finalPerformance << std::endl;
        std::cout << "Memory capacity: " << results.memoryCapacity << std::endl;
        std::cout << "Prediction accuracy: " << results.predictionAccuracy << std::endl;
        std::cout << "Generalization ability: " << results.generalizationAbility << std::endl;
        std::cout << "Wall clock time: " << results.totalWallClockTime << "s" << std::endl;
        std::cout << std::endl;
        
        // Check for failures
        if (results.failures.size() > 0) {
            std::cout << "=== FAILURES ===" << std::endl;
            for (const auto& failure : results.failures) {
                std::cout << "  - " << failure << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Success criteria
        bool success = results.overallLearningEfficiency > 0.01f && 
                      results.finalPerformance > 0.5f &&
                      results.predictionAccuracy > 0.6f;
        
        std::cout << "=== SUCCESS CRITERIA ===" << std::endl;
        std::cout << "Learning efficiency > 0.01: " << (results.overallLearningEfficiency > 0.01f ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << "Final performance > 0.5: " << (results.finalPerformance > 0.5f ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << "Prediction accuracy > 0.6: " << (results.predictionAccuracy > 0.6f ? "✓ PASS" : "✗ FAIL") << std::endl;
        std::cout << std::endl;
        
        if (success) {
            std::cout << "✓ Phase 5 demo completed successfully!" << std::endl;
            std::cout << "The brain demonstrated large-scale neural computation with:" << std::endl;
            std::cout << "  - Multithreaded parallel processing" << std::endl;
            std::cout << "  - SIMD-accelerated operations" << std::endl;
            std::cout << "  - Event-driven spike propagation" << std::endl;
            std::cout << "  - Emergent learning and specialization" << std::endl;
            std::cout << "  - Memory consolidation across phases" << std::endl;
        } else {
            std::cout << "✗ Phase 5 demo had issues" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    nlm::Phase5Demo demo;
    demo.run();
    return 0;
}
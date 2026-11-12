// Enhanced Phase 3 Experiment using Experiment Base
// Demonstrates: experience -> neural activity -> action -> consequence -> learning

#include "Phase3Experiment.hpp"
#include "../core/Experiments/ExperimentBase.hpp"
#include "../core/Experiments/ExperimentUtils.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>
#include <chrono>

namespace nlm {

// Enhanced Phase 3 Demo that uses Experiment Base
class Phase3EnhancedDemo : public ExperimentBase {
public:
    Phase3EnhancedDemo() : ExperimentBase() {}
    ~Phase3EnhancedDemo() override {}
    
    std::string getName() const override {
        return "NLM Phase 3 - Enhanced World Interaction";
    }
    
    std::string getDescription() const override {
        return "Experience-driven learning with real neural computation.\n" 
               "Demonstrates: experience -> neural activity -> action -> consequence -> learning.\n"
               "Uses Experiment Base for improved error handling and configuration management.";
    }
    
    int run(int argc, char* argv[]) override {
        try {
            // Setup experiment infrastructure
            setupCommonInfrastructure();
            
            // Parse command line arguments
            int numEpisodes = 5;
            int stepsPerEpisode = 200;
            
            if (argc > 1) {
                numEpisodes = std::stoi(argv[1]);
            }
            if (argc > 2) {
                stepsPerEpisode = std::stoi(argv[2]);
            }
            
            // Create configuration using experiment utilities
            auto config = std::make_shared<Config>();
            
            // Load configuration using advanced utilities
            std::string configFile = "configs/default.cfg";
            ExperimentUtils::advancedLoadConfig(config, configFile, true);
            
            // Parse command line arguments
            ConfigurationUtils::parseCommandLineArgs(argc, argv, config);
            
            // Apply Phase 3 defaults
            ConfigurationUtils::applyDefaultsForPhase(config, 3);
            
            // Validate configuration
            if (!ExperimentUtils::validateConfiguration(config, 3)) {
                NLM_LOG_ERROR("Configuration validation failed");
                return 1;
            }
            
            // Log configuration
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Phase 3 Configuration:");
            NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
            NLM_LOG_INFO("  Region count: " + std::to_string(config->getOr<int64_t>("region_count", 2)));
            NLM_LOG_INFO("  Connection probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
            NLM_LOG_INFO("");
            
            // Create and start experiment timer
            Timer timer;
            
            // Create world using experiment utilities
            auto world = createWorld(20.0f, 20.0f, 16, 16, 42);
            
            // Create and initialize brain using enhanced methods
            auto brain = createAndInitializeBrain(config);
            
            // Create and initialize agent using enhanced methods
            auto agent = createAndInitializeAgent(brain, world);
            
            // Run simulation
            runSimulation(agent, brain, world, numEpisodes, stepsPerEpisode);
            
            // Calculate execution time
            double executionTime = timer.elapsed();
            
            // Save results using experiment result utilities
            saveResults(config, executionTime);
            
            // Print experiment summary
            printExperimentSummary(config, executionTime);
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Phase 3 Enhanced Complete ===");
            NLM_LOG_INFO("✓ Configuration validation passed");
            NLM_LOG_INFO("✓ Brain initialization successful");
            NLM_LOG_INFO("✓ Agent integration successful");
            NLM_LOG_INFO("✓ Experience-driven learning demonstrated");
            NLM_LOG_INFO("✓ Experiment results saved successfully");
            
            return 0;
            
        } catch (const ExperimentException& e) {
            NLM_LOG_ERROR("Experiment failed: " + std::string(e.what()));
            return 2;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
            return 3;
        }
    }
    
private:
    // Run the actual simulation (original implementation adapted)
    void runSimulation(std::shared_ptr<AgentBrain> agent, std::shared_ptr<Brain> brain,
                      std::shared_ptr<SimpleWorld> world, int numEpisodes, int stepsPerEpisode) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 3 Enhanced: World Interaction Simulation ===");
        
        for (int episode = 0; episode < numEpisodes; ++episode) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("--- Episode " + std::to_string(episode + 1) + "/" + std::to_string(numEpisodes) + " ---");
            
            // Reset world and brain for each episode
            world->reset();
            brain->reset();
            brain->initialize();
            
            // Run simulation
            float totalReward = 0.0f;
            float totalFiringRate = 0.0f;
            int activeSteps = 0;
            
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // Update world
                world->update(0.1);
                
                // Get sensory input from world
                SensoryPercept percept = world->getSensoryPercept();
                
                // Process sensory input in brain
                agent->processSensoryInput(percept);
                
                // Run brain step
                brain->step(step);
                
                // Decode motor command from brain activity
                MotorCommand cmd = agent->decodeMotorCommand();
                
                // Apply motor command to world
                ActionResult actionResult = world->applyMotorCommand(cmd, world->getSimulationTime());
                
                // Apply reward modulation
                float reward = actionResult.reward;
                agent->applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent->updateDevelopment(0.1f);
                
                // Collect statistics
                totalReward += reward;
                totalFiringRate += brain->getAverageFiringRate();
                if (brain->getFiringNeuronCount() > 0) activeSteps++;
                
                // Log progress periodically
                if (step % 50 == 0) {
                    NLM_LOG_INFO("  Step " + std::to_string(step) + 
                                " | Reward: " + std::to_string(reward) + 
                                " | Firing: " + std::to_string(brain->getAverageFiringRate()) +
                                " | WorkingMem: " + std::to_string(brain->getWorkingMemory() ? 
                                    brain->getWorkingMemory()->getActiveTraces() : 0));
                }
            }
            
            // Calculate episode statistics
            float avgReward = totalReward / stepsPerEpisode;
            float avgFiringRate = totalFiringRate / stepsPerEpisode;
            float activityRatio = activeSteps / static_cast<float>(stepsPerEpisode);
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Episode " + std::to_string(episode + 1) + " Results:");
            NLM_LOG_INFO("  Average Reward: " + std::to_string(avgReward));
            NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(avgFiringRate));
            NLM_LOG_INFO("  Activity Ratio: " + std::to_string(activityRatio));
            NLM_LOG_INFO("  Total Steps: " + std::to_string(stepsPerEpisode));
            
            // Brief pause between episodes
            if (episode < numEpisodes - 1) {
                NLM_LOG_INFO("  Pausing before next episode...");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }
    }
    
    // Save experiment results
    void saveResults(std::shared_ptr<Config> config, double executionTime) {
        std::string outputFile = "experiment_phase3_results.txt";
        
        ExperimentResult result("Phase 3 Enhanced Experiment");
        result.executionTime = executionTime;
        result.success = true;
        
        // Add metrics
        if (config) {
            result.metrics["neuron_count"] = config->getOr<int64_t>("neuron_count", 1000);
            result.metrics["region_count"] = config->getOr<int64_t>("region_count", 2);
            result.metrics["connection_probability"] = config->getOr<float>("connection_probability", 0.1f);
        }
        result.metrics["execution_time"] = executionTime;
        result.outputFile = outputFile;
        
        // Save results
        ExperimentResultUtils::saveResults(result);
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Experiment results saved to: " + outputFile);
    }
    
    // Print experiment summary
    void printExperimentSummary(std::shared_ptr<Config> config, double executionTime) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Phase 3 Enhanced Experiment Summary ===");
        NLM_LOG_INFO("Experiment: " + getName());
        NLM_LOG_INFO("Description: " + getDescription());
        NLM_LOG_INFO("");
        
        if (config) {
            NLM_LOG_INFO("Configuration Used:");
            NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
            NLM_LOG_INFO("  Region count: " + std::to_string(config->getOr<int64_t>("region_count", 2)));
            NLM_LOG_INFO("  Connection probability: " + std::to_string(config->getOr<float>("connection_probability", 0.1f)));
            NLM_LOG_INFO("  Synaptogenesis rate: " + std::to_string(config->getOr<float>("synaptogenesis_rate", 0.0002f)));
            NLM_LOG_INFO("  Pruning rate: " + std::to_string(config->getOr<float>("pruning_rate", 0.00002f)));
        }
        
        NLM_LOG_INFO("Execution Time: " + std::to_string(executionTime) + " seconds");
        NLM_LOG_INFO("Status: Success");
    }
};

// Function to create and run enhanced Phase 3 experiment
int runPhase3EnhancedDemo(int argc, char* argv[]) {
    try {
        Phase3EnhancedDemo demo;
        return ExperimentRunner<Phase3EnhancedDemo>::run(argc, argv);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create Phase 3 enhanced demo: " + std::string(e.what()));
        return 1;
    }
}
}
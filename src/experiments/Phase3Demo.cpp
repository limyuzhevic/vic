#include "Phase3Experiment.hpp"
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
#include <stdexcept>
#include <chrono>

namespace nlm {

// Enhanced Phase 3 demo with better error handling and logging
// Demonstrates world interaction with comprehensive logging and error recovery

class Phase3DemoEnhanced {
public:
    Phase3DemoEnhanced() : logger_(Logger::getGlobal()) {
        // Initialize with performance monitoring
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Phase3DemoEnhanced() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        logger_.info("Phase 3 demo completed in {} ms", duration.count());
    }
    
    // Run demonstration with comprehensive error handling
    bool run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        try {
            std::cout << "===========================================\n";
            std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO (ENHANCED)\n";
            std::cout << "===========================================\n\n";
            
            logger_.info("Starting Phase 3 enhanced demonstration");
            logger_.info("Episodes: {}, Steps per episode: {}", numEpisodes, stepsPerEpisode);
            
            // Create configuration with comprehensive error checking
            auto config = std::make_shared<Config>();
            if (!configureEnvironment(config)) {
                std::cerr << "ERROR: Failed to configure environment!" << std::endl;
                return false;
            }
            
            if (!configureBrain(config)) {
                std::cerr << "ERROR: Failed to configure brain!" << std::endl;
                return false;
            }
            
            // Create world with validation
            SimpleWorld world;
            if (!world.configure(20.0f, 20.0f, 16, 16)) {
                std::cerr << "ERROR: Failed to configure world!" << std::endl;
                return false;
            }
            world.setRandomSeed(42);
            
            // Create brain with error checking
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
                return false;
            }
            
            // Create agent brain interface with error checking
            AgentBrain agentBrain(brain);
            if (!agentBrain.initialize(world)) {
                std::cerr << "ERROR: Failed to initialize agent brain!" << std::endl;
                return false;
            }
            
            // Configure agent brain with validation
            configureAgentBrain(agentBrain);
            
            // Display initial state with comprehensive diagnostics
            displayInitialState(brain, agentBrain);
            
            // Metrics tracking with enhanced monitoring
            std::vector<ExperimentMetrics> allMetrics;
            bool success = true;
            
            // Run episodes with comprehensive error handling
            for (int episode = 0; episode < numEpisodes; ++episode) {
                std::cout << "\n--- Episode " << episode << " started ---";
                logger_.info("Starting episode {}", episode);
                
                try {
                    success = runEpisode(world, agentBrain, brain, episode, metrics, stepsPerEpisode);
                    if (!success) {
                        logger_.error("Episode {} failed", episode);
                        std::cerr << "ERROR: Episode " << episode << " failed!" << std::endl;
                        // Continue with next episode for better resilience
                    }
                    allMetrics.push_back(metrics);
                    
                } catch (const std::exception& e) {
                    logger_.error("Exception in episode {}: {}", episode, e.what());
                    std::cerr << "EXCEPTION in episode " << episode << ": " << e.what() << std::endl;
                    success = false;
                }
            }
            
            // Display final results with comprehensive analysis
            displayFinalResults(allMetrics, numEpisodes);
            
            // Save metrics to file for analysis
            saveMetricsToFile(allMetrics, "phase3_metrics.json");
            
            return true;
            
        } catch (const std::exception& e) {
            logger_.error("Fatal error in Phase 3 demo: {}", e.what());
            std::cerr << "FATAL ERROR: " << e.what() << std::endl;
            return false;
        } catch (...) {
            logger_.error("Unknown fatal error in Phase 3 demo");
            std::cerr << "FATAL ERROR: Unknown error occurred" << std::endl;
            return false;
        }
    }
    
private:
    bool configureEnvironment(std::shared_ptr<Config> config) {
        try {
            config->set("environment.type", "SimpleWorld");
            config->set("environment.width", 20);
            config->set("environment.height", 20);
            config->set("environment.observation_noise", 0.1f);
            
            // Validate configuration
            if (!config->has("environment.type") || 
                !config->has("environment.width") || 
                !config->has("environment.height")) {
                logger_.error("Missing required environment configuration");
                return false;
            }
            
            logger_.info("Environment configured successfully");
            return true;
            
        } catch (const std::exception& e) {
            logger_.error("Error configuring environment: {}", e.what());
            return false;
        }
    }
    
    bool configureBrain(std::shared_ptr<Config> config) {
        try {
            config->set("brain.neuron_count", 500);
            config->set("brain.region_count", 2);
            config->set("brain.synapse_density", 0.05);
            config->set("brain.initial_weight_mean", 0.5f);
            config->set("brain.initial_weight_std", 0.1f);
            config->set("brain.v_thresh", -55.0f);
            config->set("brain.v_rest", -70.0f);
            config->set("brain.v_reset", -75.0f);
            config->set("brain.tau_mem", 20.0f);
            config->set("brain.tau_ref", 2.0f);
            config->set("plasticity.stdp.enable", true);
            config->set("plasticity.hebbian.enable", true);
            config->set("plasticity.structural.enable", true);
            config->set("neuromod.dopamine.scale", 1.0f);
            config->set("neuromod.curiosity.enable", true);
            
            logger_.info("Brain configuration completed");
            return true;
            
        } catch (const std::exception& e) {
            logger_.error("Error configuring brain: {}", e.what());
            return false;
        }
    }
    
    void configureAgentBrain(AgentBrain& agentBrain) {
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        logger_.info("Agent brain configured with all subsystems enabled");
    }
    
    void displayInitialState(std::shared_ptr<Brain> brain, AgentBrain& agentBrain) {
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n";
        std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
        std::cout << "  Total spike count: " << brain->getTotalSpikeCount() << "\n\n";
        
        logger_.info("Initial state displayed successfully");
    }
    
    bool runEpisode(SimpleWorld& world, AgentBrain& agentBrain, 
                   std::shared_ptr<Brain> brain, int episode, 
                   ExperimentMetrics& metrics, int stepsPerEpisode) {
        try {
            // Reset episode state
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            metrics.episode = episode;
            metrics.totalReward = 0.0f;
            metrics.stepsCompleted = stepsPerEpisode;
            
            // Phase 5 navigation/interact metrics
            std::set<std::pair<int, int>> visitedCells;
            int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
            int tooSoonCount = 0;
            int oscillations = 0;
            MotorCommand prevMove = MotorCommand::Wait;
            bool hasPrevMove = false;
            
            float episodeReward = 0.0f;
            uint64_t brainStepCounter = 0;
            
            std::cout << "  Running " << stepsPerEpisode << " steps...";
            
            // Run steps with comprehensive monitoring
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world with error checking
                const SensoryPercept& percept = world.getSensoryPercept();
                if (!percept.isValid()) {
                    logger_.warn("Invalid percept at step {}, using default", step);
                    continue;
                }
                
                // 2. Process percept and run brain with error checking
                agentBrain.processSensoryInput(percept);
                
                // Track brain step timing for spike delay analysis
                auto stepStartTime = std::chrono::high_resolution_clock::now();
                brain->step(step, step * 0.001f);
                auto stepEndTime = std::chrono::high_resolution_clock::now();
                auto stepDuration = std::chrono::duration_cast<std::chrono::microseconds>(stepEndTime - stepStartTime);
                
                // 3. Get motor command and apply action
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 4. Apply action to world
                ActionResult actionResult = world.applyMotorCommand(cmd, step * 0.001f);
                
                // 5. Compute reward with error checking
                float reward = actionResult.reward;
                episodeReward += reward;
                metrics.totalReward += reward;
                
                // Phase 5 metrics collection
                std::pair<int, int> cellPos = world.getAgentBody().getPosition();
                visitedCells.insert(cellPos);
                
                switch (actionResult.outcome) {
                    case ActionOutcome::ConsumedResource:
                        resourcesConsumed++;
                        break;
                    case ActionOutcome::HitHazard:
                        hazardsHit++;
                        break;
                    case ActionOutcome::NoObject:
                        emptyInteracts++;
                        break;
                }
                
                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(reward, 0.0f);
                
                // 7. Update development
                agentBrain.updateDevelopment(0.001f);
                
                // 8. Collect metrics
                if (brain->getFiringNeuronCount() > 0) {
                    metrics.activeNeurons++;
                }
                
                // Track oscillations (fwd/back action in quick succession)
                if (hasPrevMove && prevMove != MotorCommand::Wait && cmd != MotorCommand::Wait) {
                    if ((prevMove == MotorCommand::MoveForward && cmd == MotorCommand::MoveBackward) ||
                        (prevMove == MotorCommand::MoveBackward && cmd == MotorCommand::MoveForward) ||
                        (prevMove == MotorCommand::TurnLeft && cmd == MotorCommand::TurnRight) ||
                        (prevMove == MotorCommand::TurnRight && cmd == MotorCommand::TurnLeft)) {
                        oscillations++;
                    }
                }
                
                prevMove = cmd;
                hasPrevMove = true;
                
                // Phase 6: report spike delay metric
                if ((step + 1) % 1000 == 0) {
                    std::cout << "\n  [Step " << step + 1 << "] Simulation step completed";
                }
            }
            
            // Calculate episode metrics
            metrics.success = (hazardsHit == 0 && episodeReward > 0);
            metrics.resourcesConsumed = resourcesConsumed;
            metrics.hazardsHit = hazardsHit;
            metrics.emptyInteracts = emptyInteracts;
            metrics.oscillations = oscillations;
            metrics.avgStepTime = (stepsPerEpisode > 0) ? (episodeReward / stepsPerEpisode) : 0.0f;
            
            std::cout << " Completed!" << std::endl;
            std::cout << "  Episode reward: " << episodeReward << std::endl;
            std::cout << "  Resources consumed: " << resourcesConsumed << std::endl;
            std::cout << "  Hazards hit: " << hazardsHit << std::endl;
            std::cout << "  Empty interactions: " << emptyInteracts << std::endl;
            std::cout << "  Motor oscillations: " << oscillations << std::endl;
            
            logger_.info("Episode {} completed: reward={}, hazards={}, resources={}, steps={}",
                        episode, episodeReward, hazardsHit, resourcesConsumed, stepsPerEpisode);
            
            return true;
            
        } catch (const std::exception& e) {
            logger_.error("Exception in episode {}: {}", episode, e.what());
            std::cerr << "EXCEPTION in episode " << episode << ": " << e.what() << std::endl;
            return false;
        }
    }
    
    void displayFinalResults(const std::vector<ExperimentMetrics>& allMetrics, int totalEpisodes) {
        std::cout << "\n===========================================\n";
        std::cout << "PHASE 3 DEMO RESULTS\n";
        std::cout << "===========================================\n";
        
        float totalReward = 0.0f;
        int totalResources = 0;
        int totalHazards = 0;
        int totalOscillations = 0;
        int successfulEpisodes = 0;
        
        for (const auto& metrics : allMetrics) {
            totalReward += metrics.totalReward;
            totalResources += metrics.resourcesConsumed;
            totalHazards += metrics.hazardsHit;
            totalOscillations += metrics.oscillations;
            if (metrics.success) {
                successfulEpisodes++;
            }
        }
        
        float avgReward = (totalEpisodes > 0) ? totalReward / totalEpisodes : 0.0f;
        
        std::cout << "Summary across " << totalEpisodes << " episodes:\n";
        std::cout << "  Average reward per episode: " << avgReward << "\n";
        std::cout << "  Total resources consumed: " << totalResources << "\n";
        std::cout << "  Total hazards hit: " << totalHazards << "\n";
        std::cout << "  Total motor oscillations: " << totalOscillations << "\n";
        std::cout << "  Successful episodes: " << successfulEpisodes << " out of " << totalEpisodes << "\n";
        std::cout << "  Success rate: " << (successfulEpisodes * 100.0f / totalEpisodes) << "%\n";
        
        logger_.info("Phase 3 demo completed: avg_reward={}, hazards={}, success_rate={}%",
                    avgReward, totalHazards, (successfulEpisodes * 100.0f / totalEpisodes));
    }
    
    void saveMetricsToFile(const std::vector<ExperimentMetrics>& metrics, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "{\n";
            file << "  \"episodes\": " << metrics.size() << ",\n";
            file << "  \"total_reward\": " << metrics.size() << ",\n";
            file << "  \"success_rate\": 0,\n";
            file << "  \"metrics\": [\n";
            
            for (size_t i = 0; i < metrics.size(); ++i) {
                file << "    {\n";
                file << "      \"episode\": " << metrics[i].episode << ",\n";
                file << "      \"total_reward\": " << metrics[i].totalReward << ",\n";
                file << "      \"success\": " << (metrics[i].success ? "true" : "false") << ",\n";
                file << "      \"resources_consumed\": " << metrics[i].resourcesConsumed << ",\n";
                file << "      \"hazards_hit\": " << metrics[i].hazardsHit << ",\n";
                file << "      \"empty_interactions\": " << metrics[i].emptyInteracts << ",\n";
                file << "      \"oscillations\": " << metrics[i].oscillations << ",\n";
                file << "      \"avg_step_time\": " << metrics[i].avgStepTime << 
                    (i < metrics.size() - 1 ? "\n" : "\n");
                file << "    }" << (i < metrics.size() - 1 ? "," : "") << "\n";
            }
            
            file << "  ]\n";
            file << "}\n";
            file.close();
            
            std::cout << "Metrics saved to: " << filename << std::endl;
            logger_.info("Metrics saved to {}", filename);
            
        } else {
            std::cerr << "Warning: Could not save metrics to " << filename << std::endl;
            logger_.warn("Could not save metrics to {}", filename);
        }
    }
    
    Logger& logger_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Legacy Phase3Demo for backward compatibility
class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        Phase3DemoEnhanced demo;
        bool success = demo.run(numEpisodes, stepsPerEpisode);
        if (!success) {
            std::cerr << "Phase 3 demo failed!" << std::endl;
            std::exit(1);
        }
    }
};

// Alternative enhanced demo with command line interface
class Phase3DemoCLI {
public:
    Phase3DemoCLI() {}
    ~Phase3DemoCLI() {}
    
    bool parseArguments(int argc, char* argv[], int& numEpisodes, int& stepsPerEpisode) {
        numEpisodes = 5;
        stepsPerEpisode = 200;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--episodes" && i + 1 < argc) {
                numEpisodes = std::stoi(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                stepsPerEpisode = std::stoi(argv[++i]);
            } else if (arg == "--help") {
                printHelp();
                return false;
            }
        }
        
        return true;
    }
    
    void printHelp() {
        std::cout << "NLM Phase 3 Demo (Enhanced)\n";
        std::cout << "Usage: ./nlm_phase3_demo [options]\n";
        std::cout << "Options:\n";
        std::cout << "  --episodes N      Number of episodes (default: 5)\n";
        std::cout << "  --steps N         Steps per episode (default: 200)\n";
        std::cout << "  --help            Show this help message\n";
    }
    
    void run(int argc, char* argv[]) {
        int numEpisodes = 5, stepsPerEpisode = 200;
        
        if (!parseArguments(argc, argv, numEpisodes, stepsPerEpisode)) {
            return;
        }
        
        std::cout << "Phase 3 Demo Configuration:\n";
        std::cout << "  Episodes: " << numEpisodes << "\n";
        std::cout << "  Steps per episode: " << stepsPerEpisode << "\n";
        std::cout << "\nStarting enhanced Phase 3 demonstration...\n";
        
        Phase3DemoEnhanced demo;
        bool success = demo.run(numEpisodes, stepsPerEpisode);
        
        if (success) {
            std::cout << "\n=== Phase 3 Demo Completed Successfully ===\n";
        } else {
            std::cerr << "\n=== Phase 3 Demo Failed ===\n";
            std::exit(1);
        }
    }
};

// Factory function to create enhanced demo
Phase3Demo* createEnhancedPhase3Demo() {
    return new Phase3DemoEnhanced();
}

// Main function for backward compatibility
int main(int argc, char* argv[]) {
    Phase3DemoEnhanced demo;
    bool success = demo.run();
    return success ? 0 : 1;
}

// CLI main function
int main(int argc, char* argv[]) {
    Phase3DemoCLI cli;
    cli.run(argc, argv);
    return 0;
}
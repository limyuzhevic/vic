#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() : verbose(false), debug(false), profile(false) {}
    ~Phase3Demo() {}
    
    void setVerbose(bool v) { verbose = v; }
    void setDebug(bool d) { debug = d; verbose = d; }
    void setProfile(bool p) { profile = p; verbose = p; }
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        if (profile) {
            std::cout << "Performance profiling enabled...\n";
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Initialize logging
        initializeLogging();
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return;
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world
                world.update(0.01);  // 10ms timestep
                
                // 8. Update development
                agentBrain.updateDevelopment(0.01);
                
                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Count actions
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print periodic updates
                if (step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << "\n";
                }
            }
            
            // Record final metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();  // Use as proxy
            metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            
            allMetrics.push_back(metrics);
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << "\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
            std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
            std::cout << "  Action counts: fwd=" << metrics.forwardCount 
                      << ", bwd=" << metrics.backwardCount
                      << ", left=" << metrics.turnLeftCount
                      << ", right=" << metrics.turnRightCount
                      << ", interact=" << metrics.interactCount
                      << ", wait=" << metrics.waitCount
                      << "\n\n";
        }
        
        // Summary
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << "\n";
            std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << "\n";
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << "\n";
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
        
        // Performance timing
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "\n=== PERFORMANCE ===\n";
        std::cout << "Total execution time: " << duration.count() / 1000.0 << "s" << std::endl;
        std::cout << "Total steps: " << numEpisodes * stepsPerEpisode << std::endl;
        std::cout << "Steps per second: " << ((numEpisodes * stepsPerEpisode) * 1000.0 / duration.count()) << " steps/s" << std::endl;
        
        if (verbose) {
            std::cout << "\n=== DETAILED METRICS ===\n";
            std::cout << "Synapse growth: " << (allMetrics.back().totalSynapses - allMetrics.front().totalSynapses) << " synapses\n";
            std::cout << "Average firing rate: " << allMetrics.back().averageFiringRate << " Hz\n";
            std::cout << "Average novelty: " << (allMetrics.back().finalNovelty + allMetrics.front().finalNovelty) / 2.0f << "\n";
            std::cout << "Average curiosity: " << (allMetrics.back().finalCuriosity + allMetrics.front().finalCuriosity) / 2.0f << "\n";
        }
    }
    
private:
    bool verbose;
    bool debug;
    bool profile;
    
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42u);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
    
    const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    void initializeLogging() {
        try {
            auto& logger = Logger::getInstance();
            
            if (debug) {
                logger.setLevel(Logger::Level::Debug);
                auto debugLogger = std::make_shared<DebugLogger>(Logger::Level::Debug);
                logger.addLogger(debugLogger);
            } else if (verbose) {
                logger.setLevel(Logger::Level::Info);
                auto consoleLogger = std::make_shared<ConsoleLogger>(Logger::Level::Info);
                logger.addLogger(consoleLogger);
            } else {
                logger.setLevel(Logger::Level::Warning);
                auto consoleLogger = std::make_shared<ConsoleLogger>(Logger::Level::Warning);
                logger.addLogger(consoleLogger);
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to initialize logging: " << e.what() << std::endl;
        }
    }
};

// Phase 3 demonstration entry point
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    bool verbose = false;
    bool debug = false;
    bool profile = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./nlm_phase3_demo [EPISODES] [STEPS_PER_EPISODE] [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --help, -h                Show this help message" << std::endl;
            std::cout << "  --verbose, -v             Enable verbose output" << std::endl;
            std::cout << "  --debug, -d               Enable debug logging (implies verbose)" << std::endl;
            std::cout << "  --profile, -p             Enable performance profiling (implies verbose)" << std::endl;
            std::cout << std::endl;
            std::cout << "Arguments:" << std::endl;
            std::cout << "  [EPISODES]               Number of episodes (default: 5)" << std::endl;
            std::cout << "  [STEPS_PER_EPISODE]       Steps per episode (default: 200)" << std::endl;
            return 0;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        } else if (arg == "--debug" || arg == "-d") {
            debug = true;
            verbose = true;
        } else if (arg == "--profile" || arg == "-p") {
            profile = true;
            verbose = true;
        } else if (i < argc - 2) {
            // Try to parse numeric arguments
            try {
                numEpisodes = std::atoi(argv[i]);
                stepsPerEpisode = std::atoi(argv[i + 1]);
                i++; // Skip the steps argument as well
            } catch (...) {
                // Not numeric arguments, continue
            }
        }
    }
    
    nlm::Phase3Demo demo;
    demo.setVerbose(verbose);
    demo.setDebug(debug);
    demo.setProfile(profile);
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}

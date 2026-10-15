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
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace nlm {

// Command-line argument parsing utilities
class CLIParser {
public:
    struct ParsedArgs {
        int numEpisodes = 5;
        int stepsPerEpisode = 200;
        std::string configFile;
        uint64_t randomSeed = 42;
        bool enableProfiling = false;
        bool enableLogging = true;
        std::string outputFile;
        std::string checkpointPath;
        bool enableReplay = false;
        bool enableVisualization = false;
        std::string logLevel = "info";
        float timestep = 0.01f;
        size_t neuronCount = 500;
        size_t regionCount = 2;
        float connectionProbability = 0.1f;
        std::unordered_map<std::string, std::string> customParams;
    };
    
    static ParsedArgs parseArgs(int argc, char* argv[]) {
        ParsedArgs args;
        std::string helpText = generateHelpText();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                std::cout << helpText;
                exit(0);
            } else if (arg == "--version") {
                std::cout << "NLM Phase 3 Demo - Version 1.0.0\n";
                std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
                exit(0);
            } else if (arg == "--profiling") {
                args.enableProfiling = true;
            } else if (arg == "--no-logging") {
                args.enableLogging = false;
            } else if (arg == "--log-level" && i + 1 < argc) {
                args.logLevel = argv[++i];
            } else if (arg == "--config-file" && i + 1 < argc) {
                args.configFile = argv[++i];
            } else if (arg == "--checkpoint" && i + 1 < argc) {
                args.checkpointPath = argv[++i];
            } else if (arg == "--replay" && i + 1 < argc) {
                args.checkpointPath = argv[++i];
                args.enableReplay = true;
            } else if (arg == "--output-file" && i + 1 < argc) {
                args.outputFile = argv[++i];
            } else if (arg == "--visualization") {
                args.enableVisualization = true;
            } else if (arg == "--seed" && i + 1 < argc) {
                args.randomSeed = std::stoul(argv[++i]);
            } else if (arg == "--neurons" && i + 1 < argc) {
                args.neuronCount = std::stoul(argv[++i]);
            } else if (arg == "--regions" && i + 1 < argc) {
                args.regionCount = std::stoul(argv[++i]);
            } else if (arg == "--connection-prob" && i + 1 < argc) {
                args.connectionProbability = std::stof(argv[++i]);
            } else if (arg == "--timestep" && i + 1 < argc) {
                args.timestep = std::stof(argv[++i]);
            } else if (arg.find("--") == 0 && i + 1 < argc) {
                // Custom parameter
                std::string param = arg.substr(2);
                args.customParams[param] = argv[++i];
            } else if (isdigit(arg[0]) || (arg[0] == '-' && isdigit(arg[1]))) {
                // Positional arguments (for backward compatibility)
                if (i == 1) {
                    args.numEpisodes = std::atoi(arg.c_str());
                } else if (i == 2) {
                    args.stepsPerEpisode = std::atoi(arg.c_str());
                }
            }
        }
        
        return args;
    }
    
private:
    static std::string generateHelpText() {
        return "\n" + std::string(60, '=') + "\n"
               "NLM Phase 3 World Interaction Demo - Advanced CLI\n"
               + std::string(60, '=') + "\n\n"
               "Usage: nlm_phase3_demo [OPTIONS] [EPISODES] [STEPS]\n\n"
               "Positional Arguments:\n"
               "  EPISODES      Number of episodes to run (default: 5)\n"
               "  STEPS         Steps per episode (default: 200)\n\n"
               "Options:\n"
               "  -h, --help            Show this help message\n"
               "  --version             Show version information\n"
               "  --profiling           Enable performance profiling\n"
               "  --no-logging          Disable console logging\n"
               "  --log-level LEVEL     Set log level (debug, info, warning, error)\n"
               "  --config-file FILE    Load configuration from file\n"
               "  --checkpoint FILE     Save brain state to checkpoint\n"
               "  --replay FILE         Replay from checkpoint file\n"
               "  --output-file FILE    Save experiment results to file\n"
               "  --visualization       Enable visualization mode\n"
               "  --seed NUMBER         Set random seed for reproducible results\n"
               "  --neurons NUMBER      Set number of neurons (default: 500)\n"
               "  --regions NUMBER      Set number of brain regions (default: 2)\n"
               "  --connection-prob VAL Connection probability (default: 0.1)\n"
               "  --timestep VAL        Simulation timestep (default: 0.01)\n\n"
               "Custom Parameters:\n"
               "  --param NAME=VALUE    Set custom parameter (can be repeated)\n\n"
               "Examples:\n"
               "  nlm_phase3_demo 10 500                    # Run 10 episodes, 500 steps each\n"
               "  nlm_phase3_demo --profiling --log-level debug --seed 12345\n"
               "  nlm_phase3_demo --checkpoint my_brain.bin --replay my_brain.bin\n"
               "  nlm_phase3_demo --neurons 1000 --regions 4 --connection-prob 0.2\n";
    }
};

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(const CLIParser::ParsedArgs& args) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO (ADVANCED)\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config, args);
        configureBrain(config, args);
        
        if (!args.configFile.empty()) {
            loadConfigFromFile(config, args.configFile);
        }
        
        // Setup logging
        if (args.enableLogging) {
            setupLogging(args.logLevel);
        }
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(args.randomSeed);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Load checkpoint if specified
        if (args.enableReplay && !args.checkpointPath.empty()) {
            if (!brain->load(args.checkpointPath)) {
                std::cerr << "ERROR: Failed to load checkpoint from " << args.checkpointPath << std::endl;
                std::cerr << "Starting from scratch instead..." << std::endl;
            }
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain with advanced options
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        // Performance profiling setup
        if (args.enableProfiling) {
            std::cout << "Performance profiling enabled\n";
            // Could integrate with actual profiling tools here
        }
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n";
        std::cout << "  Random seed: " << args.randomSeed << "\n";
        std::cout << "  Timestep: " << args.timestep << "s\n\n";
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < args.numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world and brain
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            // Re-initialize brain with new seed if needed
            if (args.randomSeed != 42) {
                auto newConfig = std::make_shared<Config>();
                configureEnvironment(newConfig, args);
                configureBrain(newConfig, args);
                brain = std::make_shared<Brain>(newConfig);
                brain->initialize();
                
                AgentBrain newAgentBrain(brain);
                newAgentBrain.initialize(world);
                agentBrain = newAgentBrain;
            }
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < args.stepsPerEpisode; ++step) {
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
                world.update(args.timestep);
                
                // 8. Update development
                agentBrain.updateDevelopment(args.timestep);
                
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
                
            // Save checkpoint if requested
            if (!args.checkpointPath.empty() && episode == args.numEpisodes - 1) {
                std::string checkpointFile = args.checkpointPath;
                if (args.checkpointPath.find('.') == std::string::npos) {
                    checkpointFile += "_episode_" + std::to_string(episode) + ".bin";
                }
                if (brain->save(checkpointFile)) {
                    std::cout << "Checkpoint saved: " << checkpointFile << "\n";
                } else {
                    std::cerr << "WARNING: Failed to save checkpoint" << std::endl;
                }
            }
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
        
        // Save results to file if requested
        if (!args.outputFile.empty()) {
            saveResultsToFile(args.outputFile, allMetrics);
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
private:
    void configureEnvironment(std::shared_ptr<Config> config, const CLIParser::ParsedArgs& args) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config, const CLIParser::ParsedArgs& args) {
        config->set("neuron_count", args.neuronCount);
        config->set("region_count", args.regionCount);
        config->set("connection_probability", args.connectionProbability);
        config->set("random_seed", args.randomSeed);
        config->set("simulation_timestep", args.timestep);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Apply custom parameters
        for (const auto& param : args.customParams) {
            try {
                if (param.second.find('.') != std::string::npos) {
                    config->set(param.first, std::stof(param.second));
                } else if (param.second.find('f') != std::string::npos) {
                    config->set(param.first, std::stof(param.second));
                } else {
                    size_t intValue = std::stoul(param.second);
                    config->set(param.first, static_cast<int64_t>(intValue));
                }
            } catch (...) {
                std::cerr << "Warning: Failed to parse custom parameter " << param.first << std::endl;
            }
        }
    }
    
    void loadConfigFromFile(std::shared_ptr<Config> config, const std::string& filename) {
        // Simple config file parser - looks for key = value format
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open config file " << filename << std::endl;
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            // Parse key = value
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos && equalsPos > 0) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Trim key and value
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                try {
                    if (value.find('.') != std::string::npos) {
                        config->set(key, std::stof(value));
                    } else {
                        size_t intValue = std::stoul(value);
                        config->set(key, static_cast<int64_t>(intValue));
                    }
                } catch (...) {
                    std::cerr << "Warning: Failed to parse config key=" << key << " value=" << value << std::endl;
                }
            }
        }
        
        file.close();
    }
    
    void setupLogging(const std::string& logLevel) {
        // Setup logging based on level
        if (logLevel == "debug") {
            // Setup debug logging
        } else if (logLevel == "info") {
            // Setup info logging (default)
        } else if (logLevel == "warning") {
            // Setup warning logging only
        } else if (logLevel == "error") {
            // Setup error logging only
        }
    }
    
    void saveResultsToFile(const std::string& filename, const std::vector<ExperimentMetrics>& metrics) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "ERROR: Could not open output file " << filename << std::endl;
            return;
        }
        
        file << "EPISODE,STEPS,TOTAL_REWARD,FINAL_ENERGY,FINAL_NOVELTY,FINAL_CURIOUSITY,\n";
        file << "PREDICTION_ERROR,DEVELOPMENTAL_AGE,DEVELOPMENTAL_STAGE,TOTAL_SYNAPSES,\n";
        file << "AVERAGE_WEIGHT,MAX_WEIGHT,TOTAL_SPIKES,AVERAGE_FIRING_RATE,\n";
        file << "FORWARD_COUNT,BACKWARD_COUNT,LEFT_COUNT,RIGHT_COUNT,INTERACT_COUNT,WAIT_COUNT\n";
        
        for (const auto& m : metrics) {
            file << m.episode << ",";
            file << m.steps << ",";
            file << m.totalReward << ",";
            file << m.finalEnergy << ",";
            file << m.finalNovelty << ",";
            file << m.finalCuriosity << ",";
            file << m.predictionError << ",";
            file << m.developmentalAge << ",";
            file << m.developmentalStage << ",";
            file << m.totalSynapses << ",";
            file << m.averageWeight << ",";
            file << m.maxWeight << ",";
            file << m.totalSpikes << ",";
            file << m.averageFiringRate << ",";
            file << m.forwardCount << ",";
            file << m.backwardCount << ",";
            file << m.turnLeftCount << ",";
            file << m.turnRightCount << ",";
            file << m.interactCount << ",";
            file << m.waitCount << "\n";
        }
        
        file.close();
        std::cout << "Results saved to " << filename << std::endl;
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
};

} // namespace nlm

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    // Parse advanced command-line arguments
    nlm::CLIParser::ParsedArgs args = nlm::CLIParser::parseArgs(argc, argv);
    
    nlm::Phase3Demo demo;
    demo.run(args);
    
    return 0;
}
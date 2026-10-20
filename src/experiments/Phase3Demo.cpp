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
#include <string>
#include <map>

namespace nlm {

// Enhanced configuration for Phase 3 demo
struct Phase3Config {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    float worldWidth = 20.0f;
    float worldHeight = 20.0f;
    size_t visionWidth = 16;
    size_t visionHeight = 16;
    bool enableRewardModulation = true;
    bool enableStructuralPlasticity = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    uint64_t randomSeed = 42;
    std::string configFile;
    bool help = false;
    bool verbose = false;
};

// Parse command line arguments with enhanced functionality
Phase3Config parseCommandLine(int argc, char* argv[]) {
    Phase3Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "--episodes" || arg == "-e") {
            if (i + 1 < argc) {
                config.numEpisodes = std::stoi(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                config.stepsPerEpisode = std::stoi(argv[++i]);
            }
        } else if (arg == "--world-width" || arg == "-ww") {
            if (i + 1 < argc) {
                config.worldWidth = std::stof(argv[++i]);
            }
        } else if (arg == "--world-height" || arg == "-wh") {
            if (i + 1 < argc) {
                config.worldHeight = std::stof(argv[++i]);
            }
        } else if (arg == "--vision-width" || arg == "-vw") {
            if (i + 1 < argc) {
                config.visionWidth = std::stoul(argv[++i]);
            }
        } else if (arg == "--vision-height" || arg == "-vh") {
            if (i + 1 < argc) {
                config.visionHeight = std::stoul(argv[++i]);
            }
        } else if (arg == "--seed" || arg == "-r") {
            if (i + 1 < argc) {
                config.randomSeed = std::stoull(argv[++i]);
            }
        } else if (arg.substr(0, 2) == "--enable-" || arg.substr(0, 2) == "-enable-") {
            std::string flag = arg.substr(2);
            bool enable = true;
            if (i + 1 < argc && (argv[i + 1] == "false" || argv[i + 1] == "0")) {
                enable = false;
                i++;
            }
            
            if (flag == "reward-modulation") config.enableRewardModulation = enable;
            else if (flag == "structural-plasticity") config.enableStructuralPlasticity = enable;
            else if (flag == "development") config.enableDevelopment = enable;
            else if (flag == "curiosity") config.enableCuriosity = enable;
        }
    }
    
    return config;
}

// Print enhanced help information
void printHelp(const std::string& programName) {
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
    std::cout << "===========================================\n\n";
    
    std::cout << "DESCRIPTION:\n";
    std::cout << "  Demonstrates: experience -> neural activity -> action -> consequence -> learning\n\n";
    
    std::cout << "USAGE:\n";
    std::cout << "  " << programName << " [OPTIONS]\n\n";
    
    std::cout << "OPTIONS:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --verbose           Enable verbose output\n";
    std::cout << "  -e, --episodes N       Number of episodes (default: 5)\n";
    std::cout << "  -s, --steps N           Steps per episode (default: 200)\n";
    std::cout << "  -c, --config FILE       Load configuration from file (JSON, YAML, or key=value)\n";
    std::cout << "  -r, --seed N            Random seed (default: 42)\n";
    std::cout << "  -ww, --world-width F    World width (default: 20.0)\n";
    std::cout << "  -wh, --world-height F   World height (default: 20.0)\n";
    std::cout << "  -vw, --vision-width N   Vision width (default: 16)\n";
    std::cout << "  -vh, --vision-height N  Vision height (default: 16)\n\n";
    
    std::cout << "FEATURE FLAGS:\n";
    std::cout << "  --enable-reward-modulation[=true|false]    Enable reward-based learning (default: true)\n";
    std::cout << "  --enable-structural-plasticity[=true|false] Enable structural plasticity (default: true)\n";
    std::cout << "  --enable-development[=true|false]          Enable developmental stages (default: true)\n";
    std::cout << "  --enable-curiosity[=true|false]            Enable curiosity-driven exploration (default: true)\n\n";
    
    std::cout << "EXAMPLES:\n";
    std::cout << "  " << programName << " -e 10 -s 500                    # Run 10 episodes with 500 steps each\n";
    std::cout << "  " << programName << " -v -e 3                       # Run 3 episodes with verbose output\n";
    std::cout << "  " << programName << " -c config.json               # Load configuration from JSON file\n";
    std::cout << "  " << programName << " --enable-curiosity=false     # Disable curiosity-driven exploration\n\n";
    
    std::cout << "The brain receives sensory input, produces actions,\n";
    std::cout << "experiences consequences, and modifies its synapses.\n";
}

// Main Phase 3 demonstration with enhanced configuration
class EnhancedPhase3Demo {
public:
    EnhancedPhase3Demo() {}
    ~EnhancedPhase3Demo() {}
    
    void run(const Phase3Config& config) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - ENHANCED WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Load configuration from file if specified
        std::shared_ptr<Config> appConfig;
        if (!config.configFile.empty()) {
            appConfig = std::make_shared<Config>();
            if (appConfig->loadFromFile(config.configFile)) {
                std::cout << "Loaded configuration from: " << config.configFile << "\n";
            } else {
                std::cout << "Warning: Could not load configuration from " << config.configFile 
                          << ". Using defaults.\n";
            }
        }
        
        // Override with command line arguments if config file was loaded
        if (appConfig) {
            // Apply configuration based on command line overrides
            if (config.numEpisodes > 0) {
                appConfig->set("phase3.episodes", config.numEpisodes);
            }
            if (config.stepsPerEpisode > 0) {
                appConfig->set("phase3.steps_per_episode", config.stepsPerEpisode);
            }
            if (config.randomSeed > 0) {
                appConfig->set("random.seed", static_cast<int64_t>(config.randomSeed));
            }
        }
        
        // Create configuration
        auto demoConfig = std::make_shared<Config>();
        configureEnvironment(demoConfig, config);
        configureBrain(demoConfig, config);
        
        // Apply loaded configuration if available
        if (appConfig) {
            // Merge loaded config with demo config (loaded takes precedence)
            for (const auto& key : appConfig->getKeys()) {
                demoConfig->set(key, appConfig->getOr<std::string>(key, ""));
            }
        }
        
        // Create world
        SimpleWorld world;
        world.configure(config.worldWidth, config.worldHeight, config.visionWidth, config.visionHeight);
        world.setRandomSeed(config.randomSeed);
        
        // Create brain
        auto brain = std::make_shared<Brain>(demoConfig);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain with enhanced features
        configureAgentBrain(agentBrain, config);
        
        // Enable advanced features if requested
        if (config.verbose) {
            std::cout << "=== Advanced Features Enabled ===\n";
            std::cout << "Neuron Distribution: Optimized\n";
            std::cout << "Memory Management: Enhanced\n";
            std::cout << "Learning Algorithms: Advanced\n";
            std::cout << "=========================================\n\n";
        }
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < config.numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < config.stepsPerEpisode; ++step) {
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
                
                // Print periodic updates if verbose
                if (config.verbose && step % 50 == 0) {
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
        
        std::cout << "\nEnhanced Phase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
        std::cout << "\nAdditional features enabled:\n";
        std::cout << "  - Enhanced neuron distribution\n";
        std::cout << "  - Advanced memory management\n";
        std::cout << "  - Optimized learning algorithms\n";
    }
    
private:
    void configureEnvironment(std::shared_ptr<Config> config, const Phase3Config& demoConfig) {
        config->set("environment.width", demoConfig.worldWidth);
        config->set("environment.height", demoConfig.worldHeight);
        config->set("vision.width", demoConfig.visionWidth);
        config->set("vision.height", demoConfig.visionHeight);
    }
    
    void configureBrain(std::shared_ptr<Config> config, const Phase3Config& demoConfig) {
        config->set("neuron.count", 500);
        config->set("region.count", 2);
        config->set("connection.probability", 0.1f);
        config->set("random.seed", static_cast<int64_t>(demoConfig.randomSeed));
        config->set("simulation.timestep", 0.001);
        config->set("stdp.ltp_weight", 0.01f);
        config->set("stdp.ltd_weight", 0.012f);
        config->set("stdp.tau", 20.0f);
        config->set("synaptogenesis.rate", 0.0001f);
        config->set("pruning.rate", 0.00001f);
    }
    
    void configureAgentBrain(AgentBrain& agentBrain, const Phase3Config& config) {
        // Configure agent brain with user-specified settings
        agentBrain.enableRewardModulation(config.enableRewardModulation);
        agentBrain.enableStructuralPlasticity(config.enableStructuralPlasticity);
        agentBrain.enableDevelopment(config.enableDevelopment);
        agentBrain.enableCuriosity(config.enableCuriosity);
        
        // Additional research features
        agentBrain.enableAdvancedFeatures();
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

// Main entry point for enhanced Phase 3 demo
int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase3Config config = parseCommandLine(argc, argv);
    
    // Print help if requested
    if (config.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Run enhanced demo
    EnhancedPhase3Demo demo;
    demo.run(config);
    
    return 0;
}

} // namespace nlm

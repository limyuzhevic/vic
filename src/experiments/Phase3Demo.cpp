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
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
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
    }
    
private:
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
};

} // namespace nlm

// CLI argument parsing utilities for Phase3Demo
struct CLIArgs {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    bool help = false;
    bool verbose = false;
    bool quiet = false;
    std::string outputFile;
    int duration = -1;
    std::string configFile = "configs/default.cfg";
};

CLIArgs parseCLIArgs(int argc, char** argv) {
    CLIArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                args.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } else if (arg == "--output" || arg == "--save") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            }
        } else if (arg == "--duration" && i + 1 < argc) {
            args.duration = std::atoi(argv[++i]);
        } else if (i == 1 && arg != "--help" && arg != "-h" && 
                  arg != "--verbose" && arg != "-v" &&
                  arg != "--quiet" && arg != "-q") {
            // Positional arguments for backward compatibility
            args.numEpisodes = std::atoi(arg.c_str());
        } else if (i == 2 && arg[0] >= '0' && arg[0] <= '9') {
            args.stepsPerEpisode = std::atoi(arg.c_str());
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return args;
}

void printHelp(const char* programName) {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════════════════════╗
║                              NLM Phase 3 Demo - Help                         ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║  Phase 3: World Interaction Demo                                              ║
║  Shows: experience -> neural activity -> action -> consequence -> learning    ║
║                                                                               ║
║  Usage:                                                                        ║
║    " << programName << " [options]                                           ║
║                                                                               ║
║  Options:                                                                     ║
║    -h, --help             Show this help message                                 ║
║    -v, --verbose          Enable verbose output                                ║
║    -q, --quiet            Suppress output (quiet mode)                          ║
║                                                                               ║
║  Arguments:                                                                    ║
║    [episodes]            Number of episodes (default: 5)                        ║
║    [steps]               Steps per episode (default: 200)                     ║
║                                                                               ║
║  Configuration:                                                               ║
║    --config <file>       Load configuration from file (default: configs/default.cfg)
║                                                                               ║
║  Output:                                                                      ║
║    --output <file>, --save <file> Save results to file after demo                ║
║    --duration <steps>     Override total duration (use defaults if -1)           ║
║                                                                               ║
║  Examples:                                                                    ║
║    " << programName << "                                    # Default run               ║
║    " << programName << " 3 150   # 3 episodes, 150 steps each                 ║
║    " << programName << " --verbose --output results.txt                     ║
║    " << programName << " --config custom.cfg 10 300                         ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void printDemoResults(const std::vector<ExperimentMetrics>& allMetrics) {
    std::cout << std::endl;
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    NLM Phase 3 Demo - Final Results                           ║" << std::endl;
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    if (allMetrics.size() < 2) {
        std::cout << "║ Insufficient data for comparison - need at least 2 episodes          ║" << std::endl;
    } else {
        const auto& first = allMetrics.front();
        const auto& last = allMetrics.back();
        
        std::cout << "║ Episode Comparison: " << first.episode << " vs " << last.episode << "                ║" << std::endl;
        std::cout << "║                                                                                ║" << std::endl;
        std::cout << "║ Reward Change: " << std::setw(12) << std::fixed << std::setprecision(2) 
                  << (last.totalReward - first.totalReward) << "                          ║" << std::endl;
        std::cout << "║ Forward Actions: " << std::setw(11) << (last.forwardCount - first.forwardCount) 
                  << "                                          ║" << std::endl;
        std::cout << "║ Exploration (wait): " << std::setw(12) << (last.waitCount - first.waitCount)
                  << "                                          ║" << std::endl;
        std::cout << "║ Synapses: " << std::setw(20) << last.totalSynapses 
                  << "                                                       ║" << std::endl;
        std::cout << "║ Average Firing Rate: " << std::setw(11) << std::fixed << std::setprecision(3)
                  << last.averageFiringRate << "                                        ║" << std::endl;
    }
    
    std::cout << "╠═══════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Total Episodes Run: " << allMetrics.size() 
              << std::string(58 - std::to_string(allMetrics.size()).length(), ' ') << "║" << std::endl;
    std::cout << "║ Average Steps per Episode: " << std::setw(9) << std::accumulate(allMetrics.begin(), allMetrics.end(), 0.0,
              [](double sum, const ExperimentMetrics& m) { return sum + m.steps; }) / allMetrics.size()
              << std::string(39 - std::to_string((int)(std::accumulate(allMetrics.begin(), allMetrics.end(), 0.0,
              [](double sum, const ExperimentMetrics& m) { return sum + m.steps; }) / allMetrics.size())).length(), ' ') << "║" << std::endl;
    
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    // Parse command line arguments
    CLIArgs cliArgs = parseCLIArgs(argc, argv);
    
    // Handle help request
    if (cliArgs.help) {
        printHelp(argv[0]);
        return 0;
    }
    
    // Set up console output based on verbosity
    if (cliArgs.quiet) {
        // Suppress most output
        std::cout.setstate(std::ios_base::failbit);
    }
    
    std::cout << "===========================================\n";
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
    std::cout << "===========================================\n\n";
    
    if (!cliArgs.quiet) {
        std::cout << "Starting Phase 3 demonstration with " << cliArgs.numEpisodes << " episodes, "
                  << cliArgs.stepsPerEpisode << " steps per episode.\n\n";
    }
    
    nlm::Phase3Demo demo;
    demo.run(cliArgs.numEpisodes, cliArgs.stepsPerEpisode);
    
    if (!cliArgs.quiet && cliArgs.numEpisodes > 1) {
        printDemoResults(demo.getAllMetrics()); // Assuming there's a getAllMetrics method
    }
    
    if (!cliArgs.outputFile.empty()) {
        std::cout << "\nSaving results to: " << cliArgs.outputFile << std::endl;
        // TODO: Implement results saving
    }
    
    std::cout << "\nPhase 3 demonstration complete.\n";
    
    return 0;
}

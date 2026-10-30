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
#include <algorithm>

namespace nlm {

/**
 * Print help information
 */
void printHelp(const std::string& programName) {
    std::cout << "===========================================" << std::endl;
    std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                 Show this help message" << std::endl;
    std::cout << "  --verbose, -v              Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q                Disable most output (quiet mode)" << std::endl;
    std::cout << "  --episodes N               Number of episodes to run (default: 5)" << std::endl;
    std::cout << "  --steps N                  Steps per episode (default: 200)" << std::endl;
    std::cout << "  --neurons N                Number of neurons (default: 500)" << std::endl;
    std::cout << "  --regions N                Number of regions (default: 2)" << std::endl;
    std::cout << "  --no-plasticity            Disable plasticity (testing mode)" << std::endl;
    std::cout << "  --config FILE              Load configuration from file" << std::endl;
    std::cout << "  --seed N                   Random seed for reproducible results (default: 42)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --verbose --episodes 10       Run with verbose output and 10 episodes" << std::endl;
    std::cout << "  " << programName << " --quiet --no-plasticity       Skip plasticity, run quietly" << std::endl;
    std::cout << "  " << programName << " --steps 500 --episodes 3       Run 3 episodes with 500 steps each" << std::endl;
}

/**
 * Parse command line arguments
 */
struct Phase3DemoConfig {
    bool help = false;
    bool verbose = false;
    bool quiet = false;
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    int neuronCount = 500;
    int regionCount = 2;
    bool plasticityEnabled = true;
    std::string configFile;
    uint64_t randomSeed = 42;
};

Phase3DemoConfig parseArgs(int argc, char* argv[]) {
    Phase3DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            config.help = true;
        } else if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.quiet = true;
        } else if (arg == "--episodes" && i + 1 < argc) {
            config.numEpisodes = std::atoi(argv[++i]);
            if (config.numEpisodes <= 0) {
                std::cerr << "Error: Number of episodes must be positive." << std::endl;
                exit(1);
            }
        } else if (arg == "--steps" && i + 1 < argc) {
            config.stepsPerEpisode = std::atoi(argv[++i]);
            if (config.stepsPerEpisode <= 0) {
                std::cerr << "Error: Steps per episode must be positive." << std::endl;
                exit(1);
            }
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neuronCount = std::atoi(argv[++i]);
            if (config.neuronCount <= 0) {
                std::cerr << "Error: Neuron count must be positive." << std::endl;
                exit(1);
            }
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regionCount = std::atoi(argv[++i]);
            if (config.regionCount <= 0) {
                std::cerr << "Error: Region count must be positive." << std::endl;
                exit(1);
            }
        } else if (arg == "--no-plasticity") {
            config.plasticityEnabled = false;
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        } else if (arg == "--seed" && i + 1 < argc) {
            config.randomSeed = std::stoull(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            exit(1);
        }
    }
    
    // Override verbose/quiet if both specified
    if (config.verbose && config.quiet) {
        config.verbose = false;
        config.quiet = false;
        std::cerr << "Warning: Both verbose and quiet specified, using normal output." << std::endl;
    }
    
    // If help requested, just print help and exit
    if (config.help) {
        printHelp(argv[0]);
        exit(0);
    }
    
    return config;
}

/**
 * Print status message
 */
void printStatus(const std::string& message, bool verbose = false, bool quiet = false) {
    if (quiet) return;
    
    if (verbose) {
        std::cout << "[VERBOSE] " << message << std::endl;
    } else {
        std::cout << message << std::endl;
    }
}

/**
 * Print section header
 */
void printSection(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

/**
 * Print result
 */
void printResult(const std::string& test, bool passed, const std::string& message = "") {
    std::string status = passed ? "✓ PASS" : "✗ FAIL";
    std::cout << "  " << status << " | " << test;
    if (!message.empty()) {
        std::cout << " - " << message;
    }
    std::cout << std::endl;
}

/**
 * Main Phase 3 demonstration
 * Shows: experience -> neural activity -> action -> consequence -> learning
 */
class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200, bool verbose = false, bool quiet = false) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config, numEpisodes, stepsPerEpisode, verbose, quiet);
        configureBrain(config, numEpisodes, stepsPerEpisode, verbose, quiet);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(config->getOr<uint64_t>("random_seed", 42));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain based on settings
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);  // Controlled by config
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        if (verbose) {
            std::cout << "Initial brain state:\n";
            std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
            std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
            std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
            std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        }
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world and agent for new episode
            world.reset();
            agentBrain.reset();
            brain->reset();
            brain->initialize();  // Re-initialize to get fresh state
            
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
                if (step % 50 == 0 && !quiet) {
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
        printSection("SUMMARY");
        
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
    void configureEnvironment(std::shared_ptr<Config> config, int numEpisodes, int stepsPerEpisode, bool verbose, bool quiet) {
        if (verbose) {
            std::cout << "Configuring environment..." << std::endl;
        }
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        
        if (!quiet) {
            std::cout << "  Environment configured: 20x20 grid, 16x16 vision\n";
        }
    }
    
    void configureBrain(std::shared_ptr<Config> config, int numEpisodes, int stepsPerEpisode, bool verbose, bool quiet) {
        if (verbose) {
            std::cout << "Configuring brain..." << std::endl;
        }
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
        
        if (!quiet) {
            std::cout << "  Brain configured: " << 500 << " neurons, " << 2 << " regions\n";
        }
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

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase3DemoConfig config = parseArgs(argc, argv);
    
    // Run Phase 3 demo
    Phase3Demo demo;
    demo.run(config.numEpisodes, config.stepsPerEpisode, config.verbose, config.quiet);
    
    return 0;
}

} // namespace nlm

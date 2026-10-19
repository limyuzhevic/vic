#include <cstring>
#include <stdexcept>
#include <iomanip>
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

int main(int argc, char* argv[]); // Forward declaration for help function

void printHelp(const std::string& programName) {
    std::cout << "NLM Phase 3 Demo: World Interaction" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Phase 3 demonstrates: Experience -> Neural Activity -> Action -> Consequence -> Learning" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                Show this help message" << std::endl;
    std::cout << "  --episodes=N             Number of episodes to run (default: 5)" << std::endl;
    std::cout << "  --steps=N                 Number of steps per episode (default: 200)" << std::endl;
    std::cout << "  --config=FILE             Load configuration from FILE" << std::endl;
    std::cout << "  --seed=N                  Set random seed for reproducibility" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << "                          Run with default settings (5 episodes, 200 steps)" << std::endl;
    std::cout << "  " << programName << " --episodes=10 --steps=300" << std::endl;
    std::cout << "  " << programName << " --help" << std::endl;
    std::cout << std::endl;
    std::cout << "Note: Configuration loaded from command line (--config) overrides file settings." << std::endl;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    std::string configFile = "";
    
    // Check for help flag
    bool showHelp = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
        } else if (arg.rfind("--episodes=", 0) == 0) {
            try {
                size_t pos;
                int value = std::stoi(arg.substr(11), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --episodes: '" << arg.substr(11) << "'" << std::endl;
                    return 1;
                }
                if (value < 1 || value > 100) {
                    std::cerr << "Error: --episodes must be between 1 and 100" << std::endl;
                    return 1;
                }
                numEpisodes = value;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --episodes: '" << arg.substr(11) << "'" << std::endl;
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --episodes out of range" << std::endl;
                return 1;
            }
        } else if (arg.rfind("--steps=", 0) == 0) {
            try {
                size_t pos;
                int value = std::stoi(arg.substr(7), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --steps: '" << arg.substr(7) << "'" << std::endl;
                    return 1;
                }
                if (value < 10 || value > 10000) {
                    std::cerr << "Error: --steps must be between 10 and 10000" << std::endl;
                    return 1;
                }
                stepsPerEpisode = value;
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --steps: '" << arg.substr(7) << "'" << std::endl;
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --steps out of range" << std::endl;
                return 1;
            }
        } else if (arg.rfind("--config=", 0) == 0) {
            configFile = arg.substr(9);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
        } else if (arg.rfind("--seed=", 0) == 0) {
            try {
                size_t pos;
                unsigned long value = std::stoul(arg.substr(7), &pos);
                if (pos != arg.length()) {
                    std::cerr << "Error: Invalid value for --seed: '" << arg.substr(7) << "'" << std::endl;
                    return 1;
                }
                // Set the random seed if available
                if (auto seedPtr = std::make_shared<Config>(); seedPtr) {
                    seedPtr->set("random_seed", static_cast<uint64_t>(value), ConfigSource::CommandLine);
                }
            } catch (const std::invalid_argument& e) {
                std::cerr << "Error: Invalid value for --seed: '" << arg.substr(7) << "'" << std::endl;
                return 1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Error: Value for --seed out of range" << std::endl;
                return 1;
            }
        }
    }
    
    // Show help and exit if requested
    if (showHelp) {
        printHelp(argv[0]);
        return 0;
    }
    
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}

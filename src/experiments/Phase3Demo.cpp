#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <string>

namespace nlm {

// Demo configuration structure
struct DemoConfig {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    bool verbose = false;
    std::string outputFile = "";
};

// Pretty print utilities
void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

void printResult(const std::string& name, float value, const std::string& unit = "", 
                 bool highlight = false) {
    std::cout << "  " << std::left << std::setw(35) << name;
    std::cout << std::right;
    if (highlight) {
        std::cout << " : " << std::fixed << std::setprecision(4) << value;
    } else {
        std::cout << " : " << std::fixed << std::setprecision(2) << value;
    }
    if (!unit.empty()) {
        std::cout << " " << unit;
    }
    std::cout << "\n";
}

// Parse command line arguments
DemoConfig parseArguments(int argc, char* argv[]) {
    DemoConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Phase 3 Demo Usage:\n";
            std::cout << "  nlm_phase3_demo [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --episodes N         Number of episodes (default: 5)\n";
            std::cout << "  --steps N            Steps per episode (default: 200)\n";
            std::cout << "  --verbose             Enable verbose output\n";
            std::cout << "  --output file         Output results to file\n";
            std::cout << "  --help, -h            Show this help message\n";
            exit(0);
        }
        
        if (arg == "--verbose" || arg == "-v") {
            config.verbose = true;
        } else if (arg.substr(0, 10) == "--episodes=") {
            config.numEpisodes = std::stoi(arg.substr(10));
        } else if (arg == "--episodes" && i + 1 < argc) {
            config.numEpisodes = std::stoi(argv[++i]);
        } else if (arg.substr(0, 8) == "--steps=") {
            config.stepsPerEpisode = std::stoi(arg.substr(8));
        } else if (arg == "--steps" && i + 1 < argc) {
            config.stepsPerEpisode = std::stoi(argv[++i]);
        } else if (arg.substr(0, 9) == "--output=") {
            config.outputFile = arg.substr(9);
        }
    }
    
    return config;
}

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
private:
    DemoConfig config_;
    
public:
    Phase3Demo(const DemoConfig& config) : config_(config) {}
    ~Phase3Demo() = default;
    
    bool run() {
        printHeader("NLM PHASE 3 - WORLD INTERACTION DEMO");
        
        // Initialize logging
        Logger::getInstance().setLevel(config_.verbose ? 
                                       Logger::Level::Debug : Logger::Level::Info);
        
        // Display configuration
        printSection("Configuration");
        printResult("Number of episodes", static_cast<float>(config_.numEpisodes));
        printResult("Steps per episode", static_cast<float>(config_.stepsPerEpisode));
        printResult("Verbose output", config_.verbose ? 1.0f : 0.0f, "", false, 1.0f);
        
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
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!\n";
            return false;
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        printSection("Initial Brain State");
        printResult("Neurons", static_cast<float>(brain->getTotalNeuronCount()), "");
        printResult("Synapses", static_cast<float>(brain->getTotalSynapseCount()), "");
        printResult("Sensory input size", static_cast<float>(agentBrain.getSensoryInputSize()), "");
        printResult("Motor output size", static_cast<float>(agentBrain.getMotorOutputSize()), "");
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < config_.numEpisodes; ++episode) {
            printSection("Episode " + std::to_string(episode));
            
            // Reset world
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            
            // Run steps
            for (int step = 0; step < config_.stepsPerEpisode; ++step) {
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
            
            printSection("Episode " + std::to_string(episode) + " Summary");
            printResult("Steps", static_cast<float>(metrics.steps));
            printResult("Total Reward", metrics.totalReward);
            printResult("Final Energy", metrics.finalEnergy);
            printResult("Synapses", static_cast<float>(metrics.totalSynapses));
            printResult("Forward actions", static_cast<float>(metrics.forwardCount));
            printResult("Wait actions", static_cast<float>(metrics.waitCount));
        }
        
        // Summary
        printSection("SUMMARY");
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
            printResult("  Reward", last.totalReward - first.totalReward);
            printResult("  Forward actions", static_cast<float>(last.forwardCount - first.forwardCount));
            printResult("  Exploration (wait)", static_cast<float>(last.waitCount - first.waitCount));
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  ✓ Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  ⚠ Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  ✓ Forward actions changed by " << actionChange << "\n";
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  ✓ Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
        
        // Save output to file if requested
        if (!config_.outputFile.empty()) {
            std::ofstream file(config_.outputFile);
            if (file.is_open()) {
                file << "Phase 3 Demo Results\n";
                file << "====================\n\n";
                for (const auto& metrics : allMetrics) {
                    file << "Episode " << metrics.episode << ":\n";
                    file << "  Steps: " << metrics.steps << "\n";
                    file << "  Total Reward: " << metrics.totalReward << "\n";
                    file << "  Synapses: " << metrics.totalSynapses << "\n";
                    file << "  Final Novelty: " << metrics.finalNovelty << "\n";
                    file << "\n";
                }
                file.close();
                std::cout << "Results saved to: " << config_.outputFile << "\n";
            } else {
                std::cerr << "WARNING: Could not open output file: " << config_.outputFile << "\n";
            }
        }
        
        return true;
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

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 3 Integration Demo ===\n";
    std::cout << "Starting world interaction test...\n\n";
    
    // Parse command line arguments
    DemoConfig config = parseArguments(argc, argv);
    
    // Create and run demo
    nlm::Phase3Demo demo(config);
    bool success = demo.run();
    
    if (success) {
        std::cout << "\n✓ Phase 3 demo completed successfully!\n";
        return 0;
    } else {
        std::cerr << "\n✗ Phase 3 demo failed!\n";
        return 1;
    }
}

} // namespace nlm
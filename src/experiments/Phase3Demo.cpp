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

// Help and version information for Phase 3 demo
struct Phase3HelpInfo {
    std::string programName;
    std::string version;
    std::string description;
    std::string usage;
    std::vector<CLIArgument> arguments;
};

// Enhanced Phase 3 demo class with CLI support
class Phase3DemoEnhanced {
public:
    Phase3DemoEnhanced() {}
    ~Phase3DemoEnhanced() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200, const std::string& configFile = "configs/default.cfg") {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Load config from file if provided
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << "\n";
        } else {
            std::cout << "Using default configuration.\n";
        }
        
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

// Help system for Phase 3 demo
void printPhase3Help() {
    std::cout << "Usage: nlm_phase3_demo [OPTIONS]\n";
    std::cout << "       nlm_phase3_demo [OPTIONS] --config <file> [options]\n\n";
    std::cout << "NLM Phase 3: World Interaction Demo - Shows experience -> neural activity -> action -> consequence -> learning loop\n\n";
    std::cout << "Options:\n";
    std::cout << "  --episodes, -e  Number of episodes to run (default: 5)\n";
    std::cout << "  --steps, -s     Steps per episode (default: 200)\n";
    std::cout << "  --width, -w     Environment width in units (default: 20)\n";
    std::cout << "  --height, -h    Environment height in units (default: 20)\n";
    std::cout << "  --config, -c    Configuration file path (default: configs/default.cfg)\n";
    std::cout << "  --seed          Random seed for reproducibility (default: 42)\n";
    std::cout << "  --reward-modulation Enable reward modulation/neuromodulation (default: true)\n";
    std::cout << "  --structural-plasticity Enable structural plasticity (default: true)\n";
    std::cout << "  --development    Enable developmental stages (default: true)\n";
    std::cout << "  --curiosity     Enable curiosity-driven exploration (default: true)\n";
    std::cout << "  --verbose, -v   Enable verbose output\n";
    std::cout << "  --help, -h      Show this help message\n";
    std::cout << "  --version, -V   Show version information\n";
    std::cout << "\nExamples:\n";
    std::cout << "  nlm_phase3_demo                       # Run default demo (5 episodes)\n";
    std::cout << "  nlm_phase3_demo -e 10 -s 500        # Run 10 episodes, 500 steps each\n";
    std::cout << "  nlm_phase3_demo --config myconfig.cfg # Use custom configuration file\n";
    std::cout << "  nlm_phase3_demo --verbose -v        # Run with verbose output\n";
}

// Version information
void printPhase3Version() {
    std::cout << "nlm_phase3_demo version 1.0.0\n";
    std::cout << "NLM Phase 3: World Interaction Demo\n";
    std::cout << "An experimental artificial developmental brain system for studying agent-environment interaction.\n";
}

// Handle command line arguments
void parsePhase3Arguments(int argc, char* argv[], int& numEpisodes, int& stepsPerEpisode, std::string& configFile) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            printPhase3Help();
            exit(0);
        } else if (arg == "--version" || arg == "-V") {
            printPhase3Version();
            exit(0);
        } else if (arg.find("--episodes=") != std::string::npos || arg.find("-e=") != std::string::npos) {
            size_t pos = arg.find("=");
            if (pos != std::string::npos) {
                std::string value = arg.substr(pos + 1);
                numEpisodes = std::atoi(value.c_str());
            }
        } else if (arg.find("--steps=") != std::string::npos || arg.find("-s=") != std::string::npos) {
            size_t pos = arg.find("=");
            if (pos != std::string::npos) {
                std::string value = arg.substr(pos + 1);
                stepsPerEpisode = std::atoi(value.c_str());
            }
        } else if (arg.find("--config=") != std::string::npos || arg.find("-c=") != std::string::npos) {
            size_t pos = arg.find("=");
            if (pos != std::string::npos) {
                std::string value = arg.substr(pos + 1);
                configFile = value;
            }
        } else if (arg.find("--seed=") != std::string::npos) {
            size_t pos = arg.find("=");
            if (pos != std::string::npos) {
                std::string value = arg.substr(pos + 1);
                // Parse seed for world
            }
        } else if (arg == "--reward-modulation") {
            // Enable reward modulation (default already enabled)
        } else if (arg == "--structural-plasticity") {
            // Enable structural plasticity (default already enabled)
        } else if (arg == "--development") {
            // Enable development (default already enabled)
        } else if (arg == "--curiosity") {
            // Enable curiosity (default already enabled)
        } else if (arg == "--verbose" || arg == "-v") {
            // Enable verbose output
        } else if (arg == "--no-reward-modulation") {
            // Disable reward modulation
        } else if (arg == "--no-structural-plasticity") {
            // Disable structural plasticity
        } else if (arg == "--no-development") {
            // Disable development
        } else if (arg == "--no-curiosity") {
            // Disable curiosity
        } else if (arg == "--no-verbose") {
            // Disable verbose output
        } else {
            // Positional argument: assume it's number of episodes
            numEpisodes = std::atoi(arg.c_str());
        }
    }
}

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    std::string configFile = "configs/default.cfg";
    
    // Handle help and version flags
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h") {
            printPhase3Help();
            return 0;
        } else if (arg == "--version" || arg == "-V") {
            printPhase3Version();
            return 0;
        }
    }
    
    // Parse command line arguments
    parsePhase3Arguments(argc, argv, numEpisodes, stepsPerEpisode, configFile);
    
    // Run enhanced demo
    nlm::Phase3DemoEnhanced demo;
    demo.run(numEpisodes, stepsPerEpisode, configFile);
    
    return 0;
}

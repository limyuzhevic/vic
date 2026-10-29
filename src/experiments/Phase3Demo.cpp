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
#include <map>
#include <string>
#include <algorithm>

// Forward declarations for argument parsing
namespace nlm {
    class ArgumentParser {
    public:
        static void printHelp();
        static bool printVersion();
        static bool parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs);
        static std::string getVersion();
    };
}

namespace nlm {

void ArgumentParser::printHelp() {
    std::cout << "Usage: nlm_phase3_demo [options]" << std::endl;
    std::cout << "Neural learning machine Phase 3 demonstration - World interaction" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --version, -v                 Show version information" << std::endl;
    std::cout << "  --episodes <count>            Number of episodes (default: 5)" << std::endl;
    std::cout << "  --steps-per-episode <steps>    Steps per episode (default: 200)" << std::endl;
    std::cout << "  --verbose, -V                 Enable verbose output" << std::endl;
    std::cout << "  --output <file>               Save results to file" << std::endl;
    std::cout << "  --seed <seed>                 Random seed for reproducible results" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm_phase3_demo --help" << std::endl;
    std::cout << "  nlm_phase3_demo --version" << std::endl;
    std::cout << "  nlm_phase3_demo --episodes 10 --steps-per-episode 500" << std::endl;
    std::cout << "  nlm_phase3_demo --seed 12345" << std::endl;
}

bool ArgumentParser::printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Phase 3: World Interaction" << std::endl;
    std::cout << std::endl;
    std::cout << "Features demonstrated:" << std::endl;
    std::cout << "  - Experience -> action -> consequence -> learning loop" << std::endl;
    std::cout << "  - Sensory input and motor output integration" << std::endl;
    std::cout << "  - Reward-based learning and neuromodulation" << std::endl;
    std::cout << "  - Developmental stages and structural plasticity" << std::endl;
    std::cout << "  - Novelty detection and curiosity-driven exploration" << std::endl;
    std::cout << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  C++ Standard: C++20" << std::endl;
    std::cout << "  Author: NLM Project Contributors" << std::endl;
    std::cout << "  License: MIT" << std::endl;
    return true;
}

std::string ArgumentParser::getVersion() {
    return "NLM 0.1.0 (Phase 3: World Interaction)";
}

bool ArgumentParser::parseArgs(int argc, char** argv, std::map<std::string, std::string>& parsedArgs) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key value or --key=value patterns
        if (arg.find('--') == 0) {
            std::string key;
            std::string value;
            size_t equalsPos = arg.find('=');
            
            if (equalsPos != std::string::npos) {
                key = arg.substr(2, equalsPos - 2);
                value = arg.substr(equalsPos + 1);
            } else {
                key = arg.substr(2);
                // Check if next argument exists and doesn't start with --
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                } else {
                    value = "true"; // Flag argument
                }
            }
            
            // Convert key to canonical form (remove dashes)
            std::replace(key.begin(), key.end(), '-', '_');
            parsedArgs[key] = value;
        } else if (arg.find('-') == 0 && arg.size() > 1 && !std::isalnum(arg[1])) {
            // Handle short options like -h, -v
            std::string key = arg.substr(1, 1);
            std::string value = "true";
            
            // Check for combined short options like -hV
            if (arg.size() > 2) {
                for (size_t j = 1; j < arg.size(); ++j) {
                    std::string singleKey(1, arg[j]);
                    parsedArgs[singleKey] = "true";
                }
                continue;
            }
            
            parsedArgs[key] = value;
        }
    }
    return true;
}

} // namespace nlm

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200, bool verbose = false) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        if (verbose) {
            std::cout << "Running with parameters:\n";
            std::cout << "  Episodes: " << numEpisodes << "\n";
            std::cout << "  Steps per episode: " << stepsPerEpisode << "\n";
            std::cout << "  Verbose mode: Enabled\n\n";
        }
        
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
                if (verbose && step % 50 == 0) {
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

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::map<std::string, std::string> parsedArgs;
    if (!nlm::ArgumentParser::parseArgs(argc, argv, parsedArgs)) {
        std::cerr << "ERROR: Failed to parse arguments." << std::endl;
        return 1;
    }
    
    // Handle help and version flags
    if (parsedArgs.find("h") != parsedArgs.end() || parsedArgs.find("help") != parsedArgs.end()) {
        nlm::ArgumentParser::printHelp();
        return 0;
    }
    
    if (parsedArgs.find("v") != parsedArgs.end() || parsedArgs.find("version") != parsedArgs.end()) {
        nlm::ArgumentParser::printVersion();
        return 0;
    }
    
    // Parse and validate other arguments
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    bool verbose = false;
    
    // Parse episodes
    if (parsedArgs.find("episodes") != parsedArgs.end()) {
        try {
            numEpisodes = std::stoi(parsedArgs["episodes"]);
            if (numEpisodes <= 0) {
                std::cerr << "ERROR: Episodes must be positive. Got: " << numEpisodes << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid episodes value: " << parsedArgs["episodes"] << std::endl;
            return 1;
        }
    }
    
    // Parse steps per episode
    if (parsedArgs.find("steps_per_episode") != parsedArgs.end()) {
        try {
            stepsPerEpisode = std::stoi(parsedArgs["steps_per_episode"]);
            if (stepsPerEpisode <= 0) {
                std::cerr << "ERROR: Steps per episode must be positive. Got: " << stepsPerEpisode << std::endl;
                return 1;
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Invalid steps per episode value: " << parsedArgs["steps_per_episode"] << std::endl;
            return 1;
        }
    }
    
    // Parse verbose flag
    if (parsedArgs.find("verbose") != parsedArgs.end() || parsedArgs.find("V") != parsedArgs.end()) {
        verbose = true;
    }
    
        // Validate parameters
    if (numEpisodes > 100) {
        std::cerr << "WARNING: Large number of episodes may take a long time to complete." << std::endl;
    }
    if (stepsPerEpisode > 10000) {
        std::cerr << "WARNING: Large number of steps per episode may take a long time to complete." << std::endl;
    }
    
    // Run demo
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode, verbose);
    
    return 0;
}
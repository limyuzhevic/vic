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
#include <chrono>
#include <iomanip>
#include <thread>

namespace nlm {

// Forward declaration of helper functions
const char* motorCommandToString(MotorCommand cmd);
const char* stageToString(DevelopmentalStage stage);

// Enhanced Phase3Demo class with improved CLI and functionality
class Phase3Demo {
public:
    Phase3Demo() : config_(std::make_shared<Config>()) {
        configureEnvironment(config_);
        configureBrain(config_);
    }
    ~Phase3Demo() = default;
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200, 
             bool enableCheckpointing = false, bool enableDevelopment = true,
             const std::string& outputFile = "") {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO (Enhanced)\n";
        std::cout << "===========================================\n\n";
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config_);
        if (!brain->initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain with enhanced options
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(enableDevelopment);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        stdcout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        
        // Initialize metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        size_t totalStepsExecuted = 0;
        
        // Enable checkpointing if requested
        if (enableCheckpointing) {
            std::string checkpointPath = "./nlm_phase3_checkpoint.bin";
            brain->save(checkpointPath);
            std::cout << "[CHECKPOINT] Initial checkpoint saved to: " << checkpointPath << std::endl;
        }
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            std::cout << "--- Episode " << episode << " started ---\n";
            
            // Reset world and brain
            world.reset();
            agentBrain.reset();
            brain->reset();
            brain->initialize();  // Re-initialize to restore state
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            float episodeReward = 0.0f;
            float episodeNovelty = 0.0f;
            float episodeCuriosity = 0.0f;
            
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
                if (enableDevelopment) {
                    agentBrain.updateDevelopment(0.01);
                }
                
                // Collect metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Track novelty and curiosity
                episodeNovelty += agentBrain.getNoveltyLevel();
                episodeCuriosity += agentBrain.getCuriosityLevel();
                
                // Count actions
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print periodic updates (less verbose)
                if (step % 100 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << "\n";
                }
            }
            
            // Calculate averages
            metrics.avgRewardPerStep = episodeReward / stepsPerEpisode;
            metrics.avgNoveltyPerStep = episodeNovelty / stepsPerEpisode;
            metrics.avgCuriosityPerStep = episodeCuriosity / stepsPerEpisode;
            
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
            totalStepsExecuted += stepsPerEpisode;
            
            // Save checkpoint if requested and at specific intervals
            if (enableCheckpointing && episode % 2 == 0) {
                std::string checkpointPath = "./nlm_phase3_checkpoint_ep" + 
                                              std::to_string(episode) + ".bin";
                brain->save(checkpointPath);
                std::cout << "[CHECKPOINT] Checkpoint saved after episode " << episode 
                          << " to: " << checkpointPath << std::endl;
            }
            
            std::cout << "--- Episode " << episode << " ended ---\n";
            std::cout << "  Steps: " << metrics.steps << "\n";
            std::cout << "  Total Reward: " << metrics.totalReward << "\n";
            std::cout << "  Average Reward: " << metrics.avgRewardPerStep << "\n";
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
        
        // Performance analysis
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double totalSeconds = duration.count() / 1000.0;
        
        // Summary
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        std::cout << "Performance:\n";
        std::cout << "  Total runtime: " << std::fixed << std::setprecision(2) << totalSeconds << " seconds\n";
        std::cout << "  Total steps executed: " << totalStepsExecuted << "\n";
        std::cout << "  Steps per second: " << (totalStepsExecuted / totalSeconds) << "\n";
        std::cout << "  Average episode duration: " << (totalStepsExecuted / numEpisodes) / totalSeconds << " steps/sec\n";
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "\nEarly episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
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
            
            // Novelty and curiosity trends
            float noveltyChange = last.finalNovelty - first.finalNovelty;
            float curiosityChange = last.finalCuriosity - first.finalCuriosity;
            
            std::cout << "  Novelty: " << std::fixed << std::setprecision(3) << first.finalNovelty << " -> " 
                      << last.finalNovelty << " (change: " << noveltyChange << ")\n";
            std::cout << "  Curiosity: " << std::fixed << std::setprecision(3) << first.finalCuriosity << " -> " 
                      << last.finalCuriosity << " (change: " << curiosityChange << ")\n";
        }
        
        // Write results to output file if requested
        if (!outputFile.empty()) {
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << "Phase 3 Demo Results\n";
                outFile << "====================\n\n";
                outFile << "Total episodes: " << numEpisodes << "\n";
                outFile << "Steps per episode: " << stepsPerEpisode << "\n";
                outFile << "Total runtime: " << std::fixed << std::setprecision(2) << totalSeconds << " seconds\n";
                outFile << "Total steps executed: " << totalStepsExecuted << "\n";
                outFile << "\n";
                
                if (allMetrics.size() >= 2) {
                    const auto& first = allMetrics.front();
                    const auto& last = allMetrics.back();
                    
                    outFile << "Behavioral changes:\n";
                    outFile << "  Reward change: " << (last.totalReward - first.totalReward) << "\n";
                    outFile << "  Forward action change: " << (last.forwardCount - first.forwardCount) << "\n";
                    outFile << "  Synapse change: " << (last.totalSynapses - first.totalSynapses) << "\n";
                }
                
                outFile.close();
                std::cout << "\n[OUTPUT] Results written to: " << outputFile << std::endl;
            } else {
                std::cout << "\n[WARNING] Could not write results to: " << outputFile << std::endl;
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
        std::cout << "\nEnhanced features:\n";
        std::cout << "  - Checkpoint support\n";
        std::cout << "  - Performance monitoring\n";
        std::cout << "  - Detailed statistics\n";
        std::cout << "  - Output file support\n";
        std::cout << "  - Better error handling\n";
    }
    
private:
    std::shared_ptr<Config> config_;
    
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        config->set("max_energy", 100.0f);
        config->set("energy_decay_rate", 0.1f);
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
        config->set("development_factor", 1.5f);
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

// Enhanced main function with better CLI parsing
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    bool enableCheckpointing = false;
    bool enableDevelopment = true;
    std::string outputFile = "";
    
    std::cout << "NLM Phase 3 Enhanced Demo - Command Line Parser\n";
    std::cout << "==============================================\n\n";
    
    // Parse command line arguments with enhanced options
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_phase3_demo [OPTIONS]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --episodes N         Number of episodes (default: 5)\n";
            std::cout << "  --steps N             Steps per episode (default: 200)\n";
            std::cout << "  --checkpoint          Enable checkpoint saving\n";
            std::cout << "  --no-development      Disable development (default: enabled)\n";
            std::cout << "  --output FILE         Write results to FILE\n";
            std::cout << "  --help, -h            Show this help message\n\n";
            std::cout << "Examples:\n";
            std::cout << "  nlm_phase3_demo --episodes 10 --steps 500\n";
            std::cout << "  nlm_phase3_demo --checkpoint --output results.txt\n";
            std::cout << "  nlm_phase3_demo --episodes 3 --steps 1000 --no-development\n";
            return 0;
        }
        
        if (arg.substr(0, 12) == "--episodes=") {
            numEpisodes = std::atoi(arg.substr(12).c_str());
        } else if (arg.substr(0, 9) == "--steps=") {
            stepsPerEpisode = std::atoi(arg.substr(9).c_str());
        } else if (arg == "--checkpoint") {
            enableCheckpointing = true;
        } else if (arg == "--no-development") {
            enableDevelopment = false;
        } else if (arg.substr(0, 10) == "--output=") {
            outputFile = arg.substr(10);
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: nlm_phase3_demo [OPTIONS]\n\n";
            std::cout << "Options:\n";
            std::cout << "  --episodes N         Number of episodes (default: 5)\n";
            std::cout << "  --steps N             Steps per episode (default: 200)\n";
            std::cout << "  --checkpoint          Enable checkpoint saving\n";
            std::cout << "  --no-development      Disable development (default: enabled)\n";
            std::cout << "  --output FILE         Write results to FILE\n";
            std::cout << "  --help, -h            Show this help message\n\n";
            std::cout << "Examples:\n";
            std::cout << "  nlm_phase3_demo --episodes 10 --steps 500\n";
            std::cout << "  nlm_phase3_demo --checkpoint --output results.txt\n";
            std::cout << "  nlm_phase3_demo --episodes 3 --steps 1000 --no-development\n";
            return 0;
        } else {
            std::cout << "[WARNING] Unknown argument: " << arg << std::endl;
        }
    }
    
    // Validate parameters
    if (numEpisodes <= 0) {
        std::cerr << "ERROR: Number of episodes must be positive" << std::endl;
        return 1;
    }
    
    if (stepsPerEpisode <= 0) {
        std::cerr << "ERROR: Steps per episode must be positive" << std::endl;
        return 1;
    }
    
    std::cout << "Starting Phase 3 Enhanced Demo with the following parameters:\n";
    std::cout << "  Episodes: " << numEpisodes << "\n";
    std::cout << "  Steps per episode: " << stepsPerEpisode << "\n";
    std::cout << "  Checkpointing: " << (enableCheckpointing ? "Enabled" : "Disabled") << "\n";
    std::cout << "  Development: " << (enableDevelopment ? "Enabled" : "Disabled") << "\n";
    if (!outputFile.empty()) {
        std::cout << "  Output file: " << outputFile << "\n";
    }
    std::cout << std::endl;
    
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode, enableCheckpointing, enableDevelopment, outputFile);
    
    return 0;
}

} // namespace nlm

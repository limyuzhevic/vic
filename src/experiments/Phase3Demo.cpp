#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/Performance.hpp"
#include "../visualization/VisualizationInterface.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <chrono>
#include <iomanip>

namespace nlm {

// Phase 3 Demo - Enhanced World Interaction Demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning
// Enhanced with better error handling, modularity, and advanced features

class Phase3Demo {
public:
    Phase3Demo() = default;
    ~Phase3Demo() = default;
    
    // Configuration structure for Phase 3 demo
    struct DemoConfig {
        size_t numEpisodes = 5;
        size_t stepsPerEpisode = 200;
        size_t worldWidth = 20;
        size_t worldHeight = 20;
        size_t visionWidth = 16;
        size_t visionHeight = 16;
        size_t neuronCount = 500;
        size_t regionCount = 2;
        float connectionProbability = 0.1f;
        uint64_t randomSeed = 42;
        double simulationTimestep = 0.001;
        bool enableVerboseLogging = true;
        bool enableProgressReports = true;
        bool enableRealTimeVisualization = false;
        std::string outputFile = "";
        bool saveMetrics = false;
        
        // Validate configuration
        bool validate() const {
            if (numEpisodes == 0) return false;
            if (stepsPerEpisode == 0) return false;
            if (worldWidth == 0 || worldHeight == 0) return false;
            if (visionWidth == 0 || visionHeight == 0) return false;
            if (neuronCount == 0) return false;
            if (regionCount == 0) return false;
            if (connectionProbability < 0.0f || connectionProbability > 1.0f) return false;
            if (simulationTimestep <= 0.0) return false;
            return true;
        }
        
        // Get help text
        void printHelp() const {
            std::cout << "NLM Phase 3 Enhanced Demo Options:" << std::endl;
            std::cout << "  --episodes <count>          Number of episodes (default: 5)" << std::endl;
            std::cout << "  --steps <count>             Steps per episode (default: 200)" << std::endl;
            std::cout << "  --world-size <w> <h>        World dimensions (default: 20 20)" << std::endl;
            std::cout << "  --vision-size <w> <h>       Vision dimensions (default: 16 16)" << std::endl;
            std::cout << "  --neurons <count>           Number of neurons (default: 500)" << std::endl;
            std::cout << "  --regions <count>           Number of regions (default: 2)" << std::endl;
            std::cout << "  --connection-prob <float>    Connection probability (default: 0.1)" << std::endl;
            std::cout << "  --seed <seed>               Random seed for reproducibility (default: 42)" << std::endl;
            std::cout << "  --no-verbose                Disable verbose logging" << std::endl;
            std::cout << "  --no-progress                Disable progress reports" << std::endl;
            std::cout << "  --enable-viz                 Enable real-time visualization" << std::endl;
            std::cout << "  --output <file>             Save metrics to file" << std::endl;
            std::cout << "  --help, -h                  Show this help message" << std::endl;
        }
    };
    
    // Enhanced configuration parsing
    static DemoConfig parseCommandLineArgs(int argc, char* argv[]) {
        DemoConfig config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                config.printHelp();
                exit(0);
            }
            
            if (arg == "--no-verbose") {
                config.enableVerboseLogging = false;
            } else if (arg == "--no-progress") {
                config.enableProgressReports = false;
            } else if (arg == "--enable-viz") {
                config.enableRealTimeVisualization = true;
            } else if (arg.find("--episodes=") == 0) {
                try {
                    config.numEpisodes = std::stoull(arg.substr(11));
                } catch (...) {
                    std::cerr << "Error: Invalid episode count" << std::endl;
                }
            } else if (arg.find("--steps=") == 0) {
                try {
                    config.stepsPerEpisode = std::stoull(arg.substr(8));
                } catch (...) {
                    std::cerr << "Error: Invalid step count" << std::endl;
                }
            } else if (arg.find("--world-size=") == 0) {
                try {
                    std::string sizeStr = arg.substr(13);
                    size_t pos = sizeStr.find(' ');
                    if (pos != std::string::npos) {
                        config.worldWidth = std::stoull(sizeStr.substr(0, pos));
                        config.worldHeight = std::stoull(sizeStr.substr(pos + 1));
                    }
                } catch (...) {
                    std::cerr << "Error: Invalid world size format" << std::endl;
                }
            } else if (arg.find("--vision-size=") == 0) {
                try {
                    std::string sizeStr = arg.substr(15);
                    size_t pos = sizeStr.find(' ');
                    if (pos != std::string::npos) {
                        config.visionWidth = std::stoull(sizeStr.substr(0, pos));
                        config.visionHeight = std::stoull(sizeStr.substr(pos + 1));
                    }
                } catch (...) {
                    std::cerr << "Error: Invalid vision size format" << std::endl;
                }
            } else if (arg.find("--neurons=") == 0) {
                try {
                    config.neuronCount = std::stoull(arg.substr(10));
                } catch (...) {
                    std::cerr << "Error: Invalid neuron count" << std::endl;
                }
            } else if (arg.find("--regions=") == 0) {
                try {
                    config.regionCount = std::stoull(arg.substr(9));
                } catch (...) {
                    std::cerr << "Error: Invalid region count" << std::endl;
                }
            } else if (arg.find("--connection-prob=") == 0) {
                try {
                    config.connectionProbability = std::stof(arg.substr(20));
                } catch (...) {
                    std::cerr << "Error: Invalid connection probability" << std::endl;
                }
            } else if (arg.find("--seed=") == 0) {
                try {
                    config.randomSeed = std::stoull(arg.substr(7));
                } catch (...) {
                    std::cerr << "Error: Invalid seed" << std::endl;
                }
            } else if (arg.find("--output=") == 0) {
                config.outputFile = arg.substr(9);
                config.saveMetrics = true;
            }
        }
        
        return config;
    }
    
    void run(const DemoConfig& config) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - ENHANCED WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Create configuration
            auto configObj = std::make_shared<Config>();
            configureEnvironment(configObj, config);
            configureBrain(configObj, config);
            
            // Create world with error handling
            SimpleWorld world;
            if (!createWorld(world, config)) {
                throw std::runtime_error("Failed to create world");
            }
            
            // Create brain with error handling
            auto brain = createBrain(configObj);
            if (!brain->initialize()) {
                throw std::runtime_error("Failed to initialize brain");
            }
            
            // Create agent brain interface
            AgentBrain agentBrain(brain);
            if (!initializeAgent(agentBrain, world, config)) {
                throw std::runtime_error("Failed to initialize agent brain");
            }
            
            // Setup visualization if requested
            std::shared_ptr<VisualizationInterface> visualization;
            if (config.enableRealTimeVisualization) {
                visualization = createVisualization();
            }
            
            // Print initial status
            printInitialStatus(brain, agentBrain);
            
            // Run episodes with enhanced error handling
            std::vector<ExperimentMetrics> allMetrics;
            if (!runEpisodes(world, agentBrain, brain, config, allMetrics, visualization)) {
                throw std::runtime_error("Failed to run episodes");
            }
            
            // Save metrics if requested
            if (config.saveMetrics) {
                saveMetrics(allMetrics, config.outputFile);
            }
            
            // Print final summary
            printFinalSummary(allMetrics, startTime);
            
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Exception during Phase 3 demo: " << e.what() << std::endl;
            throw;
        } catch (...) {
            std::cerr << "[ERROR] Unknown exception during Phase 3 demo" << std::endl;
            throw;
        }
    }
    
private:
    // Enhanced configuration methods
    void configureEnvironment(std::shared_ptr<Config> config, const DemoConfig& demoConfig) {
        config->set("environment_width", demoConfig.worldWidth);
        config->set("environment_height", demoConfig.worldHeight);
        config->set("vision_width", demoConfig.visionWidth);
        config->set("vision_height", demoConfig.visionHeight);
    }
    
    void configureBrain(std::shared_ptr<Config> config, const DemoConfig& demoConfig) {
        config->set("neuron_count", static_cast<int64_t>(demoConfig.neuronCount));
        config->set("region_count", static_cast<int64_t>(demoConfig.regionCount));
        config->set("connection_probability", demoConfig.connectionProbability);
        config->set("random_seed", static_cast<int64_t>(demoConfig.randomSeed));
        config->set("simulation_timestep", demoConfig.simulationTimestep);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
    
    bool createWorld(SimpleWorld& world, const DemoConfig& config) {
        try {
            world.configure(static_cast<float>(config.worldWidth),
                           static_cast<float>(config.worldHeight),
                           config.visionWidth, config.visionHeight);
            world.setRandomSeed(config.randomSeed);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }
    
    bool initializeAgent(AgentBrain& agentBrain, const SimpleWorld& world, const DemoConfig& config) {
        try {
            agentBrain.initialize(world);
            agentBrain.enableRewardModulation(true);
            agentBrain.enableStructuralPlasticity(true);
            agentBrain.enableDevelopment(true);
            agentBrain.enableCuriosity(true);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    std::shared_ptr<VisualizationInterface> createVisualization() {
        return std::make_shared<VisualizationInterface>();
    }
    
    void printInitialStatus(const std::shared_ptr<Brain>& brain, const AgentBrain& agentBrain) {
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
    }
    
    bool runEpisodes(SimpleWorld& world, AgentBrain& agentBrain, 
                    const std::shared_ptr<Brain>& brain, const DemoConfig& config,
                    std::vector<ExperimentMetrics>& allMetrics,
                    const std::shared_ptr<VisualizationInterface>& visualization) {
        try {
            for (size_t episode = 0; episode < config.numEpisodes; ++episode) {
                if (!runSingleEpisode(world, agentBrain, brain, config, allMetrics, episode, visualization)) {
                    return false;
                }
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool runSingleEpisode(SimpleWorld& world, AgentBrain& agentBrain,
                         const std::shared_ptr<Brain>& brain, const DemoConfig& config,
                         std::vector<ExperimentMetrics>& allMetrics, size_t episode,
                         const std::shared_ptr<VisualizationInterface>& visualization) {
        std::cout << "--- Episode " << episode << " started ---\n";
        
        // Reset episode state
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        ExperimentMetrics metrics;
        metrics.episode = episode;
        
        float episodeReward = 0.0f;
        
        for (size_t step = 0; step < config.stepsPerEpisode; ++step) {
            if (!runSingleStep(world, agentBrain, brain, config, metrics, step, episode, visualization)) {
                return false;
            }
            
            episodeReward += metrics.totalReward;
            
            // Print progress if requested
            if (config.enableProgressReports && step % 50 == 0) {
                printProgress(step, metrics, agentBrain, world);
            }
        }
        
        // Record final metrics
        recordFinalMetrics(metrics, agentBrain, brain);
        allMetrics.push_back(metrics);
        
        printEpisodeSummary(episode, metrics);
        return true;
    }
    
    bool runSingleStep(SimpleWorld& world, AgentBrain& agentBrain,
                      const std::shared_ptr<Brain>& brain, const DemoConfig& config,
                      ExperimentMetrics& metrics, size_t step, size_t episode,
                      const std::shared_ptr<VisualizationInterface>& visualization) {
        try {
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
            
            // Update visualization if requested
            if (visualization) {
                visualization->update(brain.get(), &world, step, episode);
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
    void printProgress(size_t step, const ExperimentMetrics& metrics, 
                      const AgentBrain& agentBrain, const SimpleWorld& world) {
        std::cout << "  Step " << step 
                  << ": reward=" << metrics.totalReward
                  << ", action=" << motorCommandToString(MotorCommand::Wait)  // Placeholder
                  << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                  << ", novelty=" << agentBrain.getNoveltyLevel()
                  << ", energy=" << world.getAgentBody().energy
                  << "\n";
    }
    
    void recordFinalMetrics(ExperimentMetrics& metrics, const AgentBrain& agentBrain,
                           const std::shared_ptr<Brain>& brain) {
        metrics.finalNovelty = agentBrain.getNoveltyLevel();
        metrics.finalCuriosity = agentBrain.getCuriosityLevel();
        metrics.finalEnergy = world.getAgentBody().energy;
        metrics.predictionError = agentBrain.getPredictionError();
        metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount();  // Use as proxy
        metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
        metrics.totalSynapses = brain->getTotalSynapseCount();
        metrics.averageFiringRate = brain->getAverageFiringRate();
    }
    
    void printEpisodeSummary(size_t episode, const ExperimentMetrics& metrics) {
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
    
    void saveMetrics(const std::vector<ExperimentMetrics>& allMetrics, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[WARNING] Could not open file for saving metrics: " << filename << std::endl;
            return;
        }
        
        file << "Phase 3 Demo Metrics\n";
        file << "===================\n\n";
        
        for (const auto& metrics : allMetrics) {
            file << "Episode " << metrics.episode << ":\n";
            file << "  Steps: " << metrics.steps << "\n";
            file << "  Total Reward: " << metrics.totalReward << "\n";
            file << "  Final Novelty: " << metrics.finalNovelty << "\n";
            file << "  Final Curiosity: " << metrics.finalCuriosity << "\n";
            file << "  Final Energy: " << metrics.finalEnergy << "\n";
            file << "  Prediction Error: " << metrics.predictionError << "\n";
            file << "  Developmental Age: " << metrics.developmentalAge << "\n";
            file << "  Developmental Stage: " << metrics.developmentalStage << "\n";
            file << "  Total Synapses: " << metrics.totalSynapses << "\n";
            file << "  Average Firing Rate: " << metrics.averageFiringRate << "\n";
            file << "  Action Counts: fwd=" << metrics.forwardCount
                 << ", bwd=" << metrics.backwardCount
                 << ", left=" << metrics.turnLeftCount
                 << ", right=" << metrics.turnRightCount
                 << ", interact=" << metrics.interactCount
                 << ", wait=" << metrics.waitCount << "\n\n";
        }
        
        file.close();
    }
    
    void printFinalSummary(const std::vector<ExperimentMetrics>& allMetrics, 
                          const std::chrono::high_resolution_clock::time_point& startTime) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
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
        std::cout << "\nTotal runtime: " << duration.count() << " seconds.\n";
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

// Main entry point for Phase 3 demo with enhanced configuration
int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        Phase3Demo::DemoConfig config = Phase3Demo::parseCommandLineArgs(argc, argv);
        
        // Validate configuration
        if (!config.validate()) {
            std::cerr << "[ERROR] Invalid configuration provided" << std::endl;
            return 1;
        }
        
        // Run enhanced demo
        Phase3Demo demo;
        demo.run(config);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Fatal exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[ERROR] Unknown fatal exception" << std::endl;
        return 1;
    }
}

} // namespace nlm

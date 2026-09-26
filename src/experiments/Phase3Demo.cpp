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
#include <stdexcept>
#include <cstdint>

using namespace nlm;

// Custom error classes for Phase 3
class Phase3Error : public std::runtime_error {
public:
    explicit Phase3Error(const std::string& msg) : std::runtime_error("Phase 3 Error: " + msg) {}
};

class Phase3DemoError : public Phase3Error {
public:
    explicit Phase3DemoError(const std::string& msg) : Phase3Error("Demo Error: " + msg) {}
};

struct Phase3DemoConfig {
    uint64_t numEpisodes = 5;
    uint64_t stepsPerEpisode = 200;
    uint64_t worldWidth = 20;
    uint64_t worldHeight = 20;
    uint64_t visionWidth = 16;
    uint64_t visionHeight = 16;
    uint64_t neuronCount = 500;
    uint64_t regionCount = 2;
    float connectionProbability = 0.1f;
    uint64_t randomSeed = 42;
    double timestep = 0.001;
    float stdpLTPWeight = 0.01f;
    float stdpLTDWeight = 0.012f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    bool verbose = false;
    std::string outputFile;
    bool saveResults = false;
    bool enableDebugOutput = false;
    
    void applyToConfig(std::shared_ptr<Config> config) const {
        if (!config) return;
        
        config->set("environment_width", worldWidth);
        config->set("environment_height", worldHeight);
        config->set("vision_width", visionWidth);
        config->set("vision_height", visionHeight);
        config->set("neuron_count", neuronCount);
        config->set("region_count", regionCount);
        config->set("connection_probability", connectionProbability);
        config->set("random_seed", static_cast<int64_t>(randomSeed));
        config->set("simulation_timestep", timestep);
        config->set("stdp_ltp_weight", stdpLTPWeight);
        config->set("stdp_ltd_weight", stdpLTDWeight);
        config->set("stdp_tau", stdpTau);
        config->set("synaptogenesis_rate", synaptogenesisRate);
        config->set("pruning_rate", pruningRate);
    }
};

class CommandLineParser {
public:
    static Phase3DemoConfig parse(int argc, char* argv[]) {
        Phase3DemoConfig config;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printUsage();
                exit(0);
            }
            else if (arg == "--quick") {
                config.numEpisodes = 2;
                config.stepsPerEpisode = 50;
            }
            else if (arg == "--verbose" || arg == "-v") {
                config.verbose = true;
            }
            else if (arg.substr(0, 4) == "--episodes=") {
                try {
                    config.numEpisodes = std::stoull(arg.substr(9));
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid episodes value");
                }
            }
            else if (arg == "--episodes" && i + 1 < argc) {
                try {
                    config.numEpisodes = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid episodes value");
                }
            }
            else if (arg.substr(0, 4) == "--steps=") {
                try {
                    config.stepsPerEpisode = std::stoull(arg.substr(9));
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid steps value");
                }
            }
            else if (arg == "--steps" && i + 1 < argc) {
                try {
                    config.stepsPerEpisode = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid steps value");
                }
            }
            else if (arg.substr(0, 6) == "--seed=") {
                try {
                    config.randomSeed = std::stoull(arg.substr(7));
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid seed value");
                }
            }
            else if (arg == "--seed" && i + 1 < argc) {
                try {
                    config.randomSeed = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid seed value");
                }
            }
            else if (arg.substr(0, 4) == "--neurons=") {
                try {
                    config.neuronCount = std::stoull(arg.substr(9));
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid neuron count");
                }
            }
            else if (arg == "--neurons" && i + 1 < argc) {
                try {
                    config.neuronCount = std::stoull(argv[++i]);
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid neuron count");
                }
            }
            else if (arg.substr(0, 4) == "--world=") {
                try {
                    config.worldWidth = std::stoull(arg.substr(7));
                    config.worldHeight = config.worldWidth; // Keep it square
                } catch (const std::exception& e) {
                    throw Phase3DemoError("Invalid world size");
                }
            }
            else if (arg.substr(0, 6) == "--output=") {
                config.outputFile = arg.substr(8);
                config.saveResults = true;
            }
            else if (arg == "--output" && i + 1 < argc) {
                config.outputFile = argv[++i];
                config.saveResults = true;
            }
        }
        
        return config;
    }
    
private:
    static void printUsage() {
        std::cout << "Phase 3 Demo - World Interaction\n\n";
        std::cout << "Usage: nlm_phase3_demo [OPTIONS]\n\n";
        std::cout << "Options:\n";
        std::cout << "  -h, --help              Show help\n";
        std::cout << "  --quick                 Quick demo (2 episodes, 50 steps)\n";
        std::cout << "  -v, --verbose           Verbose output\n";
        std::cout << "  --episodes=<num>       Number of episodes (default: 5)\n";
        std::cout << "  --episodes <num>        (alternative form)\n";
        std::cout << "  --steps=<num>           Steps per episode (default: 200)\n";
        std::cout << "  --seed=<num>            Random seed (default: 42)\n";
        std::cout << "  --neurons=<num>         Neuron count (default: 500)\n";
        std::cout << "  --world=<size>          World size (default: 20x20)\n";
        std::cout << "  --output=<file>         Save results to file\n";
        std::cout << "\nExamples:\n";
        std::cout << "  nlm_phase3_demo                     # Run full demo\n";
        std::cout << "  nlm_phase3_demo --quick            # Quick demo\n";
        std::cout << "  nlm_phase3_demo --episodes=3 --steps=100\n";
    }
};

class MetricsCollector {
public:
    struct EpisodeMetrics {
        uint64_t episode;
        uint64_t steps;
        float totalReward;
        uint64_t totalSpikes;
        uint64_t firingNeuronCount;
        uint64_t forwardCount;
        uint64_t backwardCount;
        uint64_t turnLeftCount;
        uint64_t turnRightCount;
        uint64_t interactCount;
        uint64_t waitCount;
        float novelty;
        float curiosity;
        float predictionError;
        uint64_t developmentalAge;
        float finalEnergy;
        float averageFiringRate;
    };
    
    std::vector<EpisodeMetrics> episodes;
    
    void addEpisode(const EpisodeMetrics& metrics) {
        episodes.push_back(metrics);
    }
    
    float getAverageReward() const {
        if (episodes.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& ep : episodes) {
            sum += ep.totalReward;
        }
        return sum / episodes.size();
    }
    
    float getAverageFiringRate() const {
        if (episodes.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& ep : episodes) {
            sum += ep.averageFiringRate;
        }
        return sum / episodes.size();
    }
    
    void saveToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw Phase3DemoError("Cannot open output file: " + filename);
        }
        
        file << "Episode Metrics for Phase 3 Demo\n";
        file << "=================================" << std::endl;
        
        for (const auto& ep : episodes) {
            file << "\nEpisode " << ep.episode << ":\n";
            file << "  Steps: " << ep.steps << std::endl;
            file << "  Total Reward: " << ep.totalReward << std::endl;
            file << "  Total Spikes: " << ep.totalSpikes << std::endl;
            file << "  Firing Neurons: " << ep.firingNeuronCount << std::endl;
            file << "  Average Firing Rate: " << ep.averageFiringRate << std::endl;
            file << "  Actions - Fwd: " << ep.forwardCount 
                 << " Bwd: " << ep.backwardCount
                 << " Left: " << ep.turnLeftCount
                 << " Right: " << ep.turnRightCount
                 << " Interact: " << ep.interactCount
                 << " Wait: " << ep.waitCount << std::endl;
            file << "  Novelty: " << ep.novelty << std::endl;
            file << "  Curiosity: " << ep.curiosity << std::endl;
            file << "  Prediction Error: " << ep.predictionError << std::endl;
            file << "  Developmental Age: " << ep.developmentalAge << std::endl;
            file << "  Final Energy: " << ep.finalEnergy << std::endl;
        }
        
        file << "\n\nSummary:\n";
        file << "Average Reward: " << getAverageReward() << std::endl;
        file << "Average Firing Rate: " << getAverageFiringRate() << std::endl;
        file << "Total Episodes: " << episodes.size() << std::endl;
    }
};

class Phase3Demo {
public:
    Phase3Demo() : config(), metrics(), world(), agentBrain(nullptr), brain(nullptr) {}
    ~Phase3Demo() = default;
    
    void run(const Phase3DemoConfig& cfg) {
        config = cfg;
        
        try {
            if (config.verbose) {
                std::cout << "Starting Phase 3 Demo with " << config.numEpisodes 
                         << " episodes of " << config.stepsPerEpisode << " steps each." << std::endl;
            }
            
            // Configure and initialize systems
            configureSystems();
            initializeSystems();
            
            // Run episodes
            runEpisodes();
            
            // Save results if requested
            if (config.saveResults && !config.outputFile.empty()) {
                metrics.saveToFile(config.outputFile);
                if (config.verbose) {
                    std::cout << "Results saved to: " << config.outputFile << std::endl;
                }
            }
            
            if (config.verbose) {
                printSummary();
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error in Phase 3 demo: " << e.what() << std::endl;
            throw;
        }
    }
    
private:
    Phase3DemoConfig config;
    MetricsCollector metrics;
    SimpleWorld world;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agentBrain;
    
    void configureSystems() {
        // Create configuration
        auto cfg = std::make_shared<Config>();
        config.applyToConfig(cfg);
        
        // Store configuration for later use
        this->config.applyToConfig(cfg);
        
        if (config.verbose) {
            std::cout << "Systems configured:" << std::endl;
            std::cout << "  World: " << config.worldWidth << "x" << config.worldHeight << std::endl;
            std::cout << "  Vision: " << config.visionWidth << "x" << config.visionHeight << std::endl;
            std::cout << "  Brain: " << config.neuronCount << " neurons, " << config.regionCount << " regions" << std::endl;
            std::cout << "  Episodes: " << config.numEpisodes << ", Steps per episode: " << config.stepsPerEpisode << std::endl;
        }
    }
    
    void initializeSystems() {
        // Create world
        world.configure(config.worldWidth, config.worldHeight, 
                       config.visionWidth, config.visionHeight);
        world.setRandomSeed(config.randomSeed);
        world.reset();
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        config.applyToConfig(cfg);
        
        // Create brain
        brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            throw Phase3DemoError("Failed to initialize brain");
        }
        
        // Create agent brain interface
        agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(world);
        
        // Configure agent brain features
        agentBrain->enableRewardModulation(true);
        agentBrain->enableStructuralPlasticity(true);
        agentBrain->enableDevelopment(true);
        agentBrain->enableCuriosity(true);
        
        if (config.verbose) {
            std::cout << "Systems initialized:" << std::endl;
            std::cout << "  World: " << world.getWidth() << "x" << world.getHeight() << std::endl;
            std::cout << "  Brain: " << brain->getTotalNeuronCount() << " neurons" << std::endl;
            std::cout << "  Agent sensory input size: " << agentBrain->getSensoryInputSize() << std::endl;
            std::cout << "  Agent motor output size: " << agentBrain->getMotorOutputSize() << std::endl;
        }
    }
    
    void runEpisodes() {
        for (uint64_t episode = 0; episode < config.numEpisodes; ++episode) {
            if (config.verbose) {
                std::cout << "--- Episode " << episode << " started ---" << std::endl;
            }
            
            // Reset world and brain for new episode
            world.reset();
            agentBrain->reset();
            brain->reset();
            brain->initialize();
            
            EpisodeMetrics metricsForEpisode;
            metricsForEpisode.episode = episode;
            metricsForEpisode.steps = 0;
            metricsForEpisode.totalReward = 0.0f;
            metricsForEpisode.totalSpikes = 0;
            metricsForEpisode.firingNeuronCount = 0;
            metricsForEpisode.averageFiringRate = 0.0f;
            
            // Run steps for this episode
            for (uint64_t step = 0; step < config.stepsPerEpisode; ++step) {
                // 1. Update world
                world.update(config.timestep);
                
                // 2. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 3. Process sensory input with agent
                agentBrain->processSensoryInput(percept);
                
                // 4. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    brain->step(neuralStep);
                }
                
                // 5. Decode motor command from brain activity
                MotorCommand cmd = agentBrain->decodeMotorCommand();
                
                // 6. Apply action to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 7. Apply reward modulation
                agentBrain->applyRewardModulation(result.reward, 0.0f);
                
                // 8. Update development
                agentBrain->updateDevelopment(config.timestep);
                
                // Track metrics
                metricsForEpisode.steps++;
                metricsForEpisode.totalReward += result.reward;
                metricsForEpisode.totalSpikes += brain->getFiringNeuronCount();
                metricsForEpisode.firingNeuronCount = brain->getFiringNeuronCount();
                metricsForEpisode.averageFiringRate += brain->getAverageFiringRate();
                
                // Count actions
                switch (cmd) {
                    case MotorCommand::MoveForward: metricsForEpisode.forwardCount++; break;
                    case MotorCommand::MoveBackward: metricsForEpisode.backwardCount++; break;
                    case MotorCommand::TurnLeft: metricsForEpisode.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metricsForEpisode.turnRightCount++; break;
                    case MotorCommand::LookLeft: metricsForEpisode.turnLeftCount++; break;
                    case MotorCommand::LookRight: metricsForEpisode.turnRightCount++; break;
                    case MotorCommand::Interact: metricsForEpisode.interactCount++; break;
                    case MotorCommand::Wait: metricsForEpisode.waitCount++; break;
                    default: metricsForEpisode.waitCount++; break;
                }
                
                // Print periodic updates
                if (config.verbose && step % 50 == 0) {
                    std::cout << "  Step " << step 
                             << ": reward=" << result.reward
                             << ", action=" << motorCommandToString(cmd)
                             << ", neuromod=" << agentBrain->getNeuromodulationLevel()
                             << ", novelty=" << agentBrain->getNoveltyLevel()
                             << ", energy=" << world.getAgentBody().energy
                             << std::endl;
                }
            }
            
            // Calculate average firing rate for this episode
            if (metricsForEpisode.steps > 0) {
                metricsForEpisode.averageFiringRate /= metricsForEpisode.steps;
            }
            
            // Get final metrics
            metricsForEpisode.novelty = agentBrain->getNoveltyLevel();
            metricsForEpisode.curiosity = agentBrain->getCuriosityLevel();
            metricsForEpisode.predictionError = agentBrain->getPredictionError();
            metricsForEpisode.developmentalAge = brain->getTotalSynapseCount();
            metricsForEpisode.finalEnergy = world.getAgentBody().energy;
            
            // Add to metrics collector
            metrics.addEpisode(metricsForEpisode);
            
            if (config.verbose) {
                std::cout << "--- Episode " << episode << " ended ---" << std::endl;
                std::cout << "  Steps: " << metricsForEpisode.steps << std::endl;
                std::cout << "  Total Reward: " << metricsForEpisode.totalReward << std::endl;
                std::cout << "  Final Energy: " << metricsForEpisode.finalEnergy << std::endl;
                std::cout << "  Synapses: " << metricsForEpisode.developmentalAge << std::endl;
                std::cout << "  Action counts: fwd=" << metricsForEpisode.forwardCount 
                         << ", bwd=" << metricsForEpisode.backwardCount
                         << ", left=" << metricsForEpisode.turnLeftCount
                         << ", right=" << metricsForEpisode.turnRightCount
                         << ", interact=" << metricsForEpisode.interactCount
                         << ", wait=" << metricsForEpisode.waitCount << std::endl;
            }
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
    
    void printSummary() {
        std::cout << "\n=== Phase 3 Demo Summary ===" << std::endl;
        std::cout << "Total Episodes: " << config.numEpisodes << std::endl;
        std::cout << "Average Reward: " << metrics.getAverageReward() << std::endl;
        std::cout << "Average Firing Rate: " << metrics.getAverageFiringRate() << std::endl;
        
        if (metrics.episodes.size() >= 2) {
            const auto& first = metrics.episodes.front();
            const auto& last = metrics.episodes.back();
            
            std::cout << "\nEarly episode vs Late episode behavior change:" << std::endl;
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << std::endl;
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << std::endl;
            
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << std::endl;
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
    }
};

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    try {
        Phase3DemoConfig config;
        
        // Parse command line arguments
        if (argc > 1) {
            config = CommandLineParser::parse(argc, argv);
        }
        
        // Create and run demo
        Phase3Demo demo;
        demo.run(config);
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

namespace nlm {
    // Additional helper functions for Phase 3
    std::string developmentalStageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
}
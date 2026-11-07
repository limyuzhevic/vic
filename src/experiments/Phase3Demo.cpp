// Improved Phase 3 Demo with Enhanced CLI and Configuration
// 
// This demo shows: experience -> neural activity -> action -> consequence -> learning
// 
// Key improvements:
// - Comprehensive command-line interface with multiple options
// - Configuration file support
// - Environment/world configuration options
// - Brain configuration options
// - Better error handling and validation
// - Detailed help and usage information
// - Progress reporting and metrics

#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>

namespace nlm {

// Command-line argument structure
struct DemoArgs {
    std::string configFile = "";
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    int worldWidth = 20;
    int worldHeight = 20;
    int visionWidth = 16;
    int visionHeight = 16;
    int neuronCount = 500;
    int regionCount = 2;
    float connectionProbability = 0.1f;
    int64_t randomSeed = 42;
    float simulationTimestep = 0.001f;
    float stdpLtpWeight = 0.01f;
    float stdpLtdWeight = 0.012f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    bool enableRewardModulation = true;
    bool enableStructuralPlasticity = true;
    bool enableDevelopment = true;
    bool enableCuriosity = true;
    bool enableInstrumentation = false;
    std::string outputFile = "";
    bool verbose = false;
    bool quiet = false;
    
    // Parse command line arguments
    static DemoArgs parseArgs(int argc, char* argv[]);
    
    // Print help information
    static void printHelp(const char* programName);
    
    // Print current configuration
    void printConfiguration() const;
};

class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    // Run demo with enhanced arguments
    void run(const DemoArgs& args) {
        if (!quiet) {
            std::cout << "===========================================\n";
            std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
            std::cout << "===========================================\n\n";
            
            std::cout << "Configuration:" << std::endl;
            args.printConfiguration();
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Load from config file if specified
        if (!args.configFile.empty()) {
            if (config->loadFromFile(args.configFile)) {
                if (!quiet) {
                    std::cout << "Loaded configuration from: " << args.configFile << std::endl;
                }
            } else {
                std::cerr << "Warning: Could not load configuration from " << args.configFile 
                          << ". Using defaults." << std::endl;
            }
        }
        
        // Override with command-line arguments (with proper priority)
        overrideConfiguration(config, args);
        
        if (!quiet) {
            std::cout << "Final configuration:" << std::endl;
            std::cout << "  " << config->summary() << std::endl;
        }
        
        // Create world
        SimpleWorld world;
        world.configure(args.worldWidth, args.worldHeight, args.visionWidth, args.visionHeight);
        world.setRandomSeed(static_cast<unsigned int>(args.randomSeed));
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain!" << std::endl;
            return;
        }
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(args.enableRewardModulation);
        agentBrain.enableStructuralPlasticity(args.enableStructuralPlasticity);
        agentBrain.enableDevelopment(args.enableDevelopment);
        agentBrain.enableCuriosity(args.enableCuriosity);
        
        if (!quiet) {
            std::cout << "Initial brain state:" << std::endl;
            std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << std::endl;
            std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << std::endl;
            std::cout << std::endl;
        }
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Run episodes
        for (int episode = 0; episode < args.numEpisodes; ++episode) {
            if (!quiet) {
                std::cout << "--- Episode " << episode << " started ---" << std::endl;
            }
            
            // Reset world and agent
            world.reset();
            agentBrain.reset();
            brain->reset();
            
            ExperimentMetrics metrics;
            metrics.episode = episode;
            
            // Phase 5 navigation/interact metrics
            std::set<std::pair<int, int>> visitedCells;
            int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
            int tooSoonCount = 0;
            int oscillations = 0;
            MotorCommand prevMove = MotorCommand::Wait;
            bool hasPrevMove = false;
            
            float episodeReward = 0.0f;
            uint64_t brainStepCounter = 0;
            
            // Run steps
            for (int step = 0; step < args.stepsPerEpisode; ++step) {
                // 1. Get sensory percept from world
                const SensoryPercept& percept = world.getSensoryPercept();
                
                // 2. Inject sensory input into brain
                agentBrain.processSensoryInput(percept);
                
                // 3. Simulate brain (multiple steps per action)
                for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                    (void)neuralStep;
                    brain->step(brainStepCounter++);
                }
                
                // 4. Decode motor command from brain activity
                MotorCommand cmd = agentBrain.decodeMotorCommand();
                
                // 5. Apply motor command to world
                ActionResult result = world.applyMotorCommand(cmd, world.getSimulationTime());
                
                // 6. Apply reward modulation
                agentBrain.applyRewardModulation(result.reward, 0.0f);
                
                // 7. Update world
                world.update(0.1);
                
                // 8. Update development
                agentBrain.updateDevelopment(0.1);
                
                // Track metrics
                episodeReward += result.reward;
                metrics.steps++;
                metrics.totalReward += result.reward;
                metrics.totalSpikes += brain->getFiringNeuronCount();
                
                // Phase 5 metrics
                {
                    const auto& body = world.getAgentBody();
                    visitedCells.insert({static_cast<int>(std::floor(body.x)),
                                        static_cast<int>(std::floor(body.y))});
                }
                if (result.message == "too soon") ++tooSoonCount;
                if (cmd == MotorCommand::Interact) {
                    if (result.message == "consumed_resource") ++resourcesConsumed;
                    else if (result.message == "hit_hazard") ++hazardsHit;
                    else ++emptyInteracts;
                }
                if (cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward) {
                    if (hasPrevMove && prevMove != cmd) ++oscillations;
                    prevMove = cmd;
                    hasPrevMove = true;
                }
                
                // Count actions
                switch (cmd) {
                    case MotorCommand::MoveForward: metrics.forwardCount++; break;
                    case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                    case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                    case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                    case MotorCommand::Interact: metrics.interactCount++; break;
                    case MotorCommand::LookLeft: metrics.lookLeftCount++; break;
                    case MotorCommand::LookRight: metrics.lookRightCount++; break;
                    default: metrics.waitCount++; break;
                }
                
                // Print periodic updates (except when quiet)
                if (!quiet && step % 50 == 0) {
                    std::cout << "  Step " << step 
                              << ": reward=" << result.reward
                              << ", action=" << motorCommandToString(cmd)
                              << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                              << ", novelty=" << agentBrain.getNoveltyLevel()
                              << ", energy=" << world.getAgentBody().energy
                              << std::endl;
                }
                
                // Instrumentation (opt-in)
                if (args.enableInstrumentation && episode == 0) {
                    auto acts = agentBrain.getMotorGroupActivities();
                    size_t visionSeen = 0;
                    for (float v : percept.getVision()) {
                        if (v < 0.999f) ++visionSeen;
                    }
                    std::cout << "INSTR step=" << step
                              << " firing=" << brain->getFiringNeuronCount()
                              << " avgRate=" << brain->getAverageFiringRate()
                              << " visionSeen=" << visionSeen << "/"
                              << percept.getVision().size();
                    for (const auto& a : acts) {
                        std::cout << " " << a.name << "=" << a.activity
                                  << "/rate=" << a.firingRate
                                  << "(n=" << a.size << ",refr=" << a.refractoryCount << ")";
                    }
                    std::cout << " cmd=" << motorCommandToString(cmd) << std::endl;
                }
            }
            
            // Record final metrics
            metrics.finalNovelty = agentBrain.getNoveltyLevel();
            metrics.finalCuriosity = agentBrain.getCuriosityLevel();
            metrics.finalEnergy = world.getAgentBody().energy;
            metrics.predictionError = agentBrain.getPredictionError();
            metrics.developmentalAge = agentBrain.getBrain()->getTotalSynapseCount(); // Use as proxy
            metrics.developmentalStage = stageToString(agentBrain.getDevelopmentalStage());
            metrics.totalSynapses = brain->getTotalSynapseCount();
            metrics.averageFiringRate = brain->getAverageFiringRate();
            
            allMetrics.push_back(metrics);
            
            if (!quiet) {
                std::cout << "--- Episode " << episode << " ended ---" << std::endl;
                std::cout << "  Steps: " << metrics.steps << std::endl;
                std::cout << "  Total Reward: " << metrics.totalReward << std::endl;
                std::cout << "  Final Energy: " << metrics.finalEnergy << std::endl;
                std::cout << "  Synapses: " << metrics.totalSynapses << std::endl;
                std::cout << "  Action counts: fwd=" << metrics.forwardCount
                          << ", bwd=" << metrics.backwardCount
                          << ", left=" << metrics.turnLeftCount
                          << ", right=" << metrics.turnRightCount
                          << ", interact=" << metrics.interactCount
                          << ", wait=" << metrics.waitCount
                          << ", lookL=" << metrics.lookLeftCount
                          << ", lookR=" << metrics.lookRightCount
                          << ", avgFiringRate=" << metrics.averageFiringRate
                          << std::endl;
                std::cout << "  Phase5 metrics: visitedCells=" << visitedCells.size()
                          << ", resources=" << resourcesConsumed
                          << ", hazards=" << hazardsHit
                          << ", emptyInteracts=" << emptyInteracts
                          << ", oscillations=" << oscillations
                          << ", finalNovelty=" << metrics.finalNovelty
                          << ", finalCuriosity=" << metrics.finalCuriosity
                          << ", tooSoon=" << tooSoonCount
                          << ", executed=" << (args.stepsPerEpisode - tooSoonCount)
                          << ", reflexBonus=" << agentBrain.getReflexBonusCount()
                          << ", reflexSuppress=" << agentBrain.getReflexSuppressCount()
                          << ", wtaPulses=" << agentBrain.getWtaPulseCount()
                          << ", plannerOverride=" << agentBrain.getPlannerOverrideCount()
                          << ", plannerAgree=" << agentBrain.getPlannerAgreementCount()
                          << "/" << agentBrain.getPlannerDecisionCount()
                          << ", conceptBias=" << agentBrain.getConceptBiasCount()
                          << ", selfBias=" << agentBrain.getSelfBiasCount()
                          << ", socialBias=" << agentBrain.getSocialBiasCount()
                          << ", concepts=" << agentBrain.getConceptCount()
                          << ", selfConf=" << agentBrain.getSelfModelConfidence()
                          << ", wmTraces=" << (brain->getWorkingMemory() ? brain->getWorkingMemory()->getActiveTraces() : 0)
                          << std::endl << std::endl;
            }
        }
        
        // Summary
        if (!quiet) {
            std::cout << "===========================================\n";
            std::cout << "SUMMARY\n";
            std::cout << "===========================================\n";
        }
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            if (!quiet) {
                std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):" << std::endl;
                std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << std::endl;
                std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << std::endl;
                std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << std::endl;
            }
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            if (!quiet) {
                std::cout << std::endl;
                std::cout << "Behavioral change:" << std::endl;
            }
            
            if (rewardChange > 0) {
                if (!quiet) {
                    std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)" << std::endl;
                }
            } else if (rewardChange < -5) {
                if (!quiet) {
                    std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)" << std::endl;
                }
            }
            
            if (std::abs(actionChange) > 10) {
                if (!quiet) {
                    std::cout << "  Forward actions changed by " << actionChange << std::endl;
                }
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                if (!quiet) {
                    std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << std::endl;
                }
            }
        }
        
        if (!quiet) {
            std::cout << std::endl;
            std::cout << "Phase 3 demonstration complete.\n";
            std::cout << "The brain received sensory input, produced actions,\n";
            std::cout << "experienced consequences, and modified its synapses.\n";
        }
        
        // Save metrics if output file specified
        if (!args.outputFile.empty()) {
            saveMetrics(args.outputFile, allMetrics);
        }
    }
    
private:
    void overrideConfiguration(std::shared_ptr<Config> config, const DemoArgs& args) {
        // Environment settings
        config->set("environment_width", args.worldWidth);
        config->set("environment_height", args.worldHeight);
        config->set("vision_width", args.visionWidth);
        config->set("vision_height", args.visionHeight);
        
        // Brain settings
        config->set("neuron_count", args.neuronCount);
        config->set("region_count", args.regionCount);
        config->set("connection_probability", args.connectionProbability);
        config->set("random_seed", args.randomSeed);
        config->set("simulation_timestep", args.simulationTimestep);
        config->set("stdp_ltp_weight", args.stdpLtpWeight);
        config->set("stdp_ltd_weight", args.stdpLtdWeight);
        config->set("stdp_tau", args.stdpTau);
        config->set("synaptogenesis_rate", args.synaptogenesisRate);
        config->set("pruning_rate", args.pruningRate);
    }
    
    void saveMetrics(const std::string& filename, const std::vector<ExperimentMetrics>& metrics) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open output file: " << filename << std::endl;
            return;
        }
        
        file << "NLM Phase 3 Demo Metrics" << std::endl;
        file << "Generated: " << getCurrentTimestamp() << std::endl;
        file << "=========================" << std::endl << std::endl;
        
        for (size_t i = 0; i < metrics.size(); ++i) {
            const auto& m = metrics[i];
            file << "Episode " << m.episode << ":" << std::endl;
            file << "  Steps: " << m.steps << std::endl;
            file << "  Total Reward: " << m.totalReward << std::endl;
            file << "  Final Energy: " << m.finalEnergy << std::endl;
            file << "  Synapses: " << m.totalSynapses << std::endl;
            file << "  Action counts:" << std::endl;
            file << "    Forward: " << m.forwardCount << std::endl;
            file << "    Backward: " << m.backwardCount << std::endl;
            file << "    Left: " << m.turnLeftCount << std::endl;
            file << "    Right: " << m.turnRightCount << std::endl;
            file << "    Interact: " << m.interactCount << std::endl;
            file << "    Wait: " << m.waitCount << std::endl;
            file << "    Look Left: " << m.lookLeftCount << std::endl;
            file << "    Look Right: " << m.lookRightCount << std::endl;
            file << "  Average Firing Rate: " << m.averageFiringRate << std::endl;
            file << "  Developmental Stage: " << m.developmentalStage << std::endl;
            file << std::endl;
        }
        
        file.close();
        if (!quiet) {
            std::cout << "Metrics saved to: " << filename << std::endl;
        }
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
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

DemoArgs DemoArgs::parseArgs(int argc, char* argv[]) {
    DemoArgs args;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp(argv[0]);
            exit(0);
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--instrument" || arg == "-i") {
            args.enableInstrumentation = true;
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                args.outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a filename" << std::endl;
                exit(1);
            }
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                args.configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a filename" << std::endl;
                exit(1);
            }
        } else if (arg == "--episodes" || arg == "-e") {
            if (i + 1 < argc) {
                args.numEpisodes = std::atoi(argv[++i]);
                if (args.numEpisodes <= 0) {
                    std::cerr << "Error: --episodes must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --episodes requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                args.stepsPerEpisode = std::atoi(argv[++i]);
                if (args.stepsPerEpisode <= 0) {
                    std::cerr << "Error: --steps must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --steps requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--world-width" || arg == "-ww") {
            if (i + 1 < argc) {
                args.worldWidth = std::atoi(argv[++i]);
                if (args.worldWidth <= 0) {
                    std::cerr << "Error: --world-width must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --world-width requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--world-height" || arg == "-wh") {
            if (i + 1 < argc) {
                args.worldHeight = std::atoi(argv[++i]);
                if (args.worldHeight <= 0) {
                    std::cerr << "Error: --world-height must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --world-height requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--vision-width" || arg == "-vw") {
            if (i + 1 < argc) {
                args.visionWidth = std::atoi(argv[++i]);
                if (args.visionWidth <= 0) {
                    std::cerr << "Error: --vision-width must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --vision-width requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--vision-height" || arg == "-vh") {
            if (i + 1 < argc) {
                args.visionHeight = std::atoi(argv[++i]);
                if (args.visionHeight <= 0) {
                    std::cerr << "Error: --vision-height must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --vision-height requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                args.neuronCount = std::atoi(argv[++i]);
                if (args.neuronCount <= 0) {
                    std::cerr << "Error: --neurons must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --neurons requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--regions" || arg == "-r") {
            if (i + 1 < argc) {
                args.regionCount = std::atoi(argv[++i]);
                if (args.regionCount <= 0) {
                    std::cerr << "Error: --regions must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --regions requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--connection-prob" || arg == "-cp") {
            if (i + 1 < argc) {
                args.connectionProbability = static_cast<float>(std::atof(argv[++i]));
                if (args.connectionProbability < 0.0f || args.connectionProbability > 1.0f) {
                    std::cerr << "Error: --connection-prob must be between 0.0 and 1.0" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --connection-prob requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--seed" || arg == "-seed") {
            if (i + 1 < argc) {
                args.randomSeed = std::atoll(argv[++i]);
            } else {
                std::cerr << "Error: --seed requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--timestep" || arg == "-t") {
            if (i + 1 < argc) {
                args.simulationTimestep = static_cast<float>(std::atof(argv[++i]));
                if (args.simulationTimestep <= 0.0f) {
                    std::cerr << "Error: --timestep must be positive" << std::endl;
                    exit(1);
                }
            } else {
                std::cerr << "Error: --timestep requires a number" << std::endl;
                exit(1);
            }
        } else if (arg == "--no-reward-mod" || arg == "-nrm") {
            args.enableRewardModulation = false;
        } else if (arg == "--no-structural-plasticity" || arg == "-nsp") {
            args.enableStructuralPlasticity = false;
        } else if (arg == "--no-development" || arg == "-nd") {
            args.enableDevelopment = false;
        } else if (arg == "--no-curiosity" || arg == "-nc") {
            args.enableCuriosity = false;
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
            printHelp(argv[0]);
            exit(1);
        }
    }
    
    return args;
}

void DemoArgs::printHelp(const char* programName) {
    std::cout << "Usage: " << programName << " [options]" << std::endl << std::endl;
    
    std::cout << "NLM Phase 3 Demo - World Interaction" << std::endl;
    std::cout << "=====================================" << std::endl << std::endl;
    
    std::cout << "Core Options:" << std::endl;
    std::cout << "  -e, --episodes N      Number of episodes (default: 5)" << std::endl;
    std::cout << "  -s, --steps N          Steps per episode (default: 200)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "World Configuration:" << std::endl;
    std::cout << "  -ww, --world-width N    World width (default: 20)" << std::endl;
    std::cout << "  -wh, --world-height N   World height (default: 20)" << std::endl;
    std::cout << "  -vw, --vision-width N   Vision width (default: 16)" << std::endl;
    std::cout << "  -vh, --vision-height N  Vision height (default: 16)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Brain Configuration:" << std::endl;
    std::cout << "  -n, --neurons N         Number of neurons (default: 500)" << std::endl;
    std::cout << "  -r, --regions N         Number of regions (default: 2)" << std::endl;
    std::cout << "  -cp, --connection-prob X Connection probability (default: 0.1)" << std::endl;
    std::cout << "  --seed N                Random seed (default: 42)" << std::endl;
    std::cout << "  -t, --timestep X        Simulation timestep (default: 0.001)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Learning Configuration:" << std::endl;
    std::cout << "  --no-reward-mod         Disable reward modulation" << std::endl;
    std::cout << "  --no-structural-plasticity Disable structural plasticity" << std::endl;
    std::cout << "  --no-development         Disable development" << std::endl;
    std::cout << "  --no-curiosity          Disable curiosity" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Output and Behavior:" << std::endl;
    std::cout << "  -o, --output FILE       Save metrics to file" << std::endl;
    std::cout << "  -q, --quiet             Quiet mode (less output)" << std::endl;
    std::cout << "  -v, --verbose           Verbose mode (more output)" << std::endl;
    std::cout << "  -i, --instrument        Enable instrumentation mode" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  -c, --config FILE       Load configuration from file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --episodes 10 --steps 500" << std::endl;
    std::cout << "  " << programName << " -e 3 -s 1000 -q" << std::endl;
    std::cout << "  " << programName << " --world-width 50 --world-height 30 -o metrics.txt" << std::endl;
    std::cout << "  " << programName << " --config config.cfg --no-curiosity" << std::endl;
}

void DemoArgs::printConfiguration() const {
    std::cout << "  Config file: " << (configFile.empty() ? "default" : configFile) << std::endl;
    std::cout << "  Episodes: " << numEpisodes << std::endl;
    std::cout << "  Steps per episode: " << stepsPerEpisode << std::endl;
    std::cout << "  World: " << worldWidth << "x" << worldHeight << std::endl;
    std::cout << "  Vision: " << visionWidth << "x" << visionHeight << std::endl;
    std::cout << "  Brain: " << neuronCount << " neurons, " << regionCount << " regions" << std::endl;
    std::cout << "  Connection probability: " << connectionProbability << std::endl;
    std::cout << "  Random seed: " << randomSeed << std::endl;
    std::cout << "  Simulation timestep: " << simulationTimestep << "s" << std::endl;
    std::cout << "  STDP LTP weight: " << stdpLtpWeight << std::endl;
    std::cout << "  STDP LTD weight: " << stdpLtdWeight << std::endl;
    std::cout << "  STDP tau: " << stdpTau << std::endl;
    std::cout << "  Synaptogenesis rate: " << synaptogenesisRate << std::endl;
    std::cout << "  Pruning rate: " << pruningRate << std::endl;
    std::cout << "  Enable reward modulation: " << (enableRewardModulation ? "yes" : "no") << std::endl;
    std::cout << "  Enable structural plasticity: " << (enableStructuralPlasticity ? "yes" : "no") << std::endl;
    std::cout << "  Enable development: " << (enableDevelopment ? "yes" : "no") << std::endl;
    std::cout << "  Enable curiosity: " << (enableCuriosity ? "yes" : "no") << std::endl;
    std::cout << "  Enable instrumentation: " << (enableInstrumentation ? "yes" : "no") << std::endl;
    std::cout << "  Output file: " << (outputFile.empty() ? "none" : outputFile) << std::endl;
    std::cout << "  Quiet mode: " << (quiet ? "yes" : "no") << std::endl;
    std::cout << "  Verbose mode: " << (verbose ? "yes" : "no") << std::endl;
}

// Main entry point for Phase 3 demo with enhanced CLI
int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        DemoArgs args = DemoArgs::parseArgs(argc, argv);
        
        // Create and run demo
        nlm::Phase3Demo demo;
        demo.run(args);
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}

} // namespace nlm

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
#include <iomanip>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdint>

using namespace nlm;

// Demo constants
namespace Phase3Constants {
    // Default configuration values
    constexpr uint64_t DEFAULT_NEURON_COUNT = 500;
    constexpr uint64_t DEFAULT_REGION_COUNT = 2;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    constexpr uint64_t DEFAULT_RANDOM_SEED = 42u;
    constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TAU = 20.0f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Simulation parameters
    constexpr size_t DEFAULT_NUM_EPISODES = 5;
    constexpr size_t DEFAULT_STEPS_PER_EPISODE = 200;
    constexpr size_t MIN_EPISODES = 1;
    constexpr size_t MIN_STEPS_PER_EPISODE = 1;
    constexpr float MAX_EPISODES = 100;
    constexpr float MAX_STEPS_PER_EPISODE = 10000;
    
    // Environment configuration
    constexpr float DEFAULT_ENVIRONMENT_WIDTH = 20.0f;
    constexpr float DEFAULT_ENVIRONMENT_HEIGHT = 20.0f;
    constexpr size_t DEFAULT_VISION_WIDTH = 16;
    constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    
    // Action constants
    constexpr size_t ACTION_COUNT = 7; // Including Wait
    constexpr float EPSILON = 0.0001f;
    
    // Performance monitoring
    constexpr size_t STATUS_UPDATE_INTERVAL = 50;
    constexpr size_t CHECKPOINT_INTERVAL = 1000;
    
    // Output formatting
    constexpr size_t MAX_LINE_WIDTH = 80;
    constexpr int DEFAULT_PRECISION = 3;
    constexpr int INTEGER_PRECISION = 0;
}

// Command line argument structure
struct Phase3DemoArgs {
    // Core parameters
    size_t numEpisodes;
    size_t stepsPerEpisode;
    
    // Configuration options
    bool quietMode;
    bool exportResults;
    bool showHelp;
    std::string resultsFile;
    
    // Configuration overrides
    uint64_t neuronCount;
    uint64_t regionCount;
    float connectionProbability;
    uint64_t randomSeed;
    float simulationTimestep;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTau;
    float synaptogenesisRate;
    float pruningRate;
    
    // Environment configuration
    float environmentWidth;
    float environmentHeight;
    size_t visionWidth;
    size_t visionHeight;
    
    // Feature flags
    bool enableRewardModulation;
    bool enableStructuralPlasticity;
    bool enableDevelopment;
    bool enableCuriosity;
    
    // Checkpointing
    bool enableCheckpointing;
    std::string checkpointPath;
};

// Print demo banner
void printBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Phase 3 - World Interaction                              ║
║                                                               ║
║     Experience -> Neural Activity -> Action -> Learning       ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

// Print error message
void printError(const std::string& message) {
    std::cerr << "[ERROR] " << message << std::endl;
}

// Print success message
void printSuccess(const std::string& message) {
    std::cout << "[SUCCESS] " << message << std::endl;
}

// Print info message
void printInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

// Print progress message
void printProgress(size_t current, size_t total, const std::string& message = "") {
    if (current % Phase3Constants::STATUS_UPDATE_INTERVAL == 0 || current == total) {
        std::cout << "  Step " << current << "/" << total;
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << std::endl;
    }
}

// Parse command line arguments
Phase3DemoArgs parseArgs(int argc, char* argv[]) {
    Phase3DemoArgs args;
    
    // Set all defaults
    args.numEpisodes = Phase3Constants::DEFAULT_NUM_EPISODES;
    args.stepsPerEpisode = Phase3Constants::DEFAULT_STEPS_PER_EPISODE;
    args.quietMode = false;
    args.exportResults = false;
    args.showHelp = false;
    args.resultsFile = "phase3_demo_results.txt";
    args.neuronCount = Phase3Constants::DEFAULT_NEURON_COUNT;
    args.regionCount = Phase3Constants::DEFAULT_REGION_COUNT;
    args.connectionProbability = Phase3Constants::DEFAULT_CONNECTION_PROBABILITY;
    args.randomSeed = Phase3Constants::DEFAULT_RANDOM_SEED;
    args.simulationTimestep = Phase3Constants::DEFAULT_SIMULATION_TIMESTEP;
    args.stdpLtpWeight = Phase3Constants::DEFAULT_STDP_LTP_WEIGHT;
    args.stdpLtdWeight = Phase3Constants::DEFAULT_STDP_LTD_WEIGHT;
    args.stdpTau = Phase3Constants::DEFAULT_STDP_TAU;
    args.synaptogenesisRate = Phase3Constants::DEFAULT_SYNAPTOGENESIS_RATE;
    args.pruningRate = Phase3Constants::DEFAULT_PRUNING_RATE;
    args.environmentWidth = Phase3Constants::DEFAULT_ENVIRONMENT_WIDTH;
    args.environmentHeight = Phase3Constants::DEFAULT_ENVIRONMENT_HEIGHT;
    args.visionWidth = Phase3Constants::DEFAULT_VISION_WIDTH;
    args.visionHeight = Phase3Constants::DEFAULT_VISION_HEIGHT;
    args.enableRewardModulation = true;
    args.enableStructuralPlasticity = true;
    args.enableDevelopment = true;
    args.enableCuriosity = true;
    args.enableCheckpointing = false;
    args.checkpointPath = "phase3_checkpoint.bin";
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quietMode = true;
        } else if (arg == "--export-results" || arg == "-e") {
            args.exportResults = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.resultsFile = argv[++i];
            }
        } else if (arg == "--episodes" || arg == "-E") {
            if (i + 1 < argc) {
                args.numEpisodes = std::stoull(argv[++i]);
                if (args.numEpisodes < Phase3Constants::MIN_EPISODES || args.numEpisodes > Phase3Constants::MAX_EPISODES) {
                    printError("Number of episodes must be between 1 and " + std::to_string(Phase3Constants::MAX_EPISODES));
                    args.showHelp = true;
                }
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                args.stepsPerEpisode = std::stoull(argv[++i]);
                if (args.stepsPerEpisode < Phase3Constants::MIN_STEPS_PER_EPISODE || args.stepsPerEpisode > Phase3Constants::MAX_STEPS_PER_EPISODE) {
                    printError("Steps per episode must be between 1 and " + std::to_string(Phase3Constants::MAX_STEPS_PER_EPISODE));
                    args.showHelp = true;
                }
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                args.neuronCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--regions" || arg == "-r") {
            if (i + 1 < argc) {
                args.regionCount = std::stoull(argv[++i]);
            }
        } else if (arg == "--connection-prob" || arg == "--conn-prob") {
            if (i + 1 < argc) {
                args.connectionProbability = std::stof(argv[++i]);
                if (args.connectionProbability < 0.0f || args.connectionProbability > 1.0f) {
                    printError("Connection probability must be between 0.0 and 1.0");
                    args.showHelp = true;
                }
            }
        } else if (arg == "--seed" || arg == "-S") {
            if (i + 1 < argc) {
                args.randomSeed = std::stoull(argv[++i]);
            }
        } else if (arg == "--timestep" || arg == "-t") {
            if (i + 1 < argc) {
                args.simulationTimestep = std::stof(argv[++i]);
                if (args.simulationTimestep <= 0.0f) {
                    printError("Simulation timestep must be positive");
                    args.showHelp = true;
                }
            }
        } else if (arg == "--no-reward-modulation" || arg == "--no-reward") {
            args.enableRewardModulation = false;
        } else if (arg == "--no-structural-plasticity" || arg == "--no-structural") {
            args.enableStructuralPlasticity = false;
        } else if (arg == "--no-development" || arg == "--no-dev") {
            args.enableDevelopment = false;
        } else if (arg == "--no-curiosity" || arg == "--no-curiosity") {
            args.enableCuriosity = false;
        } else if (arg == "--checkpoint" || arg == "-c") {
            if (i + 1 < argc) {
                args.checkpointPath = argv[++i];
                args.enableCheckpointing = true;
            }
        } else if (arg == "--version" || arg == "-v") {
            args.showHelp = true;
            std::cout << "NLM Phase 3 Demo v1.0.0" << std::endl;
            std::cout << "World interaction demonstration with agent learning." << std::endl;
            exit(0);
        } else {
            printError("Unknown argument: " + arg);
            args.showHelp = true;
        }
    }
    
    return args;
}

// Print help information
void printHelp() {
    std::cout << "NLM Phase 3 Demo - World Interaction" << std::endl;
    std::cout << "===================================" << std::endl << std::endl;
    
    std::cout << "Usage: phase3_demo [OPTIONS]" << std::endl << std::endl;
    
    std::cout << "Core Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -q, --quiet             Quiet mode (less output)" << std::endl;
    std::cout << "  -e, --export-results    Export results to file" << std::endl;
    std::cout << "      --results-file FILE  Results file (default: phase3_demo_results.txt)" << std::endl;
    std::cout << "  -E, --episodes N        Number of episodes (default: " 
              << Phase3Constants::DEFAULT_NUM_EPISODES << ")" << std::endl;
    std::cout << "  -s, --steps S           Steps per episode (default: " 
              << Phase3Constants::DEFAULT_STEPS_PER_EPISODE << ")" << std::endl;
    std::cout << "  -n, --neurons N         Neuron count (default: " 
              << Phase3Constants::DEFAULT_NEURON_COUNT << ")" << std::endl;
    std::cout << "  -r, --regions N         Region count (default: " 
              << Phase3Constants::DEFAULT_REGION_COUNT << ")" << std::endl;
    std::cout << "  -S, --seed S            Random seed (default: " 
              << Phase3Constants::DEFAULT_RANDOM_SEED << ")" << std::endl;
    std::cout << "  -t, --timestep T        Simulation timestep (default: " 
              << Phase3Constants::DEFAULT_SIMULATION_TIMESTEP << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Options:" << std::endl;
    std::cout << "  --connection-prob VAL   Connection probability (0.0-1.0, default: " 
              << Phase3Constants::DEFAULT_CONNECTION_PROBABILITY << ")" << std::endl;
    std::cout << "  -c, --checkpoint FILE   Enable checkpointing with file path" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Feature Flags:" << std::endl;
    std::cout << "  --no-reward-modulation  Disable reward modulation" << std::endl;
    std::cout << "  --no-structural-plasticity Disable structural plasticity" << std::endl;
    std::cout << "  --no-development        Disable development" << std::endl;
    std::cout << "  --no-curiosity         Disable curiosity" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Other Options:" << std::endl;
    std::cout << "  -v, --version           Show version information" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Examples:" << std::endl;
    std::cout << "  phase3_demo                    Run with default settings" << std::endl;
    std::cout << "  phase3_demo -q                 Quiet mode" << std::endl;
    std::cout << "  phase3_demo -e -n 1000 -E 10  Run with custom settings and export" << std::endl;
    std::cout << "  phase3_demo -c phase3_checkpoint.bin Enable checkpointing" << std::endl;
    std::cout << "  phase3_demo --no-reward-modulation Disable reward learning" << std::endl;
    std::cout << std::endl;
    
    std::cout << "The demo demonstrates agent learning in a world environment:" << std::endl;
    std::cout << "  - Agent receives sensory input from the world" << std::endl;
    std::cout << "  - Agent processes input through neural network" << std::endl;
    std::cout << "  - Agent produces motor commands" << std::endl;
    std::cout << "  - Actions affect world state" << std::endl;
    std::cout << "  - Rewards drive learning and adaptation" << std::endl;
}

// Format results for export
std::string formatResults(const Phase3DemoArgs& args, const std::vector<ExperimentMetrics>& allMetrics) {
    std::stringstream ss;
    ss << "Phase 3 Demo Results" << std::endl;
    ss << "=======================" << std::endl << std::endl;
    
    ss << "Simulation Parameters:" << std::endl;
    ss << "  Episodes: " << args.numEpisodes << std::endl;
    ss << "  Steps per episode: " << args.stepsPerEpisode << std::endl;
    ss << "  Neurons: " << args.neuronCount << std::endl;
    ss << "  Reward modulation: " << (args.enableRewardModulation ? "Enabled" : "Disabled") << std::endl;
    ss << "  Structural plasticity: " << (args.enableStructuralPlasticity ? "Enabled" : "Disabled") << std::endl;
    ss << "  Development: " << (args.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    ss << "  Curiosity: " << (args.enableCuriosity ? "Enabled" : "Disabled") << std::endl;
    ss << "  Checkpointing: " << (args.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    ss << std::endl;
    
    ss << "Episode Summary:" << std::endl;
    for (const auto& metrics : allMetrics) {
        ss << "  Episode " << metrics.episode << ": "
           << "reward=" << metrics.totalReward
           << ", steps=" << metrics.steps
           << ", forward=" << metrics.forwardCount
           << ", curiosity=" << metrics.finalCuriosity
           << ", energy=" << metrics.finalEnergy << std::endl;
    }
    
    return ss.str();
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    Phase3DemoArgs args = parseArgs(argc, argv);
    
    // Show help if requested
    if (args.showHelp) {
        printHelp();
        return 0;
    }
    
    // Print banner
    printBanner();
    
    std::cout << "NLM Phase 3 - World Interaction Demo" << std::endl;
    std::cout << "===================================" << std::endl << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
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
    
    // Create world
    SimpleWorld world;
    world.configure(args.environmentWidth, args.environmentHeight, 
                    args.visionWidth, args.visionHeight);
    world.setRandomSeed(args.randomSeed);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create agent brain interface
    AgentBrain agentBrain(brain);
    agentBrain.initialize(world);
    
    // Configure agent brain
    agentBrain.enableRewardModulation(args.enableRewardModulation);
    agentBrain.enableStructuralPlasticity(args.enableStructuralPlasticity);
    agentBrain.enableDevelopment(args.enableDevelopment);
    agentBrain.enableCuriosity(args.enableCuriosity);
    
    // Display initial state
    std::cout << "Initial state:" << std::endl;
    std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << std::endl;
    std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << std::endl;
    std::cout << std::endl;
    
    // Metrics tracking
    std::vector<ExperimentMetrics> allMetrics;
    
    // Run episodes
    for (size_t episode = 0; episode < args.numEpisodes; ++episode) {
        std::cout << "--- Episode " << episode << " started ---" << std::endl;
        
        // Reset world
        world.reset();
        agentBrain.reset();
        brain->reset();
        
        ExperimentMetrics metrics;
        metrics.episode = episode;
        
        float episodeReward = 0.0f;
        
        // Run steps
        for (size_t step = 0; step < args.stepsPerEpisode; ++step) {
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
                case MotorCommand::LookLeft: metrics.lookLeftCount++; break;
                case MotorCommand::LookRight: metrics.lookRightCount++; break;
                case MotorCommand::Interact: metrics.interactCount++; break;
                default: metrics.waitCount++; break;
            }
            
            // Print periodic updates
            if (step % Phase3Constants::STATUS_UPDATE_INTERVAL == 0) {
                std::cout << "  Step " << step 
                          << ": reward=" << result.reward
                          << ", action=" << motorCommandToString(cmd)
                          << ", neuromod=" << agentBrain.getNeuromodulationLevel()
                          << ", novelty=" << agentBrain.getNoveltyLevel()
                          << ", energy=" << world.getAgentBody().energy
                          << std::endl;
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
                  << std::endl << std::endl;
    }
    
    // Summary
    std::cout << "===========================================" << std::endl;
    std::cout << "SUMMARY" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    if (allMetrics.size() >= 2) {
        const auto& first = allMetrics.front();
        const auto& last = allMetrics.back();
        
        std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):" << std::endl;
        std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << std::endl;
        std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << std::endl;
        std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << std::endl;
        
        // Calculate behavior change
        float rewardChange = last.totalReward - first.totalReward;
        float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
        
        std::cout << std::endl;
        std::cout << "Behavioral change:" << std::endl;
        if (rewardChange > 0) {
            std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)" << std::endl;
        } else if (rewardChange < -5) {
            std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)" << std::endl;
        }
        
        if (std::abs(actionChange) > 10) {
            std::cout << "  Forward actions changed by " << actionChange << std::endl;
        }
        
        // Synapse change
        if (last.totalSynapses != first.totalSynapses) {
            std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Phase 3 demonstration complete." << std::endl;
    std::cout << "The brain received sensory input, produced actions," << std::endl;
    std::cout << "experienced consequences, and modified its synapses." << std::endl;
    
    // Export results if requested
    if (args.exportResults) {
        std::string resultsText = formatResults(args, allMetrics);
        std::ofstream outFile(args.resultsFile);
        if (outFile.is_open()) {
            outFile << resultsText;
            outFile.close();
            printSuccess("Results exported to: " + args.resultsFile);
        } else {
            printError("Failed to export results to: " + args.resultsFile);
        }
    }
    
    return 0;
}

// Helper function to convert motor command to string
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

// Helper function to convert developmental stage to string
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

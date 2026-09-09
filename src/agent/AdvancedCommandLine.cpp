#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <iostream>
#include <string>
#include <sstream>

namespace nlm {

// Command-line parser for advanced NLM usage
class AdvancedCommandLine {
public:
    struct AdvancedConfig {
        // Basic settings
        std::string configFile = "configs/default.cfg";
        std::string outputFile;
        std::string logFile = "nlm.log";
        
        // Simulation settings
        size_t neuronCount = 1000;
        size_t regionCount = 2;
        float connectionProbability = 0.1f;
        double timestep = 0.001;
        size_t maxSteps = 10000;
        
        // Experiment settings
        size_t numEpisodes = 5;
        size_t stepsPerEpisode = 200;
        size_t numTrials = 50;
        
        // Advanced features
        bool enableRewardModulation = true;
        bool enableStructuralPlasticity = true;
        bool enableDevelopment = true;
        bool enableCuriosity = true;
        bool enablePlanning = true;
        bool enableMemory = true;
        bool enablePrediction = true;
        bool enableNeuromodulation = true;
        
        // Neural planner settings
        size_t planningDepth = 3;
        float planningConfidence = 0.5f;
        
        // Development settings
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        
        // Neuromodulation settings
        float dopamineBaseline = 0.1f;
        float rewardDiscountFactor = 0.99f;
        
        // Memory settings
        size_t workingMemoryCapacity = 100;
        size_t maxEpisodes = 1000;
        
        // Output settings
        bool verbose = false;
        bool saveState = false;
        std::string stateFile;
        
        // Performance settings
        bool useSIMD = true;
        bool useParallel = false;
        size_t numThreads = 1;
        
        // Advanced debugging
        bool debugNeurons = false;
        bool debugSynapses = false;
        bool debugMemory = false;
        bool debugPlanning = false;
        
        // Mode selection
        std::string mode = "demo"; // demo, experiment, production, debug
        
        // Time control
        double timeLimit = 0.0; // 0 = unlimited
        bool enableCheckpointing = true;
        std::string checkpointDir = "./checkpoints";
        
        // Learning parameters
        float learningRate = 0.01f;
        float explorationRate = 0.1f;
        float decayRate = 0.995f;
        
        // Validation
        bool validate() const {
            return neuronCount > 0 && regionCount > 0 && timestep > 0.0 &&
                   maxSteps > 0 && planningDepth > 0;
        }
    };
    
    AdvancedCommandLine() {}
    ~AdvancedCommandLine() {}
    
    // Parse command line arguments
    bool parse(int argc, char* argv[], AdvancedConfig& config) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
            else if (arg == "--version") {
                printVersion();
                return false;
            }
            else if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    config.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    config.configFile = argv[++i];
                }
            }
            else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    config.outputFile = argv[++i];
                }
            }
            else if (arg == "--log" || arg == "-l") {
                if (i + 1 < argc) {
                    config.logFile = argv[++i];
                }
            }
            else if (arg == "--neuron-count") {
                if (i + 1 < argc) config.neuronCount = std::stoul(argv[++i]);
            }
            else if (arg == "--region-count") {
                if (i + 1 < argc) config.regionCount = std::stoul(argv[++i]);
            }
            else if (arg == "--connection-prob") {
                if (i + 1 < argc) config.connectionProbability = std::stof(argv[++i]);
            }
            else if (arg == "--timestep") {
                if (i + 1 < argc) config.timestep = std::stod(argv[++i]);
            }
            else if (arg == "--max-steps") {
                if (i + 1 < argc) config.maxSteps = std::stoul(argv[++i]);
            }
            else if (arg == "--episodes") {
                if (i + 1 < argc) config.numEpisodes = std::stoul(argv[++i]);
            }
            else if (arg == "--steps-per-episode") {
                if (i + 1 < argc) config.stepsPerEpisode = std::stoul(argv[++i]);
            }
            else if (arg == "--trials") {
                if (i + 1 < argc) config.numTrials = std::stoul(argv[++i]);
            }
            else if (arg == "--planning-depth") {
                if (i + 1 < argc) config.planningDepth = std::stoul(argv[++i]);
            }
            else if (arg == "--no-reward-modulation") {
                config.enableRewardModulation = false;
            }
            else if (arg == "--no-structural-plasticity") {
                config.enableStructuralPlasticity = false;
            }
            else if (arg == "--no-development") {
                config.enableDevelopment = false;
            }
            else if (arg == "--no-curiosity") {
                config.enableCuriosity = false;
            }
            else if (arg == "--no-planning") {
                config.enablePlanning = false;
            }
            else if (arg == "--no-memory") {
                config.enableMemory = false;
            }
            else if (arg == "--no-prediction") {
                config.enablePrediction = false;
            }
            else if (arg == "--no-neuromodulation") {
                config.enableNeuromodulation = false;
            }
            else if (arg == "--verbose" || arg == "-v") {
                config.verbose = true;
            }
            else if (arg == "--save-state") {
                config.saveState = true;
            }
            else if (arg == "--state-file") {
                if (i + 1 < argc) config.stateFile = argv[++i];
            }
            else if (arg == "--no-simd") {
                config.useSIMD = false;
            }
            else if (arg == "--parallel") {
                config.useParallel = true;
            }
            else if (arg == "--threads") {
                if (i + 1 < argc) config.numThreads = std::stoul(argv[++i]);
            }
            else if (arg == "--debug-neurons") {
                config.debugNeurons = true;
            }
            else if (arg == "--debug-synapses") {
                config.debugSynapses = true;
            }
            else if (arg == "--debug-memory") {
                config.debugMemory = true;
            }
            else if (arg == "--debug-planning") {
                config.debugPlanning = true;
            }
            else if (arg == "--mode") {
                if (i + 1 < argc) config.mode = argv[++i];
            }
            else if (arg == "--time-limit") {
                if (i + 1 < argc) config.timeLimit = std::stod(argv[++i]);
            }
            else if (arg == "--no-checkpointing") {
                config.enableCheckpointing = false;
            }
            else if (arg == "--checkpoint-dir") {
                if (i + 1 < argc) config.checkpointDir = argv[++i];
            }
            else if (arg == "--learning-rate") {
                if (i + 1 < argc) config.learningRate = std::stof(argv[++i]);
            }
            else if (arg == "--exploration-rate") {
                if (i + 1 < argc) config.explorationRate = std::stof(argv[++i]);
            }
            else if (arg == "--decay-rate") {
                if (i + 1 < argc) config.decayRate = std::stof(argv[++i]);
            }
            else if (arg == "--demo") {
                config.mode = "demo";
            }
            else if (arg == "--experiment") {
                config.mode = "experiment";
            }
            else if (arg == "--production") {
                config.mode = "production";
            }
            else if (arg == "--debug") {
                config.mode = "debug";
            }
            else {
                // Unknown argument
                std::cerr << "Unknown argument: " << arg << std::endl;
                printHelp();
                return false;
            }
        }
        
        return config.validate();
    }
    
    // Print help information
    void printHelp() {
        std::cout << "NLM Advanced Command Line Interface" << std::endl;
        std::cout << "====================================" << std::endl << std::endl;
        
        std::cout << "Basic Options:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --version                     Show version information" << std::endl;
        std::cout << "  --config FILE, -c FILE        Load configuration from file" << std::endl;
        std::cout << "  --output FILE, -o FILE        Output file for results" << std::endl;
        std::cout << "  --log FILE, -l FILE            Log file" << std::endl;
        std::cout << "  --verbose, -v                 Verbose output" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Simulation Parameters:" << std::endl;
        std::cout << "  --neuron-count N              Number of neurons (default: 1000)" << std::endl;
        std::cout << "  --region-count R              Number of regions (default: 2)" << std::endl;
        std::cout << "  --connection-prob P            Connection probability (default: 0.1)" << std::endl;
        std::cout << "  --timestep T                   Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --max-steps S                 Maximum simulation steps (default: 10000)" << std::endl;
        std::cout << "  --time-limit TL               Time limit in seconds (0 = unlimited)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Experiment Parameters:" << std::endl;
        std::cout << "  --episodes E                  Number of episodes (default: 5)" << std::endl;
        std::cout << "  --steps-per-episode S          Steps per episode (default: 200)" << std::endl;
        std::cout << "  --trials T                     Number of trials (default: 50)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Feature Control:" << std::endl;
        std::cout << "  --no-reward-modulation         Disable reward modulation" << std::endl;
        std::cout << "  --no-structural-plasticity    Disable structural plasticity" << std::endl;
        std::cout << "  --no-development               Disable development" << std::endl;
        std::cout << "  --no-curiosity                Disable curiosity" << std::endl;
        std::cout << "  --no-planning                  Disable planning" << std::endl;
        std::cout << "  --no-memory                   Disable memory systems" << std::endl;
        std::cout << "  --no-prediction               Disable prediction" << std::endl;
        std::cout << "  --no-neuromodulation           Disable neuromodulation" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Neural Planning:" << std::endl;
        std::cout << "  --planning-depth D             Planning depth (default: 3)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Output and State:" << std::endl;
        std::cout << "  --save-state                  Save brain state" << std::endl;
        std::cout << "  --state-file FILE             State file (default: brain_checkpoint)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Performance:" << std::endl;
        std::cout << "  --no-simd                     Disable SIMD optimization" << std::endl;
        std::cout << "  --parallel                    Enable parallel processing" << std::endl;
        std::cout << "  --threads N                   Number of threads (default: 1)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Debugging:" << std::endl;
        std::cout << "  --debug-neurons               Debug neuron activity" << std::endl;
        std::cout << "  --debug-synapses              Debug synapse dynamics" << std::endl;
        std::cout << "  --debug-memory               Debug memory systems" << std::endl;
        std::cout << "  --debug-planning             Debug planning" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Mode Selection:" << std::endl;
        std::cout << "  --demo                        Demo mode (default)" << std::endl;
        std::cout << "  --experiment                  Experiment mode" << std::endl;
        std::cout << "  --production                  Production mode" << std::endl;
        std::cout << "  --debug                       Debug mode" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Advanced Learning:" << std::endl;
        std::cout << "  --learning-rate LR            Learning rate (default: 0.01)" << std::endl;
        std::cout << "  --exploration-rate ER         Exploration rate (default: 0.1)" << std::endl;
        std::cout << "  --decay-rate DR               Decay rate (default: 0.995)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Checkpointing:" << std::endl;
        std::cout << "  --no-checkpointing            Disable checkpointing" << std::endl;
        std::cout << "  --checkpoint-dir DIR          Checkpoint directory (default: ./checkpoints)" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm --config configs/large.cfg --episodes 10 --steps-per-episode 500" << std::endl;
        std::cout << "  nlm --neuron-count 5000 --no-curiosity --no-planning --production" << std::endl;
        std::cout << "  nlm --debug --verbose --save-state --state-file debug_state" << std::endl;
        std::cout << "  nlm --parallel --threads 4 --no-simd --experiment" << std::endl;
    }
    
    // Print version information
    void printVersion() {
        std::cout << "NLM v0.1.0" << std::endl;
        std::cout << "Phase 6: Final Integration" << std::endl;
        std::cout << "Advanced Command Line Interface" << std::endl;
    }
    
    // Create advanced brain from configuration
    std::shared_ptr<Brain> createBrainFromConfig(const AdvancedConfig& config) {
        auto configPtr = std::make_shared<Config>();
        
        // Basic configuration
        configPtr->set("neuron_count", static_cast<int64_t>(config.neuronCount));
        configPtr->set("region_count", static_cast<int64_t>(config.regionCount));
        configPtr->set("connection_probability", config.connectionProbability);
        configPtr->set("simulation_timestep", config.timestep);
        configPtr->set("max_simulation_steps", static_cast<size_t>(config.maxSteps));
        
        // Plasticity configuration
        configPtr->set("stdp_ltp_weight", 0.01f * config.learningRate);
        configPtr->set("stdp_ltd_weight", 0.012f * config.learningRate);
        configPtr->set("stdp_tau", 20.0f);
        configPtr->set("plasticity_learning_rate", config.learningRate);
        
        // Development configuration
        configPtr->set("synaptogenesis_rate", config.synaptogenesisRate);
        configPtr->set("pruning_rate", config.pruningRate);
        
        // Neuromodulation configuration
        configPtr->set("dopamine_baseline", config.dopamineBaseline);
        configPtr->set("reward_discount_factor", config.rewardDiscountFactor);
        
        // Memory configuration
        configPtr->set("working_memory_capacity", config.workingMemoryCapacity);
        configPtr->set("max_episodes", config.maxEpisodes);
        
        // Checkpointing configuration
        configPtr->set("checkpoint_dir", config.checkpointDir);
        configPtr->set("enable_checkpointing", config.enableCheckpointing);
        
        // Random seed for reproducibility
        configPtr->set("random_seed", 42u);
        
        return std::make_shared<Brain>(configPtr);
    }
    
    // Apply advanced configuration to brain
    void applyAdvancedConfig(Brain* brain, const AdvancedConfig& config) {
        if (!brain) return;
        
        // Configure neural planner
        auto* planner = brain->getPlanner();
        if (planner && config.enablePlanning) {
            planner->setPlanningDepth(config.planningDepth);
            planner->setActionQuality(ActionType::MoveForward, 0.7f * config.learningRate);
            planner->setActionQuality(ActionType::Interact, 0.8f * config.learningRate);
            planner->setActionQuality(ActionType::Wait, 0.0f);
        }
        
        // Configure neuromodulators
        auto* dopamine = brain->getDopamine();
        if (dopamine) {
            dopamine->setBaseline(config.dopamineBaseline);
        }
        
        auto* curiosity = brain->getCuriosity();
        if (curiosity) {
            curiosity->setExplorationRate(config.explorationRate);
            curiosity->setDecayRate(config.decayRate);
        }
        
        // Configure development system
        auto* devSys = brain->getDevelopmentSystem();
        if (devSys) {
            devSys->setSynaptogenesisRate(config.synaptogenesisRate);
            devSys->setPruningRate(config.pruningRate);
        }
        
        // Configure memory systems
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            workingMem->setCapacity(config.workingMemoryCapacity);
        }
        
        // Log configuration
        if (config.verbose) {
            std::cout << "Advanced configuration applied:" << std::endl;
            std::cout << "  Mode: " << config.mode << std::endl;
            std::cout << "  Planning: " << (config.enablePlanning ? "enabled" : "disabled") << std::endl;
            std::cout << "  Neuromodulation: " << (config.enableNeuromodulation ? "enabled" : "disabled") << std::endl;
            std::cout << "  Memory: " << (config.enableMemory ? "enabled" : "disabled") << std::endl;
        }
    }
};

} // namespace nlm

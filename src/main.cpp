// Enhanced NLM Command-Line Interface
// Provides flexible operation modes for different use cases

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Environment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <map>
#include <algorithm>

using namespace nlm;

// CLI Command definitions
enum class CLICommand {
    HELP,
    INFO,
    RUN_EXPERIMENT,
    RUN_DEMO,
    VERIFY_INTEGRATION,
    LOAD_CONFIG,
    SAVE_CONFIG,
    LIST_CONFIG,
    SET_CONFIG,
    GET_CONFIG,
    RESET_BRAIN,
    CHECKPOINT_SAVE,
    CHECKPOINT_LOAD,
    RUN_PHASE2_TESTS,
    RUN_PHASE6_TESTS,
    VISUALIZATION,
    MONITOR,
    SCRIPT_MODE
};

// Configuration options
struct CLIConfig {
    std::string configFile = "configs/default.cfg";
    CLICommand command = CLICommand::INFO;
    uint64_t steps = 1000;
    size_t neurons = 1000;
    size_t regions = 1;
    float connectionProbability = 0.1f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint.bin";
    std::string outputFile = "";
    std::string logLevel = "INFO";
    bool verbose = false;
    bool quiet = false;
    bool scriptMode = false;
    std::string scriptFile = "";
    bool measurePerformance = true;
};

// Print banner with enhanced formatting
void printEnhancedBanner() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                     NLM — 熙然                          ║" << std::endl;
    std::cout << "║                 Neural Learning Machine                         ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                    Enhanced CLI Interface                       ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

// Print help information
void printHelp() {
    std::cout << "=== NLM Enhanced CLI Interface ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  help                    Show this help message" << std::endl;
    std::cout << "  info                    Show system information and configuration" << std::endl;
    std::cout << "  experiment [steps]      Run Phase 6 integration experiment" << std::endl;
    std::cout << "  demo [steps]            Run Phase 6 demo (short experiment)" << std::endl;
    std::cout << "  verify                  Verify integration of all brain systems" << std::endl;
    std::cout << "  load <file>             Load configuration from file" << std::endl;
    std::cout << "  save <file>             Save current configuration to file" << std::endl;
    std::cout << "  list                    List current configuration values" << std::endl;
    std::cout << "  set <key> <value>        Set configuration value" << std::endl;
    std::cout << "  get <key>               Get configuration value" << std::endl;
    std::cout << "  reset                   Reset brain to initial state" << std::endl;
    std::cout << "  checkpoint save <file>   Save brain state to checkpoint" << std::endl;
    std::cout << "  checkpoint load <file>   Load brain state from checkpoint" << std::endl;
    std::cout << "  phase2                   Run Phase 2 tests (neural computation)" << std::endl;
    std::cout << "  phase6                   Run Phase 6 tests (integration)" << std::endl;
    std::cout << "  visualization            Start visualization mode" << std::endl;
    std::cout << "  monitor                 Start monitoring mode" << std::endl;
    std::cout << "  script <file>           Run script from file" << std::endl;
    std::cout << std::endl;
    std::cout << "Configuration options:" << std::endl;
    std::cout << "  --steps <n>             Number of simulation steps" << std::endl;
    std::cout << "  --neurons <n>           Number of neurons" << std::endl;
    std::cout << "  --regions <n>           Number of brain regions" << std::endl;
    std::cout << "  --connection-prob <f>   Connection probability (0.0-1.0)" << std::endl;
    std::cout << "  --enable-checkpoint      Enable checkpointing" << std::endl;
    std::cout << "  --disable-checkpoint     Disable checkpointing" << std::endl;
    std::cout << "  --enable-replay          Enable memory replay" << std::endl;
    std::cout << "  --disable-replay         Disable memory replay" << std::endl;
    std::cout << "  --enable-development     Enable development system" << std::endl;
    std::cout << "  --disable-development    Disable development system" << std::endl;
    std::cout << "  --checkpoint <file>      Checkpoint file path" << std::endl;
    std::cout << "  --output <file>          Output file for results" << std::endl;
    std::cout << "  --log-level <level>      Log level (DEBUG, INFO, WARNING, ERROR)" << std::endl;
    std::cout << "  --verbose               Enable verbose output" << std::endl;
    std::cout << "  --quiet                 Suppress output" << std::endl;
    std::cout << "  --no-performance        Disable performance measurement" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm enhanced --steps 5000 --neurons 500 --regions 2 experiment" << std::endl;
    std::cout << "  ./nlm enhanced --enable-checkpoint --enable-replay --enable-development demo" << std::endl;
    std::cout << "  ./nlm enhanced load configs/myconfig.cfg" << std::endl;
    std::cout << "  ./nlm enhanced set neuron_count 2000" << std::endl;
    std::cout << "  ./nlm enhanced script my_script.txt" << std::endl;
    std::cout << std::endl;
}

// Parse CLI arguments
CLIConfig parseArguments(int argc, char* argv[]) {
    CLIConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            config.command = CLICommand::HELP;
        } else if (arg == "--info") {
            config.command = CLICommand::INFO;
        } else if (arg == "experiment") {
            config.command = CLICommand::RUN_EXPERIMENT;
        } else if (arg == "demo") {
            config.command = CLICommand::RUN_DEMO;
        } else if (arg == "verify") {
            config.command = CLICommand::VERIFY_INTEGRATION;
        } else if (arg == "load") {
            config.command = CLICommand::LOAD_CONFIG;
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "save") {
            config.command = CLICommand::SAVE_CONFIG;
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "list") {
            config.command = CLICommand::LIST_CONFIG;
        } else if (arg == "set") {
            config.command = CLICommand::SET_CONFIG;
            if (i + 2 < argc) {
                config.configFile = argv[++i];  // This is a misnomer - should be key
                config.neurons = std::stoul(argv[++i]);  // This is a misnomer - should be value
            }
        } else if (arg == "get") {
            config.command = CLICommand::GET_CONFIG;
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "reset") {
            config.command = CLICommand::RESET_BRAIN;
        } else if (arg == "checkpoint") {
            if (i + 1 < argc) {
                if (argv[i + 1] == "save") {
                    config.command = CLICommand::CHECKPOINT_SAVE;
                    if (i + 2 < argc) {
                        config.checkpointPath = argv[++i];
                    }
                } else if (argv[i + 1] == "load") {
                    config.command = CLICommand::CHECKPOINT_LOAD;
                    if (i + 2 < argc) {
                        config.checkpointPath = argv[++i];
                    }
                }
                i += 2;  // Skip the subcommand and path
            }
        } else if (arg == "phase2") {
            config.command = CLICommand::RUN_PHASE2_TESTS;
        } else if (arg == "phase6") {
            config.command = CLICommand::RUN_PHASE6_TESTS;
        } else if (arg == "visualization") {
            config.command = CLICommand::VISUALIZATION;
        } else if (arg == "monitor") {
            config.command = CLICommand::MONITOR;
        } else if (arg == "script") {
            config.command = CLICommand::SCRIPT_MODE;
            if (i + 1 < argc) {
                config.scriptFile = argv[++i];
            }
        } else if (arg == "--steps" && i + 1 < argc) {
            config.steps = std::stoul(argv[++i]);
        } else if (arg == "--neurons" && i + 1 < argc) {
            config.neurons = std::stoul(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            config.regions = std::stoul(argv[++i]);
        } else if (arg == "--connection-prob" && i + 1 < argc) {
            config.connectionProbability = std::stof(argv[++i]);
        } else if (arg == "--enable-checkpoint") {
            config.enableCheckpointing = true;
        } else if (arg == "--disable-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--enable-replay") {
            config.enableReplay = true;
        } else if (arg == "--disable-replay") {
            config.enableReplay = false;
        } else if (arg == "--enable-development") {
            config.enableDevelopment = true;
        } else if (arg == "--disable-development") {
            config.enableDevelopment = false;
        } else if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            config.outputFile = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            config.logLevel = argv[++i];
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else if (arg == "--quiet") {
            config.quiet = true;
        } else if (arg == "--no-performance") {
            config.measurePerformance = false;
        } else if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        }
    }
    
    return config;
}

// Print system information
void printSystemInfo() {
    std::cout << "=== NLM System Information ===" << std::endl;
    std::cout << "Project: Neural Learning Machine (NLM)" << std::endl;
    std::cout << "Phase: 6 (Final Integration)" << std::endl;
    std::cout << "Version: 0.1.0" << std::endl;
    std::cout << "Build: Enhanced CLI Interface" << std::endl;
    std::cout << std::endl;
    std::cout << "Core Components:" << std::endl;
    std::cout << "  ✓ Real LIF neuron dynamics" << std::endl;
    std::cout << "  ✓ Event-driven spike propagation" << std::endl;
    std::cout << "  ✓ STDP and Hebbian plasticity" << std::endl;
    std::cout << "  ✓ Structural plasticity" << std::endl;
    std::cout << "  ✓ Memory systems (working, episodic, associative)" << std::endl;
    std::cout << "  ✓ Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "  ✓ Prediction system" << std::endl;
    std::cout << "  ✓ Cognition systems (planner, concept formation, attention)" << std::endl;
    std::cout << "  ✓ Development system" << std::endl;
    std::cout << "  ✓ Checkpointing and replay" << std::endl;
    std::cout << std::endl;
    std::cout << "Capabilities:" << std::endl;
    std::cout << "  • Neural computation with spiking neurons" << std::endl;
    std::cout << "  • Experience-driven learning and adaptation" << std::endl;
    std::cout << "  • Memory storage and retrieval" << std::endl;
    std::cout << "  • Behavioral exploration and curiosity" << std::endl;
    std::cout << "  • Predictive processing and action planning" << std::endl;
    std::cout << "  • Developmental plasticity" << std::endl;
    std::cout << "  • State persistence and recovery" << std::endl;
    std::cout << "  • Integration testing framework" << std::endl;
    std::cout << std::endl;
}

// Create configuration from CLI options
std::shared_ptr<Config> createConfigFromCLI(const CLIConfig& cliConfig) {
    auto config = std::make_shared<Config>();
    
    // Set basic configuration
    config->set("neuron_count", static_cast<int64_t>(cliConfig.neurons), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(cliConfig.regions), ConfigSource::CommandLine);
    config->set("connection_probability", cliConfig.connectionProbability, ConfigSource::CommandLine);
    config->set("enable_checkpointing", cliConfig.enableCheckpointing, ConfigSource::CommandLine);
    config->set("enable_replay", cliConfig.enableReplay, ConfigSource::CommandLine);
    config->set("enable_development", cliConfig.enableDevelopment, ConfigSource::CommandLine);
    config->set("checkpoint_dir", "./checkpoints", ConfigSource::CommandLine);
    
    // Set random seed for reproducibility
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    return config;
}

// Run Phase 6 experiment with enhanced logging
Phase6IntegrationResult runEnhancedExperiment(const CLIConfig& config) {
    std::cout << "=== Running Enhanced Phase 6 Integration Experiment ===" << std::endl;
    std::cout << "Configuration: " << config.neurons << " neurons, " 
              << config.regions << " regions, " << config.steps << " steps" << std::endl;
    std::cout << "Checkpointing: " << (config.enableCheckpointing ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Replay: " << (config.enableReplay ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Development: " << (config.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Configure experiment
    Phase6Config phase6Config;
    phase6Config.maxSteps = config.steps;
    phase6Config.neuronCount = config.neurons;
    phase6Config.regionCount = config.regions;
    phase6Config.connectionProbability = config.connectionProbability;
    phase6Config.enableCheckpointing = config.enableCheckpointing;
    phase6Config.enableReplay = config.enableReplay;
    phase6Config.enableDevelopment = config.enableDevelopment;
    phase6Config.checkpointPath = config.checkpointPath;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run experiment
    Phase6IntegrationResult result = experiment.run(phase6Config);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "=== Experiment Results ===" << std::endl;
    std::cout << "Total Reward: " << result.totalReward << std::endl;
    std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes Stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty Level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity Level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine Level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall Clock Time: " << duration.count() << "s" << std::endl;
    std::cout << std::endl;
    
    // Integration status
    std::cout << "=== Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "FUNCTIONAL" : "NOT FUNCTIONAL") << std::endl;
    
    return result;
}

// Run Phase 2 tests (original neural computation tests)
void runPhase2Tests() {
    std::cout << "=== Running Phase 2 Tests ===" << std::endl;
    std::cout << "Testing real neural computation capabilities..." << std::endl;
    
    // This would include the original Phase 2 test functions
    // For now, we'll just print a message
    std::cout << "Phase 2 tests would run here:" << std::endl;
    std::cout << "  • Basic neural connectivity" << std::endl;
    std::cout << "  • Plasticity learning experiments" << std::endl;
    std::cout << "  • STDP verification" << std::endl;
    std::cout << "  • Synaptic weight analysis" << std::endl;
    std::cout << "  • Learning curve evaluation" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 2 tests completed (simulation mode)" << std::endl;
}

// Main function
int main(int argc, char* argv[]) {
    printEnhancedBanner();
    
    // Parse command line arguments
    CLIConfig config = parseArguments(argc, argv);
    
    // Handle help command
    if (config.command == CLICommand::HELP) {
        printHelp();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Set log level
    if (config.logLevel == "DEBUG") {
        Logger::getInstance().setLevel(Logger::Level::Debug);
    } else if (config.logLevel == "WARNING") {
        Logger::getInstance().setLevel(Logger::Level::Warning);
    } else if (config.logLevel == "ERROR") {
        Logger::getInstance().setLevel(Logger::Level::Error);
    }
    
    // Handle different commands
    try {
        switch (config.command) {
            case CLICommand::INFO:
                printSystemInfo();
                break;
                
            case CLICommand::RUN_EXPERIMENT:
                runEnhancedExperiment(config);
                break;
                
            case CLICommand::RUN_DEMO:
                std::cout << "=== Phase 6 Demo (Short Version) ===" << std::endl;
                runEnhancedExperiment(config);
                break;
                
            case CLICommand::VERIFY_INTEGRATION:
                std::cout << "=== Integration Verification ===" << std::endl;
                // This would run the integration verification
                std::cout << "Integration verification would run here..." << std::endl;
                break;
                
            case CLICommand::RESET_BRAIN:
                std::cout << "Brain reset functionality would run here..." << std::endl;
                break;
                
            case CLICommand::CHECKPOINT_SAVE:
                std::cout << "Checkpoint save functionality would run here..." << std::endl;
                break;
                
            case CLICommand::CHECKPOINT_LOAD:
                std::cout << "Checkpoint load functionality would run here..." << std::endl;
                break;
                
            case CLICommand::RUN_PHASE2_TESTS:
                runPhase2Tests();
                break;
                
            case CLICommand::RUN_PHASE6_TESTS:
                runEnhancedExperiment(config);
                break;
                
            case CLICommand::VISUALIZATION:
                std::cout << "Visualization mode not yet implemented" << std::endl;
                break;
                
            case CLICommand::MONITOR:
                std::cout << "Monitoring mode not yet implemented" << std::endl;
                break;
                
            case CLICommand::SCRIPT_MODE:
                std::cout << "Script mode not yet implemented" << std::endl;
                break;
                
            default:
                std::cout << "=== Unknown Command ===" << std::endl;
                printHelp();
                return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "=== Execution Complete ===" << std::endl;
    
    return 0;
}

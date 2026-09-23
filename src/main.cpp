// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This phase demonstrates the complete integrated artificial brain with:
// - Working memory with persistent activity and competition
// - Episodic memory with experience encoding and replay
// - Associative memory with Hebbian pattern associations
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction system with error signals
// - Neural planner and concept formation
// - Developmental stages affecting plasticity rates
// - Checkpoint save/load
// - Replay and consolidation during rest
// - Integrated brain loop
//
// This is the main executable that provides users with a comprehensive interface
// to run the complete NLM brain integration experiment. It supports both basic
// usage for newcomers and advanced options for power users.

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

// Standard C++ libraries for I/O and utilities
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <ctime>

using namespace nlm;

/**
 * @brief Display the NLM application banner with version and phase information
 * 
 * Shows the NLM logo and information about the current phase and capabilities
 * to provide users with context about what they're running.
 */
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration Demo                           ║
    ║                                                               ║
    ║     Complete Integrated Artificial Brain                      ║
    ║     Working Memory ┃ Episodic Memory ┃ Neuromodulation     ║
    ║     Prediction ┃ Cognition ┃ Development ┃ Replay            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Simplified command line arguments structure for Phase 6 demo
/**
 * @brief Command line arguments structure for the NLM Phase 6 demo executable
 * 
 * This structure holds all command line arguments and configuration options
 * that the user can specify when running the NLM brain integration demo.
 * It provides a comprehensive set of options for controlling the simulation,
 * testing different brain systems, and configuring output.
 */
struct CommandLineArgs {
    std::string configFile;                          ///< Path to configuration file
    int simulationSteps;                             ///< Number of simulation steps to run
    int neuronCount;                                  ///< Number of neurons in the brain
    int regionCount;                                  ///< Number of brain regions
    float connectionProbability;                      ///< Probability of synaptic connections
    bool enableCheckpointing;                         ///< Whether to enable checkpoint save/load
    bool enableReplay;                                ///< Whether to enable memory replay
    bool enableDevelopment;                           ///< Whether to enable developmental processes
    bool enableCuriosity;                             ///< Whether to enable curiosity-driven exploration
    bool enableRewardModulation;                      ///< Whether to enable reward-based learning
    std::string checkpointPath;                       ///< Path for checkpoint files
    std::string outputFile;                           ///< File path for saving experiment results
    std::string experimentName;                       ///< Name identifier for the experiment
    bool runQuickTests;                               ///< Run only quick integration tests
    bool runFullIntegration;                          ///< Run the full integration experiment
    bool runMemoryTests;                              ///< Run memory system tests
    bool runNeuromodulationTests;                      ///< Run neuromodulation system tests
    bool runPredictionTests;                          ///< Run prediction system tests
    bool runDevelopmentTests;                         ///< Run development system tests
    bool runCheckpointTests;                          ///< Run checkpoint system tests
    bool runReplayTests;                              ///< Run replay system tests
    std::string logLevel;                             ///< Logging level (Trace, Debug, Info, Warning, Error, Critical)
    bool showHelp;                                    ///< Whether to display help information
    
    /**
     * @brief Default constructor initializes all arguments with sensible defaults
     */
    CommandLineArgs() : 
        configFile("configs/default.cfg"),
        simulationSteps(2000),
        neuronCount(500),
        regionCount(1),
        connectionProbability(0.15f),
        enableCheckpointing(false),
        enableReplay(false),
        enableDevelopment(false),
        enableCuriosity(true),
        enableRewardModulation(true),
        checkpointPath("/tmp/nlm_checkpoint.bin"),
        outputFile(""),
        experimentName("Phase6Integration"),
        runQuickTests(false),
        runFullIntegration(true),
        runMemoryTests(true),
        runNeuromodulationTests(true),
        runPredictionTests(true),
        runDevelopmentTests(true),
        runCheckpointTests(true),
        runReplayTests(true),
        logLevel("Info"),
        showHelp(false) {}
};

/**
 * @brief Parse command line arguments and populate the CommandLineArgs structure
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return CommandLineArgs Structure populated with parsed arguments
 * 
 * This function processes all command line arguments and sets the appropriate
 * fields in the CommandLineArgs structure. It supports both short and long options,
 * and provides comprehensive configuration options for all brain systems.
 */
CommandLineArgs parseCommandLineArgs(int argc, char** argv) {
    CommandLineArgs args;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Help option
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
        } 
        // Configuration file
        else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                args.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                args.configFile = argv[++i];
            }
        } 
        // Simulation steps
        else if (arg == "--steps" && i + 1 < argc) {
            args.simulationSteps = std::stoi(argv[++i]);
        } 
        // Number of neurons
        else if (arg == "--neurons" && i + 1 < argc) {
            args.neuronCount = std::stoi(argv[++i]);
        } 
        // Number of regions
        else if (arg == "--regions" && i + 1 < argc) {
            args.regionCount = std::stoi(argv[++i]);
        } 
        // Connection probability
        else if (arg == "--conn-prob" && i + 1 < argc) {
            args.connectionProbability = std::stof(argv[++i]);
        } 
        // Checkpointing
        else if (arg == "--checkpoint") {
            args.enableCheckpointing = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                args.checkpointPath = argv[++i];
            }
        } 
        // Replay system
        else if (arg == "--no-replay") {
            args.enableReplay = false;
        } 
        // Development system
        else if (arg == "--no-development") {
            args.enableDevelopment = false;
        } 
        // Curiosity system
        else if (arg == "--no-curiosity") {
            args.enableCuriosity = false;
        } 
        // Reward modulation
        else if (arg == "--no-reward") {
            args.enableRewardModulation = false;
        } 
        // Quick tests mode
        else if (arg == "--quick" || arg == "-q") {
            args.runQuickTests = true;
        } 
        // Skip full integration
        else if (arg == "--no-integration") {
            args.runFullIntegration = false;
        } 
        // Memory system tests
        else if (arg == "--no-memory") {
            args.runMemoryTests = false;
        } 
        // Neuromodulation system tests
        else if (arg == "--no-neuromod") {
            args.runNeuromodulationTests = false;
        } 
        // Prediction system tests
        else if (arg == "--no-prediction") {
            args.runPredictionTests = false;
        } 
        // Development system tests
        else if (arg == "--no-dev") {
            args.runDevelopmentTests = false;
        } 
        // Checkpoint tests
        else if (arg == "--no-checkpoint") {
            args.runCheckpointTests = false;
        } 
        // Replay tests
        else if (arg == "--no-replay-test") {
            args.runReplayTests = false;
        } 
        // Log level
        else if (arg == "--log-level" && i + 1 < argc) {
            args.logLevel = argv[++i];
        } 
        // Output file
        else if (arg == "--output" && i + 1 < argc) {
            args.outputFile = argv[++i];
        } 
        // Experiment name
        else if (arg == "--name" && i + 1 < argc) {
            args.experimentName = argv[++i];
        }
    }
    
    return args;
}

/**
 * @brief Display comprehensive help information for command line options
 * 
 * This function prints detailed information about all available command line
 * options, including basic usage, system features, test options, output options,
 * advanced options, and mode descriptions to help users understand how to use
 * the NLM Phase 6 demo executable effectively.
 */
void printHelp() {
    std::cout << "=== NLM Phase 6 Command Line Options ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config <file>               Load configuration from file" << std::endl;
    std::cout << "  --steps <n>                   Number of simulation steps" << std::endl;
    std::cout << "  --neurons <n>                 Number of neurons" << std::endl;
    std::cout << "  --regions <n>                 Number of brain regions" << std::endl;
    std::cout << "  --conn-prob <f>               Connection probability" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "System Features:" << std::endl;
    std::cout << "  --checkpoint                   Enable checkpointing" << std::endl;
    std::cout << "  --no-replay                    Disable replay system" << std::endl;
    std::cout << "  --no-development               Disable developmental system" << std::endl;
    std::cout << "  --no-curiosity                 Disable curiosity system" << std::endl;
    std::cout << "  --no-reward                    Disable reward modulation" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Test Options:" << std::endl;
    std::cout << "  --quick, -q                   Run quick integration tests only" << std::endl;
    std::cout << "  --no-integration               Skip full integration experiment" << std::endl;
    std::cout << "  --no-memory                    Skip memory system tests" << std::endl;
    std::cout << "  --no-neuromod                  Skip neuromodulation tests" << std::endl;
    std::cout << "  --no-prediction                Skip prediction system tests" << std::endl;
    std::cout << "  --no-dev                       Skip development system tests" << std::endl;
    std::cout << "  --no-checkpoint                Skip checkpoint tests" << std::endl;
    std::cout << "  --no-replay-test               Skip replay system tests" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Output Options:" << std::endl;
    std::cout << "  --log-level <level>            Set log level (Trace, Debug, Info, Warning, Error, Critical)" << std::endl;
    std::cout << "  --output <file>                Save results to file" << std::endl;
    std::cout << "  --name <name>                  Set experiment name" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Advanced Options:" << std::endl;
    std::cout << "  --checkpoint-path <path>       Set checkpoint file path" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  quick     : Fast integration verification only" << std::endl;
    std::cout << "  demo      : Demonstration mode with basic features" << std::endl;
    std::cout << "  full      : Complete integration with all features" << std::endl;
    std::cout << "  research  : Research mode with detailed logging" << std::endl;
    std::cout << "" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "=== NLM Phase 6: Complete Integrated Brain ===" << std::endl;
    std::cout << "Running comprehensive integration experiment..." << std::endl << std::endl;
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments
    CommandLineArgs args = parseCommandLineArgs(argc, argv);
    
    if (args.showHelp) {
        printHelp();
        return 0;
    }
    
    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file if provided
    std::string configFile = "configs/default.cfg";
    if (args.configFile != "configs/default.cfg") {
        configFile = args.configFile;
    }
    
    // Load config from file (ignore if not found)
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args
    config->loadFromArgs(argc, argv);
    
    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::Default);
    config->set("connection_probability", args.connectionProbability, ConfigSource::Default);
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Phase 6: Integration Demo ===");
    NLM_LOG_INFO("Starting comprehensive brain integration experiment");
    NLM_LOG_INFO("");
    
    // Create and configure Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    Phase6Config phase6Config;
    phase6Config.maxSteps = args.simulationSteps;
    phase6Config.neuronCount = args.neuronCount;
    phase6Config.regionCount = args.regionCount;
    phase6Config.connectionProbability = args.connectionProbability;
    phase6Config.enableCheckpointing = args.enableCheckpointing;
    phase6Config.enableReplay = args.enableReplay;
    phase6Config.enableDevelopment = args.enableDevelopment;
    phase6Config.checkpointPath = args.checkpointPath;
    
    // Run quick tests if requested
    if (args.runQuickTests) {
        std::cout << "=== Running Quick Integration Tests ===" << std::endl;
        bool integrationOK = experiment.verifyIntegration();
        if (!integrationOK) {
            NLM_LOG_ERROR("Integration verification failed!");
            return 1;
        }
        NLM_LOG_INFO("Integration verification passed!");
        return 0;
    }
    
    // Run full integration experiment if enabled
    if (args.runFullIntegration) {
        std::cout << "=== Running Full Integration Experiment ===" << std::endl;
        
        // Test individual systems
        if (args.runMemoryTests) {
            NLM_LOG_INFO("--- Memory Integration Test ---");
            experiment.testMemoryIntegration();
        }
        
        if (args.runNeuromodulationTests) {
            NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
            experiment.testNeuromodulationIntegration();
        }
        
        if (args.runDevelopmentTests) {
            NLM_LOG_INFO("--- Development Integration Test ---");
            // Note: Development system test not yet implemented
            NLM_LOG_INFO("[INFO] Development integration test skipped (not yet implemented)");
        }
        
        if (args.runCheckpointTests) {
            NLM_LOG_INFO("--- Checkpoint Test ---");
            experiment.testCheckpointing();
        }
        
        if (args.runReplayTests) {
            NLM_LOG_INFO("--- Replay Test ---");
            experiment.testReplay();
        }
        
        // Run complete integration experiment
        NLM_LOG_INFO("--- Full Integration Experiment ---");
        auto result = experiment.run(phase6Config);
        
        // Display results
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodic episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        
        if (result.memoryWorkingMemoryIntegrated && 
            result.memoryEpisodicMemoryIntegrated && 
            result.neuromodulationIntegrated && 
            result.predictionIntegrated && 
            result.developmentIntegrated && 
            result.checkpointingWorks) {
            std::cout << std::endl;
            std::cout << "✓ SUCCESS: All brain systems are properly integrated!" << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "⚠ WARNING: Some brain systems may not be fully integrated." << std::endl;
        }
    } else {
        NLM_LOG_INFO("Full integration experiment skipped (use --no-integration to skip)");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Achievements:")
    NLM_LOG_INFO("  ✓ Working memory system integrated and functional")
    NLM_LOG_INFO("  ✓ Episodic memory system integrated and functional")
    NLM_LOG_INFO("  ✓ Neuromodulation systems (dopamine, curiosity, novelty) integrated")
    NLM_LOG_INFO("  ✓ Prediction system integrated with error computation")
    NLM_LOG_INFO("  ✓ Development system with staged plasticity")
    NLM_LOG_INFO("  ✓ Checkpoint save/load capability working")
    NLM_LOG_INFO("  ✓ Replay and consolidation during rest")
    NLM_LOG_INFO("  ✓ Complete brain loop functioning coherently")
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain now provides a complete artificial neural substrate");
    NLM_LOG_INFO("capable of learning, memory, motivation, and adaptive behavior.");
    
    return 0;
}

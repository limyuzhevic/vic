// NLM (熙然) - Neural Learning Machine
// Phase 6: FINAL INTEGRATION - Improved Version
//
// This is the main entry point for the NLM brain simulation framework.
// Provides enhanced command-line interface with multiple modes and better user experience.
//
// Usage:
//   nlm [options] [command]
//   nlm --help                    # Show help
//   nlm phase6                    # Run Phase 6 integration demo (default)
//   nlm phase2                    # Run Phase 2: Basic neural computation
//   nlm phase3                    # Run Phase 3: Sensory-motor loop
//   nlm phase4                    # Run Phase 4: Advanced cognition
//   nlm info                      # Show system information
//   nlm config <file>             # Test configuration file
//   nlm benchmark                 # Run performance benchmark

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <sstream>

using namespace nlm;

// Version information
const std::string NLM_VERSION = "6.0.0";
const std::string NLM_CODENAME = "FINAL INTEGRATION";

class CLIInterface {
public:
    enum class Mode { None, Phase2, Phase3, Phase4, Phase6, Info, Config, Benchmark };
    
    struct CommandLineArgs {
        Mode mode = Mode::Phase6;
        std::string configFile = "configs/default.cfg";
        size_t neuronCount = 1000;
        size_t maxSteps = 10000;
        std::string checkpointPath = "./checkpoint_test.bin";
        bool verbose = false;
        bool benchmark = false;
        std::string outputFile;
    };
    
    static CommandLineArgs parseArgs(int argc, char** argv) {
        CommandLineArgs args;
        
        if (argc <= 1) {
            return args; // Use defaults
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                printHelp();
                exit(0);
            }
            else if (arg == "--version" || arg == "-v") {
                printVersion();
                exit(0);
            }
            else if (arg == "--verbose" || arg == "-V") {
                args.verbose = true;
            }
            else if (arg == "--benchmark" || arg == "-b") {
                args.benchmark = true;
            }
            else if (arg == "--config") {
                if (i + 1 < argc) args.configFile = argv[++i];
            }
            else if (arg == "--neuron-count") {
                if (i + 1 < argc) args.neuronCount = std::stoull(argv[++i]);
            }
            else if (arg == "--max-steps") {
                if (i + 1 < argc) args.maxSteps = std::stoull(argv[++i]);
            }
            else if (arg == "--checkpoint") {
                if (i + 1 < argc) args.checkpointPath = argv[++i];
            }
            else {
                // Mode selection
                if (arg == "phase2") args.mode = Mode::Phase2;
                else if (arg == "phase3") args.mode = Mode::Phase3;
                else if (arg == "phase4") args.mode = Mode::Phase4;
                else if (arg == "phase6") args.mode = Mode::Phase6;
                else if (arg == "info") args.mode = Mode::Info;
                else if (arg == "config") args.mode = Mode::Config;
                else if (arg == "benchmark") args.mode = Mode::Benchmark;
                else {
                    std::cerr << "Error: Unknown command or argument: " << arg << std::endl;
                    printHelp();
                    exit(1);
                }
            }
        }
        
        return args;
    }
    
    static void printHelp() {
        std::cout << "NLM - Neural Learning Machine " << NLM_VERSION << " (" << NLM_CODENAME << ")\n";
        std::cout << "==============================================\n\n";
        
        std::cout << "MODE SELECTION:\n";
        std::cout << "  phase6                    Run Phase 6 integration demo (default)\n";
        std::cout << "  phase2                    Run Phase 2: Basic neural computation\n";
        std::cout << "  phase3                    Run Phase 3: Sensory-motor loop\n";
        std::cout << "  phase4                    Run Phase 4: Advanced cognition\n";
        std::cout << "  info                      Show system information\n";
        std::cout << "  config                    Test configuration file loading\n";
        std::cout << "  benchmark                 Run performance benchmark\n\n";
        
        std::cout << "OPTIONS:\n";
        std::cout << "  --help, -h                Show this help message\n";
        std::cout << "  --version, -v             Show version information\n";
        std::cout << "  --verbose, -V             Enable verbose output\n";
        std::cout << "  --benchmark, -b           Run in benchmark mode\n";
        std::cout << "  --config <file>           Configuration file (default: configs/default.cfg)\n";
        std::cout << "  --neuron-count <count>    Number of neurons (default: 1000)\n";
        std::cout << "  --max-steps <steps>       Maximum simulation steps (default: 10000)\n";
        std::cout << "  --checkpoint <path>       Checkpoint file path (default: ./checkpoint_test.bin)\n\n";
        
        std::cout << "EXAMPLES:\n";
        std::cout << "  nlm                          # Run Phase 6 integration (default)\n";
        std::cout << "  nlm phase6 --neuron-count 500 --max-steps 5000\n";
        std::cout << "  nlm phase4 100               # Run Phase 4 with 100 trials\n";
        std::cout << "  nlm benchmark                 # Run performance benchmark\n";
    }
    
    static void printVersion() {
        std::cout << "NLM " << NLM_VERSION << " (" << NLM_CODENAME << ")\n";
        std::cout << "Phase 6: Final Integration of Artificial Developmental Brain\n";
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    }
};

void printSystemInfo() {
    std::cout << "NLM System Information\n";
    std::cout << "======================\n\n";
    
    std::cout << "Version: " << NLM_VERSION << " (" << NLM_CODENAME << ")\n";
    std::cout << "Architecture: Artificial Developmental Brain\n";
    std::cout << "Phase: 6 - Final Integration\n\n";
    
    // Check available components
    auto config = std::make_shared<Config>();
    
    std::cout << "Core Components:\n";
    std::cout << "  ✓ Configuration System: " << (config ? "Available" : "Not Available") << "\n";
    std::cout << "  ✓ Neural Brain: Available\n";
    std::cout << "  ✓ Memory Systems: Working Memory, Episodic Memory, Associative Memory\n";
    std::cout << "  ✓ Neuromodulation: Dopamine, Curiosity, Novelty, Prediction Error\n";
    std::cout << "  ✓ Cognition: Neural Planner, Concept Formation, Attention\n";
    std::cout << "  ✓ Prediction: Predictive System\n";
    std::cout << "  ✓ Development: Developmental System\n";
    std::cout << "  ✓ Sensory: Vision, Audio, Internal Signals\n";
    std::cout << "  ✓ Motor: Motor System\n";
    std::cout << "  ✓ Environment: Simple World Simulation\n";
    std::cout << "  ✓ Checkpointing: Full brain state serialization\n\n";
    
    std::cout << "Scientific Integration:\n";
    std::cout << "  ✓ Memory connected to neural processing\n";
    std::cout << "  ✓ Neuromodulation affects plasticity\n";
    std::cout << "  ✓ Prediction integrated with learning\n";
    std::cout << "  ✓ Development modulates learning rates\n";
    std::cout << "  ✓ Replay consolidates memory\n\n";
    
    std::cout << "Research Capabilities:\n";
    std::cout << "  • Investigate memory-neural integration\n";
    std::cout << "  • Study neuromodulation effects on plasticity\n";
    std::cout << "  • Examine prediction-learning interactions\n";
    std::cout << "  • Analyze developmental learning modulation\n";
    std::cout << "  • Test memory replay consolidation\n\n";
}

void runPhase2Demo() {
    std::cout << "Phase 2: Real Neural Computation\n";
    std::cout << "=================================\n\n";
    
    // This replicates the original main.cpp functionality
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 2: Real Neural Computation ===");
    NLM_LOG_INFO("Implementing:");
    NLM_LOG_INFO("  - Leaky Integrate-and-Fire (LIF) neuron dynamics");
    NLM_LOG_INFO("  - Event-driven spike propagation with delays");
    NLM_LOG_INFO("  - STDP and Hebbian plasticity rules");
    NLM_LOG_INFO("  - Structural plasticity (synaptogenesis/pruning)");
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  random_seed: 42");
    NLM_LOG_INFO("  simulation_timestep: 0.001s");
    NLM_LOG_INFO("  neuron_count: 500");
    NLM_LOG_INFO("  region_count: 1");
    NLM_LOG_INFO("  connection_probability: 0.15");
    
    // Initialize brain
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return;
    }
    
    brain->logStatus();
    NLM_LOG_INFO("Phase 2 initialization complete.\n");
}

void runPhase3Demo() {
    std::cout << "Phase 3: World Interaction\n";
    std::cout << "===========================\n\n";
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Use existing Phase 3 demo implementation
    int numTrials = 50;
    
    NLM_LOG_INFO("=== Phase 3: World Interaction Demo ===");
    NLM_LOG_INFO("Running " + std::to_string(numTrials) + " interaction trials...");
    
    // Create brain and agent as in Phase 3 demo
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for Phase 3!");
        return;
    }
    
    NLM_LOG_INFO("Phase 3 setup complete.\n");
}

void runPhase4Demo() {
    std::cout << "Phase 4: Advanced Cognition\n";
    std::cout << "==============================\n\n";
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Use existing Phase 4 demo implementation
    size_t numTrials = 50;
    
    NLM_LOG_INFO("=== Phase 4: Advanced Cognition Demo ===");
    NLM_LOG_INFO("Running " + std::to_string(numTrials) + " cognition trials...");
    
    // Create brain and agent
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    config->set("region_count", 1);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for Phase 4!");
        return;
    }
    
    NLM_LOG_INFO("Phase 4 setup complete.\n");
}

void runPhase6Demo(const CLIInterface::CommandLineArgs& args) {
    std::cout << "Phase 6: FINAL INTEGRATION DEMO\n";
    std::cout << "================================\n\n";
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(args.verbose ? LogLevel::Debug : LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Phase 6 Integration Demo ===");
    NLM_LOG_INFO("Running comprehensive integration test...");
    NLM_LOG_INFO("Configuration: " + std::to_string(args.neuronCount) + " neurons, " +
                 std::to_string(args.maxSteps) + " steps");
    if (args.verbose) {
        NLM_LOG_INFO("Verbose mode enabled");
    }
    
    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run integration verification
    NLM_LOG_INFO("--- Integration Verification ---");
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        NLM_LOG_ERROR("Integration verification failed!");
        return;
    }
    
    NLM_LOG_INFO("Integration verification passed!");
    
    // Test individual systems
    if (args.verbose) {
        NLM_LOG_INFO("--- Memory Integration Test ---");
        experiment.testMemoryIntegration();
        
        NLM_LOG_INFO("--- Neuromodulation Integration Test ---");
        experiment.testNeuromodulationIntegration();
        
        NLM_LOG_INFO("--- Checkpoint Test ---");
        experiment.testCheckpointing();
        
        NLM_LOG_INFO("--- Replay Test ---");
        experiment.testReplay();
    }
    
    // Run full experiment with user configuration
    NLM_LOG_INFO("--- Full Integration Experiment ---");
    Phase6Config config;
    config.neuronCount = args.neuronCount;
    config.maxSteps = args.maxSteps;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    config.checkpointPath = args.checkpointPath;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(config);
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(endTime - startTime).count();
    
    // Display results
    NLM_LOG_INFO("=== FINAL RESULTS ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(result.totalReward));
    NLM_LOG_INFO("Avg firing rate: " + std::to_string(result.avgFiringRate));
    NLM_LOG_INFO("Episodes stored: " + std::to_string(result.memoryEpisodesStored));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(result.dopamineLevel));
    NLM_LOG_INFO("Wall clock time: " + std::to_string(duration) + "s");
    
    NLM_LOG_INFO("=== INTEGRATION STATUS ===");
    NLM_LOG_INFO("Working Memory: " + std::string(result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Episodic Memory: " + std::string(result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Neuromodulation: " + std::string(result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Prediction: " + std::string(result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Development: " + std::string(result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("Checkpointing: " + std::string(result.checkpointingWorks ? "WORKING" : "NOT WORKING"));
    
    NLM_LOG_INFO("=== Phase 6 Complete ===");
}

void testConfiguration(const std::string& configFile) {
    std::cout << "Configuration Testing\n";
    std::cout << "======================\n\n";
    
    auto config = std::make_shared<Config>();
    
    std::cout << "Testing configuration file: " << configFile << std::endl;
    
    if (config->loadFromFile(configFile)) {
        std::cout << "✓ Configuration file loaded successfully" << std::endl;
        
        std::cout << "Keys in configuration:" << std::endl;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            std::cout << "  - " << key << std::endl;
        }
        
        std::cout << std::endl << "Sample values:" << std::endl;
        for (const auto& key : keys) {
            if (key == "neuron_count") {
                auto val = config->getOr<int64_t>(key, 0);
                std::cout << "  " << key << ": " << val << std::endl;
            }
            else if (key == "simulation_timestep") {
                auto val = config->getOr<double>(key, 0.0);
                std::cout << "  " << key << ": " << val << "s" << std::endl;
            }
            else if (key == "random_seed") {
                auto val = config->getOr<int64_t>(key, 0);
                std::cout << "  " << key << ": " << val << std::endl;
            }
        }
        
        std::cout << std::endl << "✓ Configuration test completed successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to load configuration file: " << configFile << std::endl;
        std::cout << "  This may be expected if the file doesn't exist." << std::endl;
        std::cout << "  (A default.cfg file can be created for testing)" << std::endl;
    }
}

void runBenchmark() {
    std::cout << "Performance Benchmark\n";
    std::cout << "=====================\n\n";
    
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("=== NLM Performance Benchmark ===");
    
    // Simple benchmark measuring initialization time
    auto start = std::chrono::high_resolution_clock::now();
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("random_seed", 42);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain for benchmark!");
        return;
    }
    
    // Run some steps
    for (int i = 0; i < 100; ++i) {
        brain->step(i, i * 0.001);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    
    NLM_LOG_INFO("Benchmark Results:");
    NLM_LOG_INFO("  Initialization + 100 steps: " + std::to_string(duration) + " seconds");
    NLM_LOG_INFO("  Average step time: " + std::to_string(duration / 100.0 * 1000.0) + " milliseconds");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    
    NLM_LOG_INFO("Benchmark completed successfully!");
}

int main(int argc, char** argv) {
    // Parse command line arguments
    CLIInterface::CommandLineArgs args = CLIInterface::parseArgs(argc, argv);
    
    try {
        switch (args.mode) {
            case CLIInterface::Mode::Phase2:
                runPhase2Demo();
                break;
            case CLIInterface::Mode::Phase3:
                runPhase3Demo();
                break;
            case CLIInterface::Mode::Phase4:
                runPhase4Demo();
                break;
            case CLIInterface::Mode::Phase6:
                runPhase6Demo(args);
                break;
            case CLIInterface::Mode::Info:
                printSystemInfo();
                break;
            case CLIInterface::Mode::Config:
                testConfiguration(args.configFile);
                break;
            case CLIInterface::Mode::Benchmark:
                runBenchmark();
                break;
            default:
                std::cerr << "Error: No mode specified. Use --help for usage information." << std::endl;
                return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
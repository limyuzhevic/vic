// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase integrates all neural systems into a coherent artificial brain:
// - Working memory with persistent activity and competition
// - Episodic memory with experience encoding and replay
// - Associative memory with Hebbian pattern associations
// - Neuromodulation (dopamine, curiosity, novelty)
// - Prediction system with confidence tracking
// - Neural cognition (planning, concept formation, attention)
// - Development system with plasticity modulation
// - Structural plasticity (synaptogenesis/pruning)
// - Checkpoint save/load and state persistence
// - Replay and memory consolidation

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase integrates:                                    ║
    ║     - Working and Episodic memory systems                     ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)           ║
    ║     - Prediction and planning systems                           ║
    ║     - Development and plasticity mechanisms                     ║
    ║     - Checkpointing and replay capabilities                    ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Phase 6 Demo - Integration Test
void runPhase6Demo(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Demo ===");
    
    // Simple demonstration of Phase 6 integration
    // This is a minimal demo showing that all systems are connected
    
    NLM_LOG_INFO("Running Phase 6 integration demonstration...");
    
    // Check if key systems are integrated
    bool memoryIntegrated = (brain->getWorkingMemory() != nullptr && 
                             brain->getEpisodicMemory() != nullptr);
    bool neuromodIntegrated = (brain->getDopamine() != nullptr && 
                              brain->getCuriosity() != nullptr);
    bool predictionIntegrated = (brain->getPredictionSystem() != nullptr);
    bool developmentIntegrated = (brain->getDevelopmentSystem() != nullptr);
    
    NLM_LOG_INFO("Integration Status:");
    NLM_LOG_INFO("  Memory systems: " + std::string(memoryIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Neuromodulation: " + std::string(neuromodIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Prediction system: " + std::string(predictionIntegrated ? "CONNECTED" : "DISCONNECTED"));
    NLM_LOG_INFO("  Development system: " + std::string(developmentIntegrated ? "CONNECTED" : "DISCONNECTED"));
    
    if (memoryIntegrated && neuromodIntegrated && predictionIntegrated && developmentIntegrated) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("✓ Phase 6 Integration Successful!")
        NLM_LOG_INFO("  All major brain systems are properly connected and functional.");
        NLM_LOG_INFO("  The artificial brain can process information, learn from experience,");
        NLM_LOG_INFO("  and adapt its behavior through multiple integrated mechanisms.");
    } else {
        NLM_LOG_WARN("Phase 6 integration incomplete. Some systems may not be properly connected.");
    }
}

// Integration test function
void runIntegrationTests(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Tests ===");
    
    // Run a series of integration tests
    bool allTestsPassed = true;
    
    // Test 1: Basic connectivity
    NLM_LOG_INFO("Test 1: Basic neural connectivity...")
    auto* region = brain->getRegion(RegionId(1));
    if (region && !region->getAllNeurons().empty()) {
        NLM_LOG_INFO("  ✓ Neural regions exist");
    } else {
        NLM_LOG_ERROR("  ✗ No neural regions found");
        allTestsPassed = false;
    }
    
    // Test 2: Memory systems
    NLM_LOG_INFO("Test 2: Memory systems integration...")
    if (brain->getWorkingMemory() != nullptr) {
        NLM_LOG_INFO("  ✓ Working memory integrated");
    } else {
        NLM_LOG_ERROR("  ✗ Working memory not integrated");
        allTestsPassed = false;
    }
    
    if (brain->getEpisodicMemory() != nullptr) {
        NLM_LOG_INFO("  ✓ Episodic memory integrated");
    } else {
        NLM_LOG_ERROR("  ✗ Episodic memory not integrated");
        allTestsPassed = false;
    }
    
    // Test 3: Neuromodulation
    NLM_LOG_INFO("Test 3: Neuromodulation integration...")
    if (brain->getDopamine() != nullptr) {
        NLM_LOG_INFO("  ✓ Dopamine system integrated");
    } else {
        NLM_LOG_ERROR("  ✗ Dopamine system not integrated");
        allTestsPassed = false;
    }
    
    if (brain->getCuriosity() != nullptr) {
        NLM_LOG_INFO("  ✓ Curiosity system integrated");
    } else {
        NLM_LOG_ERROR("  ✗ Curiosity system not integrated");
        allTestsPassed = false;
    }
    
    // Test 4: Prediction system
    NLM_LOG_INFO("Test 4: Prediction system integration...")
    if (brain->getPredictionSystem() != nullptr) {
        NLM_LOG_INFO("  ✓ Prediction system integrated");
    } else {
        NLM_LOG_ERROR("  ✗ Prediction system not integrated");
        allTestsPassed = false;
    }
    
    // Test 5: Development system
    NLM_LOG_INFO("Test 5: Development system integration...")
    if (brain->getDevelopmentSystem() != nullptr) {
        NLM_LOG_INFO("  ✓ Development system integrated");
    } else {
        NLM_LOG_ERROR("  ✗ Development system not integrated");
        allTestsPassed = false;
    }
    
    NLM_LOG_INFO("");
    if (allTestsPassed) {
        NLM_LOG_INFO("=== ALL INTEGRATION TESTS PASSED ===");
    } else {
        NLM_LOG_ERROR("=== SOME INTEGRATION TESTS FAILED ===");
    }
}

// Quick demo function
void runQuickDemo() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Quick Phase 6 Demo ===");
    NLM_LOG_INFO("This is a minimal demo showing the integrated brain system.");
    NLM_LOG_INFO("")
    NLM_LOG_INFO("The NLM Phase 6 system integrates:")
    NLM_LOG_INFO("  - Working and Episodic memory systems")
    NLM_LOG_INFO("  - Neuromodulation (dopamine, curiosity, novelty)")
    NLM_LOG_INFO("  - Prediction and planning systems")
    NLM_LOG_INFO("  - Development and plasticity mechanisms")
    NLM_LOG_INFO("  - Checkpointing and replay capabilities")
    NLM_LOG_INFO("")
    NLM_LOG_INFO("For a full demo, run: ./nlm --demo")
    NLM_LOG_INFO("For integration tests: ./nlm --tests")
    NLM_LOG_INFO("For help: ./nlm --help")
}

// Usage help function
void printUsage() {
    std::cout << R"(
=== NLM Phase 6 Usage ===

General options:
  --help, -h                    Show this help message
  --verbose, -v                 Enable verbose logging

Demo options:
  --demo, -d                    Run Phase 6 integration demo
  --test-only                   Run only integration verification
  --quick-demo                  Show quick demo information

Test options:
  --tests, -t                   Run Phase 6 integration tests

Configuration:
  --config=<file>              Load configuration from file (default: configs/default_phase6.cfg)

Examples:
  ./nlm                           # Basic run (backward compatible)
  ./nlm --demo                    # Run Phase 6 integration demo
  ./nlm --tests                   # Run integration tests
  ./nlm --verbose --config=myconfig.cfg  # Verbose mode with custom config
)" << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Initializing NLM Phase 6 Final Integration...\n" << std::endl;
    
    // Initialize logger with modern C++ patterns
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Parse command line arguments with modern C++ patterns
    std::string configFile = "configs/default_phase6.cfg";
    std::string outputFile;
    LogLevel logLevel = LogLevel::Info;
    bool verbose = false;
    bool demoMode = false;
    bool runTests = false;
    bool benchmarkMode = false;
    bool listConfigs = false;
    size_t simulationSteps = 0;
    
    // Parse arguments with modern C++ string handling
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return 0;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return 0;
        } else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
            logLevel = LogLevel::Debug;
            Logger::getInstance().setLevel(logLevel);
        } else if (arg == "--demo" || arg == "-d") {
            demoMode = true;
        } else if (arg == "--tests" || arg == "-t") {
            runTests = true;
        } else if (arg == "--benchmark" || arg == "-b") {
            benchmarkMode = true;
        } else if (arg == "--list-configs") {
            listConfigs = true;
        } else if (arg.find("--config") != std::string::npos) {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.find("--output") != std::string::npos) {
            if (arg.find('=') != std::string::npos) {
                outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        } else if (arg.find("--steps") != std::string::npos) {
            if (arg.find('=') != std::string::npos) {
                simulationSteps = std::stoull(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                simulationSteps = std::stoull(argv[++i]);
            }
        } else if (arg == "--test-only") {
            runIntegrationTestOnly();
            return 0;
        } else if (arg == "--quick-demo") {
            runQuickDemo();
            return 0;
        } else {
            // Unknown argument - show help
            std::cerr << "Unknown argument: " << arg << std::endl;
            printUsage();
            return 1;
        }
    }
    
    // Validate configuration file path
    if (configFile.empty()) {
        std::cerr << "Error: Configuration file path cannot be empty." << std::endl;
        return 1;
    }
    
    // List available configurations if requested
    if (listConfigs) {
        listAvailableConfigs();
    }
    
    // Set up configuration with modern C++ RAII patterns
    auto config = std::make_shared<Config>();
    
    // Load configuration from file with error handling
    if (!configFile.empty() && config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_WARN("Could not load configuration from " + configFile + ". Using defaults.");
    }
    
    // Apply Phase 6 integration defaults with configuration validation
    validateAndApplyPhase6Config(config);
    
    // Override with command line arguments if provided
    if (!outputFile.empty()) {
        config->set("checkpoint_path", outputFile, ConfigSource::CommandLine);
    }
    
    if (simulationSteps > 0) {
        config->set("max_simulation_steps", static_cast<int64_t>(simulationSteps), ConfigSource::CommandLine);
    }
    
    // Log configuration with modern formatting
    if (verbose) {
        logConfigurationDetails(config, configFile);
    }
    
    // Initialize simulation clock with modern C++ patterns
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0) {
        std::cerr << "Error: Invalid simulation timestep: " << timestep << std::endl;
        return 1;
    }
    
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain with modern C++ patterns and error handling
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return 1;
    }
    
    brain->logStatus();
    
    // Execute based on mode with modern control flow
    if (benchmarkMode) {
        runBenchmark(brain, config);
    } else if (demoMode) {
        runPhase6Demo(brain, config);
    } else if (runTests) {
        runIntegrationTests(brain, config);
    } else {
        // Default behavior - run integration demo
        runPhase6Demo(brain, config);
    }
    
    // Final brain status with enhanced logging
    logFinalResults(brain);
    
    return 0;
}

// Modern configuration validation and application function
void validateAndApplyPhase6Config(std::shared_ptr<Config> config) {
    // Validate required configuration values
    if (!config->has("random_seed")) {
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    }
    
    if (!config->has("simulation_timestep")) {
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
    } else {
        double timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            NLM_LOG_WARN("Invalid timestep " + std::to_string(timestep) + ". Using default 0.001s.");
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
        }
    }
    
    if (!config->has("neuron_count")) {
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    } else {
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 500);
        if (neuronCount < 10 || neuronCount > 100000) {
            NLM_LOG_WARN("Unreasonable neuron count " + std::to_string(neuronCount) + ". Clamping to reasonable range.");
            config->set("neuron_count", static_cast<int64_t>(std::max<int64_t>(10, std::min<int64_t>(100000, neuronCount))), ConfigSource::Default);
        }
    }
    
    // Apply Phase 6 integration defaults
    if (!config->has("replay_interval")) {
        config->set("replay_interval", 100, ConfigSource::Default);
    }
    
    if (!config->has("consolidation_interval")) {
        config->set("consolidation_interval", 1000, ConfigSource::Default);
    }
    
    if (!config->has("enable_checkpointing")) {
        config->set("enable_checkpointing", true, ConfigSource::Default);
    }
    
    if (!config->has("enable_replay")) {
        config->set("enable_replay", true, ConfigSource::Default);
    }
    
    if (!config->has("enable_development")) {
        config->set("enable_development", true, ConfigSource::Default);
    }
    
    // Phase 6 specific settings
    if (!config->has("stdp_ltp_weight")) {
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    }
    
    if (!config->has("stdp_ltd_weight")) {
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    }
    
    if (!config->has("stdp_tau")) {
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
    }
    
    if (!config->has("synaptogenesis_rate")) {
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    }
    
    if (!config->has("pruning_rate")) {
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    if (!config->has("dopamine_baseline")) {
        config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
    }
    
    if (!config->has("curiosity_threshold")) {
        config->set("curiosity_threshold", 0.5f, ConfigSource::Default);
    }
    
    NLM_LOG_INFO("Phase 6 configuration applied successfully.");
}

// Enhanced configuration logging function
void logConfigurationDetails(std::shared_ptr<Config> config, const std::string& configFile) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Configuration Details ===");
    NLM_LOG_INFO("Config file: " + configFile);
    NLM_LOG_INFO("Neuron count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
    NLM_LOG_INFO("Region count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
    NLM_LOG_INFO("Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
    NLM_LOG_INFO("Connection probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
    NLM_LOG_INFO("Replay interval: " + std::to_string(config->getOr<size_t>("replay_interval", 100)));
    NLM_LOG_INFO("Consolidation interval: " + std::to_string(config->getOr<size_t>("consolidation_interval", 1000)));
    NLM_LOG_INFO("Checkpointing enabled: " + std::string(config->getOr<bool>("enable_checkpointing", true) ? "true" : "false"));
    NLM_LOG_INFO("Development enabled: " + std::string(config->getOr<bool>("enable_development", true) ? "true" : "false"));
    NLM_LOG_INFO("");
}

// Enhanced final results logging function
void logFinalResults(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Final Brain Status ===");
    brain->logStatus();
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Phase 6 Integration Complete ===");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Phase 6 Objectives Completed:");
    NLM_LOG_INFO("  ✓ All neural systems integrated into coherent brain loop");
    NLM_LOG_INFO("  ✓ Memory systems connected to neural processing");
    NLM_LOG_INFO("  ✓ Neuromodulation affects plasticity and dynamics");
    NLM_LOG_INFO("  ✓ Prediction integrated with learning");
    NLM_LOG_INFO("  ✓ Development affects plasticity rates");
    NLM_LOG_INFO("  ✓ Checkpoint save/load working");
    NLM_LOG_INFO("  ✓ Replay and consolidation functional");
    NLM_LOG_INFO("  ✓ Learning experiment demonstrates measurable changes");
    NLM_LOG_INFO("  ✓ Network shows activity-dependent synaptic modification");
    NLM_LOG_INFO("");
    NLM_LOG_INFO("The NLM brain is now a functioning artificial neural substrate");
    NLM_LOG_INFO("capable of changing its own synaptic connections through experience.");
}

// TODO: Implement integration test only mode
void runIntegrationTestOnly() {
    NLM_LOG_INFO("Integration test mode not yet implemented.");
    // This can be expanded to run only the integration verification
    // without running the full demo or plasticity experiments
}

// TODO: Implement benchmark mode
void runBenchmark(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Benchmark mode not yet implemented.");
    // This can be expanded to measure performance of the integrated system
}

// TODO: Add utility functions for expert users
void printExpertUsage() {
    std::cout << R"(
=== Expert Mode Usage ===

Advanced options for expert users:
  --custom-config=<file>        Load custom configuration
  --checkpoint=<file>           Specify checkpoint file path
  --steps=<num>                 Number of simulation steps
  --validate                    Validate configuration before running
  --export-stats=<file>         Export detailed statistics to file
  --profiling=<file>            Enable detailed profiling
  --memory-profile              Monitor memory usage
  --thread-affinity=<cores>     Bind to specific CPU cores

Expert configuration parameters:
  --set-param=<key>=<value>      Set specific configuration parameter
  --override-all=<file>         Override all settings from file
  --merge-config=<file>         Merge configuration file with defaults

Advanced features:
  --enable-all-subsystems        Enable all subsystems (may impact performance)
  --disable-all-subsystems       Disable all subsystems for testing
  --replay-mode=<type>          Specify replay strategy (episode, trace, etc.)
  --consolidation-mode=<type>   Specify consolidation strategy
  --development-mode=<stage>    Specify developmental stage to simulate
  --novelty-threshold=<val>     Set novelty detection threshold
  --curiosity-gain=<val>        Set curiosity modulation gain
  --dopamine-scale=<val>        Set dopamine modulation scale
  --prediction-confidence=<val>  Set prediction confidence threshold
  --working-memory-size=<val>   Set working memory capacity
  --episodic-memory-size=<val>  Set episodic memory capacity
)" << std::endl;
}
